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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

#define kSettingsProgressLeft					0
#define kSettingsProgressRight					1
#define kSettingsProgressTop					2
#define kSettingsProgressBottom					3
#define kSettingsProgressOutline				4
#define kSettingsProgressOverlay				5
#define kSettingsProgressBackgroundColor		6
#define kSettingsProgressHiliteColor			7
#define kSettingsProgressOutlineColor			8

extern iface_type				iface;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Settings
      
======================================================= */

void project_palette_fill_progress(void)
{
	list_palette_set_title(&project_palette,"Project Progress",NULL,NULL,NULL,NULL,NULL);

		// progress
		
	list_palette_add_header(&project_palette,0,"Position");
	list_palette_add_int(&project_palette,kSettingsProgressLeft,"Left X",&iface.progress.lx,FALSE);
	list_palette_add_int(&project_palette,kSettingsProgressRight,"Right X",&iface.progress.rx,FALSE);
	list_palette_add_int(&project_palette,kSettingsProgressTop,"Top Y",&iface.progress.ty,FALSE);
	list_palette_add_int(&project_palette,kSettingsProgressBottom,"Bottom Y",&iface.progress.by,FALSE);

	list_palette_add_header(&project_palette,0,"Display");
	list_palette_add_checkbox(&project_palette,kSettingsProgressOutline,"Outline",&iface.progress.outline,FALSE);
	list_palette_add_checkbox(&project_palette,kSettingsProgressOverlay,"Overlay",&iface.progress.overlay,FALSE);

	list_palette_add_header(&project_palette,0,"Colors");
	list_palette_add_pick_color(&project_palette,kSettingsProgressBackgroundColor,"Background",&iface.progress.background_color,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsProgressHiliteColor,"Highlight Color",&iface.progress.hilite_color,FALSE);
	list_palette_add_pick_color(&project_palette,kSettingsProgressOutlineColor,"Outline Color",&iface.progress.outline_color,FALSE);
}

/* =======================================================

      Property Palette Click Progress
      
======================================================= */

void project_palette_click_progress(bool double_click)
{
}

