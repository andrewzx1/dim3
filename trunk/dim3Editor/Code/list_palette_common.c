/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: List Palette Common

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
#include "interface.h"
#include "view.h"
#include "dialog.h"

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;
extern file_path_setup_type		file_path_setup;

bool							list_palette_open;

/* =======================================================

      Item Palette Setup
      
======================================================= */

void list_palette_initialize(void)
{
	list_palette_open=TRUE;
}

void list_palette_shutdown(void)
{
}

void list_palette_list_initialize(list_palette_type *list,char *title)
{
	strcpy(list->title,title);

	list->scroll_page=0;
	list->total_high=0;

	list->item_type=-1;
	list->item_idx=-1;

	list->items=(list_palette_item_type*)malloc(list_max_item_count*sizeof(list_palette_item_type));
}

void list_palette_list_shutdown(list_palette_type *list)
{
	if (list->items!=NULL) free(list->items);
}

/* =======================================================

      List Add Items
      
======================================================= */

list_palette_item_type* list_palette_create_item(list_palette_type *list,int ctrl_type)
{
	list_palette_item_type		*item;

	item=&list->items[list->item_count];
	list->item_count++;

	item->ctrl_type=ctrl_type;

	item->x=list->box.lx+(list_palette_border_sz+4);
	if (ctrl_type!=list_item_ctrl_header) item->x+=10;

	item->y=((list->item_count*list_item_font_high)+(list_scroll_button_high+list_title_high))+list->box.ty;
	item->y-=(list->scroll_page*list->scroll_size);

	list->total_high+=list_item_font_high;

	return(item);
}

void list_palette_add_header(list_palette_type *list,int piece_type,char *name)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_header);

	item->type=piece_type;
	item->idx=-1;
	item->id=-1;

	item->selected=FALSE;

	strcpy(item->name,name);
}

void list_palette_add_item(list_palette_type *list,int piece_type,int piece_idx,char *name,bool selected)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_text);

	item->type=piece_type;
	item->idx=piece_idx;
	item->id=-1;

	item->selected=selected;

	strcpy(item->name,name);
}

void list_palette_add_color(list_palette_type *list,int piece_type,int piece_idx,d3col *col,bool selected)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_color);

	item->type=piece_type;
	item->idx=piece_idx;
	item->id=-1;

	item->selected=selected;

	memmove(&item->value.col,col,sizeof(d3col));
}

void list_palette_add_string(list_palette_type *list,int id,char *name,char *value)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_string);

	item->type=-1;
	item->idx=-1;
	item->id=id;

	item->selected=FALSE;

	strcpy(item->name,name);
	strcpy(item->value.str,value);
}

void list_palette_add_string_int(list_palette_type *list,int id,char *name,int value)
{
	char		str[32];
	
	sprintf(str,"%d",value);
	list_palette_add_string(list,id,name,str);
}

void list_palette_add_string_float(list_palette_type *list,int id,char *name,float value)
{
	char		str[32];
	
	sprintf(str,"%.2f",value);
	list_palette_add_string(list,id,name,str);
}

void list_palette_add_checkbox(list_palette_type *list,int id,char *name,bool value)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_checkbox);

	item->type=-1;
	item->idx=-1;
	item->id=id;

	item->selected=FALSE;

	strcpy(item->name,name);
	item->value.checkbox=value;
}

void list_palette_add_pick_color(list_palette_type *list,int id,char *name,d3col *col)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_pick_color);

	item->type=-1;
	item->idx=-1;
	item->id=id;

	item->selected=FALSE;

	strcpy(item->name,name);
	memmove(&item->value.col,col,sizeof(d3col));
}

/* =======================================================

      List Delete Items
      
======================================================= */

void list_palette_delete_all_items(list_palette_type *list)
{
	list->item_count=0;
	list->total_high=0;
}

/* =======================================================

      List Palette Draw Utilities
      
======================================================= */

