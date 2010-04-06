/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Network Host

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
#include "objects.h"
#include "remotes.h"
#include "interfaces.h"
#include "xmls.h"
#include "video.h"
#include "sounds.h"

#define host_pane_game					0
#define host_pane_options				1
#define host_pane_info					2

#define host_tab_id						0

#define host_button_host_id				10
#define host_button_cancel_id			11
#define host_game_type_id				12
#define host_table_id					13
#define host_dedicated_id				14
#define host_status_id					15

#define host_game_bot_count_id			20
#define host_game_bot_skill_id			21

#define host_game_score_limit_id		22

#define host_game_option_base			100

extern void intro_open(void);
extern bool net_host_game_start(char *err_str);
extern void net_host_game_end(void);
extern bool game_start(int skill,network_reply_join_remotes *remotes,char *err_str);
extern bool map_start(bool skip_media,char *err_str);

extern map_type				map;
extern server_type			server;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;

int							host_tab_value,host_map_count,host_first_map_idx;
char						*host_file_list;
char						net_game_types[max_net_game+1][name_str_len],
							bot_count_list[max_net_bot+2][name_str_len],
							bot_skill_list[6][32]={"Very Easy","Easy","Normal","Hard","Very Hard",""};

/* =======================================================

      Host Map Table
      
======================================================= */

void host_fill_map_table(char *game_type)
{
	int							n,nfile,sz;
	char						*c,info_name[name_str_len];
	file_path_directory_type	*map_pick_fpd;

		// need to make sure map paths are correct

	map_setup(&setup.file_path_setup,setup.anisotropic_mode,setup.mipmap_mode,setup.texture_quality_mode,setup.compress_on,TRUE);

		// load in all maps with the correct game type

	map_pick_fpd=file_paths_read_directory_data(&setup.file_path_setup,"Maps","xml");

	nfile=map_pick_fpd->nfile;

	if (nfile>0) {
		sz=(nfile+1)*128;
		
		host_file_list=malloc(sz);
		bzero(host_file_list,sz);

		c=host_file_list;
		
		for (n=0;n!=nfile;n++) {
			if (map_check_game_type(game_type,map_pick_fpd->files[n].file_name,info_name)) {
				sprintf(c,"Bitmaps/Icons_Map;%s;%s",map_pick_fpd->files[n].file_name,info_name);
				c+=128;
			}
		}

		host_map_count=nfile;
		
		element_set_table_data(host_table_id,host_file_list);
	}
	else {
		host_map_count=0;
		host_file_list=NULL;
	}

	file_paths_close_directory(map_pick_fpd);
}

/* =======================================================

      Set and Get Last Host Map
      
======================================================= */

void host_set_last_map(void)
{
	int				n,k;
	char			*c;
	char			str[256];
	bool			checked;

	host_first_map_idx=-1;

	for (n=0;n!=host_map_count;n++) {

			// get map name

		c=host_file_list+(n*128);
		
		c=strchr(c,';');
		if (c!=NULL) {
			strcpy(str,(c+1));
			c=strchr(str,';');
			if (c!=NULL) *c=0x0;
		}

			// find if map is in list

		checked=FALSE;

		for (k=0;k!=setup.network.map.count;k++) {
			if (strcasecmp(setup.network.map.maps[k].name,str)==0) {
				checked=TRUE;
				break;
			}
		}
	
		element_set_table_checkbox(host_table_id,n,checked);

			// always scroll to first selected map

		if (host_first_map_idx==-1) host_first_map_idx=n;
	}

	if (host_first_map_idx==-1) host_first_map_idx=0;
}

void host_get_last_map(void)
{
	int				n,cnt;
	char			*c;
	char			str[256];

	cnt=0;

	for (n=0;n!=host_map_count;n++) {
		if (!element_get_table_checkbox(host_table_id,n)) continue;
	
		c=host_file_list+(n*128);
		
		c=strchr(c,';');
		if (c!=NULL) {
			strcpy(str,(c+1));
			c=strchr(str,';');
			if (c!=NULL) *c=0x0;
		}
		
		strcpy(setup.network.map.maps[cnt].name,str);
		cnt++;
	}

	setup.network.map.count=cnt;
}

