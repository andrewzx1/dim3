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

extern view_type			view;
extern setup_type			setup;

#define max_touch_state		4		// supergumba move -- deal with multiple touches here

unsigned char				touch_states[max_touch_state];
bool						touch_gui_click;
d3pnt						touch_gui_pnt;

/* =======================================================

      Touch Clear
      
======================================================= */

void input_clear_touch(void)
{
	int				n;

	for (n=0;n!=max_touch_state;n++) {
		touch_states[n]=0x0;
	}
	
	touch_gui_pnt.x=touch_gui_pnt.y=-1;
	touch_gui_click=FALSE;
}

/* =======================================================

      Touch Event
      
======================================================= */

void input_touch_event_up(int id)
{
	touch_gui_click=FALSE;
}

void input_touch_event_down(int id,int x,int y)
{
	d3pnt				pt;
	
	pt.x=(y*view.desktop.high)/0x7FFF;
	pt.y=view.desktop.wid-((x*view.desktop.wid)/0x7FFF);

	touch_gui_pnt.x=pt.x;
	touch_gui_pnt.y=pt.y;
	
	touch_gui_click=TRUE;
}

/* =======================================================

      Touch GUI
      
======================================================= */

void input_touch_gui_get_position(int *x,int *y)
{
	*x=touch_gui_pnt.x;
	*y=touch_gui_pnt.y;
}

void input_touch_gui_get_hilite_position(int *x,int *y)
{
	if (touch_gui_click) {
		input_touch_gui_get_position(x,y);
	}
	else {
		*x=*y=-1;
	}
}

bool input_touch_gui_is_click_down(void)
{
	return(touch_gui_click);
}


