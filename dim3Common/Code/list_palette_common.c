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
		bitmap_open(&list_bitmaps[n],path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE,FALSE);
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

void list_palette_add_color(list_palette_type *list,int piece_type,int piece_idx,d3col *col_ptr,bool selected,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_color);

	item->type=piece_type;
	item->idx=piece_idx;
	item->id=-1;

	item->selected=selected;
	item->disabled=disabled;

	item->value.col_ptr=col_ptr;
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
	
	if (strlen(value)>=list_value_clip_size) {
		strncpy(item->value.str,value,list_value_clip_size);
		strcpy((char*)&item->value.str[list_value_clip_size],"...");
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

void list_palette_add_int(list_palette_type *list,int id,char *name,int *int_ptr,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_int);

	item->type=-1;
	item->idx=-1;
	item->id=id;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	item->value.int_ptr=int_ptr;
}

void list_palette_add_float(list_palette_type *list,int id,char *name,float *float_ptr,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_float);

	item->type=-1;
	item->idx=-1;
	item->id=id;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	item->value.float_ptr=float_ptr;
}

void list_palette_add_checkbox(list_palette_type *list,int id,char *name,bool *bool_ptr,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_checkbox);

	item->type=-1;
	item->idx=-1;
	item->id=id;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	item->value.bool_ptr=bool_ptr;
}

