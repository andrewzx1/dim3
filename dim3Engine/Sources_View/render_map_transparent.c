/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Transparent Segment Rendering

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

#define render_map_transparent_mode_none			0
#define render_map_transparent_mode_simple			1
#define render_map_transparent_mode_light_map		2
#define render_map_transparent_mode_shader			3
#define render_map_transparent_mode_glow			4

extern bool				dim3_debug;

extern map_type			map;
extern server_type		server;
extern view_type		view;
extern camera_type		camera;
extern setup_type		setup;

int						trans_cur_mode;
bool					trans_cur_additive;
map_poly_sort_type		trans_sort;

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

      Transparent Sorting List
      
======================================================= */

bool render_transparent_create_sort_list(void)
{
	int					sz;

	sz=max_sort_poly*sizeof(map_poly_sort_item_type);
	trans_sort.list=(map_poly_sort_item_type*)malloc(sz);
	if (trans_sort.list==NULL) return(FALSE);

	bzero(trans_sort.list,sz);
	
	return(TRUE);
}

void render_transparent_dispose_sort_list(void)
{
	free(trans_sort.list);
}

/* =======================================================

      Transparent Mesh Sorting
      
======================================================= */

float render_transparent_poly_far_z(map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int				n;
	float			d,dist;
	d3pnt			*pt;

		// calculate the farest z
		// that is on screen

	dist=0.0f;

	for (n=0;n!=poly->ptsz;n++) {

		pt=&mesh->vertexes[poly->v[n]];

		if (!gl_project_in_view_z(pt->x,pt->y,pt->z)) continue;
		
		d=gl_project_get_depth(pt->x,pt->y,pt->z);
		if (d>dist) dist=d;
	}

	return(dist);
}

void render_transparent_sort(void)
{
	int							n,k,i,sort_cnt,sort_idx;
	float						dist;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	map_poly_sort_item_type		*sort_list;

	sort_list=trans_sort.list;

		// create sort list

	sort_cnt=0;
		
	for (n=0;n!=view.render->draw_list.count;n++) {

		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];
		if (!mesh->draw.has_transparent) continue;
		
		for (k=0;k!=mesh->npoly;k++) {
		
			poly=&mesh->polys[k];
			if (!poly->draw.transparent_on) continue;

				// find distance from camera

			dist=render_transparent_poly_far_z(mesh,poly);

				// find position in sort list

			sort_idx=sort_cnt;

			for (i=0;i!=sort_cnt;i++) {
				if (dist>sort_list[i].dist) {
					sort_idx=i;
					break;
				}
			}

				// add to sort list

			if (sort_idx<sort_cnt) {
				memmove(&sort_list[sort_idx+1],&sort_list[sort_idx],((sort_cnt-sort_idx)*sizeof(map_poly_sort_item_type)));
			}

			sort_list[sort_idx].mesh_idx=view.render->draw_list.items[n].idx;
			sort_list[sort_idx].poly_idx=k;
			sort_list[sort_idx].dist=dist;

			sort_cnt++;
			if (sort_cnt>=max_sort_poly) break;

			poly++;
		}

		if (sort_cnt>=max_sort_poly) break;
	}

	trans_sort.count=sort_cnt;
}

/* =======================================================

      Enable and Disable Modes
      
======================================================= */

void render_transparent_mode_switch(int mode,bool additive)
{
		// change mode
		
	if (trans_cur_mode!=mode) {
	
			// only glow effects the blending mode
			
		if (trans_cur_mode==render_map_transparent_mode_glow) {
			glEnable(GL_BLEND);
		}
		else {
			if (mode==render_map_transparent_mode_glow) {
				glDisable(GL_BLEND);
			}
		}
		
			// simple and light map need color pointer
			
		if ((trans_cur_mode==render_map_transparent_mode_simple) || (trans_cur_mode==render_map_transparent_mode_light_map)) {
			if ((mode==render_map_transparent_mode_none) || (mode==render_map_transparent_mode_shader) || (mode==render_map_transparent_mode_glow)) {
				view_compile_gl_list_disable_color();
			}
		}
		else {
			if ((mode==render_map_transparent_mode_simple) || (mode==render_map_transparent_mode_light_map)) {
				view_compile_gl_list_enable_color();
			}
		}
	
			// turn off old mode
		
		switch (trans_cur_mode) {
		
			case render_map_transparent_mode_simple:
				gl_texture_transparent_end();
				break;
				
			case render_map_transparent_mode_light_map:
				gl_texture_transparent_light_map_end();
				break;
				
			case render_map_transparent_mode_shader:
				gl_shader_draw_end();
				break;
				
			case render_map_transparent_mode_glow:
				gl_texture_glow_end();
				break;
				
		}
			
			// turn on new mode
			
		switch (mode) {
		
			case render_map_transparent_mode_simple:
				gl_texture_transparent_start();
				view_compile_gl_list_attach_uv_simple();
				break;
				
			case render_map_transparent_mode_light_map:
				gl_texture_transparent_light_map_start();
				view_compile_gl_list_attach_uv_light_map();
				break;
				
			case render_map_transparent_mode_shader:
				gl_shader_draw_start();
				view_compile_gl_list_attach_uv_shader();
				break;
				
			case render_map_transparent_mode_glow:
				gl_texture_glow_start();
				view_compile_gl_list_attach_uv_glow();
				break;
				
		}
		
		trans_cur_mode=mode;
	}
	
		// change additive
		
	if (trans_cur_additive!=additive) {
	
		if (additive) {
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		}
		else {
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		}
		
		trans_cur_additive=additive;
	}
}

