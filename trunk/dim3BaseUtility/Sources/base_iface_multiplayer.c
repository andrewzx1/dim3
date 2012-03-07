/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Multiplayer XML

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
	#include "dim3baseutility.h"
#endif

extern file_path_setup_type		iface_file_path_setup;

/* =======================================================

      Read Multiplayer XML
      
======================================================= */

void iface_read_settings_multiplayer(iface_type *iface)
{
	int							cnt,multiplayer_head_tag,
								games_head_tag,game_tag,tag,
								options_head_tag,option_tag,
								character_head_tag,character_item_tag,
								bot_head_tag,bot_tag,news_tag;
	char						path[1024];
	iface_net_game_type			*game;
	iface_character_item_type	*iface_character;

		// check multiplayer file first, if it doesn't
		// exist look in interface.xml
		// supergumba -- remove later
		
	file_paths_data(&iface_file_path_setup,path,"Settings","Multiplayer","xml");
	if (xml_open_file(path)) {
		multiplayer_head_tag=xml_findrootchild("Multiplayer");
	}
	else {
		file_paths_data(&iface_file_path_setup,path,"Settings","Interface","xml");
		if (!xml_open_file(path)) return;

		multiplayer_head_tag=xml_findrootchild("Interface");
	}
		
    if (multiplayer_head_tag==-1) {
		xml_close_file();
		return;
	}

		// network games

	games_head_tag=xml_findfirstchild("Games",multiplayer_head_tag);
	if (games_head_tag!=-1) {
		
		iface->net_game.ngame=0;
		
		game_tag=xml_findfirstchild("Game",games_head_tag);
		while (game_tag!=-1) {
		
			game=&iface->net_game.games[iface->net_game.ngame];
		
			xml_get_attribute_text(game_tag,"type",game->name,name_str_len);
			game->use_teams=xml_get_attribute_boolean(game_tag,"use_teams");
			game->monsters=xml_get_attribute_boolean(game_tag,"monsters");
			
			tag=xml_findfirstchild("Script",game_tag);
			if (tag!=-1) {
				xml_get_attribute_text(tag,"player",game->script.player_script,file_str_len);
				xml_get_attribute_text(tag,"bot",game->script.bot_script,file_str_len);
			}
			
			tag=xml_findfirstchild("Spawn",game_tag);
			if (tag!=-1) {
				xml_get_attribute_text(tag,"spot_name",game->spawn.spot_name,name_str_len);
				game->spawn.force_team_spot=xml_get_attribute_boolean(tag,"force_team_spot");
			}
			
			tag=xml_findfirstchild("Score",game_tag);
			if (tag!=-1) {
				game->score.kill=xml_get_attribute_int(tag,"kill");
				game->score.death=xml_get_attribute_int(tag,"death");
				game->score.suicide=xml_get_attribute_int(tag,"suicide");
				game->score.goal=xml_get_attribute_int(tag,"goal");
			}
			
			iface->net_game.ngame++;
			if (iface->net_game.ngame==max_net_game) break;

			game_tag=xml_findnextchild(game_tag);
		}
	}

		// network options

	options_head_tag=xml_findfirstchild("Options",multiplayer_head_tag);
	if (options_head_tag!=-1) {
		
		iface->net_option.noption=0;
		
		option_tag=xml_findfirstchild("Option",options_head_tag);
		while (option_tag!=-1) {
		
			xml_get_attribute_text(option_tag,"name",iface->net_option.options[iface->net_option.noption].name,name_str_len);
			xml_get_attribute_text(option_tag,"description",iface->net_option.options[iface->net_option.noption].descript,64);
			
			iface->net_option.noption++;
			if (iface->net_option.noption==max_net_option) break;

			option_tag=xml_findnextchild(option_tag);
		}
	}

		// characters

    character_head_tag=xml_findfirstchild("Characters",multiplayer_head_tag);
    if (character_head_tag!=-1) {

		iface->character.ncharacter=0;
		character_item_tag=xml_findfirstchild("Character",character_head_tag);
		
		while (character_item_tag!=-1) {
			iface_character=&iface->character.characters[iface->character.ncharacter];
			
			xml_get_attribute_text(character_item_tag,"name",iface_character->name,name_str_len);
			xml_get_attribute_text(character_item_tag,"model",iface_character->model_name,name_str_len);
			xml_get_attribute_text(character_item_tag,"parameter",iface_character->param,name_str_len);
			iface_character->interface_resize=xml_get_attribute_float_default(character_item_tag,"interface_resize",1.0f);
			xml_get_attribute_3_coord_int(character_item_tag,"interface_offset",&iface_character->interface_offset.x,&iface_character->interface_offset.y,&iface_character->interface_offset.z);
			
			iface->character.ncharacter++;
			character_item_tag=xml_findnextchild(character_item_tag);
		}
	}
	
		// bot names
	
    bot_head_tag=xml_findfirstchild("Bots",multiplayer_head_tag);
    if (bot_head_tag!=-1) {

		iface->net_bot.on=!xml_get_attribute_boolean(bot_head_tag,"hide");
	
		cnt=0;
		bot_tag=xml_findfirstchild("Bot",bot_head_tag);
		
		while (bot_tag!=-1) {
			xml_get_attribute_text(bot_tag,"name",iface->net_bot.bots[cnt].name,name_str_len);
			cnt++;
			bot_tag=xml_findnextchild(bot_tag);
		}
	}
	
		// news
		
    news_tag=xml_findfirstchild("News",multiplayer_head_tag);
    if (news_tag!=-1) {
		xml_get_attribute_text(news_tag,"host",iface->net_news.host,64);
		iface->net_news.port=xml_get_attribute_int_default(news_tag,"port",80);
		xml_get_attribute_text(news_tag,"url",iface->net_news.url,256);
	}

	xml_close_file();
}

