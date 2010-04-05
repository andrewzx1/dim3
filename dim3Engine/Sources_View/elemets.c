/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Control Elements

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "video.h"
#include "inputs.h"

extern bool					game_app_active;

extern hud_type				hud;
extern setup_type			setup;

int							nelement,element_click_down_id,
							element_open_text_field_id,element_open_combo_id;
element_type				elements[max_element];

SDL_mutex					*element_thread_lock;

/* =======================================================

      Initialize/Shutdown Elements
      
======================================================= */

void element_reset_controls(void)
{
	nelement=0;

	element_click_down_id=-1;
	element_open_text_field_id=-1;
	element_open_combo_id=-1;
}

void element_release_control_memory(void)
{
	int				n,k;
	element_type	*element;
	
	element=elements;
	
	for (n=0;n<nelement;n++) {
		
		switch (element->type) {
		
			case element_type_button:
				view_images_free_single(element->setup.button.image_idx);
				view_images_free_single(element->setup.button.image_select_idx);
				break;
				
			case element_type_bitmap:
				view_images_free_single(element->setup.button.image_idx);
				break;
				
			case element_type_table:
				if (element->data!=NULL) free(element->data);
				for (k=0;k!=element_table_max_image;k++) {
					view_images_free_single(element->setup.table.images[k].image_idx);
				}
				break;
				
			case element_type_text_box:
				if (element->data!=NULL) free(element->data);
				break;
		
		}
		
		element++;
	}
}

void element_initialize(void)
{
	element_reset_controls();

	element_thread_lock=SDL_CreateMutex();		// used to make elements thread safe for certain UIs
}

void element_shutdown(void)
{
	element_release_control_memory();

	SDL_DestroyMutex(element_thread_lock);
}

/* =======================================================

      Clear Elements
      
======================================================= */

void element_clear(void)
{
	SDL_mutexP(element_thread_lock);

	element_release_control_memory();
	element_reset_controls();

	SDL_mutexV(element_thread_lock);
}

/* =======================================================

      Element Info
      
======================================================= */

inline int element_get_control_high(void)
{
	return((int)(((float)hud.scale_x)*element_control_draw_height)+5);
}

inline int element_get_padding(void)
{
	return((int)(((float)hud.scale_x)*element_control_padding_factor));
}

inline int element_get_tab_margin(void)
{
	return((int)(((float)hud.scale_x)*0.05f));
}

inline int element_get_tab_control_high(void)
{
	return(gl_text_get_char_height(hud.font.text_size_small));
}

inline void element_get_button_bottom_left(int *x,int *y,int wid,int high)
{
	*x=element_get_tab_margin()+element_get_padding();
	*y=hud.scale_y-(element_get_tab_margin()+element_get_padding());
}

inline void element_get_button_bottom_right(int *x,int *y,int wid,int high)
{
	*x=hud.scale_x-(element_get_tab_margin()+element_get_padding());
	*y=hud.scale_y-(element_get_tab_margin()+element_get_padding());
}

/* =======================================================

      Add Elements
      
======================================================= */

void element_button_text_add(char *name,int id,int x,int y,int wid,int high,int x_pos,int y_pos)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);
	
	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_button;
	
	element->x=x;
	element->y=y;
	
	element->selectable=TRUE;
	element->enabled=TRUE;
	element->hidden=FALSE;

	element->setup.button.text_only=TRUE;
	strcpy(element->setup.button.name,name);
	
	element->setup.button.image_idx=-1;
	element->setup.button.image_select_idx=-1;
	
	element->wid=wid;
	element->high=high;
	
	switch (x_pos) {
		case element_pos_right:
			element->x-=element->wid;
			break;
		case element_pos_center:
			element->x-=(element->wid>>1);
			break;
	}

	switch (y_pos) {
		case element_pos_bottom:
			element->y-=element->high;
			break;
		case element_pos_center:
			element->y-=(element->high>>1);
			break;
	}

	SDL_mutexV(element_thread_lock);
}

void element_button_bitmap_add(char *path,char *path2,int id,int x,int y,int wid,int high,int x_pos,int y_pos)
{
	element_type	*element;
	bitmap_type		*bitmap;

	SDL_mutexP(element_thread_lock);
	
	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_button;
	
	element->x=x;
	element->y=y;
	
	element->selectable=TRUE;
	element->enabled=TRUE;
	element->hidden=FALSE;

	element->setup.button.text_only=FALSE;
	
		// get button graphics
		
	element->setup.button.image_idx=view_images_load_single(path,FALSE,TRUE);
	element->setup.button.image_select_idx=view_images_load_single(path2,FALSE,TRUE);

		// setup size and position
		
	if ((wid!=-1) && (high!=-1)) {
		element->wid=wid;
		element->high=high;
	}
	else {
		bitmap=view_images_get_bitmap(element->setup.button.image_idx);
		element->wid=bitmap->wid;
		element->high=bitmap->high;
	}
	
	switch (x_pos) {
		case element_pos_right:
			element->x-=element->wid;
			break;
		case element_pos_center:
			element->x-=(element->wid>>1);
			break;
	}

	switch (y_pos) {
		case element_pos_bottom:
			element->y-=element->high;
			break;
		case element_pos_center:
			element->y-=(element->high>>1);
			break;
	}

	SDL_mutexV(element_thread_lock);
}

void element_bitmap_add(char *path,int id,int x,int y,int wid,int high,bool framed)
{
	element_type	*element;
	bitmap_type		*bitmap;

	SDL_mutexP(element_thread_lock);
	
	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_bitmap;
	
	element->setup.button.image_idx=view_images_load_single(path,FALSE,TRUE);
	
	element->x=x;
	element->y=y;
	
	if ((wid==-1) || (high==-1)) {
		bitmap=view_images_get_bitmap(element->setup.button.image_idx);
		element->wid=bitmap->wid;
		element->high=bitmap->high;
	}
	else {
		element->wid=wid;
		element->high=high;
	}
	
	element->selectable=FALSE;
	element->enabled=TRUE;
	element->hidden=FALSE;
	element->framed=framed;

	SDL_mutexV(element_thread_lock);
}

void element_text_add(char *str,int id,int x,int y,int size,int just,bool selectable,bool alert)
{
	int				wid,high;
	char			*c,*c2;
	element_type	*element;

	SDL_mutexP(element_thread_lock);
	
	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_text;
	
	element->x=x;
	element->y=y;
	
	element->setup.text.size=size;
	element->setup.text.just=just;
	element->setup.text.alert=alert;

	element->selectable=selectable;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	strcpy(element->str,str);

		// need to calculate size through returns

	high=gl_text_get_char_height(size);

	element->wid=0;
	element->high=high;

	c=element->str;
	
	while (*c!=0x0) {
		c2=strstr(c,"{r}");
		if (c2!=NULL) *c2=0x0;
		
		wid=gl_text_get_string_width(font_interface_index,size,c);
		if (wid>element->wid) element->wid=wid;
		
		if (c2==NULL) break;

		element->high+=high;
		
		*c2='{';
		
		c=c2+3;
	}

	SDL_mutexV(element_thread_lock);
}

void element_text_field_add(char *str,char *value_str,int max_value_str_sz,int id,int x,int y,bool selectable)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_text_field;
	
	element->x=x;
	element->y=y;

	element->wid=(int)(((float)hud.scale_x)*element_control_draw_long_width);
	element->high=(int)(((float)hud.scale_x)*element_control_draw_height);
	
	element->selectable=selectable;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	strcpy(element->str,str);
	strncpy(element->value_str,value_str,max_value_str_sz);
	element->value_str[max_value_str_sz-1]=0x0;

	element->max_value_str_sz=max_value_str_sz;

	SDL_mutexV(element_thread_lock);
}

void element_checkbox_add(char *str,int value,int id,int x,int y,bool selectable)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_checkbox;
	
	element->x=x;
	element->y=y;
	
	element->selectable=selectable;
	element->enabled=TRUE;
	element->hidden=FALSE;

	element->wid=(int)(((float)hud.scale_x)*element_control_draw_height);
	element->high=(int)(((float)hud.scale_x)*element_control_draw_height);
	
	strcpy(element->str,str);
	
	element->value=value;

	SDL_mutexV(element_thread_lock);
}

void element_combo_add(char *str,char *combo_data,int value,int id,int x,int y,bool selectable)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_combo;
	
	element->x=x;
	element->y=y;
	
	element->selectable=selectable;
	element->enabled=TRUE;
	element->hidden=FALSE;

	element->wid=(int)(((float)hud.scale_x)*element_control_draw_long_width);
	element->high=(int)(((float)hud.scale_x)*element_control_draw_height);
	
	strcpy(element->str,str);
	
	element->value=value;
	element->data=combo_data;

	SDL_mutexV(element_thread_lock);
}

void element_slider_add(char *str,float value,float value_min,float value_max,int id,int x,int y,bool selectable)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_slider;
	
	element->x=x;
	element->y=y;
	
	element->selectable=selectable;
	element->enabled=TRUE;
	element->hidden=FALSE;

	element->wid=(int)(((float)hud.scale_x)*element_control_draw_long_width);
	element->high=(int)(((float)hud.scale_x)*element_control_draw_height);

	strcpy(element->str,str);
	
	element->setup.slider.min=value_min;
	element->setup.slider.max=value_max;
	element->setup.slider.value=(value-value_min)/(value_max-value_min);

	SDL_mutexV(element_thread_lock);
}

int element_table_add_get_data_size(char *row_data)
{
	int			count;
	char		*c;
	
	count=0;
	c=row_data;
	
	while (TRUE) {
		if (*c==0x0) break;
		c+=128;
		count++;
	}

	return((count+1)*128);
}

