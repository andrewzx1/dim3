/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto Generation Additional Stories

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

extern int ag_shape_get_connector_index(ag_shape_type *shape,int v1_idx,int v2_idx);

/* =======================================================

      Auto Generate Story Utilities
      
======================================================= */

int ag_room_story_calculate_place(int room_idx)
{
	int				n;
	bool			left,right,top,bottom;
	ag_room_type	*room,*chk_room;

		// clear floor flags

	left=right=top=bottom=FALSE;

		// find connected items

	room=&ag_state.rooms[room_idx];

	for (n=0;n!=ag_state.nroom;n++) {
		if (n==room_idx) continue;

			// skip single floor rooms
			// or additional connector rooms

		chk_room=&ag_state.rooms[n];
		if (chk_room->shape_idx==-1) continue;
		if (ag_state.shapes[chk_room->shape_idx].single_floor) continue;

			// check for collisions

		if (((chk_room->min.z>=room->min.z) && (chk_room->min.z<room->max.z)) || ((chk_room->max.z>room->min.z) && (chk_room->max.z<=room->max.z))) {
			if (chk_room->min.x==room->max.x) right=TRUE;
			if (chk_room->max.x==room->min.x) left=TRUE;
		}

		if (((chk_room->min.x>=room->min.x) && (chk_room->min.x<room->max.x)) || ((chk_room->max.x>room->min.x) && (chk_room->max.x<=room->max.x))) {
			if (chk_room->min.z==room->max.z) bottom=TRUE;
			if (chk_room->max.z==room->min.z) top=TRUE;
		}
	}

		// any floor flags set?

	if ((!left) && (!right) && (!top) && (!bottom)) return(ag_story_place_none);

		// specific floor places

	if ((left) && (!right) && (!top) && (!bottom)) return(ag_story_place_left);
	if ((!left) && (right) && (!top) && (!bottom)) return(ag_story_place_right);
	if ((!left) && (!right) && (top) && (!bottom)) return(ag_story_place_top);
	if ((!left) && (!right) && (!top) && (bottom)) return(ag_story_place_bottom);

		// more than one direction means
		// complete floor coverage

	return(ag_story_place_all);
}

/* =======================================================

      Extra Walls
      
======================================================= */

void ag_room_story_extra_walls(ag_room_type *room)
{
	int					n,k,ty,by,high,
						mesh_idx;
	int					px[4],py[4],pz[4];
	float				gx[4],gy[4];
	d3vct				*size;
	ag_shape_type		*shape;

		// get height

	shape=&ag_state.shapes[room->shape_idx];

	size=&room->size;
	high=(int)(100.0f*size->y);

	by=room->min.y;
	ty=by-high;

		// build next story walls
	
	mesh_idx=map_mesh_add(&map);

	for (n=0;n!=shape->nvertex;n++) {
		k=n+1;
		if (k==shape->nvertex) k=0;

			// build the wall

		px[0]=px[3]=(int)(((float)shape->vertexes[n].x)*size->x)+room->min.x;
		px[1]=px[2]=(int)(((float)shape->vertexes[k].x)*size->x)+room->min.x;
		pz[0]=pz[3]=(int)(((float)shape->vertexes[n].z)*size->z)+room->min.z;
		pz[1]=pz[2]=(int)(((float)shape->vertexes[k].z)*size->z)+room->min.z;
		py[0]=py[1]=ty;
		py[2]=py[3]=by;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;
	
		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_wall);
	}
}

void ag_room_story_reset_size(ag_room_type *room)
{
	int					high;
	d3vct				*size;

	size=&room->size;
	high=(int)(100.0f*size->y);

	room->multi_story=TRUE;
	room->min.y-=high;
}

/* =======================================================

      Side Stories
      
======================================================= */