/* =======================================================

      Host Panes
      
======================================================= */

void host_game_pane(void)
{
	int						n,x,y,wid,high,margin,padding,control_y_add;
	element_column_type		cols[1];
	
	margin=element_get_tab_margin();
	padding=element_get_padding();
	control_y_add=element_get_control_high();
	
		// game type

	for (n=0;n!=hud.net_game.ngame;n++) {
		strcpy(net_game_types[n],hud.net_game.games[n].name);
	}
	net_game_types[hud.net_game.ngame][0]=0x0;

	x=(int)(((float)hud.scale_x)*0.20f);
	y=((margin+element_get_tab_control_high())+padding)+control_y_add;
	
	element_combo_add("Game Type",(char*)net_game_types,setup.network.game_type,host_game_type_id,x,y,TRUE);
	y+=control_y_add;

		// dedicated checkbox

	element_checkbox_add("Dedicated",setup.network.dedicated,host_dedicated_id,x,y,TRUE);
	y+=padding;

		// hosts table
		
	x=margin+padding;

	wid=hud.scale_x-((margin+padding)*2);
	high=(int)(((float)hud.scale_y)*0.85f)-y;

	strcpy(cols[0].name,"Map");
	cols[0].percent_size=1.0f;

	element_table_add(cols,NULL,host_table_id,1,x,y,wid,high,TRUE,element_table_bitmap_data);
	
		// fill table with maps

	host_fill_map_table(hud.net_game.games[setup.network.game_type].name);
	host_set_last_map();

	element_set_value(host_table_id,host_first_map_idx);
	element_make_selection_visible(host_table_id);
}

void host_options_pane(void)
{
	int							n,k,x,y,control_y_add,control_y_sz;
	char						str[32];
	bool						on;
	hud_net_option_type			*option;

		// panel sizes

	control_y_add=element_get_control_high();
	control_y_sz=(control_y_add+(control_y_add*hud.net_option.noption));
	if (hud.net_bot.on) control_y_sz+=(control_y_add*2);
	
	x=(int)(((float)hud.scale_x)*0.4f);
	y=(hud.scale_y>>1)-(control_y_sz>>1);
	
		// bots

	if (hud.net_bot.on) {

		for (n=0;n!=(max_net_bot+1);n++) {
			if (n==0) {
				strcpy(bot_count_list[n],"None");
			}
			else {
				sprintf(bot_count_list[n],"%d",n);
			}
		}
		bot_count_list[max_net_bot+1][0]=0x0;
		
		element_combo_add("Bot Count",(char*)bot_count_list,setup.network.bot.count,host_game_bot_count_id,x,y,TRUE);
		y+=control_y_add;

		element_combo_add("Bot Skill",(char*)bot_skill_list,setup.network.bot.skill,host_game_bot_skill_id,x,y,TRUE);
		y+=control_y_add;
	}

		// score limit

	sprintf(str,"%d",setup.network.score_limit);
	element_text_field_add("Score Limit:",str,32,host_game_score_limit_id,x,y,TRUE);
	y+=control_y_add;

		// project options

	y+=element_get_padding();

	option=hud.net_option.options;

	for (n=0;n!=hud.net_option.noption;n++) {

		on=FALSE;

		for (k=0;k!=setup.network.option.count;k++) {
			if (strcasecmp(option->name,setup.network.option.options[k].name)==0) {
				on=TRUE;
				break;
			}
		}

		element_checkbox_add(option->descript,on,(host_game_option_base+n),x,y,TRUE);
		y+=control_y_add;

		option++;
	}
}

