/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View dim3RTL GUI

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "ray_interface.h"

extern map_type					map;
extern server_type				server;
extern camera_type				camera;
extern view_type				view;
extern iface_type				iface;
extern setup_type				setup;
extern network_setup_type		net_setup;
extern file_path_setup_type		file_path_setup;

extern int						nelement;
extern element_type				elements[max_element];

extern int						view_rtl_gui_scene_id;
extern GLuint					view_rtl_gui_gl_id;

int								gui_rtl_cursor_material_id,
								gui_rtl_background_material_id;

extern int view_dim3rtl_create_material_from_path(char *path,int alpha_type);
extern void view_dim3rtl_material_text_start(void);
extern void view_dim3rtl_material_text_stop(void);
extern void view_dim3rtl_transfer_to_opengl(int scene_id,int x,int y,int wid,int high,GLuint gl_id,int buff_wid,int buff_high);

/* =======================================================

      Start/Stop RTL GUI
      
======================================================= */

void gui_dim3rtl_initialize(char *background_path,char *bitmap_name)
{
	char			path[1024];
	
		// clear any overlays
		
	rtlSceneOverlayDeleteAll(view_rtl_gui_scene_id);
	
		// load the background
		
	gui_rtl_background_material_id=-1;
	
	if (bitmap_name!=NULL) {
		if (view_file_paths_bitmap_check_wide(path,background_path,bitmap_name)) {
			gui_rtl_background_material_id=view_dim3rtl_create_material_from_path(path,RL_MATERIAL_ALPHA_PASS_THROUGH);
		}
	}

		// load the cursor

	file_paths_data(&file_path_setup,path,"Bitmaps/UI_Elements","cursor","png");
	gui_rtl_cursor_material_id=view_dim3rtl_create_material_from_path(path,RL_MATERIAL_ALPHA_PASS_THROUGH);
}

void gui_dim3rtl_shutdown(void)
{
		// call all overlays

	rtlSceneOverlayDeleteAll(view_rtl_gui_scene_id);

		// clean up any remaining elements

	rtlMaterialDelete(gui_rtl_cursor_material_id);
	if (gui_rtl_background_material_id!=-1) rtlMaterialDelete(gui_rtl_background_material_id);
}

/* =======================================================

      Material Utility
      
======================================================= */

int gui_dim3rtl_material_open(char *path)
{
	return(view_dim3rtl_create_material_from_path(path,RL_MATERIAL_ALPHA_PASS_THROUGH));
}

void gui_dim3rtl_material_close(int rl_material_id)
{
	if (rl_material_id!=-1) rtlMaterialDelete(rl_material_id);
}

/* =======================================================

      Overlay Utilities
      
======================================================= */

void gui_dim3rtl_add_overlay(int x,int y,int wid,int high,int material_id)
{
	int				overlay_id;
	rtl2DPoint		p_pnt,s_pnt;

	p_pnt.x=x;
	p_pnt.y=y;
	s_pnt.x=wid;
	s_pnt.y=high;

	overlay_id=rtlSceneOverlayAdd(view_rtl_gui_scene_id,material_id,0);
	rtlSceneOverlaySetPosition(view_rtl_gui_scene_id,overlay_id,&p_pnt);
	rtlSceneOverlaySetSize(view_rtl_gui_scene_id,overlay_id,&s_pnt);

	p_pnt.x=0;
	p_pnt.y=0;

	rtlSceneOverlaySetQuadCount(view_rtl_gui_scene_id,overlay_id,1);
	rtlSceneOverlaySetQuadPosition(view_rtl_gui_scene_id,overlay_id,0,&p_pnt);
	rtlSceneOverlaySetQuadSize(view_rtl_gui_scene_id,overlay_id,0,&s_pnt);
}

/* =======================================================

      Draw RTL GUI
      
======================================================= */

void gui_dim3rtl_draw(bool show_cursor)
{
	int					n,x,y,sz,id;
	element_type		*element;
	rtlColor			col;

		// clear buffer

	col.r=iface.color.background.r;
	col.g=iface.color.background.g;
	col.b=iface.color.background.b;
	col.a=1.0f;
	
	rtlSceneClearBuffer(view_rtl_gui_scene_id,&col);

		// every frame we redo the entire
		// overlay system.  This is NOT optimal, but
		// it's a GUI and it's the easiest way to
		// retrofit in with the OpenGL stuff

	rtlSceneOverlayDeleteAll(view_rtl_gui_scene_id);

		// add the background

	if (gui_rtl_background_material_id!=-1) {
		gui_dim3rtl_add_overlay(0,0,setup.screen_wid,setup.screen_high,gui_rtl_background_material_id);
	}

		// element under cursor

	id=-1;
	if (show_cursor) id=element_draw_item_under_cursor();

		// add elements
				
	element=elements;
	
	for (n=0;n<nelement;n++) {

		if (element->hidden) {
			element++;
			continue;
		}
		
			// draw element
	
		switch (element->type) {
		
			case element_type_button:
			//	element_draw_button(element,id);
				break;
			case element_type_bitmap:
			//	element_draw_bitmap(element);
				break;
			case element_type_text:
			//	element_draw_text(element,id);
				break;
			case element_type_text_field:
			//	element_draw_text_field(element,id);
				break;
			case element_type_number:
			//	element_draw_number(element,id);
				break;
			case element_type_checkbox:
			//	element_draw_checkbox(element,id);
				break;
			case element_type_combo:
			//	element_draw_combo(element,id);
				break;
			case element_type_slider:
			//	element_draw_slider(element,id);
				break;
			case element_type_table:
			//	element_draw_table(element,id);
				break;
			case element_type_tab:
			//	element_draw_tab(element,id,x,y);
				break;
			case element_type_text_box:
			//	element_draw_text_box(element);
				break;
			case element_type_info_field:
			//	element_draw_info_field(element);
				break;
			case element_type_model:
			//	element_draw_model(element);
				break;
			case element_type_count:
			//	element_draw_count(element);
				break;
			case element_type_frame:
			//	element_draw_frame(element,x,y);
				break;
				
		}
		
		element++;
	}

		// add the cursor

	if (show_cursor) {
		input_mouse_gui_get_position(&x,&y);
		sz=(int)(((float)iface.scale_x)*cursor_size_factor);
		gui_dim3rtl_add_overlay(((x*setup.screen_wid)/iface.scale_x),((y*setup.screen_high)/iface.scale_y),sz,sz,gui_rtl_cursor_material_id);
	}

		// render

	rtlSceneRender(view_rtl_gui_scene_id);
	rtlSceneRenderFinish(view_rtl_gui_scene_id);
	view_dim3rtl_transfer_to_opengl(view_rtl_gui_scene_id,0,0,setup.screen_wid,setup.screen_high,view_rtl_gui_gl_id,setup.screen_wid,setup.screen_high);
}


