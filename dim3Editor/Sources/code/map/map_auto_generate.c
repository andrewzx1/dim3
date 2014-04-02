/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto Generation Main Line

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

ag_state_type					ag_state;

extern bool ag_initialize(char *err_str);
extern void ag_release(void);
extern bool ag_check_required_textures(char *err_str);
extern void ag_random_seed(void);
extern void ag_random_previous_seed(void);
extern void ag_random_next_seed(void);
extern int ag_random_int(int max);
extern bool ag_random_bool(void);
extern void ag_generate_delete_shared_polygons(void);
extern void ag_generate_decoration_box_stack(void);
extern void ag_generate_spots_add(void);

/* =======================================================

      Auto Generate Utilities
      
======================================================= */

void ag_map_clear(void)
{
		// clear all selections

	select_clear();

		// clear static memory

	map.nspot=0;
	map.nnode=0;
	map.nscenery=0;
	map.nlight=0;
	map.nsound=0;
	map.nparticle=0;

		// meshes and liquids

	while (map.mesh.nmesh!=0) {
		map_mesh_delete(&map,0);
	}

	while (map.liquid.nliquid!=0) {
		map_liquid_delete(&map,0);
	}

		// groups, movements, cinemas

	while (map.group.ngroup!=0) {
		map_group_delete(&map,0);
	}

	while (map.movement.nmovement!=0) {
		map_movement_delete(&map,0);
	}

	while (map.cinema.ncinema!=0) {
		map_cinema_delete(&map,0);
	}
}

/* =======================================================

      Flatten Sides of Rooms
      
======================================================= */

void ag_add_room_flatten_top(ag_room_type *room)
{
	int				n,top_idx,other_idx,
					prev_idx,next_idx;

		// find top z vertex

	top_idx=0;

	for (n=1;n!=room->nvertex;n++) {
		if (room->vertexes[n].z<room->vertexes[top_idx].z) top_idx=n;
	}

		// find closest top z
		// partner index

	prev_idx=top_idx-1;
	if (prev_idx==-1) prev_idx=room->nvertex-1;

	next_idx=top_idx+1;
	if (next_idx==room->nvertex) next_idx=0;

		// flatten top

	other_idx=next_idx;
	if (room->vertexes[prev_idx].z<room->vertexes[next_idx].z) other_idx=prev_idx;

	room->vertexes[other_idx].z=room->vertexes[top_idx].z;

	if (room->vertexes[other_idx].x<room->vertexes[top_idx].x) {
		room->flat.top.p1_idx=other_idx;
		room->flat.top.p2_idx=top_idx;
	}
	else {
		room->flat.top.p1_idx=top_idx;
		room->flat.top.p2_idx=other_idx;
	}

	room->flat.top.on=TRUE;
}

void ag_add_room_flatten_bottom(ag_room_type *room)
{
	int				n,bot_idx,other_idx,
					prev_idx,next_idx;

		// find bottom z vertex

	bot_idx=0;

	for (n=1;n!=room->nvertex;n++) {
		if (room->vertexes[n].z>room->vertexes[bot_idx].z) bot_idx=n;
	}

		// find closest bottom z
		// partner index

	prev_idx=bot_idx-1;
	if (prev_idx==-1) prev_idx=room->nvertex-1;

	next_idx=bot_idx+1;
	if (next_idx==room->nvertex) next_idx=0;

		// flatten bottom

	other_idx=next_idx;
	if (room->vertexes[prev_idx].z>room->vertexes[next_idx].z) other_idx=prev_idx;

	room->vertexes[other_idx].z=room->vertexes[bot_idx].z;

	if (room->vertexes[other_idx].x<room->vertexes[bot_idx].x) {
		room->flat.bot.p1_idx=other_idx;
		room->flat.bot.p2_idx=bot_idx;
	}
	else {
		room->flat.bot.p1_idx=bot_idx;
		room->flat.bot.p2_idx=other_idx;
	}

	room->flat.bot.on=TRUE;
}

