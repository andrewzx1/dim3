/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Menus

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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
	#include "dim3engine.h"
#endif

#include "scripts.h"
#include "interfaces.h"
#include "video.h"
#include "inputs.h"

#define menu_editor_continue_id			1000
#define menu_editor_toggle_debug_id		1001
#define menu_editor_reload_map_id		1002
#define menu_editor_quit_id				1004

extern bool					game_loop_quit;

extern server_type			server;
extern map_type				map;
extern view_type			view;
extern js_type				js;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;

int							menu_idx,org_server_state;
bool						menu_start_trigger;

extern int menu_find(char *name);
extern void map_end(void);
extern void game_end(void);
extern void intro_open(void);
extern void debug_game(void);

/* =======================================================

      Menu Setup
      
======================================================= */

void menu_setup_control_interface(void)
{
	int					n,nitem,x,y,high;
	menu_type			*menu;
	menu_item_type		*item;
		
		// get current menu

	menu=&hud.menus[menu_idx];
		
	nitem=menu->nitem;
	item=menu->items;
	
		// setup the controls
		
	high=gl_text_get_char_height(hud.font.text_size_large);
	
	x=hud.scale_x>>1;
	y=((hud.scale_y-((high+5)*nitem))>>1)+(high+5);
	
	for (n=0;n!=nitem;n++) {
		element_text_add(item->data,n,x,y,hud.font.text_size_large,tx_center,TRUE,FALSE);
		if ((net_setup.client.joined) && (item->multiplayer_disable)) element_enable(n,FALSE);
		y+=(high+5);
		item++;
	}
}

void menu_setup_control_editor(void)
{
	int					x,y,high;
		
	high=gl_text_get_char_height(hud.font.text_size_large);
	
	x=hud.scale_x>>1;
	y=((hud.scale_y-((high+5)*4))>>1)+(high+5);

	element_text_add("Continue",menu_editor_continue_id,x,y,hud.font.text_size_large,tx_center,TRUE,FALSE);
	y+=(high+5);

	element_text_add("Debug Mode",menu_editor_toggle_debug_id,x,y,hud.font.text_size_large,tx_center,TRUE,FALSE);
	y+=(high+5);

	element_text_add("Reload Map",menu_editor_reload_map_id,x,y,hud.font.text_size_large,tx_center,TRUE,FALSE);
	y+=(high+5);

	element_text_add("Quit",menu_editor_quit_id,x,y,hud.font.text_size_large,tx_center,TRUE,FALSE);
}

void menu_setup_control(void)
{
		// clear current controls
		
	element_clear();

		// setup correct menu

	if (setup.editor_override.on) {
		menu_setup_control_editor();
	}
	else {
		menu_setup_control_interface();
	}
}

/* =======================================================

      Menu Open/Close
      
======================================================= */

void menu_open(void)
{
	gui_initialize(NULL,NULL,TRUE);

	menu_idx=0;
	menu_setup_control();

	org_server_state=server.state;
	
	server.state=gs_menu;
}

void menu_close(void)
{
	gui_shutdown();
	
	if (server.state==gs_menu) server.state=org_server_state;			// only reset to running if picked menu item didn't reset to something else
}

/* =======================================================

      Menu Select
      
======================================================= */

void menu_select_interface(void)
{
	int				sub_idx;
	menu_type		*menu;
	menu_item_type  *item;
	
	menu=&hud.menus[view.menu.menu_idx];
	item=&menu->items[view.menu.click_item_idx];
		
		// going into sub-menu
			
	if (item->sub_menu[0]!=0x0) {
		sub_idx=menu_find(item->sub_menu);
		if (sub_idx==-1) return;
			
		view.menu.menu_idx=sub_idx;
		return;
	}
	
		// selecting
		
	menu_draw_end(get_tick());
			
		// selecting the quit item
		
	if (item->quit) {

		if (setup.editor_override.on) {			// if in editor override, just quit
			game_loop_quit=TRUE;
		}
		else {
			map_end();
			game_end();
			intro_open();
		}

		return;
	}
	
		// selecting event items
		
	scripts_post_event_console(&js.game_attach,sd_event_menu,0,item->item_id);
}

void menu_select_editor(void)
{
	menu_close();

	switch (view.menu.click_item_idx) {

		case menu_editor_toggle_debug_id:
			debug_game();
			break;

		case menu_editor_reload_map_id:
			server.map_change=TRUE;
			server.skip_media=FALSE;
			break;

		case menu_editor_quit_id:
			game_loop_quit=TRUE;
			break;
	}
}

void menu_select(void)
{
	if (setup.editor_override.on) {
		menu_select_editor();
	}
	else {
		menu_select_interface();
	}
}

