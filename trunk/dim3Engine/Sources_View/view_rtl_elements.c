/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View dim3RTL GUI Elements

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

extern int						view_rtl_gui_scene_id,element_open_combo_id;
extern GLuint					view_rtl_gui_gl_id;

extern void gui_dim3rtl_add_overlay_box_material(int x,int y,int wid,int high,float alpha,int material_id);
extern void gui_dim3rtl_add_overlay_box_color(int x,int y,int wid,int high,bool outline,bool hilite,d3col *fill_col,d3col *outline_col,float alpha);
extern void gui_dim3rtl_add_overlay_box_gradient(int x,int y,int wid,int high,bool outline,bool hilite,d3col *fill_col,d3col *outline_col,float alpha);
extern void gui_dim3rtl_add_overlay_box_outline(int x,int y,int wid,int high,bool hilite,d3col *outline_col,float alpha);
extern void gui_dim3rtl_add_overlay_line_color(int x,int y,int x2,int y2,d3col *fill_col,float alpha);
extern void gui_dim3rtl_add_overlay_text(int x,int y,d3col *fill_col,float alpha,int just,char *str);
extern element_type* element_find(int id);
extern int element_get_combo_list_count(element_type *element);
extern void element_box_combo_open(element_type *element,int *lft,int *rgt,int *top,int *bot);
extern int element_get_table_row_count(element_type *element);
extern int element_get_table_row_high(element_type *element);
extern unsigned long element_draw_table_get_image_gl_id(element_type *element,int row_idx);
extern int element_mouse_over_tab(element_type *element,int x,int y);

/* =======================================================

      Element Buttons
      
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
	gui_dim3rtl_add_overlay_text((x+(wid>>1)),(y+(high>>1)),&iface.color.button.text,alpha,tx_center,element->setup.button.name);
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

/* =======================================================

      Element Controls
      
======================================================= */

void gui_dim3rtl_element_draw_checkbox_control(int x,int y,bool checked,bool enabled,bool hilite)
{
	int			wid,high,chk_lft;
	float		alpha;
	d3col		col;
	
		// checkbox

	wid=element_get_control_short_wid();
	high=element_get_control_high();

	y-=(high>>1);

	alpha=(enabled?1.0f:0.3f);

		// background
		
	gui_dim3rtl_add_overlay_box_gradient(x,y,wid,high,FALSE,FALSE,&iface.color.control.fill,NULL,alpha);

		// check position

	if (checked) {
		chk_lft=x+(wid>>1);
		col.r=iface.color.control.hilite.r;
		col.g=iface.color.control.hilite.g;
		col.b=iface.color.control.hilite.b;
	}
	else {
		chk_lft=x;
		col.r=iface.color.control.fill.r*element_gradient_factor_darken;
		col.g=iface.color.control.fill.g*element_gradient_factor_darken;
		col.b=iface.color.control.fill.b*element_gradient_factor_darken;
	}

	gui_dim3rtl_add_overlay_box_gradient(chk_lft,y,(wid>>1),high,TRUE,FALSE,&col,&iface.color.control.outline,alpha);

		// text
		
	gui_dim3rtl_add_overlay_text((chk_lft+(wid>>2)),(y+(high>>1)),&iface.color.control.label,alpha,tx_center,(checked?"on":"off"));

		// outline

	gui_dim3rtl_add_overlay_box_outline(x,y,wid,high,((hilite) && (enabled)),&iface.color.control.outline,alpha);
}

void gui_dim3rtl_element_draw_checkbox(element_type *element,int sel_id)
{
	int				y;

	y=element->y-(element->high>>1);

		// label

	gui_dim3rtl_add_overlay_text((element->x-15),y,&iface.color.control.label,1.0f,tx_right,element->str);
	gui_dim3rtl_add_overlay_text((element->x-10),y,&iface.color.control.label,1.0f,tx_center,":");
	
		// checkbox
	
	gui_dim3rtl_element_draw_checkbox_control(element->x,y,(element->value!=0),element->enabled,(element->id==sel_id));
}

