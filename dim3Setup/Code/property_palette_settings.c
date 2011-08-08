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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3setup.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kSettingsProjectName					0
#define kSettingsScaleX							1
#define kSettingsScaleY							2

#define kSettingsInterfaceFont					10
#define kSettingsHUDFont						20

#define kSettingsClickSound						30

#define kSettingsColorBackground				40
#define kSettingsColorDefaultTint				41

#define kSettingsColorMenuText					50
#define kSettingsColorMenuMouseOver				51
#define kSettingsColorMenuDimmed				52

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

#define kSettingColorSystemMetric				103

#define kSettingsProgressLeft					110
#define kSettingsProgressRight					111
#define kSettingsProgressTop					112
#define kSettingsProgressBottom					113
#define kSettingsProgressTextSize				114
#define kSettingsProgressOutline				115
#define kSettingsProgressBaseColorStart			116
#define kSettingsProgressBaseColorEnd			117
#define kSettingsProgressHiliteColorStart		118
#define kSettingsProgressHiliteColorEnd			119
#define kSettingsProgressTextColor				120
#define kSettingsProgressOutlineColor			121

#define kSettingsFaseTitleMilliseconds			130
#define kSettingsFaseMapMilliseconds			131

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

	list_palette_set_title(&property_palette,"Project Settings");

		// project
		
	list_palette_add_header(&property_palette,0,"Project");
	list_palette_add_string(&property_palette,kSettingsProjectName,"Name",iface.project_name,FALSE);

		// scale
		
	list_palette_add_header(&property_palette,0,"Scale");
	list_palette_add_string_int(&property_palette,kSettingsScaleX,"X",iface.scale_x,FALSE);
	list_palette_add_string_int(&property_palette,kSettingsScaleY,"Y",iface.scale_y,FALSE);

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
	list_palette_add_pick_color(&property_palette,kSettingsColorMenuText,"Text",&iface.color.menu.text,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorMenuMouseOver,"Mouse Over",&iface.color.menu.mouse_over,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorMenuDimmed,"Dimmed",&iface.color.menu.dimmed,FALSE);

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

void property_palette_click_settings(int id,bool double_click)
{
	if (!double_click) return;

		// fonts

	if ((id>=kSettingsInterfaceFont) && (id<(kSettingsInterfaceFont+max_iface_font_variant))) {
		dialog_property_string_run(list_string_value_string,(void*)iface.font.interface_name[id-kSettingsInterfaceFont],name_str_len,0,0);
		main_wind_draw();
		return;
	}

	if ((id>=kSettingsHUDFont) && (id<(kSettingsHUDFont+max_iface_font_variant))) {
		dialog_property_string_run(list_string_value_string,(void*)iface.font.hud_name[id-kSettingsHUDFont],name_str_len,0,0);
		main_wind_draw();
		return;
	}

		// regular items

	switch (id) {
	
			// project name

		case kSettingsProjectName:
			dialog_property_string_run(list_string_value_string,(void*)iface.project_name,name_str_len,0,0);
			break;

			// setting
			
		case kSettingsScaleX:
			dialog_property_string_run(list_string_value_positive_int,(void*)&iface.scale_x,0,0,0);
			break;

		case kSettingsScaleY:
			dialog_property_string_run(list_string_value_positive_int,(void*)&iface.scale_y,0,0,0);
			break;
			
			// sound
			
		case kSettingsClickSound:
			property_palette_pick_sound(iface.click_sound,FALSE);
			break;
			
			// main color
			
		case kSettingsColorBackground:
			os_pick_color(&iface.color.background);
			break;
			
		case kSettingsColorDefaultTint:
			os_pick_color(&iface.color.default_tint);
			break;
			
			// menu color
			
		case kSettingsColorMenuText:
			os_pick_color(&iface.color.menu.text);
			break;
			
		case kSettingsColorMenuMouseOver:
			os_pick_color(&iface.color.menu.mouse_over);
			break;
			
		case kSettingsColorMenuDimmed:
			os_pick_color(&iface.color.menu.dimmed);
			break;

			// dialog color
			
		case kSettingsColorDialogBackground:
			os_pick_color(&iface.color.dialog.background);
			break;

		case kSettingsColorDialogHeader:
			os_pick_color(&iface.color.dialog.header);
			break;

		case kSettingsColorDialogOutline:
			os_pick_color(&iface.color.dialog.outline);
			break;

		case kSettingsColorDialogTitle:
			os_pick_color(&iface.color.dialog.title);
			break;

			// tab color
			
		case kSettingsColorTabBackground:
			os_pick_color(&iface.color.tab.background);
			break;

		case kSettingsColorTabDimmed:
			os_pick_color(&iface.color.tab.dimmed);
			break;

		case kSettingsColorTabOutline:
			os_pick_color(&iface.color.tab.outline);
			break;
			
		case kSettingsColorTabText:
			os_pick_color(&iface.color.tab.text);
			break;
			
		case kSettingsColorTabTextMouseOver:
			os_pick_color(&iface.color.tab.text_mouse_over);
			break;
			
		case kSettingsColorTabTextDimmed:
			os_pick_color(&iface.color.tab.text_dimmed);
			break;

			// control color
			
		case kSettingsColorControlLabel:
			os_pick_color(&iface.color.control.label);
			break;

		case kSettingsColorControlText:
			os_pick_color(&iface.color.control.text);
			break;

		case kSettingsColorControlFill:
			os_pick_color(&iface.color.control.fill);
			break;

		case kSettingsColorControlHeader:
			os_pick_color(&iface.color.control.header);
			break;

		case kSettingsColorControlOutline:
			os_pick_color(&iface.color.control.outline);
			break;

		case kSettingsColorControlMouseOver:
			os_pick_color(&iface.color.control.mouse_over);
			break;

		case kSettingsColorControlHilite:
			os_pick_color(&iface.color.control.hilite);
			break;

		case kSettingsColorControlDisabled:
			os_pick_color(&iface.color.control.disabled);
			break;

			// scrollbar color
		
		case kSettingsColorScrollBarBackground:
			os_pick_color(&iface.color.scrollbar.background);
			break;

		case kSettingsColorScrollBarThumb:
			os_pick_color(&iface.color.scrollbar.thumb);
			break;

			// button color
			
		case kSettingsColorButtonFill:
			os_pick_color(&iface.color.button.fill);
			break;

		case kSettingsColorButtonText:
			os_pick_color(&iface.color.button.text);
			break;

		case kSettingsColorButtonOutline:
			os_pick_color(&iface.color.button.outline);
			break;

			// system color
		
		case kSettingColorSystemMetric:
			os_pick_color(&iface.color.system.metric);
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

