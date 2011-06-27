/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Settings

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

#define kSettingsScaleX							0
#define kSettingsScaleY							1
#define kSettingsInterfaceFont					2
#define kSettingsHUDFont						3
#define kSettingsColorDialogBackground			4
#define kSettingsColorDialogHeader				5
#define kSettingsColorDialogOutline				6
#define kSettingsColorDialogTitle				7
#define kSettingsColorTabBackground				8
#define kSettingsColorTabDimmed					9
#define kSettingsColorTabOutline				10
#define kSettingsColorControlLabel				11
#define kSettingsColorControlText				12
#define kSettingsColorControlFill				13
#define kSettingsColorControlHeader				14
#define kSettingsColorControlOutline			15
#define kSettingsColorControlMouseOver			16
#define kSettingsColorControlHilite				17
#define kSettingsColorControlDisabled			18
#define kSettingsColorButtonFill				19
#define kSettingsColorButtonText				20
#define kSettingsColorButtonOutline				21
#define kSettingsColorDefaultTint				22
#define kSettingsProgressLeft					23
#define kSettingsProgressRight					24
#define kSettingsProgressTop					25
#define kSettingsProgressBottom					26
#define kSettingsProgressTextSize				27
#define kSettingsProgressOutline				28
#define kSettingsProgressBaseColorStart			29
#define kSettingsProgressBaseColorEnd			30
#define kSettingsProgressHiliteColorStart		31
#define kSettingsProgressHiliteColorEnd			32
#define kSettingsProgressTextColor				33
#define kSettingsProgressOutlineColor			34
#define kSettingsFaseTitleMilliseconds			35
#define kSettingsFaseMapMilliseconds			36

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Settings
      
======================================================= */

void property_palette_fill_settings(void)
{
		// scale
		
	list_palette_add_header(&property_palette,0,"Scale");
	list_palette_add_string_int(&property_palette,kSettingsScaleX,"X",iface.scale_x,FALSE);
	list_palette_add_string_int(&property_palette,kSettingsScaleY,"Y",iface.scale_y,FALSE);

		// font
		
	list_palette_add_header(&property_palette,0,"Font");
	list_palette_add_string(&property_palette,kSettingsInterfaceFont,"Interface Font",iface.font.interface_name,FALSE);
	list_palette_add_string(&property_palette,kSettingsHUDFont,"HUD Font",iface.font.hud_name,FALSE);

		// colors
		
	list_palette_add_header(&property_palette,0,"Colors");
	list_palette_add_pick_color(&property_palette,kSettingsColorDialogBackground,"Dialog Background",&iface.color.dialog_background,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorDialogHeader,"Dialog Header",&iface.color.dialog_header,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorDialogOutline,"Dialog Outline",&iface.color.dialog_outline,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorDialogTitle,"Dialog Title",&iface.color.dialog_title,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorTabBackground,"Tab Background",&iface.color.tab_background,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorTabDimmed,"Tab Dimmed",&iface.color.tab_dimmed,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorTabOutline,"Tab Outline",&iface.color.tab_outline,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorControlLabel,"Control Label",&iface.color.control_label,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorControlText,"Control Text",&iface.color.control_text,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorControlFill,"Control Fill",&iface.color.control_fill,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorControlHeader,"Control Header",&iface.color.control_header,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorControlOutline,"Control Outline",&iface.color.control_outline,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorControlMouseOver,"Control Mouse Over",&iface.color.control_mouse_over,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorControlHilite,"Control Highlight",&iface.color.control_hilite,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorControlDisabled,"Control Disabled",&iface.color.control_disabled,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorButtonFill,"Button Fill",&iface.color.button_fill,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorButtonText,"Button Text",&iface.color.button_text,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorButtonOutline,"Button Outline",&iface.color.button_outline,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorDefaultTint,"Default Tint",&iface.color.default_tint,FALSE);

		// progress
		
	list_palette_add_header(&property_palette,0,"Progress");
	list_palette_add_string_int(&property_palette,kSettingsProgressLeft,"Left X",iface.progress.lx,FALSE);
	list_palette_add_string_int(&property_palette,kSettingsProgressRight,"Right X",iface.progress.rx,FALSE);
	list_palette_add_string_int(&property_palette,kSettingsProgressTop,"Top Y",iface.progress.ty,FALSE);
	list_palette_add_string_int(&property_palette,kSettingsProgressBottom,"Bottom Y",iface.progress.by,FALSE);
	list_palette_add_string_int(&property_palette,kSettingsProgressTextSize,"Text Size",iface.progress.text_size,FALSE);
	list_palette_add_checkbox(&property_palette,kSettingsProgressOutline,"Outline",iface.progress.outline,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsProgressBaseColorStart,"Base Color Start",&iface.progress.base_color_start,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsProgressBaseColorEnd,"Base Color End",&iface.progress.base_color_end,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsProgressHiliteColorStart,"Highlight Color Start",&iface.progress.hilite_color_start,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsProgressHiliteColorEnd,"Highlight Color End",&iface.progress.hilite_color_end,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsProgressTextColor,"Text Color",&iface.progress.text_color,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsProgressOutlineColor,"Outline Color",&iface.progress.outline_color,FALSE);
	
		// fades
		
	list_palette_add_header(&property_palette,0,"Fades");
	list_palette_add_string_int(&property_palette,kSettingsFaseTitleMilliseconds,"Title Milliseconds",iface.fade.title_msec,FALSE);
	list_palette_add_string_int(&property_palette,kSettingsFaseMapMilliseconds,"Map Milliseconds",iface.fade.map_msec,FALSE);
}

