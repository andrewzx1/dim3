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
#define view_obscure_max_rays			(view_obscure_max_split*view_obscure_max_split*view_obscure_max_split)

extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;

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

	if (!map.optimize.ray_trace_obscure) return(TRUE);

		// count polys

	cnt=0;

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {
			if (poly->flag.obscuring) cnt++;
			poly++;
		}

		mesh++;
	}

	if (cnt==0) return(TRUE);

		// memory for polys

	view_obscure_polys=(poly_pointer_type*)malloc(cnt*sizeof(poly_pointer_type));
	return(view_obscure_polys!=NULL);
}

void view_obscure_release(void)
{
	if (view_obscure_polys!=NULL) free(view_obscure_polys);
	view_obscure_polys=NULL;
}

/* =======================================================

      Run Obscuring
      
======================================================= */

bool view_obscure_check_box(int poly_cnt,int skip_mesh_idx,d3pnt *min,d3pnt *max)
{
	int					n,k,x,y,z,kx,ky,ray_cnt,hit_cnt;
	bool				*hit,hits[view_obscure_max_rays];
	d3pnt				div,*pnt,pnts[view_obscure_max_rays];
	d3fpnt				div_add;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	poly_pointer_type	*poly_ptr;

		// find the divisions
		// we do ray tracing against a grid
		// created from the mesh box

	k=max->x-min->x;
	div.x=k/view_obscure_split_div;
	if (div.x<1) div.x=1;
	if (div.x>view_obscure_max_split) div.x=view_obscure_max_split;
	div_add.x=((float)k)/((float)div.x);

	k=max->y-min->y;
	div.y=k/view_obscure_split_div;
	if (div.y<1) div.y=1;
	if (div.y>view_obscure_max_split) div.y=view_obscure_max_split;
	div_add.y=((float)k)/((float)div.y);

	k=max->z-min->z;
	div.z=k/view_obscure_split_div;
	if (div.z<1) div.z=1;
	if (div.z>view_obscure_max_split) div.z=view_obscure_max_split;
	div_add.z=((float)k)/((float)div.z);

		// build rays

	ray_cnt=0;

	hit=hits;
	pnt=pnts;

	for (y=0;y!=div.y;y++) {
		ky=min->y+(int)(((float)y)*div_add.y);

		for (x=0;x!=div.x;x++) {
			kx=min->x+(int)(((float)x)*div_add.x);

			for (z=0;z!=div.z;z++) {
				pnt->x=kx;
				pnt->y=ky;
				pnt->z=min->z+(int)(((float)z)*div_add.z);

				*hit++=FALSE;			// mark no hit here
				pnt++;

				ray_cnt++;
			}
		}
	}

		// check rays

	poly_ptr=view_obscure_polys;

	for (n=0;n!=poly_cnt;n++) {

			// if we are comparing meshes, don't
			// compare against itself

		if (skip_mesh_idx==poly_ptr->mesh_idx) {
			poly_ptr++;
			continue;
		}

			// check the box

		mesh=&map.mesh.meshes[poly_ptr->mesh_idx];
		poly=&mesh->polys[poly_ptr->poly_idx];

		hit_cnt=0;

		for (k=0;k!=ray_cnt;k++) {
			if (!hits[k]) hits[k]=ray_trace_single_poly_hit(mesh,poly,&view.render->camera.pnt,&pnts[k]);
			if (hits[k]) hit_cnt++;
		}

			// are we obscured?

		if (hit_cnt==ray_cnt) return(FALSE);

		poly_ptr++;
	}

	return(TRUE);
}

void view_obscure_run(void)
{
	int					n,k,idx,sz,
						mesh_idx,poly_cnt;
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

	poly_cnt=0;
	poly_ptr=view_obscure_polys;

	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

		mesh_idx=view.render->draw_list.items[n].idx;
		mesh=&map.mesh.meshes[mesh_idx];

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {
			if (poly->flag.obscuring) {
				poly_ptr->mesh_idx=mesh_idx;
				poly_ptr->poly_idx=k;
				poly_ptr++;
				poly_cnt++;
			}

			poly++;
		}
	}

	if (poly_cnt==0) return;

		// run through and obscure all
		// meshes, models, and effects

	idx=0;

	while (idx<view.render->draw_list.count) {

		remove=FALSE;

		switch (view.render->draw_list.items[idx].type) {

				// mesh

			case view_render_type_mesh:
				mesh_idx=view.render->draw_list.items[idx].idx;
				mesh=&map.mesh.meshes[mesh_idx];

				if (!view_obscure_check_box(poly_cnt,mesh_idx,&mesh->box.min,&mesh->box.max)) {
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

				if (!view_obscure_check_box(poly_cnt,-1,&min,&max)) {
					remove=TRUE;
					view.count.model--;
				}
				break;

			case view_render_type_projectile:
				proj=server.proj_list.projs[view.render->draw_list.items[idx].idx];
				if ((proj->draw.model_idx==-1) || (!proj->draw.on)) break;
	
				mdl=server.model_list.models[proj->draw.model_idx];
				model_get_view_complex_bounding_volume(mdl,&proj->draw.pnt,&proj->ang,&min,&max);

				if (!view_obscure_check_box(poly_cnt,-1,&min,&max)) {
					remove=TRUE;
					view.count.model--;
				}
				break;

				// effects

			case view_render_type_effect:
				effect=server.effect_list.effects[view.render->draw_list.items[idx].idx];
				effect_draw_get_bound_box(effect,&min,&max);

				if (!view_obscure_check_box(poly_cnt,-1,&min,&max)) {
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

		sz=(view.render->draw_list.count-(idx+1));
		if (sz>0) {
			sz*=sizeof(view_render_draw_list_item_type);
			memmove(&view.render->draw_list.items[idx],&view.render->draw_list.items[idx+1],sz);
		}
		
		view.render->draw_list.count--;
	}
}
