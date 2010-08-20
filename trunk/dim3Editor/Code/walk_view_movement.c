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
extern setup_type				setup;
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

void walk_view_mouse_get_scroll_horizontal_axis(editor_view_type *view,d3pnt *pnt,int dist)
{
	float			fx,fy,fz;
	matrix_type		mat;

	if (dist==0) return;

	fx=(float)dist;
	fy=0.0f;
	fz=0.0f;

	matrix_rotate_y(&mat,view->ang.y);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);

	pnt->x+=(int)fx;
	pnt->z-=(int)fz;
}

void walk_view_mouse_get_scroll_vertical_axis(editor_view_type *view,d3pnt *pnt,int dist)
{
	float			fx,fy,fz;
	matrix_type		mat;

	if (dist==0) return;


		// always move on axises, find
		// closest to view angle

		// top
		
	if ((view->ang.x<=315.0f) && (view->ang.x>180.0f)) {
		fx=0.0f;
		fy=0.0f;
		fz=(float)dist;

		matrix_rotate_y(&mat,view->ang.y);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);

		pnt->x-=(int)fx;
		pnt->z+=(int)fz;

		return;
	}
	
		// bottom

	if ((view->ang.x>=45.0f) && (view->ang.x<180.0f)) {
		fx=0.0f;
		fy=0.0f;
		fz=-(float)dist;

		matrix_rotate_y(&mat,view->ang.y);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);

		pnt->x-=(int)fx;
		pnt->z+=(int)fz;

		return;
	}
	
		// forward, backward, left, right

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
	editor_view_type		*view;

	view=walk_view_get_current_view();
    
    os_set_hand_cursor();
	
	memmove(&old_pnt,pnt,sizeof(d3pnt));
	
	while (!os_track_mouse_location(pnt,NULL)) {
		
		if ((pnt->x==old_pnt.x) && (pnt->y==old_pnt.y)) continue;

		x=old_pnt.x-pnt->x;
		y=old_pnt.y-pnt->y;

		if (setup.flip_movement) {
			x=-x;
			y=-y;
		}
		
		memmove(&old_pnt,pnt,sizeof(d3pnt));
		
		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		walk_view_mouse_get_scroll_horizontal_axis(view,&move_pnt,(x*move_mouse_scale));
		walk_view_mouse_get_scroll_vertical_axis(view,&move_pnt,(y*move_mouse_scale));

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
			
		if (x!=0) {
			turn_ang.x=turn_ang.z=0.0f;
			turn_ang.y=((float)x)/move_mouse_turn_reduce_scale;
			if (setup.flip_movement) turn_ang.y=-turn_ang.y;
			walk_view_turn_angle(&turn_ang);
		}

        main_wind_draw();
	}
}

void walk_view_scroll_wheel_z_movement(int delta)
{
	d3pnt			move_pnt;

	move_pnt.x=move_pnt.y=move_pnt.z=0;
	
	walk_view_mouse_get_forward_axis(&move_pnt,-(delta*move_scroll_wheel_scale));
	walk_view_move_position(&move_pnt);
	
	main_wind_draw();
}

/* =======================================================

      Turn Mouse Movement
      
======================================================= */

void walk_view_mouse_turn_free(d3pnt *pnt)
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

		if (setup.flip_movement) {
			x=-x;
			y=-y;
		}
		
		redraw=FALSE;
	
			// y turning
			
		if (x!=0) {
			old_pnt.x=pnt->x;
			
			turn_ang.x=turn_ang.z=0.0f;
			turn_ang.y=((float)x)/move_mouse_turn_reduce_scale;
			walk_view_turn_angle(&turn_ang);

			redraw=TRUE;
		}
		
			// x turning
			
		if (y!=0) {
			old_pnt.y=pnt->y;
			
			turn_ang.y=turn_ang.z=0.0f;
			turn_ang.x=-((float)y)/move_mouse_turn_reduce_scale;
			walk_view_turn_angle(&turn_ang);

			redraw=TRUE;
		}
		
		if (redraw) main_wind_draw();
	}
}

void walk_view_mouse_turn_center(d3pnt *pnt,d3pnt *cnt)
{
	int				x,y;
	float			fang;
	bool			redraw;
	d3pnt			old_pnt,org_pnt,
					view_pnt,org_view_pnt;
    
    os_set_drag_cursor();

	memmove(&old_pnt,pnt,sizeof(d3pnt));
	memmove(&org_pnt,pnt,sizeof(d3pnt));
	
	walk_view_get_position(&org_view_pnt);
	
	while (!os_track_mouse_location(pnt,NULL)) {
		
		x=old_pnt.x-pnt->x;
		y=old_pnt.y-pnt->y;

		if (setup.flip_movement) {
			x=-x;
			y=-y;
		}
		
		redraw=FALSE;

		memmove(&view_pnt,&org_view_pnt,sizeof(d3pnt));
	
			// y turning
			
		if (x!=0) {
			old_pnt.x=pnt->x;

			fang=((float)(org_pnt.x-pnt->x))/move_mouse_turn_reduce_scale;
			rotate_point(&view_pnt.x,&view_pnt.y,&view_pnt.z,cnt->x,cnt->y,cnt->z,0.0f,fang,0.0f);

			redraw=TRUE;
		}
		
			// x turning
			
		if (y!=0) {
			old_pnt.y=pnt->y;

			fang=((float)(org_pnt.y-pnt->y))/move_mouse_turn_reduce_scale;
			rotate_point(&view_pnt.x,&view_pnt.y,&view_pnt.z,cnt->x,cnt->y,cnt->z,fang,0.0f,0.0f);

			redraw=TRUE;
		}
		
			// if point has changed, then move

		if (redraw) {
			walk_view_set_position(&view_pnt);
			main_wind_draw();
		}
	}
}

void walk_view_mouse_turn(d3pnt *pnt)
{
	d3pnt			cnt;

		// rotate freely

	if (select_count()==0) {
		walk_view_mouse_turn_free(pnt);
		return;
	}

		// rotate around selection

	select_get_center(&cnt);
	walk_view_mouse_turn_center(pnt,&cnt);
}