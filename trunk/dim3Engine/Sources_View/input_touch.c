/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Touch Input Routines

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

extern setup_type			setup;

#define max_touch_state		4		// supergumba move

unsigned char				touch_states[max_touch_state];

extern d3pnt				mouse_gui_pnt;		// supergumba -- temp testing!

/* =======================================================

      Touch Clear
      
======================================================= */

void input_clear_touch(void)
{
	int				n;

	for (n=0;n!=max_touch_state;n++) {
		touch_states[n]=0x0;
	}
}

/* =======================================================

      Touch Event
      
======================================================= */

void input_touch_event_up(int id)
{
	fprintf(stdout,"touch UP %s\n",id);
}

void input_touch_event_down(int id,int x,int y)
{
	fprintf(stdout,"touch DOWN %s\n",id);

	mouse_gui_pnt.x=x;
	mouse_gui_pnt.y=y;
}

