/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Touch Input Routines

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

extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;

#define max_touch_state		4		// supergumba move

typedef struct		{
						int						id,stick_idx;
						bool					on;
						d3pnt					pt;
					} touch_state_type;

bool						touch_gui_click;
d3pnt						touch_gui_pnt;
touch_state_type			touch_states[max_touch_state];

/* =======================================================

      Touch Stubs
      
======================================================= */

#ifndef D3_OS_IPHONE

void input_touch_initialize(void) {}
void input_touch_shutdown(void) {}
void input_touch_clear(void) {}
void input_touch_event_up(int finger_id) {}
void input_touch_event_down(int touch_id,int finger_id,int x,int y) {}
void input_touch_event_move(int touch_id,int finger_id,int x,int y) {}
float input_touch_get_axis(int axis) { return(0.0f); }
bool input_touch_get_axis_as_button_min(int axis) { return(FALSE); }
bool input_touch_get_axis_as_button_max(int axis) { return(FALSE); }

#else

/* =======================================================

      Joystick Initialize
      
======================================================= */

void input_touch_initialize(void)
{
	int							n;
	iface_virtual_stick_type	*stick;

	stick=&iface.virtual_control.sticks[n];

	for (n=0;n!=max_virtual_stick;n++) {
		stick->touch_x=0.0f;
		stick->touch_y=0.0f;

		stick++;
	}
}

void input_touch_shutdown(void)
{
}

/* =======================================================

      Touch Clear
      
======================================================= */

void input_touch_clear(void)
{
	int				n;
	
	for (n=0;n!=max_touch_state;n++) {
		touch_states[n].on=FALSE;
	}
	
	touch_gui_pnt.x=touch_gui_pnt.y=-1;
	touch_gui_click=FALSE;
}

/* =======================================================

      Virtual Buttons
      
======================================================= */

void input_touch_to_virtual_button(d3pnt *pt,bool down)
{
	int							n;
	iface_virtual_button_type	*button;
	
	for (n=0;n!=max_virtual_button;n++) {
		button=&iface.virtual_control.buttons[n];
		if (!button->on) continue;

		if ((pt->x<button->x) || (pt->x>(button->x+button->x_size)) || (pt->y<button->y) || (pt->y>(button->y+button->y_size))) continue;

		input_action_set_touch_trigger_state(button->control_idx,down);
		return;
	}
}

/* =======================================================

      Virtual Sticks
      
======================================================= */

void input_touch_to_virtual_stick_move(touch_state_type *state)
{
	int							mx,my,sx,sy;
	iface_virtual_stick_type	*stick;

		// did this touch start in stick?
		
	if (state->stick_idx==-1) return;
	
		// still in stick?
		
	stick=&iface.virtual_control.sticks[state->stick_idx];
		
	if ((state->pt.x<stick->x) || (state->pt.x>(stick->x+stick->x_size)) || (state->pt.y<stick->y) || (state->pt.y>(stick->y+stick->y_size))) return;
	
		// move within stick
		
	sx=stick->x_size>>1;
	mx=stick->x+sx;
	
	sy=stick->y_size>>1;
	my=stick->y+sy;
	
	stick->touch_x=(float)(state->pt.x-mx)/(float)sx;
	if (stick->x_flip) stick->touch_x=-stick->touch_x;

	stick->touch_y=(float)(state->pt.y-my)/(float)sy;
	if (stick->y_flip) stick->touch_y=-stick->touch_y;
}

void input_touch_to_virtual_stick_start(touch_state_type *state)
{
	int							n;
	iface_virtual_stick_type	*stick;
	
	for (n=0;n!=max_virtual_stick;n++) {
		stick=&iface.virtual_control.sticks[n];
		if (!stick->on) continue;

			// in this stick?
			
		if ((state->pt.x<stick->x) || (state->pt.x>(stick->x+stick->x_size)) || (state->pt.y<stick->y) || (state->pt.y>(stick->y+stick->y_size))) continue;
		
			// touch is now tied to this stick
			
		state->stick_idx=n;
		input_touch_to_virtual_stick_move(state);
		return;
	}
}

