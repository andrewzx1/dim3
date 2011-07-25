/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Item Palette

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

extern iface_type				iface;
extern setup_state_type			state;

list_palette_type				item_palette;

/* =======================================================

      Item Palette Setup
      
======================================================= */

void item_palette_initialize(void)
{
	list_palette_list_initialize(&item_palette,"Project Items",FALSE);

	item_palette.item_type=0;
	item_palette.item_idx=-1;

	state.cur_item=item_interface_settings;
	state.cur_intro_button_idx=-1;
	state.cur_intro_model_idx=-1;
	state.cur_hud_bitmap_idx=-1;
	state.cur_hud_text_idx=-1;
	state.cur_hud_bar_idx=-1;
	state.cur_radar_icon_idx=-1;
	state.cur_menu_idx=-1;
	state.cur_menu_item_idx=-1;
	state.cur_chooser_idx=-1;
	state.cur_chooser_piece_idx=-1;
	state.cur_multiplayer_character_idx=-1;
	state.cur_multiplayer_game_idx=-1;
	state.cur_multiplayer_option_idx=-1;
	state.cur_sound_idx=-1;
	state.cur_particle_idx=-1;
	state.cur_group_particle_idx=-1;
	state.cur_ring_idx=-1;
	state.cur_halo_idx=-1;
	state.cur_mark_idx=-1;
	state.cur_crosshair_idx=-1;
	state.cur_action_idx=-1;
	state.cur_shader_idx=-1;
}

void item_palette_shutdown(void)
{
	list_palette_list_shutdown(&item_palette);
}

/* =======================================================

      Item Palette Fill
      
======================================================= */

void item_palette_fill(void)
{
	list_palette_delete_all_items(&item_palette);

		// project

	list_palette_add_header(&item_palette,item_interface,"Project");

	list_palette_add_item(&item_palette,item_interface_settings,0,"Settings",(state.cur_item==item_interface_settings),FALSE);
	list_palette_add_item(&item_palette,item_interface_intro,0,"Intro",(state.cur_item==item_interface_intro),FALSE);
	list_palette_add_item(&item_palette,item_interface_setup,0,"Setup",(state.cur_item==item_interface_setup),FALSE);
	list_palette_add_item(&item_palette,item_interface_hud,0,"HUD",(state.cur_item==item_interface_hud),FALSE);
	list_palette_add_item(&item_palette,item_interface_radar,0,"Radar",(state.cur_item==item_interface_radar),FALSE);
	list_palette_add_item(&item_palette,item_interface_multiplayer,0,"Multiplayer",(state.cur_item==item_interface_multiplayer),FALSE);
	list_palette_add_item(&item_palette,item_interface_menu,0,"Menu",(state.cur_item==item_interface_menu),FALSE);
	list_palette_add_item(&item_palette,item_interface_chooser,0,"Choosers",(state.cur_item==item_interface_chooser),FALSE);
	list_palette_add_item(&item_palette,item_interface_sound,0,"Sound",(state.cur_item==item_interface_sound),FALSE);
	list_palette_add_item(&item_palette,item_interface_particle,0,"Particles",(state.cur_item==item_interface_particle),FALSE);
	list_palette_add_item(&item_palette,item_interface_ring,0,"Rings",(state.cur_item==item_interface_ring),FALSE);
	list_palette_add_item(&item_palette,item_interface_halo,0,"Halos",(state.cur_item==item_interface_halo),FALSE);
	list_palette_add_item(&item_palette,item_interface_mark,0,"Marks",(state.cur_item==item_interface_mark),FALSE);
	list_palette_add_item(&item_palette,item_interface_crosshair,0,"Crosshairs",(state.cur_item==item_interface_crosshair),FALSE);
	list_palette_add_item(&item_palette,item_interface_action,0,"Actions",(state.cur_item==item_interface_action),FALSE);
	list_palette_add_item(&item_palette,item_interface_shader,0,"Shaders",(state.cur_item==item_interface_shader),FALSE);
}

/* =======================================================

      Item Palette Draw
      
======================================================= */

void item_palette_draw(void)
{
	if (list_palette_get_level()!=0) return;
	
	item_palette_fill();
	list_palette_draw(&item_palette);
}

/* =======================================================

      Item Palette Delete
      
======================================================= */

bool item_palette_delete(void)
{
	return(FALSE);
}

/* =======================================================

      Item Palette Scroll Wheel
      
======================================================= */

void item_palette_scroll_wheel(d3pnt *pnt,int move)
{
	if (list_palette_get_level()==0) list_palette_scroll_wheel(&item_palette,pnt,move);
}

/* =======================================================

      Item Palette Click
      
======================================================= */

bool item_palette_click(d3pnt *pnt,bool double_click)
{
	if (list_palette_get_level()!=0) return(FALSE);
	
		// click

	if (!list_palette_click(&item_palette,pnt,double_click)) return(TRUE);

	if (item_palette.item_idx==-1) return(TRUE);
	
		// handle click

	state.cur_item=item_palette.item_type;

	list_palette_set_level(1);
	main_wind_draw();
	
	return(TRUE);
}

