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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "network.h"
#include "objects.h"

#define host_pane_game					0
#define host_pane_options				1
#define host_pane_info					2

#define host_tab_id						0

#define host_button_host_id				10
#define host_button_cancel_id			11
#define host_game_type_id				12
#define host_table_id					13
#define host_dedicated_id				14
#define host_map_rotation_id			15
#define host_status_id					16

#define host_game_bot_count_id			20
#define host_game_bot_skill_id			21

#define host_game_score_limit_id		22
#define host_game_game_reset_secs_id	23
#define host_game_respawn_secs_id		24

#define host_game_option_base			100

extern void net_host_game_setup(void);
extern bool net_host_game_start(char *err_str);
extern void net_host_game_end(void);

extern map_type				map;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;

int							host_tab_value,host_first_map_idx,
							host_file_map_count,host_table_map_count;
char						*host_file_map_list,*host_table_map_list;
char						net_game_types[max_net_game+1][name_str_len],
							bot_skill_list[6][32]={"Very Easy","Easy","Normal","Hard","Very Hard",""};

/* =======================================================

      Host Map Table Data
      
======================================================= */

bool host_check_game_type(char *game_type,char *game_list)
{
	int				idx;
	char			*c,*c2;
	char			game_token[256];

	c=game_list;

	while (TRUE) {

		if (*c==0x0) break;

			// check next token

		strcpy(game_token,c);
		c2=strchr(game_token,',');
		if (c2!=NULL) *c2=0x0;

			// remove white space

		if (game_token[0]!=0x0) {
			while (game_token[0]==' ') {
				strcpy((char*)&game_token[0],(char*)&game_token[1]);
			}
			while (TRUE) {
				idx=strlen(game_token);
				if (idx==0) break;
				if (game_token[idx-1]!=' ') break;
				
				game_token[idx-1]=0x0;
			}
		}

			// compare

		if (strcasecmp(game_type,game_token)==0) return(TRUE);

			// get next token

		c2=strchr(c,',');
		if (c2==NULL) break;

		c=c2+1;
	}

	return(FALSE);
}

void host_fill_map_table(char *game_type)
{
	int							n;
	char						*c,*c2;

	host_table_map_count=0;

	c=host_file_map_list;
	c2=host_table_map_list;

	for (n=0;n!=host_file_map_count;n++) {
		
		if (host_check_game_type(game_type,(c+(file_str_len+name_str_len)))) {
			sprintf(c2,"Bitmaps/Icons_Map;%s;%s",c,(c+file_str_len));
			c2+=128;
			
			host_table_map_count++;
		}

		c+=(file_str_len+name_str_len+256);
	}

	*c2=0x0;
	element_set_table_data(host_table_id,host_table_map_list);
}

void host_map_list_initialize(void)
{
	int							n,nfile,sz;
	char						*c;
	char						info_name[name_str_len],game_list[256],
								path[1024];
	file_path_directory_type	*map_pick_fpd;
	FILE						*file;

		// initial setup

	host_file_map_count=0;
	host_file_map_list=NULL;

	host_table_map_count=0;
	host_table_map_list=NULL;

		// need to make sure map paths are correct

	map_setup(&setup.file_path_setup,setup.anisotropic_mode,setup.mipmap_mode,setup.texture_quality_mode,TRUE);

		// load in all maps with the correct game type

	map_pick_fpd=file_paths_read_directory_data(&setup.file_path_setup,"Maps","xml");

	nfile=map_pick_fpd->nfile;
	if (nfile<=0) {
		file_paths_close_directory(map_pick_fpd);
		return;
	}

		// data for maps

	sz=nfile*(file_str_len+name_str_len+256);
	host_file_map_list=malloc(sz);
	bzero(host_file_map_list,sz);

	sz=(nfile+1)*128;
	host_table_map_list=malloc(sz);
	bzero(host_table_map_list,sz);

	progress_initialize(NULL);

		// load the maps

	c=host_file_map_list;
	
	for (n=0;n!=nfile;n++) {

		progress_draw(((n+1)*100)/nfile);

			// if no JS file, don't put in list
			// this is probably a backup file

		file_paths_data(&setup.file_path_setup,path,"Scripts/Courses",map_pick_fpd->files[n].file_name,"js");
		
		file=fopen(path,"rb");
		if (file==NULL) continue;
		fclose(file);

			// get the map info

		if (!map_host_load_info(map_pick_fpd->files[n].file_name,info_name,game_list)) continue;

		strcpy(c,map_pick_fpd->files[n].file_name);
		c+=file_str_len;
		strcpy(c,info_name);
		c+=name_str_len;
		strcpy(c,game_list);
		c+=256;
		
		host_file_map_count++;
	}

	file_paths_close_directory(map_pick_fpd);

	progress_shutdown();
}

