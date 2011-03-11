/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Main Window

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
	#include "dim3setup.h"
#endif

#include "glue.h"
#include "interface.h"
#include "ui_common.h"

file_path_setup_type			file_path_setup;
setup_state_type				state;

/* =======================================================

      Initialize and Shutdown
	        
======================================================= */

void main_wind_initialize(void)
{
		// initializations
		
	text_initialize();
	
	list_palette_initialize("Setup");
	item_palette_initialize();
	property_palette_initialize();
	alt_property_palette_initialize();

		// size setups
		
	item_palette_setup();
	property_palette_setup();
	alt_property_palette_setup();
}

void main_wind_shutdown(void)
{
		// shutdown palettes
		
	alt_property_palette_shutdown();
	property_palette_shutdown();
	item_palette_shutdown();
	list_palette_shutdown();
	
	text_shutdown();
}

/* =======================================================

      GL Setup
      
======================================================= */

void main_wind_gl_setup(void)
{
	glClearColor(0.9f,0.9f,0.9f,0.0f);

	glEnable(GL_SMOOTH);
	glDisable(GL_DITHER);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
	
	glEnableClientState(GL_VERTEX_ARRAY);
}

/* =======================================================

      Main Window Draw
      
======================================================= */

void main_wind_draw_no_swap(void)
{
		// clear gl buffer
		
	glDisable(GL_SCISSOR_TEST);
	
	glClearColor(1.0f,1.0f,1.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// draw palettes

	item_palette_draw();
	property_palette_draw();
	alt_property_palette_draw();
}

void main_wind_draw(void)
{
	main_wind_draw_no_swap();
	os_swap_gl_buffer();
}

/* =======================================================

      Clicking
      
======================================================= */

void main_wind_click(d3pnt *pnt,bool double_click)
{
/*
	bool			old_playing;

		// tool palette

	if (pnt->y<tool_palette_box.by) {
		tool_palette_click(pnt);
		return;
	}

		// texture palette

	if (pnt->y>=txt_palette_box.ty) {
		texture_palette_click(model.textures,pnt,double_click);
		return;
	}

		// item, property and alt property palettes

	if ((pnt->x>=item_palette.box.lx) && (pnt->x<=item_palette.box.rx) && (pnt->y>=item_palette.box.ty) && (pnt->y<item_palette.box.by)) {
		item_palette_click(pnt,double_click);
		return;
	}

	if ((pnt->x>=property_palette.box.lx) && (pnt->x<=property_palette.box.rx) && (pnt->y>=property_palette.box.ty) && (pnt->y<property_palette.box.by)) {
		property_palette_click(pnt,double_click);
		return;
	}

	if ((pnt->x>=alt_property_palette.box.lx) && (pnt->x<=alt_property_palette.box.rx) && (pnt->y>=alt_property_palette.box.ty) && (pnt->y<alt_property_palette.box.by)) {
		alt_property_palette_click(pnt,double_click);
		return;
	}

		// model clicks
		// turn off animation as it glitches
		// up the win32 timers

	old_playing=state.playing;
	state.playing=FALSE;

	if (state.texture_edit_idx==-1) {
		model_wind_click(pnt);
	}
	else {
		texture_edit_click(pnt,double_click);
	}

	state.playing=old_playing;
	*/
}

/* =======================================================

      Scroll Wheel
      
======================================================= */

void main_wind_scroll_wheel(d3pnt *pnt,int delta)
{
/*
		// scroll wheel in item, property, or alt property palette

	if ((pnt->x>=item_palette.box.lx) && (pnt->x<=item_palette.box.rx) && (pnt->y>=item_palette.box.ty) && (pnt->y<item_palette.box.by)) {
		item_palette_scroll_wheel(pnt,delta);
		return;
	}

	if ((pnt->x>=property_palette.box.lx) && (pnt->x<=property_palette.box.rx) && (pnt->y>=property_palette.box.ty) && (pnt->y<property_palette.box.by)) {
		property_palette_scroll_wheel(pnt,delta);
		return;
	}

	if ((pnt->x>=alt_property_palette.box.lx) && (pnt->x<=alt_property_palette.box.rx) && (pnt->y>=alt_property_palette.box.ty) && (pnt->y<alt_property_palette.box.by)) {
		alt_property_palette_scroll_wheel(pnt,delta);
		return;
	}

		// scroll wheel in model

	if (state.texture_edit_idx==-1) {
		state.magnify_z+=(delta*20);
		main_wind_draw();
	}
	else {
		texture_edit_scroll_wheel(delta);
	}
	*/
}

/* =======================================================

      Mouse Movement
      
======================================================= */

void main_wind_mouse_move(d3pnt *pnt)
{
/*
	if (!state.model_open) return;

	tool_palette_mouse_move(pnt);
	*/
}

/* =======================================================

      Cursors
      
======================================================= */

bool main_wind_cursor(void)
{
/*
	if (!state.model_open) return(FALSE);

		// texture editing

	if (state.texture_edit_idx==-1) return(texture_edit_cursor());

		// model cursors

	if (os_key_space_down()) {
		os_set_hand_cursor();
		return(TRUE);
	}
	if (os_key_command_down()) {
		os_set_hand_cursor();
		return(TRUE);
	}
	if (os_key_option_down()) {
		os_set_resize_cursor();
		return(TRUE);
	}
	if (os_key_shift_down()) {
		os_set_add_cursor();
		return(TRUE);
	}
	if (os_key_control_down()) {
		os_set_subtract_cursor();
		return(TRUE);
	}
	
	*/
	os_set_arrow_cursor();
	return(TRUE);
}

/* =======================================================

      Keys
      
======================================================= */

void main_wind_key(char ch)
{
/*
	if (!state.model_open) return;
	
		// esc key deselects
		
	if (ch==0x1B) {
		vertex_clear_sel_mask(state.cur_mesh_idx);
		main_wind_draw();
		return;
	}

		// check for deletes
		// on selected item tree

	if ((ch==D3_KEY_BACKSPACE) || (ch==D3_KEY_DELETE)) {
		if (item_palette_delete()) {
			main_wind_draw();
			return;
		}
	}
	*/
}

/* =======================================================

      Resize Setup
      
======================================================= */

void main_wind_resize(void)
{
	item_palette_setup();
	property_palette_setup();
	alt_property_palette_setup();
}

