/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette HUD Text

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

#define kTextSettingsName						0
#define kTextSettingsData						1
#define kTextSettingsShow						2

#define kTextPositionX							3
#define kTextPositionY							4

#define kTextOptionSize							5
#define kTextOptionJust							6
#define kTextOptionSpecial						7
#define kTextOptionAlpha						8
#define kTextOptionColor						9

#define kTextFadeOn								10
#define kTextFadeInTick							11
#define kTextFadeLifeTick						12
#define kTextFadeOutTick						13

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

char							hud_text_just_type_str[][32]={"Left","Center","Right",""},
								hud_text_special_type_str[][32]=text_special_list_def;

/* =======================================================

      Property Palette Fill HUD Text
      
======================================================= */

void alt_property_palette_fill_hud_text(int hud_text_idx)
{
	iface_text_type				*text;

	text=&iface.text_list.texts[hud_text_idx];

	list_palette_set_sub_title(&alt_property_palette,"HUD Text",text->name);

		// settings

	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_string(&alt_property_palette,kTextSettingsName,"Name",text->name,FALSE);
	list_palette_add_string(&alt_property_palette,kTextSettingsData,"Data",text->data,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kTextSettingsShow,"Show",&text->show,FALSE);

		// position

	list_palette_add_header(&alt_property_palette,0,"Position");
	list_palette_add_string_int(&alt_property_palette,kTextPositionX,"X",text->x,FALSE);
	list_palette_add_string_int(&alt_property_palette,kTextPositionY,"Y",text->y,FALSE);

		// options

	list_palette_add_header(&alt_property_palette,0,"Options");
	list_palette_add_string_int(&alt_property_palette,kTextOptionSize,"Text Size",text->size,FALSE);
	list_palette_add_string(&alt_property_palette,kTextOptionJust,"Justification",hud_text_just_type_str[text->just],FALSE);
	list_palette_add_string(&alt_property_palette,kTextOptionSpecial,"Special",hud_text_special_type_str[text->special],FALSE);
	list_palette_add_string_float(&alt_property_palette,kTextOptionAlpha,"Alpha",text->alpha,FALSE);
	list_palette_add_pick_color(&alt_property_palette,kTextOptionColor,"Color",&text->color,FALSE);

		// fade

	list_palette_add_header(&alt_property_palette,0,"Fade");
	list_palette_add_checkbox(&alt_property_palette,kTextFadeOn,"On",&text->fade.on,FALSE);
	list_palette_add_string_int(&alt_property_palette,kTextFadeInTick,"Fade In Milliseconds",text->fade.fade_in_tick,FALSE);
	list_palette_add_string_int(&alt_property_palette,kTextFadeLifeTick,"Life Milliseconds",text->fade.life_tick,FALSE);
	list_palette_add_string_int(&alt_property_palette,kTextFadeOutTick,"Fade Out Milliseconds",text->fade.fade_out_tick,FALSE);
}

/* =======================================================

      Property Palette Click HUD Text
      
======================================================= */

void alt_property_palette_click_hud_text(int hud_text_idx,int id,bool double_click)
{
	iface_text_type				*text;

	if (!double_click) return;

	text=&iface.text_list.texts[hud_text_idx];

	switch (id) {

			// settings

		case kTextSettingsName:
			dialog_property_string_run(list_string_value_string,(void*)text->name,name_str_len,0,0);
			break;

		case kTextSettingsData:
			dialog_property_string_run(list_string_value_string,(void*)text->data,max_hud_text_str_sz,0,0);
			break;

			// position

		case kTextPositionX:
			dialog_property_string_run(list_string_value_positive_int,(void*)&text->x,0,0,0);
			break;

		case kTextPositionY:
			dialog_property_string_run(list_string_value_positive_int,(void*)&text->y,0,0,0);
			break;

			// options

		case kTextOptionSize:
			dialog_property_string_run(list_string_value_positive_int,(void*)&text->size,0,0,0);
			break;

		case kTextOptionJust:
			property_pick_list("Pick a Justification",(char*)hud_text_just_type_str,&text->just);
			break;

		case kTextOptionSpecial:
			property_pick_list("Pick a Special Display",(char*)hud_text_special_type_str,&text->special);
			break;

		case kTextOptionAlpha:
			dialog_property_string_run(list_string_value_0_to_1_float,(void*)&text->alpha,0,0,0);
			break;

			// fade

		case kTextFadeInTick:
			dialog_property_string_run(list_string_value_positive_int,(void*)&text->fade.fade_in_tick,0,0,0);
			break;

		case kTextFadeLifeTick:
			dialog_property_string_run(list_string_value_positive_int,(void*)&text->fade.life_tick,0,0,0);
			break;

		case kTextFadeOutTick:
			dialog_property_string_run(list_string_value_positive_int,(void*)&text->fade.fade_out_tick,0,0,0);
			break;

	}
}

