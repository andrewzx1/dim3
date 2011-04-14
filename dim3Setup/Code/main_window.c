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
iface_type						iface;
setup_state_type				state;

extern list_palette_type		item_palette,property_palette,
								alt_property_palette,alt2_property_palette;

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
	alt2_property_palette_initialize();

		// size setups
		
	item_palette_setup();
	property_palette_setup();
	alt_property_palette_setup();
	alt2_property_palette_setup();
}

void main_wind_shutdown(void)
{
		// shutdown palettes
		
	alt2_property_palette_shutdown();
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
	alt2_property_palette_draw();
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

	if ((pnt->x>=alt2_property_palette.box.lx) && (pnt->x<=alt2_property_palette.box.rx) && (pnt->y>=alt2_property_palette.box.ty) && (pnt->y<alt2_property_palette.box.by)) {
		alt2_property_palette_click(pnt,double_click);
		return;
	}
}

/* =======================================================

      Scroll Wheel
      
======================================================= */

void main_wind_scroll_wheel(d3pnt *pnt,int delta)
{
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

	if ((pnt->x>=alt2_property_palette.box.lx) && (pnt->x<=alt2_property_palette.box.rx) && (pnt->y>=alt2_property_palette.box.ty) && (pnt->y<alt2_property_palette.box.by)) {
		alt2_property_palette_scroll_wheel(pnt,delta);
		return;
	}
}

/* =======================================================

      Mouse Movement
      
======================================================= */

void main_wind_mouse_move(d3pnt *pnt)
{
}

/* =======================================================

      Cursors
      
======================================================= */

bool main_wind_cursor(void)
{
	return(FALSE);
}

/* =======================================================

      Keys
      
======================================================= */

void main_wind_key(char ch)
{

		// check for deletes
		// on selected item tree

	if ((ch==D3_KEY_BACKSPACE) || (ch==D3_KEY_DELETE)) {
		if (item_palette_delete()) {
			main_wind_draw();
			return;
		}
	}
}

/* =======================================================

      Resize Setup
      
======================================================= */

void main_wind_resize(void)
{
	item_palette_setup();
	property_palette_setup();
	alt_property_palette_setup();
	alt2_property_palette_setup();
}