void ag_add_room_flatten_left(ag_room_type *room)
{
	int				n,lft_idx,other_idx,
					prev_idx,next_idx;

		// find left x vertex

	lft_idx=0;

	for (n=1;n!=room->nvertex;n++) {
		if (room->vertexes[n].x<room->vertexes[lft_idx].x) lft_idx=n;
	}

		// find closest left x
		// partner index

	prev_idx=lft_idx-1;
	if (prev_idx==-1) prev_idx=room->nvertex-1;

	next_idx=lft_idx+1;
	if (next_idx==room->nvertex) next_idx=0;

		// flatten left

	other_idx=next_idx;
	if (room->vertexes[prev_idx].x<room->vertexes[next_idx].x) other_idx=prev_idx;

	room->vertexes[other_idx].x=room->vertexes[lft_idx].x;

	if (room->vertexes[other_idx].z<room->vertexes[lft_idx].z) {
		room->flat.lft.p1_idx=other_idx;
		room->flat.lft.p2_idx=lft_idx;
	}
	else {
		room->flat.lft.p1_idx=lft_idx;
		room->flat.lft.p2_idx=other_idx;
	}

	room->flat.lft.on=TRUE;
}

void ag_add_room_flatten_right(ag_room_type *room)
{
	int				n,rgt_idx,other_idx,
					prev_idx,next_idx;

		// find right x vertex

	rgt_idx=0;

	for (n=1;n!=room->nvertex;n++) {
		if (room->vertexes[n].x>room->vertexes[rgt_idx].x) rgt_idx=n;
	}

		// find closest right x
		// partner index

	prev_idx=rgt_idx-1;
	if (prev_idx==-1) prev_idx=room->nvertex-1;

	next_idx=rgt_idx+1;
	if (next_idx==room->nvertex) next_idx=0;

		// flatten right

	other_idx=next_idx;
	if (room->vertexes[prev_idx].x>room->vertexes[next_idx].x) other_idx=prev_idx;

	room->vertexes[other_idx].x=room->vertexes[rgt_idx].x;

	if (room->vertexes[other_idx].z<room->vertexes[rgt_idx].z) {
		room->flat.rgt.p1_idx=other_idx;
		room->flat.rgt.p2_idx=rgt_idx;
	}
	else {
		room->flat.rgt.p1_idx=rgt_idx;
		room->flat.rgt.p2_idx=other_idx;
	}

	room->flat.rgt.on=TRUE;
}

/* =======================================================

      Room Utilities
      
======================================================= */

void ag_add_room_get_point_from_angle(d3pnt *center_pnt,float radius,float ang,d3pnt *pnt)
{
	float			rad;

	rad=ang*ANG_to_RAD;
	pnt->x=center_pnt->x+(int)(radius*sinf(rad));
	pnt->z=center_pnt->z-(int)(radius*cosf(rad));
}

/* =======================================================

      Create a Room
      
======================================================= */

