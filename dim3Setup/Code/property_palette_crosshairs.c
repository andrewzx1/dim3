/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Crosshairs

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

#define kCrosshairPropertyAdd					0

#define kCrosshairProperyName					1000
#define kCrosshairProperyDelete					2000

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Crosshairs
      
======================================================= */

void property_palette_fill_crosshairs(void)
{
	int						n;

	list_palette_add_header_button(&property_palette,kCrosshairPropertyAdd,"Crosshairs",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=iface.crosshair_list.ncrosshair;n++) {
		list_palette_add_string_selectable_button(&property_palette,(kCrosshairProperyName+n),list_button_minus,(kCrosshairProperyDelete+n),iface.crosshair_list.crosshairs[n].name,NULL,(state.cur_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);
}

/* =======================================================

      Property Palette Click Crosshairs
      
======================================================= */

void property_palette_click_crosshairs(int id)
{
		// crosshair edit
		
	if ((id>=kCrosshairProperyName) && (id<kCrosshairProperyDelete)) {
		state.cur_idx=id-kCrosshairProperyName;
		main_wind_draw();
		return;
	}
	
		// crosshair delete
		
	if (id>=kCrosshairProperyDelete) {
		state.cur_idx=-1;
	//	supergumba
		main_wind_draw();
		return;
	}
	
		// crosshair add

	if (id==kCrosshairPropertyAdd) {
	// supergumba
	//	state.cur_idx=... new sound ...
		main_wind_draw();
		return;
	}
}

