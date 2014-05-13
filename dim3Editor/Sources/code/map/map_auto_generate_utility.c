/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto Generation Utility

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern map_type					map;
extern file_path_setup_type		file_path_setup;

extern ag_state_type			ag_state;

extern void ag_generate_lights_add_spot(d3pnt *pnt);

/* =======================================================

      Random Utilities
      
======================================================= */

void ag_random_seed(void)
{
#ifdef D3_OS_WINDOWS
	ag_state.seed=GetTickCount();
#else
	ag_state.seed=TickCount();
#endif

	srandom(ag_state.seed);
}

void ag_random_previous_seed(void)
{
	ag_state.seed--;
	srandom(ag_state.seed);
}

void ag_random_next_seed(void)
{
	ag_state.seed++;
	srandom(ag_state.seed);
}

int ag_random_int(int max)
{
	float			f;

	f=(float)random()/(float)RAND_MAX;
	return((int)(f*(float)max));
}

bool ag_random_bool(void)
{
	return(ag_random_int(100)>=50);
}

/* =======================================================

      Delete Shared Polygons
      
======================================================= */

bool ag_generate_delete_shared_polygons_compare(map_mesh_type *mesh_1,int poly_1_idx,map_mesh_type *mesh_2,int poly_2_idx)
{
	int					n,k;
	bool				hit;
	d3pnt				*pt_1,*pt_2;
	map_mesh_poly_type	*poly_1,*poly_2;

	poly_1=&mesh_1->polys[poly_1_idx];
	poly_2=&mesh_2->polys[poly_2_idx];

		// same vertex count?

	if (poly_1->ptsz!=poly_2->ptsz) return(FALSE);

		// ignore ceilings or floors

	map_prepare_mesh_poly(&map,mesh_1,poly_1);
	if (poly_1->box.flat) return(FALSE);

	map_prepare_mesh_poly(&map,mesh_2,poly_2);
	if (poly_2->box.flat) return(FALSE);

		// might be in different orders, so look
		// for equal points anywhere in polygon

	for (n=0;n!=poly_1->ptsz;n++) {
		pt_1=&mesh_1->vertexes[poly_1->v[n]];

		hit=FALSE;
		for (k=0;k!=poly_2->ptsz;k++) {
			pt_2=&mesh_2->vertexes[poly_2->v[k]];
			if ((pt_1->x==pt_2->x) && (pt_1->y==pt_2->y) && (pt_1->z==pt_2->z)) {
				hit=TRUE;
				break;
			}
		}

		if (!hit) return(FALSE);
	}

	return(TRUE);
}

void ag_generate_delete_shared_polygons(void)
{
	int					n,mesh_1_idx,poly_1_idx,
						mesh_2_idx,poly_2_idx;
	map_mesh_type		*mesh_1,*mesh_2;

	for (mesh_1_idx=0;mesh_1_idx!=map.mesh.nmesh;mesh_1_idx++) {

		mesh_1=&map.mesh.meshes[mesh_1_idx];
		if (mesh_1->npoly==0) continue;

		if (mesh_1->polys[0].txt_idx==ag_texture_door) continue;		// special skip out for doors, as they will be moving and so can't assume textures remain hidden

		poly_1_idx=0;

		while (poly_1_idx<mesh_1->npoly) {

			poly_2_idx=-1;

			for (mesh_2_idx=0;mesh_2_idx!=map.mesh.nmesh;mesh_2_idx++) {
				if (mesh_2_idx==mesh_1_idx) continue;

				mesh_2=&map.mesh.meshes[mesh_2_idx];

				for (n=0;n!=mesh_2->npoly;n++) {
					if (ag_generate_delete_shared_polygons_compare(mesh_1,poly_1_idx,mesh_2,n)) {
						poly_2_idx=n;
						break;
					}
				}

				if (poly_2_idx!=-1) break;
			}

			if (poly_2_idx!=-1) {
				map_mesh_delete_poly(&map,mesh_1_idx,poly_1_idx);
				map_mesh_delete_poly(&map,mesh_2_idx,poly_2_idx);
				continue;
			}

			poly_1_idx++;
		}
	}
}

/* =======================================================

      Delete Polygons in Box
      
======================================================= */

