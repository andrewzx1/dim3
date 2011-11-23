/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette Virtual Control Sticks

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

#define kStickOn								0
#define kStickUseBitmap							1
#define kStickOuterBitmap						2
#define kStickInnerBitmap						3
#define kStickPositionX							4
#define kStickPositionY							5
#define kStickPositionWid						6
#define kStickPositionHigh						7
#define kStickFlipX								8
#define kStickFlipY								9
#define kStickColor								10

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

/* =======================================================

      Property Palette Fill Virtual Control
      
======================================================= */

void alt_property_palette_fill_virtual_control_stick(int virtual_control_stick_idx)
{
	char						str[256];
	iface_virtual_stick_type	*stick;
	
	sprintf(str,"Stick %d",virtual_control_stick_idx);
	list_palette_set_sub_title(&alt_property_palette,"Virtual Control",str);

	stick=&iface.virtual_control.sticks[virtual_control_stick_idx];
		
	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_checkbox(&alt_property_palette,kStickOn,"On",&stick->on,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kStickUseBitmap,"Use Bitmap",&stick->use_bitmap,FALSE);
	list_palette_add_string(&alt_property_palette,kStickOuterBitmap,"Outer Bitmap",stick->outer_bitmap_name,FALSE);
	list_palette_add_string(&alt_property_palette,kStickInnerBitmap,"Inner Bitmap",stick->inner_bitmap_name,FALSE);
	list_palette_add_pick_color(&alt_property_palette,kStickColor,"Color",&stick->color,FALSE);
	
	list_palette_add_header(&alt_property_palette,0,"Position");
	list_palette_add_int(&alt_property_palette,kStickPositionX,"X",&stick->pnt.x,FALSE);
	list_palette_add_int(&alt_property_palette,kStickPositionY,"Y",&stick->pnt.y,FALSE);
	list_palette_add_int(&alt_property_palette,kStickPositionWid,"Width",&stick->size.x,FALSE);
	list_palette_add_int(&alt_property_palette,kStickPositionHigh,"Height",&stick->size.y,FALSE);

	list_palette_add_header(&alt_property_palette,0,"Axis");
	list_palette_add_checkbox(&alt_property_palette,kStickFlipX,"Flip X",&stick->flip_x,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kStickFlipY,"Flip Y",&stick->flip_y,FALSE);
}

/* =======================================================

      Property Palette Click Virtual Control
      
======================================================= */

void alt_property_palette_click_virtual_control_stick(int virtual_control_stick_idx,int id,bool double_click)
{
	iface_virtual_stick_type	*stick;

	if (!double_click) return;

	stick=&iface.virtual_control.sticks[virtual_control_stick_idx];

	switch (id) {
	
		case kStickOuterBitmap:
			property_pick_file("Pick a Outer Stick Bitmap","Bitmaps/Virtual","png",NULL,stick->outer_bitmap_name);
			break;
			
		case kStickInnerBitmap:
			property_pick_file("Pick a Inner Stick Bitmap","Bitmaps/Virtual","png",NULL,stick->inner_bitmap_name);
			break;
	
	}
}

