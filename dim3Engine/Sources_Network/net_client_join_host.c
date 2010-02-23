/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Client Join Host

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
#include "interfaces.h"
#include "network.h"

extern map_type				map;
extern server_type			server;
extern js_type				js;
extern hud_type				hud;
extern setup_type			setup;

extern bool					join_thread_quit;
extern d3socket				client_socket;

network_setup_type			net_setup;

/* =======================================================

      Find Game From Name
      
======================================================= */

int net_client_find_game(char *game_name)
{
	int			n;

	for (n=0;n!=hud.net_game.ngame;n++) {
		if (strcasecmp(hud.net_game.games[n].name,game_name)==0) return(n);
	}

	return(-1);
}

/* =======================================================

      Join and Exit Host
      
======================================================= */

int net_client_join_host_start(char *ip,char *name,char *game_name,char *map_name,int *tick_offset,int *option_flags,char *deny_reason,network_reply_join_remotes *remotes)
{
	int						player_uid,len;
	unsigned char			data[net_max_msg_size];
	char					err_str[256];
	network_header			head;
	network_request_join	request_join;
	network_reply_join		reply_join;
	
		// create client socket
		
	client_socket=net_open_udp_socket();
	if (client_socket==D3_NULL_SOCKET) {
		strcpy(err_str,"Networking: Unable to open socket");
		return(-1);
	}

		// connect
/* don't connect!		
	if (!net_connect(client_socket,ip,net_port_host,client_timeout_wait_seconds,err_str)) {
		strcpy(deny_reason,"Host Unreachable (No connection)");
		net_close_socket(&client_socket);
		return(-1);
	}
*/	
		// send join request
		
	strcpy(request_join.name,name);
	strcpy(request_join.vers,dim3_version);
	request_join.hash=ntohl(net_get_project_hash());
	request_join.tint_color_idx=(signed short)ntohs((short)setup.network.tint_color_idx);
	request_join.character_idx=(signed short)ntohs((short)setup.network.character_idx);

	// supergumba
//	net_sendto_msg(d3socket sock,unsigned long ip_addr,int port,int action,int player_uid,unsigned char *msg,int msg_len)

	net_send_message(client_socket,net_action_request_join,net_player_uid_none,(unsigned char*)&request_join,sizeof(network_request_join));

		// get the reply
	
	net_socket_blocking(client_socket,TRUE);
	len=recv(client_socket,data,net_max_msg_size,0);
	if (len<=0) {
		strcpy(deny_reason,"Host Unreachable (Bad Read)");
		net_close_socket(&client_socket);
		return(-1);
	}

		// right message?

	memmove(&head,data,sizeof(network_header));
	if (head.action!=net_action_reply_join) {
		strcpy(deny_reason,"Host Unreachable (Bad Return)");
		net_close_socket(&client_socket);
		return(-1);
	}

	memmove(&reply_join,(data+sizeof(network_header)),sizeof(network_reply_join));
	
		// check for denial
		
	player_uid=(int)ntohs(reply_join.player_uid);

	if (player_uid==-1) {
		strcpy(deny_reason,reply_join.deny_reason);
		net_close_socket(&client_socket);
		return(FALSE);
	}
	
		// finish setup
		
	strcpy(game_name,reply_join.game_name);
	strcpy(map_name,reply_join.map_name);
	
	*tick_offset=ntohl(reply_join.map_tick);
	*option_flags=ntohl(reply_join.option_flags);

		// additional objects

	remotes->count=(int)ntohs((short)reply_join.remotes.count);
	if (remotes->count!=0) memmove(remotes->objects,reply_join.remotes.objects,(sizeof(network_request_object_add)*remotes->count));
	
	return(player_uid);
}

void net_client_join_host_end(void)
{
	net_close_socket(&client_socket);
}
