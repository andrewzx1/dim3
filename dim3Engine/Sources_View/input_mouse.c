/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Mouse Input Routines

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

#include "inputs.h"
#include "timing.h"

extern hud_type				hud;
extern setup_type			setup;

int							mouse_tick,mouse_motion_x,mouse_motion_y,
							mouse_wheel_up_count,mouse_wheel_down_count;
bool						mouse_wheel_up_reset,mouse_wheel_down_reset;

/* =======================================================

      Mouse Initialize
      
======================================================= */

void input_mouse_initialize(void)
{
	SDL_ShowCursor(0);
	SDL_WM_GrabInput(SDL_GRAB_ON);
}

void input_mouse_shutdown(void)
{
	SDL_WM_GrabInput(SDL_GRAB_OFF);
}

/* =======================================================

      Mouse Clear
      
======================================================= */

void input_clear_mouse(void)
{
	mouse_tick=-1;

	mouse_motion_x=0;
	mouse_motion_y=0;
	
	mouse_wheel_up_count=0;
	mouse_wheel_down_count=0;

	mouse_wheel_up_reset=FALSE;
	mouse_wheel_down_reset=FALSE;
}

/* =======================================================

      Mouse Pausing
      
======================================================= */

void input_mouse_pause(void)
{
	SDL_WM_GrabInput(SDL_GRAB_OFF);
	SDL_ShowCursor(1);
}

void input_mouse_resume(void)
{
		// turn back off cursor

	SDL_ShowCursor(0);
	SDL_WM_GrabInput(SDL_GRAB_ON);

		// pump out any changes

	input_event_pump();
	input_clear_mouse();
}

/* =======================================================

      Mouse Events
      
======================================================= */

void input_event_mouse_button_down(int button)
{
		// treat wheel up/down as special cases as they
		// don't last long enough to be caught and we
		// also need to make sure there are paused between
		// multiple ups/downs so it doesn't look like a
		// continous key press

	if (button==SDL_BUTTON_WHEELUP) {
		mouse_wheel_up_count++;
		return;
	}
	if (button==SDL_BUTTON_WHEELDOWN) {
		mouse_wheel_down_count++;
		return;
	}
}

void input_event_mouse_motion(int x,int y)
{
	mouse_motion_x+=x;
	mouse_motion_y+=y;
}

/* =======================================================

      Game Mouse Input
      
======================================================= */

void input_get_mouse_movement(float *x,float *y)
{	
	int			tick;
	float		fx,fy,f_tick;
	
		// get movement and clear
		
	fx=(float)mouse_motion_x;
	mouse_motion_x=0;

	fy=(float)mouse_motion_y;
	mouse_motion_y=0;

		// calculate time change
		
	tick=game_time_get_raw();

	if (mouse_tick!=-1) {
		f_tick=(float)input_tick_rate/(float)(tick-mouse_tick);
		fx*=f_tick;
		fy*=f_tick;
	}

	mouse_tick=tick;

		// calculate speed and acceleration

	fx*=setup.mouse.speed;
	*x=fx+(fx*setup.mouse.acceleration);

	fy*=setup.mouse.speed;
	*y=fy+(fy*setup.mouse.acceleration);
}

bool input_get_mouse_button(int button_idx)
{
	switch (button_idx) {

		case SDL_BUTTON_LEFT:
			return(SDL_GetMouseState(NULL,NULL)&SDL_BUTTON_LMASK);
			
		case SDL_BUTTON_RIGHT:
			return(SDL_GetMouseState(NULL,NULL)&SDL_BUTTON_RMASK);
		
		case SDL_BUTTON_MIDDLE:
			return(SDL_GetMouseState(NULL,NULL)&SDL_BUTTON_MMASK);
		
		case SDL_BUTTON_WHEELUP:
			if (mouse_wheel_up_reset) return(FALSE);			// always send a single NOT DOWN message between DOWNs
			return(mouse_wheel_up_count>0);
		
		case SDL_BUTTON_WHEELDOWN:
			if (mouse_wheel_down_reset) return(FALSE);			// always send a single NOT DOWN message between DOWNs
			return(mouse_wheel_down_count>0);

	}
	
	return(FALSE);
}

void input_clear_mouse_wheel_state(void)
{
	if (mouse_wheel_up_reset) {
		mouse_wheel_up_reset=FALSE;
	}
	else {
		if (mouse_wheel_up_count>0) {
			mouse_wheel_up_count--;
			mouse_wheel_up_reset=TRUE;			// go into reset so you get NOT DOWN event
		}
	}

	if (mouse_wheel_down_reset) {
		mouse_wheel_down_reset=FALSE;
	}
	else {
		if (mouse_wheel_down_count>0) {
			mouse_wheel_down_count--;
			mouse_wheel_down_reset=TRUE;			// go into reset so you get NOT DOWN event
		}
	}
}

/* =======================================================

      GUI Mouse Input
      
======================================================= */

void input_gui_set_mouse(int x,int y)
{
		// scale from window to screen

	x=(x*setup.screen.x_sz)/hud.scale_x;
	y=(y*setup.screen.y_sz)/hud.scale_y;

		// set mouse
		
	SDL_WarpMouse((unsigned short)x,(unsigned short)y);
}

void input_gui_get_mouse_position(int *x,int *y)
{
		// get mouse

	SDL_PumpEvents();
	SDL_GetMouseState(x,y);

		// scale from window to screen

	*x=((*x)*hud.scale_x)/setup.screen.x_sz;
	*y=((*y)*hud.scale_y)/setup.screen.y_sz;
}

bool input_gui_get_mouse_left_button_down(void)
{
	SDL_PumpEvents();
	return((SDL_GetMouseState(NULL,NULL)&SDL_BUTTON_LMASK)!=0);
}

void input_gui_wait_mouse_left_button_up(void)
{
	while (input_gui_get_mouse_left_button_down()) {
		SDL_PumpEvents();
		usleep(1000);
	}
}
