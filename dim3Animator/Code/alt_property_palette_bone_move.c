/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Alt Property Palette Pose Bone Move

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
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kPoseBoneMovePropertyRot					0
#define kPoseBoneMovePropertyMove					1
#define kPoseBoneMovePropertyAcceleration			2
#define kPoseBoneMovePropertySkipBlended			3

#define kPoseBoneMovePropertyConstraintBone			4
#define kPoseBoneMovePropertyConstraintOffset		5

extern model_type				model;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		alt_property_palette;

/* =======================================================

      Alt Property Palette Fill Pose Bone Move
      
======================================================= */

void alt_property_palette_fill_pose_bone_move(int pose_idx,int bone_move_idx)
{
	model_bone_move_type	*bone_move;

	if (bone_move_idx==-1) return;

	bone_move=&model.poses[pose_idx].bone_moves[bone_move_idx];

	list_palette_set_sub2_title(&alt_property_palette,"Bone Move",model.poses[pose_idx].name,model.bones[bone_move_idx].name);

	list_palette_add_header(&alt_property_palette,0,"Pose Bone Move Position");
	list_palette_add_vector(&alt_property_palette,kPoseBoneMovePropertyRot,"Rot",&bone_move->rot,FALSE);
	list_palette_add_vector(&alt_property_palette,kPoseBoneMovePropertyMove,"Move",&bone_move->mov,FALSE);
	
	list_palette_add_header(&alt_property_palette,0,"Pose Bone Move Options");
	list_palette_add_float(&alt_property_palette,kPoseBoneMovePropertyAcceleration,"Acceleration",&bone_move->acceleration,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kPoseBoneMovePropertySkipBlended,"Skip Blending",&bone_move->skip_blended,FALSE);

	list_palette_add_header(&alt_property_palette,0,"Pose Bone Move Constraint");
	property_palette_add_string_bone(&alt_property_palette,kPoseBoneMovePropertyConstraintBone,"Constraint Bone",bone_move->constraint.bone_idx,FALSE);
	list_palette_add_point(&alt_property_palette,kPoseBoneMovePropertyConstraintOffset,"Constaint Offset",&bone_move->constraint.offset,FALSE);
}

/* =======================================================

      Alt Property Palette Click Pose Bone Move
      
======================================================= */

void alt_property_palette_click_pose_bone_move(int pose_idx,int bone_move_idx,int id,bool double_click)
{
	model_bone_move_type	*bone_move;

	if (bone_move_idx==-1) return;
	bone_move=&model.poses[pose_idx].bone_moves[bone_move_idx];

	if (!double_click) return;

	switch (id) {

		case kPoseBoneMovePropertyRot:
			dialog_property_chord_run(list_chord_value_vector,(void*)&bone_move->rot);
			break;

		case kPoseBoneMovePropertyMove:
			dialog_property_chord_run(list_chord_value_vector,(void*)&bone_move->mov);
			break;

		case kPoseBoneMovePropertyConstraintBone:
			property_palette_pick_bone(&bone_move->constraint.bone_idx,-1);
			break;

		case kPoseBoneMovePropertyConstraintOffset:
			dialog_property_chord_run(list_chord_value_point,(void*)&bone_move->constraint.offset);
			break;

	}
}

