/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Mouse Input Routines

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

#include "inputs.h"
#include "timing.h"

extern hud_type				hud;
extern setup_type			setup;

int							mouse_tick;
d3pnt						mouse_motion,mouse_gui_pnt;
bool						mouse_button_state[input_max_mouse_button_define];

/* =======================================================

      Mouse Initialize
      
======================================================= */

void input_mouse_initialize(void)
{
	SDL_SetRelativeMouseMode(SDL_TRUE);	
}

void input_mouse_shutdown(void)
{
	SDL_SetRelativeMouseMode(SDL_FALSE);
}

/* =======================================================

      Mouse Clear
      
======================================================= */

void input_clear_mouse(void)
{
	int				n;
	
	mouse_tick=-1;

	mouse_motion.x=0;
	mouse_motion.y=0;
	
	for (n=0;n!=input_max_mouse_button_define;n++) {
		mouse_button_state[n]=FALSE;
	}
}

/* =======================================================

      Mouse Pausing
      
======================================================= */

void input_mouse_pause(void)
{
	SDL_SetRelativeMouseMode(FALSE);
}

void input_mouse_resume(void)
{
		// turn back off cursor
		
	SDL_SetRelativeMouseMode(TRUE);

		// pump out any changes

	input_event_pump();
	input_clear_mouse();
}

/* =======================================================

      Mouse Events
      
======================================================= */

void input_event_mouse_button(int button,bool down)
{
	switch (button) {
		case 1:
			mouse_button_state[input_mouse_button_left]=down;
			break;
		case 2:
			mouse_button_state[input_mouse_button_middle]=down;
			break;
		case 3:
			mouse_button_state[input_mouse_button_right]=down;
			break;
	}	
}

void input_event_mouse_motion(int x,int y)
{
	mouse_motion.x+=x;
	mouse_motion.y+=y;
}

void input_event_mouse_wheel(int y)
{
	if (y>0) mouse_button_state[input_mouse_button_wheel_up]=TRUE;
	if (y<0) mouse_button_state[input_mouse_button_wheel_down]=TRUE;
}

/* =======================================================

      Game Mouse Input
      
======================================================= */

void input_get_mouse_movement(float *x,float *y)
{	
	int			tick;
	float		fx,fy,f_tick;
	
		// get movement and clear
		
	fx=(float)mouse_motion.x;
	mouse_motion.x=0;

	fy=(float)mouse_motion.y;
	mouse_motion.y=0;

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
	bool			down;
	
	down=mouse_button_state[button_idx];
	
		// need to release mouse wheel state
		
	if (button_idx==input_mouse_button_wheel_up) mouse_button_state[input_mouse_button_wheel_up]=FALSE;
	if (button_idx==input_mouse_button_wheel_down) mouse_button_state[input_mouse_button_wheel_down]=FALSE;
	
	return(down);
}

/* =======================================================

      GUI Mouse Input
      
======================================================= */

void input_gui_set_mouse(int x,int y)
{
		// clear mouse
		
	input_clear_mouse();
	
		// center to window
		
	mouse_gui_pnt.x=hud.scale_x>>1;
	mouse_gui_pnt.y=hud.scale_y>>1;
}

void input_gui_get_mouse_position(int *x,int *y)
{
	mouse_gui_pnt.x+=mouse_motion.x;
	mouse_motion.x=0;

	if (mouse_gui_pnt.x<0) mouse_gui_pnt.x=0;
	if (mouse_gui_pnt.x>=hud.scale_x) mouse_gui_pnt.x=hud.scale_x-1;
	
	mouse_gui_pnt.y+=mouse_motion.y;
	mouse_motion.y=0;

	if (mouse_gui_pnt.y<0) mouse_gui_pnt.y=0;
	if (mouse_gui_pnt.y>=hud.scale_y) mouse_gui_pnt.y=hud.scale_y-1;
	
	*x=mouse_gui_pnt.x;
	*y=mouse_gui_pnt.y;
}

inline bool input_gui_get_mouse_left_button_down(void)
{
	return(input_get_mouse_button(input_mouse_button_left));
}

inline void input_gui_wait_mouse_left_button_up(void)
{
	while (input_gui_get_mouse_left_button_down()) {
		usleep(1000);
	}
}
