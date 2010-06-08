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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "network.h"

extern char team_colors[][16];

extern server_type		server;
extern hud_type			hud;

int						net_host_player_count,net_host_next_uid;
net_host_player_type	net_host_players[host_max_remote_count];

SDL_mutex				*net_host_player_lock;

/* =======================================================

      Initialize Players
      
======================================================= */

void net_host_player_initialize(void)
{
	net_host_player_count=0;
	net_host_next_uid=net_player_uid_client_start;
	
	net_host_player_lock=SDL_CreateMutex();
}

void net_host_player_shutdown(void)
{
	SDL_DestroyMutex(net_host_player_lock);
}

/* =======================================================

      Find Players
      
======================================================= */

int net_host_player_find(int player_uid)
{
	int						n;
	net_host_player_type	*player;
	
	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {
		if (player->connect.uid==player_uid) return(n);
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

		// players start in a non-ready state

	player->connect.ready=FALSE;

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
	player->connect.uid=net_host_next_uid;

		// initialize the queue

	net_queue_initialize(&player->queue);

		// settings

	player->score=0;
	strcpy(player->name,name);
	strcpy(player->draw_name,draw_name);
	player->team_idx=net_team_none;
	player->tint_color_idx=tint_color_idx;
	
	player->pnt.x=player->pnt.y=player->pnt.z=0;
	
	net_host_next_uid++;
	net_host_player_count++;
	
		// unlock player operations

	SDL_mutexV(net_host_player_lock);
	
	return(player->connect.uid);
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
	player->connect.uid=net_host_next_uid;

		// bots don't use queues

	net_queue_initialize_empty(&player->queue);

		// settings

	player->score=obj->score.score;
	
	strcpy(player->name,obj->name);
	strcpy(player->draw_name,obj->draw.name);
	
	player->team_idx=obj->team_idx;
	player->tint_color_idx=0;
	
	memmove(&player->pnt,&obj->pnt,sizeof(d3pnt));
	
		// bots are automatically ready

	player->connect.ready=TRUE;
	
	net_host_next_uid++;
	net_host_player_count++;

		// unlock player operations
	
	SDL_mutexV(net_host_player_lock);
	
	return(player->connect.uid);
}

void net_host_player_remove(int player_uid)
{
	int						idx;
	char					name[name_str_len];
	net_host_player_type	*player;
	
		// lock all player operations
		
	SDL_mutexP(net_host_player_lock);
		
		// find player
		
	idx=net_host_player_find(player_uid);
	if (idx==-1) {
		SDL_mutexV(net_host_player_lock);
		return;
	}
	
	player=&net_host_players[idx];

		// shutdown the queue

	net_queue_shutdown(&player->queue);

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

      Player Ready State
      
======================================================= */

void net_host_player_ready(int player_uid)
{
	int				idx;
	
		// lock all player operations
		
	SDL_mutexP(net_host_player_lock);
		
		// set player ready state
		
	idx=net_host_player_find(player_uid);
	if (idx!=-1) net_host_players[idx].connect.ready=TRUE;
	
		// unlock player operation
		
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

void net_host_player_update_team(network_request_team *team)
{
	int							remote_obj_uid,idx;
	net_host_player_type		*player;
	
	remote_obj_uid=(signed short)ntohs(team->remote_obj_uid);
	
	SDL_mutexP(net_host_player_lock);

	idx=net_host_player_find(remote_obj_uid);
	if (idx==-1) {
		SDL_mutexV(net_host_player_lock);
		return;
	}

		// update team

	player=&net_host_players[idx];
	player->team_idx=(signed short)ntohs(team->team_idx);

	SDL_mutexV(net_host_player_lock);
}

void net_host_player_update(network_request_remote_update *update)
{
	int							remote_obj_uid,idx,score;
	bool						score_update;
	net_host_player_type		*player;

	remote_obj_uid=(signed short)ntohs(update->remote_obj_uid);
	score=(signed short)ntohs(update->score);
	
		// update score
		
	SDL_mutexP(net_host_player_lock);

	idx=net_host_player_find(remote_obj_uid);
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

      Build Remote List for Join Replies
      
======================================================= */

void net_host_player_create_remote_list(int player_uid,network_reply_join_remotes *remotes)
{
	int							n,cnt;
	net_host_player_type		*player;
	network_request_object_add	*obj_add;

		// find all remotes and bots
	
	cnt=0;
	obj_add=remotes->objects;
	
		// set up the player remotes

	SDL_mutexP(net_host_player_lock);

	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {

		if (player->connect.uid!=player_uid) {

			obj_add->player_uid=htons((short)player->connect.uid);
			strcpy(obj_add->name,player->name);
			strcpy(obj_add->draw_name,player->draw_name);
			obj_add->bot=htons((short)(player->connect.bot?1:0));
			obj_add->team_idx=htons((short)player->team_idx);
			obj_add->tint_color_idx=htons((short)player->tint_color_idx);
			obj_add->score=htons((short)player->score);
			obj_add->pnt_x=htonl(player->pnt.x);
			obj_add->pnt_y=htonl(player->pnt.y);
			obj_add->pnt_z=htonl(player->pnt.z);

			obj_add++;
			cnt++;
		}
		
		player++;
	}
	
	SDL_mutexV(net_host_player_lock);

		// finish with the count

	remotes->count=htons((short)cnt);
}

/* =======================================================

      Message Routing
      
======================================================= */

void net_host_player_route_msg(int player_uid,int action,unsigned char *msg,int msg_len)
{
	int				idx;

		// lock all player operations
		
	SDL_mutexP(net_host_player_lock);

		// find player

	idx=net_host_player_find(player_uid);
	if (idx==-1) {
		SDL_mutexV(net_host_player_lock);
		return;
	}
		// put on queue

	net_queue_push_message(&net_host_players[idx].queue,action,player_uid,msg,msg_len);

		// unlock player operation

	SDL_mutexV(net_host_player_lock);
}

bool net_host_player_check_msg(int player_uid,int *action,unsigned char *msg,int *msg_len)
{
	int				idx,queue_player_uid;
	bool			has_msg;

		// lock all player operations
		
	SDL_mutexP(net_host_player_lock);

		// find player

	idx=net_host_player_find(player_uid);
	if (idx==-1) {
		SDL_mutexV(net_host_player_lock);
		return(FALSE);
	}
		// read from queue

	has_msg=net_queue_check_message(&net_host_players[idx].queue,action,&queue_player_uid,msg,msg_len);

		// unlock player operation

	SDL_mutexV(net_host_player_lock);

	return(has_msg);
}

/* =======================================================

      Player Thread
      
======================================================= */

void net_host_player_start_thread(int player_uid)
{
	SDL_CreateThread(net_host_client_handler_thread,(void*)player_uid);
}

/* =======================================================

      Send Messages to Other or All Players
      
======================================================= */

void net_host_player_send_message_single(int to_player_uid,int action,int from_player_uid,unsigned char *msg,int msg_len)
{
	int						idx;
	net_host_player_type	*player;

	SDL_mutexP(net_host_player_lock);

		// find player

	idx=net_host_player_find(to_player_uid);
	if (idx!=-1) {
		SDL_mutexV(net_host_player_lock);
		return;
	}

	player=&net_host_players[idx];

		// send message
		// only players in a ready state can
		// get messages.  bots never get messages

	if ((player->connect.ready) && (!player->connect.bot)) {
		if (player->connect.local) {
			net_client_push_queue_local(action,from_player_uid,msg,msg_len);
		}
		else {
			net_sendto_msg(player->connect.sock,player->connect.ip_addr,player->connect.port,action,from_player_uid,msg,msg_len);
		}
	}

	SDL_mutexV(net_host_player_lock);
}

void net_host_player_send_message_others(int skip_player_uid,int action,int from_player_uid,unsigned char *msg,int msg_len)
{
	int						n;
	net_host_player_type	*player;
	
	SDL_mutexP(net_host_player_lock);
	
	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {

			// only players in a ready state can
			// get messages.  bots never get messages

		if ((player->connect.uid!=skip_player_uid) && (player->connect.ready) && (!player->connect.bot)) {
			if (player->connect.local) {
				net_client_push_queue_local(action,from_player_uid,msg,msg_len);
			}
			else {
				net_sendto_msg(player->connect.sock,player->connect.ip_addr,player->connect.port,action,from_player_uid,msg,msg_len);
			}
		}

		player++;
	}
	
	SDL_mutexV(net_host_player_lock);
}

void net_host_player_send_message_all(int action,int from_player_uid,unsigned char *msg,int msg_len)
{
	net_host_player_send_message_others(-1,action,from_player_uid,msg,msg_len);
}
