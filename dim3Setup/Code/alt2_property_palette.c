/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt2 Property Palette

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

list_palette_type				alt2_property_palette;

extern iface_type				iface;
extern setup_state_type			state;

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

		case item_interface_menu:
			if ((state.cur_menu_idx!=-1) && (state.cur_menu_item_idx!=-1)) {
				alt2_property_palette_fill_menu_item(state.cur_menu_idx,state.cur_menu_item_idx);
				return;
			}
			break;

		case item_interface_chooser:
			if ((state.cur_chooser_idx!=-1) && (state.cur_chooser_piece_idx!=-1)) {
				alt2_property_palette_fill_chooser_piece(state.cur_chooser_idx,state.cur_chooser_piece_idx);
				return;
			}
			break;

		case item_interface_particle:
			if (state.cur_particle_idx!=-1) {
				if (iface.particle_list.particles[state.cur_particle_idx].group.on) {
					if (state.cur_group_particle_idx!=-1) {
						alt2_property_palette_fill_group_particle(state.cur_particle_idx,state.cur_group_particle_idx);
						return;
					}
				}
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

		case item_interface_menu:
			if ((state.cur_menu_idx!=-1) && (state.cur_menu_item_idx!=-1)) {
				alt2_property_palette_click_menu_item(state.cur_menu_idx,state.cur_menu_item_idx,alt2_property_palette.item_id);
				break;
			}
			break;

		case item_interface_chooser:
			if ((state.cur_chooser_idx!=-1) && (state.cur_chooser_piece_idx!=-1)) {
				alt2_property_palette_click_chooser_piece(state.cur_chooser_idx,state.cur_chooser_piece_idx,alt2_property_palette.item_id);
				break;
			}
			break;

		case item_interface_particle:
			if (state.cur_particle_idx!=-1) {
				if (iface.particle_list.particles[state.cur_particle_idx].group.on) {
					if (state.cur_group_particle_idx!=-1) {
						alt2_property_palette_click_group_particle(state.cur_particle_idx,state.cur_group_particle_idx,alt2_property_palette.item_id);
						break;
					}
				}
			}
			break;


	}
	
	return(TRUE);
}

