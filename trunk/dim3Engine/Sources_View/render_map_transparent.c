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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

extern map_type			map;
extern server_type		server;
extern view_type		view;
extern camera_type		camera;
extern setup_type		setup;

map_poly_sort_type		trans_sort;

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
						
				// skip culling

			if (render_check_poly_cull(mesh,poly)) {
				poly++;
				continue;
			}
			
				// find distance from camera

			dist=render_transparent_poly_far_z(mesh,poly);

				// find position in sort list
				// always check from the end of the list up

			sort_idx=sort_cnt;

			for (i=0;i!=sort_cnt;i++) {
				if (dist>sort_list[i].dist) {
					sort_idx=i+1;
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

      Transparent Map Rendering
      
======================================================= */

void render_map_mesh_transparent(void)
{
	int							n,mesh_idx,cur_mesh_idx,
								frame;
	bool						in_additive,lighting_small;
	texture_type				*texture;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	view_glsl_light_list_type	light_list;

		// setup view

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();
	
		// sort meshes

	render_transparent_sort();

		// setup
		
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
				
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		// run through draw list

	in_additive=FALSE;
	cur_mesh_idx=-1;
	lighting_small=FALSE;
		
	for (n=0;n!=trans_sort.count;n++) {

		mesh_idx=trans_sort.list[n].mesh_idx;
		mesh=&map.mesh.meshes[mesh_idx];
		poly=&mesh->polys[trans_sort.list[n].poly_idx];

			// check for a changing mesh,
			// and reset some drawing operations
			
		if (cur_mesh_idx!=mesh_idx) {
		
			if (cur_mesh_idx!=-1) {
				view_unbind_mesh_liquid_vertex_object();
				view_unbind_mesh_liquid_index_object();
			}
			
				// mesh VBO

			cur_mesh_idx=mesh_idx;
			
			view_bind_mesh_liquid_vertex_object(&mesh->vbo);
			view_bind_mesh_liquid_index_object(&mesh->vbo);

				// small meshes don't create light lists
				// per-poly, instead just use the mesh list
				// if the mesh list always has less than
				// max shader lights, just use that

			lighting_small=((mesh->precalc_flag.lighting_small) || (mesh->light_cache.count<=max_shader_light));
			if (lighting_small) gl_lights_build_mesh_glsl_light_list(mesh,&light_list);
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

		if (!lighting_small) gl_lights_build_poly_glsl_light_list(mesh,poly,&light_list);
		gl_shader_draw_execute_map(texture,poly->txt_idx,frame,poly->lmap_txt_idx,1.0f,0,(3*sizeof(float)),(5*sizeof(float)),(7*sizeof(float)),(10*sizeof(float)),mesh->vbo.vertex_stride,&light_list);

		glDrawElements(GL_TRIANGLE_FAN,poly->ptsz,GL_UNSIGNED_SHORT,(GLvoid*)poly->vbo.index_offset);
		
		view.count.mesh_poly++;
	}

		// if we setup a mesh, end the vbo
		
	if (cur_mesh_idx!=-1) {
		view_unbind_mesh_liquid_vertex_object();
		view_unbind_mesh_liquid_index_object();
	}
		
	if (in_additive) glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
		// put depth mask back
		
	glDepthMask(GL_TRUE);
}
