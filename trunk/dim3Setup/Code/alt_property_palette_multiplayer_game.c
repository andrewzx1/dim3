/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette Multiplayer Game

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

#define kMPGamePropertySettingsName				0
#define kMPGamePropertySettingsTeam				1
#define kMPGamePropertySettingsMonster			2

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

/* =======================================================

      Property Palette Fill Multiplayer Game
      
======================================================= */

void alt_property_palette_fill_multiplayer_game(int multiplayer_game_idx)
{
	iface_net_game_type		*game;

	game=&iface.net_game.games[multiplayer_game_idx];

		// settings

	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_string(&alt_property_palette,kMPGamePropertySettingsName,"Name",game->name,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kMPGamePropertySettingsTeam,"Requires Teams",game->use_teams,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kMPGamePropertySettingsMonster,"Include Monsters",game->monsters,FALSE);
}

/* =======================================================

      Property Palette Click Multiplayer Game
      
======================================================= */

void alt_property_palette_click_multiplayer_game(int multiplayer_game_idx,int id)
{
	iface_net_game_type		*game;

	game=&iface.net_game.games[multiplayer_game_idx];

	switch (id) {
			
			// settings

		case kMPGamePropertySettingsName:
			dialog_property_string_run(list_string_value_string,(void*)game->name,name_str_len,0,0);
			break;

		case kMPGamePropertySettingsTeam:
			game->use_teams=!game->use_teams;
			break;

		case kMPGamePropertySettingsMonster:
			game->monsters=!game->monsters;
			break;

	}

		// redraw

	main_wind_draw();
}