void ag_add_room(bool first_room)
{
	int				n,k,poly_sz,mesh_idx,
					connect_mesh_idx,org_connect_mesh_idx,connect_side,org_connect_side,
					nvertex,random_ang,box_offset;
	int				px[8],py[8],pz[8];
	float			last_ang,next_ang,gx[8],gy[8];
	bool			hit,flat;
	d3pnt			pnt,p1,p2,min,max,mesh_min,mesh_max;
	ag_room_type	*room,*connect_room;

		// radium size

	poly_sz=ag_size_room_width_start+ag_random_int(ag_size_room_width_extra);

		// if first room, in center of map

	if (first_room) {
		pnt.x=pnt.z=map_max_size>>1;
		
		min.x=pnt.x-poly_sz;
		max.x=pnt.x+poly_sz;

		min.z=pnt.z-poly_sz;
		max.z=pnt.z+poly_sz;

		connect_mesh_idx=-1;
		connect_side=-1;
	}

		// otherwise connect it to a side
		// of an existing room

		// we start at a random room and
		// random direction and search forward
		// to find the first empty spot

	else {

		org_connect_mesh_idx=connect_mesh_idx=ag_random_int(map.mesh.nmesh);
		org_connect_side=connect_side=ag_random_int(4);

		while (TRUE) {

			pnt.x=(ag_state.rooms[connect_mesh_idx].min.x+ag_state.rooms[connect_mesh_idx].max.x)>>1;
			pnt.z=(ag_state.rooms[connect_mesh_idx].min.z+ag_state.rooms[connect_mesh_idx].max.z)>>1;

			hit=FALSE;

			switch (connect_side) {
				case ag_connect_side_top:
					if (ag_state.rooms[connect_mesh_idx].flat.top.on) {
						min.x=pnt.x-poly_sz;
						max.x=pnt.x+poly_sz;
						min.z=ag_state.rooms[connect_mesh_idx].min.z-(poly_sz<<1);
						max.z=ag_state.rooms[connect_mesh_idx].min.z;
						hit=TRUE;
					}
					break;
				case ag_connect_side_bottom:
					if (ag_state.rooms[connect_mesh_idx].flat.bot.on) {
						min.x=pnt.x-poly_sz;
						max.x=pnt.x+poly_sz;
						min.z=ag_state.rooms[connect_mesh_idx].max.z;
						max.z=ag_state.rooms[connect_mesh_idx].max.z+(poly_sz<<1);
						hit=TRUE;
					}
					break;
				case ag_connect_side_left:
					if (ag_state.rooms[connect_mesh_idx].flat.lft.on) {
						min.x=ag_state.rooms[connect_mesh_idx].min.x-(poly_sz<<1);
						max.x=ag_state.rooms[connect_mesh_idx].min.x;
						min.z=pnt.z-poly_sz;
						max.z=pnt.z+poly_sz;
						hit=TRUE;
					}
					break;
				case ag_connect_side_right:
					if (ag_state.rooms[connect_mesh_idx].flat.rgt.on) {
						min.x=ag_state.rooms[connect_mesh_idx].max.x;
						max.x=ag_state.rooms[connect_mesh_idx].max.x+(poly_sz<<1);
						min.z=pnt.z-poly_sz;
						max.z=pnt.z+poly_sz;
						hit=TRUE;
					}
					break;
			}

				// legal spot?
				// don't need to check if first room

			if (hit) {

				for (n=0;n!=map.mesh.nmesh;n++) {
					map_mesh_calculate_extent(&map,n,&mesh_min,&mesh_max);

					if (max.x<=mesh_min.x) continue;
					if (min.x>=mesh_max.x) continue;
					if (max.z<=mesh_min.z) continue;
					if (min.z>=mesh_max.z) continue;

					hit=FALSE;
					break;
				}
			}

				// got a hit, we can use this spot

			if (hit) break;

				// if no hit, then move on to the
				// next side or mesh

			if (!hit) {
				connect_side++;
				if (connect_side>=4) connect_side=0;

				if (connect_side==org_connect_side) {
					connect_mesh_idx++;
					if (connect_mesh_idx>=map.mesh.nmesh) connect_mesh_idx=0;

						// no where to place room, exit out

					if (connect_mesh_idx==org_connect_mesh_idx) return;
				}
			}
		}
	}

		// create mesh

	mesh_idx=map_mesh_add(&map);

		// parallel room structure
		// used to track changes

	room=&ag_state.rooms[mesh_idx];

	memmove(&room->min,&min,sizeof(d3pnt));
	memmove(&room->max,&max,sizeof(d3pnt));

	room->flat.top.on=FALSE;
	room->flat.bot.on=FALSE;
	room->flat.lft.on=FALSE;
	room->flat.rgt.on=FALSE;

	room->connect_box.on=FALSE;

	room->second_story=ag_random_bool();

		// create random polygon

	nvertex=4+random_int(4);
	random_ang=(int)(360.0f/(float)nvertex);

	room->nvertex=0;

	last_ang=0.0f;

	pnt.x=(min.x+max.x)>>1;
	pnt.z=(min.z+max.z)>>1;

	for (n=0;n!=nvertex;n++) {

			// get next angle

		if (n==(nvertex-1)) {
			next_ang=0.0f;
		}
		else {
			next_ang=last_ang+(30.0f+(float)ag_random_int(random_ang));
			if (next_ang>360.0f) next_ang=0.0f;
		}

		ag_add_room_get_point_from_angle(&pnt,(float)poly_sz,last_ang,&p1);
		ag_add_room_get_point_from_angle(&pnt,(float)poly_sz,next_ang,&p2);

		last_ang=next_ang;

			// polygon points

		room->vertexes[room->nvertex].x=p1.x;
		room->vertexes[room->nvertex].y=ag_map_bottom_y;
		room->vertexes[room->nvertex].z=p1.z;

		room->nvertex++;

			// exit early if we reconnect early

		if (last_ang==0.0f) break;
	}

		// flatten top/bottom or
		// left/right randomly, and
		// then possible either of the other
		// so there's always at least one pass-through

	if (ag_random_bool()) {
		ag_add_room_flatten_top(room);
		ag_add_room_flatten_bottom(room);

		if (ag_random_bool()) ag_add_room_flatten_left(room);
		if (ag_random_bool()) ag_add_room_flatten_right(room);
	}
	else {
		ag_add_room_flatten_left(room);
		ag_add_room_flatten_right(room);

		if (ag_random_bool()) ag_add_room_flatten_top(room);
		if (ag_random_bool()) ag_add_room_flatten_bottom(room);
	}

		// if connecting, make sure to flatten
		// the opposite side if not already done

	if ((connect_side==ag_connect_side_bottom) && (!room->flat.top.on)) ag_add_room_flatten_top(room);
	if ((connect_side==ag_connect_side_top) && (!room->flat.bot.on)) ag_add_room_flatten_bottom(room);
	if ((connect_side==ag_connect_side_right) && (!room->flat.lft.on)) ag_add_room_flatten_left(room);
	if ((connect_side==ag_connect_side_left) && (!room->flat.rgt.on)) ag_add_room_flatten_right(room);

		// if connecting, then fix
		// the vertexes to line up

		// if the lines are still flat (they can get
		// altered by other hookups) then insert
		// a box

	if (connect_mesh_idx!=-1) {

		flat=FALSE;
		box_offset=0;

		connect_room=&ag_state.rooms[connect_mesh_idx];

		switch (connect_side) {

			case ag_connect_side_top:
				flat=(connect_room->vertexes[connect_room->flat.top.p1_idx].z==connect_room->vertexes[connect_room->flat.top.p2_idx].z);
				if (flat) {
					box_offset=2000+ag_random_int(5000);
					room->connect_box.on=TRUE;
					room->connect_box.min.x=connect_room->vertexes[connect_room->flat.top.p1_idx].x;
					room->connect_box.max.x=connect_room->vertexes[connect_room->flat.top.p2_idx].x;
					room->connect_box.min.z=connect_room->vertexes[connect_room->flat.top.p1_idx].z-box_offset;
					room->connect_box.max.z=connect_room->vertexes[connect_room->flat.top.p1_idx].z;
				}

				room->vertexes[room->flat.bot.p1_idx].x=connect_room->vertexes[connect_room->flat.top.p1_idx].x;
				room->vertexes[room->flat.bot.p1_idx].z=connect_room->vertexes[connect_room->flat.top.p1_idx].z-box_offset;
				room->vertexes[room->flat.bot.p2_idx].x=connect_room->vertexes[connect_room->flat.top.p2_idx].x;
				room->vertexes[room->flat.bot.p2_idx].z=connect_room->vertexes[connect_room->flat.top.p2_idx].z-box_offset;
				break;

			case ag_connect_side_bottom:
				flat=(connect_room->vertexes[connect_room->flat.bot.p1_idx].z==connect_room->vertexes[connect_room->flat.bot.p2_idx].z);
				if (flat) {
					box_offset=2000+ag_random_int(5000);
					room->connect_box.on=TRUE;
					room->connect_box.min.x=connect_room->vertexes[connect_room->flat.bot.p1_idx].x;
					room->connect_box.max.x=connect_room->vertexes[connect_room->flat.bot.p2_idx].x;
					room->connect_box.min.z=connect_room->vertexes[connect_room->flat.bot.p1_idx].z;
					room->connect_box.max.z=connect_room->vertexes[connect_room->flat.bot.p1_idx].z+box_offset;
				}

				room->vertexes[room->flat.top.p1_idx].x=connect_room->vertexes[connect_room->flat.bot.p1_idx].x;
				room->vertexes[room->flat.top.p1_idx].z=connect_room->vertexes[connect_room->flat.bot.p1_idx].z+box_offset;
				room->vertexes[room->flat.top.p2_idx].x=connect_room->vertexes[connect_room->flat.bot.p2_idx].x;
				room->vertexes[room->flat.top.p2_idx].z=connect_room->vertexes[connect_room->flat.bot.p2_idx].z+box_offset;
				break;

			case ag_connect_side_left:
				flat=(connect_room->vertexes[connect_room->flat.lft.p1_idx].x==connect_room->vertexes[connect_room->flat.lft.p2_idx].x);
				if (flat) {
					box_offset=2000+ag_random_int(5000);
					room->connect_box.on=TRUE;
					room->connect_box.min.x=connect_room->vertexes[connect_room->flat.lft.p1_idx].x-box_offset;
					room->connect_box.max.x=connect_room->vertexes[connect_room->flat.lft.p1_idx].x;
					room->connect_box.min.z=connect_room->vertexes[connect_room->flat.lft.p1_idx].z;
					room->connect_box.max.z=connect_room->vertexes[connect_room->flat.lft.p2_idx].z;
				}

				room->vertexes[room->flat.rgt.p1_idx].x=connect_room->vertexes[connect_room->flat.lft.p1_idx].x-box_offset;
				room->vertexes[room->flat.rgt.p1_idx].z=connect_room->vertexes[connect_room->flat.lft.p1_idx].z;
				room->vertexes[room->flat.rgt.p2_idx].x=connect_room->vertexes[connect_room->flat.lft.p2_idx].x-box_offset;
				room->vertexes[room->flat.rgt.p2_idx].z=connect_room->vertexes[connect_room->flat.lft.p2_idx].z;
				break;

			case ag_connect_side_right:
				flat=(connect_room->vertexes[connect_room->flat.rgt.p1_idx].x==connect_room->vertexes[connect_room->flat.rgt.p2_idx].x);
				if (flat) {
					box_offset=2000+ag_random_int(5000);
					room->connect_box.on=TRUE;
					room->connect_box.min.x=connect_room->vertexes[connect_room->flat.rgt.p1_idx].x;
					room->connect_box.max.x=connect_room->vertexes[connect_room->flat.rgt.p1_idx].x+box_offset;
					room->connect_box.min.z=connect_room->vertexes[connect_room->flat.rgt.p1_idx].z;
					room->connect_box.max.z=connect_room->vertexes[connect_room->flat.rgt.p2_idx].z;
				}

				room->vertexes[room->flat.lft.p1_idx].x=connect_room->vertexes[connect_room->flat.rgt.p1_idx].x+box_offset;
				room->vertexes[room->flat.lft.p1_idx].z=connect_room->vertexes[connect_room->flat.rgt.p1_idx].z;
				room->vertexes[room->flat.lft.p2_idx].x=connect_room->vertexes[connect_room->flat.rgt.p2_idx].x+box_offset;
				room->vertexes[room->flat.lft.p2_idx].z=connect_room->vertexes[connect_room->flat.rgt.p2_idx].z;
				break;

		}

		if (flat) {
			room->connect_box.min.y=ag_map_bottom_y-ag_size_room_high;
			room->connect_box.max.y=ag_map_bottom_y;
		}

	}

		// add the walls and short
		// ceiling/second-story walls

	for (n=0;n!=room->nvertex;n++) {

		k=n+1;
		if (k==room->nvertex) k=0;

		px[0]=px[3]=room->vertexes[n].x;
		px[1]=px[2]=room->vertexes[k].x;
		pz[0]=pz[3]=room->vertexes[n].z;
		pz[1]=pz[2]=room->vertexes[k].z;
		py[0]=py[1]=ag_map_bottom_y-ag_size_room_high;
		py[2]=py[3]=ag_map_bottom_y;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_wall);

		py[0]=py[1]=ag_map_bottom_y-(ag_size_room_high+ag_size_floor_high);
		py[2]=py[3]=ag_map_bottom_y-ag_size_room_high;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_wall);

		if (room->second_story) {
			py[0]=py[1]=ag_map_bottom_y-(ag_size_room_high+ag_size_floor_high+ag_size_room_high);
			py[2]=py[3]=ag_map_bottom_y-(ag_size_room_high+ag_size_floor_high);

			map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_wall);
		}
	}

		// add the floor

	for (n=0;n!=room->nvertex;n++) {
		px[n]=room->vertexes[n].x;
		py[n]=ag_map_bottom_y;
		pz[n]=room->vertexes[n].z;
		gx[n]=gy[n]=0.0f;
	}
		
	map_mesh_add_poly(&map,mesh_idx,room->nvertex,px,py,pz,gx,gy,ag_texture_floor);

		// add the ceiling

	for (n=0;n!=room->nvertex;n++) {
		py[n]=ag_map_bottom_y-(ag_size_room_high+ag_size_floor_high);
		if (room->second_story) py[n]-=ag_size_room_high;
	}
