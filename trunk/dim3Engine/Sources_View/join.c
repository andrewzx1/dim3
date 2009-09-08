/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Network Join

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
#include "remotes.h"
#include "interfaces.h"
#include "video.h"
#include "sounds.h"

#define join_pane_news					0
#define join_pane_lan					1
#define join_pane_internet				2

#define join_tab_id						0

#define join_button_join_id				1
#define join_button_cancel_id			2
#define join_button_rescan_id			3
#define join_table_id					4
#define join_status_id					5

#define join_news_id					6

extern void intro_open(void);
extern bool game_start(int skill,network_reply_join_remotes *remotes,char *err_str);
extern bool map_start(bool skip_media,char *err_str);

extern map_type				map;
extern server_type			server;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern int					client_timeout_values[];

int							join_tab_value;
char						*join_table_data;
bool						join_thread_started,join_thread_quit;
SDL_Thread					*join_thread,*join_thread_accept;
SDL_mutex					*join_thread_lock;

int							join_count,join_start_tick_local;
join_server_info			join_list[max_setup_network_host];
			
/* =======================================================

      Build Join List
      
======================================================= */

char* join_create_list(void)
{
	int					n,sz;
	char				*c,*row_data;
	join_server_info	*info;
	
		// create row data
		
	sz=(join_count+1)*128;
	
	row_data=malloc(sz);
	bzero(row_data,sz);
	
		// build table
	
	info=join_list;
	c=row_data;
	
	for (n=0;n!=join_count;n++) {
		snprintf(c,128,"Maps;%s;%s\t%s @ %s\t%d/%d\t%dms",info->map_name,info->name,info->game_name,info->map_name,info->player_count,info->player_max_count,info->ping_msec);
		c[127]=0x0;
		
		c+=128;
		info++;
	}
	
	return(row_data);
}

void join_ping_thread_done(void)
{
	element_table_busy(join_table_id,-1,-1);
	element_enable(join_button_rescan_id,TRUE);
}

/* =======================================================

      Join Ping LAN Threads
      
======================================================= */

int join_ping_thread_lan_accept_read(void *arg)
{
	int					msec;
	unsigned char		data[net_max_msg_size];
	char				*row_data;
	network_reply_info	*reply_info;
	d3socket			sock;
	join_server_info	*info;

	msec=time_get()-join_start_tick_local;
	
		// read reply and add to list
		
	sock=(d3socket)arg;
	
	net_socket_blocking(sock,TRUE);
		
		// wait for data
	
	if (net_queue_block_single_message(sock,-1,0,NULL,0,net_action_reply_info,data,sizeof(network_reply_info))) {
		
		reply_info=(network_reply_info*)data;
		
			// is it the same dim3 project?

		if (strcasecmp(reply_info->proj_name,hud.proj_name)==0) {
	
			SDL_mutexP(join_thread_lock);

				// setup info

			info=&join_list[join_count++];

			strcpy(info->name,reply_info->host_name);
			strcpy(info->ip,reply_info->host_ip_resolve);
			strcpy(info->game_name,reply_info->game_name);
			strcpy(info->map_name,reply_info->map_name);
			info->local=TRUE;
			info->player_count=(int)ntohs(reply_info->player_count);
			info->player_max_count=(int)ntohs(reply_info->player_max_count);
			info->ping_msec=msec;

				// rebuild list

			row_data=join_create_list();
			element_set_table_data(join_table_id,row_data);
			free(row_data);
			
			SDL_mutexV(join_thread_lock);
		}
	}
	
	net_close_socket(&sock);

	return(0);
}

int join_ping_thread_lan_accept(void *arg)
{
	d3socket			broadcast_reply_sock,sock;
	socklen_t			addr_len;
	struct sockaddr		addr;
	
	broadcast_reply_sock=(d3socket)arg;

		// wait for server replies
		
	addr_len=sizeof(struct sockaddr);
	
	while (TRUE) {
		sock=accept(broadcast_reply_sock,&addr,&addr_len);
		if (sock==-1) break;				// accept ending means socket has been closed and host shutting down
		
			// spawn a thread to deal with replies
			
		join_thread_accept=SDL_CreateThread(join_ping_thread_lan_accept_read,(void*)sock);
	}

	return(0);
}