/* =======================================================

      Menu Triggers
      
======================================================= */

void menu_trigger_clear(void)
{
	menu_start_trigger=FALSE;
}

void menu_trigger_check(void)
{
	if (menu_start_trigger) menu_open();
}	

void menu_trigger_set(void)
{
	menu_start_trigger=TRUE;
}

/* =======================================================

      Menu Input
      
======================================================= */

void menu_click(void)
{
	int			id;

		// esc cancels menu

	if (input_action_get_state_single(nc_menu)) {
		menu_close();
		return;
	}

		// check for clicks
		
	id=gui_click();
	if (id==-1) return;
		
	hud_click();
	
//	menu_select(id);
}
	
/* =======================================================

      Run Menu
      
======================================================= */

void menu_run(void)
{
	gui_draw(1.0f,TRUE);
	menu_click();
}




void menu_draw_start(int tick)
{
	view.menu.fade_in=TRUE;
	view.menu.fade_out=FALSE;
	view.menu.active=FALSE;

	view.menu.fade_start_tick=tick;

	view.menu.menu_idx=0;
	view.menu.click_item_idx=-1;
	view.menu.mouse_down=FALSE;
	
	cursor_initialize();
}

void menu_draw_end(int tick)
{
	view.menu.fade_in=FALSE;
	view.menu.fade_out=TRUE;
	view.menu.active=FALSE;

	view.menu.fade_start_tick=tick;

	cursor_shutdown();
}

void menu_input(int tick)
{
		// turn on/off menu

	if (input_action_get_state_single(nc_menu)) {
		if (!view.menu.active) {
			menu_draw_start(tick);
		}
		else {
			menu_draw_end(tick);
		}

		return;
	}

		// active menu clicking

	if (!view.menu.active) return;

	if (input_gui_get_mouse_left_button_down()) {
		if (!view.menu.mouse_down) menu_select();
		view.menu.mouse_down=TRUE;
		return;
	}

	view.menu.mouse_down=FALSE;
}

void menu_draw(int tick)
{
	int					n,x,y,kx,ky,sel_idx,
						wid,high,half_high;
	float				alpha;
	d3col				*col;
	menu_type			*menu;
	menu_item_type		*item;

	if ((!view.menu.fade_in) && (!view.menu.fade_out) && (!view.menu.active)) return;
	
	gl_2D_view_interface();

		// get current menu

	menu=&hud.menus[view.menu.menu_idx];

		// get the alpha

	alpha=1.0f;

	if (view.menu.fade_in) {
		tick-=view.menu.fade_start_tick;
		if (tick>300) {
			view.menu.fade_in=FALSE;
			view.menu.active=TRUE;
		}
		else {
			alpha=((float)tick)/300.0f;
		}
	}
	
	if (view.menu.fade_out) {
		tick-=view.menu.fade_start_tick;
		if (tick>300) {
			view.menu.fade_out=FALSE;
		}
		else {
			alpha=1.0f-(((float)tick)/300.0f);
		}
	}

		// get height

	high=gl_text_get_char_height(hud.font.text_size_large);
	half_high=high>>1;

		// find any selection

	x=hud.scale_x>>1;
	y=(hud.scale_y-((high+5)*menu->nitem))>>1;

	input_gui_get_mouse_position(&kx,&ky);

	sel_idx=-1;
	item=menu->items;

	for (n=0;n!=menu->nitem;n++) {
		wid=gl_text_get_string_width(hud.font.text_size_large,item->data)>>1;
		
		if ((kx>=(x-wid)) && (kx<=(x+wid)) && (ky>=(y-half_high)) && (ky<=(y+half_high))) {
			sel_idx=n;
			break;
		}

		y+=(high+5);
		item++;
	}
	
		// draw the menus
		
	y=(hud.scale_y-((high+5)*menu->nitem))>>1;
	
	gl_text_start(hud.font.text_size_large);
	
	item=menu->items;

	for (n=0;n!=menu->nitem;n++) {

			// color

		if ((net_setup.client.joined) && (item->multiplayer_disable)) {
			col=&hud.color.control_disabled;
		}
		else {
			if (n==selIdx) {
				col=&hud.color.control_mouse_over;
			}
			else {
				col=&hud.color.control_label;
			}
		}

		gl_text_draw(x,y,item->data,tx_center,TRUE,col,alpha);

		y+=(high+5);
		item++;
	}

	gl_text_end();

		// clicks and cursor

	if (view.menu.active) cursor_draw();

	if (input_gui_get_mouse_left_button_down()) {
		if (!view.menu.mouse_down) {

		}
	}
	else {
		view.menu.mouse_down=FALSE;
	}

}


