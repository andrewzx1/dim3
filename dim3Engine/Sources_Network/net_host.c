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

#include "network.h"
#include "timing.h"

extern int					net_host_player_count;

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

      Add Bots to a Host
      
======================================================= */

void net_host_player_add_bots(void)
{
	int				n;
	char			deny_reason[256];
	obj_type		*obj;

	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {
		if (obj->type_idx==object_type_bot_multiplayer) {
			if (net_host_player_add_ok(obj->name,deny_reason)) {
				obj->remote.uid=net_host_player_add_bot(obj);
			}
		}
		obj++;
	}
}

/* =======================================================

      Host Info Responses
      
======================================================= */

void net_host_info_request(unsigned long ip_addr,int port)
{
	network_reply_info		info;
	
	info.player_count=htons((short)net_host_player_count);
	info.player_max_count=htons((short)host_max_remote_count);
	strcpy(info.host_name,net_setup.host.name);
	strcpy(info.host_ip_resolve,net_setup.host.ip_resolve);
	strcpy(info.proj_name,hud.proj_name);
	strcpy(info.game_name,hud.net_game.games[net_setup.game_idx].name);
	strcpy(info.map_name,net_setup.host.map_name);
	
	net_sendto_msg(host_socket,ip_addr,port,net_action_reply_info,net_player_uid_host,(unsigned char*)&info,sizeof(network_reply_info));
}

/* =======================================================

      Host Player Join Responses
      
======================================================= */

int net_host_join_request(unsigned long ip_addr,int port,network_request_join *request_join)
{
	int							player_uid,
								tint_color_idx,character_idx;
	bool						allow;
	network_reply_join			reply_join;
	network_request_object_add	remote_add;

		// refuse join if hash is different

	allow=TRUE;

	if (htonl(request_join->hash)!=net_get_project_hash()) {
		allow=FALSE;
		sprintf(reply_join.deny_reason,"Project files have been modified");
	}

		// refuse join if versions are different

	if (strncmp(request_join->vers,dim3_version,name_str_len)!=0) {
		allow=FALSE;
		sprintf(reply_join.deny_reason,"Client version (%s) differs from Host version (%s)",request_join->vers,dim3_version);
	}

		// name conflict or host full?

	if (!net_host_player_add_ok(request_join->name,reply_join.deny_reason)) {
		allow=FALSE;
	}

		// join to host
	
	player_uid=-1;

	if (allow) {
		tint_color_idx=htons((short)request_join->tint_color_idx);
		character_idx=htons((short)request_join->character_idx);
		player_uid=net_host_player_add(ip_addr,port,FALSE,request_join->name,tint_color_idx,character_idx);
	}

		// construct the reply
	
	strcpy(reply_join.game_name,hud.net_game.games[net_setup.game_idx].name);
	strcpy(reply_join.map_name,net_setup.host.map_name);
	reply_join.map_tick=htonl(game_time_get()-map.start_game_tick);
	reply_join.option_flags=htonl(net_setup.option_flags);
	reply_join.player_uid=htons((short)player_uid);
	
	if (player_uid!=-1) {
		net_host_player_create_remote_list(player_uid,&reply_join.remotes);
	}
	else {
		reply_join.remotes.count=htons(0);
	}
	
		// send reply

	net_sendto_msg(host_socket,ip_addr,port,net_action_reply_join,net_player_uid_host,(unsigned char*)&reply_join,sizeof(network_reply_join));
	
		// send all other players on host the new player for remote add
		
	if (player_uid!=-1) {
		remote_add.player_uid=htons((short)player_uid);
		strncpy(remote_add.name,request_join->name,name_str_len);
		remote_add.name[name_str_len-1]=0x0;
		remote_add.team_idx=htons((short)net_team_none);
		remote_add.tint_color_idx=request_join->tint_color_idx;		// already in network byte order
		remote_add.character_idx=request_join->character_idx;		// already in network byte order
		remote_add.score=0;
		remote_add.pnt_x=remote_add.pnt_y=remote_add.pnt_z=0;

		net_host_player_send_message_others(player_uid,net_action_request_remote_add,net_player_uid_host,(unsigned char*)&remote_add,sizeof(network_request_object_add));
	}

	return(player_uid);
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
		
		// bind

	if (!net_bind(host_socket,net_setup.host.ip_resolve,net_port_host,host_err_str)) {
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
			player_uid=net_host_join_request(ip_addr,port,(network_request_join*)msg);
			if (player_uid!=-1) net_host_player_start_thread(player_uid);
			continue;
		}
		
			// all other requests are routed to
			// player queues

		net_host_player_route_msg(player_uid,action,msg,msg_len);
	}
	
	return(0);
}

/* =======================================================

      Start and Stop Hosted Game
      
======================================================= */

bool net_host_game_start(char *err_str)
{
	int						player_uid;
	network_request_join	request_join;

		// initialize players
		
	net_host_player_initialize();
	
		// resolve names to IPs
		
	if (net_setup.host.name[0]==0x0) net_get_host_name(net_setup.host.name);
	net_get_host_ip(net_setup.host.ip_name,net_setup.host.ip_resolve);

		// start hosting

	if (!net_host_initialize(err_str)) {
		net_host_player_shutdown();
		return(FALSE);
	}
	
		// attempt to add local player to host

	player_uid=-1;

	if (!setup.network.dedicated) {
		strcpy(request_join.name,setup.network.name);
		strcpy(request_join.vers,dim3_version);
		request_join.tint_color_idx=(signed short)ntohs((short)setup.network.tint_color_idx);
		request_join.character_idx=(signed short)ntohs((short)setup.network.character_idx);

		player_uid=net_host_client_handle_local_join(&request_join,err_str);
		if (player_uid==-1) {
			net_host_shutdown();
			net_host_player_shutdown();
			return(FALSE);
		}
	}
	
		// setup hosting flags

	if (setup.network.dedicated) {
		net_setup.mode=net_mode_host_dedicated;
	}
	else {
		net_setup.mode=net_mode_host;
	}

	net_setup.player_uid=player_uid;

	net_setup.client.latency=0;
	net_setup.client.host_ip_addr=0;

	return(TRUE);
}

void net_host_game_end(void)
{
		// inform all player of server shutdown

	net_host_player_send_message_all(net_action_request_host_exit,net_player_uid_host,NULL,0);

		// shutdown server

	net_host_shutdown();
	net_host_player_shutdown();
	
		// make sure hosting flags are off
		
	net_setup.mode=net_mode_none;
}

