/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Main Host Code

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

#include "objects.h"
#include "remotes.h"
#include "network.h"
#include "consoles.h"
#include "timing.h"

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern hud_type				hud;

d3socket					host_socket;
bool						host_complete;
char						host_err_str[256];
SDL_Thread					*host_thread;

int net_host_thread(void *arg);		// forward reference for main thread

/* =======================================================

      Initialize and Shutdown Host Networking
      
======================================================= */

bool net_host_initialize(char *err_str)
{
		// begin listener thread
		
	host_complete=FALSE;
	host_socket=D3_NULL_SOCKET;
	
	host_thread=SDL_CreateThread(net_host_thread,NULL);
	if (host_thread==NULL) {
		strcpy(err_str,"Networking: Could not start host listener thread");
		return(FALSE);
	}
	
		// wait for listenter thread to get into accept
		
	while (TRUE) {
		if (host_complete) break;
		usleep(100000);
	}
	
	if (host_err_str[0]!=0x0) {
		strcpy(err_str,host_err_str);
		return(FALSE);
	}

		// now listening on separate thread
		
	return(TRUE);
}

void net_host_shutdown(void)
{
		// did host never start?

	if (host_socket==D3_NULL_SOCKET) return;
	
		// shutdown socket and then wait for termination
		
	net_close_socket(&host_socket);
	SDL_WaitThread(host_thread,NULL);
}

/* =======================================================

      Machine UIDs
      
======================================================= */

int net_host_create_machine_uid(void)
{
	int			uid;

	uid=net_setup.uid.next_machine_uid;
	net_setup.uid.next_machine_uid++;

	return(uid);
}

/* =======================================================

      Join Local Player to Host
      
======================================================= */

bool net_host_join_local_player(char *err_str)
{
	obj_type					*player_obj;
	network_reply_join_remote	remote;

		// get local player

	player_obj=server.obj_list.objs[server.player_obj_idx];

		// join directly to host

	player_obj->remote.uid=net_host_player_add(-1,-1,TRUE,net_setup.uid.machine_uid,player_obj->name,player_obj->draw.name,player_obj->tint_color_idx);
	if (player_obj->remote.uid==-1) {
		strcpy(err_str,"Unable to add player");
		return(FALSE);
	}

		// send all other players on host the new player for remote add

	strncpy(remote.name,player_obj->name,name_str_len);
	strncpy(remote.draw_name,player_obj->draw.name,name_str_len);
	remote.name[name_str_len-1]=0x0;
	remote.team_idx=htons((short)net_team_none);
	remote.tint_color_idx=htons((short)player_obj->tint_color_idx);
	remote.score=0;
	remote.pnt_x=remote.pnt_y=remote.pnt_z=0;

	net_host_player_send_message_others(player_obj->remote.uid,net_action_request_remote_add,(unsigned char*)&remote,sizeof(network_reply_join_remote));

	return(TRUE);
}

/* =======================================================

      Join Multiplayer Bots to a Host
      
======================================================= */

void net_host_join_multiplayer_bots(void)
{
	int				n;
	char			deny_reason[256],err_str[256];
	obj_type		*obj;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

			// can this bot join the game?

		if (obj->type!=object_type_bot_multiplayer) continue;
		if (!net_host_player_add_ok(obj->name,deny_reason)) {
			snprintf(err_str,256,"%s: %s",obj->name,deny_reason);
			err_str[255]=0x0;
			console_add(err_str);
			continue;
		}

			// add bot

		obj->remote.uid=net_host_player_add_bot(obj);
	}
}

/* =======================================================

      Host Info Reply
      
======================================================= */

void net_host_info_request(unsigned long ip_addr,int port)
{
	network_reply_info		info;
	
	strcpy(info.host_name,net_setup.host.name);
	strcpy(info.host_ip_resolve,net_setup.host.ip_resolve);

	strcpy(info.proj_name,hud.proj_name);
	strcpy(info.game_name,hud.net_game.games[net_setup.game_idx].name);
	strcpy(info.map_name,map.info.name);

	info.option_flags=htonl(net_setup.option_flags);

	net_host_player_create_info_player_list(&info.player_list);

	net_sendto_msg(host_socket,ip_addr,port,net_action_reply_info,net_player_uid_host,(unsigned char*)&info,sizeof(network_reply_info));
}

/* =======================================================

      Join Host Reply
      
======================================================= */

bool net_host_join_request_ok(network_request_join *request_join,network_reply_join *reply_join)
{
		// refuse join if hash is different

	if (htonl(request_join->hash)!=net_get_project_hash()) {
		sprintf(reply_join->deny_reason,"Client game data has been modified");
		return(FALSE);
	}

		// refuse join if versions are different

	if (strncmp(request_join->vers,dim3_version,name_str_len)!=0) {
		sprintf(reply_join->deny_reason,"Client and host version differ (%s; %s)",request_join->vers,dim3_version);
		return(FALSE);
	}

		// name conflict or host full?

	return(net_host_player_add_ok(request_join->name,reply_join->deny_reason));
}

