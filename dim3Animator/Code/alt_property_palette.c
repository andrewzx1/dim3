/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Alt Property Palette

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

extern model_type				model;
extern model_draw_setup			draw_setup;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern int						tool_palette_pixel_sz,txt_palette_pixel_sz;
extern bool						list_palette_open;

bool							alt_property_open;
list_palette_type				alt_property_palette;

/* =======================================================

      Alt Property Palette Setup
      
======================================================= */

void alt_property_palette_initialize(void)
{
	list_palette_list_initialize(&alt_property_palette,"No Properties");

	alt_property_palette.item_type=0;
	alt_property_palette.item_idx=-1;

	alt_property_open=FALSE;
}

void alt_property_palette_shutdown(void)
{
	list_palette_list_shutdown(&alt_property_palette);
}

void alt_property_palette_setup(void)
{
	d3rect			wbox;
	
	os_get_window_box(&wbox);

	if ((alt_property_open) && (list_palette_open)) {
		alt_property_palette.pixel_sz=list_palette_tree_sz;
	}
	else {
		alt_property_palette.pixel_sz=0;
	}

	alt_property_palette.box.lx=wbox.rx-alt_property_palette.pixel_sz;
	alt_property_palette.box.rx=wbox.rx;
	alt_property_palette.box.ty=wbox.ty+(tool_palette_pixel_sz+1);
	alt_property_palette.box.by=wbox.by-txt_palette_pixel_sz;
}

/* =======================================================

      Alt Property Open and Close
      
======================================================= */

void alt_property_fix_open_state(void)
{
	bool			old_alt_open;

		// determine state

	old_alt_open=alt_property_open;

	alt_property_open=((state.cur_item==item_animate) && (state.cur_animate_idx!=-1) && (state.cur_animate_pose_move_idx!=-1));
	alt_property_open|=((state.cur_item==item_pose) && (state.cur_pose_idx!=-1) && (state.cur_pose_bone_move_idx!=-1));

	if (state.texture_edit_idx!=-1) alt_property_open=FALSE;
	
	if (!alt_property_open) {
		state.cur_animate_pose_move_idx=-1;
		state.cur_pose_bone_move_idx=-1;
	}

		// if switched open state, fix and redraw

	if (old_alt_open!=alt_property_open) {
		item_palette_setup();
		property_palette_setup();
		alt_property_palette_setup();
	}
}

/* =======================================================

      Alt Property Palette Fill
      
======================================================= */

void alt_property_palette_fill(void)
{
		// delete the properties

	list_palette_delete_all_items(&alt_property_palette);

		// selection properties

	switch (state.cur_item) {

		case item_animate:
			list_palette_set_title(&alt_property_palette,"Animation Pose Properties");
			alt_property_palette_fill_animate_pose_move(state.cur_animate_idx,state.cur_animate_pose_move_idx);
			break;

		case item_pose:
			list_palette_set_title(&alt_property_palette,"Pose Bone Move Properties");
			alt_property_palette_fill_pose_bone_move(state.cur_pose_idx,state.cur_pose_bone_move_idx);
			break;

		default:
			list_palette_set_title(&alt_property_palette,"No Properties");
			break;

	}
}

/* =======================================================

      Alt Property Palette Draw
      
======================================================= */

void alt_property_palette_draw(void)
{
	alt_property_palette_fill();
	list_palette_draw(&alt_property_palette,FALSE);
}

/* =======================================================

      Alt Property Palette Scroll Wheel
      
======================================================= */

void alt_property_palette_scroll_wheel(d3pnt *pnt,int move)
{
	list_palette_scroll_wheel(&alt_property_palette,pnt,move);
}

/* =======================================================

      Alt Property Palette Click
      
======================================================= */

void alt_property_palette_click(d3pnt *pnt,bool double_click)
{
		// click

	if (!list_palette_click(&alt_property_palette,pnt,double_click)) return;

		// click editing

	if (alt_property_palette.item_id==-1) return;

		// selection properties

	switch (state.cur_item) {

		case item_animate:
			alt_property_palette_click_animate_pose_move(state.cur_animate_idx,state.cur_animate_pose_move_idx,alt_property_palette.item_id);
			break;

		case item_pose:
			alt_property_palette_click_pose_bone_move(state.cur_pose_idx,state.cur_pose_bone_move_idx,alt_property_palette.item_id);
			break;

	}
}

