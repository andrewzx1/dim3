/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;
extern iface_type				iface;
extern file_path_setup_type		file_path_setup;

char							list_texture_names[max_map_texture][name_str_len];

list_palette_type				property_palette;

/* =======================================================

      Property Palette Setup
      
======================================================= */

void property_palette_initialize(void)
{
	list_palette_list_initialize(&property_palette,"Item Properties",FALSE,FALSE,FALSE);

	property_palette.item_pane.click.id=0;
	property_palette.item_pane.click.idx=-1;
	property_palette.item_pane.click.item=NULL;
}

void property_palette_shutdown(void)
{
	list_palette_list_shutdown(&property_palette);
}

/* =======================================================

      Property Palette Fills
      
======================================================= */

void property_palette_fill_level_0(void)
{
	property_palette_fill_main();
}

void property_palette_fill_level_1(void)
{
	int					sel_type,main_idx,sub_idx;

		// special check for non-selection property lists
	
	switch (state.cur_no_sel_piece_type) {

		case map_setting_piece:
			property_palette_fill_map();
			return;

		case map_camera_piece:
			property_palette_fill_camera();
			return;

		case map_light_media_piece:
			property_palette_fill_light_media();
			return;

		case map_sky_weather_piece:
			property_palette_fill_sky_weather();
			return;

		case group_piece:
			property_palette_fill_group(state.cur_group_idx);
			return;
	
		case movement_piece:
			property_palette_fill_movement(state.cur_movement_idx);
			return;

		case cinema_piece:
			property_palette_fill_cinema(state.cur_cinema_idx);
			return;
	}

		// check selection

	main_idx=-1;
	
	if (select_count()!=0)  select_get(0,&sel_type,&main_idx,&sub_idx);
	if (main_idx==-1) return;

		// selection properties

	switch (sel_type) {

		case mesh_piece:
			if (state.drag_mode!=drag_mode_polygon) sub_idx=-1;
			property_palette_fill_mesh(main_idx,sub_idx);
			break;

		case liquid_piece:
			property_palette_fill_liquid(main_idx);
			break;

		case spot_piece:
			property_palette_fill_spot(main_idx);
			break;

		case light_piece:
			property_palette_fill_light(main_idx);
			break;

		case sound_piece:
			property_palette_fill_sound(main_idx);
			break;

		case particle_piece:
			property_palette_fill_particle(main_idx);
			break;

		case scenery_piece:
			property_palette_fill_scenery(main_idx);
			break;

		case node_piece:
			property_palette_fill_node(main_idx);
			break;

	}
}

void property_palette_fill_level_2(void)
{
		// selection properties

	switch (state.cur_no_sel_piece_type) {

		case movement_piece:
			property_palette_fill_movement_move(state.cur_movement_idx,state.cur_movement_move_idx);
			break;

		case cinema_piece:
			property_palette_fill_cinema_action(state.cur_cinema_idx,state.cur_cinema_action_idx);
			break;
	}
}

/* =======================================================

      Property Palette Draw
      
======================================================= */

void property_palette_draw(void)
{
		// delete the properties

	list_palette_delete_all_items(&property_palette);

		// if texture window is up,
		// put in texture properties

	if (state.texture_edit_idx!=-1) {
		property_palette_fill_texture(state.texture_edit_idx);
		list_palette_draw(&property_palette);
		return;
	}

		// if preference window is up,
		// put in preferences

	if (state.in_preference) {
		property_palette_fill_editor_preference();
		list_palette_draw(&property_palette);
		return;
	}

		// fill the property list
		
	switch (list_palette_get_level(&property_palette)) {
		case 0:
			property_palette_fill_level_0();
			break;
		case 1:
			property_palette_fill_level_1();
			break;
		case 2:
			property_palette_fill_level_2();
			break;
	}
	
	list_palette_draw(&property_palette);
}

/* =======================================================

      Property Palette Scrolling
      
======================================================= */

