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

extern map_type			map;
extern server_type		server;
extern view_type		view;
extern camera_type		camera;
extern setup_type		setup;

extern bitmap_type		lmap_hilite_bitmap;

map_poly_sort_type		trans_sort;

extern bool fog_solid_on(void);
extern void view_compile_gl_list_attach(void);
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
						
				// skip polys with away facing normals
				// do dot product between normal and vector
				// from poly mid-eye point
				
			if (!poly->never_cull) {
				if (((poly->tangent_space.normal.x*(float)(poly->box.mid.x-view.render->camera.pnt.x))+(poly->tangent_space.normal.y*(float)(poly->box.mid.y-view.render->camera.pnt.y))+(poly->tangent_space.normal.z*(float)(poly->box.mid.z-view.render->camera.pnt.z)))>poly_cull_dot_product) continue;
			}
			
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

      Transparent Mesh Drawing
      
======================================================= */

void render_transparent_mesh_normal(void)
{
	int						n,frame;
	bool					first_draw,in_additive;
	GLuint					gl_id,lmap_gl_id;
	texture_type			*texture;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	first_draw=TRUE;
	in_additive=FALSE;

	for (n=0;n!=trans_sort.count;n++) {
		mesh=&map.mesh.meshes[trans_sort.list[n].mesh_idx];
		poly=&mesh->polys[trans_sort.list[n].poly_idx];

			// mode switches

		if (first_draw) {
			first_draw=FALSE;
			gl_texture_transparent_light_map_start();
			view_compile_gl_list_attach_uv_light_map();
			view_compile_gl_list_enable_color();
		}

			// textures

		texture=&map.textures[poly->txt_idx];
		frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

		gl_id=texture->frames[frame].bitmap.gl_id;

		if (poly->lmap_txt_idx==-1) {
			lmap_gl_id=lmap_hilite_bitmap.gl_id;
		}
		else {
			lmap_gl_id=map.textures[poly->lmap_txt_idx].frames[frame].bitmap.gl_id;
		}

		if (texture->additive!=in_additive) {
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			in_additive=TRUE;
		}
		else {
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			in_additive=FALSE;
		}

			// draw the polygon

		gl_texture_transparent_light_map_set(gl_id,lmap_gl_id,1.0f);
		glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
	
		view.count.poly++;
	}

	if (!first_draw) {
		gl_texture_transparent_light_map_end();
		view_compile_gl_list_disable_color();
		
		if (in_additive) glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}
}

void render_transparent_mesh_shader(void)
{
	int						n,mesh_idx,frame;
	bool					first_draw,in_additive;
	texture_type			*texture;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	view_light_list_type	light_list;

	first_draw=TRUE;
	in_additive=FALSE;

	for (n=0;n!=trans_sort.count;n++) {

		mesh_idx=trans_sort.list[n].mesh_idx;
		mesh=&map.mesh.meshes[mesh_idx];
		poly=&mesh->polys[trans_sort.list[n].poly_idx];

			// mode switches

		if (first_draw) {
			first_draw=FALSE;
			gl_shader_draw_start();
			view_compile_gl_list_attach_uv_shader();
		}

			// textures

		texture=&map.textures[poly->txt_idx];
		frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

		if (texture->additive!=in_additive) {
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			in_additive=TRUE;
		}
		else {
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			in_additive=FALSE;
		}

			// draw the polygon

		gl_lights_build_poly_light_list(mesh_idx,poly,&light_list);
		gl_shader_draw_execute(TRUE,texture,poly->txt_idx,frame,poly->lmap_txt_idx,1.0f,&light_list,FALSE,NULL,NULL,&poly->tangent_space,NULL);

		glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
	
		view.count.poly++;
	}

	if (!first_draw) {
		gl_shader_draw_end();
		
		if (in_additive) glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}
}

void render_transparent_mesh_glow(void)
{
	int						n,frame;
	bool					first_draw;
	texture_type			*texture;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	first_draw=TRUE;

	for (n=0;n!=trans_sort.count;n++) {
		mesh=&map.mesh.meshes[trans_sort.list[n].mesh_idx];
		poly=&mesh->polys[trans_sort.list[n].poly_idx];
		
		if (!poly->draw.glow_on) continue;

			// mode switches

		if (first_draw) {
			first_draw=FALSE;
			gl_texture_glow_start();
			view_compile_gl_list_attach_uv_glow();
		}

			// textures

		texture=&map.textures[poly->txt_idx];
		frame=(texture->animate.current_frame+poly->draw.txt_frame_offset)&max_texture_frame_mask;

			// draw glow

		gl_texture_glow_set(texture->frames[frame].bitmap.gl_id,texture->frames[frame].glowmap.gl_id,texture->glow.current_color);
		glDrawRangeElements(GL_POLYGON,poly->draw.gl_poly_index_min,poly->draw.gl_poly_index_max,poly->ptsz,GL_UNSIGNED_INT,(GLvoid*)poly->draw.gl_poly_index_offset);
	
		view.count.poly++;
	}

	if (!first_draw) {
		gl_texture_glow_end();
	}
}

/* =======================================================

      Transparent Map Rendering
      
======================================================= */

void render_map_mesh_transparent(void)
{
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

		// draw the polygons

	if (view.shader_on) {
		render_transparent_mesh_shader();
	}
	else {
		render_transparent_mesh_normal();
	}

	render_transparent_mesh_glow();

		// put depth mask back
		
	glDepthMask(GL_TRUE);

		// dettach any lists

	view_compile_gl_list_dettach();
}

