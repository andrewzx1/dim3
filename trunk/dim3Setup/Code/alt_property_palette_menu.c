/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette Menu

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

#define kMenuPropertyName						0

#define kMenuPropertyMenuItemAdd				100

#define kMenuPropertyMenuItemName				1000
#define kMenuPropertyMenuItemDelete				2000

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

/* =======================================================

      Alt Property Palette Fill Menu
      
======================================================= */

void alt_property_palette_fill_menu(int menu_idx)
{
	int					n;
	iface_menu_type		*menu;

	menu=&iface.menu_list.menus[menu_idx];

		// settings

	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_string(&alt_property_palette,kMenuPropertyName,"Name",menu->name,FALSE);

		// menu items
		// do NOT sort these as they have a natural order

	list_palette_add_header_button(&alt_property_palette,kMenuPropertyMenuItemAdd,"Items",list_button_plus);
	
	for (n=0;n!=menu->nitem;n++) {
		list_palette_add_string_selectable_button(&alt_property_palette,(kMenuPropertyMenuItemName+n),list_button_minus,(kMenuPropertyMenuItemDelete+n),menu->items[n].data,NULL,(state.cur_menu_item_idx==n),FALSE);
	}
}

/* =======================================================

      Alt Property Palette Click Menu
      
======================================================= */

void alt_property_palette_click_menu(int menu_idx,int id)
{
	int					idx,sz;
	iface_menu_type		*menu;

	menu=&iface.menu_list.menus[menu_idx];

		// select item

	if ((id>=kMenuPropertyMenuItemName) && (id<(kMenuPropertyMenuItemName+max_menu_item))) {
		state.cur_menu_item_idx=id-kMenuPropertyMenuItemName;
		main_wind_draw();
		return;
	}

		// delete menu

	if ((id>=kMenuPropertyMenuItemDelete) && (id<(kMenuPropertyMenuItemDelete+max_menu_item))) {
		state.cur_menu_item_idx=-1;

		idx=id-kMenuPropertyMenuItemDelete;

		sz=(menu->nitem-idx)-1;
		if (sz>0) memmove(&menu->items[idx],&menu->items[idx+1],(sz*sizeof(iface_menu_item_type)));

		menu->nitem--;

		main_wind_draw();
		return;
	}

		// add menu

	if (id==kMenuPropertyMenuItemAdd) {
		state.cur_menu_item_idx=-1;

		if (menu->nitem>=max_menu_item) {
			os_dialog_alert("Add Menu Item","Reached the maximum number of menu items");
			return;
		}

		idx=menu->nitem;
		menu->nitem++;

		dialog_property_string_run(list_string_value_string,(void*)menu->items[idx].data,max_menu_item_data_sz,0,0);

		menu->items[idx].item_id=-1;
		menu->items[idx].multiplayer_disable=FALSE;
		menu->items[idx].quit=FALSE;
		menu->items[idx].sub_menu[0]=0x0;

		state.cur_menu_item_idx=idx;

		main_wind_draw();
		return;
	}

		// regular picks, always
		// disable selection

	state.cur_menu_item_idx=-1;

	switch (id) {

		case kMenuPropertyName:
			dialog_property_string_run(list_string_value_string,(void*)menu->name,name_str_len,0,0);
			break;

	}

		// redraw

	main_wind_draw();
}

