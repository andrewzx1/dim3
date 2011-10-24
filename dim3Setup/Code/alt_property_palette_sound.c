/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette Sound

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

#define kSoundSettingsName						0
#define kSoundSettingsFileName					1
#define kSoundSettingsMinDist					2
#define kSoundSettingsMaxDist					3

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

/* =======================================================

      Property Palette Fill Sound
      
======================================================= */

void alt_property_palette_fill_sound(int sound_idx)
{
	iface_sound_type			*sound;

	sound=&iface.sound_list.sounds[sound_idx];

	list_palette_set_sub_title(&alt_property_palette,"Sound",sound->name);

		// settings

	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_string(&alt_property_palette,kSoundSettingsName,"Name",sound->name,FALSE);
	list_palette_add_string(&alt_property_palette,kSoundSettingsFileName,"File",sound->file_name,FALSE);
	list_palette_add_string_int(&alt_property_palette,kSoundSettingsMinDist,"Min Distance",sound->min_dist,FALSE);
	list_palette_add_string_int(&alt_property_palette,kSoundSettingsMaxDist,"Max Distance",sound->max_dist,FALSE);
}

/* =======================================================

      Property Palette Click Sound
      
======================================================= */

void alt_property_palette_click_sound(int sound_idx,int id,bool double_click)
{
	iface_sound_type			*sound;

	if (!double_click) return;

	sound=&iface.sound_list.sounds[sound_idx];

	switch (id) {

			// settings

		case kSoundSettingsName:
			dialog_property_string_run(list_string_value_string,(void*)sound->name,name_str_len,0,0);
			break;

		case kSoundSettingsFileName:
			property_pick_file("Pick a Sound","Sounds","wav",NULL,sound->file_name);
			break;

		case kSoundSettingsMinDist:
			dialog_property_string_run(list_string_value_positive_int,(void*)&sound->min_dist,0,0,0);
			break;

		case kSoundSettingsMaxDist:
			dialog_property_string_run(list_string_value_positive_int,(void*)&sound->max_dist,0,0,0);
			break;

	}
}

