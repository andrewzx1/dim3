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

/* =======================================================

      Random Utilities
      
======================================================= */

void ag_random_seed(void)
{
	int				seed;

#ifdef D3_OS_WINDOWS
	seed=GetTickCount();
#else
	seed=TickCount();
#endif

//	seed=102;		// supergumba -- for testing

	srandom(seed);
}

int ag_random_int(int max)
{
	float			f;

	f=(float)random()/(float)RAND_MAX;
	return((int)(f*(float)max));
}

/* =======================================================

      Mirror Meshes
      
======================================================= */

void ag_generate_mirror_meshes(void)
{
	int				n,nmesh,mesh_idx;
	d3pnt			pnt;
	map_mesh_type	*mesh,*center_mesh;

		// get center mesh

	center_mesh=&map.mesh.meshes[0];
	map_prepare_mesh_box(center_mesh);

		// skip first mesh as it's
		// the middle ground

	nmesh=map.mesh.nmesh;

	for (n=1;n!=nmesh;n++) {
		mesh_idx=map_mesh_duplicate(&map,n);
		if (mesh_idx==-1) break;

			// create mesh box so we
			// can find extents to move by

		mesh=&map.mesh.meshes[mesh_idx];
		map_prepare_mesh_box(mesh);

			// flip and move

		map_mesh_flip(&map,mesh_idx,TRUE,FALSE,TRUE);

		pnt.x=(center_mesh->box.mid.x-mesh->box.mid.x)*2;
		pnt.y=0;
		pnt.z=(center_mesh->box.mid.z-mesh->box.mid.z)*2;

		map_mesh_move(&map,mesh_idx,&pnt);
	}
}

/* =======================================================

      Polygon Utilies
      
======================================================= */

bool ag_generate_is_poly_straight_wall(int mesh_idx,int poly_idx)
{
	map_mesh_poly_type		*poly;

	poly=&map.mesh.meshes[mesh_idx].polys[poly_idx];
	if (!poly->box.wall_like) return(FALSE);
	return((poly->box.min.x==poly->box.max.x) || (poly->box.min.z==poly->box.max.z));
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

      Add Spots
      
======================================================= */

void ag_generate_spots_add_single(char *name,int spot_obj_type,char *script_name)
{
	int					idx,count,mx,mz;
	ag_room_type		*room;
	spot_type			*spot;

		// add the spot

	spot=&map.spots[map.nspot];
	map.nspot++;

		// find random position

	count=ag_random_int(ag_state.nroom);

	idx=0;

	while (TRUE) {

		if (idx==ag_state.nroom) {
			idx=0;
			break;
		}

			// skip rooms that don't	
			// allow spawn spots
			// decriment count just in case there
			// are no other rooms

		room=&ag_state.rooms[idx];

		if (room->shape_idx==-1)  {
			count--;
			idx++;
			continue;
		}

		if (!ag_state.shapes[room->shape_idx].spawn_spots) {
			count--;
			idx++;
			continue;
		}

			// have we hit the right one?

		if (count<=0) break;
		count--;

		idx++;
	}

		// randomize location

	room=&ag_state.rooms[idx];

	mx=(room->min.x+room->max.x)>>1;
	mz=(room->min.z+room->max.z)>>1;

	spot->pnt.x=mx+(ag_random_int((int)(room->size.x*50.0f))-((int)(room->size.x*25.0f)));
	spot->pnt.y=room->max.y;
	spot->pnt.z=mz+(ag_random_int((int)(room->size.z*50.0f))-((int)(room->size.z*25.0f)));

	spot->ang.x=0.0f;
	spot->ang.y=angle_find(spot->pnt.x,spot->pnt.z,mx,mz);
	spot->ang.z=0.0f;

		// name and type
	
	strcpy(spot->name,name);
	spot->type=spot_obj_type;
	strcpy(spot->script,script_name);
	spot->display_model[0]=0x0;
	spot->params[0]=0x0;
}

void ag_generate_spots_add(void)
{
		// player spot

	ag_generate_spots_add_single("Start",spot_type_player,"Player");

		// other spots

}
