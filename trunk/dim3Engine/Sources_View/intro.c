/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Intro

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

#include "interfaces.h"
#include "sounds.h"
#include "inputs.h"
#include "xmls.h"

// intro elements

#define intro_button_game_new_id				0
#define intro_button_game_load_id				1
#define intro_button_game_setup_id				2

#define intro_button_game_new_easy_id			10
#define intro_button_game_new_medium_id			11
#define intro_button_game_new_hard_id			12
#define intro_button_game_new_cancel_id			13

#define intro_button_multiplayer_host_id		20
#define intro_button_multiplayer_join_id		21
#define intro_button_multiplayer_setup_id		22

#define intro_button_credit_id					30
#define intro_button_quit_id					31

#define intro_simple_save_erase_ok				40
#define intro_simple_save_erase_cancel			41

#define intro_simple_save_button_start			100
#define intro_simple_save_button_erase			200
#define intro_simple_save_text_desc				300

// intro modes

#define intro_mode_normal						0
#define intro_mode_new_game						1
#define intro_mode_simple_save_erase			2

extern bool					game_loop_quit;

extern server_type			server;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;

int							intro_mode,intro_simple_save_idx;
bool						intro_esc_down;
bitmap_type					intro_bitmap;

/* =======================================================

      Intro UI Utilities
      
======================================================= */

