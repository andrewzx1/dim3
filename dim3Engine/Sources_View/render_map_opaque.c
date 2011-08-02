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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

extern map_type			map;
extern server_type		server;
extern setup_type		setup;
extern camera_type		camera;
extern view_type		view;

extern bitmap_type		lmap_black_bitmap;

/* =======================================================

      Opaque Map Rendering Types
      
======================================================= */

void render_opaque_mesh_normal(void)
{
	int						n,k,frame;
	GLuint					gl_id,lmap_gl_id;
	texture_type			*texture;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	
		// only this route uses color arrays
		
	glEnableClientState(GL_COLOR_ARRAY);
	
	gl_texture_opaque_light_map_start();

		// run through draw list

	for (n=0;n!=view.render->draw_list.count;n++) {

		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

			// skip meshes with no opaques

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];
		if (!mesh->draw.has_opaque) continue;
		
			// the mesh vbo
			
		view_bind_mesh_liquid_vertex_object(&mesh->vbo);
		view_bind_mesh_liquid_index_object(&mesh->vbo);
		
		glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);

		glClientActiveTexture(GL_TEXTURE1);
		glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)((mesh->vbo.count*3)*sizeof(float)));

		glClientActiveTexture(GL_TEXTURE0);
		glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)((mesh->vbo.count*(3+2))*sizeof(float)));

		glColorPointer(4,GL_FLOAT,0,(GLvoid*)((mesh->vbo.count*(3+2+2))*sizeof(float)));

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

			if ((!mesh->flag.never_cull) && (!poly->never_cull) && (!map.optimize.never_cull)) {
				if (((poly->tangent_space.normal.x*(float)(poly->box.mid.x-view.render->camera.pnt.x))+(poly->tangent_space.normal.y*(float)(poly->box.mid.y-view.render->camera.pnt.y))+(poly->tangent_space.normal.z*(float)(poly->box.mid.z-view.render->camera.pnt.z)))>map.optimize.cull_angle) {
					poly++;
					continue;
				}
			}

				// get textures
				
			texture=&map.textures[poly->txt_idx];
			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

			if (poly->lmap_txt_idx==-1) {
				lmap_gl_id=lmap_black_bitmap.gl_id;
			}
			else {
				lmap_gl_id=map.textures[poly->lmap_txt_idx].frames[0].bitmap.gl_id;
			}

			if (!gl_back_render_get_texture(poly->camera,&gl_id,NULL)) gl_id=texture->frames[frame].bitmap.gl_id;

			gl_texture_opaque_light_map_set(gl_id,lmap_gl_id);

				// draw polygon

			#ifndef D3_OPENGL_ES
				glDrawRangeElements(GL_TRIANGLE_FAN,poly->vbo.index_min,poly->vbo.index_max,poly->ptsz,GL_UNSIGNED_SHORT,(GLvoid*)poly->vbo.index_offset);
			#else
				glDrawElements(GL_TRIANGLE_FAN,poly->ptsz,GL_UNSIGNED_SHORT,(GLvoid*)poly->vbo.index_offset);
			#endif
			
			poly++;
			view.count.mesh_poly++;
		}
		
		view_unbind_mesh_liquid_vertex_object();
		view_unbind_mesh_liquid_index_object();
	}

		// only this route uses color arrays
		
	glDisableClientState(GL_COLOR_ARRAY);

	gl_texture_opaque_light_map_end();
}