void list_palette_add_pick_color(list_palette_type *list,int id,char *name,d3col *col_ptr,bool disabled)
{
	list_palette_item_type		*item;

	item=list_palette_create_item(list,list_item_ctrl_pick_color);

	item->type=-1;
	item->idx=-1;
	item->id=id;

	item->selected=FALSE;
	item->disabled=disabled;

	strcpy(item->name,name);
	item->value.col_ptr=col_ptr;
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

void list_palette_start_picking_mode(char *title,char *list_ptr,int list_count,int list_item_sz,int list_name_offset,bool include_none,bool file_list,int *idx_ptr,char *name_ptr)
{
	list_picker.on=TRUE;
	list_picker.picker_idx_ptr=idx_ptr;
	list_picker.picker_name_ptr=name_ptr;

	strcpy(list_picker.title,title);

	list_picker.include_none=include_none;
	list_picker.file_list=file_list;
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

			// @ marks headers

		if (str_ptr[0]=='@') {
			list_palette_add_header(list,-1,(char*)&str_ptr[1]);
			continue;
		}

			// ~ marks disabled

		if (str_ptr[0]=='~') {
			list_palette_add_string(list,-1,(char*)&str_ptr[1],"(Circular Reference)",TRUE);
			continue;
		}

			// regular clickable items

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

void list_palette_choose_picking_mode(list_palette_type *list,int item_idx,bool double_click)
{
	int					idx;
	char				dir[256];

		// are we in the list picker?

	if (!list_picker.on) return;
	if (!double_click) return;

		// turn off list picker

	list_picker.on=FALSE;

		// integer list pickers just get index

	if (list_picker.picker_idx_ptr!=NULL) {
		*list_picker.picker_idx_ptr=list->item_idx;
		main_wind_draw();
		return;
	}

		// blank string picker

	if (list->item_idx==-1) {
		*list_picker.picker_name_ptr=0x0;
		main_wind_draw();
		return;
	}

		// if not file picker, just grab the name

	if (!list_picker.file_list) {
		strcpy(list_picker.picker_name_ptr,list->items[item_idx].name);
		main_wind_draw();
		return;
	}

		// file picker needs to find directory

	dir[0]=0x0;

	idx=item_idx-1;

	while (idx>=0) {
		if (list->items[idx].ctrl_type==list_item_ctrl_header) {
			strcpy(dir,(char*)&list->items[idx].name[5]);	// get over Root/
			break;
		}
		idx--;
	}

		// now add in the directory + file name

	if (dir[0]==0x0) {
		strcpy(list_picker.picker_name_ptr,list->items[item_idx].name);
	}
	else {
		sprintf(list_picker.picker_name_ptr,"%s/%s",dir,list->items[item_idx].name);
	}

	main_wind_draw();
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
	float				vertexes[8];
	d3rect				box;
	
	list_palette_box(&box);

	x=box.rx-(list_item_font_high+(list_palette_scroll_wid+4));
	y=item->y-(list->scroll_page*list_item_scroll_size);

	vertexes[0]=vertexes[6]=(float)x;
	vertexes[2]=vertexes[4]=(float)(x+list_item_font_high);
	vertexes[1]=vertexes[3]=(float)((y-list_item_font_high)+2);
	vertexes[5]=vertexes[7]=(float)(y-2);

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f((col->r*0.5f),(col->g*0.5f),(col->b*0.5f),1.0f);
	glDrawArrays(GL_QUADS,0,4);

	vertexes[0]=vertexes[6]=(float)(x+1);
	vertexes[2]=vertexes[4]=(float)((x+list_item_font_high)-1);
	vertexes[1]=vertexes[3]=(float)((y-list_item_font_high)+3);
	vertexes[5]=vertexes[7]=(float)(y-3);

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(col->r,col->g,col->b,1.0f);
	glDrawArrays(GL_QUADS,0,4);
}

void list_palette_draw_item_check_box(list_palette_type *list,list_palette_item_type *item,bool checked)
{
	int					lx,rx,y,ty,by;
	float				vertexes[8];
	d3rect				box;
	
	list_palette_box(&box);

	lx=box.rx-(list_item_font_high+(list_palette_scroll_wid+2));
	rx=lx+(list_item_font_high-2);

	y=item->y-(list->scroll_page*list_item_scroll_size);
	ty=(y-list_item_font_high)+2;
	by=y;

	vertexes[0]=vertexes[6]=(float)lx;
	vertexes[2]=vertexes[4]=(float)rx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_QUADS,0,4);

	vertexes[0]=vertexes[6]=(float)(lx+1);
	vertexes[2]=vertexes[4]=(float)(rx-1);
	vertexes[1]=vertexes[3]=(float)(ty+1);
	vertexes[5]=vertexes[7]=(float)(by-1);

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(0.9f,0.9f,0.9f,1.0f);
	glDrawArrays(GL_QUADS,0,4);

	if (checked) {
		glLineWidth(3.0f);

		vertexes[0]=vertexes[6]=(float)(lx+1);
		vertexes[2]=vertexes[4]=(float)(rx-1);
		vertexes[1]=vertexes[5]=(float)(ty+1);
		vertexes[3]=vertexes[7]=(float)(by-1);

		glVertexPointer(2,GL_FLOAT,0,vertexes);

		glColor4f(0.0f,0.8f,0.0f,1.0f);
		glDrawArrays(GL_LINES,0,4);

		glLineWidth(1.0f);
	}
}

void list_palette_draw_item_string(list_palette_type *list,list_palette_item_type *item,char *str)
{
	int					rx,y;
	d3col				col;
	d3rect				box;
	
	list_palette_box(&box);

	rx=box.rx-(list_palette_scroll_wid+4);
	if (item->button_type!=list_button_none) rx-=(list_item_font_high+2);
	
	y=item->y-(list->scroll_page*list_item_scroll_size);

	if (!item->disabled) {
		text_draw_right(rx,y,list_item_font_size,NULL,str);
		return;
	}

	col.r=col.g=0.0f;
	col.b=1.0f;
	text_draw_right(rx,y,list_item_font_size,&col,str);
}

void list_palette_draw_item_button(list_palette_type *list,int idx)
{
	int						lx,rx,ty,by;
	float					vertexes[8],uvs[8]={0.0f,0.0f,1.0f,0.0f,1.0f,1.0f,0.0f,1.0f};
	d3rect					box;
	list_palette_item_type *item;
	
	list_palette_box(&box);

	item=&list->items[idx];
	if (item->button_type==list_button_none) return;

	rx=box.rx-(list_palette_scroll_wid+1);
	lx=rx-16;
	ty=(item->y-list_item_font_high)-(list->scroll_page*list_item_scroll_size);
	by=ty+16;

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	vertexes[0]=vertexes[6]=(float)lx;
	vertexes[2]=vertexes[4]=(float)rx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	if ((list->push_on) && (list->push_idx==idx) && (list->button_click)) {
		glColor4f(0.6f,0.6f,0.6f,1.0f);
	}
	else {
		glColor4f(1.0f,1.0f,1.0f,1.0f);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,list_bitmaps[item->button_type+1].gl_id);

	glVertexPointer(2,GL_FLOAT,0,vertexes);
	glTexCoordPointer(2,GL_FLOAT,0,uvs);

	glDrawArrays(GL_QUADS,0,4);
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
}

/* =======================================================

      List Palette Title, Scrollbars and Border
      
======================================================= */

void list_palette_draw_setup(list_palette_type *list)
{
	float					vertexes[8];
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

	vertexes[0]=vertexes[6]=(float)box.lx;
	vertexes[2]=vertexes[4]=(float)box.rx;
	vertexes[1]=vertexes[3]=(float)box.ty;
	vertexes[5]=vertexes[7]=(float)box.by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glDrawArrays(GL_QUADS,0,4);
}

void list_palette_draw_title(list_palette_type *list)
{
	int					lx,rx,ty,by;
	float				vertexes[8],colors[16],uvs[8]={0.0f,0.0f,1.0f,0.0f,1.0f,1.0f,0.0f,1.0f};
	d3rect				box;
	
	list_palette_box(&box);

		// title background

	ty=box.ty;
	by=ty+list_title_high;

	vertexes[0]=vertexes[6]=(float)box.lx;
	vertexes[2]=vertexes[4]=(float)box.rx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	colors[0]=colors[1]=colors[2]=colors[4]=colors[5]=colors[6]=0.9f;
	colors[8]=colors[9]=colors[10]=colors[12]=colors[13]=colors[14]=0.7f;
	colors[3]=colors[7]=colors[11]=colors[15]=1.0f;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4,GL_FLOAT,0,colors);

	glDrawArrays(GL_QUADS,0,4);

	glDisableClientState(GL_COLOR_ARRAY);

		// title line

	vertexes[0]=vertexes[4]=(float)box.lx;
	vertexes[2]=vertexes[6]=(float)box.rx;
	vertexes[1]=vertexes[3]=(float)(ty+1);
	vertexes[5]=vertexes[7]=(float)by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_LINES,0,4);

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
	ty=box.ty+2;
	by=ty+32;

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	if (!list->back_push_on) {
		glColor4f(1.0f,1.0f,1.0f,1.0f);
	}
	else {
		glColor4f(0.7f,0.7f,0.7f,1.0f);
	}

	vertexes[0]=vertexes[6]=(float)lx;
	vertexes[2]=vertexes[4]=(float)rx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);
	glTexCoordPointer(2,GL_FLOAT,0,uvs);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,list_bitmaps[0].gl_id);

	glDrawArrays(GL_QUADS,0,4);
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
}

