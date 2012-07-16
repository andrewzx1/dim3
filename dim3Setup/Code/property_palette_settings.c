/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Project

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

#define kSettingsProjectName					0
#define kSettingsProjectModernize				1
#define kSettingsProjectNoShaders				2
#define kSettingsProjectSimpleSave				4
#define kSettingsProjectLoadRequiresClick		5
#define kSettingsProjectSkill					6

#define kSettingsScaleX							7
#define kSettingsScaleY							8

#define kSettingsFaseTitleMilliseconds			9
#define kSettingsFaseMapMilliseconds			10

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Project
      
======================================================= */

void property_palette_fill_project(void)
{
	list_palette_set_title(&property_palette,"Project Settings",NULL,NULL,NULL,NULL,NULL);

		// project
		
	list_palette_add_header(&property_palette,0,"Project");
	list_palette_add_string(&property_palette,kSettingsProjectName,"Name",iface.project.name,name_str_len,FALSE);
	list_palette_add_checkbox(&property_palette,kSettingsProjectModernize,"Modernize",&iface.project.modernize,FALSE);
	list_palette_add_checkbox(&property_palette,kSettingsProjectSimpleSave,"Use Simple Saves",&iface.project.simple_save,FALSE);
	list_palette_add_checkbox(&property_palette,kSettingsProjectLoadRequiresClick,"Loads Require Click to Continue",&iface.project.load_requires_click,FALSE);

		// scale
		
	list_palette_add_header(&property_palette,0,"Scale");
	list_palette_add_int(&property_palette,kSettingsScaleX,"X",&iface.scale_x,FALSE);
	list_palette_add_int(&property_palette,kSettingsScaleY,"Y",&iface.scale_y,FALSE);
	
		// fades
		
	list_palette_add_header(&property_palette,0,"Fades");
	list_palette_add_int(&property_palette,kSettingsFaseTitleMilliseconds,"Title Milliseconds",&iface.fade.title_msec,FALSE);
	list_palette_add_int(&property_palette,kSettingsFaseMapMilliseconds,"Map Milliseconds",&iface.fade.map_msec,FALSE);
}

/* =======================================================

      Property Palette Click Project
      
======================================================= */

void property_palette_click_project(bool double_click)
{
}

