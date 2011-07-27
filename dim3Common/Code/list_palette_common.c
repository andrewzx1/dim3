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

int								list_palette_level;
bool							list_palette_open;
bitmap_type						list_bitmaps[5];

list_palette_picker_type		list_picker;

/* =======================================================

      Item Palette Setup
      
======================================================= */

void list_palette_initialize(char *app_name)
{
	int				n;
	char			sub_path[1024],path[1024];
	char			btn_names[5][32]={"Back","Edit","Plus","Minus","Set"};

		// start open

	list_palette_open=TRUE;
	
		// start level 0
		
	list_palette_level=0;

		// list picker off

	list_picker.on=FALSE;

		// load buttons

	os_get_support_file_path(sub_path,app_name);
	strcat(sub_path,"/Lists");
		
	for (n=0;n!=5;n++) {
		file_paths_app(&file_path_setup,path,sub_path,btn_names[n],"png");
		bitmap_open(&list_bitmaps[n],path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);
	}
}

void list_palette_shutdown(void)
{
	int				n;

	for (n=0;n!=5;n++) {
		bitmap_close(&list_bitmaps[n]);
	}
}

void list_palette_list_initialize(list_palette_type *list,char *title,bool back_on)
{
	strcpy(list->title,title);

	list->scroll_page=0;
	list->total_high=0;
	
	list->back_on=back_on;
	list->back_push_on=FALSE;

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

void list_palette_set_sub_title(list_palette_type *list,char *title,char *item_name)
{
	sprintf(list->title,"%s:%s",title,item_name);
}

void list_palette_set_sub2_title(list_palette_type *list,char *title,char *item_name,char *sub_item_name)
{
	sprintf(list->title,"%s:%s:%s",title,item_name,sub_item_name);
}

/* =======================================================

      List Palette Levels
      
======================================================= */

int list_palette_get_level(void)
{
	return(list_palette_level);
}

void list_palette_set_level(int level)
{
	list_picker.on=FALSE;		// level change always turns off picker
	list_palette_level=level;
}

/* =======================================================

      List Palette Box
      
======================================================= */

void list_palette_box(d3rect *box)
{
#ifndef D3_SETUP
	int				pixel_sz;
#endif
	d3rect			wbox;
	
	os_get_window_box(&wbox);
	
#ifdef D3_SETUP
	box->lx=wbox.lx;
	box->rx=wbox.rx;
	box->ty=wbox.ty;
	box->by=wbox.by;
#else
	if (list_palette_open) {
		pixel_sz=list_palette_tree_sz;
	}
	else {
		pixel_sz=list_palette_border_sz;
	}

	box->lx=wbox.rx-pixel_sz;
	box->rx=box->lx+pixel_sz;
	box->ty=wbox.ty+(tool_palette_pixel_size()+1);
	box->by=wbox.by;
#endif
}

/* =======================================================

      List Add Items
      
======================================================= */

list_palette_item_type* list_palette_create_item(list_palette_type *list,int ctrl_type)
{
	d3rect						box;
	list_palette_item_type		*item;

	item=&list->items[list->item_count];
	list->item_count++;

	item->ctrl_type=ctrl_type;
	item->button_type=list_button_none;
	item->disabled=FALSE;
	
	list_palette_box(&box);

	item->x=box.lx+(list_palette_border_sz+4);
	if (ctrl_type!=list_item_ctrl_header) item->x+=10;

	item->y=box.ty+((list->item_count*list_item_font_high)+list_title_high);
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

      List Palette Picking Mode
      
======================================================= */

void list_palette_start_picking_mode(char *title,char *list_ptr,int list_count,int list_item_sz,int list_name_offset,bool include_none,int *idx_ptr,char *name_ptr)
{
	list_picker.on=TRUE;
	list_picker.picker_idx_ptr=idx_ptr;
	list_picker.picker_name_ptr=name_ptr;

	strcpy(list_picker.title,title);

	list_picker.include_none=include_none;
	list_picker.ptr=list_ptr;
	list_picker.count=list_count;
	list_picker.item_sz=list_item_sz;
	list_picker.name_offset=list_name_offset;
}

void list_palette_fill_picking_mode(list_palette_type *list)
{
	int					n;
	char				*str_ptr;
	bool				sel;

	strcpy(list->title,list_picker.title);

	list_palette_delete_all_items(list);

		// none item

	if (list_picker.include_none) {

		sel=FALSE;
		if (list_picker.picker_idx_ptr!=NULL) {
			sel=(*list_picker.picker_idx_ptr==-1);
		}
		if (list_picker.picker_name_ptr!=NULL) {
			sel=(*list_picker.picker_name_ptr==0x0);
		}

		list_palette_add_item(list,-1,-1,"None",sel,FALSE);
	}

		// other items

	for (n=0;n!=list_picker.count;n++) {
		str_ptr=list_picker.ptr+((list_picker.item_sz*n)+list_picker.name_offset);

		sel=FALSE;
		if (list_picker.picker_idx_ptr!=NULL) {
			sel=(*list_picker.picker_idx_ptr==n);
		}
		if (list_picker.picker_name_ptr!=NULL) {
			sel=(strcasecmp(list_picker.picker_name_ptr,str_ptr)==0);
		}

		list_palette_add_item(list,-1,n,str_ptr,sel,FALSE);
	}
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
	d3rect			box;
	
	list_palette_box(&box);
	
	high=list->total_high-(box.by-(box.ty+list_title_high));
	if (high<=0) return(0);
	
	return((high/list_item_scroll_size)+1);
}

/* =======================================================

      List Palette Draw Utilities
      
======================================================= */

void list_palette_draw_item_color_box(list_palette_type *list,list_palette_item_type *item,d3col *col)
{
	int					x,y;
	d3rect				box;
	
	list_palette_box(&box);

	x=box.rx-(list_item_font_high+(list_palette_scroll_wid+4));
	y=item->y;

	glColor4f((col->r*0.5f),(col->g*0.5f),(col->b*0.5f),1.0f);

	glBegin(GL_QUADS);
	glVertex2i(x,((y-list_item_font_high)+2));
	glVertex2i((x+list_item_font_high),((y-list_item_font_high)+2));
	glVertex2i((x+list_item_font_high),(y-2));
	glVertex2i(x,(y-2));
	glEnd();

	glColor4f(col->r,col->g,col->b,1.0f);
			
	glBegin(GL_QUADS);
	glVertex2i((x+1),((y-list_item_font_high)+3));
	glVertex2i(((x+list_item_font_high)-1),((y-list_item_font_high)+3));
	glVertex2i(((x+list_item_font_high)-1),(y-3));
	glVertex2i((x+1),(y-3));
	glEnd();
}

void list_palette_draw_item_check_box(list_palette_type *list,list_palette_item_type *item,bool checked)
{
	int					lx,rx,ty,by;
	d3rect				box;
	
	list_palette_box(&box);

	lx=box.rx-(list_item_font_high+(list_palette_scroll_wid+2));
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
	int					rx;
	d3col				col;
	d3rect				box;
	
	list_palette_box(&box);

	rx=box.rx-(list_palette_scroll_wid+4);
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
	d3rect					box;
	list_palette_item_type *item;
	
	list_palette_box(&box);

	item=&list->items[idx];
	if (item->button_type==list_button_none) return;

	rx=box.rx-(list_palette_scroll_wid+1);
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
	glBindTexture(GL_TEXTURE_2D,list_bitmaps[item->button_type+1].gl_id);

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

      List Palette Title, Scrollbars and Border
      
======================================================= */

void list_palette_draw_setup(list_palette_type *list)
{
	d3rect					wbox,box;

	list_palette_box(&box);
	
		// viewport setup
		
	os_get_window_box(&wbox);

	glEnable(GL_SCISSOR_TEST);
	glScissor(box.lx,(wbox.by-box.by),(box.rx-box.lx),(box.by-box.ty));

	glViewport(box.lx,(wbox.by-box.by),(box.rx-box.lx),(box.by-box.ty));
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)box.lx,(GLdouble)box.rx,(GLdouble)box.by,(GLdouble)box.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		// draw setup

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

		// background

	glColor4f(1.0f,1.0f,1.0f,1.0f);
		
	glBegin(GL_QUADS);
	glVertex2i(box.lx,box.ty);
	glVertex2i(box.rx,box.ty);
	glVertex2i(box.rx,box.by);
	glVertex2i(box.lx,box.by);
	glEnd();
}

void list_palette_draw_title(list_palette_type *list)
{
	int					lx,rx,ty,by;
	d3rect				box;
	
	list_palette_box(&box);

		// title background

	ty=box.ty;
	by=ty+list_title_high;

	glColor4f(0.6f,0.6f,0.6f,1.0f);

	glBegin(GL_QUADS);
	glVertex2i(box.lx,ty);
	glVertex2i(box.rx,ty);
	glVertex2i(box.rx,by);
	glVertex2i(box.lx,by);
	glEnd();

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	
	glBegin(GL_LINES);
	glVertex2i(box.lx,(ty+1));
	glVertex2i(box.rx,(ty+1));
	glVertex2i(box.lx,by);
	glVertex2i(box.rx,by);
	glEnd();

		// text
	
	text_draw_center(((box.lx+box.rx)>>1),(by-2),list_title_font_size,NULL,list->title);
	
		// back arrow
		
	if ((!list->back_on) && (!list_picker.on)) return;
	
#ifndef D3_SETUP
	lx=box.lx+12;
#else
	lx=box.lx+4;
#endif
	rx=lx+32;
	ty=box.ty+1;
	by=ty+32;

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	if (!list->back_push_on) {
		glColor4f(1.0f,1.0f,1.0f,1.0f);
	}
	else {
		glColor4f(0.7f,0.7f,0.7f,1.0f);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,list_bitmaps[0].gl_id);

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

void list_palette_draw_scrollbar(list_palette_type *list)
{
	int					lx,ty,by,thumb_ty,thumb_by,page_count;
	d3rect				box;

	list_palette_box(&box);
	
		// scroll bar
	
	lx=box.rx-list_palette_scroll_wid;
	ty=box.ty+list_title_high;
	by=box.by;
	
	glColor4f(0.5f,0.5f,0.5f,1.0f);
		
	glBegin(GL_QUADS);
	glVertex2i(lx,ty);
	glVertex2i(box.rx,ty);
	glVertex2i(box.rx,by);
	glVertex2i(lx,by);
	glEnd();

	page_count=list_palette_get_scroll_page_count(list);

	if ((list->item_count!=0) && (page_count!=0)) {
		
		thumb_ty=ty+(((by-ty)*list->scroll_page)/(page_count+1));
		if (thumb_ty<ty) thumb_ty=ty;
		
		thumb_by=ty+(((by-ty)*(list->scroll_page+1))/(page_count+1));
		if (thumb_by>by) thumb_by=by;

		glColor4f(0.9f,0.9f,0.9f,1.0f);
			
		glBegin(GL_QUADS);
		glVertex2i((lx+1),(thumb_ty+1));
		glVertex2i((box.rx-1),(thumb_ty+1));
		glVertex2i((box.rx-1),(thumb_by-1));
		glVertex2i((lx+1),(thumb_by-1));
		glEnd();

		glColor4f(0.0f,0.0f,0.0f,1.0f);
			
		glBegin(GL_LINE_LOOP);
		glVertex2i((lx+1),thumb_ty);
		glVertex2i(box.rx,(thumb_ty-1));
		glVertex2i(box.rx,(thumb_by-1));
		glVertex2i((lx+1),thumb_by);
		glEnd();
	}
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	glBegin(GL_LINES);
	glVertex2i(lx,box.ty);
	glVertex2i(lx,box.by);
	glEnd();
}

void list_palette_draw_border(list_palette_type *list)
{
	int					lx,rx,mx;
	d3rect				box;
	
	list_palette_box(&box);

	lx=box.lx;
	rx=box.lx+list_palette_border_sz;
	mx=(lx+rx)>>1;
		
	glBegin(GL_QUADS);
	glColor4f(0.0f,0.0f,0.5f,1.0f);
	glVertex2i(lx,box.by);
	glVertex2i(lx,box.ty);
	glColor4f(0.0f,0.0f,1.0f,1.0f);
	glVertex2i(mx,box.ty);
	glVertex2i(mx,box.by);
	glColor4f(0.0f,0.0f,0.5f,1.0f);
	glVertex2i(rx,box.by);
	glVertex2i(rx,box.ty);
	glColor4f(0.0f,0.0f,1.0f,1.0f);
	glVertex2i(mx,box.ty);
	glVertex2i(mx,box.by);
	glEnd();
}

/* =======================================================

      List Palette Draw
      
======================================================= */

void list_palette_draw_item(list_palette_type *list,int idx)
{
	int							x,y;
	bool						selected;
	char						str[32];
	d3rect						box;
	list_palette_item_type		*item;
	
	list_palette_box(&box);
	
	item=&list->items[idx];

		// early exits

	if ((item->y<(box.ty+list_title_high)) || ((item->y-list_item_font_high)>box.by)) return;

		// draw header
		
	if (item->ctrl_type==list_item_ctrl_header) {

		glColor4f(0.9f,0.9f,0.9f,1.0f);

		glBegin(GL_QUADS);
		glVertex2i(box.lx,(item->y-list_item_font_high));
		glVertex2i((box.rx-list_palette_scroll_wid),(item->y-list_item_font_high));
		glVertex2i((box.rx-list_palette_scroll_wid),item->y);
		glVertex2i(box.lx,item->y);
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
			glVertex2i(box.lx,((item->y-list_item_font_high)+1));
			glVertex2i((box.rx-list_palette_scroll_wid),((item->y-list_item_font_high)+1));
			glVertex2i((box.rx-list_palette_scroll_wid),item->y);
			glVertex2i(box.lx,item->y);
			glEnd();
		}
	}

		// draw item
		
	x=item->x;
	y=item->y;

	switch (item->ctrl_type) {

			// text or header

		case list_item_ctrl_header:
		case list_item_ctrl_text:
			text_draw(x,y,list_item_font_size,NULL,item->name);
			list_palette_draw_item_button(list,idx);
			break;

			// color

		case list_item_ctrl_color:
			text_draw(x,y,list_item_font_size,NULL,"Light");
			list_palette_draw_item_color_box(list,item,&item->value.col);
			break;

			// string

		case list_item_ctrl_string:
			text_draw(x,y,list_item_font_size,NULL,item->name);
			list_palette_draw_item_string(list,item);
			list_palette_draw_item_button(list,idx);
			break;

			// checkbox

		case list_item_ctrl_checkbox:
			text_draw(x,y,list_item_font_size,NULL,item->name);
			list_palette_draw_item_check_box(list,item,item->value.checkbox);
			break;

			// pick color

		case list_item_ctrl_pick_color:
			text_draw(x,y,list_item_font_size,NULL,item->name);
			list_palette_draw_item_color_box(list,item,&item->value.col);
			break;

	}
	
		// header counts
		
	if (item->ctrl_type==list_item_ctrl_header) {
		if (item->count!=-1) {
			sprintf(str,"%d",item->count);
			text_draw_right((box.rx-(list_palette_scroll_wid+4)),item->y,list_item_font_size,NULL,str);
		}
	}
}

void list_palette_draw(list_palette_type *list)
{
	int						n;

	list_palette_draw_setup(list);

		// in list picker?
		// if so replace content with list

	if (list_picker.on) list_palette_fill_picking_mode(list);

		// items

	for (n=0;n!=list->item_count;n++) {
		list_palette_draw_item(list,n);
	}
	
		// list pieces

	list_palette_draw_scrollbar(list);
	list_palette_draw_title(list);
#ifndef D3_SETUP
	list_palette_draw_border(list);
#endif

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
	if (list_picker.on) list_palette_fill_picking_mode(list);

	if (move>0) list_palette_scroll_up(list);
	if (move<0) list_palette_scroll_down(list);
}

void list_palette_scroll_item_into_view(list_palette_type *list,int item_type,int item_idx)
{
	int							n,y,page_count;
	bool						hit;
	list_palette_item_type		*item;

	if (list_picker.on) return;

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
	d3rect					box;
	list_palette_item_type	*item;

		// get clicked item

	item=&list->items[item_idx];
	
	if (item->disabled) return(FALSE);

		// do the hold and click
		
	list_palette_box(&box);

	list->push_on=TRUE;
	list->push_idx=item_idx;
	
	main_wind_draw();

	while (!os_track_mouse_location(&pt,NULL)) {
	
		out_box=FALSE;
		out_box=out_box||(pt.x>=(box.rx-list_palette_scroll_wid));
		out_box=out_box||(pt.y<(item->y-list_item_font_high));
		out_box=out_box||(pt.y>=item->y);
		
		if (!list->button_click) {
			out_box=out_box||(pt.x<box.lx);
		}
		else {
			out_box=out_box||(pt.x<((box.rx-list_palette_scroll_wid)-list_item_font_high));
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

bool list_palette_click_back(list_palette_type *list)
{
	bool					on,out_box;
	d3pnt					pt;
	d3rect					box;

		// do the hold and click
		
	list_palette_box(&box);
	box.by=box.ty+list_title_high;

	list->back_push_on=TRUE;
	
	main_wind_draw();

	while (!os_track_mouse_location(&pt,NULL)) {
	
		out_box=FALSE;
		out_box=out_box||(pt.x<box.lx);
		out_box=out_box||(pt.x>box.rx);
		out_box=out_box||(pt.y<box.ty);
		out_box=out_box||(pt.y>box.by);
		
		list->back_push_on=!out_box;
		main_wind_draw();
		
		usleep(10000);
	}

	on=list->back_push_on;
	list->back_push_on=FALSE;

	return(on);
}

void list_palette_click_scroll_bar(list_palette_type *list)
{
	int						old_page,page,page_count,
							page_size,y,offset_y,thumb_y;
	d3pnt					pt,org_pt;
	d3rect					box;
	
	list_palette_box(&box);

		// scrolling sizes
		
	page_count=list_palette_get_scroll_page_count(list);
	if (page_count==0) return;
	
	page_size=((box.by-box.ty)/(page_count+1));

		// scrolling
		
	old_page=list->scroll_page;
	os_get_cursor(&org_pt);

	thumb_y=box.ty+(((box.by-box.ty)*list->scroll_page)/(page_count+1));
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
	d3pnt					pt;
	d3rect					box;
	
	list_palette_box(&box);

	pt.x=pnt->x-box.lx;
	pt.y=pnt->y-box.ty;

		// click in close border

	if (pt.x<=list_palette_border_sz) {
		list_palette_open=!list_palette_open;
		main_wind_draw();
		return(FALSE);
	}
	
		// click in title
		
	if (pt.y<list_title_high) {
		if (list->back_on) {
			if (list_palette_click_back(list)) {
				if (list_picker.on) {
					list_picker.on=FALSE;
				}
				else {
					list_palette_set_level(list_palette_level-1);
				}
				main_wind_draw();
			}
		}
		return(FALSE);
	}
	
		// click in scroll bar
		
	if (pt.x>=((box.rx-box.lx)-list_palette_scroll_wid)) {
		list_palette_click_scroll_bar(list);
		return(FALSE);
	}

		// in list picker?
		// if so replace content with list

	if (list_picker.on) list_palette_fill_picking_mode(list);

		// click in item

	item_idx=((pt.y-list_title_high)+(list->scroll_page*list_item_scroll_size))/list_item_font_high;
	if ((item_idx<0) || (item_idx>=list->item_count)) return(FALSE);

		// is there a button?

	if (!list_picker.on) {
		list->button_click=FALSE;

		if (list->items[item_idx].button_type!=list_button_none) {
			if (pt.x>=(((box.rx-box.lx)-list_palette_scroll_wid)-list_item_font_high)) {
				list->button_click=TRUE;
			}
		}
	}

		// if a header or an id of -1, only click if there's
		// a button

	if (!list_picker.on) {
		if (list->items[item_idx].ctrl_type==list_item_ctrl_header) {
			if (!list->button_click) return(FALSE);
		}
	}

		// run the click

	if (!list_palette_click_item(list,item_idx)) return(FALSE);

		// list picker picks

	if (!list_picker.on) return(TRUE);

	if (list_picker.picker_idx_ptr!=NULL) {
		if (!list_picker.include_none) {
			*list_picker.picker_idx_ptr=item_idx;
		}
		else {
			*list_picker.picker_idx_ptr=item_idx-1;
		}
	}
	else {
		if (!list_picker.include_none) {
			strcpy(list_picker.picker_name_ptr,list->items[item_idx].name);
		}
		else {
			if (item_idx==0) {
				*list_picker.picker_name_ptr=0x0;
			}
			else {
				strcpy(list_picker.picker_name_ptr,list->items[item_idx].name);
			}
		}
	}

	list_picker.on=FALSE;
	main_wind_draw();

	return(TRUE);
}