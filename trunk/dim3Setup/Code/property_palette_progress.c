/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Progress

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

#define kSettingsProgressLeft					0
#define kSettingsProgressRight					1
#define kSettingsProgressTop					2
#define kSettingsProgressBottom					3
#define kSettingsProgressOutline				4
#define kSettingsProgressOverlay				5
#define kSettingsProgressBaseColorStart			6
#define kSettingsProgressBaseColorEnd			7
#define kSettingsProgressHiliteColorStart		8
#define kSettingsProgressHiliteColorEnd			9
#define kSettingsProgressOutlineColor			10

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Settings
      
======================================================= */

void property_palette_fill_progress(void)
{
	list_palette_set_title(&property_palette,"Project Progress");

		// progress
		
	list_palette_add_header(&property_palette,0,"Position");
	list_palette_add_int(&property_palette,kSettingsProgressLeft,"Left X",&iface.progress.lx,FALSE);
	list_palette_add_int(&property_palette,kSettingsProgressRight,"Right X",&iface.progress.rx,FALSE);
	list_palette_add_int(&property_palette,kSettingsProgressTop,"Top Y",&iface.progress.ty,FALSE);
	list_palette_add_int(&property_palette,kSettingsProgressBottom,"Bottom Y",&iface.progress.by,FALSE);

	list_palette_add_header(&property_palette,0,"Display");
	list_palette_add_checkbox(&property_palette,kSettingsProgressOutline,"Outline",&iface.progress.outline,FALSE);
	list_palette_add_checkbox(&property_palette,kSettingsProgressOverlay,"Overlay",&iface.progress.overlay,FALSE);

	list_palette_add_header(&property_palette,0,"Colors");
	list_palette_add_pick_color(&property_palette,kSettingsProgressBaseColorStart,"Base Color Start",&iface.progress.base_color_start,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsProgressBaseColorEnd,"Base Color End",&iface.progress.base_color_end,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsProgressHiliteColorStart,"Highlight Color Start",&iface.progress.hilite_color_start,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsProgressHiliteColorEnd,"Highlight Color End",&iface.progress.hilite_color_end,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsProgressOutlineColor,"Outline Color",&iface.progress.outline_color,FALSE);
}

/* =======================================================

      Property Palette Click Progress
      
======================================================= */

void property_palette_click_progress(int id,bool double_click)
{
}

