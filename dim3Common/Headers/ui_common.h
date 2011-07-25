/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Common UI Definitions

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// list palette
//

#define list_max_item_count									4096
#define list_max_section_count								32

#define list_title_font_size								14.0f
#define list_item_font_size									12.0f
#define list_item_font_high									14
#define list_item_scroll_size								(list_item_font_high*5)

#define list_palette_border_sz								10
#define list_palette_tree_sz								250
#define list_palette_scroll_wid								15
#define list_title_high										20

#define list_item_ctrl_header								0
#define list_item_ctrl_text									1
#define list_item_ctrl_color								2
#define list_item_ctrl_string								3
#define list_item_ctrl_checkbox								4
#define list_item_ctrl_pick_color							5

#define list_string_value_string							0
#define list_string_value_int								1
#define list_string_value_positive_int						2
#define list_string_value_range_int							3
#define list_string_value_float								4
#define list_string_value_positive_float					5
#define list_string_value_0_to_1_float						6
#define list_string_value_tag								7

#define list_chord_value_point								0
#define list_chord_value_angle								1
#define list_chord_value_vector								2
#define list_chord_value_uv									3

#define list_button_none									-1
#define list_button_edit									0
#define list_button_plus									1
#define list_button_minus									2
#define list_button_set										3

typedef union		{
						bool								checkbox;
						char								str[32];
						d3col								col;
					} list_palette_item_value_type;

typedef struct		{
						int									x,y,ctrl_type,button_type,
															id,button_id,type,count,idx;
						bool								selected,disabled;
						char								name[name_str_len];
						list_palette_item_value_type		value;
					} list_palette_item_type;

typedef struct		{
						int									item_count,item_sort_start_idx,push_idx,
															item_id,item_type,item_idx,
															pixel_sz,scroll_page,
															total_high;
						bool								push_on,back_on,button_click;
						char								title[name_str_len];
						list_palette_item_type				*items;
					} list_palette_type;

//
// property utilities
//

extern void property_string_get_values(int value_type,void *value,int i_min,int i_max,char *str,char *desc);
extern void property_string_set_values(int value_type,void *value,int value_len,int i_min,int i_max,char *str);

extern bool property_chord_get_values(int value_type,void *value,char *str_x,char *str_y,char *str_z,char *desc);
extern void property_chord_set_values(int value_type,void *value,char *str_x,char *str_y,char *str_z);

extern void property_pick_list(char *title,char *list,int *idx);

extern void property_get_parameter(int idx,char *param_list,char *str);
extern void property_set_parameter(int idx,char *param_list,char *str);

extern void dialog_property_list_run(char *title,char *list_ptr,int list_count,int list_item_sz,int list_name_offset,bool include_none,int *idx);
extern void dialog_property_string_run(int value_type,void *value,int value_len,int i_min,int i_max);
extern void dialog_property_chord_run(int value_type,void *value);

//
// text
//

extern void text_initialize(void);
extern void text_shutdown(void);
extern int text_width(float txt_size,char *str);
extern void text_draw(int x,int y,float txt_size,d3col *col,char *str);
extern void text_draw_center(int x,int y,float txt_size,d3col *col,char *str);
extern void text_draw_right(int x,int y,float txt_size,d3col *col,char *str);

//
// progress
//

extern void progress_start(char *title,int count);
extern void progress_end(void);
extern void progress_next(void);
extern void progress_next_title(char *title);

//
// list palette
//

