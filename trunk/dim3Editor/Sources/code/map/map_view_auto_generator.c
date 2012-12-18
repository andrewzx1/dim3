/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Auto-Generate View Routines

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern list_palette_type		file_palette,map_palette;

extern file_path_setup_type		file_path_setup;
extern map_type					map;
extern app_state_type			state;
extern app_pref_type			pref;

/* =======================================================

      Enter Map Auto-Generate View
      
======================================================= */

void view_map_auto_generate_start(void)
{
	state.map.auto_generate_on=TRUE;
	main_wind_draw();
}

/* =======================================================

      Map Auto-Generate View Cursors
      
======================================================= */

bool map_view_auto_generate_cursor(d3pnt *pnt)
{
	os_set_hand_cursor();
	return(TRUE);
}

/* =======================================================

      Map Auto-Generate View Scroll Wheel
      
======================================================= */

void map_view_auto_generate_scroll_wheel(d3pnt *pnt,int delta)
{
	/*
	int					mv;
	d3vct				move_vct;
	d3pnt				view_pnt,move_pnt,look_pnt;
	editor_view_type	*view;
	
		// select clicking view
		
	view_select_view(pnt);
	
		// handle click

	view=view_get_current_view();

		// get movement

	move_pnt.x=move_pnt.y=move_pnt.z=0;

	mv=delta*move_scroll_wheel_scale;
	if (pref.flip_forward_movement) mv=-mv;

		// free look

	if ((select_count()!=1) || (state.map.free_look)) {
		view_mouse_get_forward_axis(view,&move_pnt,-mv);
	}
	
		// look at

	else {
		view_get_position(&view_pnt);
		
		select_get_center(&look_pnt);

		vector_create(&move_vct,look_pnt.x,look_pnt.y,look_pnt.z,view_pnt.x,view_pnt.y,view_pnt.z);
		vector_normalize(&move_vct);

		move_pnt.x=(int)(move_vct.x*((float)mv));
		move_pnt.y=(int)(move_vct.y*((float)mv));
		move_pnt.z=(int)(move_vct.z*((float)mv));
	}

	view_move_position(&move_pnt);
	
	main_wind_draw();
	*/
}

/* =======================================================

      Map Auto-Generate View Clicking
      
======================================================= */

bool map_view_auto_generate_click(d3pnt *pnt,bool double_click)
{
	/*
	editor_view_type	*view;
	
		// select clicking view
		
	view_select_view(pnt);
	
		// handle click

	view=view_get_current_view();
	if (!view_point_in_view(view,pnt)) return(FALSE);
		
		// scrolling and movement clicks
		
	if (os_key_space_down()) {
		view_mouse_scroll_movement(view,pnt);
		return(TRUE);
	}

	if (os_key_option_down()) {
		view_mouse_forward_movement(view,pnt);
		return(TRUE);
	}

	if (os_key_command_down()) {
		view_mouse_turn(view,pnt);
		return(TRUE);
	}

		// click the view pieces

	view_click_piece(view,pnt,double_click);
	*/
	return(TRUE);
}

/* =======================================================

      Map Auto-Generate View Draw
      
======================================================= */

void map_view_auto_generate_draw(void)
{
	editor_view_type	view;
	
		// make the view
		// special top-down center

	view.box.lft=0.0f;
	view.box.rgt=1.0f;
	view.box.top=0.0f;
	view.box.bot=1.0f;

	view.pnt.x=map_max_size/2;
	view.pnt.y=(map_max_size/2)-200000;
	view.pnt.z=map_max_size/2;

	view.ang.x=270.0f;
	view.ang.y=0.0f;
	view.ang.z=0.0f;

	view.uv_layer=0;

	view.cull=FALSE;
	view.ortho=FALSE;
	view.clip=FALSE;
	view.no_rot=TRUE;

		// draw the view
		
	map_view_draw_view(&view);

}