void property_palette_reset(void)
{
	int				sel_type,main_idx,sub_idx;
	
		// clear out non-selection indexes

	state.cur_group_idx=-1;
	state.cur_movement_idx=-1;
	state.cur_movement_move_idx=-1;
	state.cur_cinema_idx=-1;
	state.cur_cinema_action_idx=-1;

		// if no select, change nothing

	if (select_count()==0) {
		state.cur_no_sel_piece_type=-1;
		list_palette_set_level(&property_palette,0);
		property_palette_scroll_into_view(map_setting_piece,0);
		return;
	}
	
		// set the type

	select_get(0,&sel_type,&main_idx,&sub_idx);
	state.cur_no_sel_piece_type=sel_type;
	
	list_palette_set_level(&property_palette,1);
	
	if ((sel_type!=mesh_piece) && (sel_type!=liquid_piece)) property_palette_scroll_into_view(sel_type,main_idx);
}

void property_palette_scroll_into_view(int item_id,int item_idx)
{
		// can only scroll into view
		// on map items page

	if (!list_palette_get_level(&property_palette)!=0) return;

	property_palette_fill_level_0();
	list_palette_scroll_item_into_view(&property_palette,item_id,item_idx);

	main_wind_draw();
}

void property_palette_scroll_wheel(d3pnt *pnt,int move)
{
	list_palette_scroll_wheel(&property_palette,pnt,move);
}

/* =======================================================

      Property Palette Delete
      
======================================================= */

bool property_palette_delete(void)
{
	int					n,k;
	bool				del_ok;
	
		// can only delete on map
		// item list

	if (list_palette_get_level(&property_palette)!=0) return(FALSE);

		// anything to delete?

	switch (state.cur_no_sel_piece_type) {

			// delete group
			
		case group_piece:
			
				// check if this group is used within a movement
				
			del_ok=TRUE;
			
			for (n=0;n!=map.movement.nmovement;n++) {
				if ((map.movement.movements[n].group_idx==state.cur_group_idx) || (map.movement.movements[n].reverse_group_idx==state.cur_group_idx)) {
					del_ok=FALSE;
					break;
				}
			}
			
			if (!del_ok) {
				os_dialog_alert("Delete Group","This group is being used in a movement, it can not be deleted");
				return(TRUE);
			}
				
				// delete group
				
			if (os_dialog_confirm("Delete Group","Is it okay to delete this group?",FALSE)!=0) return(FALSE);
			map_group_delete(&map,state.cur_group_idx);
			property_palette_reset();
			return(TRUE);

			// delete movement
			
		case movement_piece:
		
				// check if movement is used within a cinema
				
			del_ok=TRUE;
			
			for (n=0;n!=map.cinema.ncinema;n++) {
				for (k=0;k!=map.cinema.cinemas[n].naction;k++) {
					if (map.cinema.cinemas[n].actions[k].actor_type==cinema_actor_movement) {
						if (strcasecmp(map.cinema.cinemas[n].actions[k].actor_name,map.movement.movements[state.cur_movement_idx].name)==0) {
							del_ok=FALSE;
							break;
						}
					}
				}
			}
			
			if (!del_ok) {
				os_dialog_alert("Delete Movement","This movement is being used in a cinema, it can not be deleted");
				return(TRUE);
			}
			
				// delete movement
				
			if (os_dialog_confirm("Delete Movement","Is it okay to delete this movement?",FALSE)!=0) return(FALSE);
			map_movement_delete(&map,state.cur_movement_idx);
			property_palette_reset();
			return(TRUE);

			// delete cinema
			
		case cinema_piece:
			if (os_dialog_confirm("Delete Cinema","Is it okay to delete this cinema?",FALSE)!=0) return(FALSE);
			map_cinema_delete(&map,state.cur_cinema_idx);
			property_palette_reset();
			return(TRUE);

	}

	return(FALSE);
}

/* =======================================================

      Property Palette Click For Levels
      
======================================================= */

void property_palette_click_level_0(bool double_click)
{
	property_palette_click_main(double_click);
}
	
