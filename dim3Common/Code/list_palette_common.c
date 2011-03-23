/****************************** File *********************************

Module: dim3 Common
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
	#ifdef D3_EDITOR
		#include "dim3editor.h"
	#endif
	#ifdef D3_ANIMATOR
		#include "dim3Animator.h"
	#endif
	#ifdef D3_SETUP
		#include "dim3Setup.h"
	#endif
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

extern file_path_setup_type		file_path_setup;

bool							list_palette_open;
bitmap_type						list_bitmaps[4];

/* =======================================================

      Item Palette Setup
      
======================================================= */

void list_palette_initialize(char *app_name)
{
	int				n;
	char			sub_path[1024],path[1024];
	char			btn_names[4][32]={"Edit","Plus","Minus","Set"};

		// start open

	list_palette_open=TRUE;

		// load buttons

	os_get_support_file_path(sub_path,app_name);
	strcat(sub_path,"/Lists");
		
	for (n=0;n!=4;n++) {
		file_paths_app(&file_path_setup,path,sub_path,btn_names[n],"png");
		bitmap_open(&list_bitmaps[n],path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);
	}
}

void list_palette_shutdown(void)
{
	int				n;

	for (n=0;n!=4;n++) {
		bitmap_close(&list_bitmaps[n]);
	}
}

void list_palette_list_initialize(list_palette_type *list,char *title)
{
	strcpy(list->title,title);

	list->scroll_page=0;
	list->total_high=0;

	list->push_on=FALSE;
	list->push_idx=-1;

	list->item_type=-1;
	list->item_idx=-1;

	list->items=(list_palette_item_type*)malloc(list_max_item_count*sizeof(list_palette_item_type));
}

void list_palette_list_shutdown(list_palette_type *list)
{
	if (list->items!=NULL) free(list->items);
}

void list_palette_set_title(list_palette_type *list,char *title)
{
	strcpy(list->title,title);
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
	item->button_type=list_button_none;

	item->x=list->box.lx+(list_palette_border_sz+4);
	if (ctrl_type!=list_item_ctrl_header) item->x+=10;

	item->y=((list->item_count*list_item_font_high)+list_title_high)+list->box.ty;
	item->y-=(list->scroll_page*list_item_scroll_size);

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
	item->count=-1;

	item->selected=FALSE;

	strcpy(item->name,name);
}

void list_palette_add_header_count(list_palette_type *list,int piece_type,char *name,int count)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_header);

	item->type=piece_type;
	item->idx=-1;
	item->id=-1;
	item->count=count;

	item->selected=FALSE;

	strcpy(item->name,name);
}

void list_palette_add_header_button(list_palette_type *list,int id,char *name,int button_type)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_header);

	item->type=-1;
	item->idx=-1;
	item->id=-1;
	item->button_id=id;
	item->count=-1;
	item->button_type=button_type;

	item->selected=FALSE;

	strcpy(item->name,name);
}

void list_palette_add_item(list_palette_type *list,int piece_type,int piece_idx,char *name,bool selected,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_text);

	item->type=piece_type;
	item->idx=piece_idx;
	item->id=-1;

	item->selected=selected;
	item->disabled=disabled;

	strcpy(item->name,name);
}

void list_palette_add_color(list_palette_type *list,int piece_type,int piece_idx,d3col *col,bool selected,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_color);

	item->type=piece_type;
	item->idx=piece_idx;
	item->id=-1;

	item->selected=selected;
	item->disabled=disabled;

	memmove(&item->value.col,col,sizeof(d3col));
}

void list_palette_add_string_selectable(list_palette_type *list,int id,char *name,char *value,bool selected,bool disabled)
{
	list_palette_item_type		*item;
	
	item=list_palette_create_item(list,list_item_ctrl_string);

	item->type=-1;
	item->idx=-1;
	item->id=id;

	item->selected=selected;
	item->disabled=disabled;

	strcpy(item->name,name);

	if (value==NULL) {
		item->value.str[0]=0x0;
		return;
	}
	
	if (strlen(value)>=25) {
		strncpy(item->value.str,value,25);
		strcpy((char*)&item->value.str[25],"...");
	}
	else {
		strcpy(item->value.str,value);
	}
}