void list_palette_draw_scrollbar(list_palette_type *list)
{
	int					lx,mx,ty,by,thumb_ty,thumb_by,page_count;
	float				vertexes[8],colors[16];
	d3rect				box;

	list_palette_box(&box);
	
		// scroll bar background

	glEnableClientState(GL_COLOR_ARRAY);

	lx=box.rx-list_palette_scroll_wid;
	ty=box.ty+list_title_high;
	by=box.by;

	mx=(lx+box.rx)>>1;

	vertexes[0]=vertexes[6]=(float)lx;
	vertexes[2]=vertexes[4]=(float)mx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	colors[0]=colors[1]=colors[2]=colors[12]=colors[13]=colors[14]=0.6f;
	colors[4]=colors[5]=colors[6]=colors[8]=colors[9]=colors[10]=0.9f;
	colors[3]=colors[7]=colors[11]=colors[15]=1.0f;

	glVertexPointer(2,GL_FLOAT,0,vertexes);
	glColorPointer(4,GL_FLOAT,0,colors);
	glDrawArrays(GL_QUADS,0,4);

	vertexes[0]=vertexes[6]=(float)mx;
	vertexes[2]=vertexes[4]=(float)box.rx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	colors[0]=colors[1]=colors[2]=colors[12]=colors[13]=colors[14]=0.9f;
	colors[4]=colors[5]=colors[6]=colors[8]=colors[9]=colors[10]=0.6f;
	colors[3]=colors[7]=colors[11]=colors[15]=1.0f;

	glVertexPointer(2,GL_FLOAT,0,vertexes);
	glColorPointer(4,GL_FLOAT,0,colors);
	glDrawArrays(GL_QUADS,0,4);

	glDisableClientState(GL_COLOR_ARRAY);

		// scroll bar thumb

	page_count=list_palette_get_scroll_page_count(list);

	if ((list->item_count!=0) && (page_count!=0)) {
		
		thumb_ty=ty+(((by-ty)*list->scroll_page)/(page_count+1));
		if (thumb_ty<ty) thumb_ty=ty;
		
		thumb_by=ty+(((by-ty)*(list->scroll_page+1))/(page_count+1));
		if (thumb_by>by) thumb_by=by;

		vertexes[0]=vertexes[6]=(float)(lx+1);
		vertexes[2]=vertexes[4]=(float)(box.rx-1);
		vertexes[1]=vertexes[3]=(float)(thumb_ty+1);
		vertexes[5]=vertexes[7]=(float)(thumb_by-1);

		glVertexPointer(2,GL_FLOAT,0,vertexes);
	
		glColor4f(0.9f,0.9f,0.9f,1.0f);
		glDrawArrays(GL_QUADS,0,4);

		vertexes[0]=vertexes[6]=(float)(lx+1);
		vertexes[2]=vertexes[4]=(float)box.rx;
		vertexes[1]=vertexes[3]=(float)(thumb_ty+1);
		vertexes[5]=vertexes[7]=(float)thumb_by;

		glVertexPointer(2,GL_FLOAT,0,vertexes);
	
		glColor4f(0.0f,0.0f,0.0f,1.0f);
		glDrawArrays(GL_LINE_LOOP,0,4);
	}

	vertexes[0]=vertexes[2]=(float)lx;
	vertexes[1]=(float)ty;
	vertexes[3]=(float)by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_LINES,0,2);
}

