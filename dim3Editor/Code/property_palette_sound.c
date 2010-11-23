/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Sound

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "view.h"
#include "dialog.h"

#define kSoundPropertyOn					0
#define kSoundPropertyName					1
#define kSoundPropertyPitch					2

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Sound
      
======================================================= */

void property_palette_fill_sound(int sound_idx)
{
	map_sound_type		*sound;

	sound=&map.sounds[sound_idx];

	list_palette_add_header(&property_palette,0,"Sound Settings");
	list_palette_add_checkbox(&property_palette,kSoundPropertyOn,"On",sound->on);

	list_palette_add_header(&property_palette,0,"Sound Audio");
	list_palette_add_string(&property_palette,kSoundPropertyName,"Sound",sound->name);
	list_palette_add_string_float(&property_palette,kSoundPropertyPitch,"Pitch",sound->pitch);
}

/* =======================================================

      Property Palette Click Sound
      
======================================================= */

void property_palette_click_sound(int sound_idx,int id)
{
	map_sound_type		*sound;

	sound=&map.sounds[sound_idx];

	switch (id) {

		case kSoundPropertyOn:
			sound->on=!sound->on;
			break;

		case kSoundPropertyName:
		case kSoundPropertyPitch:
			break;

	}

	main_wind_draw();
}