void host_info_pane(void)
{
	int			x,y,control_y_add,control_y_sz;
	char		str[256];
	
	control_y_add=element_get_control_high();
	control_y_sz=10*control_y_add;
	
	x=(int)(((float)hud.scale_x)*0.4f);
	y=(hud.scale_y>>1)-(control_y_sz>>1);
	
		// type
		
	if (strcmp(net_setup.host.ip_resolve,"127.0.0.1")==0) {
		strcpy(str,"Local Machine");
	}
	else {
		if (net_ip_local(net_setup.host.ip_resolve)) {
			strcpy(str,"Local Network");
		}
		else {
			strcpy(str,"Internet");
		}
	}
		
	element_info_field_add("Type",str,-1,x,y);
	y+=control_y_add;
	
		// host name and IP
	
	element_info_field_add("Host Name",net_setup.host.name,-1,x,y);
	y+=control_y_add;
	element_info_field_add("Host IP",net_setup.host.ip_resolve,-1,x,y);
	y+=control_y_add;

	sprintf(str,"%d",net_port_host);
	element_info_field_add("Host Port",str,-1,x,y);
	y+=control_y_add;

	sprintf(str,"%d",net_port_host_query);
	element_info_field_add("Host Query Port",str,-1,x,y);
	y+=control_y_add;
}

void host_create_pane(void)
{
	int			x,y,wid,high,pane;
	char		tab_list[][32]={"Host Game","Options","Info"};
							
	element_clear();
	
		// tabs
		
	element_tab_add((char*)tab_list,host_tab_value,host_tab_id,3);
	
		// setup some IP information
		
	if (net_setup.host.name[0]==0x0) net_get_host_name(net_setup.host.name);
	net_get_host_ip(net_setup.host.ip_name,net_setup.host.ip_resolve);
	
		// buttons
		
	wid=(int)(((float)hud.scale_x)*0.1f);
	high=(int)(((float)hud.scale_x)*0.04f);

	element_get_button_bottom_right(&x,&y,wid,high);
	element_button_text_add("Host",host_button_host_id,x,y,wid,high,element_pos_right,element_pos_bottom);

	x=element_get_x_position(host_button_host_id)-element_get_padding();
	element_button_text_add("Cancel",host_button_cancel_id,x,y,wid,high,element_pos_right,element_pos_bottom);
	
		// specific pane controls
		
	pane=element_get_value(host_tab_id);
		
	switch (pane) {
		case host_pane_game:
			host_game_pane();
			break;
		case host_pane_options:
			host_options_pane();
			break;
		case host_pane_info:
			host_info_pane();
			break;
	}
	
		// enable host button
		
	element_enable(host_button_host_id,(setup.network.map.count!=0));
}

/* =======================================================

      Open/Close Host Game
      
======================================================= */

void host_open(void)
{
		// get the project hash

	net_create_project_hash();

		// setup gui
		
	gui_initialize(NULL,NULL,TRUE);

		// start with first tab
		
	host_tab_value=0;
	host_map_count=0;
	host_first_map_idx=-1;

	host_create_pane();

		// in host thread
	
	server.state=gs_host;
}

void host_close(bool stop_music)
{
	if (stop_music) {
		if (al_music_playing()) al_music_stop();
	}

	if (host_file_list!=NULL) free(host_file_list);
	gui_shutdown();
}

/* =======================================================

      Host Game
      
======================================================= */

void host_game_setup(void)
{
	int				n,k;
	char			*c;
	
		// game type
		
	net_setup.game_idx=setup.network.game_type;
	
		// game options
		
	net_setup.option_flags=0x0;
	
	for (n=0;n!=setup.network.option.count;n++) {
		for (k=0;k!=hud.net_option.noption;k++) {
			if (strcasecmp(setup.network.option.options[n].name,hud.net_option.options[k].name)==0) {
				net_setup.option_flags=net_setup.option_flags|(0x1<<k);
				break;
			}
		}
	}
	
		// use graphic name to get to original map name
		
	net_setup.host.map_name[0]=0x0;
				
	c=host_file_list+(host_first_map_idx*128);
	
	c=strchr(c,';');
	if (c!=NULL) {
		strcpy(net_setup.host.map_name,(c+1));
		c=strchr(net_setup.host.map_name,';');
		if (c!=NULL) *c=0x0;
	}
}