// supergumba -- testing decorations
//	map_mesh_add_poly(&map,mesh_idx,room->nvertex,px,py,pz,gx,gy,ag_texture_ceiling);
}

/* =======================================================

      Square Connector Rooms
      
======================================================= */

void ag_generate_add_connector_rooms(void)
{
	int				n,nmesh,mesh_idx;
	int				px[8],py[8],pz[8];
	float			gx[8],gy[8];
	ag_room_type	*room;

	nmesh=map.mesh.nmesh;

	for (n=0;n!=nmesh;n++) {

			// did this room have a connection box?

		room=&ag_state.rooms[n];
		if (!room->connect_box.on) continue;

			// add the mesh

		mesh_idx=map_mesh_add(&map);

			// walls

		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.min.x;
		pz[0]=pz[3]=room->connect_box.min.z;
		pz[1]=pz[2]=room->connect_box.max.z;
		py[0]=py[1]=room->connect_box.min.y;
		py[2]=py[3]=room->connect_box.max.y;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_wall_2);

		px[0]=px[3]=room->connect_box.max.x;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[3]=room->connect_box.min.z;
		pz[1]=pz[2]=room->connect_box.max.z;
		py[0]=py[1]=room->connect_box.min.y;
		py[2]=py[3]=room->connect_box.max.y;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_wall_2);

		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[3]=room->connect_box.min.z;
		pz[1]=pz[2]=room->connect_box.min.z;
		py[0]=py[1]=room->connect_box.min.y;
		py[2]=py[3]=room->connect_box.max.y;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_wall_2);

		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[3]=room->connect_box.max.z;
		pz[1]=pz[2]=room->connect_box.max.z;
		py[0]=py[1]=room->connect_box.min.y;
		py[2]=py[3]=room->connect_box.max.y;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_wall_2);

			// floor and ceiling

		px[0]=px[3]=room->connect_box.min.x;
		px[1]=px[2]=room->connect_box.max.x;
		pz[0]=pz[1]=room->connect_box.min.z;
		pz[2]=pz[3]=room->connect_box.max.z;
		py[0]=py[1]=room->connect_box.max.y;
		py[2]=py[3]=room->connect_box.max.y;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_floor_2);

		py[0]=py[1]=room->connect_box.min.y;
		py[2]=py[3]=room->connect_box.min.y;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_ceiling_2);
	}
}

