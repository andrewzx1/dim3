/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Alt Property Palette Animate Pose Move Ring

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

#define kAnimationPoseMovePropertyRingName					0
#define kAnimationPoseMovePropertyRingBone					1
#define kAnimationPoseMovePropertyRingAngle					2
#define kAnimationPoseMovePropertyRingSlop					3

extern model_type				model;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		alt2_property_palette;

/* =======================================================

      Alt Property Palette Fill Animate Pose Move Ring
      
======================================================= */

void alt2_property_palette_fill_animate_pose_move_ring(int animate_idx,int pose_move_idx,int ring_idx)
{
	model_animate_type		*animate;
	model_pose_move_type	*pose_move;
	model_ring_type			*ring;

	if (ring_idx==-1) return;

	animate=&model.animates[animate_idx];
	pose_move=&animate->pose_moves[pose_move_idx];
	ring=&pose_move->ring.rings[ring_idx];

	list_palette_set_title(&alt2_property_palette,"Animation",animate->name,"Pose Move",model.poses[pose_move->pose_idx].name,"Ring",ring->name);

	list_palette_add_string(&alt2_property_palette,kAnimationPoseMovePropertyRingName,"Name",ring->name,FALSE);
	property_palette_add_string_bone(&alt2_property_palette,kAnimationPoseMovePropertyRingBone,"Bone",ring->bone_idx,FALSE);
	list_palette_add_checkbox(&alt2_property_palette,kAnimationPoseMovePropertyRingAngle,"Follow Model Angle",&ring->angle,FALSE);
	list_palette_add_point(&alt2_property_palette,kAnimationPoseMovePropertyRingSlop,"Position Slop",&ring->slop,FALSE);
}

/* =======================================================

      Alt Property Palette Click Animate Pose Move Ring
      
======================================================= */

void alt2_property_palette_click_animate_pose_move_ring(int animate_idx,int pose_move_idx,int ring_idx,int id,bool double_click)
{
	model_animate_type		*animate;
	model_pose_move_type	*pose_move;
	model_ring_type			*ring;

	if (ring_idx==-1) return;

	animate=&model.animates[animate_idx];
	pose_move=&animate->pose_moves[pose_move_idx];
	ring=&pose_move->ring.rings[ring_idx];

	if (!double_click) return;

	switch (id) {

		case kAnimationPoseMovePropertyRingName:
			property_palette_pick_ring(ring->name);
			break;

		case kAnimationPoseMovePropertyRingBone:
			property_palette_pick_bone(&ring->bone_idx,-1);
			break;

	}
}

