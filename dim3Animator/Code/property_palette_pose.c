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

#define kPosePropertyName							0

#define kPosePropertyBoneMove						100

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
	char					str_tag[8],str[256];
	model_pose_type			*pose;

	pose=&model.poses[pose_idx];

	list_palette_add_header(&property_palette,0,"Pose Options");
	list_palette_add_string(&property_palette,kPosePropertyName,"Name",pose->name,FALSE);

	list_palette_add_header_count(&property_palette,0,"Pose Bone Moves",model.nbone);
	list_palette_sort_mark_start(&property_palette);

	for (n=0;n!=model.nbone;n++) {
		memmove(str_tag,&model.bones[n].tag,4);
		str_tag[4]=0x0;
		sprintf(str,"%s (%s)",model.bones[n].name,str_tag);
		list_palette_add_string_selectable(&property_palette,(kPosePropertyBoneMove+n),str,NULL,((state.cur_pose_idx==pose_idx)&&(state.cur_pose_bone_move_idx==n)),FALSE);
	}

	list_palette_sort(&property_palette);
}

/* =======================================================

      Property Palette Click Pose
      
======================================================= */

void property_palette_click_pose(int pose_idx,int id)
{
	model_pose_type			*pose;

	pose=&model.poses[pose_idx];

		// bone moves

	if (id>=kPosePropertyBoneMove) {
		state.cur_pose_bone_move_idx=id-kPosePropertyBoneMove;
		list_palette_set_level(2);
		main_wind_draw();
		return;
	}

	state.cur_pose_bone_move_idx=-1;

		// regular values

	switch (id) {

		case kPosePropertyName:
			dialog_property_string_run(list_string_value_string,(void*)pose->name,name_str_len,0,0);
			break;

	}

		// redraw

	main_wind_draw();
}