void list_palette_add_string_selectable_button(list_palette_type *list,int id,int button_type,int button_id,char *name,char *value,bool selected,bool disabled)
{
	list_palette_item_type		*item;

		// add item

	list_palette_add_string_selectable(list,id,name,value,selected,disabled);

		// put in the button

	item=&list->items[list->item_count-1];
	item->button_type=button_type;
	item->button_id=button_id;
}

void list_palette_add_string(list_palette_type *list,int id,char *name,char *value,bool disabled)
{
	list_palette_add_string_selectable(list,id,name,value,FALSE,disabled);
}

void list_palette_add_string_int(list_palette_type *list,int id,char *name,int value,bool disabled)
{
	char		str[64];
	
	sprintf(str,"%d",value);
	list_palette_add_string(list,id,name,str,disabled);
}

void list_palette_add_string_float(list_palette_type *list,int id,char *name,float value,bool disabled)
{
	char		str[64];
	
	sprintf(str,"%.2f",value);
	list_palette_add_string(list,id,name,str,disabled);
}

void list_palette_add_checkbox(list_palette_type *list,int id,char *name,bool value,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_checkbox);

	item->type=-1;
	item->idx=-1;
	item->id=id;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	item->value.checkbox=value;
}

void list_palette_add_pick_color(list_palette_type *list,int id,char *name,d3col *col,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_pick_color);

	item->type=-1;
	item->idx=-1;
	item->id=id;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	memmove(&item->value.col,col,sizeof(d3col));
}

void list_palette_add_point(list_palette_type *list,int id,char *name,d3pnt *pnt,bool disabled)
{
	char		str[64];
	
	sprintf(str,"%d,%d,%d",pnt->x,pnt->y,pnt->z);
	list_palette_add_string(list,id,name,str,disabled);
}

void list_palette_add_angle(list_palette_type *list,int id,char *name,d3ang *ang,bool disabled)
{
	char		str[64];
	
	sprintf(str,"%.2f,%.2f,%.2f",ang->x,ang->y,ang->z);
	list_palette_add_string(list,id,name,str,disabled);
}

void list_palette_add_vector(list_palette_type *list,int id,char *name,d3vct *vct,bool disabled)
{
	char		str[64];
	
	sprintf(str,"%.2f,%.2f,%.2f",vct->x,vct->y,vct->z);
	list_palette_add_string(list,id,name,str,disabled);
}

void list_palette_add_uv(list_palette_type *list,int id,char *name,d3fpnt *fpnt,bool disabled)
{
	char		str[64];
	
	sprintf(str,"%.2f,%.2f",fpnt->x,fpnt->y);
	list_palette_add_string(list,id,name,str,disabled);
}

void list_palette_add_texture(list_palette_type *list,texture_type *textures,int id,char *name,int txt_idx,bool disabled)
{
	char		str[name_str_len];

	str[0]=0x0;

	if (txt_idx!=-1) {
		if (textures[txt_idx].frames[0].name[0]==0x0) {
			strcpy(str,"(none)");
		}
		else {
			strcpy(str,textures[txt_idx].frames[0].name);
		}
	}
	
	list_palette_add_string(list,id,name,str,disabled);
}

void list_palette_add_shader(list_palette_type *list,int id,char *name,char *shader_name,bool disabled)
{
	if (shader_name[0]==0x0) {
		list_palette_add_string(list,id,name,"Default",disabled);
		return;
	}
	
	list_palette_add_string(list,id,name,shader_name,disabled);
}

