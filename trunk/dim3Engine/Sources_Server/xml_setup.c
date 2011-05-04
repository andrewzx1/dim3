/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Setup XML

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
	#include "dim3engine.h"
#endif

#include "interface.h"

extern network_setup_type	net_setup;

setup_type					setup;

/* =======================================================

      Default Preferences
      
======================================================= */

void setup_xml_default(void)
{
	setup.screen_wid=640;
	setup.screen_high=480;
	
	setup.anisotropic_mode=anisotropic_mode_none;
	setup.mipmap_mode=mipmap_mode_trilinear;
	setup.texture_quality_mode=texture_quality_mode_normal;
	setup.fsaa_mode=fsaa_mode_none;
	
	setup.decal_on=TRUE;
	setup.shadow_on=TRUE;
	
	setup.gamma=0.0f;
	
	setup.sound_volume=0.6f;
	setup.music_on=TRUE;
	setup.music_volume=0.5f;
    
	setup.always_run=FALSE;
	setup.toggle_run=FALSE;
	setup.invert_look=FALSE;
	setup.mouse_smooth=TRUE;

	setup.mouse.speed=0.03f;
	setup.mouse.acceleration=0.6f;

	setup.joystick.speed=0.03f;
	setup.joystick.acceleration=0.6f;

	setup.action_list.naction=0;

	strcpy(setup.network.name,"Player");
	setup.network.character_idx=0;
	setup.network.tint_color_idx=0;
	setup.network.show_names=TRUE;
	setup.network.dedicated=FALSE;
	
	setup.network.host.count=0;
	
	setup.network.bot.count=0;
	setup.network.bot.skill=2;
	
	setup.network.map.count=0;
	setup.network.option.count=0;

	setup.network.game_type=0;
	setup.network.score_limit=20;
	setup.network.game_reset_secs=15;
	setup.network.respawn_secs=5;
	
	setup.debug_console=FALSE;
	setup.window=FALSE;
	setup.window_editor=TRUE;
	setup.metrics_on=FALSE;
	setup.disable_shaders=FALSE;
}

/* =======================================================

      Read Setup
      
======================================================= */

