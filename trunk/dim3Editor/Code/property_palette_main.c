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

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Main
      
======================================================= */

void property_palette_fill_main(void)
{
	int			n;

		// title
		
	list_palette_set_title(&property_palette,"Map Items",NULL,NULL,NULL,NULL,NULL);

		// map

	list_palette_add_header(&property_palette,spot_piece,"Map");
	list_palette_add_item(&property_palette,map_setting_piece,0,"Settings",(property_palette.item_type==map_setting_piece),FALSE);
	list_palette_add_item(&property_palette,map_camera_piece,0,"Camera",(property_palette.item_type==map_camera_piece),FALSE);
	list_palette_add_item(&property_palette,map_light_media_piece,0,"Light & Media",(property_palette.item_type==map_light_media_piece),FALSE);
	list_palette_add_item(&property_palette,map_sky_weather_piece,0,"Sky & Weather",(property_palette.item_type==map_sky_weather_piece),FALSE);

		// spots

	list_palette_add_header_button(&property_palette,kPropertyAddSpot,"Spots",list_button_plus);
	list_palette_sort_mark_start(&property_palette);

	for (n=0;n!=map.nspot;n++) {
		if (map.spots[n].name[0]!=0x0) {
			list_palette_add_item(&property_palette,spot_piece,n,map.spots[n].name,((property_palette.item_type==spot_piece)&&(n==property_palette.item_idx)),FALSE);
		}
		else {
			list_palette_add_item(&property_palette,spot_piece,n,"[none]",((property_palette.item_type==spot_piece)&&(n==property_palette.item_idx)),FALSE);
		}
	}

	list_palette_sort(&property_palette);

		// lights

	list_palette_add_header_button(&property_palette,kPropertyAddLight,"Lights",list_button_plus);

	for (n=0;n!=map.nlight;n++) {
		list_palette_add_color(&property_palette,light_piece,n,&map.lights[n].setting.col,((property_palette.item_type==light_piece)&&(n==property_palette.item_idx)),FALSE);
	}

		// sounds

	list_palette_add_header_button(&property_palette,kPropertyAddSound,"Sounds",list_button_plus);
	list_palette_sort_mark_start(&property_palette);

	for (n=0;n!=map.nsound;n++) {
		list_palette_add_item(&property_palette,sound_piece,n,map.sounds[n].name,((property_palette.item_type==sound_piece)&&(n==property_palette.item_idx)),FALSE);
	}

	list_palette_sort(&property_palette);

		// particles

	list_palette_add_header_button(&property_palette,kPropertyAddParticle,"Particles",list_button_plus);
	list_palette_sort_mark_start(&property_palette);

	for (n=0;n!=map.nparticle;n++) {
		list_palette_add_item(&property_palette,particle_piece,n,map.particles[n].name,((property_palette.item_type==particle_piece)&&(n==property_palette.item_idx)),FALSE);
	}

	list_palette_sort(&property_palette);

		// sceneries

	list_palette_add_header_button(&property_palette,kPropertyAddScenery,"Scenery",list_button_plus);
	list_palette_sort_mark_start(&property_palette);

	for (n=0;n!=map.nscenery;n++) {
		list_palette_add_item(&property_palette,scenery_piece,n,map.sceneries[n].model_name,((property_palette.item_type==scenery_piece)&&(n==property_palette.item_idx)),FALSE);
	}

	list_palette_sort(&property_palette);

		// nodes

	list_palette_add_header_button(&property_palette,kPropertyAddNode,"Nodes",list_button_plus);
	list_palette_sort_mark_start(&property_palette);

	for (n=0;n!=map.nnode;n++) {
		if (map.nodes[n].name[0]!=0x0) list_palette_add_item(&property_palette,node_piece,n,map.nodes[n].name,((property_palette.item_type==node_piece)&&(n==property_palette.item_idx)),FALSE);
	}

	list_palette_sort(&property_palette);

		// groups

	list_palette_add_header_button(&property_palette,kPropertyAddGroup,"Groups",list_button_plus);
	list_palette_sort_mark_start(&property_palette);

	for (n=0;n!=map.group.ngroup;n++) {
		list_palette_add_item(&property_palette,group_piece,n,map.group.groups[n].name,((property_palette.item_type==group_piece)&&(n==property_palette.item_idx)),FALSE);
	}

	list_palette_sort(&property_palette);

		// movements

	list_palette_add_header_button(&property_palette,kPropertyAddMovement,"Movements",list_button_plus);
	list_palette_sort_mark_start(&property_palette);

	for (n=0;n!=map.movement.nmovement;n++) {
		list_palette_add_item(&property_palette,movement_piece,n,map.movement.movements[n].name,((property_palette.item_type==movement_piece)&&(n==property_palette.item_idx)),FALSE);
	}

	list_palette_sort(&property_palette);

		// cinemas

	list_palette_add_header_button(&property_palette,kPropertyAddCinema,"Cinemas",list_button_plus);
	list_palette_sort_mark_start(&property_palette);

	for (n=0;n!=map.cinema.ncinema;n++) {
		list_palette_add_item(&property_palette,cinema_piece,n,map.cinema.cinemas[n].name,((property_palette.item_type==cinema_piece)&&(n==property_palette.item_idx)),FALSE);
	}

	list_palette_sort(&property_palette);
}

/* =======================================================

      Property Palette Click Main
      
======================================================= */

void property_palette_click_main(int id,bool double_click)
{
		// add buttons

	switch (property_palette.item_id) {

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

	if (property_palette.item_idx==-1) return;

		// alt window items

	state.cur_cinema_idx=-1;
	state.cur_cinema_action_idx=-1;
	state.cur_movement_idx=-1;
	state.cur_movement_move_idx=-1;

		// turn off preferences

	state.in_preference=FALSE;

		// handle click

	select_clear();

	switch (property_palette.item_type) {

		case map_setting_piece:
		case map_camera_piece:
		case map_light_media_piece:
		case map_sky_weather_piece:
			select_clear();
			break;

		case group_piece:
			select_add_group(property_palette.item_idx);
			break;

		case movement_piece:
			state.cur_movement_idx=property_palette.item_idx;
			state.cur_movement_move_idx=-1;
			select_add_movement(property_palette.item_idx);
			break;

		case cinema_piece:
			state.cur_cinema_idx=property_palette.item_idx;
			state.cur_cinema_action_idx=-1;
			select_add_cinema(property_palette.item_idx);
			break;

		default:
			select_add(property_palette.item_type,property_palette.item_idx,-1);
			break;

	}

	if (double_click) view_goto_select();

		// turn on any hidden items

	switch (property_palette.item_type) {

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
		list_palette_set_level(&property_palette,1);
	}

	main_wind_draw();
}