int net_host_join_request(unsigned long ip_addr,int port,network_request_join *request_join)
{
	int							machine_uid,remote_uid,
								tint_color_idx;
	obj_type					*obj;
	network_reply_join			reply_join;
	network_reply_join_remote	remote;

		// check if join is OK
	
	machine_uid=-1;
	remote_uid=-1;

	reply_join.deny_reason[0]=0x0;

		// create machine and remote UID when
		// adding this new remote player

	if (net_host_join_request_ok(request_join,&reply_join)) {

		machine_uid=net_host_create_machine_uid();

		tint_color_idx=htons((short)request_join->tint_color_idx);
		remote_uid=net_host_player_add(ip_addr,port,FALSE,machine_uid,request_join->name,request_join->draw_name,tint_color_idx);
	}

		// construct the reply
	
	reply_join.machine_uid=htons((short)machine_uid);
	reply_join.remote_uid=htons((short)remote_uid);

	reply_join.team_idx=htons((short)net_team_none);
	reply_join.map_tick=htonl(game_time_get()-map.start_game_tick);
	
	if (remote_uid!=-1) {
		net_host_player_create_join_remote_list(remote_uid,&reply_join.remote_list);
	}
	else {
		reply_join.remote_list.count=htons(0);
	}

		// build a remote add request for other
		// clients in the game

		// so we can add a remote, we start with a fake
		// team (none), after we add, we can call the game
		// rules and reset it

	remote.remote_uid=htons((short)remote_uid);
	strncpy(remote.name,request_join->name,name_str_len);
	strncpy(remote.draw_name,request_join->draw_name,name_str_len);
	remote.name[name_str_len-1]=0x0;
	remote.team_idx=htons((short)net_team_none);
	remote.tint_color_idx=htons((short)tint_color_idx);
	remote.score=0;
	remote.pnt_x=remote.pnt_y=remote.pnt_z=0;

		// create the remote object

	if (!remote_add(&remote,TRUE)) {
		remote_uid=-1;
	}

		// run team rule

	if (remote_uid!=-1) {
		obj=object_find_remote_uid(remote_uid);
		object_run_game_rules(obj);

			// reset team in reply and remote add

		reply_join.team_idx=htons((short)obj->team_idx);
		remote.team_idx=htons((short)obj->team_idx);
	}
	
		// send reply back to client

	if (!net_sendto_msg(host_socket,ip_addr,port,net_action_reply_join,net_player_uid_host,(unsigned char*)&reply_join,sizeof(network_reply_join))) {
		if (remote_uid!=-1) net_host_player_remove(remote_uid);
		return(FALSE);
	}
	
		// if no player uid, then player was rejected
		
	if (remote_uid==-1) return(FALSE);

		// start host thread to get and deal with
		// messages from this remote

	net_host_player_remote_start_thread(remote_uid);
	
		// send all other players on host the new player for remote add

	net_host_player_send_message_others(remote_uid,net_action_request_remote_add,(unsigned char*)&remote,sizeof(network_reply_join_remote));

	return(remote_uid);
}

/* =======================================================

      Host Networking Main Thread
      
======================================================= */

int net_host_thread(void *arg)
{
	int					action,player_uid,port,msg_len;
	unsigned long		ip_addr;
	unsigned char		msg[net_max_msg_size];
	
		// use host err_str to flag if errors occured
		
	host_err_str[0]=0x0;
	
		// create host socket
		
	host_socket=net_open_udp_socket();
	if (host_socket==D3_NULL_SOCKET) {
		strcpy(host_err_str,"Hosting: Unable to open socket");
		host_complete=TRUE;
		return(0);
	}
		
		// bind -- use any IP on this machine to
		// get traffic

	if (!net_bind_any(host_socket,net_port_host,host_err_str)) {
		net_close_socket(&host_socket);
		host_complete=TRUE;
		return(0);
	}

		// host is OK, free thread to run independantly
		
	host_complete=TRUE;
	
		// begin waiting for messages
		// we'll let the socket block for us
	
	net_socket_blocking(host_socket,TRUE);
	
	while (TRUE) {
	
			// get message
			// false return = host shutting down
			
		if (!net_recvfrom_mesage(host_socket,&ip_addr,&port,&action,&player_uid,msg,&msg_len)) break;
		
			// reply to all info request
			
		if (action==net_action_request_info) {
			net_host_info_request(ip_addr,port);
			continue;
		}

			// reply to all join requests

		if (action==net_action_request_join) {
			net_host_join_request(ip_addr,port,(network_request_join*)msg);
			continue;
		}
		
			// all other requests are routed to
			// player queues

		net_host_player_remote_route_msg(player_uid,action,msg,msg_len);
	}
	
	return(0);
}

/* =======================================================

      Start and Stop Hosted Game
      
======================================================= */

void net_host_game_setup(void)
{
		// reset UIDs

	net_setup.uid.next_machine_uid=0;
	net_setup.uid.next_remote_uid=0;
	
		// resolve names to IPs
		
	if (net_setup.host.name[0]==0x0) net_get_host_name(net_setup.host.name);
	net_get_host_ip(net_setup.host.ip_name,net_setup.host.ip_resolve);
	
		// setup hosting flags

	net_setup.mode=setup.network.dedicated?net_mode_host_dedicated:net_mode_host;
	net_setup.client.latency=0;
	net_setup.client.host_ip_addr=0;
}

bool net_host_game_start(char *err_str)
{
		// start hosting

	if (!net_host_initialize(err_str)) return(FALSE);

		// give the host a machine uid

	net_setup.uid.machine_uid=net_host_create_machine_uid();

		// initialize host player list
		
	net_host_player_initialize();

	return(TRUE);
}

void net_host_game_end(void)
{
		// inform all player of server shutdown

	net_host_player_send_message_all(net_action_request_host_exit,NULL,0);

		// shutdown server

	net_host_shutdown();
	net_host_player_shutdown();
	
		// make sure hosting flags are off
		
	net_setup.mode=net_mode_none;
}

