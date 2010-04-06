/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Opaque Segment Rendering

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "lights.h"
#include "effects.h"
#include "consoles.h"
#include "video.h"

extern map_type			map;
extern server_type		server;
extern setup_type		setup;
extern camera_type		camera;
extern view_type		view;

extern bitmap_type		lmap_hilite_bitmap;

int						opaque_cur_mode;

extern bool fog_solid_on(void);
extern void view_compile_gl_list_attach(void);
extern void view_compile_gl_list_attach_uv_simple(void);
extern void view_compile_gl_list_attach_uv_light_map(void);
extern void view_compile_gl_list_attach_uv_shader(void);
extern void view_compile_gl_list_attach_uv_glow(void);
extern void view_compile_gl_list_enable_color(void);
extern void view_compile_gl_list_disable_color(void);
extern void view_compile_gl_list_dettach(void);

/* =======================================================

      Opaque Map Rendering Types
      
======================================================= */

void render_opaque_mesh_normal(void)
{
	int						n,k,frame;
	bool					first_draw;
	GLuint					gl_id,lmap_gl_id;
	texture_type			*texture;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	first_draw=TRUE;

		// run through draw list

	for (n=0;n!=view.render->draw_list.count;n++) {

		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

			// skip meshes with no opaques

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];
		if (!mesh->draw.has_opaque) continue;

			// draw the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// skip transparent polys

			if (poly->draw.transparent_on) {
				poly++;
				continue;
			}
		
				// skip polys with away facing normals
				// do dot product between normal and vector
				// from poly mid-eye point

			if (!poly->never_cull) {
				if (((poly->tangent_space.normal.x*(float)(poly->box.mid.x-view.render->camera.pnt.x))+(poly->tangent_space.normal.y*(float)(poly->box.mid.y-view.render->camera.pnt.y))+(poly->tangent_space.normal.z*(float)(poly->box.mid.z-view.render->camera.pnt.z)))>0.0f) {
					poly++;
					continue;
				}
			}

				// setup

			if (first_draw) {
				first_draw=FALSE;
				gl_texture_opaque_light_map_start();
				view_compile_gl_list_enable_color();
				view_compile_gl_list_attach_uv_light_map();
			}

				// get textures
				
			texture=&map.textures[poly->txt_idx];
			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

			if ((poly->lmap_txt_idx==-1) || (view.debug.on)) {
				lmap_gl_id=lmap_hilite_bitmap.gl_id;
			}
			else {
				lmap_gl_id=map.textures[poly->lmap_txt_idx].frames[0].bitmap.gl_id;
			}

			if (!gl_back_render_get_texture(poly->camera,&gl_id,NULL)) {
				if (texture->frames[frame].combinemap.gl_id==-1) {
					gl_id=texture->frames[frame].bitmap.gl_id;
				}
				else {
					gl_id=texture->frames[frame].combinemap.gl_id;
				}
			}

			gl_texture_opaque_light_map_set(gl_id,lmap_gl_id);

				// draw polygon

			glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
		
			poly++;
			view.count.poly++;
		}
	}

	if (!first_draw) {
		view_compile_gl_list_disable_color();
		gl_texture_opaque_light_map_end();
	}
}

void render_opaque_mesh_shader(void)
{
	int						n,k,mesh_idx,frame;
	float					alpha;
	bool					first_draw;
	GLuint					gl_id;
	texture_type			*texture;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	view_light_list_type	light_list;

	first_draw=TRUE;

		// run through draw list

	for (n=0;n!=view.render->draw_list.count;n++) {

		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

			// skip meshes with no opaques

		mesh_idx=view.render->draw_list.items[n].idx;

		mesh=&map.mesh.meshes[mesh_idx];
		if (!mesh->draw.has_opaque) continue;

			// draw the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// skip transparent polys

			if (poly->draw.transparent_on) {
				poly++;
				continue;
			}
		
				// skip polys with away facing normals
				// do dot product between normal and vector
				// from poly mid-eye point

			if (!poly->never_cull) {
				if (((poly->tangent_space.normal.x*(float)(poly->box.mid.x-view.render->camera.pnt.x))+(poly->tangent_space.normal.y*(float)(poly->box.mid.y-view.render->camera.pnt.y))+(poly->tangent_space.normal.z*(float)(poly->box.mid.z-view.render->camera.pnt.z)))>0.0f) {
					poly++;
					continue;
				}
			}

				// setup

			if (first_draw) {
				first_draw=FALSE;
				gl_shader_draw_start();
				view_compile_gl_list_attach_uv_shader();
			}

				// setup shader

			texture=&map.textures[poly->txt_idx];
			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

			gl_lights_build_from_poly(mesh_idx,poly,&light_list);
			gl_shader_draw_execute(TRUE,texture,poly->txt_idx,frame,poly->lmap_txt_idx,1.0f,&light_list,NULL,NULL,&poly->tangent_space,NULL);

				// fix texture if any back rendering

			if (gl_back_render_get_texture(poly->camera,&gl_id,&alpha)) {
				gl_shader_texture_override(gl_id,alpha);
			}

				// draw polygon

			glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
		
			poly++;
			view.count.poly++;
		}
	}

	if (!first_draw) {
		gl_shader_draw_end();
	}
}

void render_opaque_mesh_glow(void)
{
	int						n,k,frame;
	bool					first_draw;
	texture_type			*texture;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	first_draw=TRUE;

		// run through draw list

	for (n=0;n!=view.render->draw_list.count;n++) {

		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

			// skip meshes with no opaques
			// or no glows

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];
		if (!mesh->draw.has_opaque) continue;
		if (!mesh->draw.has_glow) continue;

			// draw the polys

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// skip transparent polys or
				// polys without glows

			if ((poly->draw.transparent_on) || (!poly->draw.glow_on)) {
				poly++;
				continue;
			}
		
				// skip polys with away facing normals
				// do dot product between normal and vector
				// from poly mid-eye point

			if (!poly->never_cull) {
				if (((poly->tangent_space.normal.x*(float)(poly->box.mid.x-view.render->camera.pnt.x))+(poly->tangent_space.normal.y*(float)(poly->box.mid.y-view.render->camera.pnt.y))+(poly->tangent_space.normal.z*(float)(poly->box.mid.z-view.render->camera.pnt.z)))>0.0f) {
					poly++;
					continue;
				}
			}

				// setup

			if (first_draw) {
				first_draw=FALSE;
				gl_texture_glow_start();
				view_compile_gl_list_attach_uv_glow();
				glDepthMask(GL_FALSE);
			}

				// get texture

			texture=&map.textures[poly->txt_idx];
			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

				// draw glow

			gl_texture_glow_set(texture->frames[frame].bitmap.gl_id,texture->frames[frame].glowmap.gl_id,texture->glow.current_color);
			glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
		
			poly++;
		}
	}

	if (!first_draw) {
		gl_texture_glow_end();
		glDepthMask(GL_TRUE);
	}
}

/* =======================================================

      Opaque Map Rendering
      
======================================================= */

void render_map_mesh_opaque(void)
{
		// setup view

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();

		// attach compiled vertex lists

	view_compile_gl_list_attach();
	
		// common setup
		
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL);

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

		// draw the polygons

	if (view.shader_on) {
		render_opaque_mesh_shader();
	}
	else {
		render_opaque_mesh_normal();
	}

	render_opaque_mesh_glow();

		// dettach lists

	view_compile_gl_list_dettach();
}

