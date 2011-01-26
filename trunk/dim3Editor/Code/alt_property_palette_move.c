/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Alt Property Palette Movement Move

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

#define kMovePropertyUserID							0
#define kMovePropertyMillisecond					1
#define kMovePropertyMove							2
#define kMovePropertyRot							3
#define kMovePropertySoundName						4
#define kMovePropertySoundPitch						5

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		alt_property_palette;

/* =======================================================

      Alt Property Palette Fill Movement Move
      
======================================================= */

void alt_property_palette_fill_movement_move(int movement_idx,int move_idx)
{
	movement_move_type	*move;

	if (move_idx==-1) return;

	move=&map.movement.movements[movement_idx].moves[move_idx];

	list_palette_add_header(&alt_property_palette,0,"Movement Move Options");
	list_palette_add_string_int(&alt_property_palette,kMovePropertyUserID,"Event User Id",move->user_id,FALSE);
	list_palette_add_string_int(&alt_property_palette,kMovePropertyMillisecond,"Millisecond Time",move->msec,FALSE);

	list_palette_add_header(&alt_property_palette,0,"Movement Move Position");
	list_palette_add_point(&alt_property_palette,kMovePropertyMove,"Move",&move->mov,FALSE);
	list_palette_add_angle(&alt_property_palette,kMovePropertyRot,"Rot",&move->rot,FALSE);
	
	list_palette_add_header(&alt_property_palette,0,"Movement Move Sound");
	list_palette_add_string(&alt_property_palette,kMovePropertySoundName,"Sound",move->sound_name,FALSE);
	list_palette_add_string_float(&alt_property_palette,kMovePropertySoundPitch,"Pitch",move->sound_pitch,FALSE);
}

/* =======================================================

      Alt Property Palette Click Movement Move
      
======================================================= */

void alt_property_palette_click_movement_move(int movement_idx,int move_idx,int id)
{
	movement_move_type	*move;

	if (move_idx==-1) return;

	move=&map.movement.movements[movement_idx].moves[move_idx];

	switch (id) {

		case kMovePropertyUserID:
			dialog_property_string_run(list_string_value_positive_int,(void*)&move->user_id,0,0,0);
			break;

		case kMovePropertyMillisecond:
			dialog_property_string_run(list_string_value_positive_int,(void*)&move->msec,0,0,0);
			break;

		case kMovePropertyMove:
			dialog_property_string_run(list_string_value_point,(void*)&move->mov,0,0,0);
			break;

		case kMovePropertyRot:
			dialog_property_string_run(list_string_value_angle,(void*)&move->rot,0,0,0);
			break;
	
		case kMovePropertySoundName:
			property_palette_pick_sound(move->sound_name,FALSE);
			break;

		case kMovePropertySoundPitch:
			dialog_property_string_run(list_string_value_positive_float,(void*)&move->sound_pitch,0,0,0);
			break;

	}

		// redraw

	main_wind_draw();
}