void element_table_add(element_column_type* cols,char *row_data,int id,int ncolumn,int x,int y,int wid,int high,bool checkbox,int bitmap_mode)
{
	int				n,sz;
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_table;
	
	element->setup.table.busy_count=element->setup.table.busy_total_count=-1;
	
	element->x=x;
	element->y=y;
	element->wid=wid;
	element->high=high;
	
	element->selectable=TRUE;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	element->value=-1;
	element->offset=0;
	
	if (row_data==NULL) {
		element->data=NULL;
	}
	else {
		sz=element_table_add_get_data_size(row_data);
		element->data=malloc(sz);
		memmove(element->data,row_data,sz);
	}

	element->setup.table.checkbox=checkbox;
	element->setup.table.bitmap_mode=bitmap_mode;

	bzero(element->setup.table.checks,element_table_max_check);
	
	for (n=0;n!=element_table_max_image;n++) {
		element->setup.table.images[n].image_idx=-1;
	}
	
	element->setup.table.next_image_idx=0;

		// if checkbox is on, we need to
		// add the checkbox column and remove
		// that space from the first column

	if (!checkbox) {
		element->setup.table.ncolumn=ncolumn;
		memmove(element->setup.table.cols,cols,(sizeof(element_column_type)*ncolumn));
	}
	else {
		element->setup.table.ncolumn=ncolumn+1;
		memmove(&element->setup.table.cols[1],cols,(sizeof(element_column_type)*ncolumn));

		element->setup.table.cols[0].percent_size=element_table_check_column_size;
		element->setup.table.cols[0].name[0]=0x0;

		element->setup.table.cols[1].percent_size-=element_table_check_column_size;
	}
	
	SDL_mutexV(element_thread_lock);
}

void element_tab_add(char *tab_list,int value,int id,int ntab)
{
	int				n,margin;
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_tab;
	
		// tab elements are special as they fit the screen
		// so set the box to just the tab part
		
	margin=element_get_tab_margin();
	element->x=margin;
	element->y=margin;
	element->wid=hud.scale_x-(margin*2);
	element->high=gl_text_get_char_height(hud.font.text_size_small);
	
	element->selectable=TRUE;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	element->value=value;
	element->offset=0;

	element->setup.tab.ntab=ntab;
	
	for (n=0;n!=ntab;n++) {
		strcpy(element->setup.tab.name[n],(char*)&tab_list[n*name_str_len]);
	}

	SDL_mutexV(element_thread_lock);
}

void element_color_add(char *str,int value,int id,int x,int y,bool selectable)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_color;
	
	element->x=x;
	element->y=y;
	
	element->selectable=selectable;
	element->enabled=TRUE;
	element->hidden=FALSE;

	element->wid=(int)(((float)hud.scale_x)*element_control_draw_long_width);
	element->high=(int)(((float)hud.scale_x)*element_control_draw_height);

	strcpy(element->str,str);

	element->value=value;

	SDL_mutexV(element_thread_lock);
}

void element_text_box_add(char *data,int id,int x,int y,int wid,int high)
{
	int				sz;
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_text_box;
	
	element->x=x;
	element->y=y;
	element->wid=wid;
	element->high=high;
	
	element->selectable=TRUE;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	element->value=-1;
	element->offset=0;
	
	element->setup.text_box.scroll_up_ok=element->setup.text_box.scroll_down_ok=FALSE;
	
	if (data==NULL) {
		element->data=NULL;
	}
	else {
		sz=strlen(data)+1;
		element->data=malloc(sz);
		memmove(element->data,data,sz);
	}
	
	SDL_mutexV(element_thread_lock);
}

void element_info_field_add(char *str,char *value_str,int id,int x,int y)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_info_field;
	
	element->x=x;
	element->y=y;

	element->wid=(int)(((float)hud.scale_x)*element_control_draw_long_width);
	element->high=(int)(((float)hud.scale_x)*element_control_draw_height);
	
	element->selectable=FALSE;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	strcpy(element->str,str);
	strncpy(element->value_str,value_str,max_element_value_str_len);
	element->value_str[max_element_value_str_len-1]=0x0;

	SDL_mutexV(element_thread_lock);
}

/* =======================================================

      Change Elements
      
======================================================= */

element_type* element_find(int id)
{
	int					n;
	element_type		*element;
		
	element=elements;
	
	for (n=0;n<nelement;n++) {
		if (element->id==id) return(element);
		element++;
	}
	
	return(NULL);
}

void element_enable(int id,bool enabled)
{
	element_type		*element;
	
	element=element_find(id);
	if (element!=NULL) element->enabled=enabled;
}

void element_hide(int id,bool hide)
{
	element_type		*element;
	
	element=element_find(id);
	if (element!=NULL) element->hidden=hide;
}

void element_table_busy(int id,char *str,int count,int total_count)
{
	element_type		*element;
	
	element=element_find(id);
	if (element!=NULL) {
		if (str==NULL) {
			element->setup.table.busy_str[0]=0x0;
		}
		else {
			strcpy(element->setup.table.busy_str,str);
		}
		element->setup.table.busy_count=count;
		element->setup.table.busy_total_count=total_count;
	}
}

void element_text_change(int id,char *str)
{
	element_type		*element;
	
	element=element_find(id);
	if (element!=NULL) strcpy(element->str,str);
}

/* =======================================================

      Select Element
      
======================================================= */

void element_get_box(element_type *element,int *lft,int *rgt,int *top,int *bot)
{
	switch (element->type) {
	
		case element_type_button:
			*lft=element->x;
			*rgt=(*lft)+element->wid;
			*top=element->y;
			*bot=(*top)+element->high;
			return;
			
		case element_type_bitmap:
		case element_type_table:
		case element_type_tab:
		case element_type_text_box:
			*lft=element->x;
			*rgt=(*lft)+element->wid;
			*top=element->y;
			*bot=(*top)+element->high;
			return;
			
		case element_type_text:
			switch (element->setup.text.just) {
				case tx_center:
					*lft=element->x-(element->wid>>1);
					*rgt=element->x+(element->wid>>1);
					break;
				case tx_right:
					*lft=element->x-element->wid;
					*rgt=element->x;
					break;
				default:
					*lft=element->x;
					*rgt=element->x+element->wid;
					break;
			}
			*top=element->y-element->high;
			*bot=element->y;
			return;

		case element_type_text_field:
		case element_type_combo:
		case element_type_slider:
		case element_type_color:
		case element_type_info_field:
			*lft=element->x+5;
			*rgt=(element->x+10)+element->wid;
			*top=(element->y-element->high)-1;
			*bot=element->y+1;
			return;
			
		case element_type_checkbox:
			*lft=element->x+5;
			*rgt=(element->x+5)+element->wid;
			*top=element->y-element->high;
			*bot=element->y;
			return;
			
	}
	
	*lft=*rgt=*top=*bot=-1;
}

int element_find_for_xy(int x,int y)
{
	int					n,lft,rgt,top,bot;
	element_type		*element;
	
	element=elements;
	
	for (n=0;n<nelement;n++) {
		if ((element->selectable) && (!element->hidden)) {
			element_get_box(element,&lft,&rgt,&top,&bot);
			if ((x>=lft) && (x<=rgt) && (y>=top) && (y<=bot)) return(element->id);
		}
		element++;
	}
	
	return(-1);
}

/* =======================================================

      Button Element
      
======================================================= */

void element_draw_button_text(element_type *element,int sel_id)
{
	int				x,y,lft,rgt,top,bot;
	float			alpha;
	d3col			txt_col,outline_col;
	
	if (element->enabled) {
		alpha=1.0f;
		if (element->id==sel_id) {
			memmove(&outline_col,&hud.color.control_mouse_over,sizeof(d3col));
			memmove(&txt_col,&hud.color.control_mouse_over,sizeof(d3col));
		}
		else {
			memmove(&outline_col,&hud.color.button_outline,sizeof(d3col));
			memmove(&txt_col,&hud.color.button_text,sizeof(d3col));
		}
	}
	else {
		alpha=0.3f;
		memmove(&outline_col,&hud.color.button_outline,sizeof(d3col));
		memmove(&txt_col,&hud.color.button_text,sizeof(d3col));
	}
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	
		// button background and outline

	view_draw_next_vertex_object_2D_color_quad(&hud.color.button_fill,alpha,lft,rgt,top,bot);
	view_draw_next_vertex_object_2D_line_quad(&outline_col,alpha,lft,rgt,top,bot);
	
		// button text

	x=(lft+rgt)>>1;
	y=((top+bot)>>1)-(hud.font.text_size_small/10);

	gl_text_start(font_interface_index,hud.font.text_size_small);
	gl_text_draw(x,y,element->setup.button.name,tx_center,TRUE,&txt_col,alpha);
	gl_text_end();
}

void element_draw_button_bitmap(element_type *element,int sel_id)
{
	int				lft,rgt,top,bot;
	float			alpha;
	GLuint			gl_id;
	
	if (element->enabled) {
		if ((element->id==sel_id) && (!view_images_is_empty(element->setup.button.image_select_idx))) {
			gl_id=view_images_get_gl_id(element->setup.button.image_select_idx);
			alpha=1.0f;
		}
		else {
			gl_id=view_images_get_gl_id(element->setup.button.image_idx);
			alpha=1.0f;
		}
	}
	else {
		gl_id=view_images_get_gl_id(element->setup.button.image_idx);
		alpha=0.5f;
	}
	
	element_get_box(element,&lft,&rgt,&top,&bot);

	view_draw_next_vertex_object_2D_texture_quad(gl_id,NULL,alpha,lft,rgt,top,bot,0.0f,0.0f);
}

void element_draw_button(element_type *element,int sel_id)
{
	if (element->setup.button.text_only) {
		element_draw_button_text(element,sel_id);
	}
	else {
		element_draw_button_bitmap(element,sel_id);
	}
}

/* =======================================================

      Bitmap Element
      
======================================================= */