void host_game(void)
{
	char			err_str[256];
	
		// start hosting

	if (!net_host_game_start(err_str)) {
		error_open(err_str,"Hosting Game Canceled");
		return;
	}

		// setup game from host
		
	map.info.name[0]=0x0;
	strcpy(map.info.host_name,net_setup.host.map_name);
	
		// start game
	
	if (!game_start(skill_medium,NULL,err_str)) {
		net_host_game_end();
		error_open(err_str,"Hosting Game Canceled");
		return;
	}
	
		// start the map
		
	if (!map_start(TRUE,err_str)) {
		net_host_game_end();
		error_open(err_str,"Hosting Game Canceled");
		return;
	}
	
		// add bots to host

	net_host_player_add_bots();

		// player on host setup

	if (net_setup.mode!=net_mode_host_dedicated) {

			// set remote UID

		object_player_set_remote_uid(net_setup.player_uid);
			
			// start local queue

		if (!net_client_start_message_queue_local(err_str)) {
			net_host_game_end();
			error_open(err_str,"Hosting Game Canceled");
			return;
		}

			// mark node as ready to receive data from host
		
		net_client_send_ready();
	}

		// game is running
	
	server.state=gs_running;
}

/* =======================================================

      Host Input
      
======================================================= */

void host_handle_click(int id)
{
	int							n,idx;
	char						str[32];
	hud_net_option_type			*option;

		// special option clicks
		// rebuild the options list

	if (id>=host_game_option_base) {
	
		setup.network.option.count=0;
		option=hud.net_option.options;

		for (n=0;n!=hud.net_option.noption;n++) {

			if (element_get_value(host_game_option_base+n)!=0) {
				strcpy(setup.network.option.options[setup.network.option.count].name,option->name);
				setup.network.option.count++;
			}

			option++;
		}

		return;
	}

		// run selection

	switch (id) {

			// tab
			
		case host_tab_id:
			host_tab_value=element_get_value(host_tab_id);
			host_create_pane();
			return;

			// controls

		case host_game_type_id:
			idx=element_get_value(host_game_type_id);
			if (idx!=setup.network.game_type) {
				setup.network.game_type=idx;
				host_fill_map_table(hud.net_game.games[idx].name);
				host_set_last_map();
				element_set_value(host_table_id,host_first_map_idx);
				element_make_selection_visible(host_table_id);
				element_enable(host_button_host_id,(host_first_map_idx!=0));
			}
			break;

		case host_table_id:
			host_get_last_map();
			element_enable(host_button_host_id,(setup.network.map.count!=0));
			break;

		case host_dedicated_id:
			setup.network.dedicated=(element_get_value(host_dedicated_id)!=0);
			break;

		case host_game_bot_count_id:
			setup.network.bot.count=element_get_value(host_game_bot_count_id);
			break;

		case host_game_bot_skill_id:
			setup.network.bot.skill=element_get_value(host_game_bot_skill_id);
			break;

		case host_game_score_limit_id:
			element_get_value_string(host_game_score_limit_id,str);
			setup.network.score_limit=atoi(str);
			if (setup.network.score_limit<0) setup.network.score_limit=0;
			break;

			// buttons

		case host_button_host_id:
			setup_xml_write();
			host_game_setup();
			host_close(TRUE);
			host_game();
			break;
			
		case host_button_cancel_id:
			setup_xml_write();
			host_close(FALSE);
			intro_open();
			break;
	}
}

void host_keyboard(void)
{
	int			id;

	id=gui_keyboard();
	if (id!=-1) host_handle_click(id);
}

void host_click(void)
{
	int			id;
	
	id=gui_click();
	if (id==-1) return;
	
	hud_click();

	host_handle_click(id);
}

/* =======================================================

      Run Host
      
======================================================= */

void host_run(void)
{
	gui_draw(1.0f,TRUE);
	host_click();
	host_keyboard();
}

