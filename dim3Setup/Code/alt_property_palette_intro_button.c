/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette Intro Button

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

#define kButtonOptionOn							0
	
#define kButtonPositionX						1
#define kButtonPositionY						2
#define kButtonPositionWid						3
#define kButtonPositionHigh						4

#define kButtonDescPositionX					5
#define kButtonDescPositionY					6
#define kButtonDescTextSize						7

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

/* =======================================================

      Property Palette Intro Button Utilities
      
======================================================= */

iface_intro_button_type* get_intro_button_from_item_idx(int item_idx)
{
	int				idx;

	if ((item_idx>=item_intro_button_simple_save_start) && (item_idx<(item_intro_button_simple_save_start+max_simple_save_spot))) {
		idx=item_idx-item_intro_button_simple_save_start;
		return(&iface.intro.simple_save[idx].button_start);
	}

	if ((item_idx>=item_intro_button_simple_save_erase) && (item_idx<(item_intro_button_simple_save_erase+max_simple_save_spot))) {
		idx=item_idx-item_intro_button_simple_save_erase;
		return(&iface.intro.simple_save[idx].button_erase);
	}

	switch (item_idx) {
		case item_intro_button_game_new:
			return(&iface.intro.button_game_new);
		case item_intro_button_game_load:
			return(&iface.intro.button_game_load);
		case item_intro_button_game_setup:
			return(&iface.intro.button_game_setup);
		case item_intro_button_game_new_easy:
			return(&iface.intro.button_game_new_easy);
		case item_intro_button_game_new_medium:
			return(&iface.intro.button_game_new_medium);
		case item_intro_button_game_new_hard:
			return(&iface.intro.button_game_new_hard);
		case item_intro_button_game_new_cancel:
			return(&iface.intro.button_game_new_cancel);
		case item_intro_button_multiplayer_host:
			return(&iface.intro.button_multiplayer_host);
		case item_intro_button_multiplayer_join:
			return(&iface.intro.button_multiplayer_join);
		case item_intro_button_multiplayer_setup:
			return(&iface.intro.button_multiplayer_setup);
		case item_intro_button_credit:
			return(&iface.intro.button_credit);
		case item_intro_button_quit:
			return(&iface.intro.button_quit);
	}

	return(NULL);
}

iface_intro_simple_save_desc_type* get_intro_button_desc_from_item_idx(int item_idx)
{
	int				idx;

	if ((item_idx>=item_intro_button_simple_save_start) && (item_idx<(item_intro_button_simple_save_start+max_simple_save_spot))) {
		idx=item_idx-item_intro_button_simple_save_start;
		return(&iface.intro.simple_save[idx].desc);
	}

	return(NULL);
}

/* =======================================================

      Property Palette Fill Intro Button
      
======================================================= */

void alt_property_palette_fill_intro_button(int intro_button_idx)
{
	iface_intro_button_type				*btn;
	iface_intro_simple_save_desc_type	*desc;

	btn=get_intro_button_from_item_idx(intro_button_idx);
	desc=get_intro_button_desc_from_item_idx(intro_button_idx);

		// settings
	
	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_checkbox(&alt_property_palette,kButtonOptionOn,"On",btn->on,FALSE);
	
		// position

	list_palette_add_header(&alt_property_palette,0,"Position");
	list_palette_add_string_int(&alt_property_palette,kButtonPositionX,"X",btn->x,FALSE);
	list_palette_add_string_int(&alt_property_palette,kButtonPositionY,"Y",btn->y,FALSE);
	list_palette_add_string_int(&alt_property_palette,kButtonPositionWid,"Width",btn->wid,FALSE);
	list_palette_add_string_int(&alt_property_palette,kButtonPositionHigh,"Height",btn->high,FALSE);

		// description

	if (desc!=NULL) {
		list_palette_add_header(&alt_property_palette,0,"Save Description");
		list_palette_add_string_int(&alt_property_palette,kButtonDescPositionX,"X",desc->x,FALSE);
		list_palette_add_string_int(&alt_property_palette,kButtonDescPositionY,"Y",desc->y,FALSE);
		list_palette_add_string_int(&alt_property_palette,kButtonDescTextSize,"Text Size",desc->text_size,FALSE);
	}
}

/* =======================================================

      Property Palette Click Intro Buttons
      
======================================================= */

void alt_property_palette_click_intro_button(int intro_button_idx,int id)
{
	iface_intro_button_type				*btn;
	iface_intro_simple_save_desc_type	*desc;

	btn=get_intro_button_from_item_idx(intro_button_idx);
	desc=get_intro_button_desc_from_item_idx(intro_button_idx);

	switch (id) {

			// settings
	
		case kButtonOptionOn:
			btn->on=!btn->on;
			break;
	
			// position

		case kButtonPositionX:
			dialog_property_string_run(list_string_value_positive_int,(void*)&btn->x,0,0,0);
			break;

		case kButtonPositionY:
			dialog_property_string_run(list_string_value_positive_int,(void*)&btn->y,0,0,0);
			break;

		case kButtonPositionWid:
			dialog_property_string_run(list_string_value_int,(void*)&btn->wid,0,0,0);
			break;

		case kButtonPositionHigh:
			dialog_property_string_run(list_string_value_int,(void*)&btn->high,0,0,0);
			break;

			// description

		case kButtonDescPositionX:
			dialog_property_string_run(list_string_value_positive_int,(void*)&desc->x,0,0,0);
			break;

		case kButtonDescPositionY:
			dialog_property_string_run(list_string_value_positive_int,(void*)&desc->y,0,0,0);
			break;

		case kButtonDescTextSize:
			dialog_property_string_run(list_string_value_positive_int,(void*)&desc->text_size,0,0,0);
			break;

	}

		// redraw

	main_wind_draw();
}

