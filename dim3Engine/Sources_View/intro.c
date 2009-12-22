/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Intro

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

extern bool game_start(int skill,network_reply_join_remotes *remotes,char *err_str);
extern bool map_start(bool skip_media,char *err_str);
extern void file_open(void);
extern void setup_game_open(bool in_game);
extern void setup_network_open(void);
extern void title_set_open(char *dir,char *name,char *sound_name,bool show_view);
extern void join_open(bool local);
extern void host_open(void);

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

		// intro UI
		
	gui_initialize("Bitmaps/Backgrounds","main",FALSE);

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
	
	server.state=gs_intro;
	
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
		
		if (start_music) al_music_play(hud.intro_music);
	}
}

void intro_close(bool in_game,bool stop_music)
{
		// stop music
		
	if (stop_music) {
		if (al_music_playing()) al_music_stop();
	}
	
		// shutdown UI
		
	gui_shutdown();
	
	server.state=gs_running;
}

/* =======================================================

      Intro Input
      
======================================================= */

void intro_click_game(int skill)
{
	char			err_str[256];

	intro_close(TRUE,TRUE);

	net_setup.host.hosting=FALSE;
	net_setup.client.joined=FALSE;

	if (!game_start(skill,NULL,err_str)) {
		error_open(err_str,"Game Start Canceled");
		return;
	}

	if (!map_start(FALSE,err_str)) {
		error_open(err_str,"Game Start Canceled");
		return;
	}
}

void intro_click_load(void)
{
	intro_close(TRUE,FALSE);

	net_setup.host.hosting=FALSE;
	net_setup.client.joined=FALSE;

	file_open();
}

void intro_click(void)
{
	int				id;
	
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
			intro_close(FALSE,FALSE);
			setup_game_open(FALSE);
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
			intro_close(TRUE,FALSE);
			host_open();
			break;

		case intro_button_multiplayer_join_id:
			intro_close(TRUE,FALSE);
			join_open(TRUE);
			break;
			
		case intro_button_multiplayer_setup_id:
			intro_close(FALSE,FALSE);
			setup_network_open();
			break;

			// credit and quit
			
		case intro_button_credit_id:
			intro_close(FALSE,FALSE);
			title_set_open("Bitmaps/Backgrounds","credit","",FALSE);
			break;

		case intro_button_quit_id:
			intro_close(FALSE,FALSE);
			game_loop_quit=TRUE;
			break;
	}
}

void intro_key(void)
{
	char			ch;
	
		// check for esc
		
	ch=input_gui_get_keyboard_key(FALSE);
	if (ch!=0x1B) {
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
		
	intro_close(FALSE,FALSE);
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