bool setup_xml_read_path(char *path)
{
	int							n,k,naction,nhost,
								setup_tag,actions_tag,hosts_tag,maps_tag,options_tag,tag;
	char						tag_name[32];
	setup_action_type			*action;
	setup_network_host_type		*host;
	
		// read file
		
	setup_xml_default();

	if (!xml_open_file(path)) return(FALSE);
    
		// decode the file
       
    setup_tag=xml_findrootchild("Setup");
    if (setup_tag==-1) {
		xml_close_file();
		return(FALSE);
    }
	
		// keys

    xml_key_read_int(setup_tag,"Screen_Width",&setup.screen_wid);
    xml_key_read_int(setup_tag,"Screen_Height",&setup.screen_high);
	xml_key_read_float(setup_tag,"Gamma",&setup.gamma);
    xml_key_read_int(setup_tag,"Anisotropic_Mode",&setup.anisotropic_mode);
    xml_key_read_int(setup_tag,"Texture_Quality_Mode",&setup.texture_quality_mode);
    xml_key_read_int(setup_tag,"Mipmap_Mode",&setup.mipmap_mode);
	xml_key_read_int(setup_tag,"FSAA_Mode",&setup.fsaa_mode);
	xml_key_read_boolean(setup_tag,"Decal_On",&setup.decal_on);
	xml_key_read_boolean(setup_tag,"Shadow_On",&setup.shadow_on);
	xml_key_read_float(setup_tag,"Sound_Volume",&setup.sound_volume);
	xml_key_read_boolean(setup_tag,"Music_On",&setup.music_on);
	xml_key_read_float(setup_tag,"Music_Volume",&setup.music_volume);
	xml_key_read_boolean(setup_tag,"Always_Run",&setup.always_run);
	xml_key_read_boolean(setup_tag,"Toggle_Run",&setup.toggle_run);
	xml_key_read_boolean(setup_tag,"Invert_Look",&setup.invert_look);
	xml_key_read_boolean(setup_tag,"Mouse_Smooth",&setup.mouse_smooth);
	xml_key_read_float(setup_tag,"Mouse_Speed",&setup.mouse.speed);
	xml_key_read_float(setup_tag,"Mouse_Acceleration",&setup.mouse.acceleration);
	xml_key_read_float(setup_tag,"Joystick_Speed",&setup.joystick.speed);
	xml_key_read_float(setup_tag,"Joystick_Acceleration",&setup.joystick.acceleration);
	xml_key_read_text(setup_tag,"Network_Name",setup.network.name,name_str_len);
    xml_key_read_int(setup_tag,"Character",&setup.network.character_idx);
    xml_key_read_int(setup_tag,"Tint",&setup.network.tint_color_idx);
	xml_key_read_int(setup_tag,"Host_Bot_Count",&setup.network.bot.count);
	xml_key_read_int(setup_tag,"Host_Bot_Skill",&setup.network.bot.skill);
	xml_key_read_int(setup_tag,"Host_Game_Type",&setup.network.game_type);
	xml_key_read_int(setup_tag,"Host_Score_Limit",&setup.network.score_limit);
	xml_key_read_int(setup_tag,"Host_Game_Reset_Secs",&setup.network.game_reset_secs);
	xml_key_read_int(setup_tag,"Host_Respawn_Secs",&setup.network.respawn_secs);
	xml_key_read_boolean(setup_tag,"Network_Show_Names",&setup.network.show_names);
	xml_key_read_boolean(setup_tag,"Network_Dedicated",&setup.network.dedicated);
	xml_key_read_boolean(setup_tag,"Debug_Console",&setup.debug_console);
	xml_key_read_boolean(setup_tag,"Window",&setup.window);
	xml_key_read_boolean(setup_tag,"Window_Editor",&setup.window_editor);
	xml_key_read_boolean(setup_tag,"Metrics_On",&setup.metrics_on);
	xml_key_read_boolean(setup_tag,"Disable_Shaders",&setup.disable_shaders);

		// actions

    actions_tag=xml_findfirstchild("Actions",setup_tag);
    if (actions_tag!=-1) {
	
		naction=xml_countchildren(actions_tag);
		tag=xml_findfirstchild("Action",actions_tag);
		
		setup.action_list.naction=naction;
		action=setup.action_list.actions;
		
        for (n=0;n!=naction;n++) {
			xml_get_attribute_text(tag,"name",action->name,32);
			
			for (k=0;k!=max_setup_action_attach;k++) {
				sprintf(tag_name,"attach_%d",k);
				xml_get_attribute_text(tag,tag_name,action->attach[k],32);
			}
           
			tag=xml_findnextchild(tag);
			
			action++;
        }
	}

		// hosts

    hosts_tag=xml_findfirstchild("Hosts",setup_tag);
    if (hosts_tag!=-1) {
	
		nhost=xml_countchildren(hosts_tag);
		tag=xml_findfirstchild("Host",hosts_tag);
		
		setup.network.host.count=nhost;
		host=setup.network.host.hosts;
		
        for (n=0;n!=nhost;n++) {
			xml_get_attribute_text(tag,"name",host->name,64);
			xml_get_attribute_text(tag,"ip",host->ip,256);

			tag=xml_findnextchild(tag);
			host++;
        }
	}

 		// maps
		
    maps_tag=xml_findfirstchild("Maps",setup_tag);
    if (maps_tag!=-1) {
	
		setup.network.map.count=xml_countchildren(maps_tag);
		tag=xml_findfirstchild("Map",maps_tag);
		
        for (n=0;n!=setup.network.map.count;n++) {
			xml_get_attribute_text(tag,"name",setup.network.map.maps[n].name,name_str_len);
			tag=xml_findnextchild(tag);
 		}
	}
	
 		// options
		
    options_tag=xml_findfirstchild("Options",setup_tag);
    if (options_tag!=-1) {
	
		setup.network.option.count=xml_countchildren(options_tag);
		tag=xml_findfirstchild("Option",options_tag);
		
        for (n=0;n!=setup.network.option.count;n++) {
			xml_get_attribute_text(tag,"name",setup.network.option.options[n].name,name_str_len);
			tag=xml_findnextchild(tag);
 		}
	}
  
	xml_close_file();
	
	return(TRUE);
}

bool setup_xml_read(void)
{
	char				path[1024];
	
		// check user specific setup XML file.  If it exists, use that,
		// otherwise use default XML file
		
	if (!file_paths_documents_exist(&setup.file_path_setup,path,"Settings","Setup","xml")) {
		file_paths_data(&setup.file_path_setup,path,"Settings","Setup","xml");
	}

	return(setup_xml_read_path(path));
}

bool setup_xml_reset(void)
{
	char				path[1024];
	
		// read directly from the default
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Setup","xml");

	return(setup_xml_read_path(path));
}

/* =======================================================

      Write Setup
      
======================================================= */

