/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette Virtual Control

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

#define kButtonOn								0
#define kButtonUseBitmap						1
#define kButtonControl							2
#define kButtonUpBitmap							3
#define kButtonDownBitmap						4
#define kButtonPositionX						5
#define kButtonPositionY						6
#define kButtonPositionWid						7
#define kButtonPositionHigh						8
#define kButtonColor							9

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

extern char						control_name_str[][32];

/* =======================================================

      Property Palette Fill Virtual Control
      
======================================================= */

void alt_property_palette_fill_virtual_control_button(int virtual_control_button_idx)
{
	char						str[256];
	iface_virtual_button_type	*button;
	
	sprintf(str,"Button %d",virtual_control_button_idx);
	list_palette_set_sub_title(&alt_property_palette,"Virtual Control",str);

		// buttons
		
	button=&iface.virtual_control.buttons[virtual_control_button_idx];
		
	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_checkbox(&alt_property_palette,kButtonOn,"On",button->on,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kButtonUseBitmap,"Use Bitmap",button->use_bitmap,FALSE);
	list_palette_add_string(&alt_property_palette,kButtonControl,"Action",control_name_str[button->control_idx],FALSE);
	list_palette_add_string(&alt_property_palette,kButtonUpBitmap,"Up Bitmap",button->up_bitmap_name,FALSE);
	list_palette_add_string(&alt_property_palette,kButtonDownBitmap,"Down Bitmap",button->down_bitmap_name,FALSE);
	list_palette_add_pick_color(&alt_property_palette,kButtonColor,"Color",&button->color,FALSE);
	
	list_palette_add_header(&alt_property_palette,0,"Position");
	list_palette_add_string_int(&alt_property_palette,kButtonPositionX,"X",button->x,FALSE);
	list_palette_add_string_int(&alt_property_palette,kButtonPositionY,"Y",button->y,FALSE);
	list_palette_add_string_int(&alt_property_palette,kButtonPositionWid,"Width",button->x_size,FALSE);
	list_palette_add_string_int(&alt_property_palette,kButtonPositionHigh,"Height",button->y_size,FALSE);
}

/* =======================================================

      Property Palette Click Virtual Control Button
      
======================================================= */

void alt_property_palette_click_virtual_control_button(int virtual_control_button_idx,int id,bool double_click)
{
	iface_virtual_button_type	*button;

	if (!double_click) return;

	button=&iface.virtual_control.buttons[virtual_control_button_idx];

	switch (id) {

		case kButtonOn:
			button->on=!button->on;
			break;
			
		case kButtonUseBitmap:
			button->use_bitmap=!button->use_bitmap;
			break;
			
		case kButtonControl:
			property_palette_pick_control(&button->control_idx);
			break;
			 
		case kButtonUpBitmap:
			property_pick_file("Pick a Up Button Bitmap","Bitmaps/Virtual","png",NULL,button->up_bitmap_name);
			break;
			
		case kButtonDownBitmap:
			property_pick_file("Pick a Down Button Bitmap","Bitmaps/Virtual","png",NULL,button->down_bitmap_name);
			break;
			
		case kButtonColor:
			os_pick_color(&button->color);
			break;
	
		case kButtonPositionX:
			dialog_property_string_run(list_string_value_positive_int,(void*)&button->x,0,0,0);
			break;

		case kButtonPositionY:
			dialog_property_string_run(list_string_value_positive_int,(void*)&button->y,0,0,0);
			break;

		case kButtonPositionWid:
			dialog_property_string_run(list_string_value_int,(void*)&button->x_size,0,0,0);
			break;

		case kButtonPositionHigh:
			dialog_property_string_run(list_string_value_int,(void*)&button->y_size,0,0,0);
			break;

	}

		// redraw

	main_wind_draw();
}

