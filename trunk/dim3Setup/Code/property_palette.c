/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette

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

extern file_path_setup_type		file_path_setup;
extern setup_state_type			state;

list_palette_type				property_palette;

/* =======================================================

      Property Palette Setup
      
======================================================= */

void property_palette_initialize(void)
{
	list_palette_list_initialize(&property_palette,"No Properties");

	property_palette.item_type=0;
	property_palette.item_idx=-1;
}

void property_palette_shutdown(void)
{
	list_palette_list_shutdown(&property_palette);
}

void property_palette_setup(void)
{
	d3rect			wbox;
	
	os_get_window_box(&wbox);

	property_palette.pixel_sz=list_palette_tree_sz;

	property_palette.box.lx=list_palette_tree_sz;
	property_palette.box.rx=property_palette.box.lx+list_palette_tree_sz;
	property_palette.box.ty=wbox.ty;
	property_palette.box.by=wbox.by;
}

/* =======================================================

      Property Palette Fill
      
======================================================= */

void property_palette_fill(void)
{
		// delete the properties

	list_palette_delete_all_items(&property_palette);

		// selection properties

	switch (state.cur_item) {

		case item_interface_sound:
			list_palette_set_title(&property_palette,"Sounds List");
			property_palette_fill_sounds();
			break;

	}
}

/* =======================================================

      Property Palette Draw
      
======================================================= */

void property_palette_draw(void)
{
	property_palette_fill();
	list_palette_draw(&property_palette,FALSE);
}

/* =======================================================

      Property Palette Reset Scroll
      
======================================================= */

void property_palette_reset(void)
{
	property_palette.scroll_page=0;
}

/* =======================================================

      Property Palette Scroll Wheel
      
======================================================= */

void property_palette_scroll_wheel(d3pnt *pnt,int move)
{
	list_palette_scroll_wheel(&property_palette,pnt,move);
}

/* =======================================================

      Property Palette Click
      
======================================================= */

void property_palette_click(d3pnt *pnt,bool double_click)
{
		// click

	if (!list_palette_click(&property_palette,pnt,double_click)) return;

		// click editing

	if (property_palette.item_id==-1) return;

		// selection properties

	switch (state.cur_item) {

		case item_interface_sound:
			property_palette_click_sounds(property_palette.item_id);
			break;

	}
}
