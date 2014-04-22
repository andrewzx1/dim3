/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto Generation Room Connectors

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

      Determine Connector Type
      
======================================================= */

void ag_generate_set_connector_type(int room_idx)
{
	int				dx,dz;
	ag_room_type	*room,*room2;

	room=&ag_state.rooms[room_idx];

		// if both rooms have second story,
		// then connect with pillar

	room2=&ag_state.rooms[room->connect_box.other_room_idx];
	if ((room->second_story) && (room2->second_story)) {
		room->connect_box.connect_type=ag_connect_type_pillar;
		return;
	}

		// check for door width

	dx=room->connect_box.max.x-room->connect_box.min.x;
	dz=room->connect_box.max.z-room->connect_box.min.z;

	if ((dx<=ag_size_door_min_width) && (dz<=ag_size_door_min_width)) {
		room->connect_box.connect_type=ag_connect_type_door;
		return;
	}

		// randomize for any stairs

	if (ag_random_int(100)<33) {
		room->connect_box.connect_type=ag_connect_type_stairs;
		return;
	}

		// normal plain connector

	room->connect_box.connect_type=ag_connect_type_normal;
}

/* =======================================================

      Door Room Connectors
      
======================================================= */

void ag_generate_add_connector_room_normal(int org_room_idx,bool has_door)
{
	int					mesh_idx,dx,dz,
						group_idx,movement_idx,move_idx;
	int					px[8],py[8],pz[8];
	float				gx[8],gy[8];
	movement_type		*movement;
	movement_move_type	*move;
	ag_room_type		*room,*room2;

	room=&ag_state.rooms[org_room_idx];
	room2=&ag_state.rooms[room->connect_box.other_room_idx];

		// remove unused polygons

	ag_generate_remove_polygons_in_box(room->mesh_idx,&room->connect_box.min,&room->connect_box.max);
	ag_generate_remove_polygons_in_box(room2->mesh_idx,&room->connect_box.min,&room->connect_box.max);

		// get dimensions

	dx=room->connect_box.max.x-room->connect_box.min.x;
	dz=room->connect_box.max.z-room->connect_box.min.z;

		// add the mesh

	mesh_idx=map_mesh_add(&map);

		// walls

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

	if (dx>dz) {
		px[0]=px[1]=px[2]=px[3]=room->connect_box.min.x;
		pz[0]=pz[3]=room->connect_box.min.z;
		pz[1]=pz[2]=room->connect_box.max.z;
		py[0]=py[1]=room->connect_box.min.y;
		py[2]=py[3]=room->connect_box.max.y;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,(has_door?ag_texture_connect:ag_texture_wall));

		px[0]=px[1]=px[2]=px[3]=room->connect_box.max.x;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,(has_door?ag_texture_connect:ag_texture_wall));
	}
	else {
		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.min.z;
		py[0]=py[1]=room->connect_box.min.y;
		py[2]=py[3]=room->connect_box.max.y;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,(has_door?ag_texture_connect:ag_texture_wall));

		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.max.z;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,(has_door?ag_texture_connect:ag_texture_wall));
	}

		// floor and ceiling

	px[0]=px[3]=room->connect_box.min.x;
	px[1]=px[2]=room->connect_box.max.x;
	pz[0]=pz[1]=room->connect_box.min.z;
	pz[2]=pz[3]=room->connect_box.max.z;
	py[0]=py[1]=py[2]=py[3]=room->connect_box.max.y;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,(has_door?ag_texture_connect:ag_texture_floor));

	py[0]=py[1]=py[2]=py[3]=room->connect_box.min.y;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,(has_door?ag_texture_connect:ag_texture_ceiling));

	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_in,FALSE);

		// add in a door?

	if (!has_door) return;

		// create door

	mesh_idx=map_mesh_add(&map);

	if (dx>dz) {
		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[3]=pz[1]=pz[2]=room->connect_box.min.z+ag_size_door_margin;
		py[0]=py[1]=room->connect_box.min.y;
		py[2]=py[3]=room->connect_box.max.y;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);

		pz[0]=pz[3]=pz[1]=pz[2]=room->connect_box.max.z-ag_size_door_margin;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);

		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[1]=room->connect_box.min.z+ag_size_door_margin;
		pz[2]=pz[3]=room->connect_box.max.z-ag_size_door_margin;
		py[0]=py[1]=py[2]=py[3]=room->connect_box.max.y;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);
	}
	else {
		px[0]=px[3]=px[1]=px[2]=room->connect_box.min.x+ag_size_door_margin;
		pz[0]=pz[3]=room->connect_box.min.z;
		pz[1]=pz[2]=room->connect_box.max.z;
		py[0]=py[1]=room->connect_box.min.y;
		py[2]=py[3]=room->connect_box.max.y;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);

		px[0]=px[3]=px[1]=px[2]=room->connect_box.max.x-ag_size_door_margin;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);

		px[0]=px[3]=room->connect_box.min.x+ag_size_door_margin;
		px[1]=px[2]=room->connect_box.max.x-ag_size_door_margin;
		pz[0]=pz[1]=room->connect_box.min.z;
		pz[2]=pz[3]=room->connect_box.max.z;
		py[0]=py[1]=py[2]=py[3]=room->connect_box.max.y;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_door);
	}

	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_out,FALSE);

		// add door group

	group_idx=map_group_add(&map);
	sprintf(map.group.groups[group_idx].name,"Door %d",ag_state.current_door_idx);

	map.mesh.meshes[mesh_idx].group_idx=group_idx;
	map.mesh.meshes[mesh_idx].flag.moveable=TRUE;

		// add in door movement

	movement_idx=map_movement_add(&map);

	movement=&map.movement.movements[movement_idx];

	sprintf(movement->name,"Door %d",ag_state.current_door_idx);
	movement->group_idx=group_idx;
	movement->auto_open=TRUE;

	move_idx=map_movement_move_add(&map,movement_idx);

	move=&movement->moves[move_idx];
	move->msec=2500;
	move->mov.y=-((room->connect_box.max.y-room->connect_box.min.y)-ag_size_door_margin);
	strcpy(move->sound_name,"Door");

		// next door

	ag_state.current_door_idx++;
}

