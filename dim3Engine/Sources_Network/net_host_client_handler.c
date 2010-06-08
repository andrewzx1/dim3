/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Host Client Message Handler Threads

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

#ifdef D3_OS_MAC
	#include <ifaddrs.h>
#endif

#include "network.h"
#include "timing.h"

extern int					net_host_player_count;

extern map_type				map;
extern hud_type				hud;
extern network_setup_type	net_setup;

extern void group_moves_synch_with_client(int group_idx,network_reply_group_synch *synch);

/* =======================================================

      Host Client Networking Message Handlers
      
======================================================= */

int net_host_client_handle_local_join(network_request_join *request_join,char *err_str)
{
	int							player_uid,
								tint_color_idx,character_idx;
	network_request_object_add	remote_add;

		// join directly to host

	tint_color_idx=htons((short)request_join->tint_color_idx);
	character_idx=htons((short)request_join->character_idx);

	player_uid=net_host_player_add(-1,-1,TRUE,request_join->name,request_join->draw_name,tint_color_idx);
	if (player_uid==-1) return(-1);

		// send all other players on host the new player for remote add
		
	strncpy(remote_add.name,request_join->name,name_str_len);
	strncpy(remote_add.draw_name,request_join->draw_name,name_str_len);
	remote_add.name[name_str_len-1]=0x0;
	remote_add.team_idx=htons((short)net_team_none);
	remote_add.tint_color_idx=htons((short)request_join->tint_color_idx);
	remote_add.score=0;
	remote_add.pnt_x=remote_add.pnt_y=remote_add.pnt_z=0;

	net_host_player_send_message_others(player_uid,net_action_request_remote_add,net_player_uid_host,(unsigned char*)&remote_add,sizeof(network_request_object_add));

	return(player_uid);
}

void net_host_client_handle_leave(int player_uid)
{
		// leave the host
		
	net_host_player_remove(player_uid);
	
		// now send all other players on host the remote remove
		
	net_host_player_send_message_others(player_uid,net_action_request_remote_remove,player_uid,NULL,0);
}

void net_host_client_handle_set_team(int player_uid,network_request_team *team)
{
	net_host_player_update_team(team);
	net_host_player_send_message_others(player_uid,net_action_request_team,player_uid,(unsigned char*)team,sizeof(network_request_team));
}

void net_host_client_handle_update(int player_uid,network_request_remote_update *update)
{
	net_host_player_update(update);
	net_host_player_send_message_others(player_uid,net_action_request_remote_update,player_uid,(unsigned char*)update,sizeof(network_request_remote_update));
}

void net_host_client_handle_group_synch(int player_uid)
{
	int								n;
	network_reply_group_synch		reply_synch;

		// send a synch for all groups that were
		// ever moved

	for (n=0;n!=map.ngroup;n++) {

		if (map.groups[n].move.was_moved) {
			group_moves_synch_with_client(n,&reply_synch);
			net_host_player_send_message_single(player_uid,net_action_reply_group_synch,net_player_uid_host,(unsigned char*)&reply_synch,sizeof(network_reply_group_synch));
		}
	}
}

/* =======================================================

      Host Client Networking Message Thread
      
======================================================= */

int net_host_client_handler_thread(void *arg)
{
	int						player_uid,action,msg_len;
	unsigned char			msg[net_max_msg_size];
	
		// get player uid from argument
		
	player_uid=(int)arg;
	
		// wait for messages
		
	while (TRUE) {

			// check player queue

		if (!net_host_player_check_msg(player_uid,&action,msg,&msg_len)) {
			usleep(host_no_data_u_wait);
			continue;
		}
		
			// route messages

		switch (action) {
		
			case net_action_request_ready:
				net_host_player_ready(player_uid);
				break;
				
			case net_action_request_team:
				net_host_client_handle_set_team(player_uid,(network_request_team*)msg);
				break;
				
			case net_action_request_leave:
				net_host_client_handle_leave(player_uid);
				player_uid=-1;
				break;
				
			case net_action_request_remote_update:
				net_host_client_handle_update(player_uid,(network_request_remote_update*)msg);
				break;
				
			case net_action_request_remote_death:
			case net_action_request_remote_chat:
			case net_action_request_remote_sound:
			case net_action_request_remote_fire:
			case net_action_request_remote_pickup:
			case net_action_request_remote_click:
				net_host_player_send_message_others(player_uid,action,player_uid,msg,msg_len);
				break;

			case net_action_request_latency_ping:
				net_host_player_send_message_single(player_uid,net_action_reply_latency_ping,net_player_uid_host,NULL,0);
				break;

			case net_action_request_group_synch:
				net_host_client_handle_group_synch(player_uid);
				break;

		}

			// if player_uid is reset to -1
			// then remote has exited

		if (player_uid==-1) break;
	}
	
	return(0);
}