void ag_room_story_side(ag_room_type *room,int place)
{
	int					n,k,k2,x,z,x2,z2,ty,by,
						high,mesh_idx;
	int					px[4],py[4],pz[4];
	float				gx[4],gy[4];
	bool				flip;
	d3vct				*size;
	ag_shape_type		*shape;
	ag_shape_poly_type	*shape_poly;

		// get height
		// the second floor needs to be slightly above
		// so doors still have tops

	shape=&ag_state.shapes[room->shape_idx];

	size=&room->size;
	high=(int)(10.0f*size->y);

	ty=room->min.y-high;
	by=ty+high;

		// build in the floor
		// we put in a floor for every side
		// that has a connection to another room
		// or if more than one, then a wrap around
		// story

	mesh_idx=map_mesh_add(&map);

	for (n=0;n!=shape->npoly;n++) {

		shape_poly=&shape->polys[n];

			// determine which place we are
			// creating second story for

		switch (place) {
			case ag_story_place_all:
				flip=FALSE;
				break;
			case ag_story_place_left:
				if (!shape_poly->top) continue;
				flip=TRUE;
				break;
			case ag_story_place_right:
				if (!shape_poly->bottom) continue;
				flip=TRUE;
				break;
			case ag_story_place_top:
				if (!shape_poly->top) continue;
				flip=FALSE;
				break;
			case ag_story_place_bottom:
				if (!shape_poly->bottom) continue;
				flip=FALSE;
				break;
		}

			// the floor

		for (k=0;k!=shape_poly->npt;k++) {

			if (!flip) {
				x=shape->vertexes[shape_poly->v[k]].x;
				z=shape->vertexes[shape_poly->v[k]].z;
			}
			else {
				x=shape->vertexes[shape_poly->v[k]].z;
				z=shape->vertexes[shape_poly->v[k]].x;
			}

			px[k]=(int)(((float)x)*size->x)+room->min.x;
			pz[k]=(int)(((float)z)*size->z)+room->min.z;
			py[k]=ty;

			gx[k]=((float)x)/100.0f;
			gy[k]=((float)z)/100.0f;
		}
	
		map_mesh_add_poly(&map,mesh_idx,shape_poly->npt,px,py,pz,gx,gy,ag_texture_additional_floor);

			// the ceiling

		for (k=0;k!=shape_poly->npt;k++) {
			py[k]=by;
		}

		map_mesh_add_poly(&map,mesh_idx,shape_poly->npt,px,py,pz,gx,gy,ag_texture_additional_floor);

			// the walls

		for (k=0;k!=shape_poly->npt;k++) {
			k2=k+1;
			if (k2==shape_poly->npt) k2=0;

			if (!flip) {
				x=shape->vertexes[shape_poly->v[k]].x;
				z=shape->vertexes[shape_poly->v[k]].z;
				x2=shape->vertexes[shape_poly->v[k2]].x;
				z2=shape->vertexes[shape_poly->v[k2]].z;
			}
			else {
				x=shape->vertexes[shape_poly->v[k]].z;
				z=shape->vertexes[shape_poly->v[k]].x;
				x2=shape->vertexes[shape_poly->v[k2]].z;
				z2=shape->vertexes[shape_poly->v[k2]].x;
			}

			px[0]=px[3]=(int)(((float)x)*size->x)+room->min.x;
			px[1]=px[2]=(int)(((float)x2)*size->x)+room->min.x;
			pz[0]=pz[3]=(int)(((float)z)*size->z)+room->min.z;
			pz[1]=pz[2]=(int)(((float)z2)*size->z)+room->min.z;
			py[0]=py[1]=ty;
			py[2]=py[3]=by;

			gx[0]=gx[3]=0.0f;
			gx[1]=gx[2]=1.0f;
			gy[0]=gy[1]=0.0f;
			gy[2]=gy[3]=1.0f;

			map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_additional_floor);
		}
	}
}

/* =======================================================

      All Plus Hole Stories
      
======================================================= */

