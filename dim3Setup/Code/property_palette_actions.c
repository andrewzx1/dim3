/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Actions

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

#define kActionPropertyAdd						0
#define kActionProperyName						1000

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

char							control_name_str[][32]=control_names;

/* =======================================================

      Property Palette Fill Actions
      
======================================================= */

void property_palette_fill_actions(void)
{
	int						n;

	list_palette_add_header(&property_palette,0,"Actions");
	
	for (n=0;n!=ncontrol;n++) {
		list_palette_add_string_selectable(&property_palette,(kActionProperyName+n),control_name_str[n],NULL,(state.cur_action_idx==n),FALSE);
	}
}

/* =======================================================

      Property Palette Click Actions
      
======================================================= */

void property_palette_click_actions(int id)
{
		// edit action
		
	state.cur_action_idx=id-kActionProperyName;
	list_palette_set_level(2);
	main_wind_draw();
}

