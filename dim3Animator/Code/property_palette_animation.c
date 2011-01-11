/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette Animation

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

#define kAnimationPropertyName					0
#define kAnimationPropertyLoop					1

#define kAnimationPropertyPoseMove				100

extern model_type				model;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Animation
      
======================================================= */

void property_palette_fill_animation(int animate_idx)
{
	int						n;
	char					str[256];
	model_animate_type		*animate;

	animate=&model.animates[animate_idx];

	list_palette_add_header(&property_palette,0,"Animation Options");
	list_palette_add_string(&property_palette,kAnimationPropertyName,"Name",animate->name,FALSE);

	list_palette_add_header(&property_palette,0,"Animation Settings");
	list_palette_add_checkbox(&property_palette,kAnimationPropertyLoop,"Looping",animate->loop,FALSE);
	
	list_palette_add_header(&property_palette,0,"Animation Poses");
	for (n=0;n!=animate->npose_move;n++) {
		strcpy(str,model.poses[animate->pose_moves[n].pose_idx].name);
		if (n==animate->loop_start) strcat(str," (loop start)");
		if (n==animate->loop_end) strcat(str," (loop end)");
		list_palette_add_string_selectable(&property_palette,(kAnimationPropertyPoseMove+n),str,NULL,((state.cur_animate_idx=animate_idx) && (state.cur_animate_pose_move_idx==n)),FALSE);
	}
}

/* =======================================================

      Property Palette Click Animation
      
======================================================= */

void property_palette_click_animation(int animate_idx,int id)
{
	model_animate_type		*animate;

	animate=&model.animates[animate_idx];
	
		// pose moves
		
	if (id>=kAnimationPropertyPoseMove) {
		state.cur_animate_pose_move_idx=id-kAnimationPropertyPoseMove;
		main_wind_draw();
		return;
	}

	state.cur_animate_pose_move_idx=-1;
	
		// regular animation settings

	switch (id) {

		case kAnimationPropertyName:
			dialog_property_string_run(list_string_value_string,(void*)animate->name,name_str_len,0,0);
			break;

		case kAnimationPropertyLoop:
			animate->loop=!animate->loop;
			break;

	}

		// redraw

	main_wind_draw();
}