void intro_show_hide_for_mode(void)
{
	int						n;
	chooser_frame_type		frame;
	
		// possible frame
		
	frame.x=(hud.scale_x>>1)-160;
	frame.y=(hud.scale_y>>1)-25;
	frame.wid=320;
	frame.high=100;
	sprintf(frame.title,"Erase Saved Game %d?",(intro_simple_save_idx+1));
	memmove(&frame.background_col,&hud.color.dialog_background,sizeof(d3col));

		// new game
		
	if (intro_mode==intro_mode_new_game) {
	
		frame.on=FALSE;
		gui_set_frame(&frame);

		element_hide(intro_button_game_new_id,TRUE);
		element_hide(intro_button_game_load_id,TRUE);
		element_hide(intro_button_game_setup_id,TRUE);
		element_hide(intro_button_multiplayer_host_id,TRUE);
		element_hide(intro_button_multiplayer_join_id,TRUE);
		element_hide(intro_button_multiplayer_setup_id,TRUE);
		element_hide(intro_button_credit_id,TRUE);
		element_hide(intro_button_quit_id,TRUE);
		
		for (n=0;n!=max_simple_save_spot;n++) {
			element_hide((intro_simple_save_button_start+n),TRUE);
			element_hide((intro_simple_save_button_erase+n),TRUE);
			element_hide((intro_simple_save_text_desc+n),TRUE);
		}
		
		element_hide(intro_simple_save_erase_ok,TRUE);
		element_hide(intro_simple_save_erase_cancel,TRUE);
		
		element_hide(intro_button_game_new_easy_id,FALSE);
		element_hide(intro_button_game_new_medium_id,FALSE);
		element_hide(intro_button_game_new_hard_id,FALSE);
		element_hide(intro_button_game_new_cancel_id,FALSE);
		
		return;
	}
	
		// simple save erase
		
	if (intro_mode==intro_mode_simple_save_erase) {
	
		frame.on=TRUE;
		gui_set_frame(&frame);

		element_enable(intro_button_game_new_id,FALSE);
		element_enable(intro_button_game_load_id,FALSE);
		element_enable(intro_button_game_setup_id,FALSE);
		element_enable(intro_button_multiplayer_host_id,FALSE);
		element_enable(intro_button_multiplayer_join_id,FALSE);
		element_enable(intro_button_multiplayer_setup_id,FALSE);
		element_enable(intro_button_credit_id,FALSE);
		element_enable(intro_button_quit_id,FALSE);
		
		for (n=0;n!=max_simple_save_spot;n++) {
			element_enable((intro_simple_save_button_start+n),FALSE);
			element_enable((intro_simple_save_button_erase+n),FALSE);
			element_enable((intro_simple_save_text_desc+n),FALSE);
		}
		
		element_hide(intro_simple_save_erase_ok,FALSE);
		element_hide(intro_simple_save_erase_cancel,FALSE);
		
		element_hide(intro_button_game_new_easy_id,TRUE);
		element_hide(intro_button_game_new_medium_id,TRUE);
		element_hide(intro_button_game_new_hard_id,TRUE);
		element_hide(intro_button_game_new_cancel_id,TRUE);
		
		return;
	}
	
		// regular
		
	frame.on=FALSE;
	gui_set_frame(&frame);

	element_hide(intro_button_game_new_id,FALSE);
	element_hide(intro_button_game_load_id,FALSE);
	element_hide(intro_button_game_setup_id,FALSE);
	element_hide(intro_button_multiplayer_host_id,FALSE);
	element_hide(intro_button_multiplayer_join_id,FALSE);
	element_hide(intro_button_multiplayer_setup_id,FALSE);
	element_hide(intro_button_credit_id,FALSE);
	element_hide(intro_button_quit_id,FALSE);
	
	element_enable(intro_button_game_new_id,TRUE);
	element_enable(intro_button_game_load_id,TRUE);
	element_enable(intro_button_game_setup_id,TRUE);
	element_enable(intro_button_multiplayer_host_id,TRUE);
	element_enable(intro_button_multiplayer_join_id,TRUE);
	element_enable(intro_button_multiplayer_setup_id,TRUE);
	element_enable(intro_button_credit_id,TRUE);
	element_enable(intro_button_quit_id,TRUE);
	
	for (n=0;n!=max_simple_save_spot;n++) {
		element_hide((intro_simple_save_button_start+n),FALSE);
		element_hide((intro_simple_save_button_erase+n),FALSE);
		element_hide((intro_simple_save_text_desc+n),FALSE);
		
		element_enable((intro_simple_save_button_start+n),TRUE);
		element_enable((intro_simple_save_button_erase+n),TRUE);
		element_enable((intro_simple_save_text_desc+n),TRUE);
		
		if (hud.simple_save_list.saves[n].save_id!=-1) {
			element_text_change((intro_simple_save_text_desc+n),hud.simple_save_list.saves[n].desc);
		}
		else {
			element_text_change((intro_simple_save_text_desc+n),"New");
		}
	}
	
	element_hide(intro_button_game_new_easy_id,TRUE);
	element_hide(intro_button_game_new_medium_id,TRUE);
	element_hide(intro_button_game_new_hard_id,TRUE);
	element_hide(intro_button_game_new_cancel_id,TRUE);
	
	element_hide(intro_simple_save_erase_ok,TRUE);
	element_hide(intro_simple_save_erase_cancel,TRUE);
}

void intro_open_add_button(hud_intro_button_type *btn,char *name,int id)
{
	char		sel_name[256],path[1024],path2[1024];

	if (!btn->on) return;

		// get name and position

	sprintf(sel_name,"%s_selected",name);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements",name,"png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements",sel_name,"png");
	element_button_bitmap_add(path,path2,id,btn->x,btn->y,btn->wid,btn->high,element_pos_left,element_pos_top);
	
	btn->element_id=id;
}

/* =======================================================

      Intro Open and Close
      
======================================================= */