void list_palette_add_string_tag(list_palette_type *list,int id,char *name,unsigned long tag,bool disabled)
{
	char		str[32];
	
	memmove(str,&tag,4);
	str[4]=0x0;

	list_palette_add_string(list,id,name,str,disabled);
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

      List Sorting
      
======================================================= */

void list_palette_sort_mark_start(list_palette_type *list)
{
	list->item_sort_start_idx=list->item_count;
}

void list_palette_sort(list_palette_type *list)
{
	int						n,k,x,y;
	bool					shuffle;
	list_palette_item_type	temp_item;

	if (list->item_sort_start_idx>=(list->item_count-1)) return;

	while (TRUE) {

		shuffle=FALSE;

		for (n=list->item_sort_start_idx;n<(list->item_count-1);n++) {
			k=n+1;

			if (strcasecmp(list->items[n].name,list->items[k].name)>0) {
				shuffle=TRUE;

				memmove(&temp_item,&list->items[n],sizeof(list_palette_item_type));
				memmove(&list->items[n],&list->items[k],sizeof(list_palette_item_type));
				memmove(&list->items[k],&temp_item,sizeof(list_palette_item_type));

				x=list->items[n].x;
				list->items[n].x=list->items[k].x;
				list->items[k].x=x;

				y=list->items[n].y;
				list->items[n].y=list->items[k].y;
				list->items[k].y=y;
			}
		}

		if (!shuffle) break;
	}
}

/* =======================================================

      List Palette Scroll Pages
      
======================================================= */

int list_palette_get_scroll_page_count(list_palette_type *list)
{
	int				high;
	
	high=list->total_high-(list->box.by-(list->box.ty+list_title_high));
	if (high<=0) return(0);
	
	return((high/list_item_scroll_size)+1);
}

/* =======================================================

      List Palette Draw Utilities
      
======================================================= */

void list_palette_draw_item_color_box(list_palette_type *list,list_palette_item_type *item,d3col *col)
{
	int					x;

	x=list->box.rx-(list_item_font_high+(list_palette_scroll_wid+4));

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
	int					lx,rx,ty,by;

	lx=list->box.rx-(list_item_font_high+(list_palette_scroll_wid+2));
	rx=lx+(list_item_font_high-2);

	ty=(item->y-list_item_font_high)+2;
	by=item->y;

	glColor4f(0.0f,0.0f,0.0f,1.0f);

	glBegin(GL_QUADS);
	glVertex2i(lx,ty);
	glVertex2i(rx,ty);
	glVertex2i(rx,by);
	glVertex2i(lx,by);
	glEnd();

	glColor4f(0.9f,0.9f,0.9f,1.0f);

	glBegin(GL_QUADS);
	glVertex2i((lx+1),(ty+1));
	glVertex2i((rx-1),(ty+1));
	glVertex2i((rx-1),(by-1));
	glVertex2i((lx+1),(by-1));
	glEnd();

	if (checked) {
		glLineWidth(3.0f);

		glColor4f(0.0f,0.8f,0.0f,1.0f);

		glBegin(GL_LINES);
		glVertex2i((lx+1),(ty+1));
		glVertex2i((rx-1),(by-1));
		glVertex2i((rx-1),(ty+1));
		glVertex2i((lx+1),(by-1));
		glEnd();

		glLineWidth(1.0f);
	}
}

void list_palette_draw_item_string(list_palette_type *list,list_palette_item_type *item)
{
	int							rx;
	d3col						col;

	rx=list->box.rx-(list_palette_scroll_wid+4);
	if (item->button_type!=list_button_none) rx-=(list_item_font_high+2);

	if (!item->disabled) {
		text_draw_right(rx,item->y,list_item_font_size,NULL,item->value.str);
		return;
	}

	col.r=col.g=0.0f;
	col.b=1.0f;
	text_draw_right(rx,item->y,list_item_font_size,&col,item->value.str);
}

void list_palette_draw_item_button(list_palette_type *list,int idx)
{
	int						lx,rx,ty,by;
	list_palette_item_type *item;

	item=&list->items[idx];
	if (item->button_type==list_button_none) return;

	rx=list->box.rx-(list_palette_scroll_wid+1);
	lx=rx-16;
	ty=item->y-list_item_font_high;
	by=ty+16;

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	if ((list->push_on) && (list->push_idx==idx) && (list->button_click)) {
		glColor4f(0.6f,0.6f,0.6f,1.0f);
	}
	else {
		glColor4f(1.0f,1.0f,1.0f,1.0f);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,list_bitmaps[item->button_type].gl_id);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f);
	glVertex2i(lx,ty);
	glTexCoord2f(1.0f,0.0f);
	glVertex2i(rx,ty);
	glTexCoord2f(1.0f,1.0f);
	glVertex2i(rx,by);
	glTexCoord2f(0.0f,1.0f);
	glVertex2i(lx,by);
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
}

