/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Network Join

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "network.h"
#include "objects.h"

#define join_pane_hosts					0
#define join_pane_news					1

#define join_tab_id						0

#define join_button_join_id				10
#define join_button_cancel_id			11
#define join_button_rescan_id			12

#define join_lan_table_id				20
#define join_wan_table_id				21

#define join_status_id					30

#define join_news_id					40

extern map_type				map;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;

int							join_tab_value,join_thread_lan_start_tick;
char						*join_table_data,*join_news;
bool						join_thread_started,join_thread_quit,join_run_wan;
SDL_Thread					*join_lan_thread,*join_wan_thread;

join_server_host_list_type	*join_host_lan_list,*join_host_wan_list;
			
/* =======================================================

      Build Join List
      
======================================================= */

void join_create_list(join_server_host_list_type *list,int table_id)
{
	int						n,sz;
	char					*c,*row_data;
	join_server_host_type	*host;
	
		// create row data
		
	sz=(list->count+1)*128;
	
	row_data=malloc(sz);
	bzero(row_data,sz);
	
		// build table
	
	host=list->hosts;
	c=row_data;
	
	for (n=0;n!=list->count;n++) {
		if (host->pinged) {
			if (!host->unreachable) {
				snprintf(c,128,"Bitmaps/Icons_Map;%s;%s\t%s @ %s\t%d/%d\t%dms",host->map_name,host->name,host->game_name,host->map_name,host->player_list.count,host->player_list.max_count,host->ping_msec);
			}
			else {
				snprintf(c,128,"Bitmaps/Icons_Map;;%s\tUnreachable\t--\t--",host->name);
			}
		}
		else {
			snprintf(c,128,"Bitmaps/Icons_Map;;%s\tWaiting...\t--\t--",host->name);
		}
		c[127]=0x0;
		
		c+=128;
		host++;
	}

	element_set_table_data(table_id,row_data);

	free(row_data);
}

/* =======================================================

      Add or Update Host Table
      
======================================================= */

bool join_ping_thread_update_host(join_server_host_type *host,int start_tick,network_reply_info *reply_info)
{
	int						n,cnt;
	
		// errors have NULL reply info
		
	if (reply_info==NULL) {
		host->pinged=TRUE;
		host->unreachable=TRUE;
		return(FALSE);
	}

		// only add if same project

	if (strcasecmp(reply_info->proj_name,iface.project.name)!=0) return(FALSE);

		// host

	strcpy(host->name,reply_info->host_name);
	strcpy(host->ip,reply_info->host_ip_resolve);

	strcpy(host->game_name,reply_info->game_name);
	strcpy(host->map_name,reply_info->map_name);

	host->score_limit=(signed short)ntohs(reply_info->score_limit);
	host->respawn_secs=(signed short)ntohs(reply_info->respawn_secs);
	host->game_reset_secs=(signed short)ntohs(reply_info->game_reset_secs);
	host->option_flags=ntohl(reply_info->option_flags);

		// players

	host->player_list.count=(signed short)ntohs(reply_info->player_list.count);
	host->player_list.max_count=(signed short)ntohs(reply_info->player_list.max_count);

	cnt=host->player_list.count;
	if (cnt>join_info_max_players) cnt=join_info_max_players;
	if (cnt<0) cnt=0;

	for (n=0;n!=cnt;n++) {
		strcpy(host->player_list.players[n].name,reply_info->player_list.players[n].name);
		host->player_list.players[n].bot=(((int)ntohs(reply_info->player_list.players[n].bot))!=0);
		host->player_list.players[n].score=(int)ntohs(reply_info->player_list.players[n].score);
	}

		// ping time

	host->ping_msec=time_get()-start_tick;

	host->pinged=TRUE;
	host->unreachable=FALSE;

	return(TRUE);
}