void render_opaque_mesh_shader(void)
{
	int						n,k,mesh_idx,frame,tangent_offset,normal_offset;
	float					alpha;
	GLuint					gl_id;
	texture_type			*texture;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	view_light_list_type	light_list;
	
	gl_shader_draw_start();
	
		// run through draw list

	for (n=0;n!=view.render->draw_list.count;n++) {

		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

			// skip meshes with no opaques

		mesh_idx=view.render->draw_list.items[n].idx;

		mesh=&map.mesh.meshes[mesh_idx];
		if (!mesh->draw.has_opaque) continue;
		
			// the mesh vbo
			
		view_bind_mesh_liquid_vertex_object(&mesh->vbo);
		view_bind_mesh_liquid_index_object(&mesh->vbo);
		
		glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);

		glClientActiveTexture(GL_TEXTURE1);
		glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)((mesh->vbo.count*(3+2))*sizeof(float)));
	
		glClientActiveTexture(GL_TEXTURE0);
		glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)((mesh->vbo.count*3)*sizeof(float)));
		
		gl_shader_draw_reset_normal_tangent_attrib();
		
		tangent_offset=(mesh->vbo.count*(3+2+2))*sizeof(float);
		normal_offset=(mesh->vbo.count*(3+3+2+2))*sizeof(float);

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

			if ((!mesh->flag.never_cull) && (!poly->never_cull) && (!map.optimize.never_cull)) {
				if (((poly->tangent_space.normal.x*(float)(poly->box.mid.x-view.render->camera.pnt.x))+(poly->tangent_space.normal.y*(float)(poly->box.mid.y-view.render->camera.pnt.y))+(poly->tangent_space.normal.z*(float)(poly->box.mid.z-view.render->camera.pnt.z)))>map.optimize.cull_angle) {
					poly++;
					continue;
				}
			}

				// setup shader

			texture=&map.textures[poly->txt_idx];
			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

			gl_lights_build_poly_light_list(mesh_idx,poly,&light_list);
			gl_shader_draw_execute(core_shader_group_map,texture,poly->txt_idx,frame,poly->lmap_txt_idx,1.0f,&light_list,tangent_offset,normal_offset);

				// fix texture if any back rendering

			if (gl_back_render_get_texture(poly->camera,&gl_id,&alpha)) {
				gl_shader_texture_override(gl_id,alpha);
			}

				// draw polygon

			#ifndef D3_OPENGL_ES
				glDrawRangeElements(GL_TRIANGLE_FAN,poly->vbo.index_min,poly->vbo.index_max,poly->ptsz,GL_UNSIGNED_SHORT,(GLvoid*)poly->vbo.index_offset);
			#else
				glDrawElements(GL_TRIANGLE_FAN,poly->ptsz,GL_UNSIGNED_SHORT,(GLvoid*)poly->vbo.index_offset);
			#endif
			
			poly++;
			view.count.mesh_poly++;
		}
		
		view_unbind_mesh_liquid_vertex_object();
		view_unbind_mesh_liquid_index_object();
	}
		
	gl_shader_draw_end();
}

void render_opaque_mesh_glow(void)
{
	int						n,k,frame;
	bool					first_hit;
	texture_type			*texture;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	
	first_hit=TRUE;
	
		// run through draw list

	for (n=0;n!=view.render->draw_list.count;n++) {

		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

			// skip meshes with no opaques
			// or no glows

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];
		if (!mesh->draw.has_opaque) continue;
		if (!mesh->draw.has_glow) continue;
		
			// turn on glow setup
			
		if (first_hit) {
			gl_texture_glow_start();
			glDepthMask(GL_FALSE);
			first_hit=FALSE;
		}
		
			// the mesh vbo
			
		view_bind_mesh_liquid_vertex_object(&mesh->vbo);
		view_bind_mesh_liquid_index_object(&mesh->vbo);
		
		glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);

		glClientActiveTexture(GL_TEXTURE1);
		glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)((map.mesh.vbo_vertex_count*3)*sizeof(float)));

		glClientActiveTexture(GL_TEXTURE0);
		glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)((map.mesh.vbo_vertex_count*3)*sizeof(float)));

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

			if ((!mesh->flag.never_cull) && (!poly->never_cull) && (!map.optimize.never_cull)) {
				if (((poly->tangent_space.normal.x*(float)(poly->box.mid.x-view.render->camera.pnt.x))+(poly->tangent_space.normal.y*(float)(poly->box.mid.y-view.render->camera.pnt.y))+(poly->tangent_space.normal.z*(float)(poly->box.mid.z-view.render->camera.pnt.z)))>map.optimize.cull_angle) {
					poly++;
					continue;
				}
			}

				// get texture

			texture=&map.textures[poly->txt_idx];
			frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

				// draw glow

			gl_texture_glow_set(texture->frames[frame].bitmap.gl_id,texture->frames[frame].glowmap.gl_id,texture->glow.current_color);
			
			#ifndef D3_OPENGL_ES
				glDrawRangeElements(GL_TRIANGLE_FAN,poly->vbo.index_min,poly->vbo.index_max,poly->ptsz,GL_UNSIGNED_SHORT,(GLvoid*)poly->vbo.index_offset);
			#else
				glDrawElements(GL_TRIANGLE_FAN,poly->ptsz,GL_UNSIGNED_SHORT,(GLvoid*)poly->vbo.index_offset);
			#endif
			
			poly++;
		}
		
		view_unbind_mesh_liquid_vertex_object();
		view_unbind_mesh_liquid_index_object();
	}
	
		// turn off glow if we used it
		
	if (!first_hit) {
		glDepthMask(GL_TRUE);
		gl_texture_glow_end();
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
	
		// common setup
		
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL);

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// draw the polygons

	if (view_shader_on()) {
		render_opaque_mesh_shader();
	}
	else {
		render_opaque_mesh_normal();
	}

	render_opaque_mesh_glow();
	
		// common finish
		
	glDisableClientState(GL_VERTEX_ARRAY);

	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

