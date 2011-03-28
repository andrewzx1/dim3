/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Intro

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

#define kIntroPropertyMusic						0

#define kIntroPropertyButtonGameNew				10
#define kIntroPropertyButtonGameLoad			11
#define kIntroPropertyButtonGameSetup			12
#define kIntroPropertyButtonGameNewEasy			13
#define kIntroPropertyButtonGameNewMedium		14
#define kIntroPropertyButtonGameNewHard			15
#define kIntroPropertyButtonGameNewCancel		16
#define kIntroPropertyButtonMultiplayerHost		17
#define kIntroPropertyButtonMultiplayerJoin		18
#define kIntroPropertyButtonMultiplayerSetup	19
#define kIntroPropertyButtonCredit				20
#define kIntroPropertyButtonQuit				21

#define kIntroPropertyButtonSimpleSaveStart		50
#define kIntroPropertyButtonSimpleSaveErase		60

#define kIntroPropertyModelAdd					100
#define kIntroPropertyModelName					1000
#define kIntroPropertyModelDelete				2000

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Intro
      
======================================================= */

void property_palette_fill_intro(void)
{
	int				n;
	char			str[256];

		// options

	list_palette_add_header(&property_palette,0,"Options");
	list_palette_add_string(&property_palette,kIntroPropertyMusic,"Music",iface.intro.music,FALSE);

		// buttons

	list_palette_add_header(&property_palette,0,"Regular Buttons");
	list_palette_add_item(&property_palette,kIntroPropertyButtonGameNew,0,"Game New",(state.cur_intro_button_idx==kIntroPropertyButtonGameNew),FALSE);
	list_palette_add_item(&property_palette,kIntroPropertyButtonGameLoad,0,"Game Load",(state.cur_intro_button_idx==kIntroPropertyButtonGameLoad),FALSE);
	list_palette_add_item(&property_palette,kIntroPropertyButtonGameSetup,0,"Game Setup",(state.cur_intro_button_idx==kIntroPropertyButtonGameSetup),FALSE);
	list_palette_add_item(&property_palette,kIntroPropertyButtonGameNewEasy,0,"Game New Easy",(state.cur_intro_button_idx==kIntroPropertyButtonGameNewEasy),FALSE);
	list_palette_add_item(&property_palette,kIntroPropertyButtonGameNewMedium,0,"Game New Medium",(state.cur_intro_button_idx==kIntroPropertyButtonGameNewMedium),FALSE);
	list_palette_add_item(&property_palette,kIntroPropertyButtonGameNewHard,0,"Game New Hard",(state.cur_intro_button_idx==kIntroPropertyButtonGameNewHard),FALSE);
	list_palette_add_item(&property_palette,kIntroPropertyButtonGameNewCancel,0,"Game New Cancel",(state.cur_intro_button_idx==kIntroPropertyButtonGameNewCancel),FALSE);
	list_palette_add_item(&property_palette,kIntroPropertyButtonMultiplayerHost,0,"Multiplayer Host",(state.cur_intro_button_idx==kIntroPropertyButtonMultiplayerHost),FALSE);
	list_palette_add_item(&property_palette,kIntroPropertyButtonMultiplayerJoin,0,"Multiplayer Join",(state.cur_intro_button_idx==kIntroPropertyButtonMultiplayerJoin),FALSE);
	list_palette_add_item(&property_palette,kIntroPropertyButtonMultiplayerSetup,0,"Multiplayer Setup",(state.cur_intro_button_idx==kIntroPropertyButtonMultiplayerSetup),FALSE);
	list_palette_add_item(&property_palette,kIntroPropertyButtonCredit,0,"Credit",(state.cur_intro_button_idx==kIntroPropertyButtonCredit),FALSE);
	list_palette_add_item(&property_palette,kIntroPropertyButtonQuit,0,"Quit",(state.cur_intro_button_idx==kIntroPropertyButtonQuit),FALSE);

	list_palette_add_header(&property_palette,0,"Simple Save Buttons");
	for (n=0;n!=max_simple_save_spot;n++) {
		sprintf(str,"Simple Save Start %d",n);
		list_palette_add_item(&property_palette,(kIntroPropertyButtonSimpleSaveStart+n),0,str,(state.cur_intro_button_idx==(kIntroPropertyButtonSimpleSaveStart+n)),FALSE);
		sprintf(str,"Simple Save Erase %d",n);
		list_palette_add_item(&property_palette,(kIntroPropertyButtonSimpleSaveErase+n),0,str,(state.cur_intro_button_idx==(kIntroPropertyButtonSimpleSaveErase+n)),FALSE);
	}

		// models

	list_palette_add_header_button(&property_palette,kIntroPropertyModelAdd,"Models",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=iface.intro.model_list.nmodel;n++) {
		list_palette_add_string_selectable_button(&property_palette,(kIntroPropertyModelName+n),list_button_minus,(kIntroPropertyModelDelete+n),iface.intro.model_list.models[n].model_name,NULL,(state.cur_intro_model_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);
}

/* =======================================================

      Property Palette Click Intro
      
======================================================= */

void property_palette_click_intro(int id)
{
	/*
		// crosshair edit
		
	if ((id>=kCrosshairProperyName) && (id<kCrosshairProperyDelete)) {
		state.cur_intro_model_idx=id-kCrosshairProperyName;
		main_wind_draw();
		return;
	}
	
		// crosshair delete
		
	if (id>=kCrosshairProperyDelete) {
		state.cur_intro_model_idx=-1;
	//	supergumba
		main_wind_draw();
		return;
	}
	
		// crosshair add

	if (id==kCrosshairPropertyAdd) {
	// supergumba
	//	state.cur_intro_model_idx=... new sound ...
		main_wind_draw();
		return;
	}
	*/
}

