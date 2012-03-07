/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Settings

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3setup.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kSettingsProjectName					0
#define kSettingsProjectModernize				1
#define kSettingsProjectSkill					2

#define kSettingsScaleX							5
#define kSettingsScaleY							6

#define kSettingsInterfaceFont					10
#define kSettingsHUDFont						20

#define kSettingsClickSound						30

#define kSettingsColorBackground				40
#define kSettingsColorDefaultTint				41

#define kSettingsColorMenuBackground			50
#define kSettingsColorMenuText					51
#define kSettingsColorMenuMouseOver				52
#define kSettingsColorMenuOutline				53

#define kSettingsColorDialogBackground			60
#define kSettingsColorDialogHeader				61
#define kSettingsColorDialogOutline				62
#define kSettingsColorDialogTitle				63

#define kSettingsColorTabBackground				70
#define kSettingsColorTabDimmed					71
#define kSettingsColorTabOutline				72
#define kSettingsColorTabText					73
#define kSettingsColorTabTextMouseOver			74
#define kSettingsColorTabTextDimmed				75

#define kSettingsColorControlLabel				80
#define kSettingsColorControlText				81
#define kSettingsColorControlFill				82
#define kSettingsColorControlHeader				83
#define kSettingsColorControlOutline			84
#define kSettingsColorControlMouseOver			85
#define kSettingsColorControlHilite				86
#define kSettingsColorControlDisabled			87

#define kSettingsColorScrollBarBackground		90
#define kSettingsColorScrollBarThumb			91

#define kSettingsColorButtonFill				100
#define kSettingsColorButtonText				101
#define kSettingsColorButtonOutline				102

#define kSettingColorSystemMetric				110

#define kSettingsFaseTitleMilliseconds			120
#define kSettingsFaseMapMilliseconds			121

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Settings
      
======================================================= */

void property_palette_fill_settings(void)
{
	int					n;
	char				name[256];

	list_palette_set_title(&property_palette,"Project Settings",NULL,NULL,NULL,NULL,NULL);

		// project
		
	list_palette_add_header(&property_palette,0,"Project");
	list_palette_add_string(&property_palette,kSettingsProjectName,"Name",iface.project.name,FALSE);
	list_palette_add_checkbox(&property_palette,kSettingsProjectModernize,"Modernize",&iface.project.modernize,FALSE);
	list_palette_add_checkbox(&property_palette,kSettingsProjectSkill,"Skill",&iface.project.skill,FALSE);

		// scale
		
	list_palette_add_header(&property_palette,0,"Scale");
	list_palette_add_int(&property_palette,kSettingsScaleX,"X",&iface.scale_x,FALSE);
	list_palette_add_int(&property_palette,kSettingsScaleY,"Y",&iface.scale_y,FALSE);

		// fonts
		
	list_palette_add_header(&property_palette,0,"Font");
	for (n=0;n!=max_iface_font_variant;n++) {
		sprintf(name,"Interface Font %d",n);
		list_palette_add_string(&property_palette,(kSettingsInterfaceFont+n),name,iface.font.interface_name[n],FALSE);
	}
	for (n=0;n!=max_iface_font_variant;n++) {
		sprintf(name,"HUD Font %d",n);
		list_palette_add_string(&property_palette,(kSettingsHUDFont+n),name,iface.font.hud_name[n],FALSE);
	}

		// sound
		
	list_palette_add_header(&property_palette,0,"Sound");
	list_palette_add_string(&property_palette,kSettingsClickSound,"Click",iface.click_sound,FALSE);

		// color
		
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

		// scrollbar color
		
	list_palette_add_header(&property_palette,0,"Scrollbar Colors");
	list_palette_add_pick_color(&property_palette,kSettingsColorScrollBarBackground,"ScrollBar Background",&iface.color.scrollbar.background,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorScrollBarThumb,"ScrollBar Thumb",&iface.color.scrollbar.thumb,FALSE);
	
		// button color
		
	list_palette_add_header(&property_palette,0,"Button Colors");
	list_palette_add_pick_color(&property_palette,kSettingsColorButtonFill,"Button Fill",&iface.color.button.fill,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorButtonText,"Button Text",&iface.color.button.text,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorButtonOutline,"Button Outline",&iface.color.button.outline,FALSE);

		// system color
		
	list_palette_add_header(&property_palette,0,"System Colors");
	list_palette_add_pick_color(&property_palette,kSettingColorSystemMetric,"Metrics",&iface.color.system.metric,FALSE);
	
		// fades
		
	list_palette_add_header(&property_palette,0,"Fades");
	list_palette_add_int(&property_palette,kSettingsFaseTitleMilliseconds,"Title Milliseconds",&iface.fade.title_msec,FALSE);
	list_palette_add_int(&property_palette,kSettingsFaseMapMilliseconds,"Map Milliseconds",&iface.fade.map_msec,FALSE);
}

/* =======================================================

      Property Palette Click Settings
      
======================================================= */

void property_palette_click_settings(int id,bool double_click)
{
	if (!double_click) return;

		// fonts

	if ((id>=kSettingsInterfaceFont) && (id<(kSettingsInterfaceFont+max_iface_font_variant))) {
		dialog_property_string_run(list_string_value_string,(void*)iface.font.interface_name[id-kSettingsInterfaceFont],name_str_len,0,0);
		return;
	}

	if ((id>=kSettingsHUDFont) && (id<(kSettingsHUDFont+max_iface_font_variant))) {
		dialog_property_string_run(list_string_value_string,(void*)iface.font.hud_name[id-kSettingsHUDFont],name_str_len,0,0);
		return;
	}

		// regular items

	switch (id) {
	
			// project

		case kSettingsProjectName:
			dialog_property_string_run(list_string_value_string,(void*)iface.project.name,name_str_len,0,0);
			break;

			// sound
			
		case kSettingsClickSound:
			property_palette_pick_sound(iface.click_sound,FALSE);
			break;			

	}
}

