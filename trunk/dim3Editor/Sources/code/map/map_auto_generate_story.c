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

      Full Room Hole Second Stories
      
======================================================= */

void ag_generate_add_room_second_story_hole(int room_idx)
{
	int				n,n2,mesh_idx,
					floor_poly_idx,ceiling_poly_idx;
	int				px[8],py[8],pz[8];
	float			gx[8],gy[8];
	d3pnt			extrude_pnt;
	ag_room_type	*room;

	room=&ag_state.rooms[room_idx];

		// add the mesh

	mesh_idx=map_mesh_add(&map);

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

		// walls

	for (n=0;n!=room->nvertex;n++) {
		n2=n+1;
		if (n2==room->nvertex) n2=0;

		px[0]=px[3]=room->vertexes[n].x;
		px[1]=px[2]=room->vertexes[n2].x;
		py[0]=py[1]=room->max.y-(ag_size_room_high+ag_size_floor_high);
		py[2]=py[3]=room->max.y-ag_size_room_high;
		pz[0]=pz[3]=room->vertexes[n].z;
		pz[1]=pz[2]=room->vertexes[n2].z;
		
		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);
	}

		// floor and ceiling

	for (n=0;n!=room->nvertex;n++) {
		px[n]=room->vertexes[n].x;
		py[n]=room->max.y-ag_size_room_high;
		pz[n]=room->vertexes[n].z;
	}

	ceiling_poly_idx=map_mesh_add_poly(&map,mesh_idx,room->nvertex,px,py,pz,gx,gy,ag_texture_connect);

	for (n=0;n!=room->nvertex;n++) {
		py[n]-=ag_size_floor_high;
	}

	floor_poly_idx=map_mesh_add_poly(&map,mesh_idx,room->nvertex,px,py,pz,gx,gy,ag_texture_connect);
	
		// recalc normals

	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_out,FALSE);

		// add in the hole

	extrude_pnt.x=0;
	extrude_pnt.y=ag_size_floor_high;
	extrude_pnt.z=0;

	map_mesh_poly_punch_hole(&map,mesh_idx,ceiling_poly_idx,NULL,FALSE,normal_mode_in);

	floor_poly_idx--;			// punch deletes original polygon, so we need to move it back
	map_mesh_poly_punch_hole(&map,mesh_idx,floor_poly_idx,&extrude_pnt,FALSE,normal_mode_in);
}

/* =======================================================

      Platform Second Stories
      
======================================================= */

void ag_generate_add_room_second_story_platform(int room_idx)
{
	int				n,n2,mesh_idx,
					v1_idx,v2_idx;
	int				px[8],py[8],pz[8];
	float			gx[8],gy[8];
	bool			horz;
	d3pnt			mid,vs[4];
	ag_room_type	*room;

	room=&ag_state.rooms[room_idx];

		// find line segments that are
		// on opposite ends of room

	mid.x=(room->min.x+room->max.x)>>1;
	mid.z=(room->min.z+room->max.z)>>1;

	v1_idx=v2_idx=-1;
	horz=ag_random_bool();

	for (n=0;n!=room->nvertex;n++) {
		n2=n+1;
		if (n2==room->nvertex) n2=0;

		if (horz) {
			if (v1_idx==-1) {
				if ((room->vertexes[n].z<mid.z) && (room->vertexes[n2].z<mid.z)) v1_idx=n;
			}
			if (v2_idx==-1) {
				if ((room->vertexes[n].z>mid.z) && (room->vertexes[n2].z>mid.z)) v2_idx=n;
			}
		}
		else {
			if (v1_idx==-1) {
				if ((room->vertexes[n].x<mid.x) && (room->vertexes[n2].x<mid.x)) v1_idx=n;
			}
			if (v2_idx==-1) {
				if ((room->vertexes[n].x>mid.x) && (room->vertexes[n2].x>mid.x)) v2_idx=n;
			}
		}
	}

	if ((v1_idx==-1) || (v2_idx==-1)) return;

		// build the polygon

	memmove(&vs[0],&room->vertexes[v1_idx],sizeof(d3pnt));

	n2=v1_idx+1;
	if (n2==4) n2=0;
	memmove(&vs[1],&room->vertexes[n2],sizeof(d3pnt));

	memmove(&vs[2],&room->vertexes[v2_idx],sizeof(d3pnt));

	n2=v2_idx+1;
	if (n2==4) n2=0;
	memmove(&vs[3],&room->vertexes[n2],sizeof(d3pnt));

		// add the mesh

	mesh_idx=map_mesh_add(&map);

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

		// walls

	for (n=0;n!=4;n++) {
		n2=n+1;
		if (n2==4) n2=0;

		px[0]=px[3]=vs[n].x;
		px[1]=px[2]=vs[n2].x;
		py[0]=py[1]=room->max.y-(ag_size_room_high+ag_size_floor_high);
		py[2]=py[3]=room->max.y-ag_size_room_high;
		pz[0]=pz[3]=vs[n].z;
		pz[1]=pz[2]=vs[n2].z;
		
		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);
	}

		// floor and ceiling

	for (n=0;n!=4;n++) {
		px[n]=vs[n].x;
		py[n]=room->max.y-ag_size_room_high;
		pz[n]=vs[n].z;
	}

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

	for (n=0;n!=4;n++) {
		py[n]-=ag_size_floor_high;
	}

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

		// build second stories

	for (n=0;n!=ag_state.nroom;n++) {

			// did this room have
			// a second story

		room=&ag_state.rooms[n];
		if (!room->second_story) continue;
		if (!room->require_top_floor) continue;

			// needs more than 4 vertexes

		if (room->nvertex<=4) continue;

			// hole in the middle take up
			// the entire room

		if (ag_random_bool()) {
			ag_generate_add_room_second_story_hole(n);
		}
		else {
			ag_generate_add_room_second_story_platform(n);
		}
	}
}

