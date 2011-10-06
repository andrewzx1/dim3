/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: App Start/Stop

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
#include "network.h"

extern iface_type			iface;
extern server_type			server;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern bool server_initialize(char *err_str);
extern void server_shutdown(void);
extern bool view_initialize(char *err_str);
extern void view_shutdown(void);

/* =======================================================

      App Start and End
      
======================================================= */

bool app_start(char *err_str)
{
		// initialize timing

	game_time_initialize();
	
		// ray tracing structures
		
	if (!ray_trace_initialize(err_str)) return(FALSE);
	
		// OS network initialization
		
	net_initialize();
	
		// read setup preferences
		
	setup_xml_read();
	
		// client network defaults
		
	net_setup.mode=net_mode_none;
	
		// initialize server
		
	if (!server_initialize(err_str)) return(FALSE);

		// initialize view
	
	if (!view_initialize(err_str)) {
		server_shutdown();
		return(FALSE);
	}

	console_initialize();

		// if no editor launch,
		// start in intro or title media
		// we hard-set the state here as there
		// is no previous state
		
	if (!setup.editor_override.on) {

			// is there a title?

		if (iface.intro.title.name[0]!=0x0) {

			if (!title_setup("Titles",iface.intro.title.name,iface.intro.title.sound,iface.intro.title.life_msec,-1,err_str)) return(FALSE);

			server.state=gs_title;
			server.last_state=gs_intro;

			title_open();
			return(TRUE);
		}

			// if not go right to intro

		server.state=gs_intro;

		intro_open();

		return(TRUE);
	}
	
		// launch directly into map
		
	server.state=gs_running;
		
	if (!game_start(FALSE,skill_medium,0,err_str)) {
		view_shutdown();
		server_shutdown();
		return(FALSE);
	}
	
	if (!map_start(FALSE,TRUE,err_str)) {
		game_end();
		view_shutdown();
		server_shutdown();
		return(FALSE);
	}

	input_clear();
	
	return(TRUE);
}

void app_end(void)
{
	if (server.map_open) map_end();
	if (server.game_open) game_end();
	
		// shut down app
		
	console_add_system("Closing App");
	
		// shutdown view
		
	view_shutdown();
	
		// shutdown server
		
	server_shutdown();
	
		// ray trace cleanup
		
	ray_trace_shutdown();
	
		// OS network shutdown
		
	net_shutdown();
}

