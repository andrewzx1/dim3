/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI SimpleSave Pick

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

#define simplesave_pick_frame_id				0
#define simplesave_pick_button_cancel_id		1

#define simplesave_pick_button_start_id			100
#define simplesave_pick_button_erase_id			200

extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;

bool						simplesave_pick_esc_down;

/* =======================================================

      SimpleSave Pick Open and Close
      
======================================================= */

void simplesave_pick_open(void)
{
	int						n,x,y,bx,by,wid,high,save_high,
							butt_wid,butt_high,padding,control_y_add;

		// intro UI
		
	gui_initialize("Bitmaps/Backgrounds","default");

		// get height

	padding=element_get_padding();
	control_y_add=element_get_control_high();
	
	butt_wid=element_get_button_short_wid();
	butt_high=element_get_button_high();
	
		// dialog and frame

	x=25;
	y=25;
	wid=iface.scale_x-50;
	high=iface.scale_y-50;
	
	element_frame_add("Pick Game",simplesave_pick_frame_id,x,y,wid,high);

		// simple save pickers

	bx=x+10;
	wid-=20;

	by=y+(control_y_add+padding);

	save_high=high-(padding*(simplesave_pick_button_start_id+1));

	for (n=0;n!=max_simple_save_spot;n++) {
		element_button_box_add((simplesave_pick_button_start_id+n),bx,by,wid,save_high,element_pos_left,element_pos_top);
		by+=(save_high+padding);
	}

		// cancel button

	bx=(x+wid)-padding;
	by=(y+high)-padding;

	element_button_text_add("Cancel",simplesave_pick_button_cancel_id,((bx-butt_wid)-padding),by,butt_wid,butt_high,element_pos_right,element_pos_bottom);
	
		// in key state
	
	simplesave_pick_esc_down=FALSE;
}

void simplesave_pick_close(void)
{
	gui_shutdown();
}

/* =======================================================

      SimpleSave Pick Input
      
======================================================= */

void simplesave_pick_click(void)
{
	int				id;
	
		// element being clicked?
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
		// regular button clicks

	if (id==simplesave_pick_button_cancel_id) {
		server.next_state=gs_intro;
		return;
	}

		// erase button clicks


		// play button clicks

	if ((id>=simplesave_pick_button_start_id) && (id<(simplesave_pick_button_start_id+max_simple_save_spot))) {

			// if we have any options, then
			// go to options interface

		if (intro_click_has_singleplayer_options()) {
			intro_simple_save_idx=id-simplesave_pick_button_start_id;
			server.next_state=gs_singleplayer_option;
			return;
		}

			// otherwise, just start the
			// game

		intro_start_game(skill_medium,0,NULL,simple_save_idx);
		return;
	}

/*		
	switch (id) {
	
		case singleplayer_option_button_ok_id:

				// get setup

			skill=skill_medium;
			if (iface.singleplayer.skill) skill=element_get_value(singleplayer_option_skill_id);
			
			option_flags=0x0;
			for (n=0;n!=iface.singleplayer.option_list.noption;n++) {
				if (element_get_value(singleplayer_option_option_start_id+n)!=0) option_flags|=(0x1<<n);
			}
			
				// picked map?
				
			map_name[0]=0x0;
			
			if (iface.singleplayer.map_pick) {
				idx=element_get_value(singleplayer_option_map_table_id);
				if (idx!=-1) singleplayer_option_map_list_get_name(idx,map_name);
			}

				// start game

			if (map_name[0]==0x0) {
				intro_start_game(skill,option_flags,NULL,intro_simple_save_idx);
			}
			else {
				intro_start_game(skill,option_flags,map_name,intro_simple_save_idx);
			}
			
			break;

		case singleplayer_option_button_cancel_id:
			server.next_state=gs_intro;
			break;

	}
	*/
}

void simplesave_pick_key(void)
{
		// check for esc
		
	if (!input_get_keyboard_escape()) {
		simplesave_pick_esc_down=FALSE;
		return;
	}
	
	if (simplesave_pick_esc_down) return;
	
	hud_click();
	
	simplesave_pick_esc_down=TRUE;

		// escape cancels

	server.next_state=gs_intro;
}

/* =======================================================

      Run SimpleSave Pick Page
      
======================================================= */

void simplesave_pick_run(void)
{
	gui_draw(1.0f,TRUE);
	simplesave_pick_click();
	simplesave_pick_key();
}


