/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Server MainLine

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
#include "scripts.h"
#include "objects.h"

map_type					map;
server_type					server;
iface_type					iface;
js_type						js;

extern setup_type			setup;
extern network_setup_type	net_setup;

extern void server_run(void);

/* =======================================================

      Server Memory
      
======================================================= */

/* =======================================================

      Server Initialize and Shutdown
      
======================================================= */

bool server_initialize(char *err_str)
{
		// clear server, js, hud structures
		
	memset(&server,0x0,sizeof(server_type));
	memset(&iface,0x0,sizeof(iface_type));
	memset(&js,0x0,sizeof(js_type));
	
		// allocate memory
		
	if (!iface_initialize(&iface,&setup.file_path_setup)) {
		iface_shutdown(&iface);
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
		// start script engine
		
	if (!scripts_engine_initialize(err_str)) {
		iface_shutdown(&iface);
		return(FALSE);
	}
	
		// load project XML
		
	iface_read(&iface);
	
		// game states
		
	server.game_open=FALSE;
	server.map_open=FALSE;

	server.map_change=FALSE;
	server.skip_media=FALSE;
	
	return(TRUE);
}

void server_shutdown(void)
{
		// shutdown script engine
		
	scripts_engine_shutdown();
	
		// release memory
	
	iface_shutdown(&iface);
}

/* =======================================================

      Server Game Start and Stop
      
======================================================= */

bool server_game_start(bool in_file_load,int skill,int simple_save_idx,char *err_str)
{
		// initialize lists

	model_initialize_list();
	object_initialize_list();
	
	scripts_initialize_list();
	script_global_initialize_list();
	timers_initialize_list();

		// setup skill and simple save index

	server.skill=skill;
	server.simple_save_idx=simple_save_idx;
	
		// run game script

	map.info.name[0]=0x0;
	map.info.player_start_name[0]=0x0;
	
	server.player_obj_idx=-1;
	
	scripts_clear_attach(&js.game_attach,thing_type_game);
	
	if (!scripts_add(&js.game_attach,"Game","Game",err_str)) return(FALSE);

		// if not reloading, then check that
		// a map was set

	if (!in_file_load) {

			// editor map override?
			
		if (setup.editor_override.on) {
			strcpy(map.info.name,setup.editor_override.map);
		}
		
			// can't start a game without a map
		
		if (map.info.name[0]==0x0) {
			strcpy(err_str,"Game: No start map specified in game script");
			return(FALSE);
		}
	}

		// create game player object

	if (net_setup.mode!=net_mode_host_dedicated) {
		server.player_obj_idx=game_player_create(err_str);
		if (server.player_obj_idx==-1) {
			scripts_dispose(js.game_attach.script_idx);
			return(FALSE);
		}
	}
	
	return(TRUE);
}

void server_game_stop(void)
{
		// dispose game bound objects

	object_dispose_2(bt_game);

		// dispose game script

	scripts_dispose(js.game_attach.script_idx);

		// finish with list frees

	object_free_list();
	model_free_list();

	timers_free_list();
	script_global_free_list();
	scripts_free_list();
}

/* =======================================================

      Server Loop
      
======================================================= */

void server_loop(void)
{
	timers_run();
	server_run();
}
