/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Alt2 Property Palette

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

list_palette_type				alt2_property_palette;

extern iface_type				iface;
extern animator_state_type		state;

/* =======================================================

      Alt2 Property Palette Setup
      
======================================================= */

void alt2_property_palette_initialize(void)
{
	list_palette_list_initialize(&alt2_property_palette,"No Properties",TRUE);

	alt2_property_palette.item_type=0;
	alt2_property_palette.item_idx=-1;
}

void alt2_property_palette_shutdown(void)
{
	list_palette_list_shutdown(&alt2_property_palette);
}

/* =======================================================

      Alt2 Property Palette Fill
      
======================================================= */

void alt2_property_palette_fill(void)
{
		// delete the properties

	list_palette_delete_all_items(&alt2_property_palette);

		// selection properties

	switch (state.cur_item) {

		case item_animate:
			if ((state.cur_animate_idx==-1) || (state.cur_animate_pose_move_idx==-1)) break;

			if (state.cur_animate_pose_move_particle_idx!=-1) {
				alt2_property_palette_fill_animate_pose_move_particle(state.cur_animate_idx,state.cur_animate_pose_move_idx,state.cur_animate_pose_move_particle_idx);
				break;
			}

			if (state.cur_animate_pose_move_ring_idx!=-1) {
				alt2_property_palette_fill_animate_pose_move_ring(state.cur_animate_idx,state.cur_animate_pose_move_idx,state.cur_animate_pose_move_ring_idx);
				break;
			}

			break;

	}
}

/* =======================================================

      Alt2 Property Palette Draw
      
======================================================= */

void alt2_property_palette_draw(void)
{
	if (list_palette_get_level()!=3) return;

	alt2_property_palette_fill();
	list_palette_draw(&alt2_property_palette);
}

/* =======================================================

      Alt2 Property Palette Scroll Wheel
      
======================================================= */

void alt2_property_palette_scroll_wheel(d3pnt *pnt,int move)
{
	if (list_palette_get_level()==3) list_palette_scroll_wheel(&alt2_property_palette,pnt,move);
}

/* =======================================================

      Alt2 Property Palette Click
      
======================================================= */

bool alt2_property_palette_click(d3pnt *pnt,bool double_click)
{
	if (list_palette_get_level()!=3) return(FALSE);
	
		// click

	if (!list_palette_click(&alt2_property_palette,pnt,double_click)) return(TRUE);

		// click editing

	if (alt2_property_palette.item_id==-1) return(TRUE);

		// selection properties

	switch (state.cur_item) {

		case item_animate:
			if ((state.cur_animate_idx==-1) || (state.cur_animate_pose_move_idx==-1)) break;

			if (state.cur_animate_pose_move_particle_idx!=-1) {
				alt2_property_palette_click_animate_pose_move_particle(state.cur_animate_idx,state.cur_animate_pose_move_idx,state.cur_animate_pose_move_particle_idx,alt2_property_palette.item_id,double_click);
				break;
			}

			if (state.cur_animate_pose_move_ring_idx!=-1) {
				alt2_property_palette_click_animate_pose_move_ring(state.cur_animate_idx,state.cur_animate_pose_move_idx,state.cur_animate_pose_move_ring_idx,alt2_property_palette.item_id,double_click);
				break;
			}

			break;

	}
	
	main_wind_draw();

	return(TRUE);
}
