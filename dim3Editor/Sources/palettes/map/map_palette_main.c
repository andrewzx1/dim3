/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Map Camera

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

#define kPropertyAddSpot			100		// this numbers need to be greater than the regular piece numbers
#define kPropertyAddLight			101
#define kPropertyAddSound			102
#define kPropertyAddParticle		103
#define kPropertyAddScenery			104
#define kPropertyAddNode			105
#define kPropertyAddGroup			106
#define kPropertyAddMovement		107
#define kPropertyAddCinema			108

extern map_type					map;
extern app_state_type			state;

extern list_palette_type		map_palette;

/* =======================================================

      Property Palette Fill Main
      
======================================================= */

void map_palette_fill_main(void)
{
	int			n;

		// title
		
	list_palette_set_title(&map_palette,"Map",NULL,NULL,NULL,NULL,NULL);

		// map

	list_palette_add_header(&map_palette,spot_piece,"Settings");
	list_palette_add_item(&map_palette,map_setting_piece,0,"General",(map_palette.item_pane.click.id==map_setting_piece),FALSE);
	list_palette_add_item(&map_palette,map_camera_piece,0,"Camera",(map_palette.item_pane.click.id==map_camera_piece),FALSE);
	list_palette_add_item(&map_palette,map_light_media_piece,0,"Light & Media",(map_palette.item_pane.click.id==map_light_media_piece),FALSE);
	list_palette_add_item(&map_palette,map_sky_weather_piece,0,"Sky & Weather",(map_palette.item_pane.click.id==map_sky_weather_piece),FALSE);

		// spots

	list_palette_add_header_button(&map_palette,kPropertyAddSpot,"Spots",list_button_plus);
	list_palette_sort_mark_start(&map_palette);

	for (n=0;n!=map.nspot;n++) {
		if (map.spots[n].name[0]!=0x0) {
			list_palette_add_item(&map_palette,spot_piece,n,map.spots[n].name,((map_palette.item_pane.click.id==spot_piece)&&(n==map_palette.item_pane.click.idx)),FALSE);
		}
		else {
			list_palette_add_item(&map_palette,spot_piece,n,"[none]",((map_palette.item_pane.click.id==spot_piece)&&(n==map_palette.item_pane.click.idx)),FALSE);
		}
	}

	list_palette_sort(&map_palette);

		// lights

	list_palette_add_header_button(&map_palette,kPropertyAddLight,"Lights",list_button_plus);

	for (n=0;n!=map.nlight;n++) {
		list_palette_add_color(&map_palette,light_piece,n,&map.lights[n].setting.col,((map_palette.item_pane.click.id==light_piece)&&(n==map_palette.item_pane.click.idx)),FALSE);
	}

		// sounds

	list_palette_add_header_button(&map_palette,kPropertyAddSound,"Sounds",list_button_plus);
	list_palette_sort_mark_start(&map_palette);

	for (n=0;n!=map.nsound;n++) {
		list_palette_add_item(&map_palette,sound_piece,n,map.sounds[n].name,((map_palette.item_pane.click.id==sound_piece)&&(n==map_palette.item_pane.click.idx)),FALSE);
	}

	list_palette_sort(&map_palette);

		// particles

	list_palette_add_header_button(&map_palette,kPropertyAddParticle,"Particles",list_button_plus);
	list_palette_sort_mark_start(&map_palette);

	for (n=0;n!=map.nparticle;n++) {
		list_palette_add_item(&map_palette,particle_piece,n,map.particles[n].name,((map_palette.item_pane.click.id==particle_piece)&&(n==map_palette.item_pane.click.idx)),FALSE);
	}

	list_palette_sort(&map_palette);

		// sceneries

	list_palette_add_header_button(&map_palette,kPropertyAddScenery,"Scenery",list_button_plus);
	list_palette_sort_mark_start(&map_palette);

	for (n=0;n!=map.nscenery;n++) {
		list_palette_add_item(&map_palette,scenery_piece,n,map.sceneries[n].model_name,((map_palette.item_pane.click.id==scenery_piece)&&(n==map_palette.item_pane.click.idx)),FALSE);
	}

	list_palette_sort(&map_palette);

		// nodes

	list_palette_add_header_button(&map_palette,kPropertyAddNode,"Nodes",list_button_plus);
	list_palette_sort_mark_start(&map_palette);

	for (n=0;n!=map.nnode;n++) {
		if (map.nodes[n].name[0]!=0x0) list_palette_add_item(&map_palette,node_piece,n,map.nodes[n].name,((map_palette.item_pane.click.id==node_piece)&&(n==map_palette.item_pane.click.idx)),FALSE);
	}

	list_palette_sort(&map_palette);

		// groups

	list_palette_add_header_button(&map_palette,kPropertyAddGroup,"Groups",list_button_plus);
	list_palette_sort_mark_start(&map_palette);

	for (n=0;n!=map.group.ngroup;n++) {
		list_palette_add_item(&map_palette,group_piece,n,map.group.groups[n].name,((map_palette.item_pane.click.id==group_piece)&&(n==map_palette.item_pane.click.idx)),FALSE);
	}

	list_palette_sort(&map_palette);

		// movements

	list_palette_add_header_button(&map_palette,kPropertyAddMovement,"Movements",list_button_plus);
	list_palette_sort_mark_start(&map_palette);

	for (n=0;n!=map.movement.nmovement;n++) {
		list_palette_add_item(&map_palette,movement_piece,n,map.movement.movements[n].name,((map_palette.item_pane.click.id==movement_piece)&&(n==map_palette.item_pane.click.idx)),FALSE);
	}

	list_palette_sort(&map_palette);

		// cinemas

	list_palette_add_header_button(&map_palette,kPropertyAddCinema,"Cinemas",list_button_plus);
	list_palette_sort_mark_start(&map_palette);

	for (n=0;n!=map.cinema.ncinema;n++) {
		list_palette_add_item(&map_palette,cinema_piece,n,map.cinema.cinemas[n].name,((map_palette.item_pane.click.id==cinema_piece)&&(n==map_palette.item_pane.click.idx)),FALSE);
	}

	list_palette_sort(&map_palette);
}

