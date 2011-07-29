/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Item Palette

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;
extern file_path_setup_type		file_path_setup;

extern bool						list_palette_open;
extern list_palette_type		property_palette;

list_palette_type				item_palette;

/* =======================================================

      Item Palette Setup
      
======================================================= */

void item_palette_initialize(void)
{
	list_palette_list_initialize(&item_palette,"Map Items",FALSE);

	item_palette.item_type=0;
	item_palette.item_idx=-1;
}

void item_palette_shutdown(void)
{
	list_palette_list_shutdown(&item_palette);
}

/* =======================================================

      Item Palette Fill
      
======================================================= */

void item_palette_fill(void)
{
	int			n;

	list_palette_delete_all_items(&item_palette);

		// map

	list_palette_add_header(&item_palette,spot_piece,"Map");
	list_palette_add_item(&item_palette,map_setting_piece,0,"Settings",(item_palette.item_type==map_setting_piece),FALSE);
	list_palette_add_item(&item_palette,map_camera_piece,0,"Camera",(item_palette.item_type==map_camera_piece),FALSE);
	list_palette_add_item(&item_palette,map_sky_weather_piece,0,"Sky & Weather",(item_palette.item_type==map_sky_weather_piece),FALSE);

		// spots

	list_palette_add_header_count(&item_palette,spot_piece,"Spots",map.nspot);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=map.nspot;n++) {
		list_palette_add_item(&item_palette,spot_piece,n,map.spots[n].name,((item_palette.item_type==spot_piece)&&(n==item_palette.item_idx)),FALSE);
	}

	list_palette_sort(&item_palette);

		// lights

	list_palette_add_header_count(&item_palette,light_piece,"Lights",map.nlight);

	for (n=0;n!=map.nlight;n++) {
		list_palette_add_color(&item_palette,light_piece,n,&map.lights[n].setting.col,((item_palette.item_type==light_piece)&&(n==item_palette.item_idx)),FALSE);
	}

		// sounds

	list_palette_add_header_count(&item_palette,sound_piece,"Sounds",map.nsound);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=map.nsound;n++) {
		list_palette_add_item(&item_palette,sound_piece,n,map.sounds[n].name,((item_palette.item_type==sound_piece)&&(n==item_palette.item_idx)),FALSE);
	}

	list_palette_sort(&item_palette);

		// particles

	list_palette_add_header_count(&item_palette,particle_piece,"Particles",map.nparticle);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=map.nparticle;n++) {
		list_palette_add_item(&item_palette,particle_piece,n,map.particles[n].name,((item_palette.item_type==particle_piece)&&(n==item_palette.item_idx)),FALSE);
	}

	list_palette_sort(&item_palette);

		// sceneries

	list_palette_add_header_count(&item_palette,scenery_piece,"Scenery",map.nscenery);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=map.nscenery;n++) {
		list_palette_add_item(&item_palette,scenery_piece,n,map.sceneries[n].model_name,((item_palette.item_type==scenery_piece)&&(n==item_palette.item_idx)),FALSE);
	}

	list_palette_sort(&item_palette);

		// nodes

	list_palette_add_header_count(&item_palette,node_piece,"Nodes",map.nnode);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=map.nnode;n++) {
		if (map.nodes[n].name[0]!=0x0) list_palette_add_item(&item_palette,node_piece,n,map.nodes[n].name,((item_palette.item_type==node_piece)&&(n==item_palette.item_idx)),FALSE);
	}

	list_palette_sort(&item_palette);

		// groups

	list_palette_add_header_count(&item_palette,group_piece,"Groups",map.group.ngroup);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=map.group.ngroup;n++) {
		list_palette_add_item(&item_palette,group_piece,n,map.group.groups[n].name,((item_palette.item_type==group_piece)&&(n==item_palette.item_idx)),FALSE);
	}

	list_palette_sort(&item_palette);

		// movements

	list_palette_add_header_count(&item_palette,movement_piece,"Movements",map.movement.nmovement);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=map.movement.nmovement;n++) {
		list_palette_add_item(&item_palette,movement_piece,n,map.movement.movements[n].name,((item_palette.item_type==movement_piece)&&(n==item_palette.item_idx)),FALSE);
	}

	list_palette_sort(&item_palette);

		// cinemas

	list_palette_add_header_count(&item_palette,cinema_piece,"Cinemas",map.cinema.ncinema);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=map.cinema.ncinema;n++) {
		list_palette_add_item(&item_palette,cinema_piece,n,map.cinema.cinemas[n].name,((item_palette.item_type==cinema_piece)&&(n==item_palette.item_idx)),FALSE);
	}

	list_palette_sort(&item_palette);
}

/* =======================================================

      Item Palette Draw
      
======================================================= */

void item_palette_draw(void)
{
	if (list_palette_get_level()!=0) return;
	
	item_palette_fill();
	list_palette_draw(&item_palette);
}

/* =======================================================

      Item Palette Reset For Selection Change
      
======================================================= */

void item_palette_scroll_into_view(int item_type,int item_idx)
{
	item_palette_fill();
	list_palette_scroll_item_into_view(&item_palette,item_type,item_idx);
}

