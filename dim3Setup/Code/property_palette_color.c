/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Color

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
	#include "dim3setup.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kSettingsColorBackground				0
#define kSettingsColorDefaultTint				1

#define kSettingsColorMenuBackground			10
#define kSettingsColorMenuText					11
#define kSettingsColorMenuMouseOver				12
#define kSettingsColorMenuOutline				13

#define kSettingsColorDialogBackground			20
#define kSettingsColorDialogHeader				21
#define kSettingsColorDialogOutline				22
#define kSettingsColorDialogTitle				23

#define kSettingsColorTabBackground				30
#define kSettingsColorTabDimmed					31
#define kSettingsColorTabOutline				32
#define kSettingsColorTabText					33
#define kSettingsColorTabTextMouseOver			34
#define kSettingsColorTabTextDimmed				35

#define kSettingsColorControlLabel				40
#define kSettingsColorControlText				41
#define kSettingsColorControlFill				42
#define kSettingsColorControlHeader				43
#define kSettingsColorControlOutline			44
#define kSettingsColorControlMouseOver			45
#define kSettingsColorControlHilite				46
#define kSettingsColorControlDisabled			47

#define kSettingsColorScrollBarBackground		50
#define kSettingsColorScrollBarThumb			51

#define kSettingsColorButtonFill				60
#define kSettingsColorButtonText				61
#define kSettingsColorButtonOutline				62

#define kSettingColorSystemMetric				70

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Color
      
======================================================= */

void property_palette_fill_color(void)
{
	list_palette_set_title(&property_palette,"Color",NULL,NULL,NULL,NULL,NULL);

		// main color
		
	list_palette_add_header(&property_palette,0,"Main Colors");
	list_palette_add_pick_color(&property_palette,kSettingsColorBackground,"Background",&iface.color.background,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorDefaultTint,"Default Tint",&iface.color.default_tint,FALSE);
	
		// menu color
		
	list_palette_add_header(&property_palette,0,"Menu Colors");
	list_palette_add_pick_color(&property_palette,kSettingsColorMenuBackground,"Background",&iface.color.menu.background,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorMenuText,"Text",&iface.color.menu.text,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorMenuMouseOver,"Mouse Over",&iface.color.menu.mouse_over,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorMenuOutline,"Outline",&iface.color.menu.outline,FALSE);

		// dialog color
		
	list_palette_add_header(&property_palette,0,"Dialog Colors");
	list_palette_add_pick_color(&property_palette,kSettingsColorDialogBackground,"Dialog Background",&iface.color.dialog.background,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorDialogHeader,"Dialog Header",&iface.color.dialog.header,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorDialogOutline,"Dialog Outline",&iface.color.dialog.outline,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorDialogTitle,"Dialog Title",&iface.color.dialog.title,FALSE);
	
		// tab color
		
	list_palette_add_header(&property_palette,0,"Tab Colors");
	list_palette_add_pick_color(&property_palette,kSettingsColorTabBackground,"Tab Background",&iface.color.tab.background,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorTabDimmed,"Tab Dimmed",&iface.color.tab.dimmed,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorTabOutline,"Tab Outline",&iface.color.tab.outline,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorTabText,"Tab Text",&iface.color.tab.text,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorTabTextMouseOver,"Tab Text Mouse Over",&iface.color.tab.text_mouse_over,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorTabTextDimmed,"Tab Text Dimmed",&iface.color.tab.text_dimmed,FALSE);
	
		// control color
		
	list_palette_add_header(&property_palette,0,"Control Colors");
	list_palette_add_pick_color(&property_palette,kSettingsColorControlLabel,"Control Label",&iface.color.control.label,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorControlText,"Control Text",&iface.color.control.text,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorControlFill,"Control Fill",&iface.color.control.fill,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorControlHeader,"Control Header",&iface.color.control.header,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorControlOutline,"Control Outline",&iface.color.control.outline,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorControlMouseOver,"Control Mouse Over",&iface.color.control.mouse_over,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorControlHilite,"Control Highlight",&iface.color.control.hilite,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorControlDisabled,"Control Disabled",&iface.color.control.disabled,FALSE);
	
		// button color
		
	list_palette_add_header(&property_palette,0,"Button Colors");
	list_palette_add_pick_color(&property_palette,kSettingsColorButtonFill,"Button Fill",&iface.color.button.fill,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorButtonText,"Button Text",&iface.color.button.text,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorButtonOutline,"Button Outline",&iface.color.button.outline,FALSE);

		// system color
		
	list_palette_add_header(&property_palette,0,"System Colors");
	list_palette_add_pick_color(&property_palette,kSettingColorSystemMetric,"Metrics",&iface.color.system.metric,FALSE);
}

/* =======================================================

      Property Palette Click Color
      
======================================================= */

void property_palette_click_color(bool double_click)
{
}

