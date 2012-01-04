/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette Action

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

#define kActionSettingsName						0
#define kActionSettingsShow						1

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

/* =======================================================

      Property Palette Fill Action
      
======================================================= */

void alt_property_palette_fill_action(int action_idx)
{
	iface_action_display_type		*action;

	action=&iface.action_display_list.action_displays[action_idx];

	list_palette_set_title(&alt_property_palette,"Actions",NULL,"Action",action->display_name,NULL,NULL);

		// settings

	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_string(&alt_property_palette,kActionSettingsName,"Name",action->display_name,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kActionSettingsShow,"Show",&action->show,FALSE);
}

/* =======================================================

      Property Palette Click Action
      
======================================================= */

void alt_property_palette_click_action(int action_idx,int id,bool double_click)
{
	iface_action_display_type		*action;

	if (!double_click) return;

	action=&iface.action_display_list.action_displays[action_idx];

	switch (id) {

		case kActionSettingsName:
			dialog_property_string_run(list_string_value_string,(void*)action->display_name,name_str_len,0,0);
			break;

	}
}