void element_draw_bitmap(element_type *element)
{
	int				lft,rgt,top,bot;
	GLuint			gl_id;
	
	element_get_box(element,&lft,&rgt,&top,&bot);

		// the picture
		
	gl_id=view_images_get_gl_id(element->setup.button.image_idx);
	view_draw_next_vertex_object_2D_texture_quad(gl_id,NULL,1.0f,lft,rgt,top,bot,0.0f,0.0f);
	
		// the frame
		
	if (element->framed) {
		view_draw_next_vertex_object_2D_line_quad(&hud.color.control_outline,1.0f,lft,rgt,top,bot);
	}
}

/* =======================================================

      Text Element
      
======================================================= */

void element_draw_text(element_type *element,int sel_id)
{
	int				y,high;
	char			*c,*c2;
	d3col			col;

		// get text color

	if (element->setup.text.alert) {
		col.r=1.0f;
		col.g=col.b=0.0f;
	}
	else {
		if (!element->enabled) {
			memmove(&col,&hud.color.control_disabled,sizeof(d3col));
		}
		else {
			if ((element->id!=-1) && (element->id==sel_id)) {
				memmove(&col,&hud.color.control_mouse_over,sizeof(d3col));
			}
			else {
				memmove(&col,&hud.color.control_label,sizeof(d3col));
			}
		}
	}

		// get height

	y=element->y;
	high=gl_text_get_char_height(element->setup.text.size);

		// draw text

	gl_text_start(font_interface_index,element->setup.text.size);

	c=element->str;
	
	while (*c!=0x0) {
		c2=strstr(c,"{r}");
		if (c2!=NULL) *c2=0x0;
		
		gl_text_draw(element->x,y,c,element->setup.text.just,FALSE,&col,1.0f);
		
		if (c2==NULL) break;
		
		*c2='{';
		
		c=c2+3;
		y+=high;
	}

	gl_text_end();
}

/* =======================================================

      Text Field Element
      
======================================================= */

void element_click_text_field(element_type *element)
{
	element_open_text_field_id=element->id;
}

bool element_click_text_field_open(element_type *element,int x,int y)
{
	int				high,lft,rgt,top,bot;
	
	high=gl_text_get_char_height(hud.font.text_size_small);
		
		// control box
		
	lft=element->x+3;
	rgt=lft+252;
	top=(element->y-high)-1;
	bot=element->y;
	
		// in box?
		
	if ((x<lft) || (x>rgt) || (y<top) || (y>bot)) return(FALSE);

	return(TRUE);
}

void element_draw_text_field(element_type *element,int sel_id)
{
	int				x,y,ky,lft,rgt,top,bot;
	float			alpha;
	char			txt[256];
	d3col			col;
	
	x=element->x;
	y=element->y;
	
		// label

	ky=y-(element->high>>1);
		
	gl_text_start(font_interface_index,hud.font.text_size_small);
	gl_text_draw((x-5),ky,element->str,tx_right,TRUE,&hud.color.control_label,1.0f);
	gl_text_draw(x,(ky-1),":",tx_center,TRUE,&hud.color.control_label,1.0f);
	gl_text_end();
		
		// control box
		
	lft=x+10;
	rgt=lft+element->wid;
	top=ky-(element->high>>1);
	bot=top+element->high;
	
	alpha=(element->enabled?1.0f:0.3f);

		// background

	col.r=col.g=col.b=0.0f;
	view_draw_next_vertex_object_2D_color_quad(&col,0.2f,lft,rgt,top,bot);

		// outline

	if ((element->id==sel_id) && (element->enabled)) {
		view_draw_next_vertex_object_2D_line_quad(&hud.color.control_mouse_over,alpha,lft,rgt,top,bot);
	}
	else {
		view_draw_next_vertex_object_2D_line_quad(&hud.color.control_outline,alpha,lft,rgt,top,bot);
	}

		// control text

	strcpy(txt,element->value_str);
	if (element_open_text_field_id==element->id) strcat(txt,"_");

	gl_text_start(font_interface_index,hud.font.text_size_small);
		
	if (element->enabled) {
		if (element->id==element_open_text_field_id) {
			gl_text_draw((x+15),(ky-1),txt,tx_left,TRUE,&hud.color.control_mouse_over,1.0f);
		}
		else {
			gl_text_draw((x+15),(ky-1),txt,tx_left,TRUE,&hud.color.control_text,1.0f);
		}
	}
	else {
		gl_text_draw((x+15),(ky-1),txt,tx_left,TRUE,&hud.color.control_disabled,1.0f);
	}
	
	gl_text_end();
}

/* =======================================================

      Checkbox Element
      
======================================================= */

void element_click_checkbox(element_type *element)
{
	element->value=element->value^0x1;
}

void element_draw_checkbox_control(int x,int y,int draw_sz,bool checked,bool enabled,bool hilite)
{
	int			lft,rgt,top,bot,px[4],py[4];
	float		alpha,f;
	d3col		col,col2,gradient_start,gradient_end;

		// checkbox

	lft=x;
	rgt=lft+draw_sz;
	top=y-(draw_sz>>1);
	bot=top+draw_sz;

	alpha=(enabled?1.0f:0.3f);

		// background
		
	memmove(&gradient_start,&hud.color.control_fill,sizeof(d3col));
	gradient_end.r=gradient_start.r*element_gradient_factor;
	gradient_end.g=gradient_start.g*element_gradient_factor;
	gradient_end.b=gradient_start.b*element_gradient_factor;

	view_draw_next_vertex_object_2D_color_poly(lft,top,&gradient_start,rgt,top,&gradient_start,rgt,bot,&gradient_end,lft,bot,&gradient_end,alpha);

		// check

	if (checked) {
		col.r=hud.color.control_hilite.r*element_gradient_factor;
		col.g=hud.color.control_hilite.g*element_gradient_factor;
		col.b=hud.color.control_hilite.b*element_gradient_factor;

		f=element_gradient_factor*1.5f;

		col2.r=hud.color.control_hilite.r*f;
		col2.g=hud.color.control_hilite.g*f;
		col2.b=hud.color.control_hilite.b*f;

		px[0]=lft;
		py[0]=bot-((bot-top)/3);
		px[1]=lft+((rgt-lft)/8);
		py[1]=top+((bot-top)/3);
		px[2]=lft+((rgt-lft)/3);
		py[2]=bot-((bot-top)/3);
		px[3]=lft+((rgt-lft)/3);
		py[3]=bot;

		view_draw_next_vertex_object_2D_color_poly(px[0],py[0],&col,px[1],py[1],&col,px[2],py[2],&col2,px[3],py[3],&hud.color.control_hilite,alpha);

		px[0]=lft+((rgt-lft)/3);
		py[0]=bot-((bot-top)/3);
		px[1]=rgt-((rgt-lft)/4);
		py[1]=top;
		px[2]=rgt;
		py[2]=top+((bot-top)/4);
		px[3]=lft+((rgt-lft)/3);
		py[3]=bot;

		view_draw_next_vertex_object_2D_color_poly(px[0],py[0],&col2,px[1],py[1],&col,px[2],py[2],&hud.color.control_hilite,px[3],py[3],&hud.color.control_hilite,alpha);
	}

		// outline

	if ((hilite) && (enabled)) {
		view_draw_next_vertex_object_2D_line_quad(&hud.color.control_mouse_over,alpha,lft,rgt,top,bot);
	}
	else {
		view_draw_next_vertex_object_2D_line_quad(&hud.color.control_outline,alpha,lft,rgt,top,bot);
	}
}

void element_draw_checkbox(element_type *element,int sel_id)
{
	int				x,y,ky;
	
	x=element->x;
	y=element->y;
	
		// label

	ky=y-(element->high>>1);

	gl_text_start(font_interface_index,hud.font.text_size_small);
	gl_text_draw((x-5),ky,element->str,tx_right,TRUE,&hud.color.control_label,1.0f);
	gl_text_draw(x,(ky-1),":",tx_center,TRUE,&hud.color.control_label,1.0f);
	gl_text_end();
	
		// checkbox

	element_draw_checkbox_control((x+10),ky,element->high,(element->value!=0),element->enabled,(element->id==sel_id));
}

/* =======================================================

      Combo Element
      
======================================================= */

int element_get_combo_list_count(element_type *element)
{
	int			count;
	char		*c;
	
	count=0;
	c=element->data;
	
	while (TRUE) {
		if (*c==0x0) return(count);
		c+=32;
		count++;
	}

	return(0);
}

void element_click_combo(element_type *element)
{
	element_open_combo_id=element->id;
}

void element_box_combo_open(element_type *element,int *lft,int *rgt,int *top,int *bot)
{
	int				cnt;

	cnt=element_get_combo_list_count(element);
		
	*lft=element->x+10;
	*rgt=(*lft+element->wid)-element->high;
	*top=element->y-element->high;
	*bot=*top+(cnt*element->high);
}

bool element_in_combo_open(element_type *element,int x,int y)
{
	int				lft,rgt,top,bot;
	
	element_box_combo_open(element,&lft,&rgt,&top,&bot);
	if ((x<lft) || (x>rgt) || (y<top) || (y>bot)) return(FALSE);
	
	return(TRUE);
}

bool element_click_combo_open(element_type *element,int x,int y)
{
	int				lft,rgt,top,bot;

		// in box?
		
	element_box_combo_open(element,&lft,&rgt,&top,&bot);
	if ((x<lft) || (x>rgt) || (y<top) || (y>bot)) return(FALSE);
	
		// change clicked item and close
		
	element->value=(y-top)/element->high;
	
		// close open combo
		
	element_open_combo_id=-1;
	
	return(TRUE);
}

