/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Devices

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

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Devices
      
======================================================= */

void property_palette_fill_devices(void)
{
	int					n;
	char				names[3][16]={"PC Scale","Phone Scale","Pad Scale"};
	iface_device_type	*device;

	list_palette_set_title(&property_palette,"Devices",NULL,NULL,NULL,NULL,NULL);

		// devices

	device=iface.devices;

	for (n=0;n!=max_device;n++) {

		list_palette_add_header(&property_palette,0,names[n]);

		list_palette_add_float(&property_palette,-1,"Control Padding",&device->scale.control_padding,FALSE);
		list_palette_add_float(&property_palette,-1,"Control Height",&device->scale.control_high,FALSE);
		list_palette_add_float(&property_palette,-1,"Control Short Width",&device->scale.control_short_wid,FALSE);
		list_palette_add_float(&property_palette,-1,"Control Long Width",&device->scale.control_long_wid,FALSE);
		list_palette_add_float(&property_palette,-1,"Control Scroll Size",&device->scale.control_scroll_size,FALSE);
		list_palette_add_float(&property_palette,-1,"Button Height",&device->scale.button_high,FALSE);
		list_palette_add_float(&property_palette,-1,"Button Short Width",&device->scale.button_short_wid,FALSE);
		list_palette_add_float(&property_palette,-1,"Button Long Width",&device->scale.button_long_wid,FALSE);
		list_palette_add_float(&property_palette,-1,"Tab Margin",&device->scale.tab_margin,FALSE);
		list_palette_add_float(&property_palette,-1,"Tab Height",&device->scale.tab_high,FALSE);
		list_palette_add_float(&property_palette,-1,"Camera Chase 1.6 Ratio Adjust",&device->scale.camera_chase_width_adjust,FALSE);
		list_palette_add_float(&property_palette,-1,"Weapon Height 1.6 Ratio Adjust",&device->scale.weapon_high_adjust,FALSE);

		device++;
	}
}

/* =======================================================

      Property Palette Click Devices
      
======================================================= */

void property_palette_click_devices(bool double_click)
{
}

