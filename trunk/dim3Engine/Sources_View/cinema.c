/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Cinemas

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

#include "scripts.h"
#include "interfaces.h"
#include "video.h"
#include "inputs.h"

extern server_type			server;
extern setup_type			setup;
extern js_type				js;
extern network_setup_type	net_setup;

int							cinema_last_state,cinema_event_id;

/* =======================================================

      Cinema Operations
      
======================================================= */

void cinema_open(void)
{
}

void cinema_close(void)
{
}

bool cinema_setup(char *name,int event_id,char *err_str)
{
		// remember last state and event

	cinema_last_state=server.next_state;
	cinema_event_id=event_id;
	
		// switch to cinema state
		
	server.next_state=gs_cinema;
	
	return(TRUE);
}

/* =======================================================

      Draw Cinema
      
======================================================= */

void cinema_draw(void)
{
		// start the frame
	
	gl_frame_clear(FALSE);

/*
	gl_2D_view_screen();
	
	gl_texture_movie_start(movie_bitmap_gl_id);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
			
		// draw movie
		
    glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2i(movie_x_offset,movie_y_offset);
	glTexCoord2f(1,0);
	glVertex2i((movie_x_offset+movie_x_sz),movie_y_offset);
	glTexCoord2f(1,1);
	glVertex2i((movie_x_offset+movie_x_sz),(movie_y_offset+movie_y_sz));
	glTexCoord2f(0,1);
	glVertex2i(movie_x_offset,(movie_y_offset+movie_y_sz));
    glEnd();
    
	gl_texture_movie_end();
	glEnable(GL_DEPTH_TEST);
*/
	gl_frame_swap();
}

/* =======================================================

      Run Cinema
      
======================================================= */

void cinema_run(void)
{
	bool				cinema_done;
	obj_type			*obj;

		// draw cinema

	cinema_draw();

		// check if cinema is over

	cinema_done=FALSE;
	
		// cinema over or mouse button pushed?
	
	if ((cinema_done) || (input_gui_get_mouse_left_button_down())) {

			// send the finish event

		if (cinema_event_id!=-1) {
			scripts_post_event_console(&js.game_attach,sd_event_interface,sd_event_interface_cinema_done,cinema_event_id);
			scripts_post_event_console(&js.course_attach,sd_event_map,sd_event_interface_cinema_done,cinema_event_id);

			if (net_setup.mode!=net_mode_host_dedicated) {
				obj=server.obj_list.objs[server.player_obj_idx];
				scripts_post_event_console(&obj->attach,sd_event_map,sd_event_interface_cinema_done,cinema_event_id);
			}
		}

		server.next_state=cinema_last_state;
	}
}