void gui_dim3rtl_element_draw_combo(element_type *element,int sel_id)
{
	int				y,lft,rgt,mid,high,ky,ty,by;
	char			str[256];
	float			alpha;
	
		// label
	
	y=element->y-(element->high>>1);

	gui_dim3rtl_add_overlay_text((element->x-15),y,&iface.color.control.label,1.0f,tx_right,element->str);
	gui_dim3rtl_add_overlay_text((element->x-10),y,&iface.color.control.label,1.0f,tx_center,":");
		
		// background and outline

	high=element_get_control_high();
	ky=y-(high>>1);
		
	alpha=(element->enabled?1.0f:0.3f);

	gui_dim3rtl_add_overlay_box_gradient(element->x,ky,element->wid,high,TRUE,((element->id==sel_id) && (element->enabled)),&iface.color.control.fill,&iface.color.control.outline,alpha);

		// arrow

	lft=(element->x+element->wid)-(high-2);
	rgt=lft+(high-8);
	mid=(lft+rgt)>>1;

	ty=ky+4;
	by=ty+(high-8);

	gui_dim3rtl_add_overlay_line_color(lft,ty,rgt,ty,&iface.color.control.outline,1.0f);
	gui_dim3rtl_add_overlay_line_color(lft,ty,mid,by,&iface.color.control.outline,1.0f);
	gui_dim3rtl_add_overlay_line_color(rgt,ty,mid,by,&iface.color.control.outline,1.0f);

		// control text

	strcpy(str,(element->data+(element->value*32)));
	gui_dim3rtl_add_overlay_text((element->x+5),y,&iface.color.control.text,1.0f,tx_left,str);
}

void gui_dim3rtl_element_draw_combo_open(element_type *element)
{
	int				x,y,wid,high,ty,n,cnt,lft,rgt,top,bot,sel_item_idx;
	char			str[256];
	
		// combo count

	cnt=element_get_combo_list_count(element);

		// get selected item

	input_gui_get_hilite_position(&x,&y);

	sel_item_idx=-1;

	element_box_combo_open(element,&lft,&rgt,&top,&bot);
	if ((x>=lft) && (x<=rgt) && (y>=top) && (y<=bot)) {
		sel_item_idx=(y-top)/element->high;
	}

		// drawing sizes

	x=element->x;
	y=element->y-(element->high*element->value);
	if (y<element->high) y=element->high;
	
	top=y-element_get_control_high();
	wid=(element->wid-element->high)-2;
	high=cnt*element->high;

		// background
		
	gui_dim3rtl_add_overlay_box_color(x,top,wid,high,FALSE,FALSE,&iface.color.control.fill,&iface.color.button.outline,1.0f);

		// combo items
		
	ty=top;

	for (n=0;n!=cnt;n++) {

		bot=top+element->high;

			// selection

		if (element->value==n) {
			gui_dim3rtl_add_overlay_box_gradient(element->x,ty,wid,element->high,FALSE,FALSE,&iface.color.control.hilite,NULL,1.0f);
		}

			// text
			
		strcpy(str,(element->data+(n*32)));

		if (sel_item_idx==n) {
			gui_dim3rtl_add_overlay_text((x+5),(ty+(element->high>>1)),&iface.color.control.mouse_over,1.0f,tx_left,str);
		}
		else {
			gui_dim3rtl_add_overlay_text((x+5),(ty+(element->high>>1)),&iface.color.control.text,1.0f,tx_left,str);
		}

		ty+=element->high;
	}
	
		// outline
	
	gui_dim3rtl_add_overlay_box_outline(x,top,wid,high,TRUE,&iface.color.control.outline,1.0f);
}

