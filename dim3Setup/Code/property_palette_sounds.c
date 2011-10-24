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

	list_palette_set_title(&property_palette,"Sounds");

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

void property_palette_click_sounds(int id,bool double_click)
{
	int					idx,sz;

		// sound edit
		
	if ((id>=kSoundProperyName) && (id<(kSoundProperyName+max_iface_sound))) {
		state.cur_sound_idx=id-kSoundProperyName;
		if (double_click) list_palette_set_level(2);
		return;
	}
	
		// sound delete
		
	if ((id>=kSoundProperyDelete) && (id<(kSoundProperyDelete+max_iface_sound))) {
		state.cur_sound_idx=-1;

		idx=id-kSoundProperyDelete;

		sz=(iface.sound_list.nsound-idx)-1;
		if (sz>0) memmove(&iface.sound_list.sounds[idx],&iface.sound_list.sounds[idx+1],(sz*sizeof(iface_sound_type)));

		iface.sound_list.nsound--;

		return;
	}
	
		// sound add

	if (id==kSoundPropertyAdd) {
		state.cur_sound_idx=-1;

		if (iface.sound_list.nsound>=max_iface_sound) {
			os_dialog_alert("Add Sound","Reached the maximum number of sounds");
			return;
		}

		idx=iface.sound_list.nsound;
		iface.sound_list.nsound++;
		
		iface.sound_list.sounds[idx].name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)iface.sound_list.sounds[idx].name,name_str_len,0,0);
	
		iface.sound_list.sounds[idx].file_name[0]=0x0;

		iface.sound_list.sounds[idx].min_dist=1500;
		iface.sound_list.sounds[idx].max_dist=30000;

		state.cur_sound_idx=idx;

		return;
	}
}

