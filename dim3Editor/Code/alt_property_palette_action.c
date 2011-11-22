/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Alt Property Palette Cinema Action

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
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

#define kCinemaActionPropertyTimeStart				0
#define kCinemaActionPropertyTimeEnd				1
#define kCinemaActionPropertyAction					2
#define kCinemaActionPropertyActorType				3
#define kCinemaActionPropertyActorName				4
#define kCinemaActionPropertyAnimation				5
#define kCinemaActionPropertyNextAnimation			6
#define kCinemaActionPropertyNode					7
#define kCinemaActionPropertyText					8
#define kCinemaActionPropertyReverseMove			9

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		alt_property_palette;

extern char						action_actor_type_str[][32],action_action_type_str[][32];

/* =======================================================

      Alt Property Palette Fill Cinema Action
      
======================================================= */

void alt_property_palette_fill_cinema_action(int cinema_idx,int action_idx)
{
	bool					has_actor,has_animation,has_node,is_fade;
	map_cinema_type			*cinema;
	map_cinema_action_type	*action;

	if (action_idx==-1) return;

	cinema=&map.cinema.cinemas[cinema_idx];
	action=&cinema->actions[action_idx];

	list_palette_set_sub2_title(&alt_property_palette,"Cinema",cinema->name,action_actor_type_str[action->actor_type]);

		// setup some editing enables
	
	has_actor=((action->actor_type!=cinema_actor_camera) && (action->actor_type!=cinema_actor_player));
	has_animation=((action->actor_type==cinema_actor_player) || (action->actor_type==cinema_actor_object));
	has_node=((action->actor_type==cinema_actor_camera) || (action->actor_type==cinema_actor_player) || (action->actor_type==cinema_actor_object) || (action->actor_type==cinema_actor_particle) || (action->actor_type==cinema_actor_sound));
	is_fade=((action->action==cinema_action_fade_in) || (action->action==cinema_action_fade_out));

		// the properties

	list_palette_add_header(&alt_property_palette,0,"Cinema Action Timing");
	list_palette_add_int(&alt_property_palette,kCinemaActionPropertyTimeStart,"Time Start",&action->start_msec,FALSE);
	list_palette_add_int(&alt_property_palette,kCinemaActionPropertyTimeEnd,"Time End",&action->end_msec,FALSE);

	list_palette_add_header(&alt_property_palette,0,"Cinema Action Actor");
	list_palette_add_picker_list_int(&alt_property_palette,kCinemaActionPropertyAction,"Action",(char*)action_action_type_str,-1,name_str_len,0,FALSE,&action->action,FALSE);
	if (!is_fade) {
		list_palette_add_picker_list_int(&alt_property_palette,kCinemaActionPropertyActorType,"Type",(char*)action_actor_type_str,-1,name_str_len,0,FALSE,&action->actor_type,FALSE);
	}
	else {
		list_palette_add_string(&alt_property_palette,kCinemaActionPropertyActorName,"Type","n/a",TRUE);
	}
	if (has_actor) {
		list_palette_add_string(&alt_property_palette,kCinemaActionPropertyActorName,"Name",action->actor_name,FALSE);
	}
	else {
		list_palette_add_string(&alt_property_palette,kCinemaActionPropertyActorName,"Name","n/a",TRUE);
	}
	if (has_animation) {
		list_palette_add_string(&alt_property_palette,kCinemaActionPropertyAnimation,"Animation",action->animation_name,FALSE);
		list_palette_add_string(&alt_property_palette,kCinemaActionPropertyNextAnimation,"Next Animation",action->next_animation_name,FALSE);
	}
	else {
		list_palette_add_string(&alt_property_palette,kCinemaActionPropertyAnimation,"Animation","n/a",TRUE);
		list_palette_add_string(&alt_property_palette,kCinemaActionPropertyNextAnimation,"Next Animation","n/a",TRUE);
	}
	if ((has_node) && (!is_fade)) {
		list_palette_add_string(&alt_property_palette,kCinemaActionPropertyNode,"Node",action->node_name,FALSE);
	}
	else {
		list_palette_add_string(&alt_property_palette,kCinemaActionPropertyNode,"Node","n/a",TRUE);
	}

	list_palette_add_header(&alt_property_palette,0,"Cinema Action Options");
	if (action->actor_type==cinema_actor_movement) {
		list_palette_add_checkbox(&alt_property_palette,kCinemaActionPropertyReverseMove,"Reverse Move",&action->move_reverse,FALSE);
	}
	else {
		list_palette_add_string(&alt_property_palette,kCinemaActionPropertyReverseMove,"Reverse Move","n/a",TRUE);
	}
	if (action->actor_type==cinema_actor_hud_text) {
		list_palette_add_string(&alt_property_palette,kCinemaActionPropertyText,"Display Text",action->text_str,FALSE);
	}
	else {
		list_palette_add_string(&alt_property_palette,kCinemaActionPropertyText,"Display Text","n/a",TRUE);
	}
}

/* =======================================================

      Alt Property Palette Click Cinema Action
      
======================================================= */

void alt_property_palette_click_cinema_action(int cinema_idx,int action_idx,int id,bool double_click)
{
	map_cinema_action_type	*action;

	if (!double_click) return;
	if (action_idx==-1) return;

	action=&map.cinema.cinemas[cinema_idx].actions[action_idx];

	switch (id) {
	
		case kCinemaActionPropertyActorName:
			switch (action->actor_type) {

				case cinema_actor_object:
					property_palette_pick_spot(action->actor_name);
					break;

				case cinema_actor_movement:
					property_palette_pick_movement(action->actor_name);
					break;

				case cinema_actor_particle:
					property_palette_pick_particle(action->actor_name);
					break;

				case cinema_actor_sound:
					property_palette_pick_sound(action->actor_name,FALSE);
					break;

				case cinema_actor_hud_text:
					property_palette_pick_hud_text(action->actor_name);
					break;

				case cinema_actor_hud_bitmap:
					property_palette_pick_hud_bitmap(action->actor_name);
					break;

			}

			break;

		case kCinemaActionPropertyAnimation:
			dialog_property_string_run(list_string_value_string,(void*)action->animation_name,name_str_len,0,0);
			break;

		case kCinemaActionPropertyNextAnimation:
			dialog_property_string_run(list_string_value_string,(void*)action->next_animation_name,name_str_len,0,0);
			break;

		case kCinemaActionPropertyNode:
			property_palette_pick_node(action->node_name);
			break;

		case kCinemaActionPropertyText:
			dialog_property_string_run(list_string_value_string,(void*)action->text_str,256,0,0);
			break;

	}
}
