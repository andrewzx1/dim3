/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Sounds

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

#define kSoundPropertyAdd						0

#define kSoundProperyName						1000
#define kSoundProperyDelete						2000

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Sounds
      
======================================================= */

void property_palette_fill_sounds(void)
{
	int						n;

	list_palette_add_header_button(&property_palette,kSoundPropertyAdd,"Sounds",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=iface.sound_list.nsound;n++) {
		list_palette_add_string_selectable_button(&property_palette,(kSoundProperyName+n),list_button_minus,(kSoundProperyDelete+n),iface.sound_list.sounds[n].name,NULL,(state.cur_sound_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);
}

/* =======================================================

      Property Palette Click Sounds
      
======================================================= */

void property_palette_click_sounds(int id)
{
		// sound edit
		
	if ((id>=kSoundProperyName) && (id<kSoundProperyDelete)) {
		state.cur_sound_idx=id-kSoundProperyName;
		main_wind_draw();
		return;
	}
	
		// sound delete
		
	if (id>=kSoundProperyDelete) {
		state.cur_sound_idx=-1;
	//	supergumba
		main_wind_draw();
		return;
	}
	
		// sound add

	if (id==kSoundPropertyAdd) {
	// supergumba
	//	state.cur_sound_idx=... new sound ...
		main_wind_draw();
		return;
	}
}