void gui_dim3rtl_element_draw_slider(element_type *element,int sel_id)
{
	int				y,ky,mid_wid,high,handle_wid,lx;
	float			alpha;
	char			str[32];
	d3col			col2;
	
		// label
		
	y=element->y-(element->high>>1);

	gui_dim3rtl_add_overlay_text((element->x-15),y,&iface.color.control.label,1.0f,tx_right,element->str);
	gui_dim3rtl_add_overlay_text((element->x-10),y,&iface.color.control.label,1.0f,tx_center,":");
	
		// slider size
		
	high=element_get_control_high();
	ky=y-(high>>1);

	mid_wid=(int)(((float)element->wid)*element->setup.slider.value);

	alpha=(element->enabled?1.0f:0.3f);
	
		// background
		
	gui_dim3rtl_add_overlay_box_gradient(element->x,ky,element->wid,high,FALSE,FALSE,&iface.color.control.fill,NULL,alpha);
	
		// slider and handle
		
	if (element->enabled) {
		gui_dim3rtl_add_overlay_box_gradient(element->x,ky,mid_wid,high,FALSE,FALSE,&iface.color.control.hilite,NULL,alpha);

		lx=(element->x+mid_wid)-(element->wid/20);
		handle_wid=(element->wid/10);

		if (lx<element->x) lx=element->x;
		if ((lx+handle_wid)>(element->x+element->wid)) lx=(element->x+element->wid)-handle_wid;

		col2.r=iface.color.control.hilite.r*element_gradient_factor_foreground;
		col2.g=iface.color.control.hilite.g*element_gradient_factor_foreground;
		col2.b=iface.color.control.hilite.b*element_gradient_factor_foreground;
	
		gui_dim3rtl_add_overlay_box_gradient(lx,ky,handle_wid,element->high,TRUE,FALSE,&col2,&iface.color.control.outline,alpha);
	}

		// text
		
	sprintf(str,"%d%%",(int)(element->setup.slider.value*100.0f));
	gui_dim3rtl_add_overlay_text(((element->x+element->wid)-5),y,&iface.color.control.text,1.0f,tx_right,str);

		// outline
		
	gui_dim3rtl_add_overlay_box_outline(element->x,ky,element->wid,high,((element->id==sel_id) && (element->enabled)),&iface.color.control.outline,1.0f);
}

/* =======================================================

      Scroll Controls
      
======================================================= */

void gui_dim3rtl_element_draw_scroll_controls(element_type *element,int header_high,bool up_ok,bool down_ok)
{
	int				lft,top,bot,ctrl_sz;
	float			alpha;
	d3col			col,col2,col3;
	
		// element size

	ctrl_sz=element_get_control_scroll_size();
	
	lft=((element->x+element->wid)-ctrl_sz)-4;
	top=element->y+(header_high+8);
	bot=((element->y+element->high)-ctrl_sz)-4;

		// colors

	memmove(&col,&iface.color.control.fill,sizeof(d3col));
	col2.r=col.r*element_gradient_factor_background;
	col2.g=col.g*element_gradient_factor_background;
	col2.b=col.b*element_gradient_factor_background;
	
	col3.r=col3.g=col3.b=0.0f;

		// top scroll bar

	alpha=up_ok?1.0f:0.1f;
	gui_dim3rtl_add_overlay_box_color(lft,top,ctrl_sz,ctrl_sz,TRUE,FALSE,&col,&iface.color.control.outline,alpha);

		// bottom scroll bar

	alpha=down_ok?1.0f:0.1f;
	gui_dim3rtl_add_overlay_box_color(lft,(bot-ctrl_sz),ctrl_sz,ctrl_sz,TRUE,FALSE,&col,&iface.color.control.outline,alpha);
}

/* =======================================================

      Tables
      
======================================================= */

void gui_dim3rtl_element_draw_table_row_column_lines(element_type *element,int ty,int by,float col_factor)
{
	int			n,x;
	float		f_wid;
	d3col		col;
	
	x=element->x;
	f_wid=(float)element->wid;
	
	col.r=iface.color.control.outline.r*col_factor;
	col.g=iface.color.control.outline.g*col_factor;
	col.b=iface.color.control.outline.b*col_factor;

	for (n=1;n<element->setup.table.ncolumn;n++) {
		x+=(int)(element->setup.table.cols[n-1].percent_size*f_wid);
		gui_dim3rtl_add_overlay_line_color(x,ty,x,by,&col,1.0f);
	}
}

void gui_dim3rtl_element_draw_table_header_fill(element_type *element,int high)
{
	gui_dim3rtl_add_overlay_box_gradient(element->x,element->y,element->wid,(high+4),FALSE,FALSE,&iface.color.control.header,NULL,1.0f);
	gui_dim3rtl_element_draw_table_row_column_lines(element,element->y,(element->y+(high+4)),1.0f);
}

