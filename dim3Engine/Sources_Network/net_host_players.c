/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Host Player Lists

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

extern char team_colors[][16];

extern map_type				map;
extern server_type			server;
extern network_setup_type	net_setup;

int							net_host_player_count;
net_host_player_type		net_host_players[host_max_remote_count];

SDL_mutex					*net_host_player_lock;

/* =======================================================

      Initialize Players
      
======================================================= */

void net_host_player_initialize(void)
{
	net_host_player_count=0;
	
	net_host_player_lock=SDL_CreateMutex();
}

void net_host_player_shutdown(void)
{
	SDL_DestroyMutex(net_host_player_lock);
}

/* =======================================================

      Find Players
      
======================================================= */

int net_host_player_find_net_uid(int net_uid)
{
	int						n;
	net_host_player_type	*player;
	
	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {
		if (player->connect.net_uid==net_uid) return(n);
		player++;
	}
	
	return(-1);
}

int net_host_player_find_ip_addr(unsigned long ip_addr,int port)
{
	int						n;
	net_host_player_type	*player;
	
	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {
		if ((player->connect.ip_addr==ip_addr) && (player->connect.port==port)) return(n);
		player++;
	}
	
	return(-1);
}

/* =======================================================

      Player Add OK?
      
======================================================= */

bool net_host_player_add_ok(char *name,char *deny_reason)
{
	int						n;
	net_host_player_type	*player;

		// host full?
		
	if (net_host_player_count==host_max_remote_count) {
		strcpy(deny_reason,"Host is full");
		return(FALSE);
	}

		// lock all player operations
		
	SDL_mutexP(net_host_player_lock);

		// id in use?

	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {
		if (strcasecmp(player->name,name)==0) {
			sprintf(deny_reason,"Player name '%s' already in use",name);
			SDL_mutexV(net_host_player_lock);
			return(FALSE);
		}
		player++;
	}

		// unlock player operation

	SDL_mutexV(net_host_player_lock);

	return(TRUE);
}

/* =======================================================

      Add and Remove Players
      
======================================================= */

int net_host_player_add(unsigned long ip_addr,int port,bool local,char *name,char *draw_name,int tint_color_idx)
{
	net_host_player_type		*player;

		// lock all player operations
		
	SDL_mutexP(net_host_player_lock);
	
		// create new player
		
	player=&net_host_players[net_host_player_count];

		// connections
		// includes a personal socket for sending to other clients

	player->connect.sock=net_open_udp_socket();
	if (player->connect.sock==D3_NULL_SOCKET) {
		SDL_mutexV(net_host_player_lock);
		return(-1);
	}

	player->connect.ip_addr=ip_addr;
	player->connect.port=port;
	player->connect.local=local;
	player->connect.bot=FALSE;

	player->connect.net_uid=net_setup.next_net_uid;
	net_setup.next_net_uid++;

		// settings

	player->score=0;
	strcpy(player->name,name);
	strcpy(player->draw_name,draw_name);
	player->team_idx=net_team_none;
	player->tint_color_idx=tint_color_idx;
	
	player->pnt.x=player->pnt.y=player->pnt.z=0;
	
	net_host_player_count++;
	
		// unlock player operations

	SDL_mutexV(net_host_player_lock);
	
	return(player->connect.net_uid);
}

int net_host_player_add_bot(obj_type *obj)
{
	net_host_player_type	*player;

		// lock all player operations
		
	SDL_mutexP(net_host_player_lock);

		// create new player
		
	player=&net_host_players[net_host_player_count];

		// bots have no connections

	player->connect.sock=D3_NULL_SOCKET;
	player->connect.ip_addr=-1;
	player->connect.port=-1;
	player->connect.local=TRUE;
	player->connect.bot=TRUE;

	player->connect.net_uid=net_setup.next_net_uid;
	net_setup.next_net_uid++;

		// settings

	player->score=obj->score.score;
	
	strcpy(player->name,obj->name);
	strcpy(player->draw_name,obj->draw.name);
	
	player->team_idx=obj->team_idx;
	player->tint_color_idx=0;
	
	memmove(&player->pnt,&obj->pnt,sizeof(d3pnt));
	
	net_host_player_count++;

		// unlock player operations
	
	SDL_mutexV(net_host_player_lock);
	
	return(player->connect.net_uid);
}

