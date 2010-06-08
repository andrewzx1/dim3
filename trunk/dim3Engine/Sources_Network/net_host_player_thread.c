/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Host Player Threads

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

      Group Synching
      
======================================================= */

void net_host_player_thread_group_synch(int remote_uid)
{
	int								n;
	network_reply_group_synch		reply_synch;

		// send a synch for all groups that were
		// ever moved

	for (n=0;n!=map.ngroup;n++) {

		if (map.groups[n].move.was_moved) {
			group_moves_synch_with_client(n,&reply_synch);
			net_host_player_send_message_single(remote_uid,net_action_reply_group_synch,(unsigned char*)&reply_synch,sizeof(network_reply_group_synch));
		}
	}
}

/* =======================================================

      Host Player Message Thread
      
======================================================= */

int net_host_player_thread(void *arg)
{
	int						remote_uid,action,msg_len;
	unsigned char			msg[net_max_msg_size];
	
		// get player uid from argument
		
	remote_uid=(int)arg;
	
		// wait for messages
		
	while (TRUE) {

			// check player queue

		if (!net_host_player_check_msg(remote_uid,&action,msg,&msg_len)) {
			usleep(host_no_data_u_wait);
			continue;
		}
		
			// route messages

		switch (action) {
		
			case net_action_request_ready:
				net_host_player_ready(remote_uid);
				break;
				
			case net_action_request_team:
				net_host_player_update_team((network_request_team*)msg);
				net_host_player_send_message_others(remote_uid,net_action_request_team,msg,msg_len);
				break;
				
			case net_action_request_leave:
				net_host_player_remove(remote_uid);
				net_host_player_send_message_others(remote_uid,net_action_request_remote_remove,NULL,0);

				remote_uid=-1;
				break;
				
			case net_action_request_remote_update:
				net_host_player_update((network_request_remote_update*)msg);
				net_host_player_send_message_others(remote_uid,net_action_request_remote_update,msg,msg_len);
				break;
				
			case net_action_request_remote_death:
			case net_action_request_remote_chat:
			case net_action_request_remote_sound:
			case net_action_request_remote_fire:
			case net_action_request_remote_pickup:
			case net_action_request_remote_click:
				net_host_player_send_message_others(remote_uid,action,msg,msg_len);
				break;

			case net_action_request_latency_ping:
				net_host_player_send_message_single(remote_uid,net_action_reply_latency_ping,NULL,0);
				break;

			case net_action_request_group_synch:
				net_host_player_thread_group_synch(remote_uid);
				break;

		}

			// if remote_uid is reset to -1
			// then remote has exited

		if (remote_uid==-1) break;
	}
	
	return(0);
}