int join_ping_thread_lan(void *arg)
{
	int			count,count_tenth_sec;
	char		ip_name[256],ip_resolve[256],err_str[256];
	d3socket	broadcast_reply_sock;
	
	join_start_tick_local=time_get();
	
	net_get_host_ip(ip_name,ip_resolve);
	
		// spawn another thread to wait for replies
		// we do this on another thread so we can
		// cancel it under a certain time limit
		// by closing the socket

	broadcast_reply_sock=net_open_socket();
	net_socket_blocking(broadcast_reply_sock,TRUE);

	if (!net_bind(broadcast_reply_sock,ip_resolve,net_port_host_broadcast_reply,err_str)) {
		join_ping_thread_done();
		return(0);
	}
	
	if (listen(broadcast_reply_sock,32)!=0) {
		net_close_socket(&broadcast_reply_sock);
		join_ping_thread_done();
		return(0);
	}

		// run thread and wait for accepts to start
		
	join_thread_accept=SDL_CreateThread(join_ping_thread_lan_accept,(void*)broadcast_reply_sock);
	
		// send broadcast to all network nodes
		// asking for connections from hosts
		
	join_start_tick_local=time_get();
		
	if (net_udp_send_broadcast(ip_resolve,net_port_host_broadcast)) {
	
		count=0;
		count_tenth_sec=client_timeout_wait_seconds*10;
		
		while ((count<count_tenth_sec) && (!join_thread_quit)) {
		
			usleep(100000);
			count++;
			
			element_table_busy(join_table_id,count,count_tenth_sec);
		}
		
	}
	
	element_table_busy(join_table_id,1,1);
	
		// reset the UI and buttons
		
	join_ping_thread_done();
	
		// close the socket to cancel the accept and
		// join the thread to wait for end
		
	net_close_socket(&broadcast_reply_sock);
	SDL_WaitThread(join_thread_accept,NULL);

	return(0);
}

/* =======================================================

      Join Ping Internet Threads
      
======================================================= */

int join_ping_thread_internet(void *arg)
{
	int							idx,player_count,player_max_count,ping_msec;
	char						status[32],host_name[name_str_len],
								proj_name[name_str_len],game_name[name_str_len],map_name[name_str_len];
	char						*row_data;
	setup_network_host_type		*host;
	join_server_info			*info;
	
		// run ping until finished or
		// join page ends

	idx=0;

	while ((!join_thread_quit) && (idx<setup.network.host.count)) {

			// ping host

		host=&setup.network.host.hosts[idx];

		if (net_client_ping_host(host->ip,status,host_name,proj_name,game_name,map_name,&player_count,&player_max_count,&ping_msec)) {

				// is this reply the same dim3 project?
				
			if (strcasecmp(proj_name,hud.proj_name)==0) {

				SDL_mutexP(join_thread_lock);
				
					// setup info

				info=&join_list[join_count++];
			
				strcpy(info->name,host_name);
				strcpy(info->ip,host->ip);
				strcpy(info->game_name,game_name);
				strcpy(info->map_name,map_name);
				info->local=FALSE;
				info->player_count=player_count;
				info->player_max_count=player_max_count;
				info->ping_msec=ping_msec;

					// rebuild list

				row_data=join_create_list();
				element_set_table_data(join_table_id,row_data);
				free(row_data);
				
				SDL_mutexV(join_thread_lock);
			}
		}
		
		idx++;
	}
	
		// reset the UI and buttons
		
	join_ping_thread_done();
	
	return(0);
}

/* =======================================================

      Join Ping Threads Mainline
      
======================================================= */

void join_ping_thread_start(void)
{
		// empty join list
		
	join_count=0;
	join_thread_quit=FALSE;
	
		// table is busy
		
	element_set_table_data(join_table_id,NULL);
	element_table_busy(join_table_id,0,1);
	
		// table update locks
		
	join_thread_lock=SDL_CreateMutex();

		// start pinging hosts
		
	join_thread_started=TRUE;
	
	if (element_get_value(join_tab_id)==join_pane_lan) {
		join_thread=SDL_CreateThread(join_ping_thread_lan,NULL);
	}
	else {
		join_thread=SDL_CreateThread(join_ping_thread_internet,NULL);
	}
}

void join_ping_thread_end(void)
{
	if (!join_thread_started) return;
	
	join_thread_quit=TRUE;
	
	SDL_WaitThread(join_thread,NULL);
	
	SDL_DestroyMutex(join_thread_lock);
}

/* =======================================================

      Join Panes
      
======================================================= */

