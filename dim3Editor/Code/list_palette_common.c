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

void list_palette_list_initialize(list_palette_type *list,int section_count)
{
	int				n;

	list->section_count=section_count;

	for (n=0;n!=list->section_count;n++) {
		list->section_open[n]=FALSE;
	}

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

      List Palette Items
      
======================================================= */

void list_palette_add_item(list_palette_type *list,int piece_type,int piece_idx,char *name,d3col *col,bool selected,bool header)
{
	list_palette_item_type		*item;

	item=&list->items[list->item_count];
	list->item_count++;

	item->x=list->box.lx+(list_palette_border_sz+4);
	if (!header) item->x+=10;

	item->y=((list->item_count*list_item_font_high)+list_scroll_button_high)+list->box.ty;
	item->y-=(list->scroll_page*list->page_high);

	list->total_high+=list_item_font_high;

	item->piece_type=piece_type;
	item->piece_idx=piece_idx;
	item->selected=selected;
	item->header=header;

	if (name!=NULL) {
		strcpy(item->name,name);
	}
	else {
		item->name[0]=0x0;
	}

	if (col!=NULL) {
		item->is_col=TRUE;
		memmove(&item->col,col,sizeof(d3col));
	}
	else {
		item->is_col=FALSE;
		item->col.r=item->col.g=item->col.b=0.0f;
	}
}

void list_palette_delete_all_items(list_palette_type *list)
{
	list->item_count=0;
	list->total_high=0;
}

/* =======================================================

      List Palette Draw
      
======================================================= */

void list_palette_draw_item(list_palette_type *list,list_palette_item_type *item)
{
		// early exits

	if ((item->y<(list->box.ty+list_scroll_button_high)) || ((item->y-list_item_font_high)>(list->box.by-list_scroll_button_high))) return;

		// draw header
		
	if (item->header) {
	
		glBegin(GL_QUADS);
		glColor4f(0.9f,0.9f,0.9f,1.0f);
		glVertex2i(list->box.lx,(item->y-list_item_font_high));
		glVertex2i(list->box.rx,(item->y-list_item_font_high));
		glColor4f(0.8f,0.8f,0.8f,1.0f);
		glVertex2i(list->box.rx,item->y);
		glVertex2i(list->box.lx,item->y);
		glEnd();
		
		glColor4f(0.0f,0.0f,0.0f,1.0f);
		
		glBegin(GL_LINES);
		glVertex2i(list->box.lx,(item->y-list_item_font_high));
		glVertex2i(list->box.rx,(item->y-list_item_font_high));
		glVertex2i(list->box.lx,(item->y+1));
		glVertex2i(list->box.rx,(item->y+1));
		glEnd();
	
	}
		
		// draw selected item
	
	else {

		if (item->selected) {
			glColor4f(1.0f,1.0f,0.0f,1.0f);
			
			glBegin(GL_QUADS);
			glVertex2i(list->box.lx,(item->y-list_item_font_high));
			glVertex2i(list->box.rx,(item->y-list_item_font_high));
			glVertex2i(list->box.rx,item->y);
			glVertex2i(list->box.lx,item->y);
			glEnd();
		}
	}

		// draw text

	if (!item->is_col) {
		text_draw(item->x,(item->y-1),list_item_font_size,FALSE,item->name);
	}

		// draw color

	else {
		glColor4f(item->col.r,item->col.g,item->col.b,1.0f);
		
		glBegin(GL_QUADS);
		glVertex2i(item->x,((item->y-list_item_font_high)+1));
		glVertex2i((item->x+list_item_font_high),((item->y-list_item_font_high)+1));
		glVertex2i((item->x+list_item_font_high),(item->y-1));
		glVertex2i(item->x,(item->y-1));
		glEnd();

		glDisable(GL_BLEND);

		glColor4f(0.0f,0.0f,0.0f,1.0f);

		glBegin(GL_LINE_LOOP);
		glVertex2i(item->x,((item->y-list_item_font_high)+1));
		glVertex2i((item->x+list_item_font_high),((item->y-list_item_font_high)+1));
		glVertex2i((item->x+list_item_font_high),(item->y-1));
		glVertex2i(item->x,(item->y-1));
		glEnd();

		glEnable(GL_BLEND);
	}
}

void list_palette_draw(list_palette_type *list)
{
	int						n,lx,mx,rx,ty,by;
	d3rect					wbox;
	list_palette_item_type	*item;

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		// background

	glColor4f(1.0f,1.0f,1.0f,1.0f);
		
	glBegin(GL_QUADS);
	glVertex2i(list->box.lx,list->box.ty);
	glVertex2i(list->box.rx,list->box.ty);
	glVertex2i(list->box.rx,list->box.by);
	glVertex2i(list->box.lx,list->box.by);
	glEnd();

		// items

	item=list->items;

	for (n=0;n!=list->item_count;n++) {
		list_palette_draw_item(list,item);
		item++;
	}

		// scroll up button

	ty=list->box.ty;
	by=list->box.ty+list_scroll_button_high;
	mx=(list->box.lx+list->box.rx)>>1;

	glColor4f(0.9f,0.9f,0.9f,1.0f);
		
	glBegin(GL_QUADS);
	glVertex2i(list->box.lx,ty);
	glVertex2i(list->box.rx,ty);
	glVertex2i(list->box.rx,by);
	glVertex2i(list->box.lx,by);
	glEnd();

	glDisable(GL_BLEND);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	glBegin(GL_LINE_LOOP);
	glVertex2i(list->box.lx,ty);
	glVertex2i(list->box.rx,ty);
	glVertex2i(list->box.rx,by);
	glVertex2i(list->box.lx,by);
	glEnd();

	glEnable(GL_BLEND);

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

	glDisable(GL_BLEND);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	glBegin(GL_LINE_LOOP);
	glVertex2i(list->box.lx,ty);
	glVertex2i(list->box.rx,ty);
	glVertex2i(list->box.rx,by);
	glVertex2i(list->box.lx,by);
	glEnd();

	glEnable(GL_BLEND);

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
	glDisable(GL_BLEND);
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
	if (list->scroll_page<(list->total_high/list->page_high)) {
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

	if (pnt->y<=list_scroll_button_high) {
		list_palette_scroll_up(list);
		return(FALSE);
	}

	if (pnt->y>=((list->box.by-list->box.ty)-list_scroll_button_high)) {
		list_palette_scroll_down(list);
		return(FALSE);
	}

		// click in item

	item_idx=((pnt->y-list_scroll_button_high)+(list->scroll_page*list->page_high))/list_item_font_high;
	if ((item_idx<0) || (item_idx>=list->item_count)) return(FALSE);

		// clicking in header

	item=&list->items[item_idx];

	if (item->piece_idx==-1) {
		list->section_open[item->piece_type]=!list->section_open[item->piece_type];
		main_wind_draw();
		return(FALSE);
	}

		// select clicked item

	list->item_type=item->piece_type;
	list->item_idx=item->piece_idx;

	return(TRUE);
}