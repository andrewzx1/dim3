/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Singleplayer Option

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

#define singleplayer_option_frame_id			0

#define singleplayer_option_skill_id			10
#define singleplayer_option_map_table_id		11

#define singleplayer_option_button_ok_id		20
#define singleplayer_option_button_cancel_id	21

#define singleplayer_option_option_start_id		100

#define singleplayer_option_table_high			310

extern int					intro_simple_save_idx;

extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;

bool						singleplayer_option_esc_down;
char						skill_list[6][32]={"Easy","Medium","Hard",""};
char						*singleplayer_option_table_map_list;

/* =======================================================

      Singleplayer Option Map Lists
      
======================================================= */

void singleplayer_option_map_list_initialize(void)
{
	singleplayer_option_table_map_list=NULL;
}

void singleplayer_option_map_list_fill(void)
{
	int							n,nfile,sz;
	char						*c;
	char						info_name[name_str_len],game_list[256];
	file_path_directory_type	*map_pick_fpd;

		// need to make sure map paths are correct

	map_setup(&setup.file_path_setup,setup.anisotropic,setup.mipmap_mode,TRUE,view_shader_on());

		// load in all maps

	map_pick_fpd=file_paths_read_directory_data(&setup.file_path_setup,"Maps","xml");

	nfile=map_pick_fpd->nfile;
	if (nfile<=0) {
		file_paths_close_directory(map_pick_fpd);
		return;
	}

		// data for maps

	sz=(nfile+1)*128;
	singleplayer_option_table_map_list=malloc(sz);
	bzero(singleplayer_option_table_map_list,sz);

		// load the maps

	c=singleplayer_option_table_map_list;
	
	for (n=0;n!=nfile;n++) {
		if (!map_host_load_info(map_pick_fpd->files[n].file_name,info_name,game_list)) continue;

		sprintf(c,"Bitmaps/Icons_Map;%s;%s",map_pick_fpd->files[n].file_name,info_name);
		c+=128;
	}

	*c=0x0;
	element_set_table_data(singleplayer_option_map_table_id,singleplayer_option_table_map_list);

		// close the directory scan

	file_paths_close_directory(map_pick_fpd);
}

void singleplayer_option_map_list_release(void)
{
	if (singleplayer_option_table_map_list!=NULL) free(singleplayer_option_table_map_list);
}

/* =======================================================

      Singleplayer Option Open and Close
      
======================================================= */

void singleplayer_option_open(void)
{
	int						n,x,y,bx,by,wid,high,
							padding,control_y_add;
	element_column_type		cols[1];
	iface_sp_option_type	*sp_option;
	iface_intro_model_type	*intro_model;

	singleplayer_option_map_list_initialize();

		// intro UI
		
	gui_initialize("Bitmaps/Backgrounds","main");

		// models

	intro_model=iface.intro.model_list.models;

	for (n=0;n!=iface.intro.model_list.nmodel;n++) {
		element_model_add(intro_model->model_name,intro_model->animate_name,intro_model->resize,NULL,&intro_model->rot,-1,intro_model->x,intro_model->y);
		intro_model++;
	}

		// get height

	padding=element_get_padding();
	control_y_add=element_get_control_high();

	high=50+(padding*4);

	if (iface.singleplayer.skill) high+=control_y_add;
	if (iface.singleplayer.map_pick) high+=(singleplayer_option_table_high+(padding*2));
	high+=(control_y_add*iface.singleplayer.option_list.noption);
	
		// dialog and frame

	x=25;
	y=(iface.scale_y-(high-control_y_add))>>1;
	wid=iface.scale_x-50;
	
	element_frame_add("New Game",singleplayer_option_frame_id,x,y,wid,high);

		// controls

	bx=x+(wid/3);
	by=(y+control_y_add)+padding;

	if (iface.singleplayer.skill) {
		element_combo_add("Skill",(char*)skill_list,skill_medium,singleplayer_option_skill_id,bx,by,TRUE);
		by+=control_y_add;
	}

	sp_option=iface.singleplayer.option_list.options;

	for (n=0;n!=iface.singleplayer.option_list.noption;n++) {
		element_checkbox_add(sp_option->descript,FALSE,(singleplayer_option_option_start_id+n),bx,by,TRUE);
		by+=control_y_add;
		sp_option++;
	}

		// map pick

	if (iface.singleplayer.map_pick) {
		strcpy(cols[0].name,"Map");
		cols[0].percent_size=1.0f;

		element_table_add(cols,NULL,singleplayer_option_map_table_id,1,(x+padding),(by-padding),(wid-(padding*2)),singleplayer_option_table_high,FALSE,element_table_bitmap_data);
	
		singleplayer_option_map_list_fill();
	}

		// buttons

	bx=(x+wid)-padding;
	by=(y+high)-padding;

	element_button_text_add("Play",singleplayer_option_button_ok_id,bx,by,100,50,element_pos_right,element_pos_bottom);
	element_button_text_add("Cancel",singleplayer_option_button_cancel_id,((bx-100)-padding),by,100,50,element_pos_right,element_pos_bottom);
	
		// in key state
	
	singleplayer_option_esc_down=FALSE;
}

void singleplayer_option_close(void)
{
		// release lists

	singleplayer_option_map_list_release();

		// shutdown UI
		
	gui_shutdown();
}

/* =======================================================

      Singleplayer Option Input
      
======================================================= */

void singleplayer_option_click(void)
{
	int				n,id,skill,option_flags;
	
		// element being clicked?
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
		// regular button clicks
		
	switch (id) {
	
		case singleplayer_option_button_ok_id:

				// get setup

			skill=element_get_value(singleplayer_option_skill_id);
			for (n=0;n!=iface.singleplayer.option_list.noption;n++) {
				if (element_get_value(singleplayer_option_option_start_id+n)!=0) option_flags|=(0x1<<n);
			}

				// start game

			intro_start_game(skill,option_flags,intro_simple_save_idx);
			break;

		case singleplayer_option_button_cancel_id:
			server.next_state=gs_intro;
			break;

	}
}

void singleplayer_option_key(void)
{
		// check for esc
		
	if (!input_get_keyboard_escape()) {
		singleplayer_option_esc_down=FALSE;
		return;
	}
	
	if (singleplayer_option_esc_down) return;
	
	hud_click();
	
	singleplayer_option_esc_down=TRUE;

		// escape cancels

	server.next_state=gs_intro;
}

/* =======================================================

      Run Singleplayer Option Page
      
======================================================= */

void singleplayer_option_run(void)
{
	gui_draw(1.0f,TRUE);
	singleplayer_option_click();
	singleplayer_option_key();
}


