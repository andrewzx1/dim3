/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Titles

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

#define title_fade_mode_in		0
#define title_fade_mode_out		1
#define title_fade_mode_none	2

extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern js_type				js;

int							title_fade_tick,title_fade_mode,title_event_id,title_last_state,
							title_start_tick,title_life_tick;
char						title_dir[name_str_len],title_name[name_str_len],title_sound_name[name_str_len];

/* =======================================================

      Title Operations
      
======================================================= */

void title_open(void)
{
	int			buffer_idx;
	
		// remember last state
		
	title_last_state=server.last_state;

		// titles
		
	gui_initialize(title_dir,title_name);
	
		// sound

	if (title_sound_name[0]!=0x0) {
		buffer_idx=al_find_buffer(title_sound_name);
		al_play_source(buffer_idx,NULL,1.0f,FALSE,FALSE,TRUE,FALSE);
	}

		// fades
		
	title_fade_tick=-1;
	title_fade_mode=title_fade_mode_in;
}

void title_close(void)
{
	gui_shutdown();

	if (title_event_id!=-1) scripts_post_event_console(js.game_script_idx,-1,sd_event_interface,sd_event_interface_title_done,title_event_id);
}

bool title_setup(char *dir,char *name,char *sound_name,int life_tick,int event_id,char *err_str)
{
	char			path[1024];
	struct stat		sb;

	strcpy(title_dir,dir);
	strcpy(title_name,name);
	if (sound_name!=NULL) {
		strcpy(title_sound_name,sound_name);
	}
	else {
		title_sound_name[0]=0x0;
	}
	title_event_id=event_id;

	title_start_tick=game_time_get_raw();
	title_life_tick=life_tick;
	
		// does title exist?
		
	file_paths_data(&setup.file_path_setup,path,dir,name,"png");
	if (stat(path,&sb)==-1) {
		sprintf(err_str,"Title does not exist: %s.png",name);
		return(FALSE);
	}
	
		// switch to title state
		
	server.next_state=gs_title;
	
	return(TRUE);
}

/* =======================================================

      Run Title Page
      
======================================================= */

void title_run(void)
{
	int				tick,raw_tick;
	float			alpha;

		// is the automatic close on?

	raw_tick=game_time_get_raw();

	if ((title_life_tick!=-1) && (title_fade_mode==title_fade_mode_none)) {
		if ((title_start_tick+title_life_tick)<raw_tick) {
			title_fade_tick=-1;
			title_fade_mode=title_fade_mode_out;
		}
	}
	
		// get the fade

	alpha=1.0f;

	switch (title_fade_mode) {

		case title_fade_mode_in:
			if (title_fade_tick==-1) title_fade_tick=raw_tick;
			
			tick=raw_tick-title_fade_tick;
			if (tick>iface.fade.title_msec) {
				title_fade_mode=title_fade_mode_none;
				title_start_tick=raw_tick;
				break;
			}
			
			alpha=((float)tick)/(float)iface.fade.title_msec;
			break;

		case title_fade_mode_out:
			if (title_fade_tick==-1) title_fade_tick=raw_tick;
			
			tick=raw_tick-title_fade_tick;
			if (tick>iface.fade.title_msec) {
				server.next_state=title_last_state;
				return;
			}
			
			alpha=1.0f-(((float)tick)/(float)iface.fade.title_msec);
			break;
	}

		// run the GUI

	gui_draw(alpha,FALSE);
	
		// the input
		
	if ((title_fade_mode==title_fade_mode_none) && (input_gui_get_mouse_left_button_down())) {
		title_fade_tick=-1;
		title_fade_mode=title_fade_mode_out;
	}
}


