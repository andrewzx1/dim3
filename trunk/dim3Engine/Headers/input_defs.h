/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Input Definitions Header

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

//
// SDL key lookups
//

#ifdef D3_SDL_1_3
	#define input_max_keyboard		103
#else
	#define input_max_keyboard		133
#endif

#define text_input_keys_count		4

#ifdef D3_SDL_1_3

#define key_names							{ \
												"Escape","F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12","F13","F14","F15", \
												"`","0","1","2","3","4","5","6","7","8","9","-","=","Backspace", \
												"Tab","Q","W","E","R","T","Y","U","I","O","P","[","]","\\", \
												"CapsLock","A","S","D","F","G","H","J","K","L",";","'","Return", \
												"Left Shift","Z","X","C","V","B","N","M",",",".","/","Right Shift", \
												"Left Control","Left Option/Alt","Left Command/Window","Space","Right Command/Window","Right Option/Alt","Right Command", \
												"Home","Page Up","Delete","End","Page Down", \
												"LeftArrow","UpArrow","DownArrow","RightArrow", \
												"Clear","Keypad =","Keypad /","Keypad *", \
												"Keypad 7","Keypad 8","Keypad 9","Keypad -", \
												"Keypad 4","Keypad 5","Keypad 6","Keypad +", \
												"Keypad 1","Keypad 2","Keypad 3", \
												"Keypad 0","Keypad .","Keypad Enter" \
											}
										
#define key_codes							{ \
												SDL_SCANCODE_ESCAPE,SDL_SCANCODE_F1,SDL_SCANCODE_F2,SDL_SCANCODE_F3,SDL_SCANCODE_F4,SDL_SCANCODE_F5,SDL_SCANCODE_F6,SDL_SCANCODE_F7,SDL_SCANCODE_F8,SDL_SCANCODE_F9,SDL_SCANCODE_F10,SDL_SCANCODE_F11,SDL_SCANCODE_F12,SDL_SCANCODE_F13,SDL_SCANCODE_F14,SDL_SCANCODE_F15, \
												SDL_SCANCODE_GRAVE,SDL_SCANCODE_0,SDL_SCANCODE_1,SDL_SCANCODE_2,SDL_SCANCODE_3,SDL_SCANCODE_4,SDL_SCANCODE_5,SDL_SCANCODE_6,SDL_SCANCODE_7,SDL_SCANCODE_8,SDL_SCANCODE_9,SDL_SCANCODE_MINUS,SDL_SCANCODE_EQUALS,SDL_SCANCODE_BACKSPACE, \
												SDL_SCANCODE_TAB,SDL_SCANCODE_Q,SDL_SCANCODE_W,SDL_SCANCODE_E,SDL_SCANCODE_R,SDL_SCANCODE_T,SDL_SCANCODE_Y,SDL_SCANCODE_U,SDL_SCANCODE_I,SDL_SCANCODE_O,SDL_SCANCODE_P,SDL_SCANCODE_LEFTBRACKET,SDL_SCANCODE_RIGHTBRACKET,SDL_SCANCODE_SLASH, \
												SDL_SCANCODE_CAPSLOCK,SDL_SCANCODE_A,SDL_SCANCODE_S,SDL_SCANCODE_D,SDL_SCANCODE_F,SDL_SCANCODE_G,SDL_SCANCODE_H,SDL_SCANCODE_J,SDL_SCANCODE_K,SDL_SCANCODE_L,SDL_SCANCODE_SEMICOLON,SDL_SCANCODE_APOSTROPHE,SDL_SCANCODE_RETURN, \
												SDL_SCANCODE_LSHIFT,SDL_SCANCODE_Z,SDL_SCANCODE_X,SDL_SCANCODE_C,SDL_SCANCODE_V,SDL_SCANCODE_B,SDL_SCANCODE_N,SDL_SCANCODE_M,SDL_SCANCODE_COMMA,SDL_SCANCODE_PERIOD,SDL_SCANCODE_BACKSLASH,SDL_SCANCODE_RSHIFT, \
												SDL_SCANCODE_LCTRL,SDL_SCANCODE_LALT,SDL_SCANCODE_LGUI,SDL_SCANCODE_SPACE,SDL_SCANCODE_RGUI,SDL_SCANCODE_RALT,SDL_SCANCODE_RCTRL, \
												SDL_SCANCODE_HOME,SDL_SCANCODE_PAGEUP,SDL_SCANCODE_DELETE,SDL_SCANCODE_END,SDL_SCANCODE_PAGEDOWN, \
												SDL_SCANCODE_LEFT,SDL_SCANCODE_UP,SDL_SCANCODE_DOWN,SDL_SCANCODE_RIGHT, \
												SDL_SCANCODE_CLEAR,SDL_SCANCODE_KP_EQUALS,SDL_SCANCODE_KP_DIVIDE,SDL_SCANCODE_KP_MULTIPLY, \
												SDL_SCANCODE_KP_7,SDL_SCANCODE_KP_8,SDL_SCANCODE_KP_9,SDL_SCANCODE_KP_MINUS, \
												SDL_SCANCODE_KP_4,SDL_SCANCODE_KP_5,SDL_SCANCODE_KP_6,SDL_SCANCODE_KP_PLUS, \
												SDL_SCANCODE_KP_1,SDL_SCANCODE_KP_2,SDL_SCANCODE_KP_3, \
												SDL_SCANCODE_KP_0,SDL_SCANCODE_KP_PERIOD,SDL_SCANCODE_KP_ENTER \
											}

