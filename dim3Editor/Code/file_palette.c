/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: File Palette

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

#define kPropertyAddSpot			0
#define kPropertyAddLight			1
#define kPropertyAddSound			2
#define kPropertyAddParticle		3
#define kPropertyAddScenery			4
#define kPropertyAddNode			5
#define kPropertyAddGroup			6
#define kPropertyAddMovement		7
#define kPropertyAddCinema			8

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		property_palette;

list_palette_type				file_palette;

/* =======================================================

      Item Palette Setup
      
======================================================= */

void file_palette_initialize(void)
{
	list_palette_list_initialize(&file_palette,"Maps");

	file_palette.item_type=0;
	file_palette.item_idx=-1;
}

void file_palette_shutdown(void)
{
	list_palette_list_shutdown(&file_palette);
}

/* =======================================================

      File Palette Fill
      
======================================================= */
/*
void item_palette_fill(void)
{
	int			n;

	list_palette_delete_all_items(&item_palette);

		// map

	list_palette_add_header(&item_palette,spot_piece,"Map");
	list_palette_add_item(&item_palette,map_setting_piece,0,"Settings",(item_palette.item_type==map_setting_piece),FALSE);
	list_palette_add_item(&item_palette,map_camera_piece,0,"Camera",(item_palette.item_type==map_camera_piece),FALSE);
	list_palette_add_item(&item_palette,map_light_media_piece,0,"Light & Media",(item_palette.item_type==map_light_media_piece),FALSE);
	list_palette_add_item(&item_palette,map_sky_weather_piece,0,"Sky & Weather",(item_palette.item_type==map_sky_weather_piece),FALSE);

		// spots

	list_palette_add_header_button(&item_palette,kPropertyAddSpot,"Spots",list_button_plus);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=map.nspot;n++) {
		if (map.spots[n].name[0]!=0x0) {
			list_palette_add_item(&item_palette,spot_piece,n,map.spots[n].name,((item_palette.item_type==spot_piece)&&(n==item_palette.item_idx)),FALSE);
		}
		else {
			list_palette_add_item(&item_palette,spot_piece,n,"[none]",((item_palette.item_type==spot_piece)&&(n==item_palette.item_idx)),FALSE);
		}
	}

	list_palette_sort(&item_palette);

		// lights

	list_palette_add_header_button(&item_palette,kPropertyAddLight,"Lights",list_button_plus);

	for (n=0;n!=map.nlight;n++) {
		list_palette_add_color(&item_palette,light_piece,n,&map.lights[n].setting.col,((item_palette.item_type==light_piece)&&(n==item_palette.item_idx)),FALSE);
	}

		// sounds

	list_palette_add_header_button(&item_palette,kPropertyAddSound,"Sounds",list_button_plus);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=map.nsound;n++) {
		list_palette_add_item(&item_palette,sound_piece,n,map.sounds[n].name,((item_palette.item_type==sound_piece)&&(n==item_palette.item_idx)),FALSE);
	}

	list_palette_sort(&item_palette);

		// particles

	list_palette_add_header_button(&item_palette,kPropertyAddParticle,"Particles",list_button_plus);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=map.nparticle;n++) {
		list_palette_add_item(&item_palette,particle_piece,n,map.particles[n].name,((item_palette.item_type==particle_piece)&&(n==item_palette.item_idx)),FALSE);
	}

	list_palette_sort(&item_palette);

		// sceneries

	list_palette_add_header_button(&item_palette,kPropertyAddScenery,"Scenery",list_button_plus);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=map.nscenery;n++) {
		list_palette_add_item(&item_palette,scenery_piece,n,map.sceneries[n].model_name,((item_palette.item_type==scenery_piece)&&(n==item_palette.item_idx)),FALSE);
	}

	list_palette_sort(&item_palette);

		// nodes

	list_palette_add_header_button(&item_palette,kPropertyAddNode,"Nodes",list_button_plus);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=map.nnode;n++) {
		if (map.nodes[n].name[0]!=0x0) list_palette_add_item(&item_palette,node_piece,n,map.nodes[n].name,((item_palette.item_type==node_piece)&&(n==item_palette.item_idx)),FALSE);
	}

	list_palette_sort(&item_palette);

		// groups

	list_palette_add_header_button(&item_palette,kPropertyAddGroup,"Groups",list_button_plus);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=map.group.ngroup;n++) {
		list_palette_add_item(&item_palette,group_piece,n,map.group.groups[n].name,((item_palette.item_type==group_piece)&&(n==item_palette.item_idx)),FALSE);
	}

	list_palette_sort(&item_palette);

		// movements

	list_palette_add_header_button(&item_palette,kPropertyAddMovement,"Movements",list_button_plus);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=map.movement.nmovement;n++) {
		list_palette_add_item(&item_palette,movement_piece,n,map.movement.movements[n].name,((item_palette.item_type==movement_piece)&&(n==item_palette.item_idx)),FALSE);
	}

	list_palette_sort(&item_palette);

		// cinemas

	list_palette_add_header_button(&item_palette,kPropertyAddCinema,"Cinemas",list_button_plus);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=map.cinema.ncinema;n++) {
		list_palette_add_item(&item_palette,cinema_piece,n,map.cinema.cinemas[n].name,((item_palette.item_type==cinema_piece)&&(n==item_palette.item_idx)),FALSE);
	}

	list_palette_sort(&item_palette);
}
*/
/* =======================================================

      File Palette Draw
      
======================================================= */

void file_palette_draw(void)
{
/*
	if (list_palette_get_level(&file_palette)!=0) return;
	
	item_palette_fill();
	list_palette_draw(&item_palette);
	*/
}

/* =======================================================

      File Palette Scroll Wheel
      
======================================================= */

void file_palette_scroll_wheel(d3pnt *pnt,int move)
{
//	if (list_palette_get_level(&file_palette)==0) list_palette_scroll_wheel(&item_palette,pnt,move);
}

/* =======================================================

      File Palette Click
      
======================================================= */

bool file_palette_click(d3pnt *pnt,bool double_click)
{
/*
	bool					old_open;
	
	if (list_palette_get_level(&file_palette)!=0) return(FALSE);

		// check if open changes
	
	old_open=list_palette_is_open(&file_palette);

		// click

	if (!list_palette_click(&file_palette,pnt,double_click)) {
		if (old_open!=list_palette_is_open(&file_palette)) main_wind_draw();
		return(TRUE);
	}

		// add buttons

	switch (item_palette.item_id) {

		case kPropertyAddSpot:
			piece_create_spot();
			break;

		case kPropertyAddLight:
			piece_create_light();
			break;

		case kPropertyAddSound:
			piece_create_sound();
			break;

		case kPropertyAddParticle:
			piece_create_particle();
			break;

		case kPropertyAddScenery:
			piece_create_scenery();
			break;

		case kPropertyAddNode:
			piece_create_node();
			break;

		case kPropertyAddGroup:
			piece_create_group();
			break;

		case kPropertyAddMovement:
			piece_create_movement();
			break;

		case kPropertyAddCinema:
			piece_create_cinema();
			break;
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
		case map_light_media_piece:
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
		list_palette_set_level(&file_palette,1);
	}

	main_wind_draw();
	*/
	return(TRUE);
}