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

int						net_host_player_count,server_next_remote_uid;
net_host_player_type	net_host_players[host_max_remote_count];

SDL_mutex				*net_host_player_lock;

/* =======================================================

      Initialize Players
      
======================================================= */

void net_host_player_initialize(void)
{
	net_host_player_count=0;
	server_next_remote_uid=net_remote_uid_client_start;
	
	net_host_player_lock=SDL_CreateMutex();
}

void net_host_player_shutdown(void)
{
	SDL_DestroyMutex(net_host_player_lock);
}

/* =======================================================

      Find Players
      
======================================================= */

int net_host_player_find(int remote_uid)
{
	int						n;
	net_host_player_type	*player;
	
	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {
		if (player->remote_uid==remote_uid) return(n);
		player++;
	}
	
	return(-1);
}

bool net_host_player_find_name(int remote_uid,char *name)
{
	int				idx;
	bool			hit;
	
	hit=FALSE;

	SDL_mutexP(net_host_player_lock);

	idx=net_host_player_find(remote_uid);
	
	if (idx!=-1) {
		strcpy(name,net_host_players[idx].name);
		hit=TRUE;
	}
	
	SDL_mutexV(net_host_player_lock);
	
	return(hit);
}

/* =======================================================

      Join and Leave Players
      
======================================================= */

int net_host_player_join(d3socket sock,char *name,int tint_color_idx,int character_idx,char *deny_reason)
{
	int						n,remote_uid;
	net_host_player_type	*player;

		// lock all player operations
		
	SDL_mutexP(net_host_player_lock);
	
		// host full?
		
	if (net_host_player_count==host_max_remote_count) {
		strcpy(deny_reason,"Server is full");
		SDL_mutexV(net_host_player_lock);
		return(-1);
	}

		// id in use?

	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {
		if (strcasecmp(player->name,name)==0) {
			sprintf(deny_reason,"Player name '%s' already in use",name);
			SDL_mutexV(net_host_player_lock);
			return(-1);
		}
		player++;
	}
	
		// create new player
		
	player=&net_host_players[net_host_player_count];
	
	player->remote_uid=server_next_remote_uid;
	player->sock=sock;
	player->score=0;
	strcpy(player->name,name);
	player->team_idx=net_team_none;
	player->tint_color_idx=tint_color_idx;
	player->character_idx=character_idx;

	player->bot=FALSE;
	
	player->pnt.x=player->pnt.y=player->pnt.z=0;
	
	player->ready=FALSE;
	
	server_next_remote_uid++;
	net_host_player_count++;
	
	remote_uid=player->remote_uid;
	
	SDL_mutexV(net_host_player_lock);
	
	return(remote_uid);
}

void net_host_player_ready(int remote_uid,bool ready)
{
	int				idx;
	
		// lock all player operations
		
	SDL_mutexP(net_host_player_lock);
		
		// set player ready state
		
	idx=net_host_player_find(remote_uid);
	if (idx!=-1) net_host_players[idx].ready=ready;
	
		// unlock player operation
		
	SDL_mutexV(net_host_player_lock);
}

void net_host_player_leave(int remote_uid)
{
	int				idx;
	char			name[name_str_len];
	
		// lock all player operations
		
	SDL_mutexP(net_host_player_lock);
		
		// find player
		
	idx=net_host_player_find(remote_uid);
	if (idx==-1) {
		SDL_mutexV(net_host_player_lock);
		return;
	}

		// remember name

	strcpy(name,net_host_players[idx].name);

		// exit player
	
	if (idx<(net_host_player_count-1)) {
		memmove(&net_host_players[idx],&net_host_players[idx+1],(sizeof(net_host_player_type)*((net_host_player_count-idx)-1)));
	}
	
	net_host_player_count--;
	
	SDL_mutexV(net_host_player_lock);
}

/* =======================================================

      Join Bots
      
======================================================= */

