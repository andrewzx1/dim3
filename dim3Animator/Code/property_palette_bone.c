/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette Bone

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

#define kBonePropertyName						0
#define kBonePropertyPosition					1
#define kBonePropertyParent						2

#define kBonePropertyNaturalRot					10
#define kBonePropertyNaturalOffset				11

#define kPoseBoneMovePropertyRot				20
#define kPoseBoneMovePropertyMove				21
#define kPoseBoneMovePropertyAcceleration		22
#define kPoseBoneMovePropertySkipBlended		23

#define kPoseBoneMovePropertyConstraintBone		30
#define kPoseBoneMovePropertyConstraintOffset	31

extern model_type				model;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Bone
      
======================================================= */

void property_palette_fill_bone(int bone_idx,int pose_idx)
{
	model_bone_type			*bone;
	model_bone_move_type	*bone_move;

	bone=&model.bones[bone_idx];

	list_palette_set_sub_title(&property_palette,"Bone",bone->name);

		// bone

	list_palette_add_header(&property_palette,0,"Bone Options");
	list_palette_add_string(&property_palette,kBonePropertyName,"Name",bone->name,FALSE);
	list_palette_add_point(&property_palette,kBonePropertyPosition,"Position",&bone->pnt,FALSE);
	property_palette_add_string_bone(&property_palette,kBonePropertyParent,"Parent Bone",bone->parent_idx,FALSE);

		// handles
	
	list_palette_add_header(&property_palette,0,"Natural Settings");
	list_palette_add_angle(&property_palette,kBonePropertyNaturalRot,"Natural Rotation",&bone->natural_rot,FALSE);
	list_palette_add_point(&property_palette,kBonePropertyNaturalOffset,"Natural Offset",&bone->natural_offset,FALSE);

		// current pose

	if (pose_idx!=-1) {
		bone_move=&model.poses[pose_idx].bone_moves[bone_idx];

		list_palette_add_header(&property_palette,0,"Current Pose Move Position");
		list_palette_add_vector(&property_palette,kPoseBoneMovePropertyRot,"Rot",&bone_move->rot,FALSE);
		list_palette_add_vector(&property_palette,kPoseBoneMovePropertyMove,"Move",&bone_move->mov,FALSE);
	
		list_palette_add_header(&property_palette,0,"Current Pose Move Options");
		list_palette_add_float(&property_palette,kPoseBoneMovePropertyAcceleration,"Acceleration",&bone_move->acceleration,FALSE);
		list_palette_add_checkbox(&property_palette,kPoseBoneMovePropertySkipBlended,"Skip Blending",&bone_move->skip_blended,FALSE);

		list_palette_add_header(&property_palette,0,"Current Pose Move Constraint");
		property_palette_add_string_bone(&property_palette,kPoseBoneMovePropertyConstraintBone,"Constraint Bone",bone_move->constraint.bone_idx,FALSE);
		list_palette_add_point(&property_palette,kPoseBoneMovePropertyConstraintOffset,"Constaint Offset",&bone_move->constraint.offset,FALSE);
	}
}

/* =======================================================

      Property Palette Click Bone
      
======================================================= */

void property_palette_click_bone(int bone_idx,int pose_idx,int id,bool double_click)
{
	model_bone_type			*bone;
	model_bone_move_type	*bone_move;
	
	if (!double_click) return;

	bone=&model.bones[bone_idx];

	switch (id) {

			// bone

		case kBonePropertyName:
			dialog_property_string_run(list_string_value_string,(void*)bone->name,name_str_len,0,0);
			break;

		case kBonePropertyParent:
			property_palette_pick_bone(&bone->parent_idx,bone_idx);
			break;

			// current pose

		case kPoseBoneMovePropertyConstraintBone:
			bone_move=&model.poses[pose_idx].bone_moves[bone_idx];
			property_palette_pick_bone(&bone_move->constraint.bone_idx,-1);
			break;

	}
}

