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

extern d3pnt					view_pnt;
extern d3ang					view_ang;

extern map_type					map;
extern editor_state_type		state;

/* =======================================================

      X/Y Mouse Movement
      
======================================================= */

void walk_view_mouse_xy_movement(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						x,y,xadd,zadd,yadd,sz;
	d3pnt					old_pt;
    
    os_set_hand_cursor();
	
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	while (!os_track_mouse_location(pt,NULL)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;

		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
		switch (view_move_dir) {
			case vm_dir_forward:
				xadd=x;
				yadd=y;
				zadd=0;
				rotate_2D_point_center(&xadd,&zadd,-view_setup->ang.y);
				break;
			case vm_dir_side:
				xadd=-x;
				yadd=y;
				zadd=0;
				rotate_2D_point_center(&xadd,&zadd,view_setup->ang.y);
				break;
			case vm_dir_top:
				sz=(int)((float)(magnify_factor_max-state.magnify_factor)*mouse_top_view_drag_scale);
				if (sz<1) sz=1;
				xadd=x*sz;
				yadd=0;
				zadd=y*sz;
				break;
			default:
				xadd=yadd=zadd=0;
				break;
		}
		
		view_pnt.z+=(zadd*32);
		view_pnt.x+=(xadd*32);
		view_pnt.y+=(yadd*32);

        main_wind_draw();
	}
}

/* =======================================================

      Walk View Z Movement
      
======================================================= */

void walk_view_mouse_yz_movement(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						x,y,xadd,zadd,yadd;
	d3pnt					old_pt;
    
    os_set_drag_cursor();

	memmove(&old_pt,pt,sizeof(d3pnt));
	
	while (!os_track_mouse_location(pt,NULL)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
			// z movement
			
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
		switch (view_move_dir) {
			case vm_dir_forward:
				xadd=0;
				yadd=0;
				zadd=y;
				rotate_2D_point_center(&xadd,&zadd,-view_setup->ang.y);
				break;
			case vm_dir_side:
				xadd=0;
				yadd=0;
				zadd=-y;
				rotate_2D_point_center(&xadd,&zadd,view_setup->ang.y);
				break;
			case vm_dir_top:
				xadd=0;
				yadd=-y;
				zadd=0;
				rotate_2D_point_center(&xadd,&zadd,view_setup->ang.y);
				break;
			default:
				xadd=yadd=zadd=0;
				break;
		}
		
		view_pnt.x+=(xadd*32);
		view_pnt.y+=(yadd*32);
		view_pnt.z+=(zadd*32);
		
			// y turn
			
		if (view_move_dir==vm_dir_forward) {
			if (labs(x)>5) {
				view_ang.y=angle_add(view_ang.y,(float)(x/5));
			}
		}

        main_wind_draw();
	}
}

void walk_view_scroll_wheel_z_movement(editor_3D_view_setup *view_setup,int delta,int view_move_dir)
{
	int						xadd,zadd,yadd;

	xadd=yadd=zadd=0;
	
	if (view_move_dir==vm_dir_forward) {
		xadd=0;
		yadd=0;
		zadd=delta*10;
		if (view_setup->swap_on) zadd=-zadd;
	}
	else {
		xadd=-(delta*10);
		yadd=0;
		zadd=0;
		if (view_setup->swap_on) xadd=-xadd;
	}
	
	view_pnt.x+=(xadd*32);
	view_pnt.y+=(yadd*32);
	view_pnt.z+=(zadd*32);
	
	main_wind_draw();
}

void walk_view_scroll_wheel_rot_z_movement(editor_3D_view_setup *view_setup,int delta)
{
	int						xadd,zadd,yadd;
	
	xadd=0;
	yadd=0;
	zadd=delta*10;
	
	rotate_point_center(&xadd,&yadd,&zadd,0.0f,view_ang.y,0.0f);
	
	view_pnt.x-=(xadd*32);
	view_pnt.y+=(yadd*32);
	view_pnt.z+=(zadd*32);
	
	main_wind_draw();
}

/* =======================================================

      Turn Mouse Movement
      
======================================================= */

void walk_view_mouse_turn(d3pnt *pt)
{
	int						x,y;
	bool					redraw;
	d3pnt					old_pt;
    
    os_set_drag_cursor();

	memmove(&old_pt,pt,sizeof(d3pnt));
	
	while (!os_track_mouse_location(pt,NULL)) {
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		redraw=FALSE;
	
			// y turning
			
		if (labs(x)>5) {
			old_pt.x=pt->x;
			
			view_ang.y=angle_add(view_ang.y,(float)(x/5));

			redraw=TRUE;
		}
		
			// x turning
			
		if (labs(y)>5) {
			old_pt.y=pt->y;
			
			view_ang.x+=(float)(y/5);
			if (view_ang.x<-30.0f) view_ang.x=-30.0f;
			if (view_ang.x>30.0f) view_ang.x=30.0f;

			redraw=TRUE;
		}
		
		if (redraw) main_wind_draw();
	}
}