void intro_open(void)
{
	int			n,x,y;
	bool		start_music;
	char		name[256],err_str[256];

		// intro UI
		
	gui_initialize("Bitmaps/Backgrounds","main");

		// buttons
		
	intro_open_add_button(&hud.intro.button_game_new,"button_game_new",intro_button_game_new_id);
	intro_open_add_button(&hud.intro.button_game_load,"button_game_load",intro_button_game_load_id);
	intro_open_add_button(&hud.intro.button_game_setup,"button_game_setup",intro_button_game_setup_id);

	intro_open_add_button(&hud.intro.button_game_new_easy,"button_game_new_easy",intro_button_game_new_easy_id);
	intro_open_add_button(&hud.intro.button_game_new_medium,"button_game_new_medium",intro_button_game_new_medium_id);
	intro_open_add_button(&hud.intro.button_game_new_hard,"button_game_new_hard",intro_button_game_new_hard_id);
	intro_open_add_button(&hud.intro.button_game_new_cancel,"button_game_new_cancel",intro_button_game_new_cancel_id);
	
	intro_open_add_button(&hud.intro.button_multiplayer_host,"button_multiplayer_host",intro_button_multiplayer_host_id);
	intro_open_add_button(&hud.intro.button_multiplayer_join,"button_multiplayer_join",intro_button_multiplayer_join_id);
	intro_open_add_button(&hud.intro.button_multiplayer_setup,"button_multiplayer_setup",intro_button_multiplayer_setup_id);

	intro_open_add_button(&hud.intro.button_credit,"button_credit",intro_button_credit_id);
	intro_open_add_button(&hud.intro.button_quit,"button_quit",intro_button_quit_id);
	
	for (n=0;n!=max_simple_save_spot;n++) {
		sprintf(name,"button_simple_start_%d",n);
		intro_open_add_button(&hud.intro.simple_save[n].button_start,name,(intro_simple_save_button_start+n));
		sprintf(name,"button_simple_erase_%d",n);
		intro_open_add_button(&hud.intro.simple_save[n].button_erase,name,(intro_simple_save_button_erase+n));

		if (hud.intro.simple_save[n].button_start.on) element_text_add("",(intro_simple_save_text_desc+n),hud.intro.simple_save[n].desc.x,hud.intro.simple_save[n].desc.y,hud.intro.simple_save[n].desc.text_size,tx_center,FALSE,FALSE);
	}
	
		// simple save options
		
	x=hud.scale_x>>1;
	y=hud.scale_y>>1;
		
	element_button_text_add("Erase",intro_simple_save_erase_ok,(x-130),y,100,50,element_pos_left,element_pos_top);
	element_button_text_add("Cancel",intro_simple_save_erase_cancel,(x+130),y,100,50,element_pos_right,element_pos_top);
	
		// read in simple saves
		
	simple_save_xml_read();
	
		// correct panel

	intro_mode=intro_mode_normal;
	intro_simple_save_idx=0;

	intro_show_hide_for_mode();

		// in intro state
	
	intro_esc_down=FALSE;
	
		// intro music
		
	if ((hud.intro_music[0]!=0x0) && (setup.music_on)) {
	
			// previously playing music?
			
		start_music=TRUE;
		
		if (al_music_playing()) {
			if (al_music_playing_is_name(hud.intro_music)) {		// same music, keep playing
				start_music=FALSE;
			}
			else {
				start_music=TRUE;
				al_music_stop();
			}
		}
		
		if (start_music) al_music_play(hud.intro_music,err_str);
	}
}

void intro_close(void)
{
		// stop music
		
	if (al_music_playing()) al_music_stop();
	
		// shutdown UI
		
	gui_shutdown();
}

/* =======================================================

      Intro Actions
      
======================================================= */

void intro_click_game(int skill,int simple_save_idx)
{
	char			err_str[256];
	
		// goto running state
		
	server.next_state=gs_running;

		// not in networking

	net_setup.mode=net_mode_none;

		// start game

	if (!game_start(FALSE,skill,simple_save_idx,err_str)) {
		error_setup(err_str,"Game Start Canceled");
		server.next_state=gs_error;
		return;
	}

	if (!map_start(FALSE,FALSE,err_str)) {
		error_setup(err_str,"Game Start Canceled");
		server.next_state=gs_error;
		return;
	}
}

