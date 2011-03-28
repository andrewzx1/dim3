/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Menus

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
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "scripts.h"
#include "video.h"

extern bool					game_app_active,game_loop_quit;

extern server_type			server;
extern map_type				map;
extern view_type			view;
extern iface_type			iface;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern void debug_game(void);
extern void map_restart_ambient(void);

/* =======================================================

      Start and Stop Menu
      
======================================================= */

void menu_draw_start(void)
{
		// if not hosting, pause

	if ((net_setup.mode!=net_mode_host) && (net_setup.mode!=net_mode_host_dedicated)) game_time_pause_start();

		// clear input and stop sounds

	input_clear();
	al_stop_all_looping_sources();
	
		// go into pause menu

	view.menu.fade_in=TRUE;
	view.menu.fade_out=FALSE;
	view.menu.active=FALSE;

	view.menu.fade_start_tick=game_time_get_raw();

	view.menu.menu_idx=0;
	view.menu.click_item_idx=-1;
	view.menu.mouse_down=FALSE;
	
		// always flip off scores
		// if they are on
		
	view.score.on=FALSE;
	
		// setup cursor

	cursor_initialize();
	input_gui_set_mouse((iface.scale_x>>1),(iface.scale_y>>1));
}

void menu_draw_end(bool fade)
{
		// leave menu

	view.menu.fade_in=FALSE;
	view.menu.fade_out=fade;
	view.menu.active=FALSE;

	view.menu.fade_start_tick=game_time_get_raw();

	cursor_shutdown();

		// clear input and restart sounds

	input_clear();
	if (server.map_open) map_restart_ambient();
	
		// if not hosting, unpause

	if ((net_setup.mode!=net_mode_host) && (net_setup.mode!=net_mode_host_dedicated)) game_time_pause_end();
}

/* =======================================================

      Menu Selection and Input
      
======================================================= */

bool menu_select(void)
{
	int						sub_idx;
	iface_menu_type			*menu;
	iface_menu_item_type	*item;
	
	if (view.menu.click_item_idx==-1) return(FALSE);
	
	menu=&iface.menu_list.menus[view.menu.menu_idx];
	item=&menu->items[view.menu.click_item_idx];
		
		// going into sub-menu
			
	if (item->sub_menu[0]!=0x0) {
		sub_idx=iface_menu_find_idx(&iface,item->sub_menu);
		if (sub_idx==-1) return(FALSE);
			
		view.menu.menu_idx=sub_idx;
		return(FALSE);
	}
	
		// exit menus
		
	menu_draw_end(FALSE);
			
		// quit item
		
	if (item->quit) {

		if (setup.editor_override.on) {			// if in editor override, just quit
			game_loop_quit=TRUE;
		}
		else {
			server.next_state=gs_intro;
		}

		return(TRUE);
	}
	
		// event items
		
	scripts_post_event_console(&js.game_attach,sd_event_menu,0,item->item_id);
	
	return(TRUE);
}

void menu_input(void)
{
		// turn on/off menu

	if (input_action_get_state_single(nc_menu)) {
	
		if ((view.menu.fade_in) || (view.menu.fade_out)) return;
		
		if (!view.menu.active) {
			menu_draw_start();
		}
		else {
			menu_draw_end(TRUE);
		}

		return;
	}

		// active menu clicking
		// only click on mouse ups so we don't get
		// the mouse down travelling through to the game

	if (!view.menu.active) return;

	if (input_gui_get_mouse_left_button_down()) {
		view.menu.mouse_down=TRUE;
		return;
	}
	
	if (view.menu.mouse_down) {
		view.menu.mouse_down=FALSE;
		menu_select();
	}
}

/* =======================================================

      Menu Drawing
      
======================================================= */

void menu_draw(void)
{
	int						n,raw_tick,x,y,kx,ky,
							wid,high,half_high;
	float					alpha;
	d3col					*col;
	iface_menu_type			*menu;
	iface_menu_item_type	*item;

	if ((!view.menu.fade_in) && (!view.menu.fade_out) && (!view.menu.active)) return;
	
	gl_2D_view_interface();

		// get current menu

	menu=&iface.menu_list.menus[view.menu.menu_idx];

		// get the alpha

	raw_tick=game_time_get_raw();

	alpha=1.0f;

	if (view.menu.fade_in) {
		raw_tick-=view.menu.fade_start_tick;
		if (raw_tick>=300) {
			view.menu.fade_in=FALSE;
			view.menu.active=TRUE;
		}
		else {
			alpha=((float)raw_tick)/300.0f;
		}
	}
	
	if (view.menu.fade_out) {
		raw_tick-=view.menu.fade_start_tick;
		if (raw_tick>=300) {
			view.menu.fade_out=FALSE;
			return;
		}
		else {
			alpha=1.0f-(((float)raw_tick)/300.0f);
		}
	}

		// get height

	high=gl_text_get_char_height(iface.font.text_size_large);
	half_high=high>>1;

		// find any selection

	x=iface.scale_x>>1;
	y=(iface.scale_y-((high+5)*menu->nitem))>>1;

	view.menu.click_item_idx=-1;

	if (game_app_active) {

		input_gui_get_mouse_position(&kx,&ky);

		item=menu->items;

		for (n=0;n!=menu->nitem;n++) {
			
			if (!((net_setup.mode!=net_mode_none) && (item->multiplayer_disable))) {

				wid=gl_text_get_string_width(font_interface_index,iface.font.text_size_large,item->data)>>1;
				
				if ((kx>=(x-wid)) && (kx<=(x+wid)) && (ky>=(y-half_high)) && (ky<=(y+half_high))) {
					view.menu.click_item_idx=n;
					break;
				}
			}
			
			y+=(high+5);
			item++;
		}
	}
	
		// draw the menus
		
	y=(iface.scale_y-((high+5)*menu->nitem))>>1;
	
	gl_text_start(font_interface_index,iface.font.text_size_large);
	
	item=menu->items;

	for (n=0;n!=menu->nitem;n++) {

			// color

		if ((net_setup.mode!=net_mode_none) && (item->multiplayer_disable)) {
			col=&iface.color.control_disabled;
		}
		else {
			if (n==view.menu.click_item_idx) {
				col=&iface.color.control_mouse_over;
			}
			else {
				col=&iface.color.control_label;
			}
		}

		gl_text_draw(x,y,item->data,tx_center,TRUE,col,alpha);

		y+=(high+5);
		item++;
	}

	gl_text_end();

		// cursor

	if ((view.menu.active) && (game_app_active)) cursor_draw();
}


