/****************************** File *********************************

Module: dim3 Editor
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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;
extern file_path_setup_type		file_path_setup;

extern bool						list_palette_open;

list_palette_type				alt_property_palette;

/* =======================================================

      Alt Property Palette Setup
      
======================================================= */

void alt_property_palette_initialize(void)
{
	list_palette_list_initialize(&alt_property_palette,"No Properties",TRUE);

	alt_property_palette.item_type=0;
	alt_property_palette.item_idx=-1;
}

void alt_property_palette_shutdown(void)
{
	list_palette_list_shutdown(&alt_property_palette);
}

/* =======================================================

      Alt Property Palette Fill
      
======================================================= */

void alt_property_palette_fill(void)
{
		// delete the properties

	list_palette_delete_all_items(&alt_property_palette);

		// selection properties

	if (state.cur_cinema_idx!=-1) {
		alt_property_palette_fill_cinema_action(state.cur_cinema_idx,state.cur_cinema_action_idx);
	}
	else {
		if (state.cur_movement_idx!=-1) {
			alt_property_palette_fill_movement_move(state.cur_movement_idx,state.cur_movement_move_idx);
		}
	}
}

/* =======================================================

      Alt Property Palette Draw
      
======================================================= */

void alt_property_palette_draw(void)
{
	if (list_palette_get_level()!=2) return;
	
	alt_property_palette_fill();
	list_palette_draw(&alt_property_palette);
}

/* =======================================================

      Alt Property Palette Scroll Wheel
      
======================================================= */

void alt_property_palette_scroll_wheel(d3pnt *pnt,int move)
{
	if (list_palette_get_level()==2) list_palette_scroll_wheel(&alt_property_palette,pnt,move);
}

/* =======================================================

      Alt Property Palette Click
      
======================================================= */

bool alt_property_palette_click(d3pnt *pnt,bool double_click)
{
	if (list_palette_get_level()!=2) return(FALSE);
	
		// click

	if (!list_palette_click(&alt_property_palette,pnt,double_click)) return(TRUE);

		// click editing

	if (alt_property_palette.item_id==-1) return(TRUE);

		// selection properties

	if (state.cur_cinema_idx!=-1) {
		alt_property_palette_click_cinema_action(state.cur_cinema_idx,state.cur_cinema_action_idx,alt_property_palette.item_id);
	}
	else {
		if (state.cur_movement_idx!=-1) {
			alt_property_palette_click_movement_move(state.cur_movement_idx,state.cur_movement_move_idx,alt_property_palette.item_id);
		}
	}
	
	return(TRUE);
}