void list_palette_draw_border(list_palette_type *list)
{
	int					lx,rx;
	float				vertexes[8],colors[16];
	d3rect				box;
	
	list_palette_box(&box);

	lx=box.lx;
	rx=box.lx+list_palette_border_sz;

	vertexes[0]=vertexes[6]=(float)lx;
	vertexes[2]=vertexes[4]=(float)rx;
	vertexes[1]=vertexes[3]=(float)box.ty;
	vertexes[5]=vertexes[7]=(float)box.by;

	colors[0]=colors[1]=colors[12]=colors[13]=0.0f;
	colors[2]=colors[14]=1.0f;
	colors[4]=colors[5]=colors[8]=colors[9]=0.0f;
	colors[6]=colors[10]=0.6f;
	colors[3]=colors[7]=colors[11]=colors[15]=1.0f;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4,GL_FLOAT,0,colors);
	
	glDrawArrays(GL_QUADS,0,4);

	glDisableClientState(GL_COLOR_ARRAY);

	vertexes[0]=vertexes[2]=(float)lx;
	vertexes[4]=vertexes[6]=(float)(rx-1);
	vertexes[1]=vertexes[5]=(float)box.ty;
	vertexes[3]=vertexes[7]=(float)box.by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_LINES,0,4);
}

/* =======================================================

      List Palette Draw
      
======================================================= */

