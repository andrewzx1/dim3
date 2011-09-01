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

bool view_obscure_check_box(int poly_cnt,d3pnt *min,d3pnt *max)
{
	int					n,k;
	bool				hits[9];
	d3pnt				pnts[9];
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	poly_pointer_type	*poly_ptr;

		// no hits

	for (n=0;n!=9;n++) {
		hits[n]=FALSE;
	}

		// build rays

	pnts[0].x=(min->x+max->x)>>1;
	pnts[0].y=(min->y+max->y)>>1;
	pnts[0].z=(min->z+max->z)>>1;

	pnts[1].y=pnts[2].y=pnts[3].y=pnts[4].y=min->y;
	pnts[5].y=pnts[6].y=pnts[7].y=pnts[8].y=max->y;

	pnts[1].x=pnts[4].x=pnts[5].x=pnts[8].x=min->x;
	pnts[2].x=pnts[3].x=pnts[6].x=pnts[7].x=max->x;

	pnts[1].z=pnts[2].z=pnts[5].z=pnts[6].z=min->z;
	pnts[3].z=pnts[4].z=pnts[7].z=pnts[8].z=max->z;

		// check rays

	poly_ptr=view_obscure_polys;

	for (n=0;n!=poly_cnt;n++) {
		mesh=&map.mesh.meshes[poly_ptr->mesh_idx];
		poly=&mesh->polys[poly_ptr->poly_idx];

		for (k=0;k!=9;k++) {
			if (!hits[k]) hits[k]=ray_trace_single_poly_hit(mesh,poly,&view.render->camera.pnt,&pnts[k]);
		}

			// are we obscured?

		if (hits[0] && hits[1] && hits[2] && hits[3] && hits[4] && hits[5] && hits[6] && hits[7] && hits[8]) return(FALSE);

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
				mesh=&map.mesh.meshes[view.render->draw_list.items[idx].idx];

				if (!view_obscure_check_box(poly_cnt,&mesh->box.min,&mesh->box.max)) {
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

				if (!view_obscure_check_box(poly_cnt,&min,&max)) {
					remove=TRUE;
					view.count.model--;
				}
				break;

			case view_render_type_projectile:
				proj=server.proj_list.projs[view.render->draw_list.items[idx].idx];
				if ((proj->draw.model_idx==-1) || (!proj->draw.on)) break;
	
				mdl=server.model_list.models[proj->draw.model_idx];
				model_get_view_complex_bounding_volume(mdl,&proj->draw.pnt,&proj->ang,&min,&max);

				if (!view_obscure_check_box(poly_cnt,&min,&max)) {
					remove=TRUE;
					view.count.model--;
				}
				break;

				// effects

			case view_render_type_effect:
				effect=server.effect_list.effects[view.render->draw_list.items[idx].idx];
				effect_draw_get_bound_box(effect,&min,&max);

				if (!view_obscure_check_box(poly_cnt,&min,&max)) {
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