bool setup_xml_write(void)
{
	int							n,k;
	char						path[1024],tag_name[32];
	bool						ok;
	setup_action_type			*action;
	setup_network_host_type		*host;
	
		// start the setup file
		
    xml_new_file();
    
    xml_add_tagstart("Setup");
    xml_add_tagend(FALSE);
	
		// keys

    xml_key_write_int("Screen_Width",setup.screen_wid);
    xml_key_write_int("Screen_Height",setup.screen_high);
	xml_key_write_float("Gamma",setup.gamma);
	xml_key_write_int("Anisotropic_Mode",setup.anisotropic_mode);
    xml_key_write_int("Texture_Quality_Mode",setup.texture_quality_mode);
    xml_key_write_int("Mipmap_Mode",setup.mipmap_mode);
	xml_key_write_int("FSAA_Mode",setup.fsaa_mode);
	xml_key_write_boolean("Decal_On",setup.decal_on);
	xml_key_write_boolean("Shadow_On",setup.shadow_on);
	xml_key_write_float("Sound_Volume",setup.sound_volume);
	xml_key_write_boolean("Music_On",setup.music_on);
	xml_key_write_float("Music_Volume",setup.music_volume);
	xml_key_write_boolean("Always_Run",setup.always_run);
	xml_key_write_boolean("Toggle_Run",setup.toggle_run);
	xml_key_write_boolean("Invert_Look",setup.invert_look);
	xml_key_write_boolean("Mouse_Smooth",setup.mouse_smooth);
	xml_key_write_float("Mouse_Speed",setup.mouse.speed);
	xml_key_write_float("Mouse_Acceleration",setup.mouse.acceleration);
	xml_key_write_float("Joystick_Speed",setup.joystick.speed);
	xml_key_write_float("Joystick_Acceleration",setup.joystick.acceleration);
	xml_key_write_text("Network_Name",setup.network.name);
	xml_key_write_int("Character",setup.network.character_idx);
	xml_key_write_int("Tint",setup.network.tint_color_idx);
	xml_key_write_int("Host_Bot_Count",setup.network.bot.count);
	xml_key_write_int("Host_Bot_Skill",setup.network.bot.skill);
	xml_key_write_int("Host_Game_Type",setup.network.game_type);
	xml_key_write_int("Host_Score_Limit",setup.network.score_limit);
	xml_key_write_int("Host_Game_Reset_Secs",setup.network.game_reset_secs);
	xml_key_write_int("Host_Respawn_Secs",setup.network.respawn_secs);
	xml_key_write_boolean("Network_Show_Names",setup.network.show_names);
	xml_key_write_boolean("Network_Dedicated",setup.network.dedicated);
	xml_key_write_boolean("Debug_Console",setup.debug_console);
	xml_key_write_boolean("Window",setup.window);
	xml_key_write_boolean("Window_Editor",setup.window_editor);
	xml_key_write_boolean("Metrics_On",setup.metrics_on);
	xml_key_write_boolean("Disable_Shaders",setup.disable_shaders);
	
		// actions

    xml_add_tagstart("Actions");
    xml_add_tagend(FALSE);
	
	action=setup.action_list.actions;
		
	for (n=0;n!=setup.action_list.naction;n++) {
		xml_add_tagstart("Action");
		xml_add_attribute_text("name",action->name);
			
		for (k=0;k!=max_setup_action_attach;k++) {
			if (action->attach[k][0]!=0x0) {
				sprintf(tag_name,"attach_%d",k);
				xml_add_attribute_text(tag_name,action->attach[k]);
			}
		}
           
	    xml_add_tagend(TRUE);
		action++;
	}

    xml_add_tagclose("Actions");
	
		// hosts

    xml_add_tagstart("Hosts");
    xml_add_tagend(FALSE);
	
	host=setup.network.host.hosts;
		
	for (n=0;n!=setup.network.host.count;n++) {
		xml_add_tagstart("Host");
		xml_add_attribute_text("name",host->name);
		xml_add_attribute_text("ip",host->ip);
	    xml_add_tagend(TRUE);
		host++;
	}

    xml_add_tagclose("Hosts");

		// maps
		
    xml_add_tagstart("Maps");
    xml_add_tagend(FALSE);
	
	for (n=0;n!=setup.network.map.count;n++) {
		xml_add_tagstart("Map");
		xml_add_attribute_text("name",setup.network.map.maps[n].name);
		xml_add_tagend(TRUE);
	}

    xml_add_tagclose("Maps");
	
		// options
		
    xml_add_tagstart("Options");
    xml_add_tagend(FALSE);
	
	for (n=0;n!=setup.network.option.count;n++) {
		xml_add_tagstart("Option");
		xml_add_attribute_text("name",setup.network.option.options[n].name);
		xml_add_tagend(TRUE);
	}

    xml_add_tagclose("Options");
		
		// close setup
		
    xml_add_tagclose("Setup");

        // save the setup
		// always save to user specific data
		
	file_paths_documents(&setup.file_path_setup,path,"Settings","Setup","xml");
		
	ok=xml_save_file(path);
    xml_close_file();
	
	return(ok);
}

/* =======================================================

      Restore Setup on Bad Startup
      
======================================================= */

void setup_restore(void)
{
		// reset important defaults
		// to their simpliest form
		
	setup.screen_wid=640;
	setup.screen_high=480;
	
	setup.decal_on=FALSE;
	setup.shadow_on=FALSE;
	
	setup.anisotropic_mode=anisotropic_mode_none;
	setup.mipmap_mode=mipmap_mode_none;
	setup.texture_quality_mode=texture_quality_mode_low;
	setup.fsaa_mode=fsaa_mode_none;
	
	setup.music_on=FALSE;

	setup.debug_console=TRUE;
	setup.window=FALSE;
	setup.window_editor=TRUE;
	setup.metrics_on=FALSE;
	setup.disable_shaders=FALSE;
	
		// save XML
		
	setup_xml_write();
}

