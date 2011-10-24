/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette Radar Icon

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

#define kRadarIconSettingsName					0
#define kRadarIconSettingsBitmapName			1

#define kRadarIconOptionSize					2
#define kRadarIconOptionRotate					3

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

/* =======================================================

      Property Palette Fill Radar Icon
      
======================================================= */

void alt_property_palette_fill_radar_icon(int radar_icon_idx)
{
	iface_radar_icon_type		*icon;

	icon=&iface.radar.icons[radar_icon_idx];

	list_palette_set_sub_title(&alt_property_palette,"Radar Icon",icon->name);

		// settings

	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_string(&alt_property_palette,kRadarIconSettingsName,"Name",icon->name,FALSE);
	list_palette_add_string(&alt_property_palette,kRadarIconSettingsBitmapName,"Bitmap",icon->bitmap_name,FALSE);

		// options

	list_palette_add_header(&alt_property_palette,0,"Options");
	list_palette_add_string_int(&alt_property_palette,kRadarIconOptionSize,"Size",icon->size,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kRadarIconOptionRotate,"Rotate",&icon->rot,FALSE);
}

/* =======================================================

      Property Palette Click Radar Icon
      
======================================================= */

void alt_property_palette_click_radar_icon(int radar_icon_idx,int id,bool double_click)
{
	iface_radar_icon_type		*icon;

	if (!double_click) return;

	icon=&iface.radar.icons[radar_icon_idx];

	switch (id) {
			
			// settings

		case kRadarIconSettingsName:
			dialog_property_string_run(list_string_value_string,(void*)icon->name,name_str_len,0,0);
			break;

		case kRadarIconSettingsBitmapName:
			property_pick_file("Pick a Icon Bitmap","Bitmaps/Radar","png",NULL,icon->bitmap_name);
			break;

			// options

		case kRadarIconOptionSize:
			dialog_property_string_run(list_string_value_positive_int,(void*)&icon->size,0,0,0);
			break;

	}
}

