/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Multiplayer Option

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
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
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Multiplayer Option
      
======================================================= */

void property_palette_fill_multiplayer_option(int multiplayer_option_idx)
{
	iface_net_option_type		*option;

	option=&iface.net_option.options[multiplayer_option_idx];

	list_palette_set_title(&property_palette,"Multiplayer",NULL,"Option",option->name,NULL,NULL);

		// settings

	list_palette_add_header(&property_palette,0,"Settings");
	list_palette_add_string(&property_palette,kMPOptionPropertySettingsName,"Name",option->name,name_str_len,FALSE);
	list_palette_add_string(&property_palette,kMPOptionPropertySettingsDescript,"Description",option->descript,name_str_len,FALSE);
}

/* =======================================================

      Property Palette Click Multiplayer Option
      
======================================================= */

void property_palette_click_multiplayer_option(int multiplayer_option_idx,bool double_click)
{
}

