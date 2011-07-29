/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette Multiplayer Option

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

#define kMPOptionPropertySettingsName			0
#define kMPOptionPropertySettingsDescript		1

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

/* =======================================================

      Property Palette Fill Multiplayer Option
      
======================================================= */

void alt_property_palette_fill_multiplayer_option(int multiplayer_option_idx)
{
	iface_net_option_type		*option;

	option=&iface.net_option.options[multiplayer_option_idx];

	list_palette_set_sub_title(&alt_property_palette,"Option",option->name);

		// settings

	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_string(&alt_property_palette,kMPOptionPropertySettingsName,"Name",option->name,FALSE);
	list_palette_add_string(&alt_property_palette,kMPOptionPropertySettingsDescript,"Description",option->descript,FALSE);
}

/* =======================================================

      Property Palette Click Multiplayer Option
      
======================================================= */

void alt_property_palette_click_multiplayer_option(int multiplayer_option_idx,int id,bool double_click)
{
	iface_net_option_type		*option;

	option=&iface.net_option.options[multiplayer_option_idx];

	if (!double_click) return;

	switch (id) {
			
			// settings

		case kMPOptionPropertySettingsName:
			dialog_property_string_run(list_string_value_string,(void*)option->name,name_str_len,0,0);
			break;

		case kMPOptionPropertySettingsDescript:
			dialog_property_string_run(list_string_value_string,(void*)option->descript,name_str_len,0,0);
			break;

	}

		// redraw

	main_wind_draw();
}

