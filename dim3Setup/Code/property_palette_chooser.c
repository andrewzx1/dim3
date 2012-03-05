/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Chooser

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

#define kChooserPropertyChooserAdd				0

#define kChooserPropertyChooserName				1000
#define kChooserPropertyChooserDelete			2000

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Chooser
      
======================================================= */

void property_palette_fill_chooser(void)
{
	int				n;

	list_palette_set_title(&property_palette,"Choosers",NULL,NULL,NULL,NULL,NULL);

	list_palette_add_header_button(&property_palette,kChooserPropertyChooserAdd,"Choosers",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=iface.chooser_list.nchooser;n++) {
		list_palette_add_string_selectable_button(&property_palette,(kChooserPropertyChooserName+n),list_button_minus,(kChooserPropertyChooserDelete+n),iface.chooser_list.choosers[n].name,NULL,(state.cur_chooser_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);
}

/* =======================================================

      Property Palette Click Chooser
      
======================================================= */

void property_palette_click_chooser(int id,bool double_click)
{
	int					idx,sz;

		// select chooser

	if ((id>=kChooserPropertyChooserName) && (id<(kChooserPropertyChooserName+max_iface_chooser))) {
		state.cur_chooser_idx=id-kChooserPropertyChooserName;
		state.cur_chooser_piece_idx=-1;
		if (double_click) list_palette_set_level(&property_palette,2);
		return;
	}

		// delete chooser

	if ((id>=kChooserPropertyChooserDelete) && (id<(kChooserPropertyChooserDelete+max_iface_chooser))) {
		state.cur_chooser_idx=-1;
		state.cur_chooser_piece_idx=-1;

		idx=id-kChooserPropertyChooserDelete;

		sz=(iface.chooser_list.nchooser-idx)-1;
		if (sz>0) memmove(&iface.chooser_list.choosers[idx],&iface.chooser_list.choosers[idx+1],(sz*sizeof(iface_chooser_type)));

		iface.chooser_list.nchooser--;

		return;
	}

		// add chooser

	if (id==kChooserPropertyChooserAdd) {
		state.cur_chooser_idx=-1;
		state.cur_chooser_piece_idx=-1;

		if (iface.chooser_list.nchooser>=max_iface_chooser) {
			os_dialog_alert("Add Chooser","Reached the maximum number of choosers");
			return;
		}

		idx=iface.chooser_list.nchooser;
		iface.chooser_list.nchooser++;
		
		iface.chooser_list.choosers[idx].name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)iface.chooser_list.choosers[idx].name,name_str_len,0,0);
	
		iface.chooser_list.choosers[idx].template_name[0]=0x0;

		iface.chooser_list.choosers[idx].frame.x=50;
		iface.chooser_list.choosers[idx].frame.y=50;
		iface.chooser_list.choosers[idx].frame.wid=800;
		iface.chooser_list.choosers[idx].frame.high=600;
		strcpy(iface.chooser_list.choosers[idx].frame.title,"Chooser");
		iface.chooser_list.choosers[idx].frame.on=TRUE;

		iface.chooser_list.choosers[idx].key.ok_id=-1;
		iface.chooser_list.choosers[idx].key.cancel_id=-1;

		iface.chooser_list.choosers[idx].npiece=0;

		state.cur_chooser_idx=idx;

		return;
	}
}

