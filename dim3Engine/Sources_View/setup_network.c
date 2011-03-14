/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Setup Network

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

#include "interfaces.h"
#include "xmls.h"
#include "video.h"
#include "sounds.h"
#include "inputs.h"

#define setup_network_pane_player			0
#define setup_network_pane_host				1

#define ctrl_network_tab_id					0

#define ctrl_network_name_id				10
#define ctrl_network_show_names_id			11
#define ctrl_color_id						12
#define ctrl_character_id					13
#define ctrl_character_model_id				14

#define ctrl_network_host_id				20
#define ctrl_network_host_name_id			21
#define ctrl_network_host_ip_id				22

#define setup_network_host_add_button		30
#define setup_network_host_update_button	31
#define setup_network_host_delete_button	32

#define setup_network_ok_button				100
#define setup_network_cancel_button			101

extern render_info_type		render_info;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;

extern setup_type			setup_backup;

int							setup_network_tab_value,setup_network_host_scroll_pos;
char						setup_host_list[max_setup_network_host+1][128],
							setup_character_list[max_character+1][128];

/* =======================================================

      Character Table
      
======================================================= */

void setup_network_fill_character_table(void)
{
	int							n;
	char						*c;

	c=(char*)setup_character_list;
	
	for (n=0;n!=iface.character.ncharacter;n++) {
		sprintf(c,"%s",iface.character.characters[n].name);
		c+=128;
	}

	element_set_table_data(ctrl_character_id,(char*)setup_character_list);
}

/* =======================================================

      Setup Network Panes
      
======================================================= */

void setup_network_create_host_list(void)
{
	int							n;
	setup_network_host_type		*host;

	host=setup.network.host.hosts;

	for (n=0;n!=setup.network.host.count;n++) {
		sprintf(setup_host_list[n],"%s\t%s",host->name,host->ip);
		host++;
	}
	
	setup_host_list[setup.network.host.count][0]=0x0;
}