/* =======================================================

      Property Palette Click Settings
      
======================================================= */

void property_palette_click_settings(int id)
{
	switch (id) {

			// setting
			
		case kSettingsScaleX:
			dialog_property_string_run(list_string_value_positive_int,(void*)&iface.scale_x,0,0,0);
			break;

		case kSettingsScaleY:
			dialog_property_string_run(list_string_value_positive_int,(void*)&iface.scale_y,0,0,0);
			break;
			
			// font

		case kSettingsInterfaceFont:
			dialog_property_string_run(list_string_value_string,(void*)iface.font.interface_name,name_str_len,0,0);
			break;

		case kSettingsHUDFont:
			dialog_property_string_run(list_string_value_string,(void*)iface.font.hud_name,name_str_len,0,0);
			break;
			
			// color

		case kSettingsColorDialogBackground:
			os_pick_color(&iface.color.dialog_background);
			break;

		case kSettingsColorDialogHeader:
			os_pick_color(&iface.color.dialog_header);
			break;

		case kSettingsColorDialogOutline:
			os_pick_color(&iface.color.dialog_outline);
			break;

		case kSettingsColorDialogTitle:
			os_pick_color(&iface.color.dialog_title);
			break;

		case kSettingsColorTabBackground:
			os_pick_color(&iface.color.tab_background);
			break;

		case kSettingsColorTabDimmed:
			os_pick_color(&iface.color.tab_dimmed);
			break;

		case kSettingsColorTabOutline:
			os_pick_color(&iface.color.tab_outline);
			break;

		case kSettingsColorControlLabel:
			os_pick_color(&iface.color.control_label);
			break;

		case kSettingsColorControlText:
			os_pick_color(&iface.color.control_text);
			break;

		case kSettingsColorControlFill:
			os_pick_color(&iface.color.control_fill);
			break;

		case kSettingsColorControlHeader:
			os_pick_color(&iface.color.control_header);
			break;

		case kSettingsColorControlOutline:
			os_pick_color(&iface.color.control_outline);
			break;

		case kSettingsColorControlMouseOver:
			os_pick_color(&iface.color.control_mouse_over);
			break;

		case kSettingsColorControlHilite:
			os_pick_color(&iface.color.control_hilite);
			break;

		case kSettingsColorControlDisabled:
			os_pick_color(&iface.color.control_disabled);
			break;

		case kSettingsColorButtonFill:
			os_pick_color(&iface.color.button_fill);
			break;

		case kSettingsColorButtonText:
			os_pick_color(&iface.color.button_text);
			break;

		case kSettingsColorButtonOutline:
			os_pick_color(&iface.color.button_outline);
			break;

		case kSettingsColorDefaultTint:
			os_pick_color(&iface.color.default_tint);
			break;
			
			// progress
			
		case kSettingsProgressLeft:
			dialog_property_string_run(list_string_value_positive_int,(void*)&iface.progress.lx,0,0,0);
			break;

		case kSettingsProgressRight:
			dialog_property_string_run(list_string_value_positive_int,(void*)&iface.progress.rx,0,0,0);
			break;

		case kSettingsProgressTop:
			dialog_property_string_run(list_string_value_positive_int,(void*)&iface.progress.ty,0,0,0);
			break;

		case kSettingsProgressBottom:
			dialog_property_string_run(list_string_value_positive_int,(void*)&iface.progress.by,0,0,0);
			break;

		case kSettingsProgressTextSize:
			dialog_property_string_run(list_string_value_positive_int,(void*)&iface.progress.text_size,0,0,0);
			break;

		case kSettingsProgressOutline:
			iface.progress.outline=!iface.progress.outline;
			break;
			
		case kSettingsProgressBaseColorStart:
			os_pick_color(&iface.progress.base_color_start);
			break;
			
		case kSettingsProgressBaseColorEnd:
			os_pick_color(&iface.progress.base_color_end);
			break;
			
		case kSettingsProgressHiliteColorStart:
			os_pick_color(&iface.progress.hilite_color_start);
			break;
			
		case kSettingsProgressHiliteColorEnd:
			os_pick_color(&iface.progress.hilite_color_end);
			break;
			
		case kSettingsProgressTextColor:
			os_pick_color(&iface.progress.text_color);
			break;
			
		case kSettingsProgressOutlineColor:
			os_pick_color(&iface.progress.outline_color);
			break;
	
			// fades
		
		case kSettingsFaseTitleMilliseconds:
			dialog_property_string_run(list_string_value_positive_int,(void*)&iface.fade.title_msec,0,0,0);
			break;
			
		case kSettingsFaseMapMilliseconds:
			dialog_property_string_run(list_string_value_positive_int,(void*)&iface.fade.map_msec,0,0,0);
			break;
			

	}

		// redraw

	main_wind_draw();
}