void net_host_player_remove(int net_uid)
{
	int						idx;
	char					name[name_str_len];
	net_host_player_type	*player;
	
		// lock all player operations
		
	SDL_mutexP(net_host_player_lock);
		
		// find player
		
	idx=net_host_player_find_net_uid(net_uid);
	if (idx==-1) {
		SDL_mutexV(net_host_player_lock);
		return;
	}
	
	player=&net_host_players[idx];

		// close the socket

	if (player->connect.sock!=D3_NULL_SOCKET) net_close_socket(&player->connect.sock);

		// remember name

	strcpy(name,player->name);

		// exit player
	
	if (idx<(net_host_player_count-1)) {
		memmove(&net_host_players[idx],&net_host_players[idx+1],(sizeof(net_host_player_type)*((net_host_player_count-idx)-1)));
	}
	
	net_host_player_count--;
	
	SDL_mutexV(net_host_player_lock);
}

/* =======================================================

      Count Teams
      
======================================================= */

int net_host_player_count_team(int team_idx)
{
	int						n,count;
	net_host_player_type	*player;
	
	count=0;
	
	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {
		if (player->team_idx==team_idx) count++;
		player++;
	}
	
	return(count);
}

/* =======================================================

      Update Player Information
      
======================================================= */

void net_host_player_update(int net_uid,network_request_remote_update *update)
{
	int							idx,score;
	bool						score_update;
	net_host_player_type		*player;

	score=(signed short)ntohs(update->score);
	
		// update score
		
	SDL_mutexP(net_host_player_lock);

	idx=net_host_player_find_net_uid(net_uid);
	if (idx==-1) {
		SDL_mutexV(net_host_player_lock);
		return;
	}

	player=&net_host_players[idx];
	
		// position
		
	player->pnt.x=ntohl(update->pnt_x);
	player->pnt.y=ntohl(update->pnt_y);
	player->pnt.z=ntohl(update->pnt_z);
		
		// score
	
	score_update=(score!=player->score);
	player->score=score;
	
	SDL_mutexV(net_host_player_lock);
}

/* =======================================================

      Moveable Group Synching
      
======================================================= */

void net_host_player_remote_group_synch(int net_uid)
{
	int								n;
	network_reply_group_synch		reply_synch;

		// send a synch for all groups that were
		// ever moved

	for (n=0;n!=map.group.ngroup;n++) {

		if (map.group.groups[n].run.was_moved) {
			group_moves_synch_with_client(n,&reply_synch);
			net_host_player_send_message_single(net_uid,net_action_reply_group_synch,(unsigned char*)&reply_synch,sizeof(network_reply_group_synch));
		}
	}
}

/* =======================================================

      Build Lists for Info/Join Replies
      
======================================================= */

void net_host_player_create_join_remote_list(int net_uid,network_reply_join_remote_list *remote_list)
{
	int							n,cnt;
	net_host_player_type		*player;
	network_reply_join_remote	*remote;

		// find all remotes and bots
	
	cnt=0;
	
		// set up the player remotes

	SDL_mutexP(net_host_player_lock);

	player=net_host_players;
	remote=remote_list->remotes;
	
	for (n=0;n!=net_host_player_count;n++) {

		if (player->connect.net_uid!=net_uid) {

			remote->net_uid=htons((short)player->connect.net_uid);
			strcpy(remote->name,player->name);
			strcpy(remote->draw_name,player->draw_name);
			remote->bot=htons((short)(player->connect.bot?1:0));
			remote->team_idx=htons((short)player->team_idx);
			remote->tint_color_idx=htons((short)player->tint_color_idx);
			remote->score=htons((short)player->score);
			remote->pnt_x=htonl(player->pnt.x);
			remote->pnt_y=htonl(player->pnt.y);
			remote->pnt_z=htonl(player->pnt.z);

			remote++;
			cnt++;
		}
		
		player++;
	}
	
	SDL_mutexV(net_host_player_lock);

		// finish with the count

	remote_list->count=htons((short)cnt);
}