void gui_dim3rtl_element_draw_table_line_header(element_type *element,int row_high)
{
	int			n,x,col_wid;
	d3col		col;
	
	x=element->x;

	col.r=col.g=col.b=0.0f;
	
	for (n=0;n!=element->setup.table.ncolumn;n++) {
		col_wid=(int)(element->setup.table.cols[n].percent_size*(float)element->wid);
		gui_dim3rtl_add_overlay_text((x+4),(element->y+(row_high>>1)),&col,1.0f,tx_left,element->setup.table.cols[n].name);
		x+=col_wid;
	}
}

void gui_dim3rtl_element_draw_table_line_background(element_type *element,int idx,int y,int row_high)
{
	d3col			col;

	memmove(&col,&iface.color.control.fill,sizeof(d3col));

	if (((idx+element->offset)&0x1)==0) {
		col.r*=element_gradient_factor_table_line_1;
		col.g*=element_gradient_factor_table_line_1;
		col.b*=element_gradient_factor_table_line_1;
	}
	else {
		col.r*=element_gradient_factor_table_line_2;
		col.g*=element_gradient_factor_table_line_2;
		col.b*=element_gradient_factor_table_line_2;
	}
	
	gui_dim3rtl_add_overlay_box_color((element->x+1),y,(element->wid-2),row_high,FALSE,FALSE,&col,NULL,1.0f);
}

void gui_dim3rtl_element_draw_table_line_data_text(int x,int y,int row_high,d3col *txt_col,char *txt)
{
	int				dy,high;
	char			*c;
	
	high=gl_text_get_char_height(iface.font.text_size_medium);
	
		// any returns?
		
	c=strchr(txt,'\n');
	if (c==NULL) {
		dy=y+((row_high>>1)-3);
		gui_dim3rtl_add_overlay_text(x,(dy+2),txt_col,1.0f,tx_left,txt);
	}
	else {
		*c=0x0;
		
		dy=y+(high+8);
		gui_dim3rtl_add_overlay_text(x,dy,txt_col,1.0f,tx_left,txt);
		gui_dim3rtl_add_overlay_text(x,(dy+(high+2)),txt_col,1.0f,tx_left,(c+1));
	}
}

void gui_dim3rtl_element_draw_table_line_data(element_type *element,int x,int y,int row,int wid,int row_high,d3col *txt_col,char *data)
{
	int				n,dx,dy,tx,col_wid,bitmap_sz,
					material_id;
	char			*c,*c2,txt[256];
	bool			first_col,checked;
	d3col			col;

	dx=x+4;
	dy=y+((row_high>>1)-3);

		// data column

	c=data;
	first_col=TRUE;
	
	for (n=0;n!=element->setup.table.ncolumn;n++) {

			// checkboxes

		if ((element->setup.table.checkbox) && (n==0)) {

			col_wid=(int)(element->setup.table.cols[n].percent_size*(float)wid);

			checked=FALSE;
			if ((row>=0) && (row<element_table_max_check)) checked=element->setup.table.checks[row];

			gui_dim3rtl_element_draw_checkbox_control((dx+4),dy,checked,TRUE,FALSE);

			dx+=col_wid;
			continue;
		}
	
			// get current data
			
		strncpy(txt,c,255);
		txt[255]=0x0;
		c2=strchr(txt,'\t');
		if (c2!=NULL) *c2=0x0;
		
			// text draw spot
			
		tx=dx;

			// draw any bitmaps

		if ((element->setup.table.bitmap_mode!=element_table_bitmap_none) && (first_col)) {

			bitmap_sz=(int)(((float)iface.scale_x)*element_table_bitmap_size);

				// draw bitmap

			material_id=element_draw_table_get_image_gl_id(element,row);
			if (material_id!=-1) {
				gui_dim3rtl_add_overlay_box_material(dx,(y+2),bitmap_sz,bitmap_sz,1.0f,material_id);
			}

				// missing graphic

			else {
				col.r=col.g=col.b=0.4f;
				gui_dim3rtl_add_overlay_box_color(dx,(y+2),bitmap_sz,bitmap_sz,TRUE,FALSE,&col,&iface.color.control.outline,1.0f);

				col.r=col.g=col.b=1.0f;
				gui_dim3rtl_add_overlay_text((dx+(bitmap_sz>>1)),(y+(bitmap_sz>>1)),&col,1.0f,tx_center,"?");
			}

				// get to correct text

			c2=strchr(txt,';');
			if (c2!=NULL) {
				c2=strchr((c2+1),';');
				if (c2!=NULL) {
					strcpy(txt,(c2+1));
				}
			}

			tx+=(bitmap_sz+4);
		}

		first_col=FALSE;
		
			// draw text
			
		gui_dim3rtl_element_draw_table_line_data_text(tx,y,row_high,txt_col,txt);
		
			// get next data
			
		c=strchr(c,'\t');
		if (c==NULL) break;
		c++;
		
		dx+=(int)(element->setup.table.cols[n].percent_size*(float)wid);
	}
}

