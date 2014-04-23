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

      Add Room Second Story Pieces
      
======================================================= */

void ag_generate_add_room_second_story(void)
{
	int				n,k,k2,mesh_idx,
					floor_poly_idx,ceiling_poly_idx,
					extrude_poly_idx;
	int				px[8],py[8],pz[8];
	float			gx[8],gy[8];
	d3pnt			extrude_pnt;
	ag_room_type	*room;

		// setup extruding point
		// for second stories with
		// holes punched in them

	extrude_pnt.x=0;
	extrude_pnt.y=ag_size_floor_high;
	extrude_pnt.z=0;

		// build second stories

	for (n=0;n!=ag_state.nroom;n++) {

			// did this room have
			// a second story

		room=&ag_state.rooms[n];
		if (!room->second_story) continue;
		if (!room->require_top_floor) continue;

			// needs more than 4 vertexes

		if (room->nvertex<=4) continue;

			// add the mesh

		mesh_idx=map_mesh_add(&map);

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;

			// walls

		for (k=0;k!=4;k++) {
			k2=k+1;
			if (k2==4) k2=0;

			px[0]=px[3]=room->vertexes[k].x;
			px[1]=px[2]=room->vertexes[k2].x;
			py[0]=py[1]=room->max.y-(ag_size_room_high+ag_size_floor_high);
			py[2]=py[3]=room->max.y-ag_size_room_high;
			pz[0]=pz[3]=room->vertexes[k].z;
			pz[1]=pz[2]=room->vertexes[k2].z;
			
			map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);
		}

			// floor and ceiling

		for (k=0;k!=4;k++) {
			px[k]=room->vertexes[k].x;
			py[k]=room->max.y-ag_size_room_high;
			pz[k]=room->vertexes[k].z;
		}

		ceiling_poly_idx=map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		for (k=0;k!=4;k++) {
			py[k]-=ag_size_floor_high;
		}

		floor_poly_idx=map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);
		
			// recalc normals

		map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_out,FALSE);

			// add in the hole

		if (ag_random_bool()) {
			
			map_mesh_poly_punch_hole(&map,mesh_idx,ceiling_poly_idx,NULL);

			floor_poly_idx--;			// punch deletes original polygon, so we need to move it back
			map_mesh_poly_punch_hole(&map,mesh_idx,floor_poly_idx,&extrude_pnt);

			extrude_poly_idx=map.mesh.meshes[mesh_idx].npoly-4;		// the last 4 polygons will be the extruded ones

			for (k=extrude_poly_idx;k!=map.mesh.meshes[mesh_idx].npoly;k++) {
				map_recalc_normals_mesh_poly(&map,&map.mesh.meshes[mesh_idx],k,normal_mode_in,FALSE);
			}
		}
	}
}

