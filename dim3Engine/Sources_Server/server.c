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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"
#include "objects.h"
#include "remotes.h"
#include "weapons.h"
#include "models.h"
#include "lights.h"
#include "effects.h"
#include "projectiles.h"
#include "interfaces.h"
#include "xmls.h"
#include "video.h"

map_type					map;
server_type					server;
js_type						js;
hud_type					hud;

extern setup_type			setup;
extern network_setup_type	net_setup;

extern void server_run(void);

/* =======================================================

      Server Memory
      
======================================================= */

bool server_memory_allocate(void)
{
		// initialize pointers
		
	server.particles=NULL;
 	server.rings=NULL;
	server.halos=NULL;
	server.marks=NULL;
	server.crosshairs=NULL;
	
	hud.bitmaps=NULL;
	hud.texts=NULL;
	hud.bars=NULL;
	hud.menus=NULL;
	hud.choosers=NULL;
	hud.chat.lines=NULL;

		// hud pointers

	hud.bitmaps=(hud_bitmap_type*)malloc(max_hud_bitmap*sizeof(hud_bitmap_type));
	if (hud.bitmaps==NULL) return(FALSE);
	
	hud.texts=(hud_text_type*)malloc(max_hud_text*sizeof(hud_text_type));
	if (hud.texts==NULL) return(FALSE);
	
	hud.bars=(hud_bar_type*)malloc(max_hud_bar*sizeof(hud_bar_type));
	if (hud.bars==NULL) return(FALSE);

	hud.menus=(menu_type*)malloc(max_menu*sizeof(menu_type));
	if (hud.menus==NULL) return(FALSE);
	
	hud.choosers=(chooser_type*)malloc(max_chooser*sizeof(chooser_type));
	if (hud.choosers==NULL) return(FALSE);
	
	hud.chat.lines=(hud_chat_line_type*)malloc(max_chat_lines*sizeof(hud_chat_line_type));
	if (hud.chat.lines==NULL) return(FALSE);
	
		// zero memory
		
	bzero(hud.bitmaps,(max_hud_bitmap*sizeof(hud_bitmap_type)));
	bzero(hud.texts,(max_hud_text*sizeof(hud_text_type)));
	bzero(hud.bars,(max_hud_bar*sizeof(hud_bar_type)));
	bzero(hud.menus,(max_menu*sizeof(menu_type)));
	bzero(hud.choosers,(max_chooser*sizeof(chooser_type)));
	bzero(hud.chat.lines,(max_chat_lines*sizeof(hud_chat_line_type)));

	return(TRUE);
}

void server_memory_release(void)
{
		// catch all for dynamic server pointers
		
	if (server.particles!=NULL) free(server.particles);
	if (server.rings!=NULL) free(server.rings);
	if (server.halos!=NULL) free(server.halos);
	if (server.marks!=NULL) free(server.marks);
	if (server.crosshairs!=NULL) free(server.crosshairs);
	
		// hud pointers
		
	if (hud.bitmaps!=NULL) free(hud.bitmaps);
	if (hud.texts!=NULL) free(hud.texts);
	if (hud.bars!=NULL) free(hud.bars);
	if (hud.menus!=NULL) free(hud.menus);
	if (hud.choosers!=NULL) free(hud.choosers);
	if (hud.chat.lines!=NULL) free(hud.chat.lines);
}

/* =======================================================

      Server Initialize and Shutdown
      
======================================================= */

bool server_initialize(char *err_str)
{
		// clear server, js, hud structures
		
	memset(&server,0x0,sizeof(server_type));
	memset(&js,0x0,sizeof(js_type));
	memset(&hud,0x0,sizeof(hud_type));
	
		// allocate memory
		
	if (!server_memory_allocate()) {
		server_memory_release();
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
		// start script engine
		
	if (!scripts_engine_initialize(err_str)) {
		server_memory_release();
		return(FALSE);
	}
	
		// initialize project settings
		
	hud_initialize();
	particle_initialize();
	ring_initialize();
	halo_initialize();
	mark_initialize();
	crosshair_initialize();
	
		// load project XML
		
	read_settings_interface();
	read_settings_particle();
	read_settings_ring();
	read_settings_halo();
	read_settings_mark();
	read_settings_crosshair();
	
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
		
	server_memory_release();
}

/* =======================================================

      Server Game Start and Stop
      
======================================================= */

bool server_game_start(char *game_script_name,int skill,char *err_str)
{
		// initialize lists

	model_initialize();
		
	object_initialize_list();
	
	scripts_initialize_list();
	script_global_initialize_list();
	timers_initialize_list();

		// setup skill level

	server.skill=skill;
	
		// run game script

	map.info.name[0]=0x0;
	map.info.player_start_name[0]=0x0;
	
	server.player_obj_idx=-1;
	
	js.game_attach.thing_type=thing_type_game;

	scripts_clear_attach_data(&js.game_attach);
	
	if (!scripts_add(&js.game_attach,"Game",game_script_name,NULL,err_str)) return(FALSE);

		// editor map override?
		
	if (setup.editor_override.on) {
		strcpy(map.info.name,setup.editor_override.map);
	}
	
		// can't start a game without a map
	
	if (map.info.name[0]==0x0) {
		strcpy(err_str,"Game: No start map specified in game script");
		return(FALSE);
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
