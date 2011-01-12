/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Alt Property Palette Animate Pose Move

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

#define kAnimatePoseMovePropertyPose				0
#define kAnimatePoseMovePropertyMilliseconds		1
#define kAnimatePoseMovePropertyAcceleration		2

#define kAnimatePoseMovePropertyMove				3
#define kAnimatePoseMovePropertySway				4

#define kAnimatePoseMovePropertyLoopStart			5
#define kAnimatePoseMovePropertyLoopEnd				6

#define kAnimationPoseMovePropertySoundBone			7
#define kAnimationPoseMovePropertySoundName			8
#define kAnimationPoseMovePropertySoundPitch		9
#define kAnimationPoseMovePropertySoundGlobal		10

extern model_type				model;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		alt_property_palette;

/* =======================================================

      Alt Property Palette Fill Aniate Pose Move
      
======================================================= */

void alt_property_palette_fill_animate_pose_move(int animate_idx,int pose_move_idx)
{
	model_animate_type		*animate;
	model_pose_move_type	*pose_move;

	if (pose_move_idx==-1) return;

	animate=&model.animates[animate_idx];
	pose_move=&animate->pose_moves[pose_move_idx];

	list_palette_add_header(&alt_property_palette,0,"Animate Pose Options");
	list_palette_add_string(&alt_property_palette,kAnimatePoseMovePropertyPose,"Pose",model.poses[pose_move->pose_idx].name,FALSE);
	list_palette_add_string_int(&alt_property_palette,kAnimatePoseMovePropertyMilliseconds,"Milliseconds",pose_move->msec,FALSE);
	list_palette_add_string_float(&alt_property_palette,kAnimatePoseMovePropertyAcceleration,"Acceleration",pose_move->acceleration,FALSE);

	list_palette_add_header(&alt_property_palette,0,"Animate Pose Position");
	list_palette_add_vector(&alt_property_palette,kAnimatePoseMovePropertyMove,"Move",&pose_move->mov,FALSE);
	list_palette_add_vector(&alt_property_palette,kAnimatePoseMovePropertySway,"Sway",&pose_move->sway,FALSE);

	list_palette_add_header(&alt_property_palette,0,"Animate Pose Loop");
	list_palette_add_string(&alt_property_palette,kAnimatePoseMovePropertyLoopStart,"Set Pose As Loop Start",NULL,FALSE);
	list_palette_add_string(&alt_property_palette,kAnimatePoseMovePropertyLoopEnd,"Set Pose As Loop End",NULL,FALSE);

	list_palette_add_header(&alt_property_palette,0,"Animate Pose Sound");
	property_palette_add_string_bone(&alt_property_palette,kAnimationPoseMovePropertySoundBone,"Sound Bone",pose_move->sound.bone_idx,FALSE);
	list_palette_add_string(&alt_property_palette,kAnimationPoseMovePropertySoundName,"Sound",pose_move->sound.name,FALSE);
	list_palette_add_string_float(&alt_property_palette,kAnimationPoseMovePropertySoundPitch,"Pitch",pose_move->sound.pitch,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kAnimationPoseMovePropertySoundGlobal,"Play Globally",pose_move->sound.no_position,FALSE);
}

/* =======================================================

      Alt Property Palette Click Animate Pose Move
      
======================================================= */

void alt_property_palette_click_animate_pose_move(int animate_idx,int pose_move_idx,int id)
{
	model_animate_type		*animate;
	model_pose_move_type	*pose_move;

	if (pose_move_idx==-1) return;

	animate=&model.animates[animate_idx];
	pose_move=&animate->pose_moves[pose_move_idx];

	switch (id) {

		case kAnimatePoseMovePropertyPose:
			property_palette_pick_pose(&pose_move->pose_idx);
			break;

		case kAnimatePoseMovePropertyMilliseconds:
			dialog_property_string_run(list_string_value_positive_int,(void*)&pose_move->msec,0,0,0);
			break;

		case kAnimatePoseMovePropertyAcceleration:
			dialog_property_string_run(list_string_value_positive_float,(void*)&pose_move->acceleration,0,0,0);
			break;

		case kAnimatePoseMovePropertyMove:
			dialog_property_string_run(list_string_value_vector,(void*)&pose_move->mov,0,0,0);
			break;

		case kAnimatePoseMovePropertySway:
			dialog_property_string_run(list_string_value_vector,(void*)&pose_move->sway,0,0,0);
			break;

		case kAnimatePoseMovePropertyLoopStart:
			model_animate_set_loop_start(&model,animate_idx,pose_move_idx);
			break;

		case kAnimatePoseMovePropertyLoopEnd:
			model_animate_set_loop_end(&model,animate_idx,pose_move_idx);
			break;

		case kAnimationPoseMovePropertySoundBone:
			property_palette_pick_bone(&pose_move->sound.bone_idx);
			break;

		case kAnimationPoseMovePropertySoundName:
			property_palette_pick_sound(pose_move->sound.name,TRUE);
			break;

		case kAnimationPoseMovePropertySoundPitch:
			dialog_property_string_run(list_string_value_positive_float,(void*)&pose_move->sound.pitch,0,0,0);
			break;

		case kAnimationPoseMovePropertySoundGlobal:
			pose_move->sound.no_position=!pose_move->sound.no_position;
			break;

	}

		// redraw

	main_wind_draw();
}

