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
#include "dialog.h"

#define kPoseBoneMovePropertyRot					0
#define kPoseBoneMovePropertyMove					1
#define kPoseBoneMovePropertyAcceleration			2
#define kPoseBoneMovePropertySkipBlended			3

#define kPoseBoneMovePropertyConstraintBone			4
#define kPoseBoneMovePropertyConstraintOffset		5

/*
#define kCinemaActionActorType				FOUR_CHAR_CODE('actt')
#define kCinemaActionActorName				FOUR_CHAR_CODE('actr')
#define kCinemaActionAction					FOUR_CHAR_CODE('actn')
#define kCinemaActionNode					FOUR_CHAR_CODE('node')
#define kCinemaActionTimeStart				FOUR_CHAR_CODE('tsrt')
#define kCinemaActionTimeEnd				FOUR_CHAR_CODE('tend')
#define kCinemaActionAnimation				FOUR_CHAR_CODE('anim')
#define kCinemaActionNextAnimation			FOUR_CHAR_CODE('nanm')
#define kCinemaActionText					FOUR_CHAR_CODE('text')
#define kCinemaActionReverseMove			FOUR_CHAR_CODE('rmov')
*/

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		alt_property_palette;

/*
typedef struct		{
						int									action,actor_type,
															start_msec,end_msec;
						bool								move_reverse;
						char								actor_name[name_str_len],
															animation_name[name_str_len],
															next_animation_name[name_str_len],
															node_name[name_str_len],
															text_str[256];
					} map_cinema_action_type;
*/

/* =======================================================

      Alt Property Palette Fill Cinema Action
      
======================================================= */

void alt_property_palette_fill_cinema_action(int cinema_idx,int action_idx)
{
	/*
	model_bone_move_type	*bone_move;

	if (bone_move_idx==-1) return;

	bone_move=&model.poses[pose_idx].bone_moves[bone_move_idx];

	list_palette_add_header(&alt_property_palette,0,"Pose Bone Move Position");
	list_palette_add_vector(&alt_property_palette,kPoseBoneMovePropertyRot,"Rot",&bone_move->rot,FALSE);
	list_palette_add_vector(&alt_property_palette,kPoseBoneMovePropertyMove,"Move",&bone_move->mov,FALSE);
	
	list_palette_add_header(&alt_property_palette,0,"Pose Bone Move Options");
	list_palette_add_string_float(&alt_property_palette,kPoseBoneMovePropertyAcceleration,"Acceleration",bone_move->acceleration,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kPoseBoneMovePropertySkipBlended,"Skip Blending",bone_move->skip_blended,FALSE);

	list_palette_add_header(&alt_property_palette,0,"Pose Bone Move Constraint");
	property_palette_add_string_bone(&alt_property_palette,kPoseBoneMovePropertyConstraintBone,"Constraint Bone",bone_move->constraint.bone_idx,FALSE);
	list_palette_add_point(&alt_property_palette,kPoseBoneMovePropertyConstraintOffset,"Constaint Offset",&bone_move->constraint.offset,FALSE);
	*/
}

/* =======================================================

      Alt Property Palette Click Cinema Action
      
======================================================= */

void alt_property_palette_click_cinema_action(int cinema_idx,int action_idx,int id)
{
	/*
	model_bone_move_type	*bone_move;

	if (bone_move_idx==-1) return;

	bone_move=&model.poses[pose_idx].bone_moves[bone_move_idx];

	switch (id) {

		case kPoseBoneMovePropertyRot:
			dialog_property_string_run(list_string_value_vector,(void*)&bone_move->rot,0,0,0);
			break;

		case kPoseBoneMovePropertyMove:
			dialog_property_string_run(list_string_value_vector,(void*)&bone_move->mov,0,0,0);
			break;

		case kPoseBoneMovePropertyAcceleration:
			dialog_property_string_run(list_string_value_positive_float,(void*)&bone_move->acceleration,0,0,0);
			break;

		case kPoseBoneMovePropertySkipBlended:
			bone_move->skip_blended=!bone_move->skip_blended;
			break;

		case kPoseBoneMovePropertyConstraintBone:
			property_palette_pick_bone(&bone_move->constraint.bone_idx);
			break;

		case kPoseBoneMovePropertyConstraintOffset:
			dialog_property_string_run(list_string_value_point,(void*)&bone_move->constraint.offset,0,0,0);
			break;

	}

		// redraw

	main_wind_draw();
	*/
}
