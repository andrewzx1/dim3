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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// list palette
//

#define list_max_item_count									4096
#define list_max_section_count								32

#define list_max_value_sz									64

#define list_title_font_size								14.0f
#define list_item_font_size									12.0f
#define list_item_font_high									14

#define list_wheel_scroll_size								14

#ifndef D3_SETUP
	#define list_palette_border_sz							10
	#define list_palette_tree_sz							260
	#define list_value_clip_size							25
#else
	#define list_palette_border_sz							0
	#define list_palette_tree_sz							400
	#define list_value_clip_size							35
#endif

#define list_palette_scroll_wid								15
#define list_title_line_high								15

#define list_item_ctrl_header								0
#define list_item_ctrl_text									1
#define list_item_ctrl_color								2
#define list_item_ctrl_string								3
#define list_item_ctrl_param								4
#define list_item_ctrl_na_blank								5
#define list_item_ctrl_int									6
#define list_item_ctrl_float								7
#define list_item_ctrl_checkbox								8
#define list_item_ctrl_point								9
#define list_item_ctrl_angle								10
#define list_item_ctrl_vector								11
#define list_item_ctrl_normal_vector						12
#define list_item_ctrl_uv									13
#define list_item_ctrl_picker								14
#define list_item_ctrl_pick_color							15

#define list_string_value_string							0
#define list_string_value_int								1
#define list_string_value_positive_int						2
#define list_string_value_range_int							3
#define list_string_value_float								4
#define list_string_value_positive_float					5
#define list_string_value_0_to_1_float						6

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
						int									*int_ptr;
						float								*float_ptr;
						bool								*bool_ptr;
						char								*str_ptr;
						d3pnt								*pnt_ptr;
						d3ang								*ang_ptr;
						d3vct								*vct_ptr;
						d3uv								*uv_ptr;
						d3col								*col_ptr;
					} list_palette_item_value_type;

typedef struct		{
						int									param_idx,str_len;
					} list_palette_item_limit_type;

typedef struct		{
						char								search_path[64],extension[8],required_file_name[file_str_len];
						bool								file_list;
					} list_palette_item_file_type;

typedef struct		{
						int									count,item_sz,name_offset;
						bool								is_index,include_none;
						char								*ptr;
						list_palette_item_file_type			file;
					} list_palette_item_list_type;

typedef struct		{
						int									x,y,ctrl_type,button_type,
															id,idx,button_id,count;
						bool								selected,disabled;
						char								name[name_str_len];
						list_palette_item_value_type		value;
						list_palette_item_limit_type		limit;
						list_palette_item_list_type			list;
					} list_palette_item_type;

typedef struct		{
						int									*picker_idx_ptr;
						bool								on,file_list,catch_last_click;
						char								*picker_name_ptr;
					} list_palette_picker_type;

typedef struct		{
						int									id,idx,item_idx;
						list_palette_item_type				*item;
					} list_palette_click_type;

typedef struct		{
						int									level,item_count,
															item_sort_start_idx,push_idx,
															scroll_offset,total_high;
						bool								back_push_on,
															push_on,button_click;
						char								titles[3][128];
						list_palette_click_type				click;
						list_palette_item_type				*items;
					} list_palette_pane_type;

typedef struct		{
						bool								left,never_hide_picker,never_open;
					} list_palette_flag_type;

typedef struct		{
						bool								open;
						list_palette_flag_type				flag;
						list_palette_pane_type				item_pane,picker_pane;
						list_palette_picker_type			picker;
					} list_palette_type;

//
// property utilities
//

extern void property_string_get_values(int value_type,void *value,char *str);
extern void property_string_get_title(int value_type,int i_min,int i_max,char *title);
extern void property_string_set_values(int value_type,void *value,int value_len,int i_min,int i_max,char *str);

extern bool property_chord_get_values(int value_type,void *value,char *str_x,char *str_y,char *str_z);
extern void property_chord_get_title(int value_type,char *title);
extern void property_chord_set_values(int value_type,void *value,char *str_x,char *str_y,char *str_z);

extern int property_pick_get_list_count(char *list_ptr,int list_item_sz,int list_name_offset);

extern int property_pick_file_fill_list(char *search_path,char *extension,char *required_file_name);
extern char* property_pick_file_get_list(void);
extern void property_pick_file(char *title,char *search_path,char *extension,char *required_file_name,char *file_name);

extern void property_get_parameter(int idx,char *param_list,char *str);
extern void property_set_parameter(int idx,char *param_list,char *str);

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