void host_map_list_shutdown(void)
{
	if (host_file_map_list!=NULL) free(host_file_map_list);
	if (host_table_map_list!=NULL) free(host_table_map_list);
}

/* =======================================================

      Set and Get Map List
      
======================================================= */

void host_map_list_get_name(int idx,char *name)
{
	char			*c;

	name[0]=0x0;

	c=host_table_map_list+(idx*128);
		
	c=strchr(c,';');
	if (c!=NULL) {
		strcpy(name,(c+1));
		c=strchr(name,';');
		if (c!=NULL) *c=0x0;
	}
}

void host_map_list_to_table(void)
{
	int				n,k;
	char			name[256];
	bool			checked;

		// single map?

	if (!setup.network.map_rotation) {

		host_first_map_idx=-1;
		if (setup.network.map.count==0) return;

		for (n=0;n!=host_table_map_count;n++) {
			host_map_list_get_name(n,name);
			if (strcmp(setup.network.map.maps[0].name,name)==0) {
				host_first_map_idx=n;
				return;
			}
		}

		return;
	}

		// rotating maps

	host_first_map_idx=-1;

	for (n=0;n!=host_table_map_count;n++) {

			// get map name

		host_map_list_get_name(n,name);

			// find if map is in list

		checked=FALSE;

		for (k=0;k!=setup.network.map.count;k++) {
			if (strcasecmp(setup.network.map.maps[k].name,name)==0) {
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

void host_map_table_to_list(void)
{
	int				n,idx,count;
	char			name[256];

	setup.network.map.count=0;

		// single map?

	if (!setup.network.map_rotation) {
		idx=element_get_value(host_table_id);
		if (idx==-1) return;

		host_map_list_get_name(idx,name);

		setup.network.map.count=1;
		strcpy(setup.network.map.maps[0].name,name);

		return;
	}
	
		// rotating maps

	count=0;

	for (n=0;n!=host_table_map_count;n++) {
		if (!element_get_table_checkbox(host_table_id,n)) continue;
	
		host_map_list_get_name(n,name);
		strcpy(setup.network.map.maps[count++].name,name);
	}

	setup.network.map.count=count;
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

	for (n=0;n!=iface.net_game.ngame;n++) {
		strcpy(net_game_types[n],iface.net_game.games[n].name);
	}
	net_game_types[iface.net_game.ngame][0]=0x0;

	x=(int)(((float)iface.scale_x)*0.20f);
	y=((margin+element_get_tab_control_high())+padding)+control_y_add;
	
	element_combo_add("Game Type",(char*)net_game_types,setup.network.game_type,host_game_type_id,x,y,TRUE);
	y+=control_y_add;

		// rotating maps button

	element_checkbox_add("Map Rotation",setup.network.map_rotation,host_map_rotation_id,x,y,TRUE);
	y+=padding;

		// dedicated checkbox

// supergumba -- not fully implemented yet
	setup.network.dedicated=FALSE;
//	element_checkbox_add("Dedicated",setup.network.dedicated,host_dedicated_id,x,y,TRUE);
//	y+=padding;

		// hosts table
		
	x=margin+padding;

	wid=iface.scale_x-((margin+padding)*2);
	high=(int)(((float)iface.scale_y)*0.85f)-y;

	strcpy(cols[0].name,"Map");
	cols[0].percent_size=1.0f;

	element_table_add(cols,NULL,host_table_id,1,x,y,wid,high,setup.network.map_rotation,element_table_bitmap_data);
	y+=(high+padding);
	
		// fill table with maps

	host_fill_map_table(iface.net_game.games[setup.network.game_type].name);
	host_map_list_to_table();

	element_set_value(host_table_id,host_first_map_idx);
	element_make_selection_visible(host_table_id);
}

void host_options_pane(void)
{
	int							n,k,x,y,control_y_add,control_y_sz;
	bool						on;
	iface_net_option_type		*option;

		// panel sizes

	control_y_add=element_get_control_high();
	control_y_sz=((control_y_add*3)+(control_y_add*iface.net_option.noption));
	if (iface.net_bot.on) control_y_sz+=(control_y_add*2);
	
	x=(int)(((float)iface.scale_x)*0.4f);
	y=(iface.scale_y>>1)-(control_y_sz>>1);
	
		// bots

	if (iface.net_bot.on) {

		element_number_add("Bot Count",setup.network.bot.count,host_game_bot_count_id,x,y,0,15);
		y+=control_y_add;

		element_combo_add("Bot Skill",(char*)bot_skill_list,setup.network.bot.skill,host_game_bot_skill_id,x,y,TRUE);
		y+=control_y_add;
	}

		// score limits and spawn times

	element_number_add("Score Limit",setup.network.score_limit,host_game_score_limit_id,x,y,1,100);
	y+=control_y_add;
	
	element_number_add("Game Reset Seconds",setup.network.game_reset_secs,host_game_game_reset_secs_id,x,y,5,30);
	y+=control_y_add;
	
	element_number_add("Respawn Seconds",setup.network.respawn_secs,host_game_respawn_secs_id,x,y,0,30);
	y+=control_y_add;

		// project options

	y+=element_get_padding();

	option=iface.net_option.options;

	for (n=0;n!=iface.net_option.noption;n++) {

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
	control_y_sz=5*control_y_add;
	
	x=(int)(((float)iface.scale_x)*0.4f);
	y=(iface.scale_y>>1)-(control_y_sz>>1);
	
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

void host_enable_host_button(void)
{
	int			pane;
	bool		enable;
	
	pane=element_get_value(host_tab_id);
	if (pane!=host_pane_game) return;
	
	if (!setup.network.map_rotation) {
		enable=(element_get_value(host_table_id)!=-1);
	}
	else {
		enable=element_has_table_check(host_table_id);
	}
		
	element_enable(host_button_host_id,enable);
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
		
	wid=(int)(((float)iface.scale_x)*0.1f);
	high=(int)(((float)iface.scale_x)*0.04f);

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
		
	host_enable_host_button();
}

/* =======================================================

      Open/Close Host Game
      
======================================================= */

void host_open(void)
{
		// load the maps

	host_map_list_initialize();

		// get the project hash

	net_create_project_hash();

		// setup gui
		
	gui_initialize(NULL,NULL);

		// start with first tab
		
	host_tab_value=0;
	host_first_map_idx=-1;

	host_create_pane();
}

void host_close(void)
{
	host_map_list_shutdown();
	gui_shutdown();
}

/* =======================================================

      Host Game
      
======================================================= */

void host_game_setup(void)
{
	int				n,k;
	
		// game type
		
	net_setup.game_idx=setup.network.game_type;
	
		// game options
		
	net_setup.score_limit=setup.network.score_limit;
	net_setup.respawn_secs=setup.network.respawn_secs;
	net_setup.game_reset_secs=setup.network.game_reset_secs;
		
	net_setup.option_flags=0x0;
	
	for (n=0;n!=setup.network.option.count;n++) {
		for (k=0;k!=iface.net_option.noption;k++) {
			if (strcasecmp(setup.network.option.options[n].name,iface.net_option.options[k].name)==0) {
				net_setup.option_flags=net_setup.option_flags|(0x1<<k);
				break;
			}
		}
	}

		// starting game map

	net_setup.host.current_map_idx=0;
}

void host_game(void)
{
	char			err_str[256];
	
		// setup hosting flags and IPs
		
	net_host_game_setup();

	net_setup.mode=setup.network.dedicated?net_mode_host_dedicated:net_mode_host;
	net_setup.client.latency=0;
	net_setup.client.host_ip_addr=0;

		// setup map
		
	map.info.name[0]=0x0;
	strcpy(map.info.host_name,setup.network.map.maps[net_setup.host.current_map_idx].name);
	
		// start game
	
	if (!game_start(FALSE,skill_medium,0,err_str)) {
		net_host_game_end();
		error_setup(err_str,"Hosting Game Canceled");
		server.next_state=gs_error;
		return;
	}
	
		// add any multiplayer bots
		
	if (!game_multiplayer_bots_create(err_str)) {
		net_host_game_end();
		error_setup(err_str,"Hosting Game Canceled");
		server.next_state=gs_error;
		return;
	}
	
		// start the map
		
	if (!map_start(FALSE,TRUE,err_str)) {
		net_host_game_end();
		error_setup(err_str,"Hosting Game Canceled");
		server.next_state=gs_error;
		return;
	}

		// start hosting

	if (!net_host_game_start(err_str)) {
		net_host_game_end();
		error_setup(err_str,"Hosting Game Canceled");
		server.next_state=gs_error;
		return;
	}

		// add local player to host

	if (!net_host_join_local_player(err_str)) {
		net_host_game_end();
		error_setup(err_str,"Hosting Game Canceled");
		server.next_state=gs_error;
		return;
	}
	
		// add multiplayer bots to host

	net_host_join_multiplayer_bots();

		// game is running
	
	server.next_state=gs_running;
}

/* =======================================================

      Host Input
      
======================================================= */

void host_handle_click(int id)
{
	int							n,idx;
	iface_net_option_type		*option;

		// special option clicks
		// rebuild the options list

	if (id>=host_game_option_base) {
	
		setup.network.option.count=0;
		option=iface.net_option.options;

		for (n=0;n!=iface.net_option.noption;n++) {

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
				host_fill_map_table(iface.net_game.games[idx].name);
				host_map_list_to_table();
				element_set_value(host_table_id,host_first_map_idx);
				element_make_selection_visible(host_table_id);
				host_enable_host_button();
			}
			break;

		case host_table_id:
			host_map_table_to_list();
			element_enable(host_button_host_id,(setup.network.map.count!=0));
			break;

// supergumba -- not fully implemented yet
//		case host_dedicated_id:
//			setup.network.dedicated=(element_get_value(host_dedicated_id)!=0);
//			break;

		case host_map_rotation_id:
			setup.network.map_rotation=(element_get_value(host_map_rotation_id)!=0);
			host_create_pane();
			break;

		case host_game_bot_count_id:
			setup.network.bot.count=element_get_value(host_game_bot_count_id);
			break;

		case host_game_bot_skill_id:
			setup.network.bot.skill=element_get_value(host_game_bot_skill_id);
			break;

		case host_game_score_limit_id:
			setup.network.score_limit=element_get_value(host_game_score_limit_id);
			if (setup.network.score_limit<0) setup.network.score_limit=0;
			break;
			
		case host_game_game_reset_secs_id:
			setup.network.game_reset_secs=element_get_value(host_game_game_reset_secs_id);
			break;
			
		case host_game_respawn_secs_id:
			setup.network.respawn_secs=element_get_value(host_game_respawn_secs_id);
			break;

			// buttons

		case host_button_host_id:
			setup_xml_write();
			host_game_setup();
			host_game();
			break;
			
		case host_button_cancel_id:
			setup_xml_write();
			server.next_state=gs_intro;
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

