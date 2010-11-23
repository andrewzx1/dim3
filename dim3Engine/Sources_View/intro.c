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

extern bool					game_loop_quit;

extern server_type			server;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;

bool						intro_in_new_game,intro_esc_down;
bitmap_type					intro_bitmap;

/* =======================================================

      Intro Operations
      
======================================================= */

void intro_state_button_show(void)
{
	element_hide(intro_button_game_new_id,intro_in_new_game);
	element_hide(intro_button_game_load_id,intro_in_new_game);
	element_hide(intro_button_game_setup_id,intro_in_new_game);

	element_hide(intro_button_game_new_easy_id,(!intro_in_new_game));
	element_hide(intro_button_game_new_medium_id,(!intro_in_new_game));
	element_hide(intro_button_game_new_hard_id,(!intro_in_new_game));
	element_hide(intro_button_game_new_cancel_id,(!intro_in_new_game));
	
	element_hide(intro_button_multiplayer_host_id,intro_in_new_game);
	element_hide(intro_button_multiplayer_join_id,intro_in_new_game);
	element_hide(intro_button_multiplayer_setup_id,intro_in_new_game);

	element_hide(intro_button_credit_id,intro_in_new_game);
	element_hide(intro_button_quit_id,intro_in_new_game);
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

void intro_open(void)
{
	bool		start_music;
	char		err_str[256];

		// intro UI
		
	gui_initialize("Bitmaps/Backgrounds","main");

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

		// correct panel

	intro_in_new_game=FALSE;
	intro_state_button_show();

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

      Intro Input
      
======================================================= */

void intro_click_game(int skill)
{
	char			err_str[256];
	
		// goto running state
		
	server.next_state=gs_running;

		// not in networking

	net_setup.mode=net_mode_none;

		// start game

	if (!game_start(FALSE,skill,err_str)) {
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

void intro_click(void)
{
	int				id;
	char			err_str[256];
	
		// element being clicked?
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
		// run click
		
	switch (id) {
	
		case intro_button_game_new_id:
			if (hud.skill) {
				intro_in_new_game=TRUE;
				intro_state_button_show();
			}
			else {
				intro_click_game(skill_medium);
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
			intro_click_game(skill_easy);
			break;

		case intro_button_game_new_medium_id:
			intro_click_game(skill_medium);
			break;

		case intro_button_game_new_hard_id:
			intro_click_game(skill_hard);
			break;

		case intro_button_game_new_cancel_id:
			intro_in_new_game=FALSE;
			intro_state_button_show();
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
		
	if (intro_in_new_game) {
		intro_in_new_game=FALSE;
		intro_state_button_show();
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