void join_news_pane(void)
{
	int			x,y,wid,high;
	
		// show and hide proper elements
		
	element_hide(join_button_rescan_id,TRUE);
	element_enable(join_button_join_id,FALSE);
	element_text_change(join_status_id,"");

		// news
		
	x=(int)(((float)hud.scale_x)*0.03f);
	y=(int)(((float)hud.scale_y)*0.15f);

	wid=hud.scale_x-(x*2);
	high=(int)(((float)hud.scale_y)*0.83f)-y;

	element_text_box_add(net_get_news(),join_news_id,x,y,wid,high);
}

void join_lan_internet_pane(bool lan)
{
	int						x,y,wid,high;
	element_column_type		cols[4];

		// show and hide proper elements
		
	element_hide(join_button_rescan_id,FALSE);
	element_enable(join_button_rescan_id,FALSE);
	element_enable(join_button_join_id,FALSE);
	element_text_change(join_status_id,"");
	
		// hosts table
		
	x=(int)(((float)hud.scale_x)*0.03f);
	y=(int)(((float)hud.scale_y)*0.15f);

	wid=hud.scale_x-(x*2);
	high=(int)(((float)hud.scale_y)*0.83f)-y;

	strcpy(cols[0].name,"Name");
	cols[0].percent_size=0.45f;
	strcpy(cols[1].name,"Game");
	cols[1].percent_size=0.37f;
	strcpy(cols[2].name,"Players");
	cols[2].percent_size=0.10f;
	strcpy(cols[3].name,"Ping");
	cols[3].percent_size=0.8f;

	element_table_add(cols,NULL,join_table_id,4,x,y,wid,high,element_table_bitmap_data);
	element_table_busy(join_table_id,0,1);

		// start the thread to build the table
		
	join_ping_thread_start();
}

void join_create_pane(void)
{
	int						x,y,yadd,wid,high,padding,tab_idx,pane,
							tab_list_wid,tab_pane_high;
	char					tab_list[][32]={"News","LAN","Internet"};
	
		// turn off any scanning threads
		
	join_ping_thread_end();

		// controls

	element_clear();
	
		// tabs
		
	padding=element_get_padding();
	
	wid=hud.scale_x;
	yadd=(int)(((float)hud.scale_y)*0.015f);
	high=(int)(((float)hud.scale_y)*0.065f);
	tab_list_wid=(int)(((float)hud.scale_x)*0.85f);
	tab_pane_high=(int)(((float)hud.scale_y)*0.82f);
	
	tab_idx=0;
	if (hud.net_news.host[0]==0x0) {
		tab_idx=1;
		if (join_tab_value==join_pane_news) join_tab_value=join_pane_lan;
	}
	
	element_tab_add((char*)(tab_list[tab_idx]),join_tab_value,join_tab_id,(3-tab_idx),0,(padding+yadd),wid,high,tab_list_wid,tab_pane_high);

		// status

	padding=element_get_padding();
	high=(int)(((float)hud.scale_x)*0.05f);
		
	y=hud.scale_y-((padding+(high/2))-(element_get_control_high()/2));
	element_text_add("",join_status_id,15,y,hud.font.text_size_small,tx_left,FALSE,FALSE);
	
		// buttons
		
	x=padding;
	y=hud.scale_y-padding;
	
	wid=(int)(((float)hud.scale_x)*0.2f);

	element_button_text_add("Rescan Hosts",join_button_rescan_id,x,y,wid,high,element_pos_left,element_pos_bottom);
	
	x=hud.scale_x-padding;
	wid=(int)(((float)hud.scale_x)*0.1f);

	element_button_text_add("Join",join_button_join_id,x,y,wid,high,element_pos_right,element_pos_bottom);

	x=element_get_x_position(join_button_join_id)-padding;

	element_button_text_add("Cancel",join_button_cancel_id,x,y,wid,high,element_pos_right,element_pos_bottom);

		// specific pane controls
		
	pane=element_get_value(join_tab_id);
		
	switch (pane) {
		case join_pane_news:
			join_news_pane();
			break;
		case join_pane_lan:
			join_lan_internet_pane(TRUE);
			break;
		case join_pane_internet:
			join_lan_internet_pane(FALSE);
			break;
	}
}

/* =======================================================

      Join Operations
      
======================================================= */

void join_open(bool local)
{
		// get the project hash and news

	net_create_project_hash();
	net_load_news();
	
		// setup gui
		
	gui_initialize("Bitmaps/Backgrounds","setup",FALSE);
	
		// start with first tab
		
	join_tab_value=join_pane_news;
	join_thread_started=FALSE;
	
		// create panes
		
	join_create_pane();

		// in join thread
		
	server.state=gs_join;
}