/* =======================================================

      Transparent Mesh Drawing
      
======================================================= */

void render_transparent_mesh_simple(map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int						frame;
	texture_type			*texture;

		// skip meshes or polys with shaders
		// or light maps unless debug is on

	if (!dim3_debug) {
		if (poly->draw.shader_on) return;
		if (mesh->draw.has_light_map) return;
	}
	
		// time to turn on some gl pointers?
		
	texture=&map.textures[poly->txt_idx];
	frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;
	
	render_transparent_mode_switch(render_map_transparent_mode_simple,texture->additive);

		// draw the polygon

	gl_texture_transparent_set(texture->frames[frame].bitmap.gl_id,1.0f);
	glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
}

void render_transparent_mesh_light_map(map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int						frame;
	texture_type			*texture,*lm_texture;

		// skip meshes or polys with no shaders
		// unless debug is on

	if (!dim3_debug) {
		if (poly->draw.shader_on) return;
		if (!mesh->draw.has_light_map) return;
	}
	
		// time to turn on some gl pointers?

	texture=&map.textures[poly->txt_idx];
	lm_texture=&map.textures[poly->lmap_txt_idx];
	frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;
	
	render_transparent_mode_switch(render_map_transparent_mode_light_map,texture->additive);

		// draw the polygon

	gl_texture_transparent_light_map_set(texture->frames[frame].bitmap.gl_id,lm_texture->frames[0].bitmap.gl_id,1.0f);
	glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
}

void render_transparent_mesh_shader(int mesh_idx,map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int						frame;
	texture_type			*texture;
	view_light_list_type	light_list;

		// skip meshes or polys with no shaders

	if (!poly->draw.shader_on) return;

		// time to turn on some gl pointers?

	texture=&map.textures[poly->txt_idx];
	frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;
	
	render_transparent_mode_switch(render_map_transparent_mode_shader,texture->additive);

		// draw shader

	if (!mesh->flag.hilite) {
		gl_lights_build_from_poly(mesh_idx,poly,&light_list);
		gl_shader_draw_execute(texture,poly->txt_idx,frame,poly->lmap_txt_idx,1.0f,&light_list,NULL,NULL,FALSE);
	}
	else {
		gl_shader_draw_execute(texture,poly->txt_idx,frame,poly->lmap_txt_idx,1.0f,NULL,&poly->box.mid,NULL,FALSE);
	}

	glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
}

void render_transparent_mesh_glow(map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int						frame;
	texture_type			*texture;

		// skip meshes or polys with no glows

	if ((!mesh->draw.has_glow) || (!poly->draw.glow_on)) return;

		// time to turn on some gl pointers?

	texture=&map.textures[poly->txt_idx];
	frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

	render_transparent_mode_switch(render_map_transparent_mode_glow,FALSE);
	
		// draw glow

	gl_texture_glow_set(texture->frames[frame].bitmap.gl_id,texture->frames[frame].glowmap.gl_id,texture->glow.current_color);
	glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
}

/* =======================================================

      Transparent Map Rendering
      
======================================================= */

void render_map_mesh_transparent(void)
{
	int						n,sort_cnt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	map_poly_sort_item_type	*sort_list;
	
		// setup view

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();
	
		// attach compiled vertex lists

	view_compile_gl_list_attach();

		// sort meshes

	render_transparent_sort();

		// common setup
		
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
				
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
		// remember current modes to
		// reduce number of state switches
		
	trans_cur_mode=render_map_transparent_mode_none;
	trans_cur_additive=FALSE;
	
		// run through the sorted meshes
		
	sort_cnt=trans_sort.count;
	sort_list=trans_sort.list;
		
	for (n=0;n!=sort_cnt;n++) {
		mesh=&map.mesh.meshes[sort_list[n].mesh_idx];
		poly=&mesh->polys[sort_list[n].poly_idx];

		render_transparent_mesh_simple(mesh,poly);
		if (!dim3_debug) {
			render_transparent_mesh_light_map(mesh,poly);
			render_transparent_mesh_shader(sort_list[n].mesh_idx,mesh,poly);
		}
		
		render_transparent_mesh_glow(mesh,poly);
	}
	
		// turn off any mode left on
		// from rendering
		
	render_transparent_mode_switch(render_map_transparent_mode_none,FALSE);

	glDepthMask(GL_TRUE);

		// dettach any attached lists

	view_compile_gl_list_dettach();
}