/* =======================================================

      Pillar Room Connectors
      
======================================================= */

void ag_generate_add_connector_room_pillar(int org_room_idx)
{
	int					n,mesh_idx,dx,dz;
	int					px[8],py[8],pz[8],
						flip_poly_idx[4];
	float				gx[8],gy[8];
	ag_room_type		*room,*room2;

	room=&ag_state.rooms[org_room_idx];
	room2=&ag_state.rooms[room->connect_box.other_room_idx];

		// pillars go to the top

	room->connect_box.min.y-=(ag_size_room_high+ag_size_floor_high);

		// remove unused polygons

	ag_generate_remove_polygons_in_box(room->mesh_idx,&room->connect_box.min,&room->connect_box.max);
	ag_generate_remove_polygons_in_box(room2->mesh_idx,&room->connect_box.min,&room->connect_box.max);

		// get pillar direction

	dx=room->connect_box.max.x-room->connect_box.min.x;
	dz=room->connect_box.max.z-room->connect_box.min.z;

		// add mesh

	mesh_idx=map_mesh_add(&map);

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

	if (dx>dz) {

			// outside walls

		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.min.x+ag_size_pillar_wid;
		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.min.z;
		py[0]=room->connect_box.min.y;
		py[1]=room->connect_box.min.y+ag_size_pillar_wid;
		py[2]=room->connect_box.max.y-ag_size_bump_high;
		py[3]=room->connect_box.max.y;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.max.z;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[3]=room->connect_box.max.x-ag_size_pillar_wid;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.min.z;
		py[0]=room->connect_box.min.y+ag_size_pillar_wid;
		py[1]=room->connect_box.min.y;
		py[2]=room->connect_box.max.y;
		py[3]=room->connect_box.max.y-ag_size_bump_high;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.max.z;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[1]=px[2]=px[3]=room->connect_box.min.x+ag_size_pillar_wid;
		pz[0]=pz[3]=room->connect_box.min.z;
		pz[1]=pz[2]=room->connect_box.max.z;
		py[0]=py[1]=room->connect_box.min.y+ag_size_pillar_wid;
		py[2]=py[3]=room->connect_box.max.y-ag_size_bump_high;

		flip_poly_idx[0]=map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[1]=px[2]=px[3]=room->connect_box.max.x-ag_size_pillar_wid;

		flip_poly_idx[1]=map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

			// top pillar

		px[0]=room->connect_box.min.x;
		px[1]=room->connect_box.max.x;
		px[2]=room->connect_box.max.x-ag_size_pillar_wid;
		px[3]=room->connect_box.min.x+ag_size_pillar_wid;
		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.min.z;
		py[0]=py[1]=room->connect_box.min.y;
		py[2]=py[3]=room->connect_box.min.y+ag_size_pillar_wid;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.max.z;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

			// bottom bump

		px[0]=room->connect_box.min.x+ag_size_pillar_wid;
		px[1]=room->connect_box.max.x-ag_size_pillar_wid;
		px[2]=room->connect_box.max.x;
		px[3]=room->connect_box.min.x;
		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.min.z;
		py[0]=py[1]=room->connect_box.max.y-ag_size_bump_high;
		py[2]=py[3]=room->connect_box.max.y;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.max.z;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

			// ceiling and floor

		px[0]=px[3]=room->connect_box.min.x+ag_size_pillar_wid;
		px[1]=px[2]=room->connect_box.max.x-ag_size_pillar_wid;
		pz[0]=pz[1]=room->connect_box.min.z;
		pz[2]=pz[3]=room->connect_box.max.z;
		py[0]=py[1]=py[2]=py[3]=room->connect_box.max.y-ag_size_bump_high;

		flip_poly_idx[2]=map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		py[0]=py[1]=py[2]=py[3]=room->connect_box.min.y+ag_size_pillar_wid;

		flip_poly_idx[3]=map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);
	}
	else {

			// outside walls

		px[0]=px[1]=px[2]=px[3]=room->connect_box.min.x;
		pz[0]=pz[3]=room->connect_box.min.z;
		pz[1]=pz[2]=room->connect_box.min.z+ag_size_pillar_wid;
		py[0]=room->connect_box.min.y;
		py[1]=room->connect_box.min.y+ag_size_pillar_wid;
		py[2]=room->connect_box.max.y-ag_size_bump_high;
		py[3]=room->connect_box.max.y;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[1]=px[2]=px[3]=room->connect_box.max.x;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[1]=px[2]=px[3]=room->connect_box.min.x;
		pz[0]=pz[3]=room->connect_box.max.z-ag_size_pillar_wid;
		pz[1]=pz[2]=room->connect_box.max.z;
		py[0]=room->connect_box.min.y+ag_size_pillar_wid;
		py[1]=room->connect_box.min.y;
		py[2]=room->connect_box.max.y;
		py[3]=room->connect_box.max.y-ag_size_bump_high;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[1]=px[2]=px[3]=room->connect_box.max.x;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.min.z+ag_size_pillar_wid;
		py[0]=py[1]=room->connect_box.min.y+ag_size_pillar_wid;
		py[2]=py[3]=room->connect_box.max.y-ag_size_bump_high;

		flip_poly_idx[0]=map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.max.z-ag_size_pillar_wid;

		flip_poly_idx[1]=map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

			// top pillar

		px[0]=px[1]=px[2]=px[3]=room->connect_box.min.x;
		pz[0]=room->connect_box.min.z;
		pz[1]=room->connect_box.max.z;
		pz[2]=room->connect_box.max.z-ag_size_pillar_wid;
		pz[3]=room->connect_box.min.z+ag_size_pillar_wid;
		py[0]=py[1]=room->connect_box.min.y;
		py[2]=py[3]=room->connect_box.min.y+ag_size_pillar_wid;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[1]=px[2]=px[3]=room->connect_box.max.x;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

			// bottom bump

		px[0]=px[1]=px[2]=px[3]=room->connect_box.min.x;
		pz[0]=room->connect_box.min.z+ag_size_pillar_wid;
		pz[1]=room->connect_box.max.z-ag_size_pillar_wid;
		pz[2]=room->connect_box.max.z;
		pz[3]=room->connect_box.min.z;
		py[0]=py[1]=room->connect_box.max.y-ag_size_bump_high;
		py[2]=py[3]=room->connect_box.max.y;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[1]=px[2]=px[3]=room->connect_box.max.x;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

			// ceiling and floor

		px[0]=px[1]=room->connect_box.min.x;
		px[2]=px[3]=room->connect_box.max.x;
		pz[0]=pz[3]=room->connect_box.min.z+ag_size_pillar_wid;
		pz[1]=pz[2]=room->connect_box.max.z-ag_size_pillar_wid;
		py[0]=py[1]=py[2]=py[3]=room->connect_box.max.y-ag_size_bump_high;

		flip_poly_idx[2]=map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		py[0]=py[1]=py[2]=py[3]=room->connect_box.min.y+ag_size_pillar_wid;

		flip_poly_idx[3]=map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);
	}

		// recalc normals

	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_out,FALSE);

		// because of the inside-out nature,
		// we need to flip some normals

	for (n=0;n!=4;n++) {
		map.mesh.meshes[mesh_idx].polys[flip_poly_idx[n]].tangent_space.normal.x=-map.mesh.meshes[mesh_idx].polys[flip_poly_idx[n]].tangent_space.normal.x;
		map.mesh.meshes[mesh_idx].polys[flip_poly_idx[n]].tangent_space.normal.y=-map.mesh.meshes[mesh_idx].polys[flip_poly_idx[n]].tangent_space.normal.y;
		map.mesh.meshes[mesh_idx].polys[flip_poly_idx[n]].tangent_space.normal.z=-map.mesh.meshes[mesh_idx].polys[flip_poly_idx[n]].tangent_space.normal.z;
	}
}

