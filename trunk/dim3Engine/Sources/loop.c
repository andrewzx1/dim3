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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "network.h"
#include "objects.h"

extern bool					game_app_active;

extern map_type				map;
extern server_type			server;
extern view_type			view;
extern setup_type			setup;
extern network_setup_type	net_setup;

/* =======================================================

      Main Game Loop Run
      
======================================================= */

void loop_game_run(void)
{
		// networking

	if ((net_setup.mode==net_mode_host) || (net_setup.mode==net_mode_host_dedicated)) {
		net_host_process_messages();
	}
		
	if (net_setup.mode!=net_mode_none) {
		if (!net_client_process_messages()) return;
	}
	
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
	}

		// if state has changed to intro,
		// then we must be exiting a running
		// game
		
	if (server.next_state==gs_intro) {
		map_end();
		game_end();
		return;
	}
	
		// any other state change early exists
		
	if (server.next_state!=gs_running) return;

		// draw the view

	if (net_setup.mode==net_mode_host_dedicated) {
		view_loop_draw_dedicated_host();
	}
	else {
		view_run();
		view_loop_draw();
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
		
		SDL_PauseAudio(1);

			// don't pause if this game is a host

		if ((net_setup.mode!=net_mode_host) && (net_setup.mode!=net_mode_host_dedicated)) game_time_pause_start();

		return;
	}

		// becoming active?

	if (!input_app_active()) return;

	game_app_active=TRUE;
	
	input_clear();
	input_mouse_resume();
	
	SDL_PauseAudio(0);

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
			
		case gs_file:
			file_run();
			return;
			
		case gs_chooser:
			chooser_run();
			return;
			
		case gs_title:
			title_run();
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
	
	switch (server.state) {

		case gs_intro:
			intro_close();
			return;

		case gs_setup_game:
			setup_game_close();
			return;
			
		case gs_setup_network:
			setup_network_close();
			return;
			
		case gs_join:
			join_close();
			return;

		case gs_host:
			host_close();
			return;
			
		case gs_file:
			file_close();
			return;

		case gs_chooser:
			chooser_close();
			return;
			
		case gs_title:
			title_close();
			return;
			
		case gs_error:
			error_close();
			return;

		case gs_score_limit:
			score_limit_close();
			return;

	}
}

void loop_state_next_open(void)
{
	switch (server.state) {

		case gs_intro:
			intro_open();
			return;

		case gs_setup_game:
			setup_game_open();
			return;
			
		case gs_setup_network:
			setup_network_open();
			return;
			
		case gs_join:
			join_open();
			return;

		case gs_host:
			host_open();
			return;
			
		case gs_file:
			file_open();
			return;

		case gs_chooser:
			chooser_open();
			return;
			
		case gs_title:
			title_open();
			return;
			
		case gs_error:
			error_open();
			return;

		case gs_score_limit:
			score_limit_open();
			return;
	}
}

/* =======================================================

      Main App Loop Run
      
======================================================= */

bool loop_main(char *err_str)
{
		// pump the input
		// if there's an activation change, handle it
		
	if (input_event_pump()) {
		loop_app_active();
	}
	
		// calculate timing
		
	game_time_calculate();
	
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
		// we need to check for state changes within
		// the map change (like media launching)
		
	if (server.state==gs_running) {
		if (map_need_rebuild()) {
		
			server.next_state=gs_running;
			
			if (!map_rebuild_changes(err_str)) return(FALSE);			// bad map changes is a fatal error
			
			if (server.next_state!=gs_running) {
				server.last_state=server.state;
				server.state=server.next_state;
				loop_state_next_open();
			}
			
		}
	}
	
	return(TRUE);
}