void net_host_player_create_info_player_list(network_reply_info_player_list *player_list)
{
	int							n;
	net_host_player_type		*player;
	network_reply_info_player	*info_player;

		// set up the player info

	SDL_mutexP(net_host_player_lock);

	player=net_host_players;
	info_player=player_list->players;
	
	for (n=0;n!=net_host_player_count;n++) {
		strcpy(info_player->name,player->name);
		info_player->score=htons((short)player->score);
		info_player->bot=htons((short)(player->connect.bot?1:0));

		info_player++;
		player++;
	}

		// finish with the count

	player_list->count=htons((short)net_host_player_count);
	player_list->max_count=htons((short)host_max_remote_count);

	SDL_mutexV(net_host_player_lock);
}

/* =======================================================

      Route Remote Messages to Host
      
======================================================= */

void net_host_player_remote_route_msg(net_queue_msg_type *msg)
{
	switch (msg->action) {
	
		case net_action_request_leave:
			net_host_player_remove(msg->net_uid);
			net_host_player_send_message_others(msg->net_uid,net_action_request_remote_remove,NULL,0);
			break;
			
		case net_action_request_remote_update:
			net_host_player_update(msg->net_uid,(network_request_remote_update*)msg->msg);
			net_host_player_send_message_others(msg->net_uid,net_action_request_remote_update,msg->msg,msg->msg_len);
			break;
			
		case net_action_request_remote_death:
		case net_action_request_remote_chat:
		case net_action_request_remote_sound:
		case net_action_request_remote_fire:
		case net_action_request_remote_pickup:
		case net_action_request_remote_click:
			net_host_player_send_message_others(msg->net_uid,msg->action,msg->msg,msg->msg_len);
			break;

		case net_action_request_latency_ping:
			net_host_player_send_message_single(msg->net_uid,net_action_reply_latency_ping,NULL,0);
			break;

		case net_action_request_group_synch:
			net_host_player_remote_group_synch(msg->net_uid);
			break;

	}
		
			// since this is the host, we need to pass on
			// the messages to the remotes on the host
			
	if (net_setup.mode!=net_mode_host_dedicated) remote_route_message(msg);
}

/* =======================================================

      Send Messages to Other or All Players
      
======================================================= */

void net_host_player_send_message_single(int net_uid,int action,unsigned char *msg,int msg_len)
{
	int						idx;
	net_host_player_type	*player;

	SDL_mutexP(net_host_player_lock);

		// find player

	idx=net_host_player_find_net_uid(net_uid);
	if (idx!=-1) {
		SDL_mutexV(net_host_player_lock);
		return;
	}

	player=&net_host_players[idx];

		// send message

	net_sendto_msg(player->connect.sock,player->connect.ip_addr,player->connect.port,action,net_uid,msg,msg_len);

	SDL_mutexV(net_host_player_lock);
}

void net_host_player_send_message_others(int net_uid,int action,unsigned char *msg,int msg_len)
{
	int						n,idx;
	net_host_player_type	*player;
	
	SDL_mutexP(net_host_player_lock);

		// find sending player

	if (net_uid==-1) {
		idx=-1;
	}
	else {
		idx=net_host_player_find_net_uid(net_uid);
		if (idx==-1) {
			SDL_mutexV(net_host_player_lock);
			return;
		}
	}
	
		// send to others
	
	for (n=0;n!=net_host_player_count;n++) {

			// skip person sending the message

		if (n==idx) continue;

			// skip the machine it's running on (usually
			// the same as skipping the player but will
			// be different when the host relays messages
			// for bots)

		player=&net_host_players[n];

			// bots never get messages,

		if (player->connect.bot) continue;

			// send to network
		
		net_sendto_msg(player->connect.sock,player->connect.ip_addr,player->connect.port,action,net_uid,msg,msg_len);
	}
	
	SDL_mutexV(net_host_player_lock);
}

void net_host_player_send_message_all(int action,unsigned char *msg,int msg_len)
{
	net_host_player_send_message_others(-1,action,msg,msg_len);
}