void element_draw_combo(element_type *element,int sel_id)
{
	int				x,y,sz,kx,ky,lft,rgt,top,bot;
	char			str[256];
	float			alpha;
	d3col			col,gradient_start,gradient_end;
	
	x=element->x;
	y=element->y;
	
		// label
	
	ky=y-(element->high>>1);

	gl_text_start(font_interface_index,hud.font.text_size_small);
	gl_text_draw((x-5),ky,element->str,tx_right,TRUE,&hud.color.control_label,1.0f);
	gl_text_draw(x,(ky-1),":",tx_center,TRUE,&hud.color.control_label,1.0f);
	gl_text_end();
		
		// combo box
		
	lft=x+10;
	rgt=lft+element->wid;
	top=ky-(element->high>>1);
	bot=top+element->high;

	alpha=(element->enabled?1.0f:0.3f);

		// background
		
	memmove(&gradient_start,&hud.color.control_fill,sizeof(d3col));
	gradient_end.r=gradient_start.r*element_gradient_factor;
	gradient_end.g=gradient_start.g*element_gradient_factor;
	gradient_end.b=gradient_start.b*element_gradient_factor;

	view_draw_next_vertex_object_2D_color_poly(lft,top,&gradient_start,rgt,top,&gradient_start,rgt,bot,&gradient_end,lft,bot,&gradient_end,alpha);

		// outline

	sz=(bot-top)-8;
	kx=(rgt-8)-sz;

	if ((element->id==sel_id) && (element->enabled)) {
		view_draw_next_vertex_object_2D_line_quad(&hud.color.control_mouse_over,alpha,lft,kx,top,bot);
		view_draw_next_vertex_object_2D_line_quad(&hud.color.control_mouse_over,alpha,kx,rgt,top,bot);
	}
	else {
		view_draw_next_vertex_object_2D_line_quad(&hud.color.control_outline,alpha,lft,kx,top,bot);
		view_draw_next_vertex_object_2D_line_quad(&hud.color.control_outline,alpha,kx,rgt,top,bot);
	}

		// arrow

	lft=(rgt-element->high)+4;
	rgt-=4;
	top+=4;
	bot-=4;

	if ((element->id==element_open_combo_id) || ((element->id==sel_id) && (element->enabled))) {
		view_draw_next_vertex_object_2D_color_trig(&hud.color.control_mouse_over,alpha,lft,rgt,top,bot,FALSE);
	}
	else {
		view_draw_next_vertex_object_2D_color_trig(&hud.color.control_hilite,alpha,lft,rgt,top,bot,FALSE);
	}

	col.r=col.g=col.b=0.0f;
	view_draw_next_vertex_object_2D_line_trig(&col,alpha,lft,rgt,top,bot,FALSE);

		// control text

	strcpy(str,(element->data+(element->value*32)));

	gl_text_start(font_interface_index,hud.font.text_size_small);
	gl_text_draw((x+15),(ky-1),str,tx_left,TRUE,&hud.color.control_text,alpha);
	gl_text_end();
}

void element_draw_combo_open(element_type *element)
{
	int				x,y,n,cnt,lft,rgt,top,bot,sel_item_idx;
	char			str[256];
	d3col			gradient_start,gradient_end;
	
		// combo count

	cnt=element_get_combo_list_count(element);

		// get selected item

	input_gui_get_mouse_position(&x,&y);

	sel_item_idx=-1;

	element_box_combo_open(element,&lft,&rgt,&top,&bot);
	if ((x>=lft) && (x<=rgt) && (y>=top) && (y<=bot)) {
		sel_item_idx=(y-top)/element->high;
	}

		// drawing sizes

	x=element->x;
	y=element->y;
	
	lft=x+10;
	rgt=(lft+element->wid)-element->high;
	top=y-element->high;

		// background
		
	memmove(&gradient_start,&hud.color.control_fill,sizeof(d3col));
	gradient_end.r=gradient_start.r*element_gradient_factor;
	gradient_end.g=gradient_start.g*element_gradient_factor;
	gradient_end.b=gradient_start.b*element_gradient_factor;

	bot=top+(cnt*element->high);

	view_draw_next_vertex_object_2D_color_poly(lft,top,&gradient_start,rgt,top,&gradient_start,rgt,bot,&gradient_end,lft,bot,&gradient_end,1.0f);

		// combo items

	for (n=0;n!=cnt;n++) {

		bot=top+element->high;

			// selection

		if (element->value==n) view_draw_next_vertex_object_2D_color_quad(&hud.color.control_hilite,1.0f,lft,rgt,top,bot);

			// text

		gl_text_start(font_interface_index,hud.font.text_size_small);
		strcpy(str,(element->data+(n*32)));

		if (sel_item_idx==n) {
			gl_text_draw((x+10),((top+bot)>>1),str,tx_left,TRUE,&hud.color.control_mouse_over,1.0f);
		}
		else {
			gl_text_draw((x+10),((top+bot)>>1),str,tx_left,TRUE,&hud.color.control_text,1.0f);
		}

		gl_text_end();

		top+=element->high;
	}

		// outline

	top=y-element->high;
	bot=top+(cnt*element->high);

	view_draw_next_vertex_object_2D_line_quad(&hud.color.control_outline,1.0f,lft,rgt,top,bot);
}

/* =======================================================

      Slider Element
      
======================================================= */

void element_click_slider(element_type *element,int x,int y)
{
	x-=(element->x+10);
	if (x<0) x=0;
	if (x>element->wid) x=element->wid;
	
	element->setup.slider.value=((float)x)/((float)element->wid);
}

void element_move_slider(element_type *element,int x,int y)
{
	int				id;
	
	id=element_find_for_xy(x,y);
	if (id==element->id) element_click_slider(element,x,y);
}

void element_draw_slider(element_type *element,int sel_id)
{
	int				x,y,ky,lft,rgt,top,bot,mid;
	float			alpha;
	d3col			col2,gradient_start,gradient_end;
	
	x=element->x;
	y=element->y;
	
		// label
	
	ky=y-(element->high>>1);

	gl_text_start(font_interface_index,hud.font.text_size_small);
	gl_text_draw((x-5),ky,element->str,tx_right,TRUE,&hud.color.control_label,1.0f);
	gl_text_draw(x,(ky-1),":",tx_center,TRUE,&hud.color.control_label,1.0f);
	gl_text_end();
	
		// slider size
		
	lft=x+10;
	rgt=lft+element->wid;
	top=ky-(element->high>>1);
	bot=top+element->high;
	
	mid=lft+(int)(((float)element->wid)*element->setup.slider.value);

	alpha=(element->enabled?1.0f:0.3f);
	
		// background
		
	memmove(&gradient_start,&hud.color.control_fill,sizeof(d3col));
	gradient_end.r=gradient_start.r*element_gradient_factor;
	gradient_end.g=gradient_start.g*element_gradient_factor;
	gradient_end.b=gradient_start.b*element_gradient_factor;

	view_draw_next_vertex_object_2D_color_poly(lft,top,&gradient_start,rgt,top,&gradient_start,rgt,bot,&gradient_end,lft,bot,&gradient_end,1.0f);

		// slider value
		
	if (element->enabled) {
		col2.r=hud.color.control_hilite.r*0.5f;
		col2.g=hud.color.control_hilite.g*0.5f;
		col2.b=hud.color.control_hilite.b*0.5f;

		view_draw_next_vertex_object_2D_color_poly(lft,top,&hud.color.control_hilite,mid,top,&hud.color.control_hilite,mid,bot,&col2,lft,bot,&col2,alpha);
	}

		// outline

	if ((element->id==sel_id) && (element->enabled)) {
		view_draw_next_vertex_object_2D_line_quad(&hud.color.control_mouse_over,alpha,lft,rgt,top,bot);
	}
	else {
		view_draw_next_vertex_object_2D_line_quad(&hud.color.control_outline,alpha,lft,rgt,top,bot);
	}

		// slider drag

	if ((mid+16)>rgt) mid=rgt-16;

	col2.r=hud.color.control_hilite.r*0.5f;
	col2.g=hud.color.control_hilite.g*0.5f;
	col2.b=hud.color.control_hilite.b*0.5f;

	view_draw_next_vertex_object_2D_color_poly(mid,top,&hud.color.control_hilite,(mid+8),top,&col2,(mid+8),bot,&col2,mid,bot,&hud.color.control_hilite,alpha);
	view_draw_next_vertex_object_2D_color_poly((mid+8),top,&col2,(mid+16),top,&hud.color.control_hilite,(mid+16),bot,&hud.color.control_hilite,(mid+8),bot,&col2,alpha);

	view_draw_next_vertex_object_2D_line_quad(&hud.color.control_outline,alpha,mid,(mid+16),top,bot);
}

/* =======================================================

      Table Element
      
======================================================= */

int element_get_table_row_count(element_type *element)
{
	int			count;
	char		*c;
	
	c=element->data;
	if (c==NULL) return(0);
	
	count=0;
	
	while (TRUE) {
		if (*c==0x0) return(count);
		c+=128;
		count++;
	}

	return(0);
}

inline int element_get_table_row_high(element_type *element)
{
	if (element->setup.table.bitmap_mode==element_table_bitmap_none) return(gl_text_get_char_height(hud.font.text_size_small)+2);

	return(element_table_bitmap_size+2);
}

