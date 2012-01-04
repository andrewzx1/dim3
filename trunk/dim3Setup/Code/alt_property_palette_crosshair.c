/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette Crosshair

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

#define kCrosshairSettingsName					0
#define kCrosshairSettingsFileName				1

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

/* =======================================================

      Property Palette Fill Crosshair
      
======================================================= */

void alt_property_palette_fill_crosshair(int crosshair_idx)
{
	iface_crosshair_type		*crosshair;

	crosshair=&iface.crosshair_list.crosshairs[crosshair_idx];

	list_palette_set_title(&alt_property_palette,"Crosshairs",NULL,"Crosshair",crosshair->name,NULL,NULL);

		// settings

	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_string(&alt_property_palette,kCrosshairSettingsName,"Name",crosshair->name,FALSE);
	list_palette_add_string(&alt_property_palette,kCrosshairSettingsFileName,"Bitmap",crosshair->bitmap_name,FALSE);
}

/* =======================================================

      Property Palette Click Crosshair
      
======================================================= */

void alt_property_palette_click_crosshair(int crosshair_idx,int id,bool double_click)
{
	iface_crosshair_type		*crosshair;

	if (!double_click) return;

	crosshair=&iface.crosshair_list.crosshairs[crosshair_idx];

	switch (id) {

			// settings

		case kCrosshairSettingsName:
			dialog_property_string_run(list_string_value_string,(void*)crosshair->name,name_str_len,0,0);
			break;

		case kCrosshairSettingsFileName:
			property_pick_file("Pick a Crosshair Bitmap","Bitmaps/Crosshairs","png",NULL,crosshair->bitmap_name);
			break;

	}
}

