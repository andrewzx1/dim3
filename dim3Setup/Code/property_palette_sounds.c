/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Sounds

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
	#include "dim3setup.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kSoundsPropertyAdd						0

#define kSoundProperyName						1000
#define kSoundProperyDelete						2000

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Sounds
      
======================================================= */

void property_palette_fill_sounds(void)
{
	int						n;

	list_palette_add_header_button(&property_palette,kSoundsPropertyAdd,"Sounds",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=iface.sound_list.nsound;n++) {
		list_palette_add_string_selectable_button(&property_palette,(kSoundProperyName+n),list_button_minus,(kSoundProperyDelete+n),iface.sound_list.sounds[n].name,NULL,(state.cur_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);
}

/* =======================================================

      Property Palette Click Sounds
      
======================================================= */

void property_palette_click_sounds(int id)
{
	/*
	model_animate_type		*animate;

		// pose moves
		
	if ((id>=kAnimationPropertyPoseMove) && (id<kAnimationPropertyPoseMoveDelete)) {
		state.cur_animate_pose_move_idx=id-kAnimationPropertyPoseMove;
		main_wind_draw();
		return;
	}
	
		// pose move delete
		
	if (id>=kAnimationPropertyPoseMoveDelete) {
		state.cur_animate_pose_move_idx=-1;
		model_animate_pose_delete(&model,animate_idx,(id-kAnimationPropertyPoseMoveDelete));
		main_wind_draw();
		return;
	}
	
		// pose move add
		
	if (id==kAnimationPropertyPoseAdd) {
		state.cur_animate_pose_move_idx=model_animate_pose_insert(&model,animate_idx,state.cur_animate_pose_move_idx,0);
		property_palette_pick_pose(&model.animates[animate_idx].pose_moves[state.cur_animate_pose_move_idx].pose_idx);
		main_wind_draw();
		return;
	}
	
		// regular click

	state.cur_animate_pose_move_idx=-1;

	switch (id) {

		case kAnimationPropertyName:
			dialog_property_string_run(list_string_value_string,(void*)animate->name,name_str_len,0,0);
			break;

		case kAnimationPropertyLoop:
			animate->loop=!animate->loop;
			break;
			
		case kAnimationPropertyNoSmooth:
			animate->no_smooth=!animate->no_smooth;
			break;

	}
*/
		// redraw

	main_wind_draw();
}

