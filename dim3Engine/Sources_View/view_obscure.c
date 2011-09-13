/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Ray Trace Obscuring

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
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
#include "objects.h"

#define view_obscure_split_div			15000
#define view_obscure_max_split			4
#define view_obscure_max_rays			((view_obscure_max_split+1)*(view_obscure_max_split+1)*(view_obscure_max_split+1))

extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;

d3pnt						*view_obscure_pnts;
poly_pointer_type			*view_obscure_polys;

/* =======================================================

      Initialize Obscuring
      
======================================================= */

bool view_obscure_initialize(void)
{
	int					n,k,cnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// is view obscuring on?

	view_obscure_polys=NULL;
	view_obscure_pnts=NULL;

	if (!map.optimize.ray_trace_obscure) return(TRUE);

		// count polys

	cnt=0;

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {
	
			// all polys in a mesh
			
		if (mesh->flag.obscuring) {
			cnt+=mesh->npoly;
		}
		
			// specific polys
			
		else {
			poly=mesh->polys;

			for (k=0;k!=mesh->npoly;k++) {
				if (poly->flag.obscuring) cnt++;
				poly++;
			}
		}
		
		mesh++;
	}

	if (cnt==0) return(TRUE);

		// one more poly for marker
		// at end

	cnt++;

		// memory for polys

	view_obscure_polys=(poly_pointer_type*)malloc(cnt*sizeof(poly_pointer_type));
	if (view_obscure_polys==NULL) return(FALSE);

		// memory for rays

	view_obscure_pnts=(d3pnt*)malloc(view_obscure_max_rays*sizeof(d3pnt));
	if (view_obscure_pnts==NULL) {
		free(view_obscure_polys);
		view_obscure_polys=NULL;
		return(FALSE);
	}

	return(TRUE);
}

void view_obscure_release(void)
{
	if (view_obscure_polys!=NULL) free(view_obscure_polys);
	view_obscure_polys=NULL;

	if (view_obscure_pnts!=NULL) free(view_obscure_pnts);
	view_obscure_pnts=NULL;
}

/* =======================================================

      Run Obscuring
      
======================================================= */

bool view_obscure_check_box(int skip_mesh_idx,d3pnt *min,d3pnt *max)
{
	int					k,x,y,z,kx,ky,ray_cnt,hit_cnt,last_mesh_idx;
	bool				hits[view_obscure_max_rays];
	bool				*hit;
	d3pnt				div,div_add,ray_min,ray_max;
	d3pnt				*pnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	poly_pointer_type	*poly_ptr;

		// find the divisions
		// we do ray tracing against a grid
		// created from the mesh box

		// we always have at least 0..2 divisions
		// so we hit corners and middle

	k=max->x-min->x;
	div.x=k/view_obscure_split_div;
	if (div.x<2) div.x=2;
	if (div.x>view_obscure_max_split) div.x=view_obscure_max_split;
	div_add.x=k/div.x;

	k=max->y-min->y;
	div.y=k/view_obscure_split_div;
	if (div.y<2) div.y=2;
	if (div.y>view_obscure_max_split) div.y=view_obscure_max_split;
	div_add.y=k/div.y;

	k=max->z-min->z;
	div.z=k/view_obscure_split_div;
	if (div.z<2) div.z=2;
	if (div.z>view_obscure_max_split) div.z=view_obscure_max_split;
	div_add.z=k/div.z;

		// build rays
		// get the min/max for quick mesh elimination

	ray_cnt=0;

	hit=hits;
	pnt=view_obscure_pnts;

	for (y=0;y<=div.y;y++) {
		ky=min->y+(y*div_add.y);

		for (x=0;x<=div.x;x++) {
			kx=min->x+(x*div_add.x);

			for (z=0;z<=div.z;z++) {
				pnt->x=kx;
				pnt->y=ky;
				pnt->z=min->z+(z*div_add.z);

				*hit++=FALSE;			// mark no hit here
				pnt++;

				ray_cnt++;
			}
		}
	}

		// get the rays min/max

	ray_min.x=ray_max.x=view.render->camera.pnt.x;
	ray_min.y=ray_max.y=view.render->camera.pnt.y;
	ray_min.z=ray_max.z=view.render->camera.pnt.z;

	if (min->x<ray_min.x) ray_min.x=min->x;
	if (max->x>ray_max.x) ray_max.x=max->x;
	if (min->y<ray_min.y) ray_min.y=min->y;
	if (max->y>ray_max.y) ray_max.y=max->y;
	if (min->z<ray_min.z) ray_min.z=min->z;
	if (max->z>ray_max.z) ray_max.z=max->z;

		// remember what the last mesh was
		// we do this so we can check when a mesh
		// changes to see if we can completely
		// eliminate meshes from comparison

	last_mesh_idx=-1;

		// check rays

	poly_ptr=view_obscure_polys;

	while (TRUE) {

			// last poly?

		if (poly_ptr->mesh_idx==-1) break;

			// if we are comparing meshes, don't
			// compare against itself, and then
			// skip all polys for this mesh

		if (skip_mesh_idx==poly_ptr->mesh_idx) {

			while (skip_mesh_idx==poly_ptr->mesh_idx) {
				poly_ptr++;
			}

			last_mesh_idx=-1;
			continue;
		}

			// are we in a new mesh?  Check all the rays
			// against the mesh, and skip all polys if
			// there isn't a collision

		mesh=&map.mesh.meshes[poly_ptr->mesh_idx];

		if (last_mesh_idx!=poly_ptr->mesh_idx) {
			last_mesh_idx=poly_ptr->mesh_idx;

			if ((ray_max.x<mesh->box.min.x) || (ray_min.x>mesh->box.max.x) || (ray_max.y<mesh->box.min.y) || (ray_min.y>mesh->box.max.y) || (ray_max.z<mesh->box.min.z) || (ray_min.z>mesh->box.max.z)) {

				while (last_mesh_idx==poly_ptr->mesh_idx) {
					poly_ptr++;
				}

				last_mesh_idx=-1;
				continue;
			}
		}

			// min-max poly elimination
			
		poly=&mesh->polys[poly_ptr->poly_idx];
		
		if ((ray_max.x<poly->box.min.x) || (ray_min.x>poly->box.max.x) || (ray_max.y<poly->box.min.y) || (ray_min.y>poly->box.max.y) || (ray_max.z<poly->box.min.z) || (ray_min.z>poly->box.max.z)) {
			poly_ptr++;
			continue;
		}
		
			// check the box
			// skipping checking rays that
			// all already obscured

		hit_cnt=0;

		for (k=0;k!=ray_cnt;k++) {
			if (hits[k]) {
				hit_cnt++;
				continue;
			}

			if (ray_trace_single_poly_hit(mesh,poly,&view.render->camera.pnt,&view_obscure_pnts[k])) {
				hits[k]=TRUE;
				hit_cnt++;
			}
		}

			// are we obscured?

		if (hit_cnt==ray_cnt) return(FALSE);

		poly_ptr++;
	}

	return(TRUE);
}

