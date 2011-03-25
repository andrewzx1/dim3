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

#define kSettingsScaleX							0
#define kSettingsScaleY							1
#define kSettingsInterfaceFont					2
#define kSettingsHUDFont						3
#define kSettingsColorDialogBackground			4
#define kSettingsColorDialogBase				5
#define kSettingsColorDialogDimmed				6
#define kSettingsColorDialogOutline				7
#define kSettingsColorControlLabel				8
#define kSettingsColorControlText				9
#define kSettingsColorControlFill				10
#define kSettingsColorControlHeader				11
#define kSettingsColorControlOutline			12
#define kSettingsColorControlMouseOver			13
#define kSettingsColorControlHilite				14
#define kSettingsColorControlDisabled			15
#define kSettingsColorButtonFill				16
#define kSettingsColorButtonText				17
#define kSettingsColorButtonOutline				18
#define kSettingsColorDefaultTint				19

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Settings
      
======================================================= */

void property_palette_fill_settings(void)
{
	list_palette_add_header(&property_palette,0,"Scale");
	list_palette_add_string_int(&property_palette,kSettingsScaleX,"X",iface.scale_x,FALSE);
	list_palette_add_string_int(&property_palette,kSettingsScaleY,"Y",iface.scale_y,FALSE);

	list_palette_add_header(&property_palette,0,"Font");
	list_palette_add_string(&property_palette,kSettingsInterfaceFont,"Interface Font",iface.font.interface_name,FALSE);
	list_palette_add_string(&property_palette,kSettingsHUDFont,"HUD Font",iface.font.hud_name,FALSE);

	list_palette_add_header(&property_palette,0,"Interface Colors");
	list_palette_add_pick_color(&property_palette,kSettingsColorDialogBackground,"Dialog Background",&iface.color.dialog_background,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorDialogBase,"Dialog Base",&iface.color.dialog_base,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorDialogDimmed,"Dialog Dimmed",&iface.color.dialog_dimmed,FALSE);
	list_palette_add_pick_color(&property_palette,kSettingsColorDialogOutline,"Dialog Outline",&iface.color.dialog_outline,FALSE);
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

	list_palette_add_header(&property_palette,0,"Interface Progress");
	list_palette_add_header(&property_palette,0,"Interface Chat");
	list_palette_add_header(&property_palette,0,"Interface Fade");

	/*
			iface->progress.lx=xml_get_attribute_int(progress_tag,"left_x");
		iface->progress.rx=xml_get_attribute_int(progress_tag,"right_x");
		iface->progress.ty=xml_get_attribute_int(progress_tag,"top_y");
		iface->progress.by=xml_get_attribute_int(progress_tag,"bottom_y");
		iface->progress.text_size=xml_get_attribute_int(progress_tag,"text_size");
		iface->progress.outline=xml_get_attribute_boolean(progress_tag,"outline");
		xml_get_attribute_color(progress_tag,"base_color_start",&iface->progress.base_color_start);
		xml_get_attribute_color(progress_tag,"base_color_end",&iface->progress.base_color_end);
		xml_get_attribute_color(progress_tag,"hilite_color_start",&iface->progress.hilite_color_start);
		xml_get_attribute_color(progress_tag,"hilite_color_end",&iface->progress.hilite_color_end);
		xml_get_attribute_color(progress_tag,"text_color",&iface->progress.text_color);
		xml_get_attribute_color(progress_tag,"outline_color",&iface->progress.outline_color);
	}
	
		// chat

	chat_tag=xml_findfirstchild("Chat",interface_head_tag);
	if (chat_tag!=-1) {
		iface->chat.x=xml_get_attribute_int(chat_tag,"x");
		iface->chat.y=xml_get_attribute_int(chat_tag,"y");
		iface->chat.last_add_life_sec=xml_get_attribute_int(chat_tag,"last_add_life_sec");
		iface->chat.next_life_sec=xml_get_attribute_int(chat_tag,"next_life_sec");
	}
	
		// fade

	fade_tag=xml_findfirstchild("Fade",interface_head_tag);
	if (fade_tag!=-1) {
		iface->fade.title_msec=xml_get_attribute_int(fade_tag,"title_msec");
		iface->fade.map_msec=xml_get_attribute_int(fade_tag,"map_msec");
*/
}

/* =======================================================

      Property Palette Click Settings
      
======================================================= */

void property_palette_click_settings(int id)
{
	switch (id) {

		case kSettingsScaleX:
			dialog_property_string_run(list_string_value_positive_int,(void*)&iface.scale_x,0,0,0);
			break;

		case kSettingsScaleY:
			dialog_property_string_run(list_string_value_positive_int,(void*)&iface.scale_y,0,0,0);
			break;

		case kSettingsInterfaceFont:
			dialog_property_string_run(list_string_value_string,(void*)iface.font.interface_name,name_str_len,0,0);
			break;

		case kSettingsHUDFont:
			dialog_property_string_run(list_string_value_string,(void*)iface.font.hud_name,name_str_len,0,0);
			break;

		case kSettingsColorDialogBackground:
			os_pick_color(&iface.color.dialog_background);
			break;

		case kSettingsColorDialogBase:
			os_pick_color(&iface.color.dialog_base);
			break;

		case kSettingsColorDialogDimmed:
			os_pick_color(&iface.color.dialog_dimmed);
			break;

		case kSettingsColorDialogOutline:
			os_pick_color(&iface.color.dialog_outline);
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

	}

		// redraw

	main_wind_draw();
}