void element_click_table(element_type *element,int x,int y)
{
	int				high,row_high,row_cnt,cnt,scroll_high,my;
	bool			up_ok,down_ok;
	
		// get text sizes
		
	high=gl_text_get_char_height(hud.font.text_size_small)+2;
	row_high=element_get_table_row_high(element);
	
	row_cnt=element_get_table_row_count(element);
	
		// check for clicking in scroll bar

	if ((x>((element->x+element->wid)-24)) && (row_cnt!=0)) {

			// get scrolling sizes
			
		cnt=((element->high-(high+4))/row_high)-1;

		scroll_high=element->high/row_cnt;
		my=(28+(scroll_high*element->offset))+((scroll_high*(cnt+1))/2);

			// is up and down OK?

		up_ok=(element->offset!=0);
		down_ok=((element->offset+(cnt+1))<row_cnt);
		
		y-=element->y;
		
		if ((y>=(high+4)) && (y<=my) && (up_ok)) {
			element->offset-=(cnt+1);
		}
		else {
			if ((y>=my) && (down_ok)) {
				element->offset+=(cnt+1);
			}
		}
		
		return;
	}
	
		// select cliked on element
		
	y-=((element->y+4)+(high+2));
	element->value=element->offset+(y/element_get_table_row_high(element));
	
	if ((element->value<0) || (element->value>=row_cnt)) {
		element->value=-1;
	}

		// flip any check boxes

	if ((element->setup.table.checkbox) && (element->value>=0) && (element->value<element_table_max_check)) element->setup.table.checks[element->value]^=0x1;
}

void element_draw_table_row_column_lines(element_type *element,int ty,int by,float col_factor)
{
	int			n,x;
	float		f_wid;
	d3col		col;
	
	x=element->x;
	f_wid=(float)(element->wid-30);
	
	col.r=hud.color.control_outline.r*col_factor;
	col.g=hud.color.control_outline.g*col_factor;
	col.b=hud.color.control_outline.b*col_factor;

	for (n=1;n<element->setup.table.ncolumn;n++) {
		x+=(int)(element->setup.table.cols[n-1].percent_size*f_wid);
		view_draw_next_vertex_object_2D_line(&col,1.0f,x,ty,x,by);
	}
}

void element_draw_table_header_fill(element_type *element,int high)
{
	int				lft,rgt,top,bot,y;
	d3col			col2;
	
		// header
		
	element_get_box(element,&lft,&rgt,&top,&bot);
	
	bot=(top+high)+4;
	y=(top+bot)>>1;
	
	col2.r=hud.color.control_header.r*0.5f;
	col2.g=hud.color.control_header.g*0.5f;
	col2.b=hud.color.control_header.b*0.5f;
	
	view_draw_next_vertex_object_2D_color_poly(lft,top,&hud.color.control_header,rgt,top,&hud.color.control_header,rgt,y,&col2,lft,y,&col2,1.0f);
	view_draw_next_vertex_object_2D_color_poly(lft,y,&col2,rgt,y,&col2,rgt,bot,&hud.color.control_header,lft,bot,&hud.color.control_header,1.0f);
	
	element_draw_table_row_column_lines(element,top,bot,1.0f);
}

void element_draw_table_line_header(element_type *element,int x,int y,int wid,int row_high)
{
	int			n;
	d3col		col;
	
	col.r=col.g=col.b=0.0f;

	y+=((row_high>>1)-1);
	
	for (n=0;n!=element->setup.table.ncolumn;n++) {
		gl_text_start(font_interface_index,hud.font.text_size_small);
		gl_text_draw((x+4),y,element->setup.table.cols[n].name,tx_left,TRUE,&col,1.0f);
		gl_text_end();
		
		x+=(int)(element->setup.table.cols[n].percent_size*(float)wid);
	}
}

unsigned long element_draw_table_get_image_gl_id(element_type *element,int row_idx)
{
	int				n,idx;
	char			*c,*c2,subdir[256],filename[256],path[1024];
	
		// get path
		
	c=element->data+(row_idx*128);
	
	strcpy(subdir,c);
	c2=strchr(subdir,';');
	if (c2==NULL) return(-1);

	*c2=0x0;
	strcpy(filename,(c2+1));

	c2=strchr(filename,';');
	if (c2==NULL) return(-1);

	*c2=0x0;

	if (element->setup.table.bitmap_mode==element_table_bitmap_data) {
		file_paths_data(&setup.file_path_setup,path,subdir,filename,"png");
	}
	else {
		file_paths_documents(&setup.file_path_setup,path,subdir,filename,"png");
	}
	
		// is it already loaded?
		
	for (n=0;n!=element_table_max_image;n++) {
		if (element->setup.table.images[n].image_idx==-1) continue;
		if (strcmp(element->setup.table.images[n].path,path)==0) return(view_images_get_gl_id(element->setup.table.images[n].image_idx));
	}
	
		// need to load, find open bitmap
		
	idx=-1;
	
	for (n=0;n!=element_table_max_image;n++) {
		if (element->setup.table.images[n].image_idx!=-1) {
			idx=n;
			break;
		}
	}
	
		// no spots left, replace a image
		
	if (idx==-1) {
		idx=element->setup.table.next_image_idx;
		element->setup.table.next_image_idx++;
		if (element->setup.table.next_image_idx==element_table_max_image) element->setup.table.next_image_idx=0;
	}
	
		// open
		
	strcpy(element->setup.table.images[idx].path,path);
	element->setup.table.images[idx].image_idx=view_images_load_single(path,FALSE,TRUE);

	return(view_images_get_gl_id(element->setup.table.images[idx].image_idx));
}

void element_draw_table_line_data(element_type *element,int x,int y,int row,int wid,int row_high,d3col *txt_col,char *data)
{
	int				n,dx,dy,tx,col_wid,ctrl_sz;
	unsigned long	gl_id;
	char			*c,*c2,txt[256];
	bool			first_col,checked;
	d3col			col,col2;

	dx=x+4;
	dy=y+((row_high>>1)-1);

		// data column

	c=data;
	first_col=TRUE;
	
	for (n=0;n!=element->setup.table.ncolumn;n++) {

			// checkboxes

		if ((element->setup.table.checkbox) && (n==0)) {

			col_wid=(int)(element->setup.table.cols[n].percent_size*(float)wid);
			ctrl_sz=col_wid>>1;

			checked=FALSE;
			if ((row>=0) && (row<element_table_max_check)) checked=element->setup.table.checks[row];

			element_draw_checkbox_control((((dx-4)+(col_wid>>1))-(ctrl_sz>>1)),dy,ctrl_sz,checked,TRUE,FALSE);

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

				// draw bitmap

			gl_id=element_draw_table_get_image_gl_id(element,row);

			if (gl_id!=-1) {
				view_draw_next_vertex_object_2D_texture_quad(gl_id,NULL,1.0f,dx,(dx+element_table_bitmap_size),(y+1),((y+1)+element_table_bitmap_size),0.0f,0.0f);
				view_draw_next_vertex_object_2D_line_quad(&hud.color.control_outline,1.0f,dx,(dx+element_table_bitmap_size),(y+1),((y+1)+element_table_bitmap_size));
			}

				// missing graphic

			else {
				
				col.r=col.g=col.b=0.6f;
				col2.r=col2.g=col2.b=0.4f;

				view_draw_next_vertex_object_2D_color_poly(dx,(y+1),&col,(dx+element_table_bitmap_size),(y+1),&col,(dx+element_table_bitmap_size),((y+1)+element_table_bitmap_size),&col2,dx,((y+1)+element_table_bitmap_size),&col2,1.0f);
				view_draw_next_vertex_object_2D_line_quad(&hud.color.control_outline,1.0f,dx,(dx+element_table_bitmap_size),(y+1),((y+1)+element_table_bitmap_size));

				col.r=col.g=col.b=1.0f;

				gl_text_start(font_interface_index,hud.font.text_size_large);
				gl_text_draw((dx+(element_table_bitmap_size>>1)),(y+(element_table_bitmap_size>>1)),"?",tx_center,TRUE,&col,1.0f);
				gl_text_end();
			}

				// get to correct text

			c2=strchr(txt,';');
			if (c2!=NULL) {
				c2=strchr((c2+1),';');
				if (c2!=NULL) {
					strcpy(txt,(c2+1));
				}
			}

			tx+=(element_table_bitmap_size+4);
		}

		first_col=FALSE;
		
			// draw text
			
		gl_text_start(font_interface_index,hud.font.text_size_small);
		gl_text_draw(tx,dy,txt,tx_left,TRUE,txt_col,1.0f);
		gl_text_end();
		
			// get next data
			
		c=strchr(c,'\t');
		if (c==NULL) break;
		c++;
		
		dx+=(int)(element->setup.table.cols[n].percent_size*(float)wid);
	}
}

