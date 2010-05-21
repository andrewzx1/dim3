/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Game Start/Stop

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "network.h"
#include "objects.h"
#include "projectiles.h"
#include "effects.h"
#include "consoles.h"
#include "interfaces.h"
#include "scripts.h"

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern void game_time_pause_start(void);
extern void game_time_pause_end(void);
extern bool server_game_start(char *game_script_name,int skill,network_reply_join_remotes *remotes,char *err_str);
extern void server_game_stop(void);
extern void view_game_start(void);
extern void view_game_stop(void);
extern void net_host_game_end(void);
extern bool map_rebuild_changes(char *err_str);

/* =======================================================

      Game Start and End
      
======================================================= */

bool game_start(int skill,network_reply_join_remotes *remotes,char *err_str)
{
		// pause time
		
	game_time_pause_start();
	
		// reset random numbers
		
	random_reset();
	
		// start progress
	
	console_add_system("Starting Game");

		// start server
	
	if (!server_game_start("Game",skill,remotes,err_str)) return(FALSE);

		// start view
		
	view_game_start();

		// game in running state
		
	server.game_open=TRUE;
	server.state=gs_running;
	
	game_time_pause_end();
	
	return(TRUE);
}

void game_end(void)
{
	console_add_system("Closing Game");
	
		// close any network joins or hosting
	
	switch (net_setup.mode) {
		case net_mode_client:
			net_client_send_leave_host();
			net_client_end_message_queue();
			net_client_join_host_end();
			break;
		case net_mode_host:
		case net_mode_host_dedicated:
			net_host_game_end();
			net_client_end_message_queue_local();
			break;
	}
	
		// stop view
		
	view_game_stop();

		// stop server
		
	server_game_stop();
	
		// game closed
		
	server.game_open=FALSE;
}

/* =======================================================

      Game Reset
      
======================================================= */

void game_reset(void)
{
	int							n;
	char						err_str[256];
	obj_type					*obj,*player_obj;
	network_request_game_reset	reset;
	
		// switch to next map

	net_setup.host.current_map_idx++;
	if (net_setup.host.current_map_idx>=setup.network.map.count) net_setup.host.current_map_idx=0;

	strcpy(map.info.name,setup.network.map.maps[net_setup.host.current_map_idx].name);
	map.info.player_start_name[0]=0x0;

	if (!map_rebuild_changes(err_str)) {
		game_end();
		error_setup(err_str,"Hosting Game Canceled");
		server.next_state=gs_error;
		return;
	}

		// all objects will be re-spawned when the new map
		// loads.  We setup the next spawn to be a game
		// reset and clear the scores
		
		// remotes get hidden (until the next update)
		// the respawn just to update any lists or items
		// before the client sends in it's next packet
		
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
	
		switch (obj->type) {
		
			case object_type_player:
			case object_type_bot_multiplayer:
				object_score_reset(obj);
				obj->next_spawn_sub_event=sd_event_spawn_game_reset;
				break;
				
			case object_type_remote:
				object_score_reset(obj);
				obj->next_spawn_sub_event=sd_event_spawn_game_reset;
				obj->hidden=TRUE;
				break;
				
		}
	}

		// signal remotes to reset

	strcpy(reset.map_name,map.info.name);

	player_obj=object_find_uid(server.player_obj_uid);
	net_host_player_send_message_others(player_obj->remote.uid,net_action_request_game_reset,net_player_uid_host,(unsigned char*)&reset,sizeof(network_request_game_reset));
}



