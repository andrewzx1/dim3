/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Movement

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
#include "dialog.h"

#define kMovementPropertyName				0
#define kMovementPropertyAutoOpenDistance	1
#define kMovementPropertyGroup				2
#define kMovementPropertyReverseGroup		3
#define kMovementPropertyAutoStart			4
#define kMovementPropertyAutoOpen			5
#define kMovementPropertyAutoOpenStand		6
#define kMovementPropertyAutoOpenNeverClose	7
#define kMovementPropertyLoop				8

#define kMovementPropertyActionAdd			10

#define kMovementPropertyMoveBase			100

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

/*


typedef struct		{
						int									msec,user_id,sound_buffer_idx;
						char								sound_name[name_str_len];
						float								sound_pitch;
						d3pnt								mov;
						d3ang								rot;
					} movement_move_type;

typedef struct		{
						int									nmove,group_idx,reverse_group_idx,
															auto_open_distance;
						char								name[name_str_len];
						bool								auto_start,auto_open,auto_open_stand,
															auto_open_never_close,
															loop,started,opened,reverse;
						movement_move_type					*moves;
					} movement_type;
*/

/* =======================================================

      Property Palette Fill Movement
      
======================================================= */

void property_palette_fill_movement(int movement_idx)
{
	movement_type		*movement;

	movement=&map.movement.movements[movement_idx];

	list_palette_add_header(&property_palette,0,"Movement Options");
	list_palette_add_string(&property_palette,kMovementPropertyName,"Name",movement->name,FALSE);
	list_palette_add_string_int(&property_palette,kMovementPropertyAutoOpenDistance,"Auto-Open Distance",movement->auto_open_distance,FALSE);

	list_palette_add_header(&property_palette,0,"Movement Groups");
	if (movement->group_idx==-1) {
		list_palette_add_string(&property_palette,kMovementPropertyGroup,"Group","",FALSE);
	}
	else {
		list_palette_add_string(&property_palette,kMovementPropertyGroup,"Group",map.group.groups[movement->group_idx].name,FALSE);
	}
	if (movement->reverse_group_idx==-1) {
		list_palette_add_string(&property_palette,kMovementPropertyGroup,"Reverse Group","",FALSE);
	}
	else {
		list_palette_add_string(&property_palette,kMovementPropertyGroup,"Reverse Group",map.group.groups[movement->reverse_group_idx].name,FALSE);
	}

	list_palette_add_header(&property_palette,0,"Movement Settings");
	list_palette_add_checkbox(&property_palette,kMovementPropertyAutoStart,"Auto-Start",movement->auto_start,FALSE);
	list_palette_add_checkbox(&property_palette,kMovementPropertyAutoOpen,"Auto-Open on Distance",movement->auto_open,FALSE);
	list_palette_add_checkbox(&property_palette,kMovementPropertyAutoOpenStand,"Auto-Open on Stand",movement->auto_open_stand,FALSE);
	list_palette_add_checkbox(&property_palette,kMovementPropertyAutoOpenNeverClose,"Never Close After Auto-Open",movement->auto_open_never_close,FALSE);
	list_palette_add_checkbox(&property_palette,kMovementPropertyLoop,"Looping",movement->loop,FALSE);

	list_palette_add_header_button(&property_palette,kMovementPropertyActionAdd,"Movement Moves",list_button_plus);
}

/* =======================================================

      Property Palette Click Movement
      
======================================================= */

void property_palette_click_movement(int movement_idx,int id)
{
	/*
	map_sound_type		*sound;

	sound=&map.sounds[sound_idx];

	switch (id) {

		case kSoundPropertyOn:
			sound->on=!sound->on;
			break;

		case kSoundPropertyName:
			property_palette_pick_sound(sound->name,FALSE);
			break;

		case kSoundPropertyPitch:
			dialog_property_string_run(list_string_value_positive_float,(void*)&sound->pitch,0,0,0);
			break;

	}
*/
	main_wind_draw();
}