/* =======================================================

      Generate Map
      
======================================================= */

bool ag_generate_run(char *err_str)
{
	int				n,
					decoration_count;

		// supergumba -- hard coded

	ag_state.room_count=30;
	decoration_count=20;

		// check if auto generator has
		// required textures

	if (!ag_check_required_textures(err_str)) return(FALSE);

		// initialize auto generate structures

	ag_state.rooms=(ag_room_type*)malloc(ag_max_room*sizeof(ag_room_type));
	if (ag_state.rooms==NULL) return(FALSE);

		// clear the VBOs
		// and map data

	view_vbo_map_free();
	ag_map_clear();

		// add rooms

	for (n=0;n!=ag_state.room_count;n++) {
		ag_add_room(n==0);
	}

		// add square connector
		// rooms that happen when connected rooms
		// are squared off

	ag_generate_add_connector_rooms();

		// delete any polygons that share the
		// same space

	ag_generate_delete_shared_polygons();

		// decorations

	for (n=0;n!=decoration_count;n++) {
		ag_generate_decoration_box_stack();
	}

		// add spots and nodes

	ag_generate_spots_add();

		// free auto generate structures

	free(ag_state.rooms);

		// restore the VBOs,
		// center view, reset UVs
		// and redraw

	view_vbo_map_initialize();

	map_recalc_normals(&map,FALSE);
	map_mesh_reset_uv_all();
	map_view_goto_map_center_all();

	main_wind_draw();

	return(TRUE);
}

bool auto_generate_map(char *err_str)
{
		// run the auto generator
		// with a new unique seed

	ag_random_seed();
	return(ag_generate_run(err_str));
}

bool auto_generate_previous_map(char *err_str)
{
	ag_random_previous_seed();
	return(ag_generate_run(err_str));
}

bool auto_generate_next_map(char *err_str)
{
	ag_random_next_seed();
	return(ag_generate_run(err_str));
}

int auto_generate_get_seed(void)
{
	return(ag_state.seed);
}
