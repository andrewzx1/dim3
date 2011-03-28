/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Halos

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

#define kHaloPropertyAdd						0

#define kHaloProperyName						1000
#define kHaloProperyDelete						2000

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Halos
      
======================================================= */

void property_palette_fill_halos(void)
{
	int						n;

	list_palette_add_header_button(&property_palette,kHaloPropertyAdd,"Halos",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=iface.halo_list.nhalo;n++) {
		list_palette_add_string_selectable_button(&property_palette,(kHaloProperyName+n),list_button_minus,(kHaloProperyDelete+n),iface.halo_list.halos[n].name,NULL,(state.cur_halo_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);
}

/* =======================================================

      Property Palette Click Halos
      
======================================================= */

void property_palette_click_halos(int id)
{
		// halo edit
		
	if ((id>=kHaloProperyName) && (id<kHaloProperyDelete)) {
		state.cur_halo_idx=id-kHaloProperyName;
		main_wind_draw();
		return;
	}
	
		// halo delete
		
	if (id>=kHaloProperyDelete) {
		state.cur_halo_idx=-1;
	//	supergumba
		main_wind_draw();
		return;
	}
	
		// halo add

	if (id==kHaloPropertyAdd) {
	// supergumba
	//	state.cur_halo_idx=... new sound ...
		main_wind_draw();
		return;
	}
}

