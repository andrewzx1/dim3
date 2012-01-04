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
	
#define kButtonPositionX						10
#define kButtonPositionY						11
#define kButtonPositionWid						12
#define kButtonPositionHigh						13

#define kButtonMobileSkip						20
#define kButtonMobilePositionX					21
#define kButtonMobilePositionY					22

#define kButtonDescPositionX					30
#define kButtonDescPositionY					31
#define kButtonDescTextSize						32

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

/* =======================================================

      Property Palette Intro Button Utilities
      
======================================================= */

iface_intro_button_type* get_intro_button_from_item_idx(int item_idx,char *name)
{
	int				idx;

	if ((item_idx>=item_intro_button_simple_save_start) && (item_idx<(item_intro_button_simple_save_start+max_simple_save_spot))) {
		idx=item_idx-item_intro_button_simple_save_start;
		if (name!=NULL) sprintf(name,"Simple Save %d",idx);
		return(&iface.intro.simple_save[idx].button_start);
	}

	if ((item_idx>=item_intro_button_simple_save_erase) && (item_idx<(item_intro_button_simple_save_erase+max_simple_save_spot))) {
		idx=item_idx-item_intro_button_simple_save_erase;
		if (name!=NULL) sprintf(name,"Simple Erase %d",idx);
		return(&iface.intro.simple_save[idx].button_erase);
	}

	switch (item_idx) {

		case item_intro_button_game_new:
			if (name!=NULL) strcpy(name,"Game New");
			return(&iface.intro.button_game_new);

		case item_intro_button_game_load:
			if (name!=NULL) strcpy(name,"Game Load");
			return(&iface.intro.button_game_load);

		case item_intro_button_game_setup:
			if (name!=NULL) strcpy(name,"Game Setup");
			return(&iface.intro.button_game_setup);

		case item_intro_button_game_new_easy:
			if (name!=NULL) strcpy(name,"Game New Easy");
			return(&iface.intro.button_game_new_easy);

		case item_intro_button_game_new_medium:
			if (name!=NULL) strcpy(name,"Game New Medium");
			return(&iface.intro.button_game_new_medium);

		case item_intro_button_game_new_hard:
			if (name!=NULL) strcpy(name,"Game New Hard");
			return(&iface.intro.button_game_new_hard);

		case item_intro_button_game_new_cancel:
			if (name!=NULL) strcpy(name,"Game New Cancel");
			return(&iface.intro.button_game_new_cancel);

		case item_intro_button_multiplayer_host:
			if (name!=NULL) strcpy(name,"Multiplayer Host");
			return(&iface.intro.button_multiplayer_host);

		case item_intro_button_multiplayer_join:
			if (name!=NULL) strcpy(name,"Multiplayer Join");
			return(&iface.intro.button_multiplayer_join);

		case item_intro_button_multiplayer_setup:
			if (name!=NULL) strcpy(name,"Multiplayer Setup");
			return(&iface.intro.button_multiplayer_setup);

		case item_intro_button_credit:
			if (name!=NULL) strcpy(name,"Credits");
			return(&iface.intro.button_credit);

		case item_intro_button_quit:
			if (name!=NULL) strcpy(name,"Quit");
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
	char								name[64];
	iface_intro_button_type				*btn;
	iface_intro_simple_save_desc_type	*desc;

	btn=get_intro_button_from_item_idx(intro_button_idx,name);
	desc=get_intro_button_desc_from_item_idx(intro_button_idx);

	list_palette_set_title(&alt_property_palette,"Intro Button",name,NULL,NULL,NULL,NULL);

		// settings
	
	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_checkbox(&alt_property_palette,kButtonOptionOn,"On",&btn->on,FALSE);
	
		// position

	list_palette_add_header(&alt_property_palette,0,"Position");
	list_palette_add_int(&alt_property_palette,kButtonPositionX,"X",&btn->x,FALSE);
	list_palette_add_int(&alt_property_palette,kButtonPositionY,"Y",&btn->y,FALSE);
	list_palette_add_int(&alt_property_palette,kButtonPositionWid,"Width",&btn->wid,FALSE);
	list_palette_add_int(&alt_property_palette,kButtonPositionHigh,"Height",&btn->high,FALSE);

		// mobile override

	list_palette_add_header(&alt_property_palette,0,"Mobile Override");
	list_palette_add_checkbox(&alt_property_palette,kButtonMobileSkip,"Hide if Mobile",&btn->mobile_hide,FALSE);
	list_palette_add_int(&alt_property_palette,kButtonMobilePositionX,"X",&btn->mobile_x,FALSE);
	list_palette_add_int(&alt_property_palette,kButtonMobilePositionY,"Y",&btn->mobile_y,FALSE);

		// description

	if (desc!=NULL) {
		list_palette_add_header(&alt_property_palette,0,"Save Description");
		list_palette_add_int(&alt_property_palette,kButtonDescPositionX,"X",&desc->x,FALSE);
		list_palette_add_int(&alt_property_palette,kButtonDescPositionY,"Y",&desc->y,FALSE);
		list_palette_add_int(&alt_property_palette,kButtonDescTextSize,"Text Size",&desc->text_size,FALSE);
	}
}

/* =======================================================

      Property Palette Click Intro Buttons
      
======================================================= */

void alt_property_palette_click_intro_button(int intro_button_idx,int id,bool double_click)
{
}

