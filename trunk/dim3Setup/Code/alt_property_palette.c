/****************************** File *********************************

Module: dim3 Setup
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
	#include "dim3setup.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

list_palette_type				alt_property_palette;

extern setup_state_type			state;

/* =======================================================

      Alt Property Palette Setup
      
======================================================= */

void alt_property_palette_initialize(void)
{
	list_palette_list_initialize(&alt_property_palette,"No Properties");

	alt_property_palette.item_type=0;
	alt_property_palette.item_idx=-1;
}

void alt_property_palette_shutdown(void)
{
	list_palette_list_shutdown(&alt_property_palette);
}

void alt_property_palette_setup(void)
{
	d3rect			wbox;
	
	os_get_window_box(&wbox);

	alt_property_palette.pixel_sz=list_palette_tree_sz;

	alt_property_palette.box.lx=list_palette_tree_sz*2;
	alt_property_palette.box.rx=alt_property_palette.box.lx+list_palette_tree_sz;
	alt_property_palette.box.ty=wbox.ty;
	alt_property_palette.box.by=wbox.by;
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

		case item_interface_intro:
			if (state.cur_intro_button_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"Intro Button");
				alt_property_palette_fill_intro_button(state.cur_intro_button_idx);
				break;
			}
			if (state.cur_intro_model_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"Intro Model");
				alt_property_palette_fill_intro_model(state.cur_intro_model_idx);
				break;
			}
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

		case item_interface_intro:
			if (state.cur_intro_button_idx!=-1) {
				alt_property_palette_click_intro_button(state.cur_intro_button_idx,alt_property_palette.item_id);
				break;
			}
			if (state.cur_intro_model_idx!=-1) {
				alt_property_palette_click_intro_model(state.cur_intro_model_idx,alt_property_palette.item_id);
				break;
			}

	}
}

