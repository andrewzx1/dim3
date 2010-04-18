/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Network Score Limit

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

#define SCORE_LIMIT_SECOND_PAUSE		15

#include "objects.h"
#include "remotes.h"
#include "scripts.h"
#include "interfaces.h"
#include "video.h"
#include "inputs.h"
#include "network.h"
#include "timing.h"

extern bool					game_loop_quit;

extern server_type			server;
extern map_type				map;
extern js_type				js;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;

int							score_limit_start_tick;

extern void game_reset(void);
extern void remote_host_exit(void);

/* =======================================================

      Start/End Score Limits
      
======================================================= */

void score_limit_start(void)
{
	server.state=gs_score_limit;
	score_limit_start_tick=game_time_get_raw();

	game_time_pause_start();
}

void score_limit_end(void)
{
	game_time_pause_end();
	
	server.state=gs_running;
}

/* =======================================================

      Check for Score Limits
      
======================================================= */

void score_limit_push_remotes(void)
{
	int					n;
	obj_type			*obj;
	
		// push score limit to all remotes

	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {
		if (obj->type_idx==object_type_remote) net_host_player_send_message_others(obj->remote.uid,net_action_request_game_score_limit,net_player_uid_host,NULL,0);
		obj++;
	}
}

void score_limit_check_scores(void)
{
	int				n,limit,red_score,blue_score;
	obj_type		*obj;

		// only check score limits
		// during network games on the host

	if ((net_setup.mode!=net_mode_host) && (net_setup.mode!=net_mode_host_dedicated)) return;

		// is limit already on?

	if (server.state==gs_score_limit) return;

		// get score limit
		
	limit=setup.network.score_limit;
	if (limit==0) return;

		// team checks?
		
	if (hud.net_game.games[net_setup.game_idx].use_teams) {
		
		red_score=blue_score=0;

		obj=server.objs;

		for (n=0;n!=server.count.obj;n++) {
		
			if ((obj->type_idx!=object_type_player) && (obj->type_idx!=object_type_remote) && (obj->type_idx!=object_type_bot_multiplayer)) {
				obj++;
				continue;
			}
			
			if (obj->team_idx==net_team_red) {
				red_score+=obj->score.score;
				if (red_score>=limit) {
					score_limit_start();
					score_limit_push_remotes();
					return;
				}
			}
			else {
				blue_score+=obj->score.score;
				if (blue_score>=limit) {
					score_limit_start();
					score_limit_push_remotes();
					return;
				}
			}
			
			obj++;
		}
		
		return;
	}
	
		// regular player checks

	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {
		if ((obj->type_idx==object_type_player) || (obj->type_idx==object_type_remote) || (obj->type_idx==object_type_bot_multiplayer)) {
			if (obj->score.score>=limit) {
				score_limit_start();
				score_limit_push_remotes();
				return;
			}
		}

		obj++;
	}
}

/* =======================================================

      Run Score Limit
      
======================================================= */

void score_limit_run(void)
{
	gl_frame_clear(FALSE);
	view_draw_next_vertex_object_2D_color_quad(&hud.color.dialog_background,1.0f,0,hud.scale_x,0,hud.scale_y);
	network_score_draw();
	gl_frame_swap();

		// cancel by either timeout or menu key

	if ((game_time_get_raw()>(score_limit_start_tick+(SCORE_LIMIT_SECOND_PAUSE*1000))) || (input_action_get_state_single(nc_menu))) {
		score_limit_end();
		game_reset();
	}
}

