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

extern iface_type				iface;
extern file_path_setup_type		file_path_setup;
extern setup_state_type			state;

list_palette_type				property_palette;

/* =======================================================

      Property Palette Setup
      
======================================================= */

void property_palette_initialize(void)
{
	list_palette_list_initialize(&property_palette,"No Properties",TRUE);

	property_palette.item_type=0;
	property_palette.item_idx=-1;
}

void property_palette_shutdown(void)
{
	list_palette_list_shutdown(&property_palette);
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

		case item_interface_settings:
			property_palette_fill_settings();
			return;

		case item_interface_intro:
			property_palette_fill_intro();
			return;

		case item_interface_setup:
			property_palette_fill_setup();
			return;

		case item_interface_hud:
			property_palette_fill_hud();
			return;

		case item_interface_radar:
			property_palette_fill_radar();
			return;

		case item_interface_multiplayer:
			property_palette_fill_multiplayer();
			return;

		case item_interface_menu:
			property_palette_fill_menu();
			return;

		case item_interface_chooser:
			property_palette_fill_chooser();
			return;

		case item_interface_sound:
			property_palette_fill_sounds();
			return;

		case item_interface_particle:
			property_palette_fill_particles();
			return;

		case item_interface_ring:
			property_palette_fill_rings();
			return;

		case item_interface_halo:
			property_palette_fill_halos();
			return;

		case item_interface_mark:
			property_palette_fill_marks();
			return;

		case item_interface_crosshair:
			property_palette_fill_crosshairs();
			return;

		case item_interface_action:
			property_palette_fill_actions();
			return;

		case item_interface_shader:
			property_palette_fill_shaders();
			return;

	}
	
	list_palette_set_title(&property_palette,"No Properties");
}

/* =======================================================

      Property Palette Draw
      
======================================================= */

void property_palette_draw(void)
{
	if (list_palette_get_level()!=1) return;
	
	property_palette_fill();
	list_palette_draw(&property_palette);
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
	if (list_palette_get_level()==1) list_palette_scroll_wheel(&property_palette,pnt,move);
}

/* =======================================================

      Property Palette Click
      
======================================================= */

bool property_palette_click(d3pnt *pnt,bool double_click)
{
	if (list_palette_get_level()!=1) return(FALSE);

		// click

	if (!list_palette_click(&property_palette,pnt,double_click)) return(TRUE);

		// click editing

	if (property_palette.item_id==-1) return(TRUE);

		// selection properties

	switch (state.cur_item) {

		case item_interface_settings:
			property_palette_click_settings(property_palette.item_id,double_click);
			break;

		case item_interface_intro:
			property_palette_click_intro(property_palette.item_id,double_click);
			break;

		case item_interface_setup:
			property_palette_click_setup(property_palette.item_id,double_click);
			break;

		case item_interface_hud:
			property_palette_click_hud(property_palette.item_id,double_click);
			break;

		case item_interface_radar:
			property_palette_click_radar(property_palette.item_id,double_click);
			break;

		case item_interface_multiplayer:
			property_palette_click_multiplayer(property_palette.item_id,double_click);
			break;

		case item_interface_menu:
			property_palette_click_menu(property_palette.item_id,double_click);
			break;

		case item_interface_chooser:
			property_palette_click_chooser(property_palette.item_id,double_click);
			break;

		case item_interface_sound:
			property_palette_click_sounds(property_palette.item_id,double_click);
			break;

		case item_interface_particle:
			property_palette_click_particles(property_palette.item_id,double_click);
			break;

		case item_interface_ring:
			property_palette_click_rings(property_palette.item_id,double_click);
			break;

		case item_interface_halo:
			property_palette_click_halos(property_palette.item_id,double_click);
			break;

		case item_interface_mark:
			property_palette_click_marks(property_palette.item_id,double_click);
			break;

		case item_interface_crosshair:
			property_palette_click_crosshairs(property_palette.item_id,double_click);
			break;

		case item_interface_action:
			property_palette_click_actions(property_palette.item_id,double_click);
			break;

		case item_interface_shader:
			property_palette_click_shaders(property_palette.item_id,double_click);
			break;

	}
	
	return(TRUE);
}

/* =======================================================

      Property Palette Utilities
      
======================================================= */

void property_palette_pick_sound(char *name,bool include_none)
{
	list_palette_start_picking_mode("Pick a Sound",(char*)iface.sound_list.sounds,iface.sound_list.nsound,sizeof(iface_sound_type),(int)offsetof(iface_sound_type,name),include_none,NULL,name);
}

void property_palette_pick_particle(char *name)
{
	list_palette_start_picking_mode("Pick a Particle",(char*)iface.particle_list.particles,iface.particle_list.nparticle,sizeof(iface_particle_type),(int)offsetof(iface_particle_type,name),FALSE,NULL,name);
}

