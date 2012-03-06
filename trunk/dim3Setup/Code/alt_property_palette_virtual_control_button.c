/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Virtual Control

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
extern list_palette_type		property_palette;

extern char						control_name_str[][32];

/* =======================================================

      Property Palette Fill Virtual Control
      
======================================================= */

void property_palette_fill_virtual_control_button(int virtual_control_button_idx)
{
	char						str[256];
	iface_virtual_button_type	*button;
	
	sprintf(str,"%d",virtual_control_button_idx);
	list_palette_set_title(&property_palette,"Virtual Control",NULL,"Button",str,NULL,NULL);

		// buttons
		
	button=&iface.virtual_control.buttons[virtual_control_button_idx];
		
	list_palette_add_header(&property_palette,0,"Settings");
	list_palette_add_checkbox(&property_palette,kButtonOn,"On",&button->on,FALSE);
	list_palette_add_checkbox(&property_palette,kButtonUseBitmap,"Use Bitmap",&button->use_bitmap,FALSE);
	list_palette_add_picker_list_int(&property_palette,kButtonControl,"Action",(char*)control_name_str,-1,32,0,FALSE,&button->control_idx,FALSE);
	list_palette_add_picker_file(&property_palette,kButtonUpBitmap,list_button_none,0,"Up Bitmap","Bitmaps/Virtual","png","",button->up_bitmap_name,FALSE);
	list_palette_add_picker_file(&property_palette,kButtonDownBitmap,list_button_none,0,"Down Bitmap","Bitmaps/Virtual","png","",button->down_bitmap_name,FALSE);
	list_palette_add_pick_color(&property_palette,kButtonColor,"Color",&button->color,FALSE);
	
	list_palette_add_header(&property_palette,0,"Position");
	list_palette_add_int(&property_palette,kButtonPositionX,"X",&button->pnt.x,FALSE);
	list_palette_add_int(&property_palette,kButtonPositionY,"Y",&button->pnt.y,FALSE);
	list_palette_add_int(&property_palette,kButtonPositionWid,"Width",&button->size.x,FALSE);
	list_palette_add_int(&property_palette,kButtonPositionHigh,"Height",&button->size.y,FALSE);
}

/* =======================================================

      Property Palette Click Virtual Control Button
      
======================================================= */

void property_palette_click_virtual_control_button(int virtual_control_button_idx,int id,bool double_click)
{
}