int net_host_bot_join(obj_type *obj)
{
	int						remote_uid;
	net_host_player_type	*player;

		// lock all player operations
		
	SDL_mutexP(net_host_player_lock);
	
		// host full?
		
	if (net_host_player_count==host_max_remote_count) {
		SDL_mutexV(net_host_player_lock);
		return(-1);
	}

		// create new player
		
	player=&net_host_players[net_host_player_count];
	
	player->remote_uid=server_next_remote_uid;
	player->sock=D3_NULL_SOCKET;
	player->score=obj->score.score;
	strcpy(player->name,obj->name);
	player->team_idx=obj->team_idx;
	player->tint_color_idx=0;
	player->character_idx=0;
	
	player->bot=TRUE;
	
	memmove(&player->pnt,&obj->pnt,sizeof(d3pnt));
	
	player->ready=TRUE;		// bots are automatically ready
	
	server_next_remote_uid++;
	net_host_player_count++;
	
	remote_uid=player->remote_uid;
	
	SDL_mutexV(net_host_player_lock);
	
	return(remote_uid);
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

void net_host_player_update_team(int remote_uid,network_request_team *team)
{
	int							idx;
	net_host_player_type		*player;
	
	SDL_mutexP(net_host_player_lock);

	idx=net_host_player_find(remote_uid);
	if (idx==-1) {
		SDL_mutexV(net_host_player_lock);
		return;
	}

		// update team

	player=&net_host_players[idx];
	player->team_idx=(signed short)ntohs(team->team_idx);

	SDL_mutexV(net_host_player_lock);
}

void net_host_player_update(int remote_uid,network_request_remote_update *update)
{
	int							idx,score;
	bool						score_update;
	net_host_player_type		*player;

	score=(signed short)ntohs(update->score);
	
		// update score
		
	SDL_mutexP(net_host_player_lock);

	idx=net_host_player_find(remote_uid);
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

      Add bots to player list
      
======================================================= */

void net_host_player_add_bots_to_list(void)
{
	int				n;
	obj_type		*obj;

	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {
		if (obj->type_idx==object_type_bot) obj->remote.uid=net_host_bot_join(obj);
		obj++;
	}
}

/* =======================================================

      Build Remote List for Join Replies
      
======================================================= */

void net_host_player_create_remote_list(int player_remote_uid,network_reply_join_remotes *remotes)
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

		if (player->remote_uid!=player_remote_uid) {

			obj_add->uid=htons((short)player->remote_uid);
			strcpy(obj_add->name,player->name);
			obj_add->bot=htons((short)(player->bot?1:0));
			obj_add->team_idx=htons((short)player->team_idx);
			obj_add->tint_color_idx=htons((short)player->tint_color_idx);
			obj_add->character_idx=htons((short)player->character_idx);
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

      Send Packet to Other or All Players
      
======================================================= */

void net_host_player_send_others_packet(int player_remote_uid,int action,unsigned char *data,int len)
{
	int						n,nsock;
	d3socket				socks[host_max_remote_count];
	net_host_player_type	*player;
	
		// we build the list of other player's sockets
		// inside the lock to deal with changing player list
		// we then send outside the lock so we don't hold
		// the lock too long
		
	SDL_mutexP(net_host_player_lock);
	
	nsock=0;
	player=net_host_players;
	
	for (n=0;n!=net_host_player_count;n++) {
		if ((player->remote_uid!=player_remote_uid) && (player->ready) && (!player->bot)) socks[nsock++]=player->sock;
		player++;
	}
	
	SDL_mutexV(net_host_player_lock);
	
		// send to all other players
	
	for (n=0;n!=nsock;n++) {
	
			// local (hosting) player
			
		if (socks[n]==D3_NULL_SOCKET) {
			net_client_push_queue_local(action,player_remote_uid,data,len);
		}
		
			// remote joined players
			
		else {
			if (net_send_ready(socks[n])) net_send_message(socks[n],action,player_remote_uid,data,len);
		}
	}
}

void net_host_player_send_all_packet(int action,unsigned char *data,int len)
{
	net_host_player_send_others_packet(-1,action,data,len);
}