void property_palette_click_level_1(bool double_click)
{
	int					sel_type,main_idx,sub_idx;
	
		// special check for non-selection property lists
		// and check state changes

	switch (state.cur_no_sel_piece_type) {

		case group_piece:
			property_palette_click_group(double_click);
			return;

		case movement_piece:
			property_palette_click_movement(double_click);
			return;

		case cinema_piece:
			property_palette_click_cinema(double_click);
			return;
	}

		// get the selection

	main_idx=-1;
	
	if (select_count()!=0) select_get(0,&sel_type,&main_idx,&sub_idx);

		// no selection, map properties

	if (main_idx==-1) {

		switch (state.cur_no_sel_piece_type) {
			
			case map_setting_piece:
				property_palette_click_map(double_click);
				break;

			case map_camera_piece:
				property_palette_click_camera(double_click);
				break;

			case map_light_media_piece:
				property_palette_click_light_media(double_click);
				break;

			case map_sky_weather_piece:
				property_palette_click_sky_weather(double_click);
				break;

		}

		return;
	}

		// selection properties

	switch (sel_type) {

		case mesh_piece:
			if (state.drag_mode!=drag_mode_polygon) sub_idx=-1;
			property_palette_click_mesh(main_idx,sub_idx,double_click);
			break;

		case liquid_piece:
			property_palette_click_liquid(main_idx,double_click);
			break;

		case spot_piece:
			property_palette_click_spot(main_idx,double_click);
			break;

		case light_piece:
			property_palette_click_light(main_idx,double_click);
			break;

		case sound_piece:
			property_palette_click_sound(main_idx,double_click);
			break;

		case particle_piece:
			property_palette_click_particle(main_idx,double_click);
			break;

		case scenery_piece:
			property_palette_click_scenery(main_idx,double_click);
			break;

		case node_piece:
			property_palette_click_node(main_idx,double_click);
			break;

	}
}

void property_palette_click_level_2(bool double_click)
{
		// selection properties

	switch (state.cur_no_sel_piece_type) {

		case movement_piece:
			property_palette_click_movement_move(state.cur_movement_idx,state.cur_movement_move_idx,double_click);
			return;

		case cinema_piece:
			property_palette_click_cinema_action(state.cur_cinema_idx,state.cur_cinema_action_idx,double_click);
			break;
	}
}

/* =======================================================

      Property Palette Click MainLine
      
======================================================= */

void property_palette_click(d3pnt *pnt,bool double_click)
{
	bool				old_open;
	
		// check if open changes
	
	old_open=list_palette_is_open(&property_palette);

		// click

	if (!list_palette_click(&property_palette,pnt,double_click)) {
		if (old_open!=list_palette_is_open(&property_palette)) main_wind_draw();
		return;
	}
		
		// if texture window is up, texture properties

	if (state.texture_edit_idx!=-1) {
		property_palette_click_texture(state.texture_edit_idx,property_palette.item_pane.click.id,double_click);
		main_wind_draw();
		return;
	}

		// if preference window is up, preference properties

	if (state.in_preference) {
		property_palette_click_editor_preference(property_palette.item_pane.click.id,double_click);
		main_wind_draw();
		return;
	}
	
		// click by level
		
	switch (list_palette_get_level(&property_palette)) {
		case 0:
			property_palette_click_level_0(double_click);
			break;
		case 1:
			property_palette_click_level_1(double_click);
			break;
		case 2:
			property_palette_click_level_2(double_click);
			break;
	}
	
	main_wind_draw();
}

/* =======================================================

      Property Palette List Utilities
      
======================================================= */

void property_palette_pick_group(int *group_idx)
{
	list_palette_start_picking_mode(&property_palette,"Pick a Group",(char*)map.group.groups,map.group.ngroup,sizeof(group_type),(int)offsetof(group_type,name),TRUE,FALSE,group_idx,NULL);
}

void property_palette_pick_spot(char *name)
{
	list_palette_start_picking_mode(&property_palette,"Pick a Spot",(char*)map.spots,map.nspot,sizeof(spot_type),(int)offsetof(spot_type,name),TRUE,FALSE,NULL,name);
}

