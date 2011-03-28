/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Marks

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

#define kMarkPropertyAdd						0

#define kMarkProperyName						1000
#define kMarkProperyDelete						2000

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Marks
      
======================================================= */

void property_palette_fill_marks(void)
{
	int						n;

	list_palette_add_header_button(&property_palette,kMarkPropertyAdd,"Marks",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=iface.mark_list.nmark;n++) {
		list_palette_add_string_selectable_button(&property_palette,(kMarkProperyName+n),list_button_minus,(kMarkProperyDelete+n),iface.mark_list.marks[n].name,NULL,(state.cur_mark_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);
}

/* =======================================================

      Property Palette Click Marks
      
======================================================= */

void property_palette_click_marks(int id)
{
		// mark edit
		
	if ((id>=kMarkProperyName) && (id<kMarkProperyDelete)) {
		state.cur_mark_idx=id-kMarkProperyName;
		main_wind_draw();
		return;
	}
	
		// mark delete
		
	if (id>=kMarkProperyDelete) {
		state.cur_mark_idx=-1;
	//	supergumba
		main_wind_draw();
		return;
	}
	
		// mark add

	if (id==kMarkPropertyAdd) {
	// supergumba
	//	state.cur_mark_idx=... new sound ...
		main_wind_draw();
		return;
	}
}