void intro_click_load(void)
{
	net_setup.mode=net_mode_none;

	file_setup(FALSE);
	server.next_state=gs_file;
}

void intro_click_simple_save_start(int idx)
{
	intro_click_game(skill_medium,idx);
}

void intro_click_simple_save_erase(int idx)
{
	intro_mode=intro_mode_simple_save_erase;
	intro_simple_save_idx=idx;
	intro_show_hide_for_mode();
}

void intro_click_simple_save_erase_ok(void)
{
	hud_simple_save_type	*save;
	
		// reset save
		
	save=&hud.simple_save_list.saves[intro_simple_save_idx];
	save->save_id=-1;
	save->desc[0]=0x0;
	
	simple_save_xml_write();
	
		// back to intro

	intro_mode=intro_mode_normal;
	intro_show_hide_for_mode();
}

/* =======================================================

      Intro Input
      
======================================================= */

void intro_click(void)
{
	int				id;
	char			err_str[256];
	
		// element being clicked?
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
		// simple save start
		
	if ((id>=intro_simple_save_button_start) && (id<(intro_simple_save_button_start+max_simple_save_spot))) {
		intro_click_simple_save_start(id-intro_simple_save_button_start);
		return;
	}

		// simple save erase
		
	if ((id>=intro_simple_save_button_erase) && (id<(intro_simple_save_button_erase+max_simple_save_spot))) {
		intro_click_simple_save_erase(id-intro_simple_save_button_erase);
		return;
	}
	
		// regular button clicks
		
	switch (id) {
	
		case intro_button_game_new_id:
			if (hud.skill) {
				intro_mode=intro_mode_new_game;
				intro_show_hide_for_mode();
			}
			else {
				intro_click_game(skill_medium,0);
			}
			break;

		case intro_button_game_load_id:
			intro_click_load();
			break;

		case intro_button_game_setup_id:
			server.next_state=gs_setup_game;
			break;

			// new game buttons

		case intro_button_game_new_easy_id:
			intro_click_game(skill_easy,0);
			break;

		case intro_button_game_new_medium_id:
			intro_click_game(skill_medium,0);
			break;

		case intro_button_game_new_hard_id:
			intro_click_game(skill_hard,0);
			break;

		case intro_button_game_new_cancel_id:
			intro_mode=intro_mode_normal;
			intro_show_hide_for_mode();
			break;
			
			// simple save buttons
			
		case intro_simple_save_erase_ok:
			intro_click_simple_save_erase_ok();
			break;

		case intro_simple_save_erase_cancel:
			intro_mode=intro_mode_normal;
			intro_show_hide_for_mode();
			break;

			// multiplayer buttons

		case intro_button_multiplayer_host_id:
			server.next_state=gs_host;
			break;

		case intro_button_multiplayer_join_id:
			server.next_state=gs_join;
			break;
			
		case intro_button_multiplayer_setup_id:
			server.next_state=gs_setup_network;
			break;

			// credit and quit
			
		case intro_button_credit_id:
			title_setup("Bitmaps/Backgrounds","credit","",-1,err_str);
			break;

		case intro_button_quit_id:
			game_loop_quit=TRUE;
			break;
	}
}

void intro_key(void)
{
		// check for esc
		
	if (!input_get_keyboard_escape()) {
		intro_esc_down=FALSE;
		return;
	}
	
	if (intro_esc_down) return;
	
	hud_click();
	
	intro_esc_down=TRUE;

		// escape quits new game
		// or simple save
		
	if ((intro_mode==intro_mode_new_game) || (intro_mode==intro_mode_simple_save_erase)) {
		intro_mode=intro_mode_normal;
		intro_show_hide_for_mode();
		return;
	}
	
		// or entire game
		
	intro_close();
	game_loop_quit=TRUE;
}

/* =======================================================

      Run Intro Page
      
======================================================= */

void intro_run(void)
{
	gui_draw(1.0f,TRUE);
	intro_click();
	intro_key();
}