bool join_ping_thread_add_host(join_server_host_list_type *list,int start_tick,network_reply_info *reply_info)
{
	join_server_host_type	*host;

		// only add if same project

	if (strcasecmp(reply_info->proj_name,iface.project.name)!=0) return(FALSE);

		// add to list

	host=&list->hosts[list->count];

		// update

	if (join_ping_thread_update_host(host,start_tick,reply_info)) {
		list->count++;
		return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Join Ping LAN Threads
      
======================================================= */

int join_ping_thread_lan(void *arg)
{
	int					max_tick,action,net_uid;
	unsigned char		msg[net_max_msg_size];
	bool				good_reply;
	d3socket			broadcast_sock;

		// create a socket and send out a broadcast
		// to all addresses on this subnet, looking for
		// replies from a dim3 server

	broadcast_sock=net_open_udp_socket();
	if (broadcast_sock==D3_NULL_SOCKET) return(0);

	net_socket_enable_broadcast(broadcast_sock);
	
	if (!net_sendto_msg(broadcast_sock,INADDR_BROADCAST,net_port_host,net_action_request_info,net_uid_constant_none,NULL,0)) {
		net_close_socket(&broadcast_sock);
		return(0);
	}
	
		// now wait for any replies
	
	join_thread_lan_start_tick=time_get();
	max_tick=client_timeout_wait_seconds*1000;
	
		// put socket in non-blocking so UI runs
		
	net_socket_blocking(broadcast_sock,FALSE);
	
	while (((join_thread_lan_start_tick+max_tick)>time_get()) && (!join_thread_quit)) {

			// any replies?
			
		good_reply=FALSE;
		
		if (net_recvfrom_mesage(broadcast_sock,NULL,NULL,&action,&net_uid,msg,NULL)) {
			if (action==net_action_reply_info) {
				good_reply=join_ping_thread_add_host(join_host_lan_list,join_thread_lan_start_tick,(network_reply_info*)msg);
				if (good_reply) join_create_list(join_host_lan_list,join_lan_table_id);
			}
		}
		
			// if no good reply, then sleep for a bit
			
		if (!good_reply) {
			usleep(100000);
		}
		
		element_table_busy(join_lan_table_id,"Looking for Local Hosts",(time_get()-join_thread_lan_start_tick),max_tick);
	}
	
	net_close_socket(&broadcast_sock);

	element_table_busy(join_lan_table_id,NULL,-1,-1);
	element_enable(join_button_rescan_id,TRUE);

	return(0);
}

/* =======================================================

      Join Ping Internet Threads
      
======================================================= */

void join_ping_thread_wan_host(join_server_host_type *host)
{
	int						action,net_uid,msec,max_tick;
	unsigned long			ip_addr,recv_ip_addr;
	unsigned char			msg[net_max_msg_size];
	bool					got_reply;
	d3socket				sock;

	msec=time_get();
	
		// send request info to host
		
	sock=net_open_udp_socket();
	if (sock==D3_NULL_SOCKET) return;
	
	net_socket_blocking(sock,FALSE);
	
	ip_addr=inet_addr(host->ip);
	if (ip_addr==INADDR_NONE) return;
	
	if (!net_sendto_msg(sock,ip_addr,net_port_host,net_action_request_info,net_uid_constant_none,NULL,0)) {
		net_close_socket(&sock);
		return;
	}

		// get the reply
	
	got_reply=FALSE;
	
	max_tick=client_timeout_wait_seconds*1000;
	
	while (((msec+max_tick)>time_get()) && (!join_thread_quit)) {
		if (net_recvfrom_mesage(sock,&recv_ip_addr,NULL,&action,&net_uid,msg,NULL)) {
			if ((recv_ip_addr==ip_addr) && (action==net_action_reply_info)) {
				got_reply=TRUE;
				break;
			}
		}
		usleep(100000);
	}
	
		// close socket
		
	net_close_socket(&sock);
	
		// update list

	if (got_reply) {
		join_ping_thread_update_host(host,msec,(network_reply_info*)msg);
	}
	else {
		join_ping_thread_update_host(host,msec,NULL);
	}

	join_create_list(join_host_wan_list,join_wan_table_id);
}

int join_ping_thread_wan(void *arg)
{
	int							n;
	char						str[256];
	join_server_host_type		*host;
	
		// run ping until finished or
		// join page ends

	for (n=0;n!=join_host_wan_list->count;n++) {
	
		if (join_thread_quit) break;

			// progress

		host=&join_host_wan_list->hosts[n];
		
		if (host->name[0]!=0x0) {
			sprintf(str,"Querying %s",host->name);
		}
		else {
			sprintf(str,"Querying %s",host->ip);
		}
		
		element_table_busy(join_wan_table_id,str,(n+1),(join_host_wan_list->count+1));

			// ping host

		join_ping_thread_wan_host(host);
	}

	element_table_busy(join_wan_table_id,NULL,-1,-1);
	element_enable(join_button_rescan_id,TRUE);

	return(0);
}

/* =======================================================

      Join Ping Threads Mainline
      
======================================================= */

void join_ping_thread_start(void)
{
		// empty join list
		
	join_thread_quit=FALSE;

		// wan table OK

	join_run_wan=FALSE;
	if (!iface.multiplayer.local_only) {
		join_run_wan=join_host_wan_list->count!=0;
	}
	
		// tables are busy
		// local table is always cleared

	join_host_lan_list->count=0;

	element_set_table_data(join_lan_table_id,NULL);
	element_table_busy(join_lan_table_id,NULL,0,1);

	if (join_run_wan) element_table_busy(join_wan_table_id,NULL,0,1);

		// start pinging hosts
		
	join_thread_started=TRUE;

#ifndef D3_SDL_1_3
	join_lan_thread=SDL_CreateThread(join_ping_thread_lan,NULL);
	if (join_run_wan) join_wan_thread=SDL_CreateThread(join_ping_thread_wan,NULL);
#else
	join_lan_thread=SDL_CreateThread(join_ping_thread_lan,"join_lan",NULL);
	if (join_run_wan) join_wan_thread=SDL_CreateThread(join_ping_thread_wan,"join_wan",NULL);
#endif
}

void join_ping_thread_end(void)
{
	if (!join_thread_started) return;
	
	join_thread_quit=TRUE;
	
	SDL_WaitThread(join_lan_thread,NULL);
	if (join_run_wan) SDL_WaitThread(join_wan_thread,NULL);
	
	join_thread_started=FALSE;
}

/* =======================================================

      Join Panes
      
======================================================= */

void join_news_pane(void)
{
	int			x,y,wid,high,margin,padding;
	
		// show and hide proper elements
		
	element_hide(join_button_rescan_id,TRUE);
	element_enable(join_button_join_id,FALSE);
	element_text_change(join_status_id,"");

		// news
		
	margin=element_get_tab_margin();
	padding=element_get_padding();
	
	x=margin+padding;
	y=(margin+element_get_tab_control_high())+padding;

	wid=iface.scale_x-((margin+padding)*2);
	high=(int)(((float)iface.scale_y)*0.84f)-y;

	element_text_box_add(join_news,join_news_id,x,y,wid,high);
}

void join_lan_internet_hosts(void)
{
	int						x,y,wid,high,margin,padding;
	element_column_type		cols[4];

		// show and hide proper elements
		
	element_hide(join_button_rescan_id,FALSE);
	element_enable(join_button_rescan_id,FALSE);
	element_enable(join_button_join_id,FALSE);
	element_text_change(join_status_id,"");
	
		// hosts tables
		
	margin=element_get_tab_margin();
	padding=element_get_padding();
	
	x=margin+padding;
	y=(margin+element_get_tab_control_high())+padding;

	wid=iface.scale_x-((margin+padding)*2);
	high=(int)(((float)iface.scale_y)*0.84f)-y;

	if (!iface.multiplayer.local_only) high=(high/2)-5;

	strcpy(cols[0].name,"Local Host");
	cols[0].percent_size=0.36f;
	strcpy(cols[1].name,"Game");
	cols[1].percent_size=0.40f;
	strcpy(cols[2].name,"Players");
	cols[2].percent_size=0.10f;
	strcpy(cols[3].name,"Ping");
	cols[3].percent_size=0.14f;

		// lan list

	element_table_add(cols,NULL,join_lan_table_id,4,x,y,wid,high,FALSE,element_table_bitmap_data);

		// wan list

	if (!iface.multiplayer.local_only) {
		y+=(high+5);
		strcpy(cols[0].name,"Internet Host");

		element_table_add(cols,NULL,join_wan_table_id,4,x,y,wid,high,FALSE,element_table_bitmap_data);

		join_create_list(join_host_wan_list,join_wan_table_id);
	}

		// start the thread to build the table
		
	join_ping_thread_start();
}

void join_create_pane(void)
{
	int						x,y,wid,high,pane;
	char					tab_list[][32]={"Hosts","News"};
	
		// turn off any scanning threads
		
	join_ping_thread_end();

		// controls

	element_clear();
	
		// tabs

	if (!iface.multiplayer.local_only) {
		element_tab_add((char*)tab_list,join_tab_value,join_tab_id,2);
	}
	else {
		element_tab_add((char*)tab_list,join_tab_value,join_tab_id,1);
	}
	
		// buttons
		
	wid=element_get_button_long_wid();
	high=element_get_button_high();

	element_get_button_bottom_left(&x,&y,wid,high);
	element_button_text_add("Rescan Hosts",join_button_rescan_id,x,y,wid,high,element_pos_left,element_pos_bottom);
	
	wid=element_get_button_short_wid();

	element_get_button_bottom_right(&x,&y,wid,high);
	element_button_text_add("Join",join_button_join_id,x,y,wid,high,element_pos_right,element_pos_bottom);

	x=element_get_x_position(join_button_join_id)-element_get_padding();
	element_button_text_add("Cancel",join_button_cancel_id,x,y,wid,high,element_pos_right,element_pos_bottom);

		// status

	wid=(int)(((float)iface.scale_x)*0.2f);
	x=(element_get_x_position(join_button_rescan_id)+wid)+element_get_padding();
	y-=element_get_padding();
	
	element_text_add("",join_status_id,x,y,iface.font.text_size_small,tx_left,NULL,FALSE);

		// specific pane controls
		
	pane=element_get_value(join_tab_id);
		
	switch (pane) {
		case join_pane_hosts:
			join_lan_internet_hosts();
			break;
		case join_pane_news:
			join_news_pane();
			break;
	}
}

/* =======================================================

      Join Operations
      
======================================================= */

void join_open(void)
{
	int				n;

		// news data

	join_news=(char*)malloc(max_join_server_news_len);
	join_news[0]=0x0;

		// host lists

	join_host_lan_list=(join_server_host_list_type*)malloc(sizeof(join_server_host_list_type));
	join_host_wan_list=(join_server_host_list_type*)malloc(sizeof(join_server_host_list_type));

	join_host_lan_list->count=0;
	join_host_wan_list->count=0;

		// get the project hash

	net_create_project_hash();

		// if internet hosts, then get
		// news and master host list

	if (!iface.multiplayer.local_only) {

			// load the news

		net_load_news(join_host_wan_list,join_news);

		join_host_wan_list->count=3;		// supergumba -- testing
		strcpy(join_host_wan_list->hosts[0].name,"Test 1");
		strcpy(join_host_wan_list->hosts[0].ip,"10.0.0.1");
		strcpy(join_host_wan_list->hosts[1].name,"Test 2");
		strcpy(join_host_wan_list->hosts[1].ip,"10.0.0.2");
		strcpy(join_host_wan_list->hosts[2].name,"Test 3");
		strcpy(join_host_wan_list->hosts[2].ip,"10.0.0.3");

			// nothing queried yet

		for (n=0;n!=join_host_wan_list->count;n++) {
			join_host_wan_list->hosts[n].pinged=FALSE;
		}
	}

		// setup gui
		
	gui_initialize("Bitmaps/Backgrounds","default");
	
		// start with first tab
		
	join_tab_value=join_pane_hosts;
	join_thread_started=FALSE;
	
		// create panes
		
	join_create_pane();
}

void join_close(void)
{
		// stop any pinging

	join_ping_thread_end();

		// free lists

	free(join_news);
	free(join_host_lan_list);
	free(join_host_wan_list);

		// shutdown gui
	
	gui_shutdown();
}

/* =======================================================

      Enabling
	        
======================================================= */

void join_activity_start(void)
{
		// stop any pinging
		
	join_ping_thread_end();
		
		// disable buttons and redraw
		
	element_enable(join_button_join_id,FALSE);
	element_enable(join_button_cancel_id,FALSE);
	
	gui_draw(1.0f,FALSE);
}

void join_activity_complete(bool single,char *msg)
{
	element_enable(join_button_join_id,single);
	element_enable(join_button_cancel_id,TRUE);
	
	if (!single) {		// supergumba -- might need to get rid of this
		element_set_value(join_lan_table_id,-1);
		element_set_value(join_wan_table_id,-1);
	}

	if (msg!=NULL) element_text_change(join_status_id,msg);
	
	gui_draw(1.0f,FALSE);
}

/* =======================================================

      Join Game
      
======================================================= */

void join_game(void)
{
	int								idx,net_uid,tick_offset;
	char							deny_reason[64],err_str[256];
	obj_type						*player_obj;
	join_server_host_type			*host;
	network_reply_join_remote_list	remote_list;

		// get selected host

	idx=element_get_value(join_lan_table_id);
	if (idx!=-1) {
		host=&join_host_lan_list->hosts[idx];
	}
	else {
		if (iface.multiplayer.local_only) return;
		
		idx=element_get_value(join_wan_table_id);
		host=&join_host_wan_list->hosts[idx];
	}

		// reject if server is full

	if (host->player_list.count>=host->player_list.max_count) {
		error_setup("Unable to Join Game: Server if Full","Network Game Canceled");
		server.next_state=gs_error;
		return;
	}

		// set the network flags
	
	if (!net_ip_to_address(host->ip,&net_setup.client.host_ip_addr,err_str)) {
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}

	net_setup.mode=net_mode_client;
	net_setup.client.latency=0;
	net_setup.score_limit=host->score_limit;
	net_setup.respawn_secs=host->respawn_secs;
	net_setup.game_reset_secs=host->game_reset_secs;
	net_setup.option_flags=host->option_flags;

		// setup game type and map

	net_setup.game_idx=net_client_find_game(host->game_name);
	if (net_setup.game_idx==-1) {
		sprintf(err_str,"Could not find game type: %s",host->game_name);
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}
	
	map.info.name[0]=0x0;
	strcpy(map.info.host_name,host->map_name);
	
		// start game
	
	if (!game_start(FALSE,skill_medium,0,0,err_str)) {
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}
	
		// start the map
		
	if (!map_start(FALSE,TRUE,err_str)) {
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}

		// get player

	player_obj=server.obj_list.objs[server.player_obj_idx];
							
		// attempt to join

	net_uid=net_client_join_host_start(player_obj,&tick_offset,deny_reason,&remote_list);
	if (net_uid==-1) {
		map_end();
		game_end();
		sprintf(err_str,"Unable to Join Game: %s",deny_reason);
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}

	player_obj->remote.net_uid=net_uid;
	
		// start client network thread
		
	if (!net_client_start_message_queue(err_str)) {
		net_client_send_leave_host(server.obj_list.objs[server.player_obj_idx]);
		net_client_join_host_end();
		map_end();
		game_end();
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}

		// add in any remotes

	game_remotes_create(&remote_list);

		// request moving group synchs

	net_client_request_group_synch_ping(player_obj);
	
		// game is running
	
	server.next_state=gs_running;
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
			server.next_state=gs_intro;
			break;
			
		case join_button_rescan_id:
			element_enable(join_button_rescan_id,FALSE);
			join_ping_thread_end();
			join_ping_thread_start();
			break;

		case join_lan_table_id:
			enable=(element_get_value(join_lan_table_id)!=-1);
			element_set_value(join_wan_table_id,-1);
			element_enable(join_button_join_id,enable);
			break;

		case join_wan_table_id:
			enable=(element_get_value(join_wan_table_id)!=-1);
			element_set_value(join_lan_table_id,-1);
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

