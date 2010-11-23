/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Input Functions Header

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

extern void input_initialize(bool in_window);
extern void input_shutdown(void);
extern void input_clear(void);
extern void input_set_key_start(void);
extern bool input_set_key_wait(char *name,bool *no_key_up);
extern bool input_app_active(void);
extern bool input_event_pump(void);

extern void input_action_clear(void);
extern void input_action_attach(char *attach_name,int action_index);
extern bool input_check_action_same_attachment(int action_1_index,int action_2_index);
extern bool input_action_get_state(int action_index);
extern bool input_action_get_state_range(int start_action_index,int end_action_index);
extern bool input_action_get_state_single(int action_index);

extern void input_mouse_initialize(void);
extern void input_mouse_shutdown(void);
extern void input_clear_mouse(void);
extern void input_mouse_pause(void);
extern void input_mouse_resume(void);
extern void input_event_mouse_button(int button,bool down);
extern void input_event_mouse_motion(int x,int y);
extern void input_event_mouse_wheel(int y);
extern void input_get_mouse_movement(float *x,float *y);
extern bool input_get_mouse_button(int button_idx);
extern void input_mouse_wheel_reset(void);

extern void input_gui_set_mouse(int x,int y);
extern void input_gui_get_mouse_position(int *x,int *y);
extern bool input_gui_get_mouse_left_button_down(void);
extern void input_gui_wait_mouse_left_button_up(void);

extern void input_clear_keyboard(void);
extern void input_event_key(int key_idx,bool down);
extern bool input_get_keyboard_escape(void);
extern bool input_get_keyboard_return(void);
extern bool input_get_keyboard_key(int key_idx);
extern void input_clear_text_input(void);
extern char input_get_text_input_key(void);

extern bool input_joystick_initialize(void);
extern void input_joystick_shutdown(void);
extern bool input_check_joystick_ok(void);
extern float input_get_joystick_axis(int axis);
extern bool input_get_joystick_axis_as_button_min(int axis);
extern bool input_get_joystick_axis_as_button_max(int axis);
extern bool input_get_joystick_button(int button_idx);