void ag_generate_remove_polygons_in_box(int mesh_idx,d3pnt *min,d3pnt *max)
{
	int					n,poly_idx;
	bool				out;
	d3pnt				*pnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh=&map.mesh.meshes[mesh_idx];

	poly_idx=0;

	while (poly_idx<mesh->npoly) {

			// any vertexes outside the box?

		out=FALSE;
		poly=&mesh->polys[poly_idx];

		for (n=0;n!=poly->ptsz;n++) {

			pnt=&mesh->vertexes[poly->v[n]];

			if ((pnt->x<min->x) || (pnt->x>max->x) || (pnt->y<min->y) || (pnt->y>max->y) || (pnt->z<min->z) || (pnt->z>max->z)) {
				out=TRUE;
				break;
			}
		}

		if (out) {
			poly_idx++;
			continue;
		}

			// delete this mesh

		map_mesh_delete_poly(&map,mesh_idx,poly_idx);
	}
}

/* =======================================================

      Find Specific Polygons
      
======================================================= */

int ag_generate_find_floor_polygon(int room_idx)
{
	int					n,k;
	bool				flat;
	d3pnt				min,max;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	ag_room_type		*room;

	room=&ag_state.rooms[room_idx];

		// find the floor

	map_mesh_calculate_extent(&map,room->mesh_idx,&min,&max);

	mesh=&map.mesh.meshes[room->mesh_idx];

	for (n=0;n!=mesh->npoly;n++) {
		poly=&mesh->polys[n];

		if (poly->txt_idx!=ag_texture_floor) continue;

		flat=TRUE;

		for (k=0;k!=poly->ptsz;k++) {
			if (mesh->vertexes[mesh->polys[n].v[k]].y!=max.y) {
				flat=FALSE;
				break;
			}
		}

		if (flat) return(n);
	}

	return(-1);
}

bool ag_generate_is_polygon_window_target(int mesh_idx,int poly_idx)
{
	d3pnt				min,max;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// is Y high enough?

	map_mesh_poly_calculate_extent(&map,mesh_idx,poly_idx,&min,&max);
	if (abs(max.y-min.y)<ag_window_min_high) return(FALSE);

		// only items with 5 vertexes

	mesh=&map.mesh.meshes[mesh_idx];
	if (mesh->nvertex!=4) return(FALSE);

		// pairs of Ys

	poly=&mesh->polys[poly_idx];
	if (mesh->vertexes[poly->v[0]].y!=mesh->vertexes[poly->v[1]].y) return(FALSE);
	return(mesh->vertexes[poly->v[2]].y==mesh->vertexes[poly->v[3]].y);
}

/* =======================================================

      Windows
      
======================================================= */

void ag_generate_windows_add(void)
{
	int				n,k,t,mesh_idx,poly_idx,
					window_count;
	d3pnt			min,max,extrude_pnt,lit_pnt;
	d3vct			extrude_vct;
	map_mesh_type	*mesh;
	ag_room_type	*room;
	
	for (n=0;n!=ag_state.nroom;n++) {

		room=&ag_state.rooms[n];
		mesh_idx=room->mesh_idx;

		mesh=&map.mesh.meshes[mesh_idx];
		if (mesh->npoly==0) continue;

			// is this a windowed room?

		if (ag_random_int(100)>ag_window_random_percent) continue;

		room->has_windows=TRUE;

		window_count=ag_window_count_start+ag_random_int(ag_window_count_extra);

			// try a couple times to get
			// a polygon that would make a
			// good window candidate

		for (t=0;t!=window_count;t++) {
			for (k=0;k!=10;k++) {
				poly_idx=ag_random_int(mesh->npoly);

				map_mesh_poly_calculate_extent(&map,mesh_idx,poly_idx,&min,&max);
				if (abs(max.y-min.y)<ag_window_min_high) continue;
				
					// get extrude from the
					// normal of the wall
					
				extrude_vct.x=map.mesh.meshes[mesh_idx].polys[poly_idx].tangent_space.normal.x;
				extrude_vct.y=map.mesh.meshes[mesh_idx].polys[poly_idx].tangent_space.normal.y;
				extrude_vct.z=map.mesh.meshes[mesh_idx].polys[poly_idx].tangent_space.normal.z;
				
				extrude_pnt.x=-(int)(extrude_vct.x*(float)ag_window_depth);
				extrude_pnt.y=0;
				extrude_pnt.z=-(int)(extrude_vct.z*(float)ag_window_depth);

					// punch the window

				map_mesh_poly_punch_hole(&map,mesh_idx,poly_idx,&extrude_pnt,TRUE,normal_mode_in);

					// last polygon is always the extruded poly

				poly_idx=map.mesh.meshes[mesh_idx].npoly-1;
				map.mesh.meshes[mesh_idx].polys[poly_idx].txt_idx=ag_texture_window;

					// add in the light

				map_mesh_calculate_center(&map,mesh_idx,&lit_pnt);
				ag_generate_lights_add_spot(&lit_pnt);

				break;
			}
		}
	}
}