int element_draw_table_scrollbar(element_type *element,int high,int row_high,int row_count,bool up_ok,bool down_ok)
{
	int				lft,rgt,top,bot,x,top2,bot2,cnt,pos_my,
					scroll_high;
	float			alpha;
	d3col			col,col2;
	
		// scroll bar arrow fills
		
	element_get_box(element,&lft,&rgt,&top,&bot);
	
	lft=rgt-24;
	top+=high;
	bot2=top+24;
	
	col.r=0.5f;
	col.g=0.5f;
	col.b=0.5f;
	
	col2.r=0.3f;
	col2.g=0.3f;
	col2.b=0.3f;

	view_draw_next_vertex_object_2D_color_poly(lft,top,&col,rgt,top,&col,rgt,bot2,&col2,lft,bot2,&col2,1.0f);
	
	top2=bot-24;
	
	view_draw_next_vertex_object_2D_color_poly(lft,top2,&col2,rgt,top2,&col2,rgt,bot,&col,lft,bot,&col,1.0f);
	
		// scroll position
		
	top+=24;
	bot-=24;
		
	if (row_count!=0) {
	
		scroll_high=(bot-top)/row_count;
		cnt=((element->high-high)/row_high)-1;

		if (element->offset!=0) top+=(scroll_high*element->offset);
		if ((element->offset+(cnt+1))<row_count) {
			bot=top+(scroll_high*(cnt+1));
		}
	}
	
	x=(lft+rgt)>>1;
	
	view_draw_next_vertex_object_2D_color_poly(lft,bot,&col,lft,top,&col,x,top,&col2,x,bot,&col2,1.0f);
	view_draw_next_vertex_object_2D_color_poly(x,bot,&col2,x,top,&col2,rgt,top,&col,rgt,bot,&col,1.0f);
	view_draw_next_vertex_object_2D_line_quad(&hud.color.control_outline,1.0f,lft,rgt,top,bot);
	
	pos_my=(top+bot)/2;
	
		// scrolling lines
		
	element_get_box(element,&lft,&rgt,&top,&bot);
	lft=rgt-24;

	view_draw_next_vertex_object_2D_line(&hud.color.control_outline,1.0f,lft,top,lft,bot);
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	lft=rgt-24;
	top+=(high+24);
	
	view_draw_next_vertex_object_2D_line(&hud.color.control_outline,1.0f,lft,top,rgt,top);
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	lft=rgt-24;
	bot-=24;

	view_draw_next_vertex_object_2D_line(&hud.color.control_outline,1.0f,lft,bot,rgt,bot);
	
		// scroll up

	element_get_box(element,&lft,&rgt,&top,&bot);
	lft=rgt-20;
	rgt=lft+16;
	top+=(high+4);
	bot=top+16;

	alpha=up_ok?1.0f:0.1f;
	view_draw_next_vertex_object_2D_color_trig(&hud.color.control_hilite,alpha,lft,rgt,top,bot,TRUE);

	col.r=col.g=col.b=0.0f;
	view_draw_next_vertex_object_2D_line_trig(&col,alpha,lft,rgt,top,bot,TRUE);

		// scroll down
		
	element_get_box(element,&lft,&rgt,&top,&bot);
	lft=rgt-20;
	rgt=lft+16;
	bot-=4;
	top=bot-16;

	alpha=down_ok?1.0f:0.1f;
	view_draw_next_vertex_object_2D_color_trig(&hud.color.control_hilite,alpha,lft,rgt,top,bot,FALSE);

	col.r=col.g=col.b=0.0f;
	view_draw_next_vertex_object_2D_line_trig(&col,alpha,lft,rgt,top,bot,FALSE);
	
	return(pos_my);
}

void element_draw_table(element_type *element,int sel_id)
{
	int				n,x,y,wid,high,cnt,lft,rgt,top,bot,mid,row_high;
	float			alpha;
	char			*c;
	bool			up_ok,down_ok;
	d3col			col,col2;

		// sizes
	
	wid=element->wid-30;
	high=gl_text_get_char_height(hud.font.text_size_small)+2;
	row_high=element_get_table_row_high(element);
	
		// get element counts
		
	cnt=((element->high-(high+4))/row_high)-1;
	up_ok=(element->offset!=0);
	down_ok=((element->offset+(cnt+1))<element_get_table_row_count(element));
	
		// header fill
		
	element_draw_table_header_fill(element,high);
	
		// scrollbar
		
	element_draw_table_scrollbar(element,(high+4),row_high,element_get_table_row_count(element),up_ok,down_ok);
		
		// outline
		
	element_get_box(element,&lft,&rgt,&top,&bot);
	view_draw_next_vertex_object_2D_line_quad(&hud.color.control_outline,1.0f,lft,rgt,top,bot);
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	top+=(high+4);

	view_draw_next_vertex_object_2D_line(&hud.color.control_outline,1.0f,lft,top,rgt,top);

		// text positions
		
	x=element->x;
	y=element->y+3;
	
		// header
		
	element_draw_table_line_header(element,x,y,wid,high);
	
		// items
		
	if (element->data!=NULL) {
	
		y=(element->y+4)+(high+2);
		
		c=element->data+(element->offset*128);
		
		for (n=0;n<=cnt;n++) {
			if (*c==0x0) break;
			
				// selection or background
				
			if ((n+element->offset)==element->value) {
				memmove(&col,&hud.color.control_hilite,sizeof(d3col));
				col2.r=col.r*0.5f;
				col2.g=col.g*0.5f;
				col2.b=col.b*0.5f;
				alpha=0.75f;
			}
			else {
				if (((n+element->offset)&0x1)==0) {
					col.r=col.g=col.b=0.0f;
					col2.r=col2.g=col2.b=0.0f;
				}
				else {
					col.r=col.g=col.b=0.35f;
					col2.r=col2.g=col2.b=0.35f;
				}
				alpha=0.3f;
			}
				
			element_get_box(element,&lft,&rgt,&top,&bot);
			
			lft+=1;
			rgt-=25;
			top=y;
			bot=y+row_high;
			
			view_draw_next_vertex_object_2D_color_poly(lft,top,&col,rgt,top,&col,rgt,bot,&col2,lft,bot,&col2,alpha);
			
			element_draw_table_row_column_lines(element,top,bot,0.5f);
			
				// table line data
				
			element_draw_table_line_data(element,x,y,(element->offset+n),wid,row_high,&hud.color.control_text,c);
			
			c+=128;
			y+=row_high;
		}
	}
	
		// busy
		
	if (element->setup.table.busy_count==-1) return;
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	
	bot-=10;
	top=bot-25;
	lft+=10;
	rgt-=35;
	
	col.r=hud.color.control_hilite.r*0.5f;
	col.g=hud.color.control_hilite.g*0.5f;
	col.b=hud.color.control_hilite.b*0.5f;
	
	if (element->setup.table.busy_count!=0) {
	
		x=rgt;
		
		if (element->setup.table.busy_count<element->setup.table.busy_total_count) {
			x=lft+(((rgt-lft)*element->setup.table.busy_count)/element->setup.table.busy_total_count);
		}

		mid=(top+bot)>>1;

		view_draw_next_vertex_object_2D_color_poly(lft,top,&hud.progress.base_color_start,x,top,&hud.progress.base_color_start,x,mid,&hud.progress.base_color_end,lft,mid,&hud.progress.base_color_end,1.0f);
		view_draw_next_vertex_object_2D_color_poly(lft,mid,&hud.progress.base_color_end,x,mid,&hud.progress.base_color_end,x,bot,&hud.progress.base_color_start,lft,bot,&hud.progress.base_color_start,1.0f);
	}
	
	if (element->setup.table.busy_str[0]!=0x0) {
		col.r=col.g=col.b=0.0f;
		gl_text_start(font_interface_index,hud.font.text_size_small);
		gl_text_draw(((lft+rgt)>>1),((top+bot)>>1),element->setup.table.busy_str,tx_center,TRUE,&col,1.0f);
		gl_text_end();
	}
	
	view_draw_next_vertex_object_2D_line_quad(&hud.color.control_outline,1.0f,lft,rgt,top,bot);
}

/* =======================================================

      Tab Element
      
======================================================= */

int element_mouse_over_tab(element_type *element,int x,int y)
{
	int				margin,high,max_sz,xadd,idx;
	
		// within tab box?
		
	margin=element_get_tab_margin();
	if ((x<margin) || (x>(hud.scale_x-margin))) return(-1);
	
	high=element_get_tab_control_high();
	if ((y<margin) || (y>(margin+high))) return(-1);
	
		// select value
		
	xadd=(hud.scale_x-(margin*2))/element->setup.tab.ntab;
	
	max_sz=(int)(((float)hud.scale_x)*0.2f);
	if (xadd>max_sz) xadd=max_sz;

	idx=(x-margin)/xadd;
	if ((idx<0) || (idx>=element->setup.tab.ntab)) return(-1);

	return(idx);
}

bool element_click_tab(element_type *element,int x,int y)
{
	int				value;
	
	value=element_mouse_over_tab(element,x,y);
	if (value==-1) return(FALSE);
	
	element->value=value;
	
	return(TRUE);
}

