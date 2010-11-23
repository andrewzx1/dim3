/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Keyboard Input Routines

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

extern setup_type			setup;

extern bool					game_loop_quit;

int							text_input_keys[text_input_keys_count];
char						text_input_shift_numbs[10]={'!','@','#','$','%','^','&','*','(',')'};
unsigned char				key_states[1024];

#ifndef D3_SDL_1_3
	#define SDL_SCANCODE_A			SDLK_a
	#define SDL_SCANCODE_Q			SDLK_q
	#define SDL_SCANCODE_Z			SDLK_z
	#define SDL_SCANCODE_0			SDLK_0
	#define SDL_SCANCODE_1			SDLK_1
	#define SDL_SCANCODE_9			SDLK_9
	#define SDL_SCANCODE_KP_0		SDLK_KP0
	#define SDL_SCANCODE_KP_9		SDLK_KP9
	#define SDL_SCANCODE_SPACE		SDLK_SPACE
	#define SDL_SCANCODE_BACKSPACE	SDLK_BACKSPACE
	#define SDL_SCANCODE_ESCAPE		SDLK_ESCAPE
	#define SDL_SCANCODE_RETURN		SDLK_RETURN
	#define SDL_SCANCODE_KP_ENTER	SDLK_KP_ENTER
	#define SDL_SCANCODE_MINUS		SDLK_MINUS
	#define SDL_SCANCODE_EQUALS		SDLK_EQUALS
	#define SDL_SCANCODE_COMMA		SDLK_COMMA
	#define SDL_SCANCODE_PERIOD		SDLK_PERIOD
	#define SDL_SCANCODE_SLASH		SDLK_SLASH
	#define SDL_SCANCODE_SEMICOLON	SDLK_SEMICOLON
	#define SDL_SCANCODE_APOSTROPHE	SDLK_QUOTE
	#define SDL_SCANCODE_PERIOD		SDLK_PERIOD
	#define SDL_SCANCODE_KP_PERIOD	SDLK_KP_PERIOD
	#define SDL_SCANCODE_TAB		SDLK_TAB
	#define SDL_SCANCODE_CAPSLOCK	SDLK_CAPSLOCK
#endif

/* =======================================================

      Keyboard Clear
      
======================================================= */

void input_clear_keyboard(void)
{
	bzero(key_states,1024);
	bzero(text_input_keys,(sizeof(int)*text_input_keys_count));
}

/* =======================================================

      Keyboard Events
      
======================================================= */

void input_event_key(int key_idx,bool down)
{
	int					n;
#ifdef D3_OS_MAC
	int					mod;
#endif

		// set the key

	key_states[key_idx]=down;

		// rest of function only cares
		// about key downs

	if (!down) return;
	
		// save the key in the text
		// input list
		
	for (n=0;n!=text_input_keys_count;n++) {
		if (text_input_keys[n]==0x0) {
			text_input_keys[n]=key_idx;
			break;
		}
	}

		// check for cmd-q quits

#ifdef D3_OS_MAC
	if (key==SDL_SCANCODE_Q) {
		mod=SDL_GetModState();
		if (((mod&KMOD_LMETA)!=0) || ((mod&KMOD_RMETA))) {
			game_loop_quit=TRUE;
		}
	}
#endif
}

/* =======================================================

      Keyboard Input
      
======================================================= */

bool input_get_keyboard_key(int key_idx)
{
	return(key_states[key_idx]);
}

bool input_get_keyboard_escape(void)
{
	return(key_states[SDL_SCANCODE_ESCAPE]);
}

bool input_get_keyboard_return(void)
{
	return((key_states[SDL_SCANCODE_RETURN]) || (key_states[SDL_SCANCODE_KP_ENTER]));
}

/* =======================================================

      Text Input
      
======================================================= */

void input_clear_text_input(void)
{
	bzero(text_input_keys,(sizeof(int)*text_input_keys_count));
}

char input_get_text_input_key(void)
{
	int				mod,key;
	bool			shifted;

	key=text_input_keys[0];
	
	if (key==0x0) return(0x0);

		// clear the key

	memmove(&text_input_keys[0],&text_input_keys[1],(sizeof(int)*(text_input_keys_count-1)));
	text_input_keys[text_input_keys_count-1]=0x0;
	
		// special keys
		
	if (key==SDL_SCANCODE_SPACE) return(0x20);
	if (key==SDL_SCANCODE_BACKSPACE) return(0x8);
	if ((key==SDL_SCANCODE_RETURN) || (key==SDL_SCANCODE_KP_ENTER) || (key==SDL_SCANCODE_ESCAPE)) return(0xD);

		// get shift state
		
	mod=SDL_GetModState();
	shifted=(((mod&KMOD_LSHIFT)!=0) || ((mod&KMOD_RSHIFT)));
	
		// character runs
		
	if ((key>=SDL_SCANCODE_A) && (key<=SDL_SCANCODE_Z)) {
		if (shifted) return('A'+(char)(key-SDL_SCANCODE_A));
		return('a'+(key-SDL_SCANCODE_A));
	}
	
	if ((key>=SDL_SCANCODE_0) && (key<=SDL_SCANCODE_9)) {
		if (!shifted) return('0'+(char)(key-SDL_SCANCODE_0));
		if (key==SDL_SCANCODE_0) return(')');
		return(text_input_shift_numbs[key-SDL_SCANCODE_1]);
	}
	
		// couple misc characters
		
	if (key==SDL_SCANCODE_MINUS) {
		if (!shifted) return('-');
		return('_');
	}
	if (key==SDL_SCANCODE_EQUALS) {
		if (!shifted) return('=');
		return('+');
	}
	if (key==SDL_SCANCODE_COMMA) {
		if (!shifted) return(',');
		return('<');
	}
	if (key==SDL_SCANCODE_PERIOD) {
		if (!shifted) return('.');
		return('>');
	}
	if (key==SDL_SCANCODE_SLASH) {
		if (!shifted) return('/');
		return('?');
	}
	if (key==SDL_SCANCODE_SEMICOLON) {
		if (!shifted) return(';');
		return(':');
	}
	if (key==SDL_SCANCODE_APOSTROPHE) {
		if (!shifted) return('\'');
		return('"');
	}

	return(0x0);
}

