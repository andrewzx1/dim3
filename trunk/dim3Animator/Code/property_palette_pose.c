/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette Pose

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
#include "dialog.h"

#define kPosePropertyName							0

#define kPosePropertyBoneMoveRot					1000
#define kPosePropertyBoneMoveMove					2000
#define kPosePropertyBoneMoveAcceleration			3000
#define kPosePropertyBoneMoveSkipBlended			4000

#define kPosePropertyBoneMoveConstraintBone			5000
#define kPosePropertyBoneMoveConstraintOffset		6000

extern model_type				model;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Pose
      
======================================================= */

void property_palette_fill_pose(int pose_idx)
{
	int						n;
	model_pose_type			*pose;

	pose=&model.poses[pose_idx];

	list_palette_add_header(&property_palette,0,"Pose Options");
	list_palette_add_string(&property_palette,kPosePropertyName,"Name",pose->name,FALSE);

	for (n=0;n!=model.nbone;n++) {
		list_palette_add_header(&property_palette,0,model.bones[n].name);
		list_palette_add_vector(&property_palette,(kPosePropertyBoneMoveRot+n),"Rot",&pose->bone_moves[n].rot,FALSE);
		list_palette_add_vector(&property_palette,(kPosePropertyBoneMoveMove+n),"Move",&pose->bone_moves[n].mov,FALSE);
		list_palette_add_string_float(&property_palette,(kPosePropertyBoneMoveAcceleration+n),"Acceleration",pose->bone_moves[n].acceleration,FALSE);
		list_palette_add_checkbox(&property_palette,(kPosePropertyBoneMoveSkipBlended+n),"Skip Blending",pose->bone_moves[n].skip_blended,FALSE);
		if (pose->bone_moves[n].constraint.bone_idx==-1) {
			list_palette_add_string(&property_palette,(kPosePropertyBoneMoveConstraintBone+n),"Constraint Bone","",FALSE);
		}
		else {
			list_palette_add_string(&property_palette,(kPosePropertyBoneMoveConstraintBone+n),"Constraint Bone",model.bones[pose->bone_moves[n].constraint.bone_idx].name,FALSE);
		}
		list_palette_add_point(&property_palette,(kPosePropertyBoneMoveConstraintOffset+n),"Constaint Offset",&pose->bone_moves[n].constraint.offset,FALSE);
	}
}

/* =======================================================

      Property Palette Click Pose
      
======================================================= */

void property_palette_click_pose(int pose_idx,int id)
{
	model_pose_type			*pose;

	pose=&model.poses[pose_idx];

		// bone moves

	if ((id>=kPosePropertyBoneMoveRot) && (id<(kPosePropertyBoneMoveRot+max_model_bone))) {



	}

// supergumba -- do all the bone moves!

	/*

			case kBonePropertyParent:
			property_palette_pick_bone(&bone->parent_idx);
			break;
*/

	switch (id) {

		case kPosePropertyName:
			dialog_property_string_run(list_string_value_string,(void*)pose->name,name_str_len,0,0);
			break;

	}

		// redraw

	main_wind_draw();
}