/* =======================================================

      List Palette Draw
      
======================================================= */

void list_palette_draw_item(list_palette_type *list,int idx)
{
	bool						selected;
	char						str[32];
	list_palette_item_type		*item;
	
	item=&list->items[idx];

		// early exits

	if ((item->y<(list->box.ty+list_title_high)) || ((item->y-list_item_font_high)>list->box.by)) return;

		// draw header
		
	if (item->ctrl_type==list_item_ctrl_header) {

		glColor4f(0.9f,0.9f,0.9f,1.0f);

		glBegin(GL_QUADS);
		glVertex2i(list->box.lx,(item->y-list_item_font_high));
		glVertex2i((list->box.rx-list_palette_scroll_wid),(item->y-list_item_font_high));
		glVertex2i((list->box.rx-list_palette_scroll_wid),item->y);
		glVertex2i(list->box.lx,item->y);
		glEnd();
	}
		
		// draw selected item
	
	else {

		if (!list->push_on) {
			selected=item->selected;
		}
		else {
			if (list->button_click) {
				selected=FALSE;
			}
			else {
				selected=(idx==list->push_idx);
			}
		}

		if (selected) {
			glColor4f(1.0f,1.0f,0.0f,1.0f);

			glBegin(GL_QUADS);
			glVertex2i(list->box.lx,((item->y-list_item_font_high)+1));
			glVertex2i((list->box.rx-list_palette_scroll_wid),((item->y-list_item_font_high)+1));
			glVertex2i((list->box.rx-list_palette_scroll_wid),item->y);
			glVertex2i(list->box.lx,item->y);
			glEnd();
		}
	}

		// draw item

	switch (item->ctrl_type) {

			// text or header

		case list_item_ctrl_header:
		case list_item_ctrl_text:
			text_draw(item->x,item->y,list_item_font_size,NULL,item->name);
			list_palette_draw_item_button(list,idx);
			break;

			// color

		case list_item_ctrl_color:
			text_draw(item->x,item->y,list_item_font_size,NULL,"Light");
			list_palette_draw_item_color_box(list,item,&item->value.col);
			break;

			// string

		case list_item_ctrl_string:
			text_draw(item->x,item->y,list_item_font_size,NULL,item->name);
			list_palette_draw_item_string(list,item);
			list_palette_draw_item_button(list,idx);
			break;

			// checkbox

		case list_item_ctrl_checkbox:
			text_draw(item->x,item->y,list_item_font_size,NULL,item->name);
			list_palette_draw_item_check_box(list,item,item->value.checkbox);
			break;

			// pick color

		case list_item_ctrl_pick_color:
			text_draw(item->x,item->y,list_item_font_size,NULL,item->name);
			list_palette_draw_item_color_box(list,item,&item->value.col);
			break;

	}
	
		// header counts
		
	if (item->ctrl_type==list_item_ctrl_header) {
		if (item->count!=-1) {
			sprintf(str,"%d",item->count);
			text_draw_right((list->box.rx-(list_palette_scroll_wid+4)),item->y,list_item_font_size,NULL,str);
		}
	}
}