void element_draw_tab(element_type *element,int sel_id,int x,int y)
{
	int				n,kx,xadd,x_slant,x_overlap,y_push,
					high,max_sz,mouse_idx,tab_idx,
					lx,rx,ty,by,margin;
	int				tab_draw_list[max_element_tab];
	d3col			col;
	
	high=gl_text_get_char_height(hud.font.text_size_small);
		
		// sizes
	
	margin=element_get_tab_margin();
	
	xadd=(hud.scale_x-(margin*2))/element->setup.tab.ntab;
	
	max_sz=(int)(((float)hud.scale_x)*0.2f);
	if (xadd>max_sz) xadd=max_sz;

		// get mouse over element
	
	mouse_idx=-1;
	
	if (element->id==sel_id) {
		mouse_idx=element_mouse_over_tab(element,x,y);
	}
	
		// pick drawing order (last to first, selected always on top)
		
	tab_idx=0;
	
	for (n=(element->setup.tab.ntab-1);n>=0;n--) {
		if (n!=element->value) tab_draw_list[tab_idx++]=n;
	}
	
	tab_draw_list[tab_idx]=element->value;
	
		// tabs
		
	x_slant=(int)(((float)xadd)*0.025f);
	y_push=(int)(((float)high)*0.3f);
	
	by=margin+high;
	
	for (n=0;n!=element->setup.tab.ntab;n++) {
	
		tab_idx=tab_draw_list[n];

		lx=margin+(xadd*tab_idx);
		rx=lx+xadd;
		
		if (tab_idx!=(element->setup.tab.ntab-1)) {
			x_overlap=x_slant/2;
		}
		else {
			x_overlap=0;
		}
		
		if (element->value!=tab_idx) {
			ty=margin;
			memmove(&col,&hud.color.dialog_dimmed,sizeof(d3col));
		}
		else {
			ty=margin-y_push;
			memmove(&col,&hud.color.dialog_base,sizeof(d3col));
		}
		
		view_draw_next_vertex_object_2D_color_poly((lx+x_slant),ty,&col,((rx-x_slant)+x_overlap),ty,&col,(rx+x_overlap),by,&col,lx,by,&col,1.0f);

		view_draw_next_vertex_object_2D_line(&hud.color.dialog_outline,1.0f,lx,by,(lx+x_slant),ty);
		view_draw_next_vertex_object_2D_line(&hud.color.dialog_outline,1.0f,(lx+x_slant),ty,((rx-x_slant)+x_overlap),ty);
		view_draw_next_vertex_object_2D_line(&hud.color.dialog_outline,1.0f,((rx-x_slant)+x_overlap),ty,(rx+x_overlap),by);

		memmove(&col,&hud.color.control_label,sizeof(d3col));
		
		if (element->value==tab_idx) {
			memmove(&col,&hud.color.control_hilite,sizeof(d3col));
		}
		
		if (mouse_idx==tab_idx) {
			memmove(&col,&hud.color.control_mouse_over,sizeof(d3col));
		}

		gl_text_start(font_interface_index,hud.font.text_size_small);
		gl_text_draw(((lx+rx)>>1),((ty+by)>>1),element->setup.tab.name[tab_idx],tx_center,TRUE,&col,1.0f);
		gl_text_end();
	}

		// tab dialog
		
	lx=margin;
	rx=hud.scale_x-margin;
	ty=high+margin;
	by=hud.scale_y-margin;
	
	view_draw_next_vertex_object_2D_color_quad(&hud.color.dialog_base,1.0f,lx,rx,by,ty);
	
		// outside line
		
	view_draw_next_vertex_object_2D_line(&hud.color.dialog_outline,1.0f,lx,ty,lx,by);
	view_draw_next_vertex_object_2D_line(&hud.color.dialog_outline,1.0f,lx,by,rx,by);
	view_draw_next_vertex_object_2D_line(&hud.color.dialog_outline,1.0f,rx,by,rx,ty);
	
		// top line
		
	kx=margin+(element->value*xadd);
	if (element->value!=0) {
		view_draw_next_vertex_object_2D_line(&hud.color.dialog_outline,1.0f,lx,ty,kx,ty);
	}
	if (element->value!=(element->setup.tab.ntab-1)) {
		view_draw_next_vertex_object_2D_line(&hud.color.dialog_outline,1.0f,(kx+xadd+x_overlap),ty,rx,ty);
	}

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

/* =======================================================

      Color Element
      
======================================================= */

void element_click_color(element_type *element,int x)
{
	int				idx;

	idx=(x-element->x)/(element->wid/max_tint_color);
	if (idx<0) idx=0;
	if (idx>=max_tint_color) idx=max_tint_color-1;

	element->value=idx;
}

void element_draw_color(element_type *element,int sel_id)
{
	int				n,x,y,ky,lft,rgt,top,bot,lx,rx,xadd,s_lx,s_rx;
	float			alpha;
	d3col			col;
	
	x=element->x;
	y=element->y;
	
		// label
	
	ky=y-(element->high>>1);

	gl_text_start(font_interface_index,hud.font.text_size_small);
	gl_text_draw((x-5),ky,element->str,tx_right,TRUE,&hud.color.control_label,1.0f);
	gl_text_draw(x,(ky-1),":",tx_center,TRUE,&hud.color.control_label,1.0f);
	gl_text_end();

		// color size
		
	lft=x+10;
	rgt=lft+element->wid;
	top=ky-(element->high>>1);
	bot=top+element->high;

	alpha=(element->enabled?1.0f:0.3f);

		// colors

	lx=lft;
	xadd=(rgt-lft)/max_tint_color;

	for (n=0;n!=max_tint_color;n++) {
		if (n==(max_tint_color-1)) {
			rx=rgt;
		}
		else {
			rx=lx+xadd;
		}

		if (n==element->value) {
			s_lx=lx;
			s_rx=rx;
		}

		col.r=hud.color.tints[n].r*0.5f;
		col.g=hud.color.tints[n].g*0.5f;
		col.b=hud.color.tints[n].b*0.5f;

		view_draw_next_vertex_object_2D_color_poly(lx,top,&hud.color.tints[n],rx,top,&hud.color.tints[n],rx,bot,&col,lx,bot,&col,alpha);

		lx=rx;
	}

		// outline and selection

	if ((element->id==sel_id) && (element->enabled)) {
		view_draw_next_vertex_object_2D_line_quad(&hud.color.control_mouse_over,alpha,lft,rgt,top,bot);
	}
	else {
		view_draw_next_vertex_object_2D_line_quad(&hud.color.control_outline,alpha,lft,rgt,top,bot);
	}

	col.r=col.g=col.b=0.0f;
	view_draw_next_vertex_object_2D_line_quad(&col,1.0f,(s_lx+1),(s_rx-1),(top+1),(bot-1));

	col.r=col.g=col.b=1.0f;
	view_draw_next_vertex_object_2D_line_quad(&col,0.5f,(s_lx+2),(s_rx-2),(top+2),(bot-2));
}

/* =======================================================

      Text Box Element
      
======================================================= */

void element_click_text_box(element_type *element,int x,int y)
{
	int				high,page_count;
	
		// any scrollbars?
		
	if (element->setup.text_box.pos_y==-1) return;
	
		// check for clicking in scroll bar

	if (x<((element->x+element->wid)-24)) return;
	
		// get page sizes
		
	high=gl_text_get_char_height(hud.font.text_size_small);
	page_count=(element->high-5)/high;
	
		// is up and down OK?

	if ((y<=element->setup.text_box.pos_y) && (element->setup.text_box.scroll_up_ok)) {
		element->offset-=page_count;
		if (element->offset<0) element->offset=0;
	}

	if ((y>=element->setup.text_box.pos_y) && (element->setup.text_box.scroll_down_ok)) {
		element->offset+=page_count;
	}
}

void element_draw_text_box(element_type *element)
{
	int				lft,rgt,top,bot,x,y,wid,high,idx,last_space_idx,
					line_count,line_offset;
	char			*c,str[256];
	bool			line_break;
	d3col			col;
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	
	high=gl_text_get_char_height(hud.font.text_size_small);
	
		// background

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
		
	col.r=col.g=col.b=0.0f;
	view_draw_next_vertex_object_2D_color_quad(&col,0.2f,lft,rgt,top,bot);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	
		// scrolling flags
		
	element->setup.text_box.scroll_up_ok=(element->offset!=0);
	element->setup.text_box.scroll_down_ok=FALSE;
	
		// any text?
		
	if (element->data==NULL) return;

		// draw text
		
	x=lft+5;
	y=element->y+(high+5);

	gl_text_start(font_interface_index,hud.font.text_size_small);

	idx=0;
	last_space_idx=-1;
	line_count=0;
	
	line_offset=element->offset;
	
	c=element->data;
	
	while (TRUE) {
	
			// have we reached the end?
			
		if (*c==0x0) {
			if ((y<bot) && (line_offset==0)) {
				str[idx]=0x0;
				gl_text_draw(lft,y,str,tx_left,FALSE,&hud.color.control_text,1.0f);
			}
			line_count++;
			break;
		}
		
			// check to see if there's a line break
	
		line_break=FALSE;
	
		if (*c=='\n') {
			line_break=TRUE;
		}
		else {
			if (*c<0x20) {		// skip all control characters
				c++;
				continue;
			}
		}
		
			// time for a wrap?
			
		str[idx]=0x0;
		
		wid=gl_text_get_string_width(font_interface_index,hud.font.text_size_small,str);
		
		if (wid>=(element->wid-30)) {
			line_break=TRUE;
			
			if (last_space_idx!=-1) {
				str[last_space_idx]=0x0;		// backup past the wrapping text
				c-=(idx-last_space_idx);
				idx=last_space_idx;
			}
		}
		
			// special break if too many characters
			
		if (idx==255) line_break=TRUE;
		
			// draw or add up line
		
		if (line_break) {
		
			if ((y<bot) && (line_offset==0)) gl_text_draw(x,y,str,tx_left,FALSE,&hud.color.control_text,1.0f);
		
			idx=0;
			last_space_idx=-1;
			
			line_count++;
			
			if (line_offset>0) {
				line_offset--;
			}
			else {
				y+=high;
				if (y>=bot) {
					element->setup.text_box.scroll_down_ok=TRUE;
				}
			}
		}
		else {
			str[idx]=*c;
			
			if (*c==0x20) last_space_idx=idx;
			
			idx++;
		}
		
		c++;
	}

	gl_text_end();
	
	element->setup.text_box.line_count=line_count;
	
		// scrollbar
		
	element->setup.text_box.pos_y=-1;
	
	if ((element->setup.text_box.scroll_up_ok) || (element->setup.text_box.scroll_down_ok)) {
		element->setup.text_box.pos_y=element_draw_table_scrollbar(element,0,high,line_count,element->setup.text_box.scroll_up_ok,element->setup.text_box.scroll_down_ok);
	}
	
		// outline

	view_draw_next_vertex_object_2D_line_quad(&hud.color.control_outline,1.0f,lft,rgt,top,bot);
}

/* =======================================================

      Info Field Elements
      
======================================================= */

void element_draw_info_field(element_type *element)
{
	int				x,y,ky;
	
	x=element->x;
	y=element->y;
	
	ky=y-(element->high>>1);
		
	gl_text_start(font_interface_index,hud.font.text_size_small);
	gl_text_draw((x-5),ky,element->str,tx_right,TRUE,&hud.color.control_label,1.0f);
	gl_text_draw(x,(ky-1),":",tx_center,TRUE,&hud.color.control_label,1.0f);
	gl_text_draw((x+5),ky,element->value_str,tx_left,TRUE,&hud.color.control_label,1.0f);
	gl_text_end();
}

/* =======================================================

      Draw Elements
      
======================================================= */

void element_draw_lock(bool cursor_hilite)
{
	int					n,id,x,y;
	element_type		*element;
	
		// get element under cursor
		// only works if app active

	id=-1;

	if ((cursor_hilite) && (game_app_active)) {

			// find hilited item

		if (element_open_text_field_id!=-1) {
			id=element_open_text_field_id;
		}
		else {
			if (element_open_combo_id!=-1) {
				id=element_open_combo_id;
			}
			else {
				input_gui_get_mouse_position(&x,&y);
				id=element_find_for_xy(x,y);
			}
		}

			// if clicking and holding, then we
			// can only hilite clicked item

		if (element_click_down_id!=-1) {
			if (id!=element_click_down_id) id=-1;
		}
	}

		// setup draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	glDisable(GL_LINE_SMOOTH);

	glLineWidth(1.0f);
	
		// draw elements
				
	element=elements;
	
	for (n=0;n<nelement;n++) {

		if (element->hidden) {
			element++;
			continue;
		}
		
			// draw element
	
		switch (element->type) {
		
			case element_type_button:
				element_draw_button(element,id);
				break;
			case element_type_bitmap:
				element_draw_bitmap(element);
				break;
			case element_type_text:
				element_draw_text(element,id);
				break;
			case element_type_text_field:
				element_draw_text_field(element,id);
				break;
			case element_type_checkbox:
				element_draw_checkbox(element,id);
				break;
			case element_type_combo:
				element_draw_combo(element,id);
				break;
			case element_type_slider:
				element_draw_slider(element,id);
				break;
			case element_type_table:
				element_draw_table(element,id);
				break;
			case element_type_tab:
				element_draw_tab(element,id,x,y);
				break;
			case element_type_color:
				element_draw_color(element,id);
				break;
			case element_type_text_box:
				element_draw_text_box(element);
				break;
			case element_type_info_field:
				element_draw_info_field(element);
				break;
				
		}
		
		element++;
	}
	
		// draw open combo
		
	if (element_open_combo_id!=-1) {
		element_draw_combo_open(element_find(element_open_combo_id));
	}

		// reset line smoothing

	glEnable(GL_LINE_SMOOTH);
}

void element_draw(bool cursor_hilite)
{
	SDL_mutexP(element_thread_lock);
	element_draw_lock(cursor_hilite);
	SDL_mutexV(element_thread_lock);
}

/* =======================================================

      Click Elements
      
======================================================= */

void element_click_down_lock(int x,int y)
{
	element_type		*element;

		// already in a down click, if so,
		// check if we are in a slider

	if (element_click_down_id!=-1) {
		element=element_find(element_click_down_id);
		if (element->type==element_type_slider) {
			element_move_slider(element,x,y);
		}
		return;
	}

		// in open combo?

	if (element_open_combo_id!=-1) {
	
		element=element_find(element_open_combo_id);
		if (element_in_combo_open(element,x,y)) {
			element_click_down_id=element->id;
			return;
		}

		element_open_combo_id=-1;
	}
	
		// get click

	element_click_down_id=element_find_for_xy(x,y);

		// if open text, outside down clicks close

	if (element_open_text_field_id!=-1) {
		if (element_click_down_id!=element_open_text_field_id) element_open_text_field_id=-1;
	}
}

int element_click_up_lock(int x,int y)
{
	int					id;
	element_type		*element;
	
		// no click down ID
		
	if (element_click_down_id==-1) return(-1);
	
		// in open combo?
		
	if (element_open_combo_id!=-1) {

		element=element_find(element_open_combo_id);
		if (element_click_combo_open(element,x,y)) {
			element_click_down_id=-1;
			return(element->id);
		}

		element_open_combo_id=-1;
	}

		// find element

	id=element_find_for_xy(x,y);
	if (id!=element_click_down_id) {
		element_click_down_id=-1;
		return(-1);
	}
	
	element=element_find(id);

		// ignore disabled elements

	if (!element->enabled) {
		element_click_down_id=-1;
		return(-1);
	}
	
		// click it
		
	switch (element->type) {
		
		case element_type_text_field:
			element_click_text_field(element);
			break;
		case element_type_checkbox:
			element_click_checkbox(element);
			break;
		case element_type_combo:
			element_click_combo(element);
			break;
		case element_type_slider:
			element_click_slider(element,x,y);
			break;
		case element_type_table:
			element_click_table(element,x,y);
			break;
		case element_type_tab:
			if (!element_click_tab(element,x,y)) {
				element_click_down_id=-1;
				return(-1);
			}
			break;
		case element_type_color:
			element_click_color(element,x);
			break;
		case element_type_text_box:
			element_click_text_box(element,x,y);
			break;
			
	}
	
		// turn off down click
		
	element_click_down_id=-1;
	
	return(id);
}

void element_click_down(int x,int y)
{
	SDL_mutexP(element_thread_lock);
	element_click_down_lock(x,y);
	SDL_mutexV(element_thread_lock);
}

int element_click_up(int x,int y)
{
	int			id;

	SDL_mutexP(element_thread_lock);
	id=element_click_up_lock(x,y);
	SDL_mutexV(element_thread_lock);

	return(id);
}

/* =======================================================

      Key Elements
      
======================================================= */

int element_key_lock(char ch)
{
	int					len;
	element_type		*element;

		// text field open?

	if (element_open_text_field_id==-1) return(-1);

	element=element_find(element_open_text_field_id);

		// returns or tabs close box

	if ((ch==0xD) || (ch==0x9)) {
		element_open_text_field_id=-1;
		return(element->id);
	}

		// get text field

	len=strlen(element->value_str);

		// backspace?

	if (ch==0x8) {
		if (len==0) return(-1);
		element->value_str[len-1]=0x0;
		return(element->id);
	}

		// regular text

	if (len>=(element->max_value_str_sz-1)) return(-1);

	element->value_str[len]=ch;
	element->value_str[len+1]=0x0;

	return(element->id);
}

int element_key(char ch)
{
	int			rtn;

	SDL_mutexP(element_thread_lock);
	rtn=element_key_lock(ch);
	SDL_mutexV(element_thread_lock);

	return(rtn);
}

/* =======================================================

      Find Hilite
      
======================================================= */

int element_get_selected(void)
{
	int			x,y;

	if (element_open_text_field_id!=-1) return(element_open_text_field_id);
	if (element_open_combo_id!=-1) return(element_open_combo_id);

	input_gui_get_mouse_position(&x,&y);
				
	return(element_find_for_xy(x,y));
}

/* =======================================================

      Values
      
======================================================= */

int element_get_value(int id)
{
	int				rtn;
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element==NULL) {
		rtn=-1;
	}
	else {
		rtn=element->value;
	}

	SDL_mutexV(element_thread_lock);

	return(rtn);
}

