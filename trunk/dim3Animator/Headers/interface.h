/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Interface Functions

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
// main window
//

extern void main_wind_draw(void);
extern void main_wind_draw_no_swap(void);

extern void model_wind_open(void);
extern void model_wind_close(void);
extern void model_wind_reset_tools(void);
extern void model_wind_switch_mesh_mode(void);
extern void model_wind_reset_modifiers(void);
extern void model_wind_play(bool play,bool blend);

extern void click_model_wind(d3pnt *pnt,unsigned long modifiers);

//
// menus
//

extern void menu_fix_enable(void);
extern bool menu_save_changes_dialog(void);
extern bool menu_event_run(int cmd);

extern void open_model_xml(void);

//
// undo
//

extern void undo_clear(void);
extern void undo_set_bone_move(int pose_idx,int bone_idx);
extern void undo_run(void);

//
// text
//

extern void text_initialize(void);
extern void text_shutdown(void);
extern void text_draw(int x,int y,float txt_size,char *str);
extern void text_draw_center(int x,int y,float txt_size,char *str);
extern void text_draw_right(int x,int y,float txt_size,char *str);

//
// progress
//

extern void progress_start(char *title,int count);
extern void progress_end(void);
extern void progress_next(void);

//
// tool palette
//

extern void tool_palette_initialize(char *app_name);
extern void tool_palette_shutdown(void);
extern void tool_palette_setup(void);
extern void tool_palette_draw(void);

//
// texture palette
//

extern void texture_palette_setup(void);
extern int texture_palette_get_selected_texture(void);
extern void texture_palette_put_selected_texture(int txt_idx);
extern void texture_palette_draw(texture_type *txt_list);
extern void texture_palette_click(texture_type *txt_list,d3pnt *pnt,bool dblclick);

//
// importing
//

extern bool import_obj(char *path,bool *found_normals,char *err_str);
extern bool import_lightwave(char *path,char *err_str);
extern bool import_c4d_xml(char *path,char *err_str);
extern void insert_model(char *file_name);

extern int textdecode_count_linestarts(void);
extern void textdecode_linestarts(void);
extern bool textdecode_open(char *path,char p_piece_break);
extern void textdecode_close(void);
extern int textdecode_count(void);
extern int textdecode_find(int line_idx,char *txt);
extern void textdecode_get_line(int line_idx,char *txt,int len);
extern void textdecode_get_piece(int line_idx,int piece_idx,char *txt);

extern void clear_materials(void);
extern int texture_count(void);
extern int texture_pick(char *material_name,char *err_str);
extern bool texture_use_single(void);

//
// controls
//

extern void add_db_column(ControlRef ctrl,char *name,int idx,int type,int sz,int spot);

extern void start_pose_controls(WindowRef wind,Rect *box);
extern void end_pose_controls(void);
extern void resize_pose_controls(Rect *box);

extern void start_bone_controls(WindowRef wind,Rect *box);
extern void end_bone_controls(void);
extern void resize_bone_controls(Rect *box);

extern void start_animate_controls(WindowRef wind,Rect *bpx);
extern void end_animate_controls(void);
extern void resize_animate_controls(Rect *box);

extern void start_mesh_controls(WindowRef wind,Rect *bpx);
extern void end_mesh_controls(void);
extern void resize_mesh_controls(Rect *box);

extern void start_vertex_controls(WindowRef wind,Rect *box);
extern void end_vertex_controls(void);
extern void resize_vertex_controls(Rect *box);

extern void reset_pose_list(void);
extern void reset_bone_list(void);
extern void reset_animate_list(void);
extern void reset_mesh_list(void);
extern void reset_vertex_tab(void);

extern void hilite_vertex_rows(void);