/* =======================================================

      Stair Room Connectors
      
======================================================= */

void ag_generate_add_connector_room_step(d3pnt *min,d3pnt *max)
{
	int				mesh_idx;
	int				px[8],py[8],pz[8];
	float			gx[8],gy[8];

		// step mesh

	mesh_idx=map_mesh_add(&map);

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

		// sides

	px[0]=px[1]=px[2]=px[3]=min->x;
	pz[0]=pz[3]=min->z;
	pz[1]=pz[2]=max->z;
	py[0]=py[1]=min->y;
	py[2]=py[3]=max->y;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_floor);

	px[0]=px[1]=px[2]=px[3]=max->x;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_floor);

	px[0]=px[3]=min->x;
	px[1]=px[2]=max->x;
	pz[0]=pz[1]=pz[2]=pz[3]=min->z;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_floor);

	pz[0]=pz[1]=pz[2]=pz[3]=max->z;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_floor);

		// top

	px[0]=px[3]=min->x;
	px[1]=px[2]=max->x;
	pz[0]=pz[1]=min->z;
	pz[2]=pz[3]=max->z;
	py[0]=py[1]=py[2]=py[3]=min->y;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_floor);

		// reset normals

	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_out,FALSE);
}

void ag_generate_add_connector_room_stairs(int org_room_idx)
{
	int					n,mesh_idx,
						stair_type,stair_len,step_len,step_high,
						step_x_add,step_z_add,sz;
	int					px[8],py[8],pz[8];
	float				gx[8],gy[8];
	d3pnt				min,max,step_min,step_max;
	ag_room_type		*room,*room2;

	room=&ag_state.rooms[org_room_idx];
	room2=&ag_state.rooms[room->connect_box.other_room_idx];

		// remove unused polygons
		// since they are on different levels, we
		// need to remove only correct Ys

	memmove(&min,&room->connect_box.min,sizeof(d3pnt));
	memmove(&max,&room->connect_box.max,sizeof(d3pnt));
	ag_generate_remove_polygons_in_box(room2->mesh_idx,&min,&max);

	min.y+=ag_size_room_high;
	max.y+=ag_size_room_high;
	ag_generate_remove_polygons_in_box(room->mesh_idx,&min,&max);

		// outside walls/floor/ceiling

	mesh_idx=map_mesh_add(&map);

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

	if ((room->connect_box.stair_dir==ag_stair_dir_top) || (room->connect_box.stair_dir==ag_stair_dir_bottom)) {

			// walls

		px[0]=px[1]=px[2]=px[3]=room->connect_box.min.x;
		pz[0]=pz[3]=room->connect_box.min.z;
		pz[1]=pz[2]=room->connect_box.max.z;
		py[0]=py[1]=room->connect_box.min.y;
		py[2]=py[3]=room->connect_box.max.y+ag_size_room_high;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[1]=px[2]=px[3]=room->connect_box.max.x;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.max.x;
		py[0]=py[1]=room->connect_box.max.y;
		py[2]=py[3]=room->connect_box.max.y+ag_size_room_high;

		pz[0]=pz[1]=pz[2]=pz[3]=((room->connect_box.stair_dir==ag_stair_dir_top)?room->connect_box.max.z:room->connect_box.min.z);

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

			// ceilings

		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[1]=room->connect_box.min.z;
		pz[2]=pz[3]=room->connect_box.max.z;

		if (room->connect_box.stair_dir==ag_stair_dir_top) {
			py[0]=py[1]=room->connect_box.min.y+ag_size_room_high;
			py[2]=py[3]=room->connect_box.min.y;
		}
		else {
			py[0]=py[1]=room->connect_box.min.y;
			py[2]=py[3]=room->connect_box.min.y+ag_size_room_high;
		}

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);
	}
	else {
			// walls

		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.min.z;
		py[0]=py[1]=room->connect_box.min.y;
		py[2]=py[3]=room->connect_box.max.y+ag_size_room_high;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		pz[0]=pz[1]=pz[2]=pz[3]=room->connect_box.max.z;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

		py[0]=py[1]=room->connect_box.max.y;
		py[2]=py[3]=room->connect_box.max.y+ag_size_room_high;
		pz[0]=pz[3]=room->connect_box.min.z;
		pz[1]=pz[2]=room->connect_box.max.z;

		px[0]=px[1]=px[2]=px[3]=((room->connect_box.stair_dir==ag_stair_dir_left)?room->connect_box.max.x:room->connect_box.min.x);

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

			// ceilings

		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[1]=room->connect_box.min.z;
		pz[2]=pz[3]=room->connect_box.max.z;

		if (room->connect_box.stair_dir==ag_stair_dir_left) {
			py[0]=py[3]=room->connect_box.min.y+ag_size_room_high;
			py[1]=py[2]=room->connect_box.min.y;
		}
		else {
			py[0]=py[3]=room->connect_box.min.y;
			py[1]=py[2]=room->connect_box.min.y+ag_size_room_high;
		}

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

	}

		// floor

	px[0]=px[3]=room->connect_box.min.x;
	px[1]=px[2]=room->connect_box.max.x;
	py[0]=py[2]=py[1]=py[3]=room->connect_box.max.y+ag_size_room_high;
	pz[0]=pz[1]=room->connect_box.min.z;
	pz[2]=pz[3]=room->connect_box.max.z;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_connect);

	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_in,FALSE);

		// get stair type

	stair_type=ag_random_int(3);

		// stairs going across the Z

	if ((room->connect_box.stair_dir==ag_stair_dir_top) || (room->connect_box.stair_dir==ag_stair_dir_bottom)) {

		stair_len=room->connect_box.max.z-room->connect_box.min.z;
		step_len=stair_len/ag_size_stair_count;
		step_high=(room->connect_box.max.y-room->connect_box.min.y)/ag_size_stair_count;

			// steps

		if (room->connect_box.stair_dir==ag_stair_dir_top) {
			step_min.z=room->connect_box.min.z;
			step_max.z=room->connect_box.min.z+step_len;
			step_z_add=step_len;
		}
		else {
			step_min.z=room->connect_box.max.z-step_len;
			step_max.z=room->connect_box.max.z;
			step_z_add=-step_len;
		}

		switch (stair_type) {
			
			case ag_stair_type_one_side:
				step_min.x=room->connect_box.min.x;
				step_max.x=room->connect_box.min.x+((room->connect_box.max.x-room->connect_box.min.x)>>1);
				break;

			case ag_stair_type_middle:
				sz=(room->connect_box.max.x-room->connect_box.min.x)>>2;
				step_min.x=room->connect_box.min.x+sz;
				step_max.x=room->connect_box.max.x-sz;
				break;

			case ag_stair_type_full:
				step_min.x=room->connect_box.min.x;
				step_max.x=room->connect_box.max.x;
				break;

		}

		step_min.y=(room->connect_box.max.y+ag_size_room_high)-step_high;
		step_max.y=(room->connect_box.max.y+ag_size_room_high);

		for (n=0;n!=ag_size_stair_count;n++) {
			ag_generate_add_connector_room_step(&step_min,&step_max);
			step_min.y-=step_high;
			step_min.z+=step_z_add;
			step_max.z+=step_z_add;
		}
	}

		// stairs going across the X

	else {
		stair_len=room->connect_box.max.x-room->connect_box.min.x;
		step_len=stair_len/ag_size_stair_count;
		step_high=(room->connect_box.max.y-room->connect_box.min.y)/ag_size_stair_count;

			// steps

		if (room->connect_box.stair_dir==ag_stair_dir_left) {
			step_min.x=room->connect_box.min.x;
			step_max.x=room->connect_box.min.x+step_len;
			step_x_add=step_len;
		}
		else {
			step_min.x=room->connect_box.max.x-step_len;
			step_max.x=room->connect_box.max.x;
			step_x_add=-step_len;
		}

		switch (stair_type) {
			
			case ag_stair_type_one_side:
				step_min.z=room->connect_box.min.z;
				step_max.z=room->connect_box.min.z+((room->connect_box.max.z-room->connect_box.min.z)>>1);
				break;

			case ag_stair_type_middle:
				sz=(room->connect_box.max.z-room->connect_box.min.z)>>2;
				step_min.z=room->connect_box.min.z+sz;
				step_max.z=room->connect_box.max.z-sz;
				break;

			case ag_stair_type_full:
				step_min.z=room->connect_box.min.z;
				step_max.z=room->connect_box.max.z;
				break;

		}

		step_min.y=(room->connect_box.max.y+ag_size_room_high)-step_high;
		step_max.y=(room->connect_box.max.y+ag_size_room_high);

		for (n=0;n!=ag_size_stair_count;n++) {
			ag_generate_add_connector_room_step(&step_min,&step_max);
			step_min.y-=step_high;
			step_min.x+=step_x_add;
			step_max.x+=step_x_add;
		}
	}
}

/* =======================================================

      Add Room Connectors
      
======================================================= */

void ag_generate_add_connector_rooms(void)
{
	int				n;
	ag_room_type	*room;

	for (n=0;n!=ag_state.nroom;n++) {

			// did this room have a connection box?

		room=&ag_state.rooms[n];
		if (!room->connect_box.on) continue;

		switch (room->connect_box.connect_type) {

			case ag_connect_type_normal:
				ag_generate_add_connector_room_normal(n,FALSE);
				break;

			case ag_connect_type_door:
				ag_generate_add_connector_room_normal(n,TRUE);
				break;

			case ag_connect_type_pillar:
				ag_generate_add_connector_room_pillar(n);
				break;

			case ag_connect_type_stairs:
				ag_generate_add_connector_room_stairs(n);
				break;

		}
	}
}