void property_palette_pick_sound(char *name,bool include_none)
{
	list_palette_start_picking_mode(&property_palette,"Pick a Sound",(char*)iface.sound_list.sounds,iface.sound_list.nsound,sizeof(iface_sound_type),(int)offsetof(iface_sound_type,name),include_none,FALSE,NULL,name);
}

void property_palette_pick_halo(char *name)
{
	list_palette_start_picking_mode(&property_palette,"Pick a Halo",(char*)iface.halo_list.halos,iface.halo_list.nhalo,sizeof(iface_halo_type),(int)offsetof(iface_halo_type,name),TRUE,FALSE,NULL,name);
}

void property_palette_pick_particle(char *name)
{
	list_palette_start_picking_mode(&property_palette,"Pick a Particle",(char*)iface.particle_list.particles,iface.particle_list.nparticle,sizeof(iface_particle_type),(int)offsetof(iface_particle_type,name),FALSE,FALSE,NULL,name);
}

void property_palette_pick_node(char *name)
{
	int				n,count,mem_sz,list_pos,idx;
	char			*list_ptr,*list;
	
		// only pick from nodes with names
		
	count=0;
	
	for (n=0;n!=map.nnode;n++) {
		if (map.nodes[n].name[0]!=0x0) count++;
	}
	
	if (count==0) {
		mem_sz=name_str_len;
	}
	else {
		mem_sz=count*name_str_len;
	}
	
	list_ptr=(char*)malloc(mem_sz);
	if (list_ptr==NULL) return;
	
	idx=-1;
	list_pos=0;
	
	for (n=0;n!=map.nnode;n++) {
		if (map.nodes[n].name[0]==0x0) continue;
		
		list=list_ptr+(list_pos*name_str_len);
		strcpy(list,map.nodes[n].name);
			
		if (strcmp(map.nodes[n].name,name)==0) idx=list_pos;
		list_pos++;
	}
	
	list_palette_start_picking_mode(&property_palette,"Pick a Node",list_ptr,count,name_str_len,0,TRUE,FALSE,NULL,name);
	
	free(list_ptr);
}

void property_palette_pick_movement(char *name)
{
	list_palette_start_picking_mode(&property_palette,"Pick a Movement",(char*)map.movement.movements,map.movement.nmovement,sizeof(movement_type),(int)offsetof(movement_type,name),TRUE,FALSE,NULL,name);
}

void property_palette_pick_hud_text(char *name)
{
	list_palette_start_picking_mode(&property_palette,"Pick a HUD Text",(char*)iface.text_list.texts,iface.text_list.ntext,sizeof(iface_text_type),(int)offsetof(iface_text_type,name),FALSE,FALSE,NULL,name);
}

void property_palette_pick_hud_bitmap(char *name)
{
	list_palette_start_picking_mode(&property_palette,"Pick a HUD Bitmap",(char*)iface.bitmap_list.bitmaps,iface.bitmap_list.nbitmap,sizeof(iface_bitmap_type),(int)offsetof(iface_bitmap_type,name),FALSE,FALSE,NULL,name);
}

void property_palette_pick_texture(char *title,int *txt_idx)
{
	int				n;
	
	for (n=0;n!=max_map_texture;n++) {
		if (map.textures[n].frames[0].name[0]==0x0) {
			strcpy((char*)list_texture_names[n],"(none)");
		}
		else {
			strcpy((char*)list_texture_names[n],map.textures[n].frames[0].name);
		}
	}
	
	if (title==NULL) {
		list_palette_start_picking_mode(&property_palette,"Pick a Texture",(char*)list_texture_names,max_map_texture,name_str_len,0,TRUE,FALSE,txt_idx,NULL);
	}
	else {
		list_palette_start_picking_mode(&property_palette,title,(char*)list_texture_names,max_map_texture,name_str_len,0,TRUE,FALSE,txt_idx,NULL);
	}
}