void list_palette_draw_item(list_palette_type *list,int idx)
{
	int							x,y;
	float						vertexes[8];
	bool						selected;
	char						str[256];
	d3col						col;
	d3rect						box;
	list_palette_item_type		*item;

	list_palette_box(&box);
	
	item=&list->items[idx];

		// early exits
		
	y=item->y-(list->scroll_page*list_item_scroll_size);

	if ((y<(box.ty+list_title_high)) || ((y-list_item_font_high)>box.by)) return;

		// draw header
		
	if (item->ctrl_type==list_item_ctrl_header) {
		vertexes[0]=vertexes[6]=(float)box.lx;
		vertexes[2]=vertexes[4]=(float)(box.rx-list_palette_scroll_wid);
		vertexes[1]=vertexes[3]=(float)(y-list_item_font_high);
		vertexes[5]=vertexes[7]=(float)y;

		glVertexPointer(2,GL_FLOAT,0,vertexes);
	
		glColor4f(0.9f,0.9f,0.9f,1.0f);
		glDrawArrays(GL_QUADS,0,4);
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
			vertexes[0]=vertexes[6]=(float)box.lx;
			vertexes[2]=vertexes[4]=(float)(box.rx-list_palette_scroll_wid);
			vertexes[1]=vertexes[3]=(float)((y-list_item_font_high)+1);
			vertexes[5]=vertexes[7]=(float)y;

			glVertexPointer(2,GL_FLOAT,0,vertexes);
		
			glColor4f(1.0f,1.0f,0.0f,1.0f);
			glDrawArrays(GL_QUADS,0,4);
		}
	}

		// disable/enable colors

	if (!item->disabled) {
		col.r=col.g=col.b=0.0f;
	}
	else {
		col.r=col.g=0.0f;
		col.b=1.0f;
	}

		// draw item
		
	x=item->x;
	y=item->y-(list->scroll_page*list_item_scroll_size);

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
			list_palette_draw_item_color_box(list,item,item->value.col_ptr);
			break;

			// string

		case list_item_ctrl_string:
			text_draw(x,y,list_item_font_size,&col,item->name);
			list_palette_draw_item_string(list,item,item->value.str);
			list_palette_draw_item_button(list,idx);
			break;

			// int

		case list_item_ctrl_int:
			sprintf(str,"%d",*item->value.int_ptr);
			text_draw(x,y,list_item_font_size,&col,item->name);
			list_palette_draw_item_string(list,item,str);
			list_palette_draw_item_button(list,idx);
			break;

			// float

		case list_item_ctrl_float:
			sprintf(str,"%.2f",*item->value.float_ptr);
			text_draw(x,y,list_item_font_size,&col,item->name);
			list_palette_draw_item_string(list,item,str);
			list_palette_draw_item_button(list,idx);
			break;

			// checkbox

		case list_item_ctrl_checkbox:
			text_draw(x,y,list_item_font_size,NULL,item->name);
			list_palette_draw_item_check_box(list,item,*item->value.bool_ptr);
			break;

			// pick color

		case list_item_ctrl_pick_color:
			text_draw(x,y,list_item_font_size,NULL,item->name);
			list_palette_draw_item_color_box(list,item,item->value.col_ptr);
			break;

	}
	
		// header counts
		
	if (item->ctrl_type==list_item_ctrl_header) {
		if (item->count!=-1) {
			sprintf(str,"%d",item->count);
			text_draw_right((box.rx-(list_palette_scroll_wid+4)),y,list_item_font_size,NULL,str);
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
	
		// check if scrolling is bad,
		// usually when a page was switched
		
	if (list->scroll_page>list_palette_get_scroll_page_count(list)) list->scroll_page=0;

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
	int						y;
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
	
	y=item->y-(list->scroll_page*list_item_scroll_size);
	
	main_wind_draw();

	while (!os_track_mouse_location(&pt,NULL)) {
	
		out_box=FALSE;
		out_box=out_box||(pt.x>=(box.rx-list_palette_scroll_wid));
		out_box=out_box||(pt.y<(y-list_item_font_high));
		out_box=out_box||(pt.y>=y);
		
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

		// is there a button

	list->button_click=FALSE;

	if (!list_picker.on) {

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

		// handle click editing

	if (!list_picker.on) {

		switch (list->items[item_idx].ctrl_type) {
			
			case list_item_ctrl_int:
				if (!double_click) return(FALSE);
				dialog_property_string_run(list_string_value_int,(void*)list->items[item_idx].value.int_ptr,0,0,0);
				main_wind_draw();
				return(FALSE);

			case list_item_ctrl_float:
				if (!double_click) return(FALSE);
				dialog_property_string_run(list_string_value_float,(void*)list->items[item_idx].value.float_ptr,0,0,0);
				main_wind_draw();
				return(FALSE);

			case list_item_ctrl_checkbox:
				*list->items[item_idx].value.bool_ptr=!(*list->items[item_idx].value.bool_ptr);
				main_wind_draw();
				return(FALSE);
		
			case list_item_ctrl_pick_color:
				os_pick_color(list->items[item_idx].value.col_ptr);
				main_wind_draw();
				return(FALSE);

		}
	}

		// run any list picker modes

	list_palette_choose_picking_mode(list,item_idx,double_click);

	return(TRUE);
}