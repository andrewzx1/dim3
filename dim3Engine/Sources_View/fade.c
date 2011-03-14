/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Fading

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

#include "consoles.h"
#include "video.h"
#include "interfaces.h"
#include "timing.h"

int							fade_screen_tick;

extern iface_type			iface;
extern setup_type			setup;

/* =======================================================

      Map Screen Fade
      
======================================================= */

void view_draw_fade_start(void)
{
	if (iface.fade.map_msec<=0) {
		fade_screen_tick=-1;
		return;
	}

	fade_screen_tick=game_time_get();
}

void view_draw_fade_cancel(void)
{
	fade_screen_tick=-1;
}

void view_draw_fade_draw(void)
{
	int				tick;
	float			alpha;
	d3col			col;

	if (fade_screen_tick==-1) return;

	tick=game_time_get()-fade_screen_tick;
	if (tick>iface.fade.map_msec) {
		fade_screen_tick=-1;
		return;
	}

		// calculate fade

	alpha=1.0f-(float)tick/(float)iface.fade.map_msec;
	glColor4f(0.0f,0.0f,0.0f,alpha);
	
		// draw
		
	gl_2D_view_screen();

	col.r=col.g=col.b=0.0f;
	view_draw_next_vertex_object_2D_color_quad(&col,alpha,0,setup.screen.x_sz,0,setup.screen.y_sz);
}

