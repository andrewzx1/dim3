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

#define render_map_opaque_mode_none					0
#define render_map_opaque_mode_simple				1
#define render_map_opaque_mode_light_map			2
#define render_map_opaque_mode_shader				3
#define render_map_opaque_mode_glow					4

extern map_type			map;
extern server_type		server;
extern setup_type		setup;
extern camera_type		camera;
extern view_type		view;

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

      Enable and Disable Modes
      
======================================================= */

void render_opaque_mode_switch(int mode)
{
		// change mode
		
	if (opaque_cur_mode!=mode) {
		
			// simple and light map need color pointer
			
		if ((opaque_cur_mode==render_map_opaque_mode_simple) || (opaque_cur_mode==render_map_opaque_mode_light_map)) {
			if ((mode==render_map_opaque_mode_none) || (mode==render_map_opaque_mode_shader) || (mode==render_map_opaque_mode_glow)) {
				view_compile_gl_list_disable_color();
			}
		}
		else {
			if ((mode==render_map_opaque_mode_simple) || (mode==render_map_opaque_mode_light_map)) {
				view_compile_gl_list_enable_color();
			}
		}
	
			// turn off old mode
		
		switch (opaque_cur_mode) {
		
			case render_map_opaque_mode_simple:
				gl_texture_opaque_end();
				break;
				
			case render_map_opaque_mode_light_map:
				gl_texture_opaque_light_map_end();
				break;
				
			case render_map_opaque_mode_shader:
				gl_shader_draw_end();
				break;
				
			case render_map_opaque_mode_glow:
				gl_texture_glow_end();
				glDepthMask(GL_TRUE);
				break;
				
		}
			
			// turn on new mode
			
		switch (mode) {
		
			case render_map_opaque_mode_simple:
				gl_texture_opaque_start();
				view_compile_gl_list_attach_uv_simple();
				break;
				
			case render_map_opaque_mode_light_map:
				gl_texture_opaque_light_map_start();
				view_compile_gl_list_attach_uv_light_map();
				break;
				
			case render_map_opaque_mode_shader:
				gl_shader_draw_start();
				view_compile_gl_list_attach_uv_shader();
				break;
				
			case render_map_opaque_mode_glow:
				gl_texture_glow_start();
				view_compile_gl_list_attach_uv_glow();
				glDepthMask(GL_FALSE);
				break;
				
		}
		
		opaque_cur_mode=mode;
	}
}

/* =======================================================

      Opaque Map Shaders
      
======================================================= */

void render_opaque_mesh_simple(map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int							frame;
	GLuint						gl_id;
	texture_type				*texture;
	
		// mode switch

	render_opaque_mode_switch(render_map_opaque_mode_simple);

		// get texture
		
	texture=&map.textures[poly->txt_idx];
	frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

	if (!gl_back_render_get_texture(poly->camera,&gl_id,NULL)) {
		if (texture->frames[frame].combinemap.gl_id==-1) {
			gl_id=texture->frames[frame].bitmap.gl_id;
		}
		else {
			gl_id=texture->frames[frame].combinemap.gl_id;
		}
	}

	gl_texture_opaque_set(gl_id);

		// draw polygon
		
	glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
}

void render_opaque_mesh_light_map(map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int							frame;
	GLuint						gl_id,lmap_gl_id;
	texture_type				*texture;
	
		// mode switch

	render_opaque_mode_switch(render_map_opaque_mode_light_map);

		// get texture
		
	texture=&map.textures[poly->txt_idx];
	frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

	lmap_gl_id=map.textures[poly->lmap_txt_idx].frames[0].bitmap.gl_id;

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
}

void render_opaque_mesh_shader(int mesh_idx,map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int						frame;
	float					alpha;
	GLuint					gl_id;
	texture_type			*texture;
	view_light_list_type	light_list;

		// mode switch

	render_opaque_mode_switch(render_map_opaque_mode_shader);

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
}

void render_opaque_mesh_glow(map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int					frame;
	texture_type		*texture;
	
		// mode switch

	render_opaque_mode_switch(render_map_opaque_mode_glow);

		// get texture

	texture=&map.textures[poly->txt_idx];
	frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

		// draw glow

	gl_texture_glow_set(texture->frames[frame].bitmap.gl_id,texture->frames[frame].glowmap.gl_id,texture->glow.current_color);
	glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
}

/* =======================================================

      Opaque Map Rendering
      
======================================================= */

void render_map_mesh_opaque(void)
{
	int					n,k;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// setup view

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();

		// attach map complied open gl list

	view_compile_gl_list_attach();
	
		// common setup
		
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL);

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

		// remember current modes to
		// reduce number of state switches
		
	opaque_cur_mode=render_map_opaque_mode_none;

		// run through the polys

	for (n=0;n!=view.render->draw_list.count;n++) {

		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

			// skip meshes with no opaques

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];
		if (!mesh->draw.has_opaque) continue;

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// skip transparent polys

			if (poly->draw.transparent_on) {
				poly++;
				continue;
			}

				// render the poly

			if (view.debug.on) {
				render_opaque_mesh_simple(mesh,poly);
			}
			else {
				if ((poly->draw.shader_on) && (!poly->draw.dist_shader_override)) {
					render_opaque_mesh_shader(view.render->draw_list.items[n].idx,mesh,poly);
				}
				else {
					if (poly->lmap_txt_idx!=-1) {
						render_opaque_mesh_light_map(mesh,poly);
					}
					else {
						render_opaque_mesh_simple(mesh,poly);
					}
				}
			}

			if (poly->draw.glow_on) render_opaque_mesh_glow(mesh,poly);

			poly++;
		}
	}

		// turn off any mode left on
		// from rendering
		
	render_opaque_mode_switch(render_map_opaque_mode_none);

		// dettach any attached lists

	view_compile_gl_list_dettach();
}

