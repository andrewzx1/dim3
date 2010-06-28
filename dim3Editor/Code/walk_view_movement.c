/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Movement Routines

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

extern map_type					map;
extern editor_state_type		state;

/* =======================================================

      Mouse Movement
      
======================================================= */

void walk_view_get_direction(d3pnt *pnt)
{
	float			fx,fy,fz;
	d3ang			ang;
	matrix_type		mat;
	
	walk_view_get_angle(&ang);
	
	fx=(float)pnt->x;
	fy=(float)pnt->y;
	fz=(float)pnt->z;

	if (ang.x!=0) {
		matrix_rotate_x(&mat,ang.x);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	}
	
	if (ang.y!=0) {
		matrix_rotate_y(&mat,angle_add(ang.y,180.0f));
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	}
	
	if (ang.z!=0) {
		matrix_rotate_z(&mat,ang.z);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	}
	
	pnt->x=(int)fx;
	pnt->y=(int)fy;
	pnt->z=(int)fz;
}

void walk_view_mouse_get_scroll_horizontal_axis(d3pnt *pnt,int dist)
{
	d3ang			ang;
	
		// always move on axises, find
		// closest to view angle
		
	walk_view_get_angle(&ang);
	
	if ((ang.y>=45.0f) && (ang.y<135.0f)) {
		pnt->z+=-dist;
		return;
	}
	if ((ang.y>=135.0f) && (ang.y<225.0f)) {
		pnt->x+=-dist;
		return;
	}
	if ((ang.y>=225.0f) && (ang.y<315.0f)) {
		pnt->z+=dist;
		return;
	}
	
	pnt->x+=dist;
}

void walk_view_mouse_get_scroll_vertical_axis(d3pnt *pnt,int dist)
{
	d3ang			ang;
	
		// always move on axises, find
		// closest to view angle
		
	walk_view_get_angle(&ang);

	if ((ang.x<=315.0f) && (ang.x>180.0f)) {
		if ((ang.y>=45.0f) && (ang.y<135.0f)) {
			pnt->x+=dist;
			return;
		}
		if ((ang.y>=135.0f) && (ang.y<225.0f)) {
			pnt->z+=-dist;
			return;
		}
		if ((ang.y>=225.0f) && (ang.y<315.0f)) {
			pnt->x+=-dist;
			return;
		}
		
		pnt->z+=dist;
		return;
	}
	
	if ((ang.x>=45.0f) && (ang.x<180.0f)) {
		if ((ang.y>=45.0f) && (ang.y<135.0f)) {
			pnt->x+=-dist;
			return;
		}
		if ((ang.y>=135.0f) && (ang.y<225.0f)) {
			pnt->z+=dist;
			return;
		}
		if ((ang.y>=225.0f) && (ang.y<315.0f)) {
			pnt->x+=dist;
			return;
		}
		
		pnt->z+=-dist;
		return;
	}
	
	pnt->y+=dist;
	
}

void walk_view_mouse_get_forward_axis(d3pnt *pnt,int dist)
{
	d3ang			ang;
	
		// always move on axises, find
		// closest to view angle
		
	walk_view_get_angle(&ang);

	if ((ang.x<=315.0f) && (ang.x>180.0f)) {
		pnt->y+=-dist;
		return;
	}
	
	if ((ang.x>=45.0f) && (ang.x<180.0f)) {
		pnt->y+=dist;
		return;
	}

	if ((ang.y>=45.0f) && (ang.y<135.0f)) {
		pnt->x+=dist;
		return;
	}
	if ((ang.y>=135.0f) && (ang.y<225.0f)) {
		pnt->z+=-dist;
		return;
	}
	if ((ang.y>=225.0f) && (ang.y<315.0f)) {
		pnt->x+=-dist;
		return;
	}
	
	pnt->z+=dist;
}


void walk_view_mouse_scroll_movement(d3pnt *pnt)
{
	int						x,y;
	d3pnt					old_pnt,move_pnt;
    
    os_set_hand_cursor();
	
	memmove(&old_pnt,pnt,sizeof(d3pnt));
	
	while (!os_track_mouse_location(pnt,NULL)) {
		
		if ((pnt->x==old_pnt.x) && (pnt->y==old_pnt.y)) continue;

		x=old_pnt.x-pnt->x;
		y=old_pnt.y-pnt->y;
		
		memmove(&old_pnt,pnt,sizeof(d3pnt));
		
		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		walk_view_mouse_get_scroll_horizontal_axis(&move_pnt,(x*move_mouse_scale));
		walk_view_mouse_get_scroll_vertical_axis(&move_pnt,(y*move_mouse_scale));

		walk_view_move_position(&move_pnt);
		
        main_wind_draw();
	}
}

void walk_view_mouse_forward_movement(d3pnt *pnt)
{
	int				x,y;
	d3pnt			old_pnt,move_pnt;
	d3ang			turn_ang;
    
    os_set_drag_cursor();

	memmove(&old_pnt,pnt,sizeof(d3pnt));
	
	while (!os_track_mouse_location(pnt,NULL)) {
		
		if ((pnt->x==old_pnt.x) && (pnt->y==old_pnt.y)) continue;
		
			// z movement
			
		x=old_pnt.x-pnt->x;
		y=old_pnt.y-pnt->y;
		
		memmove(&old_pnt,pnt,sizeof(d3pnt));
		
		move_pnt.x=move_pnt.y=move_pnt.z=0;

		walk_view_mouse_get_forward_axis(&move_pnt,(y*move_mouse_scale));
		walk_view_move_position(&move_pnt);
		
			// y turn
			
		if (labs(x)>5) {
			turn_ang.x=turn_ang.z=0.0f;
			turn_ang.y=(float)(x/move_mouse_turn_reduce_scale);
			walk_view_turn_angle(&turn_ang);
		}

        main_wind_draw();
	}
}

void walk_view_scroll_wheel_z_movement(int delta)
{
	d3pnt			move_pnt;

	move_pnt.x=move_pnt.y=move_pnt.z=0;
	
	walk_view_mouse_get_forward_axis(&move_pnt,(delta*move_scroll_wheel_scale));
	walk_view_move_position(&move_pnt);
	
	main_wind_draw();
}

/* =======================================================

      Turn Mouse Movement
      
======================================================= */

void walk_view_mouse_turn(d3pnt *pnt)
{
	int				x,y;
	bool			redraw;
	d3pnt			old_pnt;
	d3ang			turn_ang;
    
    os_set_drag_cursor();

	memmove(&old_pnt,pnt,sizeof(d3pnt));
	
	while (!os_track_mouse_location(pnt,NULL)) {
		
		x=old_pnt.x-pnt->x;
		y=old_pnt.y-pnt->y;
		
		redraw=FALSE;
	
			// y turning
			
		if (labs(x)>5) {
			old_pnt.x=pnt->x;
			
			turn_ang.x=turn_ang.z=0.0f;
			turn_ang.y=-(float)(x/move_mouse_turn_reduce_scale);
			walk_view_turn_angle(&turn_ang);

			redraw=TRUE;
		}
		
			// x turning
			
		if (labs(y)>5) {
			old_pnt.y=pnt->y;
			
			turn_ang.y=turn_ang.z=0.0f;
			turn_ang.x=-(float)(y/move_mouse_turn_reduce_scale);
			walk_view_turn_angle(&turn_ang);

			redraw=TRUE;
		}
		
		if (redraw) main_wind_draw();
	}
}

