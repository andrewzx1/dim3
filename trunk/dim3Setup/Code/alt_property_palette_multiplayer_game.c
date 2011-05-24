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
#define kMPGamePropertyScriptPlayerScript		3
#define kMPGamePropertyScriptBotScript			4
#define kMPGamePropertySpawnSpotName			5
#define kMPGamePropertySpawnForceTeam			6
#define kMPGamePropertyScoreKill				7
#define kMPGamePropertyScoreDeath				8
#define kMPGamePropertyScoreSuicide				9
#define kMPGamePropertyScoreGoal				10

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

		// bots

	list_palette_add_header(&alt_property_palette,0,"Scripts");
	list_palette_add_string(&alt_property_palette,kMPGamePropertyScriptPlayerScript,"Player",game->script.player_script,FALSE);
	list_palette_add_string(&alt_property_palette,kMPGamePropertyScriptBotScript,"Bot",game->script.bot_script,FALSE);
	
		// spawn

	list_palette_add_header(&alt_property_palette,0,"Spawning");
	list_palette_add_string(&alt_property_palette,kMPGamePropertySpawnSpotName,"Spot Name",game->spawn.spot_name,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kMPGamePropertySpawnForceTeam,"Force Team Spots",game->spawn.force_team_spot,FALSE);
	
		// score

	list_palette_add_header(&alt_property_palette,0,"Scoring");
	list_palette_add_string_int(&alt_property_palette,kMPGamePropertyScoreKill,"Kill",game->score.kill,FALSE);
	list_palette_add_string_int(&alt_property_palette,kMPGamePropertyScoreDeath,"Death",game->score.death,FALSE);
	list_palette_add_string_int(&alt_property_palette,kMPGamePropertyScoreSuicide,"Suicide",game->score.suicide,FALSE);
	list_palette_add_string_int(&alt_property_palette,kMPGamePropertyScoreGoal,"Goal",game->score.goal,FALSE);
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

			// bots

		case kMPGamePropertyScriptPlayerScript:
			dialog_property_string_run(list_string_value_string,(void*)game->script.player_script,file_str_len,0,0);
			break;
			
		case kMPGamePropertyScriptBotScript:
			dialog_property_string_run(list_string_value_string,(void*)game->script.bot_script,file_str_len,0,0);
			break;
			
			// spawn

		case kMPGamePropertySpawnSpotName:
			dialog_property_string_run(list_string_value_string,(void*)game->spawn.spot_name,name_str_len,0,0);
			break;

		case kMPGamePropertySpawnForceTeam:
			game->spawn.force_team_spot=!game->spawn.force_team_spot;
			break;
	
			// score

		case kMPGamePropertyScoreKill:
			dialog_property_string_run(list_string_value_int,(void*)&game->score.kill,0,0,0);
			break;
			
		case kMPGamePropertyScoreDeath:
			dialog_property_string_run(list_string_value_int,(void*)&game->score.death,0,0,0);
			break;
			
		case kMPGamePropertyScoreSuicide:
			dialog_property_string_run(list_string_value_int,(void*)&game->score.suicide,0,0,0);
			break;
			
		case kMPGamePropertyScoreGoal:
			dialog_property_string_run(list_string_value_int,(void*)&game->score.goal,0,0,0);
			break;


	}

		// redraw

	main_wind_draw();
}