void item_palette_reset(void)
{
	int				sel_type,main_idx,sub_idx;

	if (select_count()==0) {
		item_palette.item_type=map_setting_piece;
		item_palette.item_idx=-1;

		item_palette_scroll_into_view(map_setting_piece,0);
		return;
	}
	
	select_get(0,&sel_type,&main_idx,&sub_idx);
	if ((sel_type==mesh_piece) || (sel_type==liquid_piece)) {
		item_palette.item_type=-1;
		item_palette.item_idx=-1;
		return;
	}

	item_palette.item_type=sel_type;
	item_palette.item_idx=main_idx;

	item_palette_scroll_into_view(sel_type,main_idx);

		// alt window items

	state.cur_cinema_idx=-1;
	state.cur_cinema_action_idx=-1;
	state.cur_movement_idx=-1;
	state.cur_movement_move_idx=-1;

		// reset windows

	property_palette_reset();
}

void item_palette_select(int sel_type,int sel_idx)
{
	item_palette.item_type=sel_type;
	item_palette.item_idx=sel_idx;
	
	item_palette_scroll_into_view(sel_type,sel_idx);

	main_wind_draw();
}

/* =======================================================

      Item Palette Delete
      
======================================================= */

bool item_palette_delete(void)
{
	int					n,k;
	bool				del_ok;
	
		// anything to delete?

	if ((item_palette.item_type==-1) || (item_palette.item_idx==-1)) return(FALSE);

	switch (item_palette.item_type) {

			// delete group
			
		case group_piece:
			
				// check if this group is used within a movement
				
			del_ok=TRUE;
			
			for (n=0;n!=map.movement.nmovement;n++) {
				if ((map.movement.movements[n].group_idx==item_palette.item_idx) || (map.movement.movements[n].reverse_group_idx==item_palette.item_idx)) {
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
			map_group_delete(&map,item_palette.item_idx);
			item_palette_reset();
			return(TRUE);

			// delete movement
			
		case movement_piece:
		
				// check if movement is used within a cinema
				
			del_ok=TRUE;
			
			for (n=0;n!=map.cinema.ncinema;n++) {
				for (k=0;k!=map.cinema.cinemas[n].naction;k++) {
					if (map.cinema.cinemas[n].actions[k].actor_type==cinema_actor_movement) {
						if (strcasecmp(map.cinema.cinemas[n].actions[k].actor_name,map.movement.movements[item_palette.item_idx].name)==0) {
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
			map_movement_delete(&map,item_palette.item_idx);
			item_palette_reset();
			return(TRUE);

			// delete cinema
			
		case cinema_piece:
			if (os_dialog_confirm("Delete Cinema","Is it okay to delete this cinema?",FALSE)!=0) return(FALSE);
			map_cinema_delete(&map,item_palette.item_idx);
			item_palette_reset();
			return(TRUE);

	}

	return(FALSE);
}

/* =======================================================

      Item Palette Scroll Wheel
      
======================================================= */

void item_palette_scroll_wheel(d3pnt *pnt,int move)
{
	if (list_palette_get_level()==0) list_palette_scroll_wheel(&item_palette,pnt,move);
}

/* =======================================================

      Item Palette Click
      
======================================================= */

bool item_palette_click(d3pnt *pnt,bool double_click)
{
	bool					old_open;
	
	if (list_palette_get_level()!=0) return(FALSE);

		// check if open changes
	
	old_open=list_palette_open;

		// click

	if (!list_palette_click(&item_palette,pnt,double_click)) {
		if (old_open!=list_palette_open) main_wind_draw();
		return(TRUE);
	}

	if (item_palette.item_idx==-1) return(TRUE);

		// alt window items

	state.cur_cinema_idx=-1;
	state.cur_cinema_action_idx=-1;
	state.cur_movement_idx=-1;
	state.cur_movement_move_idx=-1;

		// turn off preferences

	state.in_preference=FALSE;

		// handle click

	select_clear();

	switch (item_palette.item_type) {

		case map_setting_piece:
		case map_camera_piece:
		case map_sky_weather_piece:
			select_clear();
			break;

		case group_piece:
			select_add_group(item_palette.item_idx);
			break;

		case movement_piece:
			state.cur_movement_idx=item_palette.item_idx;
			state.cur_movement_move_idx=-1;
			select_add_movement(item_palette.item_idx);
			break;

		case cinema_piece:
			state.cur_cinema_idx=item_palette.item_idx;
			state.cur_cinema_action_idx=-1;
			select_add_cinema(item_palette.item_idx);
			break;

		default:
			select_add(item_palette.item_type,item_palette.item_idx,-1);
			break;

	}

	if (double_click) view_goto_select();

		// turn on any hidden items

	switch (item_palette.item_type) {

		case spot_piece:
		case scenery_piece:
			state.show_object=TRUE;
			break;

		case light_piece:
		case sound_piece:
		case particle_piece:
			state.show_lightsoundparticle=TRUE;
			break;

		case node_piece:
			state.show_node=TRUE;
			break;
	}
	
		// open the property window for double-click
		
	if (double_click) {
		property_palette_reset();
		list_palette_set_level(1);
	}

	main_wind_draw();
	
	return(TRUE);
}