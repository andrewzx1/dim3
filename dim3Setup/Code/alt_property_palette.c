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
				return;
			}
			if (state.cur_intro_model_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"Intro Model");
				alt_property_palette_fill_intro_model(state.cur_intro_model_idx);
				return;
			}
			break;

		case item_interface_hud:
			if (state.cur_hud_bitmap_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"HUD Bitmap");
				alt_property_palette_fill_hud_bitmap(state.cur_hud_bitmap_idx);
				return;
			}
			if (state.cur_hud_text_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"HUD Text");
				alt_property_palette_fill_hud_text(state.cur_hud_text_idx);
				return;
			}
			if (state.cur_hud_bar_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"HUD Bar");
				alt_property_palette_fill_hud_bar(state.cur_hud_bar_idx);
				return;
			}
			break;

		case item_interface_radar:
			if (state.cur_radar_icon_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"Radar Icon");
				alt_property_palette_fill_radar_icon(state.cur_radar_icon_idx);
				return;
			}
			break;

		case item_interface_multiplayer:
			if (state.cur_multiplayer_character_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"Multiplayer Character");
				alt_property_palette_fill_multiplayer_character(state.cur_multiplayer_character_idx);
				return;
			}
			if (state.cur_multiplayer_game_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"Multiplayer Game");
				alt_property_palette_fill_multiplayer_game(state.cur_multiplayer_game_idx);
				return;
			}
			if (state.cur_multiplayer_option_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"Multiplayer Option");
				alt_property_palette_fill_multiplayer_option(state.cur_multiplayer_option_idx);
				return;
			}
			break;

		case item_interface_menu:
			if (state.cur_menu_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"Menu");
				alt_property_palette_fill_menu(state.cur_menu_idx);
				return;
			}
			break;

		case item_interface_chooser:
			if (state.cur_chooser_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"Chooser");
				alt_property_palette_fill_chooser(state.cur_chooser_idx);
				return;
			}
			break;

		case item_interface_sound:
			if (state.cur_sound_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"Sound");
				alt_property_palette_fill_sound(state.cur_sound_idx);
				return;
			}
			break;

		case item_interface_particle:
			if (state.cur_particle_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"Particle");
				alt_property_palette_fill_particle(state.cur_particle_idx);
				return;
			}
			break;

		case item_interface_ring:
			if (state.cur_ring_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"Ring");
				alt_property_palette_fill_ring(state.cur_ring_idx);
				return;
			}
			break;

		case item_interface_halo:
			if (state.cur_halo_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"Halo");
				alt_property_palette_fill_halo(state.cur_halo_idx);
				return;
			}
			break;

		case item_interface_mark:
			if (state.cur_mark_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"Mark");
				alt_property_palette_fill_mark(state.cur_mark_idx);
				return;
			}
			break;

		case item_interface_crosshair:
			if (state.cur_crosshair_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"Crosshair");
				alt_property_palette_fill_crosshair(state.cur_crosshair_idx);
				return;
			}
			break;

		case item_interface_action:
			if (state.cur_action_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"Action");
				alt_property_palette_fill_action(state.cur_action_idx);
				return;
			}
			break;

		case item_interface_shader:
			if (state.cur_shader_idx!=-1) {
				list_palette_set_title(&alt_property_palette,"Shader");
				alt_property_palette_fill_shader(state.cur_shader_idx);
				return;
			}
			break;

	}

	list_palette_set_title(&alt_property_palette,"No Properties");
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
			break;

		case item_interface_hud:
			if (state.cur_hud_bitmap_idx!=-1) {
				alt_property_palette_click_hud_bitmap(state.cur_hud_bitmap_idx,alt_property_palette.item_id);
				break;
			}
			if (state.cur_hud_text_idx!=-1) {
				alt_property_palette_click_hud_text(state.cur_hud_text_idx,alt_property_palette.item_id);
				break;
			}
			if (state.cur_hud_bar_idx!=-1) {
				alt_property_palette_click_hud_bar(state.cur_hud_bar_idx,alt_property_palette.item_id);
				break;
			}
			break;

		case item_interface_radar:
			if (state.cur_radar_icon_idx!=-1) {
				alt_property_palette_click_radar_icon(state.cur_radar_icon_idx,alt_property_palette.item_id);
				break;
			}
			break;

		case item_interface_multiplayer:
			if (state.cur_multiplayer_character_idx!=-1) {
				alt_property_palette_click_multiplayer_character(state.cur_multiplayer_character_idx,alt_property_palette.item_id);
				break;
			}
			if (state.cur_multiplayer_game_idx!=-1) {
				alt_property_palette_click_multiplayer_game(state.cur_multiplayer_game_idx,alt_property_palette.item_id);
				break;
			}
			if (state.cur_multiplayer_option_idx!=-1) {
				alt_property_palette_click_multiplayer_option(state.cur_multiplayer_option_idx,alt_property_palette.item_id);
				break;
			}
			break;

		case item_interface_menu:
			if (state.cur_menu_idx!=-1) {
				alt_property_palette_click_menu(state.cur_menu_idx,alt_property_palette.item_id);
				break;
			}
			break;

		case item_interface_chooser:
			if (state.cur_chooser_idx!=-1) {
				alt_property_palette_click_chooser(state.cur_chooser_idx,alt_property_palette.item_id);
				break;
			}
			break;

		case item_interface_sound:
			if (state.cur_sound_idx!=-1) {
				alt_property_palette_click_sound(state.cur_sound_idx,alt_property_palette.item_id);
				break;
			}
			break;

		case item_interface_particle:
			if (state.cur_particle_idx!=-1) {
				alt_property_palette_click_particle(state.cur_particle_idx,alt_property_palette.item_id);
				break;
			}
			break;

		case item_interface_ring:
			if (state.cur_ring_idx!=-1) {
				alt_property_palette_click_ring(state.cur_ring_idx,alt_property_palette.item_id);
				break;
			}
			break;

		case item_interface_halo:
			if (state.cur_halo_idx!=-1) {
				alt_property_palette_click_halo(state.cur_halo_idx,alt_property_palette.item_id);
				break;
			}
			break;

		case item_interface_mark:
			if (state.cur_mark_idx!=-1) {
				alt_property_palette_click_mark(state.cur_mark_idx,alt_property_palette.item_id);
				break;
			}
			break;

		case item_interface_crosshair:
			if (state.cur_crosshair_idx!=-1) {
				alt_property_palette_click_crosshair(state.cur_crosshair_idx,alt_property_palette.item_id);
				break;
			}
			break;

		case item_interface_action:
			if (state.cur_action_idx!=-1) {
				alt_property_palette_click_action(state.cur_action_idx,alt_property_palette.item_id);
				break;
			}
			break;

		case item_interface_shader:
			if (state.cur_shader_idx!=-1) {
				alt_property_palette_click_shader(state.cur_shader_idx,alt_property_palette.item_id);
				break;
			}
			break;

	}
}