void list_palette_draw(list_palette_type *list,bool close_border)
{
	int						n,lx,mx,rx,ty,by,
							thumb_ty,thumb_by,page_count;
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

		// items

	for (n=0;n!=list->item_count;n++) {
		list_palette_draw_item(list,n);
	}
	
		// scroll bar
	
	lx=list->box.rx-list_palette_scroll_wid;
	ty=list->box.ty+list_title_high;
	by=list->box.by;
	
	glColor4f(0.5f,0.5f,0.5f,1.0f);
		
	glBegin(GL_QUADS);
	glVertex2i(lx,ty);
	glVertex2i(list->box.rx,ty);
	glVertex2i(list->box.rx,by);
	glVertex2i(lx,by);
	glEnd();

	page_count=list_palette_get_scroll_page_count(list);

	if ((list->item_count!=0) && (page_count!=0)) {
		
		thumb_ty=ty+(((by-ty)*list->scroll_page)/(page_count+1));
		if (thumb_ty<ty) thumb_ty=ty;
		
		thumb_by=ty+(((by-ty)*(list->scroll_page+1))/(page_count+1));
		if (thumb_by>by) thumb_by=by;

		glColor4f(0.0f,0.0f,0.0f,1.0f);
			
		glBegin(GL_QUADS);
		glVertex2i(lx,thumb_ty);
		glVertex2i(list->box.rx,thumb_ty);
		glVertex2i(list->box.rx,thumb_by);
		glVertex2i(lx,thumb_by);
		glEnd();

		glColor4f(0.9f,0.9f,0.9f,1.0f);
			
		glBegin(GL_QUADS);
		glVertex2i((lx+1),(thumb_ty+1));
		glVertex2i((list->box.rx-1),(thumb_ty+1));
		glVertex2i((list->box.rx-1),(thumb_by-1));
		glVertex2i((lx+1),(thumb_by-1));
		glEnd();
	}
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	glBegin(GL_LINES);
	glVertex2i(lx,list->box.ty);
	glVertex2i(lx,list->box.by);
	glEnd();

		// title

	ty=list->box.ty;
	by=ty+list_title_high;

	glColor4f(0.6f,0.6f,0.6f,1.0f);

	glBegin(GL_QUADS);
	glVertex2i(list->box.lx,ty);
	glVertex2i(list->box.rx,ty);
	glVertex2i(list->box.rx,by);
	glVertex2i(list->box.lx,by);
	glEnd();
	
	text_draw_center(((list->box.lx+list->box.rx)>>1),(by-2),list_title_font_size,NULL,list->title);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	
	glBegin(GL_LINES);
	glVertex2i(list->box.lx,(ty+1));
	glVertex2i(list->box.rx,(ty+1));
	glVertex2i(list->box.lx,by);
	glVertex2i(list->box.rx,by);
	glEnd();
	
		// click-close border or
		// just a line
	
	if (!close_border) {
		glColor4f(0.0f,0.0f,0.0f,1.0f);
	
		glBegin(GL_LINES);
		glVertex2i((list->box.lx+1),list->box.ty);
		glVertex2i((list->box.lx+1),list->box.by);
		glEnd();

		return;
	}

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
	int				page_count;
	
	page_count=list_palette_get_scroll_page_count(list);
	
	if (list->scroll_page<page_count) {
		list->scroll_page++;
		main_wind_draw();
	}
}

void list_palette_scroll_wheel(list_palette_type *list,d3pnt *pnt,int move)
{
	if (move>0) list_palette_scroll_up(list);
	if (move<0) list_palette_scroll_down(list);
}

void list_palette_scroll_item_into_view(list_palette_type *list,int item_type,int item_idx)
{
	int							n,y,page_count;
	bool						hit;
	list_palette_item_type		*item;

		// find item

	y=0;
	hit=FALSE;

	item=list->items;

	for (n=0;n!=list->item_count;n++) {

		y+=list_item_font_high;

		if ((item->type==item_type) && (item->idx==item_idx)) {
			hit=TRUE;
			break;
		}

		item++;
	}
	
	if (!hit) return;

		// bring it into view
		// somewhere near the top if possible

	list->scroll_page=(y/list_item_scroll_size)-1;

	if (list->scroll_page<0) list->scroll_page=0;
	page_count=list_palette_get_scroll_page_count(list);
	if (list->scroll_page>page_count) list->scroll_page=page_count;
}

/* =======================================================

      Item Palette Click
      
======================================================= */