float element_get_slider_value(int id)
{
	float			rtn;
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element==NULL) {
		rtn=0.0f;
	}
	else {
		rtn=element->setup.slider.min+((element->setup.slider.max-element->setup.slider.min)*element->setup.slider.value);
	}

	SDL_mutexV(element_thread_lock);

	return(rtn);
}

void element_get_value_string(int id,char *str)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	str[0]=0x0;

	element=element_find(id);
	if (element!=NULL) strcpy(str,element->value_str);

	SDL_mutexV(element_thread_lock);
}

void element_set_value(int id,int value)
{
	element_type	*element;
	
	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) element->value=value;

	SDL_mutexV(element_thread_lock);
}

void element_set_slider_value(int id,float value)
{
	element_type	*element;
	
	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) element->setup.slider.value=(value-element->setup.slider.min)/(element->setup.slider.max-element->setup.slider.min);

	SDL_mutexV(element_thread_lock);
}

void element_set_value_string(int id,char *str)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) strcpy(element->value_str,str);

	SDL_mutexV(element_thread_lock);
}

/* =======================================================

      Table Data
      
======================================================= */

void element_set_table_data(int id,char *row_data)
{
	int				idx,sz;
	element_type	*element;
	
	idx=element_get_value(id);

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) {
			
		if (element->data!=NULL) free(element->data);

		if (row_data==NULL) {
			element->data=NULL;
		}
		else {
			sz=element_table_add_get_data_size(row_data);
			element->data=malloc(sz);
			memmove(element->data,row_data,sz);
		}
	}
	
	SDL_mutexV(element_thread_lock);
	
	element_set_value(id,idx);
}

void element_set_bitmap(int id,char *path)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) {
		view_images_free_single(element->setup.button.image_idx);
		element->setup.button.image_idx=view_images_load_single(path,FALSE,TRUE);
	}

	SDL_mutexV(element_thread_lock);
}

void element_set_table_checkbox(int id,int idx,bool on)
{
	element_type	*element;
	
	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) element->setup.table.checks[idx]=(on?0x1:0x0);
	
	SDL_mutexV(element_thread_lock);
}

bool element_get_table_checkbox(int id,int idx)
{
	bool			on;
	element_type	*element;
	
	on=FALSE;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) on=(element->setup.table.checks[idx]!=0x0);
	
	SDL_mutexV(element_thread_lock);

	return(on);
}

/* =======================================================

      Position/Scrolling
      
======================================================= */

int element_get_scroll_position(int id)
{
	int				rtn;
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element==NULL) {
		rtn=0;
	}
	else {
		rtn=element->offset;
	}

	SDL_mutexV(element_thread_lock);

	return(rtn);
}

void element_set_scroll_position(int id,int pos)
{
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) element->offset=pos;

	SDL_mutexV(element_thread_lock);
}

void element_make_selection_visible(int id)
{
	int				high,row_high,cnt;
	element_type	*element;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) {
	
		if (element->value!=-1) {
	
			high=gl_text_get_char_height(hud.font.text_size_small)+2;
			row_high=element_get_table_row_high(element);
			cnt=((element->high-(high+4))/row_high)-1;
			
			element->offset=0;
			while (element->value>=(element->offset+(cnt+1))) {
				element->offset+=(cnt+1);
			}
		}
	}
	
	SDL_mutexV(element_thread_lock);
}

int element_get_x_position(int id)
{
	int				x;
	element_type	*element;

	x=0;

	SDL_mutexP(element_thread_lock);

	element=element_find(id);
	if (element!=NULL) x=element->x;

	SDL_mutexV(element_thread_lock);

	return(x);
}
