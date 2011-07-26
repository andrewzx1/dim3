/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette Multiplayer Character

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

#define kMPCharacterPropertySettingsName		0
#define kMPCharacterPropertySettingsModelName	1
#define kMPCharacterPropertySettingsParam		2

#define kMPCharacterPropertyOptionsResize		3
#define kMPCharacterPropertyOptionsOffset		4

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

/* =======================================================

      Property Palette Fill Multiplayer Character
      
======================================================= */

void alt_property_palette_fill_multiplayer_character(int multiplayer_character_idx)
{
	iface_character_item_type		*character;

	character=&iface.character.characters[multiplayer_character_idx];

	list_palette_set_sub_title(&alt_property_palette,"Character",character->name);

		// settings

	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_string(&alt_property_palette,kMPCharacterPropertySettingsName,"Name",character->name,FALSE);
	list_palette_add_string(&alt_property_palette,kMPCharacterPropertySettingsModelName,"Model Name",character->model_name,FALSE);
	list_palette_add_string(&alt_property_palette,kMPCharacterPropertySettingsParam,"Parameters",character->param,FALSE);

		// options

	list_palette_add_header(&alt_property_palette,0,"UI Drawing");
	list_palette_add_string_float(&alt_property_palette,kMPCharacterPropertyOptionsResize,"Resize",character->interface_resize,FALSE);
	list_palette_add_point(&alt_property_palette,kMPCharacterPropertyOptionsOffset,"Offset",&character->interface_offset,FALSE);
}

/* =======================================================

      Property Palette Click Multiplayer Character
      
======================================================= */

void alt_property_palette_click_multiplayer_character(int multiplayer_character_idx,int id)
{
	char							model_name[name_str_len];
	iface_character_item_type		*character;

	character=&iface.character.characters[multiplayer_character_idx];

	switch (id) {

			// settings

		case kMPCharacterPropertySettingsName:
			dialog_property_string_run(list_string_value_string,(void*)character->name,name_str_len,0,0);
			break;

		case kMPCharacterPropertySettingsModelName:
			strcpy(model_name,character->model_name);
			if (dialog_file_open_run("Pick a Model","Models",NULL,"Mesh.xml",model_name)) strcpy(character->model_name,model_name);
			break;

		case kMPCharacterPropertySettingsParam:
			dialog_property_string_run(list_string_value_string,(void*)character->param,name_str_len,0,0);
			break;

			// options

		case kMPCharacterPropertyOptionsResize:
			dialog_property_string_run(list_string_value_positive_float,(void*)&character->interface_resize,0,0,0);
			break;

		case kMPCharacterPropertyOptionsOffset:
			dialog_property_chord_run(list_chord_value_point,(void*)&character->interface_offset);
			break;

	}

		// redraw

	main_wind_draw();
}

