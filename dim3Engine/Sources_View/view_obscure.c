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

bool view_obscure_check_box(int poly_cnt,d3pnt *min,d3pnt *mid,d3pnt *max)
{
	int					n;
	bool				hits[9];
	d3pnt				pnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	poly_pointer_type	*poly_ptr;

		// no hits

	for (n=0;n!=9;n++) {
		hits[n]=FALSE;
	}

		// check rays

	poly_ptr=view_obscure_polys;

	for (n=0;n!=poly_cnt;n++) {
		mesh=&map.mesh.meshes[poly_ptr->mesh_idx];
		poly=&mesh->polys[poly_ptr->poly_idx];

			// center

		if (ray_trace_single_poly_hit(mesh,poly,&view.render->camera.pnt,mid)) hits[0]=TRUE;

			// top corners

		pnt.y=min->y;

		pnt.x=min->x;
		pnt.z=min->z;
		if (!ray_trace_single_poly_hit(mesh,poly,&view.render->camera.pnt,&pnt)) hits[1]=TRUE;

		pnt.z=max->z;
		if (!ray_trace_single_poly_hit(mesh,poly,&view.render->camera.pnt,&pnt)) hits[2]=TRUE;

		pnt.x=max->x;
		if (!ray_trace_single_poly_hit(mesh,poly,&view.render->camera.pnt,&pnt)) hits[3]=TRUE;

		pnt.z=min->z;
		if (!ray_trace_single_poly_hit(mesh,poly,&view.render->camera.pnt,&pnt)) hits[4]=TRUE;

			// bottom corners

		pnt.y=max->y;

		pnt.x=min->x;
		pnt.z=min->z;
		if (!ray_trace_single_poly_hit(mesh,poly,&view.render->camera.pnt,&pnt)) hits[5]=TRUE;

		pnt.z=max->z;
		if (!ray_trace_single_poly_hit(mesh,poly,&view.render->camera.pnt,&pnt)) hits[6]=TRUE;

		pnt.x=max->x;
		if (!ray_trace_single_poly_hit(mesh,poly,&view.render->camera.pnt,&pnt)) hits[7]=TRUE;

		pnt.z=min->z;
		if (!ray_trace_single_poly_hit(mesh,poly,&view.render->camera.pnt,&pnt)) hits[8]=TRUE;

			// are we obscured?

		if (hits[0] && hits[1] && hits[2] && hits[3] && hits[4] && hits[5] && hits[6] && hits[7] && hits[8]) return(FALSE);

		poly_ptr++;
	}

	return(TRUE);
}

void view_obscure_run(void)
{
	int					n,k,mesh_idx,poly_cnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	poly_pointer_type	*poly_ptr;

	return;		// supergumba -- working on this

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
		// meshes

	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

		mesh_idx=view.render->draw_list.items[n].idx;
		mesh=&map.mesh.meshes[mesh_idx];

		if (!view_obscure_check_box(poly_cnt,&mesh->box.min,&mesh->box.mid,&mesh->box.max)) {
			fprintf(stdout,"Remove %d\n",mesh_idx);
			// supergumba -- remove here
		}


	}
}
