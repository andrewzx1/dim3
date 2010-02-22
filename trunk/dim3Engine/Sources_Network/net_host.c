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

extern int					net_host_player_count;

extern server_type			server;
extern network_setup_type	net_setup;
extern hud_type				hud;

d3socket					host_socket;
bool						host_complete;
char						host_err_str[256];
SDL_Thread					*host_thread;

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

      Host Networking Listener Thread
      
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
	
	net_sendto_msg(host_socket,ip_addr,port,net_action_reply_info,-1,(unsigned char*)&info,sizeof(network_reply_info));
}

/* =======================================================

      Host Networking Listener Thread
      
======================================================= */

int net_host_thread(void *arg)
{
	int					action,net_node_uid,port,len,err;
	unsigned long		ip_addr;
	unsigned char		data[net_max_msg_size];
	d3socket			sock;
	socklen_t			addr_in_len;
	struct sockaddr_in	addr_in;
	network_header		*head;
	SDL_Thread			*message_thread;
	
		// use host err_str to flag if errors occured
		
	host_err_str[0]=0x0;
	
		// create host socket
		
	host_socket=net_open_udp_socket();
	if (host_socket==D3_NULL_SOCKET) {
		strcpy(host_err_str,"Networking: Unable to open socket");
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
		// we'll let it block until we have real data
	
	net_socket_blocking(host_socket,TRUE);
	
	while (TRUE) {
	
			// get message
			// false return = host shutting down
			
		if (!net_recvfrom_mesage(host_socket,&ip_addr,&port,&action,&net_node_uid,data)) break;
		
			// reply to all info request
			
		if (action==net_action_request_info) {
			net_host_info_request(ip_addr,port);
			continue;
		}
		
			// is there a player thread for this?
			
		// if not, make a thread, otherwise drop in proper queue
		
	
/*
		sock=accept(host_socket,&addr,&addr_len);
		if (sock==-1) break;				// accept ending means socket has been closed and host shutting down
		
			// spawn thread for client
			
		net_socket_blocking(sock,FALSE);
		
		message_thread=SDL_CreateThread(net_host_client_handler_thread,(void*)sock);
		if (message_thread==NULL) net_close_socket(&sock);
		*/
	}
	
	return(0);
}

/* =======================================================

      Start and Stop Hosted Game
      
======================================================= */

bool net_host_game_start(char *err_str)
{
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
	
	if (!net_host_broadcast_initialize(err_str)) {
		net_host_shutdown();
		net_host_player_shutdown();
		return(FALSE);
	}

	return(TRUE);
}

void net_host_game_end(void)
{
		// inform all player of server shutdown

	net_host_player_send_all_packet(net_action_request_host_exit,NULL,0);

		// shutdown server

	net_host_broadcast_shutdown();
	net_host_shutdown();
	
	net_host_player_shutdown();
}