void gui_dim3rtl_element_draw_table(element_type *element,int sel_id)
{
	int				n,y,wid,cnt,vis_cnt,
					title_high,row_high,
					by,last_idx;
	char			*c;
	bool			up_ok,down_ok;
	rtl2DPoint		top_lft_pnt,bot_rgt_pnt;

		// sizes
	
	wid=element->wid;
	title_high=gl_text_get_char_height(iface.font.text_size_medium)+2;
	row_high=element_get_table_row_high(element);
	
		// get element counts
		
	vis_cnt=cnt=((element->high-(title_high+4))/row_high);
	if (((element->high-(title_high+4))%row_high)!=0) vis_cnt--;
	
		// header fill
		
	gui_dim3rtl_element_draw_table_header_fill(element,title_high);

		// outline

	y=element->y+(title_high+3);

	gui_dim3rtl_add_overlay_line_color(element->x,y,(element->x+element->wid),y,&iface.color.control.outline,1.0f);
	gui_dim3rtl_add_overlay_box_outline(element->x,element->y,element->wid,element->high,(element->id==sel_id),&iface.color.control.outline,1.0f);

		// header
		
	gui_dim3rtl_element_draw_table_line_header(element,title_high);
	
		// items

	if (element->data!=NULL) {

			// clip inside the table
			
		rtlSceneOverlaySaveClip(view_rtl_gui_scene_id);

		top_lft_pnt.x=(element->x+1);
		top_lft_pnt.y=((element->y+1)+(title_high+4));
		bot_rgt_pnt.x=(element->x+element->wid)-1;
		bot_rgt_pnt.y=(element->y+element->high)-1;
		rtlSceneOverlaySetClip(view_rtl_gui_scene_id,&top_lft_pnt,&bot_rgt_pnt);

			// draw the data lines

		last_idx=0;
		y=(element->y+1)+(title_high+4);
		
		c=element->data+(element->offset*128);
		
		for (n=0;n<=cnt;n++) {
			if (*c==0x0) {
				last_idx=n;
				break;
			}
			
				// selection or background

			if (((n+element->offset)==element->value) && (!element->setup.table.checkbox)) {
				gui_dim3rtl_add_overlay_box_gradient((element->x+1),y,(element->wid-2),row_high,FALSE,FALSE,&iface.color.control.hilite,NULL,1.0f);
			}
			else {
				gui_dim3rtl_element_draw_table_line_background(element,n,y,row_high);
			}

			gui_dim3rtl_element_draw_table_row_column_lines(element,y,(y+row_high),0.5f);

				// table line data
				
			gui_dim3rtl_element_draw_table_line_data(element,element->x,y,(element->offset+n),wid,row_high,&iface.color.control.text,c);
			
			c+=128;
			y+=row_high;
		}

			// finish with blank rows

		by=element->y+element->high;

		while (y<by) {
			gui_dim3rtl_element_draw_table_line_background(element,last_idx,y,row_high);
			gui_dim3rtl_element_draw_table_row_column_lines(element,y,(y+row_high),0.5f);
			
			last_idx++;
			y+=row_high;
		}

		rtlSceneOverlayRestoreClip(view_rtl_gui_scene_id);
	}

		// scroll controls

	up_ok=(element->offset!=0);
	down_ok=((element->offset+(vis_cnt+1))<element_get_table_row_count(element));

	gui_dim3rtl_element_draw_scroll_controls(element,title_high,up_ok,down_ok);
	
		// busy
/* supergumba		
	if (element->setup.table.busy) element_draw_table_busy(element);
	*/
}

/* =======================================================

      Element Tabs and Frames
      
======================================================= */