/* =======================================================

      Property Palette Click Main
      
======================================================= */

void map_palette_click_main(bool double_click)
{
		// add buttons

	switch (map_palette.item_pane.click.id) {

		case kPropertyAddSpot:
			piece_create_spot();
			return;

		case kPropertyAddLight:
			piece_create_light();
			return;

		case kPropertyAddSound:
			piece_create_sound();
			return;

		case kPropertyAddParticle:
			piece_create_particle();
			return;

		case kPropertyAddScenery:
			piece_create_scenery();
			return;

		case kPropertyAddNode:
			piece_create_node();
			return;

		case kPropertyAddGroup:
			piece_create_group();
			return;

		case kPropertyAddMovement:
			piece_create_movement();
			return;

		case kPropertyAddCinema:
			piece_create_cinema();
			return;
	}

	if (map_palette.item_pane.click.idx==-1) return;

		// no selection states
	
	state.map.cur_no_sel_piece_type=map_palette.item_pane.click.id;

	state.map.cur_group_idx=-1;
	state.map.cur_movement_idx=-1;
	state.map.cur_movement_move_idx=-1;
	state.map.cur_cinema_idx=-1;
	state.map.cur_cinema_action_idx=-1;

		// turn off preferences

	state.map.in_preference=FALSE;

		// special check for non-selection
		// type double-clicks

	if (double_click) {
		switch (state.map.cur_no_sel_piece_type) {

			case map_setting_piece:
			case map_camera_piece:
			case map_light_media_piece:
			case map_sky_weather_piece:
				list_palette_set_level(&map_palette,1);
				main_wind_draw();
				return;

		}
	}

		// handle click

	select_clear();

	switch (state.map.cur_no_sel_piece_type) {

		case group_piece:
			state.map.cur_group_idx=map_palette.item_pane.click.idx;
			select_add_group(state.map.cur_group_idx);
			break;

		case movement_piece:
			state.map.cur_movement_idx=map_palette.item_pane.click.idx;
			state.map.cur_movement_move_idx=-1;
			select_add_movement(state.map.cur_movement_idx);
			break;

		case cinema_piece:
			state.map.cur_cinema_idx=map_palette.item_pane.click.idx;
			state.map.cur_cinema_action_idx=-1;
			select_add_cinema(state.map.cur_cinema_idx);
			break;

		default:
			select_add(map_palette.item_pane.click.id,map_palette.item_pane.click.idx,-1);
			break;

	}

	if (double_click) view_goto_select();

		// turn on any hidden items

	switch (state.map.cur_no_sel_piece_type) {

		case spot_piece:
		case scenery_piece:
			state.map.show_object=TRUE;
			break;

		case light_piece:
		case sound_piece:
		case particle_piece:
			state.map.show_lightsoundparticle=TRUE;
			break;

		case node_piece:
			state.map.show_node=TRUE;
			break;
	}
	
		// open the property window for double-click
		
	if (double_click) list_palette_set_level(&map_palette,1);

	main_wind_draw();
}