void view_obscure_run(void)
{
	int					n,k,idx,remove_count,org_count,
						mesh_idx;
	bool				remove;
	d3pnt				min,max;
	poly_pointer_type	*poly_ptr;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	obj_type			*obj;
	proj_type			*proj;
	model_type			*mdl;
	effect_type			*effect;

		// view obscure on?

	if (view_obscure_polys==NULL) return;

		// build the obscure polygons
		// to ray trace against

	poly_ptr=view_obscure_polys;

	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

		mesh_idx=view.render->draw_list.items[n].idx;
		mesh=&map.mesh.meshes[mesh_idx];
		
			// all over mesh obscure
			
		if (mesh->flag.obscuring) {
			poly=mesh->polys;

			for (k=0;k!=mesh->npoly;k++) {
				poly_ptr->mesh_idx=mesh_idx;
				poly_ptr->poly_idx=k;
				poly_ptr++;
				poly++;
			}
		}
		
			// single poly obscure
			
		else {
			poly=mesh->polys;

			for (k=0;k!=mesh->npoly;k++) {
				if (poly->flag.obscuring) {
					poly_ptr->mesh_idx=mesh_idx;
					poly_ptr->poly_idx=k;
					poly_ptr++;
				}

				poly++;
			}
		}
	}

	if (poly_ptr==view_obscure_polys) return;

		// end marker

	poly_ptr->mesh_idx=-1;

		// run through and obscure all
		// meshes, models, and effects

	idx=0;

	remove_count=0;
	org_count=view.render->draw_list.count;

	while (idx<view.render->draw_list.count) {

		remove=FALSE;

		switch (view.render->draw_list.items[idx].type) {

				// mesh

			case view_render_type_mesh:
				mesh_idx=view.render->draw_list.items[idx].idx;
				mesh=&map.mesh.meshes[mesh_idx];
				if (mesh->flag.never_obscure) break;

				if (!view_obscure_check_box(mesh_idx,&mesh->box.min,&mesh->box.max)) {
					remove=TRUE;
					view.count.mesh--;
				}
				break;

				// model

			case view_render_type_object:
				obj=server.obj_list.objs[view.render->draw_list.items[idx].idx];
				if ((obj->draw.model_idx==-1) || (!obj->draw.on)) break;
	
				mdl=server.model_list.models[obj->draw.model_idx];
				model_get_view_complex_bounding_volume(mdl,&obj->draw.pnt,&obj->ang,&min,&max);

				if (!view_obscure_check_box(-1,&min,&max)) {
					remove=TRUE;
					view.count.model--;
				}
				break;

			case view_render_type_projectile:
				proj=server.proj_list.projs[view.render->draw_list.items[idx].idx];
				if ((proj->draw.model_idx==-1) || (!proj->draw.on)) break;
	
				mdl=server.model_list.models[proj->draw.model_idx];
				model_get_view_complex_bounding_volume(mdl,&proj->draw.pnt,&proj->ang,&min,&max);

				if (!view_obscure_check_box(-1,&min,&max)) {
					remove=TRUE;
					view.count.model--;
				}
				break;

				// effects

			case view_render_type_effect:
				effect=server.effect_list.effects[view.render->draw_list.items[idx].idx];
				effect_draw_get_bound_box(effect,&min,&max);

				if (!view_obscure_check_box(-1,&min,&max)) {
					remove=TRUE;
					view.count.effect--;
				}
				break;

		}

		if (!remove) {
			idx++;
			continue;
		}

			// remove

		view.render->draw_list.items[idx].type=view_render_type_none;
		view.render->draw_list.count--;

		remove_count++;
	}

	view.count.obscure_percent=(100*remove_count)/org_count;
}