extern void list_palette_initialize(char *app_name);
extern void list_palette_shutdown(void);
extern void list_palette_list_initialize(list_palette_type *list,char *title,bool back_on);
extern void list_palette_list_shutdown(list_palette_type *list);
extern void list_palette_box(d3rect *box);
extern void list_palette_set_title(list_palette_type *list,char *title);
extern int list_palette_get_level(void);
extern void list_palette_set_level(int level);
extern void list_palette_add_header(list_palette_type *list,int piece_type,char *name);
extern void list_palette_add_header_count(list_palette_type *list,int piece_type,char *name,int count);
extern void list_palette_add_header_button(list_palette_type *list,int id,char *name,int button_type);
extern void list_palette_add_item(list_palette_type *list,int piece_type,int piece_idx,char *name,bool selected,bool disabled);
extern void list_palette_add_color(list_palette_type *list,int piece_type,int piece_idx,d3col *col,bool selected,bool disabled);
extern void list_palette_add_string_selectable(list_palette_type *list,int id,char *name,char *value,bool selected,bool disabled);
extern void list_palette_add_string_selectable_button(list_palette_type *list,int id,int button_type,int button_id,char *name,char *value,bool selected,bool disabled);
extern void list_palette_add_string(list_palette_type *list,int id,char *name,char *value,bool disabled);
extern void list_palette_add_string_int(list_palette_type *list,int id,char *name,int value,bool disabled);
extern void list_palette_add_string_float(list_palette_type *list,int id,char *name,float value,bool disabled);
extern void list_palette_add_checkbox(list_palette_type *list,int id,char *name,bool value,bool disabled);
extern void list_palette_add_pick_color(list_palette_type *list,int id,char *name,d3col *col,bool disabled);
extern void list_palette_add_point(list_palette_type *list,int id,char *name,d3pnt *pnt,bool disabled);
extern void list_palette_add_angle(list_palette_type *list,int id,char *name,d3ang *ang,bool disabled);
extern void list_palette_add_vector(list_palette_type *list,int id,char *name,d3vct *vct,bool disabled);
extern void list_palette_add_uv(list_palette_type *list,int id,char *name,d3fpnt *fpnt,bool disabled);
extern void list_palette_add_texture(list_palette_type *list,texture_type *textures,int id,char *name,int txt_idx,bool disabled);
extern void list_palette_add_shader(list_palette_type *list,int id,char *name,char *shader_name,bool disabled);
extern void list_palette_add_string_tag(list_palette_type *list,int id,char *name,unsigned long tag,bool disabled);
extern void list_palette_delete_all_items(list_palette_type *list);
extern void list_palette_sort_mark_start(list_palette_type *list);
extern void list_palette_sort(list_palette_type *list);
extern void list_palette_draw(list_palette_type *list);
extern void list_palette_scroll_up(list_palette_type *list);
extern void list_palette_scroll_down(list_palette_type *list);
extern void list_palette_scroll_wheel(list_palette_type *list,d3pnt *pnt,int move);
extern void list_palette_scroll_item_into_view(list_palette_type *list,int item_type,int item_idx);
extern bool list_palette_click(list_palette_type *list,d3pnt *pnt,bool double_click);

//
// tool tips
//

#define tool_tip_font_size						15.0f

extern void tool_tip_initialize(void);
extern void tool_tip_setup(d3pnt *pnt,char *str,bool right);
extern void tool_tip_clear(void);
extern void tool_tip_draw(void);

//
// tool palette
//

#define tool_palette_seperator_size				4

extern void tool_palette_initialize(char *app_name);
extern void tool_palette_shutdown(void);
extern void tool_palette_draw(void);
extern void tool_palette_click(d3pnt *pnt);
extern void tool_palette_mouse_move(d3pnt *pnt);

//
// text palette
//

extern void texture_palette_draw(texture_type *txt_list);
extern void texture_palette_click(texture_type *txt_list,d3pnt *pnt,bool dbl_click);

//
// texture editing
//

#define texture_edit_item_high						140
#define texture_edit_scroll_wheel_move				25

#define texture_edit_frame_click_return_idx			100
#define texture_edit_frame_click_delete_idx			101

extern void texture_edit_switch(int texture_idx);
extern void texture_edit_draw(void);
extern bool texture_edit_cursor(void);
extern void texture_edit_scroll_wheel(int delta);
extern bool texture_edit_click(d3pnt *pnt,bool double_click);

//
// import text decoder
//

extern int textdecode_count_linestarts(void);
extern void textdecode_linestarts(void);
extern bool textdecode_open(char *path,char p_piece_break);
extern void textdecode_close(void);
extern int textdecode_count(void);
extern int textdecode_find(int str_line,char *txt);
extern void textdecode_get_line(int i,char *txt);
extern void textdecode_get_piece(int i,int k,char *txt);


