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
#define kBonePropertyTag						2
#define kBonePropertyParent						3

extern model_type				model;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Bone
      
======================================================= */

void property_palette_fill_bone(int bone_idx)
{
	model_bone_type			*bone;

	bone=&model.bones[bone_idx];

	list_palette_add_header(&property_palette,0,"Bone Options");
	list_palette_add_string(&property_palette,kBonePropertyName,"Name",bone->name,FALSE);
	list_palette_add_point(&property_palette,kBonePropertyPosition,"Position",&bone->pnt,FALSE);
	list_palette_add_string_tag(&property_palette,kBonePropertyTag,"Tag",bone->tag,FALSE);
	property_palette_add_string_bone(&property_palette,kBonePropertyParent,"Parent Bone",bone->parent_idx,FALSE);
}

/* =======================================================

      Property Palette Click Bone
      
======================================================= */

void property_palette_click_bone(int bone_idx,int id)
{
	int						old_parent_bone_idx;
	model_bone_type			*bone;
	
	bone=&model.bones[bone_idx];

	switch (id) {

		case kBonePropertyName:
			dialog_property_string_run(list_string_value_string,(void*)bone->name,name_str_len,0,0);
			break;

		case kBonePropertyPosition:
			dialog_property_chord_run(list_chord_value_point,(void*)&bone->pnt);
			model_calculate_parents(&model);
			break;

		case kBonePropertyTag:
			dialog_property_string_run(list_string_value_tag,(void*)&bone->tag,0,0,0);
			break;

		case kBonePropertyParent:
			old_parent_bone_idx=bone->parent_idx;
			property_palette_pick_bone(&bone->parent_idx);
			if (bone->parent_idx==bone_idx) {
				os_dialog_alert("Parent Bone","Can not set itself as the parent bone.");
				bone->parent_idx=old_parent_bone_idx;
			}
			if (model_check_bone_circular(&model,bone)) {
				os_dialog_alert("Parent Bone","This parent bone will make a circular bone reference.");
				bone->parent_idx=old_parent_bone_idx;
			}			
			break;

	}

		// redraw

	main_wind_draw();
}

