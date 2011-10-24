/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Halos

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

	list_palette_set_title(&property_palette,"Halos");

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

void property_palette_click_halos(int id,bool double_click)
{
	int				idx,sz;

		// halo edit
		
	if ((id>=kHaloProperyName) && (id<(kHaloProperyName+max_iface_halo))) {
		state.cur_halo_idx=id-kHaloProperyName;
		if (double_click) list_palette_set_level(2);
		return;
	}
	
		// halo delete
		
	if ((id>=kHaloProperyDelete) && (id<(kHaloProperyDelete+max_iface_halo))) {
		state.cur_halo_idx=-1;

		idx=id-kHaloProperyDelete;

		sz=(iface.halo_list.nhalo-idx)-1;
		if (sz>0) memmove(&iface.halo_list.halos[idx],&iface.halo_list.halos[idx+1],(sz*sizeof(iface_halo_type)));

		iface.halo_list.nhalo--;

		return;
	}
	
		// halo add

	if (id==kHaloPropertyAdd) {
		state.cur_halo_idx=-1;

		if (iface.halo_list.nhalo>=max_iface_halo) {
			os_dialog_alert("Add Halo","Reached the maximum number of halos");
			return;
		}

		idx=iface.halo_list.nhalo;
		iface.halo_list.nhalo++;
		
		iface.halo_list.halos[idx].name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)iface.halo_list.halos[idx].name,name_str_len,0,0);
	
		iface.halo_list.halos[idx].bitmap_name[0]=0x0;

		iface.halo_list.halos[idx].min_dist=3000;
		iface.halo_list.halos[idx].max_dist=50000;
		iface.halo_list.halos[idx].min_size=500;
		iface.halo_list.halos[idx].max_size=200;
		iface.halo_list.halos[idx].min_alpha=0.8f;
		iface.halo_list.halos[idx].max_alpha=0.05f;
		iface.halo_list.halos[idx].no_clip_object=FALSE;
		iface.halo_list.halos[idx].no_clip_self=TRUE;

		state.cur_halo_idx=idx;

		return;
	}
}