void join_close(bool stop_music)
{
	if (stop_music) {
		if (al_music_playing()) al_music_stop();
	}

	join_ping_thread_end();
	gui_shutdown();
}

/* =======================================================

      Enabling
	        
======================================================= */

void join_activity_start(void)
{
	element_enable(join_button_join_id,FALSE);
	element_enable(join_button_cancel_id,FALSE);
	
	gui_draw(1.0f,FALSE);
}

void join_activity_complete(bool single,char *msg)
{
	element_enable(join_button_join_id,single);
	element_enable(join_button_cancel_id,TRUE);
	
	if (!single) element_set_value(join_table_id,-1);
	if (msg!=NULL) element_text_change(join_status_id,msg);
	
	gui_draw(1.0f,FALSE);
}

/* =======================================================

      Join Game
      
======================================================= */

void join_game(void)
{
	int							idx,remote_uid,tick_offset,option_flags;
	char						game_name[name_str_len],map_name[name_str_len],
								deny_reason[64],err_str[256];
	network_reply_join_remotes	remotes;
	
		// get game to join
		
	join_activity_start();
	
	idx=element_get_value(join_table_id);
	strcpy(net_setup.client.joined_ip,join_list[idx].ip);
							
		// status
		
	element_text_change(join_status_id,"Joining ...");
	gui_draw(1.0f,FALSE);
							
		// attempt to join

	if (!net_client_join_host_start(net_setup.client.joined_ip,setup.network.name,&remote_uid,game_name,map_name,&tick_offset,&option_flags,deny_reason,&remotes)) {
		join_close(TRUE);
		sprintf(err_str,"Unable to Join Game: %s",deny_reason);
		error_open(err_str,"Network Game Canceled");
		return;
	}

		// mark remote and joined
		
	net_setup.host.hosting=FALSE;
	
	net_setup.client.joined=TRUE;
	net_setup.client.latency=0;
	
		// setup game type

	net_setup.game_idx=net_client_find_game(game_name);
	if (net_setup.game_idx==-1) {
		net_client_send_leave_host(remote_uid);
		net_client_join_host_end();
		sprintf(err_str,"Could not find game type: %s",game_name);
		error_open(err_str,"Network Game Canceled");
		return;
	}
	
	map.info.name[0]=0x0;
	strcpy(map.info.host_name,map_name);
	
	net_setup.option_flags=option_flags;
	
		// start game
	
	join_close(TRUE);
	
	if (!game_start(skill_medium,&remotes,err_str)) {
		net_client_send_leave_host(remote_uid);
		net_client_join_host_end();
		error_open(err_str,"Network Game Canceled");
		return;
	}
	
		// start the map
		
	if (!map_start(TRUE,err_str)) {
		net_client_send_leave_host(remote_uid);
		net_client_join_host_end();
		error_open(err_str,"Network Game Canceled");
		return;
	}
	
	object_player_set_remote_uid(remote_uid);
	
		// start client network thread
		
	if (!net_client_start_message_queue(err_str)) {
		net_client_send_leave_host(remote_uid);
		net_client_join_host_end();
		error_open(err_str,"Network Game Canceled");
		return;
	}

		// mark as ready to receive data from host

	net_client_send_ready(remote_uid);

		// request moving group synchs

	net_client_request_group_synch_ping(remote_uid);
	
		// game is running
	
	server.state=gs_running;
}

/* =======================================================

      Join Input
      
======================================================= */

void join_click(void)
{
	int			id;
	bool		enable;
	
		// is element being clicked
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
		// run selection

	switch (id) {
			
		case join_tab_id:
			join_tab_value=element_get_value(join_tab_id);
			join_create_pane();
			return;
	
		case join_button_join_id:
			join_game();
			break;
			
		case join_button_cancel_id:
			join_close(FALSE);
			intro_open();
			break;
			
		case join_button_rescan_id:
			element_enable(join_button_rescan_id,FALSE);
			join_ping_thread_end();
			join_ping_thread_start();
			break;

		case join_table_id:
			enable=(element_get_value(join_table_id)!=-1);
			element_enable(join_button_join_id,enable);
			break;
	}
}
	
/* =======================================================

      Run Join
      
======================================================= */

void join_run(void)
{
	gui_draw(1.0f,TRUE);
	join_click();
}