#else

#define key_names							{ \
												"Backspace","Tab","Clear","Return","Pause","Escape","Space", \
												"!","\"","#","$","&","'","(",")","*","+",",","-",".","/", \
												"0","1","2","3","4","5","6","7","8","9", \
												":",";","<","=",">","?","@","[","\\","]","^","_","`", \
												"A","B","C","D","E","F","G","H","I","J","K","L","M", \
												"N","O","P","Q","R","S","T","U","V","W","X","Y","Z", \
												"Delete","KeyPad0","KeyPad1","KeyPad2","KeyPad3","KeyPad4","KeyPad5", \
												"KeyPad6","KeyPad7","KeyPad8","KeyPad9", \
												"KeyPad.","KeyPad/","KeyPad*","KeyPad-","KeyPad+","KeyPadEnter","KeyPad=", \
												"UpArrow","DownArrow","RightArrow","LeftArrow", \
												"Insert","Home","End","PageUp","PageDown", \
												"F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12","F13","F14","F15", \
												"NumLock","CapsLock","ScrollLock", \
												"RightShift","LeftShift","RightControl","LeftControl","RightOption","LeftOption", \
												"RightCommand","LeftCommand","LeftWindow","RightWindow", \
												"ModeShift","Help","PrintScreen","SysRq","Break","Menu","Power","Euro" \
											}
										
