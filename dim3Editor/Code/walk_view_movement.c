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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

extern map_type					map;
extern editor_setup_type		setup;
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
	pnt->z+=(int)fz;
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

void walk_view_mouse_get_forward_axis(editor_view_type *view,d3pnt *pnt,int dist)
{
	float			fx,fy,fz,ang_x;
	matrix_type		mat;

	if (dist==0) return;

	fx=0.0f;
	fy=0.0f;
	fz=(float)dist;

	matrix_rotate_y(&mat,view->ang.y);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
	ang_x=view->ang.x;
	if (fabs(view->ang.x)<45.0f) ang_x=-ang_x;

	matrix_rotate_x(&mat,ang_x);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);

	pnt->x+=(int)fx;
	pnt->y+=(int)fy;
	pnt->z+=(int)fz;
}

void walk_view_mouse_scroll_movement(editor_view_type *view,d3pnt *pnt)
{
	int						x,y;
	d3pnt					old_pnt,move_pnt;
    
    os_set_hand_cursor();
	
	memmove(&old_pnt,pnt,sizeof(d3pnt));
	
	while (!os_track_mouse_location(pnt,NULL)) {
		
		if ((pnt->x==old_pnt.x) && (pnt->y==old_pnt.y)) continue;

		x=old_pnt.x-pnt->x;
		y=old_pnt.y-pnt->y;

		if (setup.flip_horz_movement) x=-x;
		if (setup.flip_vert_movement) y=-y;
		
		memmove(&old_pnt,pnt,sizeof(d3pnt));
		
		move_pnt.x=move_pnt.y=move_pnt.z=0;
		
		walk_view_mouse_get_scroll_horizontal_axis(view,&move_pnt,(x*move_mouse_scale));
		walk_view_mouse_get_scroll_vertical_axis(view,&move_pnt,(y*move_mouse_scale));

		walk_view_move_position(&move_pnt);
		
        main_wind_draw();
	}
}

void walk_view_mouse_forward_movement(editor_view_type *view,d3pnt *pnt)
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

		walk_view_mouse_get_forward_axis(view,&move_pnt,(y*move_mouse_scale));
		walk_view_move_position(&move_pnt);
		
			// y turn
			
		if (x!=0) {
			turn_ang.x=turn_ang.z=0.0f;
			turn_ang.y=((float)x)/move_mouse_turn_reduce_scale;
			if (setup.flip_forward_movement) turn_ang.y=-turn_ang.y;
			walk_view_turn_angle(&turn_ang);
		}

        main_wind_draw();
	}
}

/* =======================================================

      Scroll Wheel Mouse Movement
      
======================================================= */

void walk_view_scroll_wheel_z_movement(editor_view_type *view,int delta)
{
	int				mv;
	d3vct			move_vct;
	d3pnt			pnt,move_pnt,look_pnt;

	move_pnt.x=move_pnt.y=move_pnt.z=0;

	mv=delta*move_scroll_wheel_scale;
	if (setup.flip_forward_movement) mv=-mv;

		// free look

	if ((select_count()!=1) || (state.free_look)) {
		walk_view_mouse_get_forward_axis(view,&move_pnt,-mv);
	}
	
		// look at

	else {
		walk_view_get_position(&pnt);
		
		select_get_center(&look_pnt);

		vector_create(&move_vct,look_pnt.x,look_pnt.y,look_pnt.z,pnt.x,pnt.y,pnt.z);
		vector_normalize(&move_vct);

		move_pnt.x=(int)(move_vct.x*((float)mv));
		move_pnt.y=(int)(move_vct.y*((float)mv));
		move_pnt.z=(int)(move_vct.z*((float)mv));
	}

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

		if (setup.flip_horz_turn) x=-x;
		if (setup.flip_vert_turn) y=-y;
		
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
	float			ang_y,ang_x;
	d3pnt			old_pnt,org_pnt,
					view_pnt,org_view_pnt;
	d3ang			view_ang,org_view_ang;
    
    os_set_drag_cursor();

	memmove(&old_pnt,pnt,sizeof(d3pnt));
	memmove(&org_pnt,pnt,sizeof(d3pnt));
	
	walk_view_get_position(&org_view_pnt);
	walk_view_get_angle(&org_view_ang);
	
	while (!os_track_mouse_location(pnt,NULL)) {
		
		x=old_pnt.x-pnt->x;
		y=old_pnt.y-pnt->y;
		if ((x==0) && (y==0)) continue;
		
		old_pnt.x=pnt->x;
		old_pnt.y=pnt->y;
	
		ang_y=-(((float)(org_pnt.x-pnt->x))/move_mouse_turn_reduce_scale);
		ang_x=-(((float)(org_pnt.y-pnt->y))/move_mouse_turn_reduce_scale);
		
		ang_x=0.0f;		// supergumba -- just rotate around Y now
		
		if (setup.flip_horz_turn) ang_x=-ang_x;
		if (setup.flip_vert_turn) ang_y=-ang_y;

			// run the angle first to get any constraints

		memmove(&view_ang,&org_view_ang,sizeof(d3ang));
		view_ang.x=angle_add(view_ang.x,ang_x);
		view_ang.y=angle_add(view_ang.y,ang_y);
		walk_view_set_angle(&view_ang);

			// rebuild the angle change
			// as setting the angle can constraint it

		walk_view_get_angle(&view_ang);
		ang_x=view_ang.x-org_view_ang.x;
		ang_y=view_ang.y-org_view_ang.y;

			// now move the point

		memmove(&view_pnt,&org_view_pnt,sizeof(d3pnt));
		rotate_point(&view_pnt.x,&view_pnt.y,&view_pnt.z,cnt->x,cnt->y,cnt->z,0.0f,ang_y,0.0f);
		rotate_point(&view_pnt.x,&view_pnt.y,&view_pnt.z,cnt->x,cnt->y,cnt->z,ang_x,0.0f,0.0f);
		walk_view_set_position(&view_pnt);
		
		main_wind_draw();
	}
}

void walk_view_mouse_turn(editor_view_type *view,d3pnt *pnt)
{
	d3pnt			cnt;

		// free look

	if ((select_count()!=1) || (state.free_look)) {
		walk_view_mouse_turn_free(pnt);
		return;
	}

		// look at

	select_get_center(&cnt);
	walk_view_mouse_turn_center(pnt,&cnt);
}