void list_palette_draw_item_color_box(list_palette_type *list,list_palette_item_type *item,bool right_align,d3col *col)
{
	int					x;

	x=item->x;
	if (right_align) x=list->box.rx-(list_item_font_high+4);

	glColor4f((col->r*0.5f),(col->g*0.5f),(col->b*0.5f),1.0f);

	glBegin(GL_QUADS);
	glVertex2i(x,((item->y-list_item_font_high)+2));
	glVertex2i((x+list_item_font_high),((item->y-list_item_font_high)+2));
	glVertex2i((x+list_item_font_high),(item->y-2));
	glVertex2i(x,(item->y-2));
	glEnd();

	glColor4f(col->r,col->g,col->b,1.0f);
			
	glBegin(GL_QUADS);
	glVertex2i((x+1),((item->y-list_item_font_high)+3));
	glVertex2i(((x+list_item_font_high)-1),((item->y-list_item_font_high)+3));
	glVertex2i(((x+list_item_font_high)-1),(item->y-3));
	glVertex2i((x+1),(item->y-3));
	glEnd();
}

void list_palette_draw_item_check_box(list_palette_type *list,list_palette_item_type *item,bool checked)
{
	int					x;

	x=list->box.rx-(list_item_font_high+4);

	glBegin(GL_QUADS);
	glColor4f(0.7f,0.7f,0.7f,1.0f);
	glVertex2i(x,((item->y-list_item_font_high)+2));
	glVertex2i((x+list_item_font_high),((item->y-list_item_font_high)+2));
	glColor4f(0.5f,0.5f,0.5f,1.0f);
	glVertex2i((x+list_item_font_high),(item->y-2));
	glVertex2i(x,(item->y-2));
	glEnd();

	if (checked) {
		glBegin(GL_QUADS);
		glColor4f(0.0f,0.9f,0.0f,1.0f);
		glVertex2i((x+2),((item->y-list_item_font_high)+4));
		glVertex2i(((x+list_item_font_high)-2),((item->y-list_item_font_high)+4));
		glColor4f(0.0f,1.0f,0.0f,1.0f);
		glVertex2i(((x+list_item_font_high)-2),(item->y-4));
		glVertex2i((x+2),(item->y-4));
		glEnd();
	}
}

/* =======================================================

      List Palette Draw
      
======================================================= */

void list_palette_draw_item(list_palette_type *list,int idx)
{
	list_palette_item_type		*item;
	
	item=&list->items[idx];

		// early exits

	if ((item->y<(list->box.ty+(list_scroll_button_high+list_title_high))) || ((item->y-list_item_font_high)>(list->box.by-list_scroll_button_high))) return;

		// draw header
		
	if (item->ctrl_type==list_item_ctrl_header) {
	
		glBegin(GL_QUADS);
		glColor4f(0.5f,0.5f,0.9f,1.0f);
		glVertex2i(list->box.lx,(item->y-list_item_font_high));
		glVertex2i(list->box.rx,(item->y-list_item_font_high));
		glColor4f(0.5f,0.5f,0.8f,1.0f);
		glVertex2i(list->box.rx,item->y);
		glVertex2i(list->box.lx,item->y);
		glEnd();
		
		glColor4f(0.0f,0.0f,0.0f,1.0f);
		
		glBegin(GL_LINES);
		glVertex2i(list->box.lx,(item->y-list_item_font_high));
		glVertex2i(list->box.rx,(item->y-list_item_font_high));
		glVertex2i(list->box.lx,item->y);
		glVertex2i(list->box.rx,item->y);
		glEnd();
	
	}
		
		// draw selected item
	
	else {

		if (item->selected) {
			glColor4f(1.0f,1.0f,0.0f,1.0f);
		}
		else {
			if ((idx%2)==0) {
				glColor4f(1.0f,1.0f,1.0f,1.0f);
			}
			else {
				glColor4f(0.95f,0.95f,0.95f,1.0f);
			}
		}
			
		glBegin(GL_QUADS);
		glVertex2i(list->box.lx,((item->y-list_item_font_high)+1));
		glVertex2i(list->box.rx,((item->y-list_item_font_high)+1));
		glVertex2i(list->box.rx,item->y);
		glVertex2i(list->box.lx,item->y);
		glEnd();
	}

		// draw item

	switch (item->ctrl_type) {

			// text or header

		case list_item_ctrl_header:
		case list_item_ctrl_text:
			text_draw(item->x,item->y,list_item_font_size,item->name);
			break;

			// color

		case list_item_ctrl_color:
			list_palette_draw_item_color_box(list,item,FALSE,&item->value.col);
			break;

			// string

		case list_item_ctrl_string:
			text_draw(item->x,item->y,list_item_font_size,item->name);
			text_draw_right((list->box.rx-4),item->y,list_item_font_size,item->value.str);
			break;

			// checkbox

		case list_item_ctrl_checkbox:
			text_draw(item->x,item->y,list_item_font_size,item->name);
			list_palette_draw_item_check_box(list,item,item->value.checkbox);
			break;

			// pick color

		case list_item_ctrl_pick_color:
			text_draw(item->x,item->y,list_item_font_size,item->name);
			list_palette_draw_item_color_box(list,item,TRUE,&item->value.col);
			break;

	}
}

