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

int							last_raw_key[last_raw_key_count];
char						raw_key_shift_numbs[10]={'!','@','#','$','%','^','&','*','(',')'};

/* =======================================================

      Keyboard Clear
      
======================================================= */

void input_clear_keyboard(void)
{
	bzero(last_raw_key,(sizeof(int)*last_raw_key_count));
}

/* =======================================================

      Keyboard Events
      
======================================================= */

void input_event_keydown(int key)
{
	int					n,mod;
	
		// save last key for message typing
		
	for (n=0;n!=last_raw_key_count;n++) {
		if (last_raw_key[n]==0x0) {
			last_raw_key[n]=key;
			break;
		}
	}
	
		// check for cmd-q quits
		
	if (key==SDL_SCANCODE_Q) {
		mod=SDL_GetModState();
		if (((mod&KMOD_LMETA)!=0) || ((mod&KMOD_RMETA))) {
			game_loop_quit=TRUE;
		}
	}
}

/* =======================================================

      Keyboard Input
      
======================================================= */

bool input_get_keyboard_key(int key_idx)
{
	Uint8			*keystate;
	
	keystate=SDL_GetKeyboardState(NULL);
	return(keystate[key_idx]);
}

void input_clear_last_raw_key(void)
{
	if (last_raw_key[0]==0x0) return;

	memmove(&last_raw_key[0],&last_raw_key[1],(sizeof(int)*(last_raw_key_count-1)));
	last_raw_key[last_raw_key_count-1]=0x0;
}

void input_clear_all_last_raw_key(void)
{
	bzero(last_raw_key,(sizeof(int)*last_raw_key_count));
}

char input_get_last_raw_key(void)
{
	int				mod,key;
	bool			shifted;

	key=last_raw_key[0];
	
	if (key==0x0) return(0x0);
	
		// special keys
		
	if (key==SDL_SCANCODE_SPACE) return(0x20);
	if (key==SDL_SCANCODE_BACKSPACE) return(0x8);
	if (key==SDL_SCANCODE_ESCAPE) return(0x1B);
	if ((key==SDL_SCANCODE_RETURN) || (key==SDL_SCANCODE_KP_ENTER)) return(0xD);
	
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
		return(raw_key_shift_numbs[key-SDL_SCANCODE_1]);
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

/* =======================================================

      Keyboard Input
      
======================================================= */

char input_gui_get_keyboard_key(bool filter_esc)
{
	int				n,mod;
	bool			shifted;
	Uint8			*keystate;
	
	keystate=SDL_GetKeyboardState(NULL);

		// special keys

	if (keystate[SDL_SCANCODE_SPACE]) return(0x20);
	if ((keystate[SDL_SCANCODE_PERIOD]) || (keystate[SDL_SCANCODE_KP_PERIOD])) return('.');
	if (keystate[SDL_SCANCODE_BACKSPACE]) return(0x8);
	if (keystate[SDL_SCANCODE_TAB]) return(0x9);
	if ((keystate[SDL_SCANCODE_RETURN]) || (keystate[SDL_SCANCODE_KP_ENTER])) return(0xD);
	
	if (keystate[SDL_SCANCODE_ESCAPE]) {
		if (filter_esc) return(0x0);
		return(0x1B);
	}

		// numbers

	for (n=SDL_SCANCODE_0;n<=SDL_SCANCODE_9;n++) {
		if (keystate[n]) return('0'+(n-SDL_SCANCODE_0));
	}
	for (n=SDL_SCANCODE_KP_0;n<=SDL_SCANCODE_KP_9;n++) {
		if (keystate[n]) return('0'+(n-SDL_SCANCODE_KP_0));
	}

		// letters

	mod=SDL_GetModState();
	shifted=(((mod&KMOD_LSHIFT)!=0) || ((mod&KMOD_RSHIFT)));

	for (n=SDL_SCANCODE_A;n<=SDL_SCANCODE_Z;n++) {
		if (keystate[n]) {
			if (shifted) {
				return('A'+(n-SDL_SCANCODE_A));
			}
			else {
				return('a'+(n-SDL_SCANCODE_A));
			}
		}
	}
	
	return(0x0);
}

