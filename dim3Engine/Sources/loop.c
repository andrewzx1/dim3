/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Main Loop

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "remotes.h"
#include "interfaces.h"
#include "inputs.h"
#include "video.h"
#include "consoles.h"
#include "timing.h"

extern bool					game_app_active;

extern map_type				map;
extern server_type			server;
extern view_type			view;
extern setup_type			setup;
extern network_setup_type	net_setup;

bool						interface_quit;

extern bool server_initialize(char *err_str);
extern void server_shutdown(void);
extern void server_loop(void);
extern bool view_initialize(char *err_str);
extern void view_shutdown(void);
extern void view_loop_input(void);
extern void view_run(void);
extern void view_loop_draw(void);
extern void view_loop_draw_dedicated_host(void);
extern void map_clear_changes(void);
extern bool map_need_rebuild(void);
extern bool map_rebuild_changes(char *err_str);
extern void game_end(void);
extern void map_end(void);
extern void view_clear_fps(void);
extern void view_calculate_fps(void);
extern void menu_draw_start(void);
extern void intro_close(void);
extern void error_close(void);

/* =======================================================

      Game Loop Interface Quit
      
======================================================= */

void interface_quit_trigger_clear(void)
{
	interface_quit=FALSE;
}

void interface_quit_trigger_set(void)
{
	interface_quit=TRUE;
}

bool interface_quit_trigger_check(void)
{
	return(interface_quit);
}

/* =======================================================

      Main Game Loop Run
      
======================================================= */

void loop_game_run(void)
{
		// receive networking updates
		
	if (net_setup.mode!=net_mode_none) {
		if (!remote_network_get_updates()) return;
	}
	
		// mark for interface quits
		
	interface_quit_trigger_clear();
	
		// run game

	server_loop();
	
		// sending network updates

	if (net_setup.mode!=net_mode_none) {
		remote_network_send_updates();
		remote_network_send_group_synch();
		remote_network_send_latency_ping();
	}
	
		// check for score limits
		
	score_limit_check_scores();
	
		// view input

		// it's possible we could exit here, so
		// make sure to jump out if state changes

	if (net_setup.mode!=net_mode_host_dedicated) {
		view_loop_input();
		if (server.state!=gs_running) return;
	}

		// draw the view

	if (net_setup.mode==net_mode_host_dedicated) {
		view_loop_draw_dedicated_host();
	}
	else {
		view_run();
		view_loop_draw();
	}

		// check interface quits
		
	if (interface_quit_trigger_check()) {
		map_end();
		game_end();
		server.next_state=gs_intro;
		return;
	}
	
		// calculate fps

	view_calculate_fps();
}

/* =======================================================

      App Activate/Deactivate
      
======================================================= */

void loop_app_active(void)
{
		// only windowed versions can go inactive

	if (!gl_in_window_mode()) return;

		// going inactive?

	if (game_app_active) {
		if (input_app_active()) return;

		game_app_active=FALSE;

		input_clear();
		input_mouse_pause();

			// don't pause if this game is a host

		if ((net_setup.mode!=net_mode_host) && (net_setup.mode!=net_mode_host_dedicated)) game_time_pause_start();

		return;
	}

		// becoming active?

	if (!input_app_active()) return;

	game_app_active=TRUE;
	
	input_clear();
	input_mouse_resume();

		// host games weren't paused

	if ((net_setup.mode!=net_mode_host) && (net_setup.mode!=net_mode_host_dedicated)) game_time_pause_end();
}

/* =======================================================

      State Loops
      
======================================================= */

void loop_state_run(void)
{
	switch (server.state) {
	
		case gs_running:
			loop_game_run();
			return;

		case gs_intro:
			intro_run();
			return;
			
		case gs_setup_game:
			setup_game_run();
			return;
			
		case gs_setup_network:
			setup_network_run();
			return;
			
		case gs_join:
			join_run();
			return;

		case gs_host:
			host_run();
			return;
			
		case gs_chooser:
			chooser_run();
			return;
			
		case gs_file:
			file_run();
			return;
			
		case gs_story:
			story_run();
			return;
			
		case gs_title:
			title_run();
			return;
			
		case gs_movie:
			movie_run();
			return;
			
		case gs_error:
			error_run();
			return;
			
		case gs_score_limit:
			score_limit_run();
			return;
			
	}
}

void loop_state_last_close(void)
{
	view_clear_fps();
	
	// supergumba -- get the rest of these working!
	switch (server.state) {
	/*
		case gs_running:
			loop_game_run();
			return;
*/
		case gs_intro:
			intro_close();
			return;

		case gs_setup_game:
			setup_game_close();
			return;
			
		case gs_setup_network:
			setup_network_close();
			return;
			
/*			
		case gs_join:
			join_run();
			return;

		case gs_host:
			host_run();
			return;
			
		case gs_chooser:
			chooser_run();
			return;
			
		case gs_file:
			file_run();
			return;
			
		case gs_story:
			story_run();
			return;
			
		case gs_title:
			title_run();
			return;
			
		case gs_movie:
			movie_run();
			return;
		*/
		case gs_error:
			error_close();
			return;
		/*	

		case gs_score_limit:
			score_limit_run();
			return;
*/
	}
}

void loop_state_next_open(void)
{
	switch (server.state) {
/*	
		case gs_running:
			loop_game_run();
			return;
*/
		case gs_intro:
			intro_open();
			return;

		case gs_setup_game:
			setup_game_open();
			return;
			
		case gs_setup_network:
			setup_network_open();
			return;
			
	/*		
		case gs_join:
			join_run();
			return;

		case gs_host:
			host_run();
			return;
			
		case gs_chooser:
			chooser_run();
			return;
			
		case gs_file:
			file_run();
			return;
			
		case gs_story:
			story_run();
			return;
			
		case gs_title:
			title_run();
			return;
			
		case gs_movie:
			movie_run();
			return;
	*/
		case gs_error:
			error_open();
			return;
	/*		

		case gs_score_limit:
			score_limit_run();
			return;
*/			
	}
}

/* =======================================================

      Main App Loop Run
      
======================================================= */

bool loop_main(char *err_str)
{
		// check for app activation changes

	loop_app_active();
	
		// calculate timing
		
	game_time_calculate();
	
		// clear all triggers
		
	story_trigger_clear();
	title_trigger_clear();
	chooser_trigger_clear();
	movie_trigger_clear();
	file_trigger_clear();
	
		// clear map changes
	
	map_clear_changes();
		
		// run proper game state
		
	server.next_state=server.state;
	
	loop_state_run();
	
		// switching states
		
	if (server.state!=server.next_state) {
		loop_state_last_close();
		
		server.last_state=server.state;
		server.state=server.next_state;

		loop_state_next_open();
	}
	
		// map changes
		
	if (server.state==gs_running) {
		if (map_need_rebuild()) {
			if (!map_rebuild_changes(err_str)) return(FALSE);			// bad map changes is a fatal error
		}
	}
	
		// check all triggers
		
	story_trigger_check();
	title_trigger_check();
	chooser_trigger_check();
	movie_trigger_check();
	file_trigger_check();
	
	return(TRUE);
}