void list_palette_draw(list_palette_type *list)
{
	int						n,lx,mx,rx,ty,by;
	d3rect					wbox;

		// viewport setup
		
	os_get_window_box(&wbox);

	glEnable(GL_SCISSOR_TEST);
	glScissor(list->box.lx,(wbox.by-list->box.by),(list->box.rx-list->box.lx),(list->box.by-list->box.ty));

	glViewport(list->box.lx,(wbox.by-list->box.by),(list->box.rx-list->box.lx),(list->box.by-list->box.ty));
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)list->box.lx,(GLdouble)list->box.rx,(GLdouble)list->box.by,(GLdouble)list->box.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		// draw setup

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

		// background

	glColor4f(1.0f,1.0f,1.0f,1.0f);
		
	glBegin(GL_QUADS);
	glVertex2i(list->box.lx,list->box.ty);
	glVertex2i(list->box.rx,list->box.ty);
	glVertex2i(list->box.rx,list->box.by);
	glVertex2i(list->box.lx,list->box.by);
	glEnd();

		// title

	ty=list->box.ty;
	by=ty+list_title_high;

	glBegin(GL_QUADS);
	glColor4f(0.6f,0.4f,0.6f,1.0f);
	glVertex2i(list->box.lx,ty);
	glVertex2i(list->box.rx,ty);
	glColor4f(0.6f,0.4f,0.7f,1.0f);
	glVertex2i(list->box.rx,by);
	glVertex2i(list->box.lx,by);
	glEnd();
	
	text_draw_center(((list->box.lx+list->box.rx)>>1),(by-2),list_title_font_size,list->title);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	
	glBegin(GL_LINES);
	glVertex2i(list->box.lx,(ty+1));
	glVertex2i(list->box.rx,(ty+1));
	glVertex2i(list->box.lx,by);
	glVertex2i(list->box.rx,by);
	glEnd();

		// items

	for (n=0;n!=list->item_count;n++) {
		list_palette_draw_item(list,n);
	}

		// scroll up button

	ty=list->box.ty+list_title_high;
	by=ty+list_scroll_button_high;
	mx=(list->box.lx+list->box.rx)>>1;

	glColor4f(0.9f,0.9f,0.9f,1.0f);
		
	glBegin(GL_QUADS);
	glVertex2i(list->box.lx,ty);
	glVertex2i(list->box.rx,ty);
	glVertex2i(list->box.rx,by);
	glVertex2i(list->box.lx,by);
	glEnd();

	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	glBegin(GL_LINES);
	glVertex2i(list->box.lx,ty);
	glVertex2i(list->box.rx,ty);
	glVertex2i(list->box.lx,by);
	glVertex2i(list->box.rx,by);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor4f(0.2f,0.2f,1.0f,1.0f);
	glVertex2i(mx,(ty+5));
	glColor4f(0.0f,0.0f,1.0f,1.0f);
	glVertex2i((mx+10),(by-5));
	glVertex2i((mx-10),(by-5));
	glEnd();

		// scroll down button

	ty=list->box.by-list_scroll_button_high;
	by=list->box.by;
	mx=(list->box.lx+list->box.rx)>>1;

	glColor4f(0.9f,0.9f,0.9f,1.0f);
		
	glBegin(GL_QUADS);
	glVertex2i(list->box.lx,ty);
	glVertex2i(list->box.rx,ty);
	glVertex2i(list->box.rx,by);
	glVertex2i(list->box.lx,by);
	glEnd();

	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	glBegin(GL_LINES);
	glVertex2i(list->box.lx,ty);
	glVertex2i(list->box.rx,ty);
	glVertex2i(list->box.lx,by);
	glVertex2i(list->box.rx,by);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor4f(0.2f,0.2f,1.0f,1.0f);
	glVertex2i(mx,(by-5));
	glColor4f(0.0f,0.0f,1.0f,1.0f);
	glVertex2i((mx+10),(ty+5));
	glVertex2i((mx-10),(ty+5));
	glEnd();
	
		// click-close border
	
	lx=list->box.lx;
	rx=list->box.lx+list_palette_border_sz;
	mx=(lx+rx)>>1;
		
	glBegin(GL_QUADS);
	glColor4f(0.0f,0.0f,0.5f,1.0f);
	glVertex2i(lx,list->box.by);
	glVertex2i(lx,list->box.ty);
	glColor4f(0.0f,0.0f,1.0f,1.0f);
	glVertex2i(mx,list->box.ty);
	glVertex2i(mx,list->box.by);
	glColor4f(0.0f,0.0f,0.5f,1.0f);
	glVertex2i(rx,list->box.by);
	glVertex2i(rx,list->box.ty);
	glColor4f(0.0f,0.0f,1.0f,1.0f);
	glVertex2i(mx,list->box.ty);
	glVertex2i(mx,list->box.by);
	glEnd();

	glDisable(GL_ALPHA_TEST);
}

