/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt2 Property Palette Menu Item

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

#define kMenuItemPropertySettingsData				0
#define kMenuItemPropertySettingsID					1

#define kMenuItemPropertyOptionsSubMenu				2
#define kMenuItemPropertyOptionsMultiplayerDisable	3
#define kMenuItemPropertyOptionsQuit				4

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt2_property_palette;

/* =======================================================

      Property Palette Fill Menu Item
      
======================================================= */

void alt2_property_palette_fill_menu_item(int menu_idx,int menu_item_idx)
{
	iface_menu_type				*menu;
	iface_menu_item_type		*item;

	menu=&iface.menu_list.menus[menu_idx];
	item=&menu->items[menu_item_idx];

	list_palette_set_sub2_title(&alt2_property_palette,"Menu Item",menu->name,item->data);

		// settings

	list_palette_add_header(&alt2_property_palette,0,"Settings");
	list_palette_add_string(&alt2_property_palette,kMenuItemPropertySettingsData,"Data",item->data,FALSE);
	list_palette_add_string_int(&alt2_property_palette,kMenuItemPropertySettingsID,"Id",item->item_id,FALSE);

		// options

	list_palette_add_header(&alt2_property_palette,0,"Options");
	list_palette_add_string(&alt2_property_palette,kMenuItemPropertyOptionsSubMenu,"Goto SubMenu",item->sub_menu,FALSE);
	list_palette_add_checkbox(&alt2_property_palette,kMenuItemPropertyOptionsMultiplayerDisable,"Multiplayer Disable",item->multiplayer_disable,FALSE);
	list_palette_add_checkbox(&alt2_property_palette,kMenuItemPropertyOptionsQuit,"Quit",item->quit,FALSE);
}

/* =======================================================

      Property Palette Click Menu Item
      
======================================================= */

void alt2_property_palette_click_menu_item(int menu_idx,int menu_item_idx,int id,bool double_click)
{
	iface_menu_item_type		*item;

	if (!double_click) return;

	item=&iface.menu_list.menus[menu_idx].items[menu_item_idx];

	switch (id) {
			
			// settings

		case kMenuItemPropertySettingsData:
			dialog_property_string_run(list_string_value_string,(void*)item->data,max_menu_item_data_sz,0,0);
			break;

		case kMenuItemPropertySettingsID:
			dialog_property_string_run(list_string_value_int,(void*)&item->item_id,0,0,0);
			break;

			// options

		case kMenuItemPropertyOptionsSubMenu:
			dialog_property_string_run(list_string_value_string,(void*)item->sub_menu,name_str_len,0,0);
			break;

		case kMenuItemPropertyOptionsMultiplayerDisable:
			item->multiplayer_disable=!item->multiplayer_disable;
			break;

		case kMenuItemPropertyOptionsQuit:
			item->quit=!item->quit;
			break;


	}

		// redraw

	main_wind_draw();
}