bool list_palette_click_item(list_palette_type *list,int item_idx)
{
	bool					out_box;
	d3pnt					pt;
	list_palette_item_type	*item;

		// get clicked item

	item=&list->items[item_idx];
	
	if (item->disabled) return(FALSE);

		// do the hold and click

	list->push_on=TRUE;
	list->push_idx=item_idx;
	
	main_wind_draw();

	while (!os_track_mouse_location(&pt,NULL)) {
	
		out_box=FALSE;
		out_box=out_box||(pt.x>=(list->box.rx-list_palette_scroll_wid));
		out_box=out_box||(pt.y<(item->y-list_item_font_high));
		out_box=out_box||(pt.y>=item->y);
		
		if (!list->button_click) {
			out_box=out_box||(pt.x<list->box.lx);
		}
		else {
			out_box=out_box||(pt.x<((list->box.rx-list_palette_scroll_wid)-list_item_font_high));
		}
		
		if (out_box) {
			if (list->push_idx!=-1) {
				list->push_idx=-1;
				main_wind_draw();
			}
		}
		else {
			if (list->push_idx!=item_idx) {
				list->push_idx=item_idx;
				main_wind_draw();
			}
		}
		
		usleep(10000);
	}

	list->push_on=FALSE;

	if (list->push_idx!=item_idx) {
		main_wind_draw();
		return(FALSE);
	}

		// pass back clicked item
		
	list->item_type=item->type;
	list->item_idx=item->idx;

	if (!list->button_click) {
		list->item_id=item->id;
	}
	else {
		list->item_id=item->button_id;
	}

	return(TRUE);
}

void list_palette_click_scroll_bar(list_palette_type *list)
{
	int						old_page,page,page_count,
							page_size,y,offset_y,thumb_y;
	d3pnt					pt,org_pt;
	
		// scrolling sizes
		
	page_count=list_palette_get_scroll_page_count(list);
	if (page_count==0) return;
	
	page_size=((list->box.by-list->box.ty)/(page_count+1));

		// scrolling
		
	old_page=list->scroll_page;
	os_get_cursor(&org_pt);

	thumb_y=list->box.ty+(((list->box.by-list->box.ty)*list->scroll_page)/(page_count+1));
	offset_y=thumb_y-org_pt.y;
	
	while (!os_track_mouse_location(&pt,NULL)) {
		
		y=(pt.y-org_pt.y)-offset_y;
		page=old_page+(y/page_size);
		if (page<0) page=0;
		if (page>page_count) page=page_count;
		
		if (page!=list->scroll_page) {
			list->scroll_page=page;
			main_wind_draw();
		}
		
		usleep(10000);
	}
}

bool list_palette_click(list_palette_type *list,d3pnt *pnt,bool double_click)
{
	int						item_idx;

	pnt->x-=list->box.lx;
	pnt->y-=list->box.ty;

		// click in close border

	if (pnt->x<=list_palette_border_sz) {
		list_palette_open=!list_palette_open;
		main_wind_draw();
		return(FALSE);
	}
	
		// click in title
		
	if (pnt->y<list_title_high) return(FALSE);
	
		// click in scroll bar
		
	if (pnt->x>=((list->box.rx-list->box.lx)-list_palette_scroll_wid)) {
		list_palette_click_scroll_bar(list);
		return(FALSE);
	}

		// click in item

	item_idx=((pnt->y-list_title_high)+(list->scroll_page*list_item_scroll_size))/list_item_font_high;
	if ((item_idx<0) || (item_idx>=list->item_count)) return(FALSE);

		// is there a button?

	list->button_click=FALSE;

	if (list->items[item_idx].button_type!=list_button_none) {
		if (pnt->x>=(((list->box.rx-list->box.lx)-list_palette_scroll_wid)-list_item_font_high)) {
			list->button_click=TRUE;
		}
	}

		// if a header or an id of -1, only click if there's
		// a button

	if (list->items[item_idx].ctrl_type==list_item_ctrl_header) {
		if (!list->button_click) return(FALSE);
	}

		// run the click

	return(list_palette_click_item(list,item_idx));
}