#define key_codes							{ \
												SDLK_BACKSPACE,SDLK_TAB,SDLK_CLEAR,SDLK_RETURN,SDLK_PAUSE,SDLK_ESCAPE,SDLK_SPACE, \
												SDLK_EXCLAIM,SDLK_QUOTEDBL,SDLK_HASH,SDLK_DOLLAR,SDLK_AMPERSAND,SDLK_QUOTE,SDLK_LEFTPAREN,SDLK_RIGHTPAREN,SDLK_ASTERISK,SDLK_PLUS,SDLK_COMMA,SDLK_MINUS,SDLK_PERIOD,SDLK_SLASH, \
												SDLK_0,SDLK_1,SDLK_2,SDLK_3,SDLK_4,SDLK_5,SDLK_6,SDLK_7,SDLK_8,SDLK_9, \
												SDLK_COLON,SDLK_SEMICOLON,SDLK_LESS,SDLK_EQUALS,SDLK_GREATER,SDLK_QUESTION,SDLK_AT,SDLK_LEFTBRACKET,SDLK_BACKSLASH,SDLK_RIGHTBRACKET,SDLK_CARET,SDLK_UNDERSCORE,SDLK_BACKQUOTE, \
												SDLK_a,SDLK_b,SDLK_c,SDLK_d,SDLK_e,SDLK_f,SDLK_g,SDLK_h,SDLK_i,SDLK_j,SDLK_k,SDLK_l,SDLK_m, \
												SDLK_n,SDLK_o,SDLK_p,SDLK_q,SDLK_r,SDLK_s,SDLK_t,SDLK_u,SDLK_v,SDLK_w,SDLK_x,SDLK_y,SDLK_z, \
												SDLK_DELETE,SDLK_KP0,SDLK_KP1,SDLK_KP2,SDLK_KP3,SDLK_KP4,SDLK_KP5, \
												SDLK_KP6,SDLK_KP7,SDLK_KP8,SDLK_KP9, \
												SDLK_KP_PERIOD,SDLK_KP_DIVIDE,SDLK_KP_MULTIPLY,SDLK_KP_MINUS,SDLK_KP_PLUS,SDLK_KP_ENTER,SDLK_KP_EQUALS, \
												SDLK_UP,SDLK_DOWN,SDLK_RIGHT,SDLK_LEFT, \
												SDLK_INSERT,SDLK_HOME,SDLK_END,SDLK_PAGEUP,SDLK_PAGEDOWN, \
												SDLK_F1,SDLK_F2,SDLK_F3,SDLK_F4,SDLK_F5,SDLK_F6,SDLK_F7,SDLK_F8,SDLK_F9,SDLK_F10,SDLK_F11,SDLK_F12,SDLK_F13,SDLK_F14,SDLK_F15, \
												SDLK_NUMLOCK,SDLK_CAPSLOCK,SDLK_SCROLLOCK, \
												SDLK_RSHIFT,SDLK_LSHIFT,SDLK_RCTRL,SDLK_LCTRL,SDLK_RALT,SDLK_LALT, \
												SDLK_RMETA,SDLK_LMETA,SDLK_LSUPER,SDLK_RSUPER, \
												SDLK_HELP,SDLK_PRINT,SDLK_SYSREQ,SDLK_BREAK,SDLK_MENU,SDLK_POWER,SDLK_EURO \
											}

#endif

#define input_max_mouse_button				5

#define input_mouse_button_left				0
#define input_mouse_button_right			1
#define input_mouse_button_middle			2
#define input_mouse_button_wheel_up			3
#define input_mouse_button_wheel_down		4

#define mouse_button_names					{"MouseLeft","MouseRight","MouseMiddle","MouseWheelUp","MouseWheelDown"}
#define mouse_button_codes					{input_mouse_button_left,input_mouse_button_right,input_mouse_button_middle,input_mouse_button_wheel_up,input_mouse_button_wheel_down}

#define input_max_joystick_button			20

#define joystick_button_names				{ \
												"Left","Right","Up","Down", \
												"Button 1","Button 2","Button 3","Button 4","Button 5", \
												"Button 6","Button 7","Button 8","Button 9","Button 10", \
												"Button 11","Button 12","Button 13","Button 14","Button 15", \
												"Button 16" \
											}

//
// action structures
//

#define input_max_action_item					4

#define input_type_key							0
#define input_type_mouse_button					1
#define input_type_joystick_button				2

#define input_joystick_button_left				0
#define input_joystick_button_right				1
#define input_joystick_button_up				2
#define input_joystick_button_down				3
#define input_joystick_button_1					4
#define input_joystick_button_2					5
#define input_joystick_button_3					6
#define input_joystick_button_4					7
#define input_joystick_button_5					8
#define input_joystick_button_6					9
#define input_joystick_button_7					10
#define input_joystick_button_8					11
#define input_joystick_button_9					12
#define input_joystick_button_10				13
#define input_joystick_button_11				14
#define input_joystick_button_12				15
#define input_joystick_button_13				16
#define input_joystick_button_14				17
#define input_joystick_button_15				18
#define input_joystick_button_16				19

typedef struct		{
						int						type,index;
					} input_action_item_type;
					
typedef struct		{
						int						nitem;
						input_action_item_type  items[input_max_action_item];
						bool					touch_trigger,still_down;
					} input_action_type;