void input_touch_to_virtual_stick_release(touch_state_type *state)
{
	iface_virtual_stick_type	*stick;

	if (state->stick_idx==-1) return;
	
	stick=&iface.virtual_control.sticks[state->stick_idx];
	stick->touch_x=0.0f;
	stick->touch_y=0.0f;
}

/* =======================================================

      Handle Touch States
      
======================================================= */

bool input_touch_get_state(int idx)
{
	return(touch_states[idx].on);
}

void input_touch_get_point(int idx,int *x,int *y)
{
	*x=touch_states[idx].pt.x;
	*y=touch_states[idx].pt.y;
}

void input_touch_state_add_up(int id)
{
	int					n;
	touch_state_type	*state;
	
	state=touch_states;
	
	for (n=0;n!=max_touch_state;n++) {
		if (state->on) {
			if (state->id==id) {
				state->on=FALSE;
				input_touch_to_virtual_button(&state->pt,FALSE);
				input_touch_to_virtual_stick_release(state);
				return;
			}
		}
		
		state++;
	}
}

void input_touch_state_add_down(int id,d3pnt *pt)
{
	int					n,idx;
	touch_state_type	*state;
	
	state=touch_states;
	
	idx=-1;
	
		// if already in, then update
		// the point
		
	for (n=0;n!=max_touch_state;n++) {
	
		if (state->on) {
			if (state->id==id) {
				state->pt.x=pt->x;
				state->pt.y=pt->y;
				input_touch_to_virtual_stick_move(state);
				return;
			}
		}
		else {
			if (idx==-1) idx=n;
		}
		
		state++;
	}
	
		// otherwise add it
		// if there is room
		
	if (idx==-1) return;
	
	state=&touch_states[idx];
	
	state->on=TRUE;
	state->id=id;
	state->stick_idx=-1;
	state->pt.x=pt->x;
	state->pt.y=pt->y;

	input_touch_to_virtual_button(&state->pt,TRUE);
	input_touch_to_virtual_stick_start(state);
}

/* =======================================================

      Touch Event
      
======================================================= */

void input_touch_event_up(int finger_id)
{
	input_touch_state_add_up(finger_id);

	touch_gui_click=FALSE;
}

void input_touch_scrub_point(d3pnt *pt,int touch_id,int x,int y)
{
	float				fx,fy;
	SDL_Touch			*touch;
	
	touch=SDL_GetTouch(touch_id);
	
	fx=((float)y)/((float)touch->yres);
	pt->x=(int)(fx*((float)view.desktop.high));
	
	fy=((float)x)/((float)touch->xres);
	pt->y=view.desktop.wid-(int)(fy*((float)view.desktop.wid));
}

void input_touch_event_down(int touch_id,int finger_id,int x,int y)
{
	d3pnt				pt;
	
	input_touch_scrub_point(&pt,touch_id,x,y);
	input_touch_state_add_down(finger_id,&pt);

	touch_gui_pnt.x=pt.x;
	touch_gui_pnt.y=pt.y;
	
	touch_gui_click=TRUE;
}

void input_touch_event_move(int touch_id,int finger_id,int x,int y)
{
	d3pnt				pt;
	
	input_touch_scrub_point(&pt,touch_id,x,y);
	input_touch_state_add_down(finger_id,&pt);
}

/* =======================================================

      Touch Stick
      
======================================================= */

float input_touch_get_axis(int axis)
{
	iface_virtual_stick_type	*stick;
	
		// get right stick
		
	if (axis<2) {
		stick=&iface.virtual_control.sticks[0];
	}
	else {
		stick=&iface.virtual_control.sticks[1];
	}
	
		// if stick not on, always centered
		
	if (!stick->on) return(0.0f);
	
		// get right axis
		
	if ((axis==0) || (axis==2)) return(stick->touch_x);
	return(stick->touch_y);
}

bool input_touch_get_axis_as_button_min(int axis)
{
	return(input_touch_get_axis(axis)<-0.5f);
}

bool input_touch_get_axis_as_button_max(int axis)
{
	return(input_touch_get_axis(axis)>0.5f);
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

#endif