/* =======================================================

      Write Multiplayer XML
      
======================================================= */

bool iface_write_settings_multiplayer(iface_type *iface,char *err_str)
{
	int							n;
	char						path[1024];
	bool						ok;
	iface_net_game_type			*game;
	iface_character_item_type	*iface_character;
	
		// start new file
		
	xml_new_file();

		// multiplayer

	xml_add_tagstart("Multiplayer");
	xml_add_tagend(FALSE);

		// network games

	xml_add_tagstart("Games");
	xml_add_tagend(FALSE);
	
	game=iface->net_game.games;

	for (n=0;n!=iface->net_game.ngame;n++) {
	
		xml_add_tagstart("Game");
		xml_add_attribute_text("type",game->name);
		xml_add_attribute_boolean("use_teams",game->use_teams);
		xml_add_attribute_boolean("monsters",game->monsters);
		xml_add_tagend(FALSE);
		
		xml_add_tagstart("Script");
		xml_add_attribute_text("player",game->script.player_script);
		xml_add_attribute_text("bot",game->script.bot_script);
		xml_add_tagend(TRUE);
			
		xml_add_tagstart("Spawn");
		xml_add_attribute_text("spot_name",game->spawn.spot_name);
		xml_add_attribute_boolean("force_team_spot",game->spawn.force_team_spot);
		xml_add_tagend(TRUE);
			
		xml_add_tagstart("Score");
		xml_add_attribute_int("kill",game->score.kill);
		xml_add_attribute_int("death",game->score.death);
		xml_add_attribute_int("suicide",game->score.suicide);
		xml_add_attribute_int("goal",game->score.goal);
		xml_add_tagend(TRUE);

		xml_add_tagclose("Game");
		
		game++;
	}

	xml_add_tagclose("Games");

		// network options

	xml_add_tagstart("Options");
	xml_add_tagend(FALSE);

	for (n=0;n!=iface->net_option.noption;n++) {
		xml_add_tagstart("Option");
		xml_add_attribute_text("name",iface->net_option.options[n].name);
		xml_add_attribute_text("description",iface->net_option.options[n].descript);
		xml_add_tagend(TRUE);
	}

	xml_add_tagclose("Options");

		// characters

	xml_add_tagstart("Characters");
	xml_add_tagend(FALSE);

	iface_character=iface->character.characters;

	for (n=0;n!=iface->character.ncharacter;n++) {
		xml_add_tagstart("Character");
		xml_add_attribute_text("name",iface_character->name);
		xml_add_attribute_text("model",iface_character->model_name);
		xml_add_attribute_text("parameter",iface_character->param);
		xml_add_attribute_float("interface_resize",iface_character->interface_resize);
		xml_add_attribute_3_coord_int("interface_offset",iface_character->interface_offset.x,iface_character->interface_offset.y,iface_character->interface_offset.z);
		xml_add_tagend(TRUE);

		iface_character++;
	}

	xml_add_tagclose("Characters");
	
		// bot names

	xml_add_tagstart("Bots");
	xml_add_attribute_boolean("hide",!iface->net_bot.on);
	xml_add_tagend(FALSE);

	for (n=0;n!=max_net_bot;n++) {
		if (iface->net_bot.bots[n].name[0]==0x0) continue;

		xml_add_tagstart("Bot");
		xml_add_attribute_text("name",iface->net_bot.bots[n].name);
		xml_add_tagend(TRUE);
	}

	xml_add_tagclose("Bots");

		// news
		
	xml_add_tagstart("News");
	xml_add_attribute_text("host",iface->net_news.host);
	xml_add_attribute_int("port",iface->net_news.port);
	xml_add_attribute_text("url",iface->net_news.url);
	xml_add_tagend(TRUE);

	xml_add_tagclose("Multiplayer");

        // write the xml
		
	file_paths_data(&iface_file_path_setup,path,"Settings","Multiplayer","xml");
		
	ok=xml_save_file(path,err_str);
    xml_close_file();
	
	return(ok);
}
