/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View dim3RTL GUI

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
								gui_rtl_background_material_id,
								gui_rtl_button_fill_material_id;

extern int view_dim3rtl_create_material_from_path(char *path,int alpha_type);
extern int view_dim3rtl_create_material_from_color(d3col *col);
extern texture_font_size_type* view_dim3rtl_material_text_get_font(int text_font,int text_size);
extern bool view_dim3rtl_overlay_set_to_text(int scene_id,int overlay_id,d3pnt *pnt,int font_index,int size,int just,bool monospaced,rtlColor *col,char *data);
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
	
		// control colors
		
	gui_rtl_button_fill_material_id=view_dim3rtl_create_material_from_color(&iface.color.button.fill);
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

void gui_dim3rtl_add_overlay_box(int x,int y,int wid,int high,float alpha,int material_id)
{
	int				overlay_id;
	rtl2DPoint		p_pnt,s_pnt;
	rtlColor		col;

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

	col.r=col.g=col.b=1.0f;
	col.a=alpha;
	rtlSceneOverlaySetQuadColor(view_rtl_gui_scene_id,overlay_id,0,&col);
}

void gui_dim3rtl_add_overlay_text(int x,int y,int wid,int high,float alpha,char *str)
{
	int						overlay_id;
	d3pnt					pnt;
	texture_font_size_type	*font_size;
	rtl2DPoint				p_pnt,s_pnt;
	rtlColor				col;

	p_pnt.x=x;
	p_pnt.y=y;
	s_pnt.x=wid;
	s_pnt.y=high;

	font_size=view_dim3rtl_material_text_get_font(font_interface_index,iface.font.text_size_medium);

	overlay_id=rtlSceneOverlayAdd(view_rtl_gui_scene_id,font_size->rtl_material_id,0);
	rtlSceneOverlaySetPosition(view_rtl_gui_scene_id,overlay_id,&p_pnt);
	rtlSceneOverlaySetSize(view_rtl_gui_scene_id,overlay_id,&s_pnt);

	col.r=iface.color.button.text.r;
	col.g=iface.color.button.text.g;
	col.b=iface.color.button.text.b;
	col.a=alpha;

	x=x+(wid>>1);
	y=(y+(high>>1))-(iface.font.text_size_medium/10);

	view_dim3rtl_overlay_set_to_text(view_rtl_gui_scene_id,overlay_id,&pnt,font_interface_index,iface.font.text_size_medium,tx_center,FALSE,&col,str);
}

/* =======================================================

      GUI RTL Elements
      
======================================================= */

void gui_dim3rtl_element_get_box(element_type *element,int *x,int *y,int *wid,int *high)
{
	*x=((element->x*setup.screen_wid)/iface.scale_x);
	*y=((element->y*setup.screen_high)/iface.scale_y);
	*wid=((element->wid*setup.screen_wid)/iface.scale_x);
	*high=((element->high*setup.screen_high)/iface.scale_y);
}

/*
void element_draw_button_text(element_type *element,int sel_id)
{
	int				x,y,lft,rgt,top,bot;
	float			alpha;
	d3col			outline_col;
	
	if (element->enabled) {
		alpha=1.0f;
		memmove(&outline_col,&iface.color.button.outline,sizeof(d3col));
	}
	else {
		alpha=0.3f;
		memmove(&outline_col,&iface.color.button.outline,sizeof(d3col));
	}
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	
		// button background and outline

	view_primitive_2D_color_quad(&iface.color.button.fill,alpha,lft,rgt,top,bot);
	view_primitive_2D_line_quad(&outline_col,alpha,lft,rgt,top,bot);

	if (element->id==sel_id) view_primitive_2D_line_quad(&iface.color.control.mouse_over,alpha,(lft-1),(rgt+1),(top-1),(bot+1));
	
		// button text

	x=(lft+rgt)>>1;
	y=((top+bot)>>1)-(iface.font.text_size_medium/10);

	gl_text_start(font_interface_index,iface.font.text_size_medium,FALSE);
	gl_text_draw(x,y,element->setup.button.name,tx_center,TRUE,&iface.color.button.text,alpha);
	gl_text_end();
}
*/
void gui_dim3rtl_element_draw_button_text(element_type *element,int sel_id)
{
	int		x,y,wid,high;
	float	alpha;
	
	if (element->enabled) {
		alpha=1.0f;
	//	memmove(&outline_col,&iface.color.button.outline,sizeof(d3col));
	}
	else {
		alpha=0.3f;
	//	memmove(&outline_col,&iface.color.button.outline,sizeof(d3col));
	}

	gui_dim3rtl_element_get_box(element,&x,&y,&wid,&high);
	gui_dim3rtl_add_overlay_box(x,y,wid,high,alpha,gui_rtl_button_fill_material_id);
	gui_dim3rtl_add_overlay_text(x,y,wid,high,alpha,element->setup.button.name);
}

void gui_dim3rtl_element_draw_button_bitmap(element_type *element,int sel_id)
{
	int		x,y,wid,high;
	
	gui_dim3rtl_element_get_box(element,&x,&y,&wid,&high);
	gui_dim3rtl_add_overlay_box(x,y,wid,high,1.0f,((element->id==sel_id)?element->setup.button.rl_select_material_id:element->setup.button.rl_material_id));
}

void gui_dim3rtl_element_draw_button(element_type *element,int sel_id)
{
	switch (element->setup.button.mode) {

		case element_button_mode_text:
			gui_dim3rtl_element_draw_button_text(element,sel_id);
			break;

		case element_button_mode_bitmap:
			gui_dim3rtl_element_draw_button_bitmap(element,sel_id);
			break;

		case element_button_mode_box:
		//	gui_dim3rtl_element_draw_button_box(element,sel_id);
			break;

	}
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
		gui_dim3rtl_add_overlay_box(0,0,setup.screen_wid,setup.screen_high,1.0f,gui_rtl_background_material_id);
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
				gui_dim3rtl_element_draw_button(element,id);
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
		gui_dim3rtl_add_overlay_box(((x*setup.screen_wid)/iface.scale_x),((y*setup.screen_high)/iface.scale_y),sz,sz,1.0f,gui_rtl_cursor_material_id);
	}

		// render

	rtlSceneRender(view_rtl_gui_scene_id);
	rtlSceneRenderFinish(view_rtl_gui_scene_id);
	view_dim3rtl_transfer_to_opengl(view_rtl_gui_scene_id,0,0,setup.screen_wid,setup.screen_high,view_rtl_gui_gl_id,setup.screen_wid,setup.screen_high);
}