void gui_dim3rtl_element_draw_tab(element_type *element,int sel_id)
{
	int				n,x,y,xadd,max_sz,mouse_idx,tab_idx,
					lx,rx,ty,by,high,margin;
	int				tab_draw_list[max_element_tab];
	
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
		
	high=element->high;
	
	ty=element->y;
	by=ty+high;
	
	for (n=0;n!=element->setup.tab.ntab;n++) {
	
		tab_idx=tab_draw_list[n];

		lx=element->x+(xadd*tab_idx);
		rx=lx+xadd;
		
		if (element->value!=tab_idx) {
			gui_dim3rtl_add_overlay_box_color(lx,ty,xadd,high,FALSE,FALSE,&iface.color.tab.dimmed,NULL,1.0f);
		}
		else {
			gui_dim3rtl_add_overlay_box_color(lx,ty,xadd,high,FALSE,FALSE,&iface.color.tab.background,NULL,1.0f);
		}

		gui_dim3rtl_add_overlay_line_color(lx,by,lx,ty,&iface.color.tab.outline,1.0f);
		gui_dim3rtl_add_overlay_line_color(lx,ty,rx,ty,&iface.color.tab.outline,1.0f);
		gui_dim3rtl_add_overlay_line_color(rx,ty,rx,by,&iface.color.tab.outline,1.0f);
		
		if (element->value==tab_idx) {
			gui_dim3rtl_add_overlay_text((lx+(xadd>>1)),(ty+(high>>1)),&iface.color.tab.text,1.0f,tx_center,element->setup.tab.name[tab_idx]);
		}
		else {
			if (mouse_idx==tab_idx) {
				gui_dim3rtl_add_overlay_text((lx+(xadd>>1)),(ty+(high>>1)),&iface.color.tab.text_mouse_over,1.0f,tx_center,element->setup.tab.name[tab_idx]);
			}
			else {
				gui_dim3rtl_add_overlay_text((lx+(xadd>>1)),(ty+(high>>1)),&iface.color.tab.text_dimmed,1.0f,tx_center,element->setup.tab.name[tab_idx]);
			}
		}
	}

		// final underline

	lx=element->x+(xadd*element->value);
	rx=lx+xadd;

	if (element->value!=0) gui_dim3rtl_add_overlay_line_color(element->x,by,lx,by,&iface.color.tab.outline,1.0f);
	gui_dim3rtl_add_overlay_line_color(rx,by,(element->x+element->wid),by,&iface.color.tab.outline,1.0f);
}

void gui_dim3rtl_element_draw_frame(element_type *element)
{
	int				x,y,wid,high,title_high;
	
		// setup draw
		
	gui_dim3rtl_element_get_box(element,&x,&y,&wid,&high);
	
		// inside frame
		
	gui_dim3rtl_add_overlay_box_color(x,y,wid,high,TRUE,FALSE,&iface.color.dialog.background,&iface.color.dialog.outline,1.0f);
	
		// header
		
	if (element->str[0]!=0x0) {
		title_high=element_get_frame_title_high();
	
		if (element->enabled) {
			gui_dim3rtl_add_overlay_box_gradient(x,y,wid,title_high,TRUE,FALSE,&iface.color.dialog.header,&iface.color.dialog.outline,1.0f);
		}
		else {
			gui_dim3rtl_add_overlay_box_gradient(x,y,wid,title_high,TRUE,FALSE,&iface.color.dialog.background,&iface.color.dialog.outline,1.0f);
		}
	
		gui_dim3rtl_add_overlay_text((x+(element->wid>>1)),(y+(title_high>>1)),&iface.color.dialog.title,1.0f,tx_center,element->str);
	}
}

/* =======================================================

      RTL Elements Mainline
      
======================================================= */

void gui_dim3rtl_elements_draw(bool show_cursor)
{
	int					n,id;
	element_type		*element;

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
				gui_dim3rtl_element_draw_checkbox(element,id);
				break;
			case element_type_combo:
				gui_dim3rtl_element_draw_combo(element,id);
				break;
			case element_type_slider:
				gui_dim3rtl_element_draw_slider(element,id);
				break;
			case element_type_table:
				gui_dim3rtl_element_draw_table(element,id);
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
	
		// draw open combo
		
	if (element_open_combo_id!=-1) {
		gui_dim3rtl_element_draw_combo_open(element_find(element_open_combo_id));
	}
}


