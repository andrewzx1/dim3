/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette HUD

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

#define kIntroPropertyBitmapAdd					1000
#define kIntroPropertyBitmapName				2000
#define kIntroPropertyBitmapDelete				3000

#define kIntroPropertyTextAdd					4000
#define kIntroPropertyTextName					5000
#define kIntroPropertyTextDelete				6000

#define kIntroPropertyBarAdd					7000
#define kIntroPropertyBarName					8000
#define kIntroPropertyBarDelete					9000

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill HUD
      
======================================================= */

void property_palette_fill_hud(void)
{
	int				n;

		// bitmaps

	list_palette_add_header_button(&property_palette,kIntroPropertyBitmapAdd,"Bitmaps",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
		list_palette_add_string_selectable_button(&property_palette,(kIntroPropertyBitmapName+n),list_button_minus,(kIntroPropertyBitmapDelete+n),iface.bitmap_list.bitmaps[n].name,NULL,(state.cur_hud_bitmap_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);
}

/* =======================================================

      Property Palette Click HUD
      
======================================================= */

void property_palette_click_hud(int id)
{
	int					idx,sz;
	char				model_name[name_str_len];
/*
		// select button

	if ((id>=kIntroPropertyButtonGameNew) && (id<=kIntroPropertyButtonQuit)) {
		state.cur_intro_button_idx=(id-kIntroPropertyButtonGameNew)+item_intro_button_game_new;
		state.cur_intro_model_idx=-1;
		main_wind_draw();
		return;
	}

		// select save state button

	if ((id>=kIntroPropertyButtonSimpleSaveStart) && (id<(kIntroPropertyButtonSimpleSaveStart+max_simple_save_spot))) {
		state.cur_intro_button_idx=(id-kIntroPropertyButtonSimpleSaveStart)+item_intro_button_simple_save_start;
		state.cur_intro_model_idx=-1;
		main_wind_draw();
		return;
	}

	if ((id>=kIntroPropertyButtonSimpleSaveErase) && (id<(kIntroPropertyButtonSimpleSaveErase+max_simple_save_spot))) {
		state.cur_intro_button_idx=(id-kIntroPropertyButtonSimpleSaveErase)+item_intro_button_simple_save_erase;
		state.cur_intro_model_idx=-1;
		main_wind_draw();
		return;
	}

		// select model

	if ((id>=kIntroPropertyModelName) && (id<kIntroPropertyModelDelete)) {
		state.cur_intro_button_idx=-1;
		state.cur_intro_model_idx=id-kIntroPropertyModelName;
		main_wind_draw();
		return;
	}

		// delete model

	if (id>=kIntroPropertyModelDelete) {
		state.cur_intro_button_idx=-1;
		state.cur_intro_model_idx=-1;

		idx=id-kIntroPropertyModelDelete;

		sz=(iface.intro.model_list.nmodel-idx)-1;
		if (sz>0) memmove(&iface.intro.model_list.models[idx],&iface.intro.model_list.models[idx+1],(sz*sizeof(iface_intro_model_type)));

		iface.intro.model_list.nmodel--;

		main_wind_draw();
		return;
	}

		// add model

	if (id==kIntroPropertyModelAdd) {
		state.cur_intro_button_idx=-1;
		state.cur_intro_model_idx=-1;

		if (iface.intro.model_list.nmodel>=max_hud_intro_model) {
			os_dialog_alert("Reached the maximum number of models","Add Intro Model");
			return;
		}
		
		model_name[0]=0x0;
		if (!dialog_file_open_run("Pick a Model","Models",NULL,"Mesh.xml",model_name)) return;

		idx=iface.intro.model_list.nmodel;
		iface.intro.model_list.nmodel++;

		iface.intro.model_list.models[idx].x=0;
		iface.intro.model_list.models[idx].y=0;
		iface.intro.model_list.models[idx].resize=1.0f;
		iface.intro.model_list.models[idx].rot.x=0.0f;
		iface.intro.model_list.models[idx].rot.y=0.0f;
		iface.intro.model_list.models[idx].rot.z=0.0f;
		strcpy(iface.intro.model_list.models[idx].model_name,model_name);
		iface.intro.model_list.models[idx].animate_name[0]=0x0;

		state.cur_intro_model_idx=idx;

		main_wind_draw();
		return;
	}

		// regular picks, always
		// disable selection

	state.cur_intro_button_idx=-1;
	state.cur_intro_model_idx=-1;

	switch (id) {

		case kIntroPropertyMusic:
			dialog_property_string_run(list_string_value_string,(void*)iface.intro.music,name_str_len,0,0);
			break;

	}
*/
		// redraw

	main_wind_draw();
}

