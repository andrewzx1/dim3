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

#define view_obscure_split_div			20000
#define view_obscure_max_split			8
#define view_obscure_skip_range			50000

extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;

unsigned char				*view_obscure_hits;
d3pnt						*view_obscure_pnts;
poly_pointer_type			*view_obscure_polys;

/* =======================================================

      Initialize Obscuring
      
======================================================= */

bool view_obscure_initialize(void)
{
	int					n,k,cnt,ray_count;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// is view obscuring on?

	view_obscure_polys=NULL;
	view_obscure_pnts=NULL;
	view_obscure_hits=NULL;

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

		// memory for rays and hits
		// rays have grid divisions on each side
		// of a cube plus one extra for the end of
		// the division, and one more for the direct
		// to mid point check

	ray_count=(((view_obscure_max_split+1)*(view_obscure_max_split+1))*6)+1;

	view_obscure_pnts=(d3pnt*)malloc(ray_count*sizeof(d3pnt));
	if (view_obscure_pnts==NULL) {
		free(view_obscure_polys);
		view_obscure_polys=NULL;
		return(FALSE);
	}

	view_obscure_hits=(unsigned char*)malloc(ray_count*sizeof(unsigned char));
	if (view_obscure_hits==NULL) {
		free(view_obscure_polys);
		free(view_obscure_polys);
		view_obscure_polys=NULL;
		view_obscure_hits=NULL;
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

	if (view_obscure_hits!=NULL) free(view_obscure_hits);
	view_obscure_hits=NULL;
}

/* =======================================================

      Run Obscuring
      
======================================================= */

bool view_obscure_check_box(d3pnt *camera_pnt,int skip_mesh_idx,d3pnt *min,d3pnt *max)
{
	int					n,k,x,y,z,kx,ky,kz,ray_cnt,hit_cnt,last_mesh_idx;
	unsigned char		*hit;
	d3pnt				mid,div,div_add,ray_min,ray_max;
	d3pnt				*pnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	poly_pointer_type	*poly_ptr;

		// if camera is inside this box, do
		// not obscure

	if ((min->x>=camera_pnt->x) && (max->x<=camera_pnt->x) && (min->y>=camera_pnt->y) && (max->y<=camera_pnt->y) && (min->z>=camera_pnt->z) && (max->z<=camera_pnt->z)) return(TRUE);

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

		// get the mid point

	mid.x=(min->x+max->x)>>1;
	mid.y=(min->y+max->y)>>1;
	mid.z=(min->z+max->z)>>1;

		// build rays
		// we check against grids on a cube
		// we auto-eliminate sides of the cube
		// that aren't seen from the camera point

	ray_cnt=0;

	pnt=view_obscure_pnts;

		// top and bottom

	for (x=0;x<=div.x;x++) {
		kx=min->x+(x*div_add.x);

		for (z=0;z<=div.z;z++) {
			kz=min->z+(z*div_add.z);

			if (camera_pnt->y<=mid.y) {
				pnt->x=kx;
				pnt->y=min->y;
				pnt->z=kz;
			}
			else {
				pnt->x=kx;
				pnt->y=max->y;
				pnt->z=kz;
			}

			pnt++;
			ray_cnt++;
		}
	}

		// left and right

	for (y=0;y<=div.y;y++) {
		ky=min->y+(y*div_add.y);

		for (z=0;z<=div.z;z++) {
			kz=min->z+(z*div_add.z);

			if (camera_pnt->x<=mid.x) {
				pnt->x=min->x;
				pnt->y=ky;
				pnt->z=kz;
			}
			else {
				pnt->x=max->x;
				pnt->y=ky;
				pnt->z=kz;
			}

			pnt++;
			ray_cnt++;
		}
	}

		// front and back

	for (x=0;x<=div.x;x++) {
		kx=min->x+(x*div_add.x);

		for (y=0;y<=div.y;y++) {
			ky=min->y+(y*div_add.y);

			if (camera_pnt->z<=mid.z) {
				pnt->x=kx;
				pnt->y=ky;
				pnt->z=min->z;
			}
			else {
				pnt->x=kx;
				pnt->y=ky;
				pnt->z=max->z;
			}

			pnt++;
			ray_cnt++;
		}
	}

		// the midpoint ray

	pnt->x=mid.x;
	pnt->y=mid.y;
	pnt->z=mid.z;

	ray_cnt++;

		// mark no hits

	hit=view_obscure_hits;

	for (n=0;n!=ray_cnt;n++) {
		*hit++=0x0;
	}

		// get the rays min/max

	ray_min.x=ray_max.x=camera_pnt->x;
	ray_min.y=ray_max.y=camera_pnt->y;
	ray_min.z=ray_max.z=camera_pnt->z;

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
			if (*(view_obscure_hits+k)==0x1) {
				hit_cnt++;
				continue;
			}

			if (ray_trace_single_poly_hit(mesh,poly,camera_pnt,&view_obscure_pnts[k])) {
				*(view_obscure_hits+k)=0x1;
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
	int					n,k,remove_count,org_count,
						mesh_idx;
	d3pnt				min,max,camera_pnt;
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

		// build the camera pnt

	camera_pnt.x=view.render->camera.pnt.x;
	camera_pnt.y=view.render->camera.pnt.y;
	camera_pnt.z=view.render->camera.pnt.z+camera.setup.plane.near_z;

		// run through and obscure all
		// meshes, models, and effects

	remove_count=0;
	org_count=view.render->draw_list.count;

	for (n=0;n!=view.render->draw_list.count;n++) {

			// if too close, don't obscure

		if (view.render->draw_list.items[n].dist<view_obscure_skip_range) continue;

			// check for removal

		switch (view.render->draw_list.items[n].type) {

				// mesh

			case view_render_type_mesh:
				mesh_idx=view.render->draw_list.items[n].idx;
				mesh=&map.mesh.meshes[mesh_idx];
				if (mesh->flag.never_obscure) break;

				if (!view_obscure_check_box(&camera_pnt,mesh_idx,&mesh->box.min,&mesh->box.max)) {
					view.render->draw_list.items[n].type=view_render_type_none;
					remove_count++;
					view.count.mesh--;
				}
				break;

				// model

			case view_render_type_object:
				obj=server.obj_list.objs[view.render->draw_list.items[n].idx];
				if ((obj->draw.model_idx==-1) || (!obj->draw.on)) break;
	
				mdl=server.model_list.models[obj->draw.model_idx];
				model_get_view_complex_bounding_volume(mdl,&obj->draw.pnt,&obj->ang,&min,&max);

				if (!view_obscure_check_box(&camera_pnt,-1,&min,&max)) {
					view.render->draw_list.items[n].type=view_render_type_none;
					remove_count++;
					view.count.model--;
				}
				break;

			case view_render_type_projectile:
				proj=server.proj_list.projs[view.render->draw_list.items[n].idx];
				if ((proj->draw.model_idx==-1) || (!proj->draw.on)) break;
	
				mdl=server.model_list.models[proj->draw.model_idx];
				model_get_view_complex_bounding_volume(mdl,&proj->draw.pnt,&proj->ang,&min,&max);

				if (!view_obscure_check_box(&camera_pnt,-1,&min,&max)) {
					view.render->draw_list.items[n].type=view_render_type_none;
					remove_count++;
					view.count.model--;
				}
				break;

				// effects

			case view_render_type_effect:
				effect=server.effect_list.effects[view.render->draw_list.items[n].idx];
				effect_draw_get_bound_box(effect,&min,&max);

				if (!view_obscure_check_box(&camera_pnt,-1,&min,&max)) {
					view.render->draw_list.items[n].type=view_render_type_none;
					remove_count++;
					view.count.effect--;
				}
				break;

		}
	}

	view.count.obscure_percent=(100*remove_count)/org_count;
}