float ag_room_story_all_fix_coordinate(int x)
{
	float			fx;

	fx=(float)(x-50);
	fx*=0.5f;
	return(fx+50.0f);
}

void ag_room_story_all(ag_room_type *room)
{
	int					n,k,ty,by,high,
						mesh_idx;
	int					px[4],py[4],pz[4];
	float				gx[4],gy[4];
	d3vct				*size;
	ag_shape_type		*shape;

		// get height
		// the second floor needs to be slightly above
		// so doors still have tops

	shape=&ag_state.shapes[room->shape_idx];

	size=&room->size;
	high=(int)(10.0f*size->y);

	ty=room->min.y-high;
	by=ty+high;

		// build next story 
		// floor and ceilings
	
	mesh_idx=map_mesh_add(&map);
	
	for (n=0;n!=shape->nvertex;n++) {
		k=n+1;
		if (k==shape->nvertex) k=0;

			// these floors go from the wall
			// to a spot inside the wall

		px[0]=(int)(((float)shape->vertexes[n].x)*size->x)+room->min.x;
		pz[0]=(int)(((float)shape->vertexes[n].z)*size->z)+room->min.z;
		px[1]=(int)(((float)shape->vertexes[k].x)*size->x)+room->min.x;
		pz[1]=(int)(((float)shape->vertexes[k].z)*size->z)+room->min.z;
		px[2]=(int)(ag_room_story_all_fix_coordinate(shape->vertexes[k].x)*size->x)+room->min.x;
		pz[2]=(int)(ag_room_story_all_fix_coordinate(shape->vertexes[k].z)*size->z)+room->min.z;
		px[3]=(int)(ag_room_story_all_fix_coordinate(shape->vertexes[n].x)*size->x)+room->min.x;
		pz[3]=(int)(ag_room_story_all_fix_coordinate(shape->vertexes[n].z)*size->z)+room->min.z;
		py[0]=py[1]=py[2]=py[3]=ty;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;
	
		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_additional_floor);

			// the ceiling

		py[0]=py[1]=py[2]=py[3]=by;
		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_additional_floor);
	}

		// walls

	for (n=0;n!=shape->nvertex;n++) {
		k=n+1;
		if (k==shape->nvertex) k=0;

			// these floors go from the wall
			// to a spot inside the wall

		px[0]=px[3]=(int)(ag_room_story_all_fix_coordinate(shape->vertexes[k].x)*size->x)+room->min.x;
		pz[0]=pz[3]=(int)(ag_room_story_all_fix_coordinate(shape->vertexes[k].z)*size->z)+room->min.z;
		px[1]=px[2]=(int)(ag_room_story_all_fix_coordinate(shape->vertexes[n].x)*size->x)+room->min.x;
		pz[1]=pz[2]=(int)(ag_room_story_all_fix_coordinate(shape->vertexes[n].z)*size->z)+room->min.z;
		py[0]=py[1]=ty;
		py[2]=py[3]=by;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;
	
		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_additional_floor);
	}
}

/* =======================================================

      Build Additional Stories
      
======================================================= */

void ag_generate_additional_stories(void)
{
	int					n,place;
	ag_room_type		*room;

		// find all the rooms that
		// are connected to another room
		// (non-corridor)  All these get
		// additional stories

	for (n=0;n!=ag_state.nroom;n++) {

			// skip single floors
			// or additional connector rooms

		room=&ag_state.rooms[n];
		if (room->shape_idx==-1) continue;
		if (ag_state.shapes[room->shape_idx].single_floor) continue;

			// where is the story place?

		place=ag_room_story_calculate_place(n);

			// create the story

		switch (place) {

			case ag_story_place_none:
				break;

			case ag_story_place_all:
				ag_room_story_extra_walls(room);
				ag_room_story_all(room);
				ag_room_story_reset_size(room);
				break;

			default:
				ag_room_story_extra_walls(room);
				ag_room_story_side(room,place);
				ag_room_story_reset_size(room);
				break;

		}
	}
}

