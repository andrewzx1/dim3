/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette Hit Box

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kHitBoxPropertyName				0
#define kHitBoxPropertyBone				1
#define kHitBoxPropertyOffset			2
#define kHitBoxPropertySize				3

extern model_type				model;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Hit Box
      
======================================================= */

void property_palette_fill_hit_box(int hit_box_idx)
{
	model_hit_box_type			*hit_box;
	
	hit_box=&model.hit_boxes[hit_box_idx];

	list_palette_set_title(&property_palette,"Hit Box",hit_box->name,NULL,NULL,NULL,NULL);

	list_palette_add_header(&property_palette,0,"Hit Box Options");
	list_palette_add_string(&property_palette,kHitBoxPropertyName,"Name",hit_box->name,FALSE);
	property_palette_add_string_bone(&property_palette,kHitBoxPropertyBone,"Bone",hit_box->bone_idx,FALSE);
	list_palette_add_point(&property_palette,kHitBoxPropertyOffset,"Offset",&hit_box->box.offset,FALSE);
	list_palette_add_point(&property_palette,kHitBoxPropertySize,"Size",&hit_box->box.size,FALSE);
}

/* =======================================================

      Property Palette Click Hit Box
      
======================================================= */

void property_palette_click_hit_box(int hit_box_idx,int id,bool double_click)
{
	model_hit_box_type			*hit_box;
	
	if (!double_click) return;

	hit_box=&model.hit_boxes[hit_box_idx];

	switch (id) {

		case kHitBoxPropertyName:
			dialog_property_string_run(list_string_value_string,(void*)hit_box->name,name_str_len,0,0);
			break;

		case kHitBoxPropertyBone:
			property_palette_pick_bone(&hit_box->bone_idx,-1);
			break;

	}
}

