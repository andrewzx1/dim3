/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Window Routines

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

map_type						map;
editor_setup_type				setup;
file_path_setup_type			file_path_setup;
iface_type						iface;
editor_state_type				state;

extern list_palette_type		item_palette,property_palette,alt_property_palette;

/* =======================================================

      Initialize and Shutdown
	        
======================================================= */

void main_wind_initialize(void)
{
		// initializations
		
	text_initialize();
	
	tool_palette_initialize("Editor");
	list_palette_initialize("Editor");
	item_palette_initialize();
	property_palette_initialize();
	alt_property_palette_initialize();

	tool_tip_initialize();

	view_initialize();
   
        // misc setup
        
	state.vertex_mode=vertex_mode_none;
	state.drag_mode=drag_mode_mesh;
	state.grid_mode=grid_mode_small;
	state.handle_mode=handle_mode_rotate;
	
	state.free_look=TRUE;
	state.select_add=FALSE;
	
	state.drag_handle_idx=-1;
	
		// update view
	
	menu_update_view();
}

void main_wind_shutdown(void)
{
	view_shutdown();
	
	alt_property_palette_shutdown();
	property_palette_shutdown();
	item_palette_shutdown();
	list_palette_shutdown();
	
	tool_palette_shutdown();
	
	text_shutdown();
}

/* =======================================================

      GL Setup
      
======================================================= */

void main_wind_gl_setup(void)
{
	glClearColor(1.0f,1.0f,1.0f,0.0f);

	glEnable(GL_SMOOTH);
	glDisable(GL_DITHER);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_ALPHA_TEST);

 	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

/* =======================================================

      Drawing
      
======================================================= */

void main_wind_clear_viewport(void)
{
	int				wid,high;
	d3rect			wbox;

	os_get_window_box(&wbox);

	wid=wbox.rx-wbox.lx;
	high=wbox.by-wbox.ty;

	glScissor(0,0,wid,high);
	glViewport(0,0,wid,high);
}

void main_wind_draw_no_swap(void)
{
#ifdef D3_OS_WINDOWS
	int			tick;	// supergumba -- testing
	char	str[256];
	d3col	col;
#endif

		// clear draw buffer

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	main_wind_clear_viewport();

	glClearColor(0.75f,0.75f,0.75f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// draw window

	if (state.map_opened) {

#ifdef D3_OS_WINDOWS
		tick=GetTickCount();		// supergumba testing
#endif

		if (state.texture_edit_idx==-1) {
			view_draw();
		}
		else {
			texture_edit_draw();
		}

		tool_palette_draw();
		texture_palette_draw(map.textures);
		item_palette_draw();
		property_palette_draw();
		alt_property_palette_draw();

#ifdef D3_OS_WINDOWS
		sprintf(str,"%d",GetTickCount()-tick);		// supergumba -- testing
		col.r=col.g=1.0f;
		col.b=0.0f;
		text_draw(1600,60,20,&col,str);
#endif

		tool_tip_draw();
	}
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
	d3rect				tbox;
	
		// tool palette

	tool_palette_box(&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		tool_palette_click(pnt);
		return;
	}

		// texture palette
		
	texture_palette_box(&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		texture_palette_click(map.textures,pnt,double_click);
		return;
	}

		// item, property and alt property palettes
		
	list_palette_box(&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		if (!item_palette_click(pnt,double_click)) {
			if (!property_palette_click(pnt,double_click)) {
				alt_property_palette_click(pnt,double_click);
			}
		}
		return;
	}

		// view clicks

	if (state.texture_edit_idx==-1) {
		view_click(pnt,double_click);
	}
	else {
		texture_edit_click(pnt,double_click);
	}
}

/* =======================================================

      Scroll Wheel
      
======================================================= */

void main_wind_scroll_wheel(d3pnt *pnt,int delta)
{
	d3rect				tbox;
	
		// scroll wheel in item, property, or alt property palette

	list_palette_box(&tbox);

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<tbox.by)) {
		item_palette_scroll_wheel(pnt,delta);
		property_palette_scroll_wheel(pnt,delta);
		alt_property_palette_scroll_wheel(pnt,delta);
		return;
	}

		// scroll wheel in view

	if (state.texture_edit_idx==-1) {
		view_scroll_wheel(pnt,delta);
	}
	else {
		texture_edit_scroll_wheel(delta);
	}
}

/* =======================================================

      Mouse Movement
      
======================================================= */

void main_wind_mouse_move(d3pnt *pnt)
{
	if (!state.map_opened) return;

	tool_palette_mouse_move(pnt);
}

/* =======================================================

      Cursors
      
======================================================= */

bool main_wind_cursor(void)
{
	d3pnt			pnt;
	
	os_get_cursor(&pnt);

	if (state.texture_edit_idx==-1) return(view_cursor(&pnt));
	return(texture_edit_cursor());
}

/* =======================================================

      Keys
      
======================================================= */

void main_wind_key(char ch)
{
		// special tool keys
		
	if ((ch=='q') || (ch=='Q')) {
		tool_switch_vertex_mode();
		return;
	}
	
	if ((ch=='w') || (ch=='W')) {
		tool_switch_drag_mode();
		return;
	}
	
	if ((ch=='e') || (ch=='E')) {
		tool_switch_grid_mode();
		return;
	}
	
	if ((ch=='r') || (ch=='R')) {
		tool_switch_node_mode();
		return;
	}
	
		// panel keys
		
	view_key(ch);
}

/* =======================================================

      Resize Setup
      
======================================================= */

void main_wind_resize(void)
{
}














