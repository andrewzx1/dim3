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
								gui_rtl_background_material_id;

extern int view_dim3rtl_create_material_from_path(char *path,int alpha_type);
extern int view_dim3rtl_create_material_from_color(d3col *col);
extern texture_font_size_type* view_dim3rtl_material_text_get_font(int text_font,int text_size);
extern void view_dim3rtl_overlay_set_to_char(int scene_id,int overlay_id,texture_font_size_type *font_size,int ch);
extern void view_dim3rtl_transfer_to_opengl(int scene_id,int x,int y,int wid,int high,GLuint gl_id,int buff_wid,int buff_high);
extern int element_mouse_over_tab(element_type *element,int x,int y);

/* =======================================================

      Start/Stop RTL GUI
      
======================================================= */

void gui_dim3rtl_initialize(char *background_path,char *bitmap_name)
{
	char			path[1024];
	rtl2DPoint		scale;
	
		// clear any overlays
		
	rtlSceneOverlayDeleteAll(view_rtl_gui_scene_id);
	
		// set the scale
		
	scale.x=iface.scale_x;
	scale.y=iface.scale_y;

	rtlSceneOverlaySetScale(view_rtl_gui_scene_id,&scale);
	
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

void gui_dim3rtl_add_overlay_box_material(int x,int y,int wid,int high,float alpha,int material_id)
{
	int				overlay_id;
	rtl2DPoint		p_pnt,s_pnt;
	rtlColor		col;

		// the box

	overlay_id=rtlSceneOverlayAdd(view_rtl_gui_scene_id,RL_OVERLAY_TYPE_QUAD_MATERIAL,0);
	rtlSceneOverlaySetMaterial(view_rtl_gui_scene_id,overlay_id,material_id);

	col.r=col.g=col.b=1.0f;
	col.a=alpha;
	rtlSceneOverlaySetTint(view_rtl_gui_scene_id,overlay_id,&col);

	p_pnt.x=x;
	p_pnt.y=y;
	s_pnt.x=wid;
	s_pnt.y=high;

	rtlSceneOverlaySetQuadPosition(view_rtl_gui_scene_id,overlay_id,&p_pnt,&s_pnt);
}

void gui_dim3rtl_add_overlay_box_color(int x,int y,int wid,int high,bool outline,bool hilite,d3col *fill_col,d3col *outline_col,float alpha)
{
	int				overlay_id;
	rtl2DPoint		p_pnt,s_pnt,l_pnt[4];
	rtlColor		col;

		// the box

	overlay_id=rtlSceneOverlayAdd(view_rtl_gui_scene_id,RL_OVERLAY_TYPE_QUAD_COLOR,0);

	col.r=fill_col->r;
	col.g=fill_col->g;
	col.b=fill_col->b;
	col.a=alpha;
	rtlSceneOverlaySetTint(view_rtl_gui_scene_id,overlay_id,&col);

	p_pnt.x=x;
	p_pnt.y=y;
	s_pnt.x=wid;
	s_pnt.y=high;

	rtlSceneOverlaySetQuadPosition(view_rtl_gui_scene_id,overlay_id,&p_pnt,&s_pnt);

		// the outline

	if (outline) {
		col.r=outline_col->r;
		col.g=outline_col->g;
		col.b=outline_col->b;
		col.a=alpha;

		l_pnt[0].x=l_pnt[3].x=x;
		l_pnt[1].x=l_pnt[2].x=x+wid;
		l_pnt[0].y=l_pnt[1].y=y;
		l_pnt[2].y=l_pnt[3].y=y+high;

		overlay_id=rtlSceneOverlayAdd(view_rtl_gui_scene_id,RL_OVERLAY_TYPE_LINE_COLOR,0);
		rtlSceneOverlaySetTint(view_rtl_gui_scene_id,overlay_id,&col);
		rtlSceneOverlaySetLinePosition(view_rtl_gui_scene_id,overlay_id,&l_pnt[0],&l_pnt[1]);
		
		overlay_id=rtlSceneOverlayAdd(view_rtl_gui_scene_id,RL_OVERLAY_TYPE_LINE_COLOR,0);
		rtlSceneOverlaySetTint(view_rtl_gui_scene_id,overlay_id,&col);
		rtlSceneOverlaySetLinePosition(view_rtl_gui_scene_id,overlay_id,&l_pnt[1],&l_pnt[2]);
		
		overlay_id=rtlSceneOverlayAdd(view_rtl_gui_scene_id,RL_OVERLAY_TYPE_LINE_COLOR,0);
		rtlSceneOverlaySetTint(view_rtl_gui_scene_id,overlay_id,&col);
		rtlSceneOverlaySetLinePosition(view_rtl_gui_scene_id,overlay_id,&l_pnt[2],&l_pnt[3]);
		
		overlay_id=rtlSceneOverlayAdd(view_rtl_gui_scene_id,RL_OVERLAY_TYPE_LINE_COLOR,0);
		rtlSceneOverlaySetTint(view_rtl_gui_scene_id,overlay_id,&col);
		rtlSceneOverlaySetLinePosition(view_rtl_gui_scene_id,overlay_id,&l_pnt[3],&l_pnt[0]);
	}
	
		// the highlight
		
	if (hilite) {
		col.r=iface.color.control.mouse_over.r;
		col.g=iface.color.control.mouse_over.g;
		col.b=iface.color.control.mouse_over.b;
		col.a=alpha;

		l_pnt[0].x=l_pnt[3].x=x-1;
		l_pnt[1].x=l_pnt[2].x=(x+wid)+1;
		l_pnt[0].y=l_pnt[1].y=y-1;
		l_pnt[2].y=l_pnt[3].y=(y+high)+1;

		overlay_id=rtlSceneOverlayAdd(view_rtl_gui_scene_id,RL_OVERLAY_TYPE_LINE_COLOR,0);
		rtlSceneOverlaySetTint(view_rtl_gui_scene_id,overlay_id,&col);
		rtlSceneOverlaySetLinePosition(view_rtl_gui_scene_id,overlay_id,&l_pnt[0],&l_pnt[1]);
		
		overlay_id=rtlSceneOverlayAdd(view_rtl_gui_scene_id,RL_OVERLAY_TYPE_LINE_COLOR,0);
		rtlSceneOverlaySetTint(view_rtl_gui_scene_id,overlay_id,&col);
		rtlSceneOverlaySetLinePosition(view_rtl_gui_scene_id,overlay_id,&l_pnt[1],&l_pnt[2]);
		
		overlay_id=rtlSceneOverlayAdd(view_rtl_gui_scene_id,RL_OVERLAY_TYPE_LINE_COLOR,0);
		rtlSceneOverlaySetTint(view_rtl_gui_scene_id,overlay_id,&col);
		rtlSceneOverlaySetLinePosition(view_rtl_gui_scene_id,overlay_id,&l_pnt[2],&l_pnt[3]);
		
		overlay_id=rtlSceneOverlayAdd(view_rtl_gui_scene_id,RL_OVERLAY_TYPE_LINE_COLOR,0);
		rtlSceneOverlaySetTint(view_rtl_gui_scene_id,overlay_id,&col);
		rtlSceneOverlaySetLinePosition(view_rtl_gui_scene_id,overlay_id,&l_pnt[3],&l_pnt[0]);
	}
}

void gui_dim3rtl_add_overlay_line_color(int x,int y,int x2,int y2,d3col *fill_col,float alpha)
{
	int				overlay_id;
	rtl2DPoint		s_pnt,e_pnt;
	rtlColor		col;

	col.r=fill_col->r;
	col.g=fill_col->g;
	col.b=fill_col->b;
	col.a=alpha;

	s_pnt.x=x;
	s_pnt.y=y;
	e_pnt.x=x2;
	e_pnt.y=y2;

	overlay_id=rtlSceneOverlayAdd(view_rtl_gui_scene_id,RL_OVERLAY_TYPE_LINE_COLOR,0);
	rtlSceneOverlaySetTint(view_rtl_gui_scene_id,overlay_id,&col);
	rtlSceneOverlaySetLinePosition(view_rtl_gui_scene_id,overlay_id,&s_pnt,&e_pnt);
}

void gui_dim3rtl_add_overlay_text(int x,int y,int wid,int high,float alpha,char *str)
{
	int						n,str_len,
							char_wid,txt_wid,txt_high,
							ch,overlay_id;
	char					*c;
	texture_font_size_type	*font_size;
	rtl2DPoint				p_pnt,s_pnt;
	rtlColor				col;

	str_len=strlen(str);

	font_size=view_dim3rtl_material_text_get_font(font_interface_index,iface.font.text_size_medium);

		// the color

	col.r=iface.color.button.text.r;
	col.g=iface.color.button.text.g;
	col.b=iface.color.button.text.b;
	col.a=alpha;

        // get position

	char_wid=(iface.font.text_size_medium*setup.screen_wid)/iface.scale_x;

	txt_wid=gl_text_get_string_width(font_interface_index,iface.font.text_size_medium,FALSE,str);
	txt_wid=(txt_wid*setup.screen_wid)/iface.scale_x;

	txt_high=(int)(((float)iface.font.text_size_medium)*text_height_factor);
	txt_high=(txt_high*setup.screen_high)/iface.scale_y;

		// quad size

	s_pnt.x=char_wid;
	s_pnt.y=txt_high;

		// create the quads
	
	p_pnt.x=x+((wid>>1)-(txt_wid>>1));
	p_pnt.y=y+((high-txt_high)>>1);
	
	c=str;

	for (n=0;n!=str_len;n++) {
	
		ch=(int)*c++;

			// the overlay quad

		overlay_id=rtlSceneOverlayAdd(view_rtl_gui_scene_id,RL_OVERLAY_TYPE_QUAD_MATERIAL,0);
		rtlSceneOverlaySetMaterial(view_rtl_gui_scene_id,overlay_id,font_size->rtl_material_id);
		rtlSceneOverlaySetTint(view_rtl_gui_scene_id,overlay_id,&col);
		rtlSceneOverlaySetQuadPosition(view_rtl_gui_scene_id,overlay_id,&p_pnt,&s_pnt);
		view_dim3rtl_overlay_set_to_char(view_rtl_gui_scene_id,overlay_id,font_size,ch);

			// next character

		if ((ch<'!') || (ch>'z')) {
			p_pnt.x+=(int)(((float)char_wid)*0.33f);
		}
		else {
			p_pnt.x+=(int)(((float)char_wid)*font_size->char_size[ch-'!']);
		}
	}
}

/* =======================================================

      GUI RTL Elements
      
======================================================= */

void gui_dim3rtl_element_get_box(element_type *element,int *x,int *y,int *wid,int *high)
{
	*x=element->x;
	*y=element->y;
	*wid=element->wid;
	*high=element->high;
}

void gui_dim3rtl_element_draw_button_text(element_type *element,int sel_id)
{
	int			x,y,wid,high;
	float		alpha;
	
	alpha=element->enabled?1.0f:0.3f;

	gui_dim3rtl_element_get_box(element,&x,&y,&wid,&high);
	gui_dim3rtl_add_overlay_box_color(x,y,wid,high,TRUE,(element->id==sel_id),&iface.color.button.fill,&iface.color.button.outline,alpha);
	gui_dim3rtl_add_overlay_text(x,y,wid,high,alpha,element->setup.button.name);
}

void gui_dim3rtl_element_draw_button_bitmap(element_type *element,int sel_id)
{
	int		x,y,wid,high;
	
	gui_dim3rtl_element_get_box(element,&x,&y,&wid,&high);
	gui_dim3rtl_add_overlay_box_material(x,y,wid,high,1.0f,((element->id==sel_id)?element->setup.button.rl_select_material_id:element->setup.button.rl_material_id));
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

void gui_dim3rtl_element_draw_tab(element_type *element,int sel_id)
{
	int				n,x,y,xadd,x_slant,x_overlap,y_push,
					max_sz,mouse_idx,tab_idx,
					lx,rx,ty,by,margin;
	int				tab_draw_list[max_element_tab];
	d3col			*col_ptr;
	
		// tab sizes
	
	xadd=element->wid/element->setup.tab.ntab;
	max_sz=(int)(((float)iface.scale_x)*0.2f);
	if (xadd>max_sz) xadd=max_sz;

	margin=element_get_margin();

		// get mouse over element
	
	input_gui_get_hilite_position(&x,&y);
	mouse_idx=element_mouse_over_tab(element,x,y);

		// pick drawing order (last to first, selected always on top)
		
	tab_idx=0;
	
	for (n=(element->setup.tab.ntab-1);n>=0;n--) {
		if (n!=element->value) tab_draw_list[tab_idx++]=n;
	}
	
	tab_draw_list[tab_idx]=element->value;
	
		// tabs
		
	x_slant=(int)(((float)xadd)*0.025f);
	y_push=margin>>1;
	
	ty=element->y;
	by=ty+element->high;
	
	for (n=0;n!=element->setup.tab.ntab;n++) {
	
		tab_idx=tab_draw_list[n];

		lx=element->x+(xadd*tab_idx);
		rx=lx+xadd;
		
		if (tab_idx!=(element->setup.tab.ntab-1)) {
			x_overlap=x_slant/2;
		}
		else {
			x_overlap=0;
		}
		
		y=ty;

		if (element->value!=tab_idx) {
			col_ptr=&iface.color.tab.dimmed;
		}
		else {
			y-=y_push;
			col_ptr=&iface.color.tab.background;
		}
		
		gui_dim3rtl_add_overlay_box_color(lx,y,xadd,by,FALSE,FALSE,col_ptr,NULL,1.0f);

		gui_dim3rtl_add_overlay_line_color(lx,by,(lx+x_slant),y,&iface.color.tab.outline,1.0f);
		gui_dim3rtl_add_overlay_line_color((lx+x_slant),y,((rx-x_slant)+x_overlap),y,&iface.color.tab.outline,1.0f);
		gui_dim3rtl_add_overlay_line_color(((rx-x_slant)+x_overlap),y,(rx+x_overlap),by,&iface.color.tab.outline,1.0f);

		col_ptr=&iface.color.tab.text_dimmed;
		
		if (element->value==tab_idx) {
			col_ptr=&iface.color.tab.text;
		}
		
		if (mouse_idx==tab_idx) {
			col_ptr=&iface.color.tab.text_mouse_over;
		}

		gui_dim3rtl_add_overlay_text(((lx+rx)>>1),((y+by)>>1),xadd,element->high,1.0f,element->setup.tab.name[tab_idx]);
	}

		// final underline

	lx=element->x+(xadd*element->value);
	rx=lx+xadd;
	if (element->value!=0) view_primitive_2D_line(&iface.color.tab.outline,1.0f,element->x,by,lx,by);
	
	gui_dim3rtl_add_overlay_line_color(rx,by,(element->x+element->wid),by,&iface.color.tab.outline,1.0f);
}

void gui_dim3rtl_element_draw_frame(element_type *element)
{
	int				x,y,wid,high,lft,top,mid;
	bool			is_header;
	d3col			col;
	
		// header?
		
	is_header=element->str[0]!=0x0;
	
		// setup draw
		
	gui_dim3rtl_element_get_box(element,&x,&y,&wid,&high);

	lft=x;
	top=y;
	mid=top+element_get_frame_title_high();
	
		// inside frame
		
	gui_dim3rtl_add_overlay_box_color(x,y,wid,high,TRUE,FALSE,&iface.color.dialog.background,&iface.color.dialog.outline,1.0f);
	
		// header
		
	if (is_header) {
		y=(top+mid)>>1;

		if (element->enabled) {
			memmove(&col,&iface.color.dialog.header,sizeof(d3col));
		}
		else {
			memmove(&col,&iface.color.dialog.background,sizeof(d3col));
		}
	
		gui_dim3rtl_add_overlay_box_color(lft,top,wid,mid,TRUE,FALSE,&col,&iface.color.dialog.outline,1.0f);
		gui_dim3rtl_add_overlay_text(lft,top,wid,mid,1.0f,element->str);
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
		gui_dim3rtl_add_overlay_box_material(0,0,iface.scale_x,iface.scale_y,1.0f,gui_rtl_background_material_id);
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
				gui_dim3rtl_element_draw_tab(element,id);
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
				gui_dim3rtl_element_draw_frame(element);
				break;
				
		}
		
		element++;
	}

		// add the cursor

	if (show_cursor) {
		input_mouse_gui_get_position(&x,&y);
		sz=(int)(((float)iface.scale_x)*cursor_size_factor);
		gui_dim3rtl_add_overlay_box_material(x,y,sz,sz,1.0f,gui_rtl_cursor_material_id);
	}

		// draw the overlay, there
		// is no 3D to render here

	rtlSceneOverlayDraw(view_rtl_gui_scene_id);
	view_dim3rtl_transfer_to_opengl(view_rtl_gui_scene_id,0,0,setup.screen_wid,setup.screen_high,view_rtl_gui_gl_id,setup.screen_wid,setup.screen_high);
}