void setup_network_player_pane(void)
{
	int						x,y,wid,high,margin,padding,
							control_y_add,control_y_sz;
	element_column_type		cols[1];
	hud_character_item_type	*hud_character;

	margin=element_get_tab_margin();
	padding=element_get_padding();

	control_y_add=element_get_control_high();
	control_y_sz=control_y_add*3;
	
	x=(int)(((float)iface.scale_x)*0.24f);
	
	if (iface.character.ncharacter!=0) {
		y=((margin+element_get_tab_control_high())+padding)+control_y_add;
	}
	else {
		y=(iface.scale_y>>1)-(control_y_sz>>1);
	}
	
		// names and colors
		
	element_text_field_add("Name",setup.network.name,name_str_len,ctrl_network_name_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Show Names",setup.network.show_names,ctrl_network_show_names_id,x,y,TRUE);
	y+=control_y_add;
	element_color_add("Color",setup.network.tint_color_idx,ctrl_color_id,x,y,TRUE);
	
		// is there a character control?

	if (iface.character.ncharacter==0) return;
	
		// character table
		
	x=margin+padding;
	y+=padding;

	wid=(int)(((float)iface.scale_x)*0.80f)-((margin+padding)*2);
	high=(int)(((float)iface.scale_y)*0.85f)-y;

	strcpy(cols[0].name,"Characters");
	cols[0].percent_size=1.0f;

	element_table_add(cols,NULL,ctrl_character_id,1,x,y,wid,high,FALSE,element_table_bitmap_none);

		// fill and select table

	setup_network_fill_character_table();

	element_set_value(ctrl_character_id,setup.network.character_idx);
	element_make_selection_visible(ctrl_character_id);
	
		// character model

	x=(int)(((float)iface.scale_x)*0.81f);
	y=(int)(((float)iface.scale_y)*0.8f);
	
	hud_character=&iface.character.characters[setup.network.character_idx];

	element_model_add(hud_character->model_name,"Idle",hud_character->interface_resize,&hud_character->interface_offset,NULL,ctrl_character_model_id,x,y);
}

void setup_network_host_pane(void)
{
	int						x,y,wid,high,margin,padding,control_y_add;
	element_column_type		cols[2];

	margin=element_get_tab_margin();
	padding=element_get_padding();
	
	x=margin+padding;
	y=(margin+element_get_tab_control_high())+padding;

	wid=iface.scale_x-((margin+padding)*2);
	high=(int)(((float)iface.scale_y)*0.72f)-y;

	control_y_add=element_get_control_high();

		// setup host list
		
	setup_network_create_host_list();

	strcpy(cols[0].name,"Name");
	cols[0].percent_size=0.5f;

	strcpy(cols[1].name,"Address");
	cols[1].percent_size=0.5f;
	
	element_table_add(cols,(char*)setup_host_list,ctrl_network_host_id,2,x,y,wid,high,FALSE,element_table_bitmap_none);

		// host editing

	x=(int)(((float)iface.scale_x)*0.15f);
	y+=(high+control_y_add+8);

	element_text_field_add("Name","",name_str_len,ctrl_network_host_name_id,x,y,TRUE);
	y+=control_y_add;

	element_text_field_add("Address","",name_str_len,ctrl_network_host_ip_id,x,y,TRUE);

		// host buttons

	padding=element_get_padding();

	x=iface.scale_x>>1;
	
	wid=(int)(((float)iface.scale_x)*0.2f);
	high=(int)(((float)iface.scale_x)*0.04f);
	
	y+=(padding+(high/2));

	element_button_text_add("Add Host",setup_network_host_add_button,(x-(padding/2)),y,wid,high,element_pos_right,element_pos_center);

	element_button_text_add("Update Host",setup_network_host_update_button,(x-(padding/2)),y,wid,high,element_pos_right,element_pos_center);
	element_hide(setup_network_host_update_button,TRUE);
	
	element_button_text_add("Delete Host",setup_network_host_delete_button,(x+(padding/2)),y,wid,high,element_pos_left,element_pos_center);
	element_enable(setup_network_host_delete_button,FALSE);
}

void setup_network_create_pane(void)
{
	int			x,y,wid,high,pane;
	char		tab_list[][32]={"Player","Hosts"};
							
	element_clear();
	
		// tabs
		
	element_tab_add((char*)tab_list,setup_network_tab_value,ctrl_network_tab_id,2);
	
		// buttons
		
	wid=(int)(((float)iface.scale_x)*0.1f);
	high=(int)(((float)iface.scale_x)*0.04f);

	element_get_button_bottom_right(&x,&y,wid,high);
	element_button_text_add("OK",setup_network_ok_button,x,y,wid,high,element_pos_right,element_pos_bottom);

	x=element_get_x_position(setup_network_ok_button)-element_get_padding();
	element_button_text_add("Cancel",setup_network_cancel_button,x,y,wid,high,element_pos_right,element_pos_bottom);
	
		// specific pane controls
		
	pane=element_get_value(ctrl_network_tab_id);
		
	switch (pane) {
		case setup_network_pane_player:
			setup_network_player_pane();
			break;
		case setup_network_pane_host:
			setup_network_host_pane();
			break;
	}
}

/* =======================================================

      Setup Network Operations
      
======================================================= */

void setup_network_open(void)
{	
		// setup gui
		
	gui_initialize(NULL,NULL);

		// start with first tab
		
	setup_network_tab_value=0;
	setup_network_create_pane();

		// save setup
		
	memmove(&setup_backup,&setup,sizeof(setup_type));
}

void setup_network_close(void)
{
	gui_shutdown();
}

void setup_network_done(void)
{
	server.next_state=gs_intro;
}

void setup_network_restore(void)
{
	memmove(&setup,&setup_backup,sizeof(setup_type));
}

void setup_network_save(void)
{
	setup_xml_write();
}

/* =======================================================

      Network Actions
      
======================================================= */

void setup_network_enable_buttons(void)
{
	int							host_idx;
	setup_network_host_type		*host;
	
		// get select host

	host_idx=element_get_value(ctrl_network_host_id);
	if (host_idx==-1) {

			// hide the update and disable the delete

		element_hide(setup_network_host_add_button,FALSE);
		element_hide(setup_network_host_update_button,TRUE);
		element_enable(setup_network_host_delete_button,FALSE);

			// clear the text

		element_set_value_string(ctrl_network_host_name_id,"");
		element_set_value_string(ctrl_network_host_ip_id,"");
		return;
	}

		// enable all buttons

	element_hide(setup_network_host_add_button,TRUE);
	element_hide(setup_network_host_update_button,FALSE);
	element_enable(setup_network_host_delete_button,TRUE);

		// set controls

	host=&setup.network.host.hosts[host_idx];

	element_set_value_string(ctrl_network_host_name_id,host->name);
	element_set_value_string(ctrl_network_host_ip_id,host->ip);
}

void setup_network_host_add_update(bool in_add)
{
	int							host_idx;
	setup_network_host_type		*host;
	
		// get item to add or update

	if (in_add) {
		if (setup.network.host.count>=max_setup_network_host) return;
		host_idx=setup.network.host.count;
		setup.network.host.count++;
	}
	else {
		host_idx=element_get_value(ctrl_network_host_id);
		if (host_idx==-1) return;
	}

		// add/update item

	host=&setup.network.host.hosts[host_idx];

	element_get_value_string(ctrl_network_host_name_id,host->name);
	element_get_value_string(ctrl_network_host_ip_id,host->ip);
	
		// rebuild list
		
	setup_network_create_host_list();
	element_set_table_data(ctrl_network_host_id,(char*)setup_host_list);
	element_set_value(ctrl_network_host_id,host_idx);
	setup_network_enable_buttons();
}

void setup_network_host_delete(void)
{
	int			host_idx,msz;

	if (setup.network.host.count==0) return;

		// delete item

	host_idx=element_get_value(ctrl_network_host_id);
	if (host_idx==-1) return;

	msz=(setup.network.host.count-(host_idx+1))*sizeof(setup_network_host_type);
	if (msz>0) memmove(&setup.network.host.hosts[host_idx],&setup.network.host.hosts[host_idx+1],msz);

	setup.network.host.count--;
	
		// rebuild list
		
	element_set_value(ctrl_network_host_id,-1);
		
	setup_network_create_host_list();
	element_set_table_data(ctrl_network_host_id,(char*)setup_host_list);
	setup_network_enable_buttons();
}

/* =======================================================

      Setup Network Input
      
======================================================= */

void setup_network_handle_click(int id)
{
	hud_character_item_type	*hud_character;

	switch (id) {

			// tab
			
		case ctrl_network_tab_id:
			setup_network_tab_value=element_get_value(ctrl_network_tab_id);
			setup_network_create_pane();
			return;
	
			// buttons
			
		case setup_network_ok_button:
			setup_network_save();
			setup_network_done();
			return;
			
		case setup_network_cancel_button:
			setup_network_restore();
			setup_network_done();
			return;
			
		case setup_network_host_add_button:
			setup_network_host_scroll_pos=element_get_scroll_position(ctrl_network_host_id);
			setup_network_host_add_update(TRUE);
			element_set_scroll_position(ctrl_network_host_id,setup_network_host_scroll_pos);
			return;

		case setup_network_host_update_button:
			setup_network_host_scroll_pos=element_get_scroll_position(ctrl_network_host_id);
			setup_network_host_add_update(FALSE);
			element_set_scroll_position(ctrl_network_host_id,setup_network_host_scroll_pos);
			return;

		case setup_network_host_delete_button:
			setup_network_host_scroll_pos=element_get_scroll_position(ctrl_network_host_id);
			setup_network_host_delete();
			element_set_scroll_position(ctrl_network_host_id,setup_network_host_scroll_pos);
			return;

			// controls

		case ctrl_network_name_id:
			element_get_value_string(ctrl_network_name_id,setup.network.name);
			break;

		case ctrl_network_show_names_id:
			setup.network.show_names=element_get_value(ctrl_network_show_names_id);
			break;

		case ctrl_color_id:
			setup.network.tint_color_idx=element_get_value(ctrl_color_id);
			break;
			
		case ctrl_character_id:
			setup.network.character_idx=element_get_value(ctrl_character_id);
			hud_character=&iface.character.characters[setup.network.character_idx];
			element_replace_model(ctrl_character_model_id,hud_character->model_name,"Idle",hud_character->interface_resize,&hud_character->interface_offset,NULL);
			break;

		case ctrl_network_host_id:
			setup_network_enable_buttons();
			break;
			
	}
}

void setup_network_keyboard(void)
{
	int			id;

	id=gui_keyboard();
	if (id==-1) return;

	setup_network_handle_click(id);
}

void setup_network_click(void)
{
	int			id;
	
	id=gui_click();
	if (id==-1) return;
	
	hud_click();

	setup_network_handle_click(id);
}

/* =======================================================

      Run Network Setup
      
======================================================= */

void setup_network_run(void)
{
	gui_draw(1.0f,TRUE);
	setup_network_keyboard();
	setup_network_click();
}