/* =======================================================

      List Palette Scrolling
      
======================================================= */

void list_palette_scroll_up(list_palette_type *list)
{
	if (list->scroll_page>0) {
		list->scroll_page--;
		main_wind_draw();
	}
}

void list_palette_scroll_down(list_palette_type *list)
{
	if (list->scroll_page<(list->total_high/list->scroll_size)) {
		list->scroll_page++;
		main_wind_draw();
	}
}

void list_palette_scroll_wheel(list_palette_type *list,d3pnt *pnt,int move)
{
	if (move>0) list_palette_scroll_up(list);
	if (move<0) list_palette_scroll_down(list);
}

/* =======================================================

      Item Palette Click
      
======================================================= */

bool list_palette_click(list_palette_type *list,d3pnt *pnt,bool double_click)
{
	int						item_idx;
	list_palette_item_type	*item;

	pnt->x-=list->box.lx;
	pnt->y-=list->box.ty;

		// click in close border

	if (pnt->x<=list_palette_border_sz) {
		list_palette_open=!list_palette_open;
		main_wind_draw();
		return(FALSE);
	}

		// click in scroll item

	if ((pnt->y>=list_title_high) && (pnt->y<=(list_title_high+list_scroll_button_high))) {
		list_palette_scroll_up(list);
		return(FALSE);
	}

	if (pnt->y>=((list->box.by-list->box.ty)-list_scroll_button_high)) {
		list_palette_scroll_down(list);
		return(FALSE);
	}

		// click in item

	item_idx=((pnt->y-(list_scroll_button_high+list_title_high))+(list->scroll_page*list->scroll_size))/list_item_font_high;
	if ((item_idx<0) || (item_idx>=list->item_count)) return(FALSE);

		// clicking in header

	item=&list->items[item_idx];

		// select clicked item

	list->item_type=item->type;
	list->item_idx=item->idx;
	list->item_id=item->id;

	return(TRUE);
}