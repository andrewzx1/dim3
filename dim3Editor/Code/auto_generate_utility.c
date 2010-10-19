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
#include "dialog.h"
#include "walk_view.h"

extern map_type					map;
extern file_path_setup_type		file_path_setup;

extern ag_state_type			ag_state;

/* =======================================================

      Random Utilities
      
======================================================= */

void ag_random_seed(int seed)
{
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
	int				n,cx,cz,
					nmesh,mesh_idx;
	d3pnt			pnt;
	map_mesh_type	*mesh;

		// flip around map center

	cx=map_max_size>>1;
	cz=map_max_size>>1;

		// skip first mesh as it's
		// the middle ground

	nmesh=map.mesh.nmesh;

	for (n=1;n!=nmesh;n++) {
		mesh_idx=map_mesh_duplicate(&map,n);
		if (mesh_idx==-1) break;

			// create mesh box so we
			// can find extents to move by

		mesh=&map.mesh.meshes[n];
		map_prepare_mesh_box(mesh);

			// flip and move

		map_mesh_flip(&map,mesh_idx,TRUE,FALSE,TRUE);

		pnt.x=(cx-mesh->box.max.x)*2;
		pnt.y=0;
		pnt.z=(cz-mesh->box.max.z)*2;

		map_mesh_move(&map,mesh_idx,&pnt);
	}
}

/* =======================================================

      Add Spots
      
======================================================= */

void ag_generate_spots_add_single(char *name,int spot_obj_type,char *script_name)
{
	int					idx,count,mx,mz;
	ag_shape_type		*shape;
	ag_room_type		*room;
	spot_type			*spot;

		// add the spot

	spot=&map.spots[map.nspot];
	map.nspot++;

		// find random position

	count=ag_random_int(ag_state.nroom);

	idx=0;

	while (TRUE) {
		if (idx>ag_state.nroom) idx=0;

		room=&ag_state.rooms[idx];
		shape=&ag_state.shapes[room->shape_idx];

			// ignore corridors

		if (shape->corridor) {
			idx++;
			continue;
		}

			// have we hit the right one?

		count--;
		if (count==0) break;

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
