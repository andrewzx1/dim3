/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Multiplayer

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3setup.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kMPPropertyOn							0
#define kMPPropertyLocalOnly					1

#define kMPPropertyNewsHost						2
#define kMPPropertyNewsURL						3
#define kMPPropertyNewsPort						4

#define kMPPropertyBotOn						5

#define kMPPropertyCharacterAdd					6
#define kMPPropertyGameAdd						7
#define kMPPropertyOptionAdd					8

#define kMPPropertyBotName						100
#define kMPPropertyCharacterName				200
#define kMPPropertyCharacterDelete				300
#define kMPPropertyGameName						400
#define kMPPropertyGameDelete					500
#define kMPPropertyOptionName					600
#define kMPPropertyOptionDelete					700

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Multiplayer
      
======================================================= */

void property_palette_fill_multiplayer(void)
{
	int				n;
	char			str[256];

	list_palette_set_title(&property_palette,"Multiplayer",NULL,NULL,NULL,NULL,NULL);

		// settings

	list_palette_add_header(&property_palette,0,"Settings");
	list_palette_add_checkbox(&property_palette,kMPPropertyOn,"On",&iface.multiplayer.on,FALSE);
	list_palette_add_checkbox(&property_palette,kMPPropertyLocalOnly,"Local Only",&iface.multiplayer.local_only,FALSE);

		// games

	list_palette_add_header_button(&property_palette,kMPPropertyGameAdd,"Games",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=iface.multiplayer.game_list.ngame;n++) {
		list_palette_add_string_selectable_button(&property_palette,(kMPPropertyGameName+n),list_button_minus,(kMPPropertyGameDelete+n),iface.multiplayer.game_list.games[n].name,(state.cur_multiplayer_game_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);

		// options

	list_palette_add_header_button(&property_palette,kMPPropertyOptionAdd,"Options",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=iface.multiplayer.option_list.noption;n++) {
		list_palette_add_string_selectable_button(&property_palette,(kMPPropertyOptionName+n),list_button_minus,(kMPPropertyOptionDelete+n),iface.multiplayer.option_list.options[n].name,(state.cur_multiplayer_option_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);

		// characters

	list_palette_add_header_button(&property_palette,kMPPropertyCharacterAdd,"Characters",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=iface.multiplayer.character_list.ncharacter;n++) {
		list_palette_add_string_selectable_button(&property_palette,(kMPPropertyCharacterName+n),list_button_minus,(kMPPropertyCharacterDelete+n),iface.multiplayer.character_list.characters[n].name,(state.cur_multiplayer_character_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);

		// bots

	list_palette_add_header(&property_palette,0,"Bots");
	list_palette_add_checkbox(&property_palette,kMPPropertyBotOn,"On",&iface.multiplayer.bot_list.on,FALSE);

	list_palette_add_header(&property_palette,0,"Bot Names");
	for (n=0;n!=max_net_bot;n++) {
		sprintf(str,"Bot %d",(n+1));
		list_palette_add_string(&property_palette,(kMPPropertyBotName+n),str,iface.multiplayer.bot_list.bots[n].name,name_str_len,FALSE);
	}

		// news

	list_palette_add_header(&property_palette,0,"News");
	list_palette_add_string(&property_palette,kMPPropertyNewsHost,"Host",iface.multiplayer.news.host,64,FALSE);
	list_palette_add_string(&property_palette,kMPPropertyNewsURL,"URL",iface.multiplayer.news.url,256,FALSE);
	list_palette_add_int(&property_palette,kMPPropertyNewsPort,"Port",&iface.multiplayer.news.port,FALSE);
}

/* =======================================================

      Property Palette Click Multiplayer
      
======================================================= */

void property_palette_click_multiplayer(bool double_click)
{
	int							id,idx,sz;
	iface_mp_game_type			*mp_game;
	iface_mp_option_type		*mp_option;
	iface_mp_character_type		*mp_character;

	id=property_palette.item_pane.click.id;

		// selects

	if ((id>=kMPPropertyGameName) && (id<(kMPPropertyGameName+max_net_game))) {
		state.cur_multiplayer_character_idx=-1;
		state.cur_multiplayer_game_idx=id-kMPPropertyGameName;
		state.cur_multiplayer_option_idx=-1;
		if (double_click) list_palette_set_level(&property_palette,2);
		return;
	}

	if ((id>=kMPPropertyOptionName) && (id<(kMPPropertyOptionName+max_net_option))) {
		state.cur_multiplayer_character_idx=-1;
		state.cur_multiplayer_game_idx=-1;
		state.cur_multiplayer_option_idx=id-kMPPropertyOptionName;
		if (double_click) list_palette_set_level(&property_palette,2);
		return;
	}

	if ((id>=kMPPropertyCharacterName) && (id<(kMPPropertyCharacterName+max_character))) {
		state.cur_multiplayer_character_idx=id-kMPPropertyCharacterName;
		state.cur_multiplayer_game_idx=-1;
		state.cur_multiplayer_option_idx=-1;
		if (double_click) list_palette_set_level(&property_palette,2);
		return;
	}

		// delete

	if ((id>=kMPPropertyGameDelete) && (id<(kMPPropertyGameDelete+max_net_game))) {
		state.cur_multiplayer_character_idx=-1;
		state.cur_multiplayer_game_idx=-1;
		state.cur_multiplayer_option_idx=-1;

		idx=id-kMPPropertyGameDelete;

		sz=(iface.multiplayer.game_list.ngame-idx)-1;
		if (sz>0) memmove(&iface.multiplayer.game_list.games[idx],&iface.multiplayer.game_list.games[idx+1],(sz*sizeof(iface_mp_game_type)));

		iface.multiplayer.game_list.ngame--;

		return;
	}

	if ((id>=kMPPropertyOptionDelete) && (id<(kMPPropertyOptionDelete+max_net_option))) {
		state.cur_multiplayer_character_idx=-1;
		state.cur_multiplayer_game_idx=-1;
		state.cur_multiplayer_option_idx=-1;

		idx=id-kMPPropertyOptionDelete;

		sz=(iface.multiplayer.option_list.noption-idx)-1;
		if (sz>0) memmove(&iface.multiplayer.option_list.options[idx],&iface.multiplayer.option_list.options[idx+1],(sz*sizeof(iface_mp_option_type)));

		iface.multiplayer.option_list.noption--;

		return;
	}

	if ((id>=kMPPropertyCharacterDelete) && (id<(kMPPropertyCharacterDelete+max_character))) {
		state.cur_multiplayer_character_idx=-1;
		state.cur_multiplayer_game_idx=-1;
		state.cur_multiplayer_option_idx=-1;

		idx=id-kMPPropertyCharacterDelete;

		sz=(iface.multiplayer.character_list.ncharacter-idx)-1;
		if (sz>0) memmove(&iface.multiplayer.character_list.characters[idx],&iface.multiplayer.character_list.characters[idx+1],(sz*sizeof(iface_mp_character_type)));

		iface.multiplayer.character_list.ncharacter--;

		return;
	}

		// add

	if (id==kMPPropertyGameAdd) {
		state.cur_multiplayer_character_idx=-1;
		state.cur_multiplayer_game_idx=-1;
		state.cur_multiplayer_option_idx=-1;

		if (iface.multiplayer.game_list.ngame>=max_net_game) {
			os_dialog_alert("Add Multiplayer Game","Reached the maximum number of multiplayer games");
			return;
		}

		idx=iface.multiplayer.game_list.ngame;
		iface.multiplayer.game_list.ngame++;

		mp_game=&iface.multiplayer.game_list.games[idx];
		
		mp_game->name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)mp_game->name,name_str_len,0,0);

		mp_game->use_teams=FALSE;
		mp_game->monsters=FALSE;
		
		strcpy(mp_game->script.player_script,"Player");
		strcpy(mp_game->script.bot_script,"Bot DM");
		
		strcpy(mp_game->spawn.spot_name,"Spawn");
		mp_game->spawn.force_team_spot=FALSE;
		
		mp_game->score.kill=1;
		mp_game->score.death=0;
		mp_game->score.suicide=-1;
		mp_game->score.goal=1;

		state.cur_multiplayer_game_idx=idx;

		return;
	}

	if (id==kMPPropertyOptionAdd) {
		state.cur_multiplayer_character_idx=-1;
		state.cur_multiplayer_game_idx=-1;
		state.cur_multiplayer_option_idx=-1;

		if (iface.multiplayer.option_list.noption>=max_net_option) {
			os_dialog_alert("Add Multiplayer Option","Reached the maximum number of multiplayer options");
			return;
		}

		idx=iface.multiplayer.option_list.noption;
		iface.multiplayer.option_list.noption++;
		
		mp_option=&iface.multiplayer.option_list.options[idx];

		mp_option->name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)mp_option->name,name_str_len,0,0);

		mp_option->descript[0]=0x0;

		state.cur_multiplayer_option_idx=idx;

		return;
	}

	if (id==kMPPropertyCharacterAdd) {
		state.cur_multiplayer_character_idx=-1;
		state.cur_multiplayer_game_idx=-1;
		state.cur_multiplayer_option_idx=-1;

		if (iface.multiplayer.character_list.ncharacter>=max_character) {
			os_dialog_alert("Add Multiplayer Character","Reached the maximum number of characters");
			return;
		}

		idx=iface.multiplayer.character_list.ncharacter;
		iface.multiplayer.character_list.ncharacter++;

		mp_character=&iface.multiplayer.character_list.characters[idx];
		
		mp_character->name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)mp_character->name,name_str_len,0,0);

		mp_character->model_name[0]=0x0;
		mp_character->param[0]=0x0;
		mp_character->interface_resize=1.0f;
		mp_character->interface_offset.x=0;
		mp_character->interface_offset.y=0;
		mp_character->interface_offset.z=0;

		state.cur_multiplayer_character_idx=idx;

		return;
	}

}

