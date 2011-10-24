/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette HUD Bar

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

#define kBarSettingsName						0
#define kBarSettingsShow						1
#define kBarSettingsVertical					2

#define kBarPositionX							3
#define kBarPositionY							4
#define kBarPositionWid							5
#define kBarPositionHigh						6

#define kBarFillStartColor						7
#define kBarFillEndColor						8
#define kBarFillAlpha							9

#define kBarSettingsOutline						10
#define kBarOutlineColor						11
#define kBarOutlineAlpha						12

#define kBarSettingsBackground					20
#define kBarBackgroundColor						21
#define kBarBackgroundAlpha						22

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

/* =======================================================

      Property Palette Fill HUD Bar
      
======================================================= */

void alt_property_palette_fill_hud_bar(int hud_bar_idx)
{
	iface_bar_type				*bar;

	bar=&iface.bar_list.bars[hud_bar_idx];

	list_palette_set_sub_title(&alt_property_palette,"HUD Bar",bar->name);

		// settings

	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_string(&alt_property_palette,kBarSettingsName,"Name",bar->name,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kBarSettingsShow,"Show",&bar->show,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kBarSettingsVertical,"Vertical",&bar->vert,FALSE);

		// position

	list_palette_add_header(&alt_property_palette,0,"Position");
	list_palette_add_string_int(&alt_property_palette,kBarPositionX,"X",bar->x,FALSE);
	list_palette_add_string_int(&alt_property_palette,kBarPositionY,"Y",bar->y,FALSE);
	list_palette_add_string_int(&alt_property_palette,kBarPositionWid,"Width",bar->x_size,FALSE);
	list_palette_add_string_int(&alt_property_palette,kBarPositionHigh,"Height",bar->y_size,FALSE);

		// fill

	list_palette_add_header(&alt_property_palette,0,"Fill");
	list_palette_add_pick_color(&alt_property_palette,kBarFillStartColor,"Start Color",&bar->fill_start_color,FALSE);
	list_palette_add_pick_color(&alt_property_palette,kBarFillEndColor,"End Color",&bar->fill_end_color,FALSE);
	list_palette_add_string_float(&alt_property_palette,kBarFillAlpha,"Alpha",bar->fill_alpha,FALSE);

		// outline

	list_palette_add_header(&alt_property_palette,0,"Outline");
	list_palette_add_checkbox(&alt_property_palette,kBarSettingsOutline,"On",&bar->outline,FALSE);
	list_palette_add_pick_color(&alt_property_palette,kBarOutlineColor,"Color",&bar->outline_color,FALSE);
	list_palette_add_string_float(&alt_property_palette,kBarOutlineAlpha,"Alpha",bar->outline_alpha,FALSE);

		// background

	list_palette_add_header(&alt_property_palette,0,"Background");
	list_palette_add_checkbox(&alt_property_palette,kBarSettingsBackground,"On",&bar->background,FALSE);
	list_palette_add_pick_color(&alt_property_palette,kBarBackgroundColor,"Color",&bar->background_color,FALSE);
	list_palette_add_string_float(&alt_property_palette,kBarBackgroundAlpha,"Alpha",bar->background_alpha,FALSE);
}

/* =======================================================

      Property Palette Click HUD Bar
      
======================================================= */

void alt_property_palette_click_hud_bar(int hud_bar_idx,int id,bool double_click)
{
	iface_bar_type				*bar;

	if (!double_click) return;

	bar=&iface.bar_list.bars[hud_bar_idx];

	switch (id) {

			// settings

		case kBarSettingsName:
			dialog_property_string_run(list_string_value_string,(void*)bar->name,name_str_len,0,0);
			break;

			// position

		case kBarPositionX:
			dialog_property_string_run(list_string_value_positive_int,(void*)&bar->x,0,0,0);
			break;

		case kBarPositionY:
			dialog_property_string_run(list_string_value_positive_int,(void*)&bar->y,0,0,0);
			break;

		case kBarPositionWid:
			dialog_property_string_run(list_string_value_positive_int,(void*)&bar->x_size,0,0,0);
			break;

		case kBarPositionHigh:
			dialog_property_string_run(list_string_value_positive_int,(void*)&bar->y_size,0,0,0);
			break;

			// fill

		case kBarFillAlpha:
			dialog_property_string_run(list_string_value_0_to_1_float,(void*)&bar->fill_alpha,0,0,0);
			break;

			// outline

		case kBarOutlineAlpha:
			dialog_property_string_run(list_string_value_0_to_1_float,(void*)&bar->outline_alpha,0,0,0);
			break;

			//  background

		case kBarBackgroundAlpha:
			dialog_property_string_run(list_string_value_0_to_1_float,(void*)&bar->background_alpha,0,0,0);
			break;

	}
}