extern void list_palette_initialize(void);
extern void list_palette_shutdown(void);
extern void list_palette_list_initialize(list_palette_type *list,char *title,bool left,bool never_hide_picker,bool never_open);
extern void list_palette_list_shutdown(list_palette_type *list);
extern int list_palette_width(list_palette_type *list);
extern void list_palette_total_box(list_palette_type *list,d3rect *box);
extern void list_palette_set_title(list_palette_type *list,char *title_0,char *name_0,char *title_1,char *name_1,char *title_2,char *name_2);
extern int list_palette_get_level(list_palette_type *list);
extern void list_palette_set_level(list_palette_type *list,int level);
extern bool list_palette_is_open(list_palette_type *list);
extern void list_palette_add_header(list_palette_type *list,int id,char *name);
extern void list_palette_add_header_count(list_palette_type *list,int id,char *name,int count);
extern void list_palette_add_header_button(list_palette_type *list,int id,char *name,int button_type);
extern void list_palette_add_item(list_palette_type *list,int id,int idx,char *name,bool selected,bool disabled);
extern void list_palette_add_color(list_palette_type *list,int id,int idx,d3col *col_ptr,bool selected,bool disabled);
extern void list_palette_add_string_selectable(list_palette_type *list,int id,char *name,char *str_ptr,int str_len,bool selected,bool disabled);
extern void list_palette_add_string_selectable_button(list_palette_type *list,int id,int button_type,int button_id,char *name,bool selected,bool disabled);
extern void list_palette_add_string(list_palette_type *list,int id,char *name,char *str_ptr,int str_len,bool disabled);
extern void list_palette_add_parameter(list_palette_type *list,int id,char *name,char *params,int param_idx,bool disabled);
extern void list_palette_add_na_blank(list_palette_type *list,int id,char *name);
extern void list_palette_add_int(list_palette_type *list,int id,char *name,int *int_ptr,bool disabled);
extern void list_palette_add_float(list_palette_type *list,int id,char *name,float *float_ptr,bool disabled);
extern void list_palette_add_checkbox(list_palette_type *list,int id,char *name,bool *bool_ptr,bool disabled);
extern void list_palette_add_pick_color(list_palette_type *list,int id,char *name,d3col *col_ptr,bool disabled);
extern void list_palette_add_point(list_palette_type *list,int id,char *name,d3pnt *pnt_ptr,bool disabled);
extern void list_palette_add_angle(list_palette_type *list,int id,char *name,d3ang *ang_ptr,bool disabled);
extern void list_palette_add_vector(list_palette_type *list,int id,char *name,d3vct *vct_ptr,bool disabled);
extern void list_palette_add_normal_vector(list_palette_type *list,int id,char *name,d3vct *vct_ptr,bool disabled);
extern void list_palette_add_uv(list_palette_type *list,int id,char *name,d3uv *uv_ptr,bool disabled);
extern void list_palette_add_picker_list_int(list_palette_type *list,int id,char *name,char *list_ptr,int list_count,int list_item_sz,int list_name_offset,bool include_none,int *int_ptr,bool disabled);
extern void list_palette_add_picker_list_string(list_palette_type *list,int id,char *name,char *list_ptr,int list_count,int list_item_sz,int list_name_offset,bool include_none,char *str_ptr,bool disabled);
extern void list_palette_add_picker_file(list_palette_type *list,int id,int button_type,int button_id,char *name,char *search_path,char *extension,char *required_file_name,char *str_ptr,bool disabled);
extern void list_palette_delete_all_items(list_palette_type *list);
extern void list_palette_start_picking_mode(list_palette_type *list,char *title,char *list_ptr,int list_count,int list_item_sz,int list_name_offset,bool include_none,bool file_list,int *idx_ptr,char *name_ptr);
extern void list_palette_sort_mark_start(list_palette_type *list);
extern void list_palette_sort(list_palette_type *list);
extern void list_palette_draw(list_palette_type *list);
extern void list_palette_scroll_wheel(list_palette_type *list,d3pnt *pnt,int move);
extern void list_palette_scroll_item_into_view(list_palette_type *list,int item_id,int item_idx);
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

extern void tool_palette_initialize(tool_palette_type *tool_palette,int count,tool_palette_tool_type *tools);
extern void tool_palette_shutdown(tool_palette_type *tool_palette);
extern void tool_palette_box(d3rect *box);
extern void tool_palette_set_state(tool_palette_type *tool_palette,int idx,bool selected,bool disabled);
extern void tool_palette_draw(tool_palette_type *tool_palette);
extern int tool_palette_click(tool_palette_type *tool_palette,d3pnt *pnt);
extern void tool_palette_mouse_move(tool_palette_type *tool_palette,d3pnt *pnt);

//
// text palette
//

extern void texture_palette_draw(void);
extern void texture_palette_click(d3pnt *pnt,bool dbl_click);

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


