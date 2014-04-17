/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto Generation Room Second Story

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

extern ag_state_type			ag_state;

extern int ag_random_int(int max);
extern bool ag_random_bool(void);
extern void ag_generate_remove_polygons_in_box(int mesh_idx,d3pnt *min,d3pnt *max);

/* =======================================================

      Walkways
      
======================================================= */

void ag_generate_add_room_second_story_walkway_horz(int room_idx)
{
	int					mesh_idx,z,tz,bz;
	int					px[8],py[8],pz[8];
	float				gx[8],gy[8];
	d3pnt				min,max;
	ag_room_type		*room;

	room=&ag_state.rooms[room_idx];

		// get midpoint
	
	map_mesh_calculate_extent(&map,room->mesh_idx,&min,&max);

	z=(max.z+min.z)>>1;
	tz=z-(ag_story_walkway_width>>1);
	bz=z+(ag_story_walkway_width>>1);

		// add the mesh

	mesh_idx=map_mesh_add(&map);

		// walls

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

	px[0]=px[3]=min.x;
	px[1]=px[2]=max.x;
	pz[0]=pz[2]=pz[1]=pz[3]=tz;
	py[0]=py[1]=max.y-(ag_size_room_high+ag_size_floor_high);
	py[2]=py[3]=max.y-ag_size_room_high;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

	pz[0]=pz[2]=pz[1]=pz[3]=bz;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);
	
		// floor and ceiling

	px[0]=px[3]=min.x;
	px[1]=px[2]=max.x;
	pz[0]=pz[1]=tz;
	pz[2]=pz[3]=bz;
	py[0]=py[1]=py[2]=py[3]=max.y-ag_size_room_high;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

	py[0]=py[1]=py[2]=py[3]=max.y-(ag_size_room_high+ag_size_floor_high);

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		// recalc normals

	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_out,FALSE);
}

void ag_generate_add_room_second_story_walkway_vert(int room_idx)
{
	int					mesh_idx,x,lx,rx;
	int					px[8],py[8],pz[8];
	float				gx[8],gy[8];
	d3pnt				min,max;
	ag_room_type		*room;

	room=&ag_state.rooms[room_idx];

		// get midpoint
	
	map_mesh_calculate_extent(&map,room->mesh_idx,&min,&max);

	x=(max.x+min.x)>>1;
	lx=x-(ag_story_walkway_width>>1);
	rx=x+(ag_story_walkway_width>>1);

		// add the mesh

	mesh_idx=map_mesh_add(&map);

		// walls

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

	px[0]=px[2]=px[1]=px[3]=lx;
	pz[0]=pz[3]=min.z;
	pz[1]=pz[2]=max.z;
	py[0]=py[1]=max.y-(ag_size_room_high+ag_size_floor_high);
	py[2]=py[3]=max.y-ag_size_room_high;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

	px[0]=px[2]=px[1]=px[3]=rx;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);
	
		// floor and ceiling

	px[0]=px[1]=lx;
	px[2]=px[3]=rx;
	pz[0]=pz[3]=min.z;
	pz[1]=pz[2]=max.z;
	py[0]=py[1]=py[2]=py[3]=room->connect_box.max.y-ag_size_room_high;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

	py[0]=py[1]=py[2]=py[3]=room->connect_box.max.y-(ag_size_room_high+ag_size_floor_high);

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		// recalc normals

	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_out,FALSE);
}

/* =======================================================

      Add Room Second Story Pieces
      
======================================================= */

void ag_generate_add_room_second_story(void)
{
	int				n;
	ag_room_type	*room;

	for (n=0;n!=ag_state.nroom;n++) {

			// did this room have
			// a second story

		room=&ag_state.rooms[n];
		if (!room->second_story) continue;

			// possibly second story
			// additions

		switch (ag_random_int(3)) {

			case 0:
				break;

			case 1:
				ag_generate_add_room_second_story_walkway_horz(n);
				break;

			case 2:
				ag_generate_add_room_second_story_walkway_vert(n);
				break;

		}
	}
}

