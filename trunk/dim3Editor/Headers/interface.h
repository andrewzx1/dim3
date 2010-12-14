/****************************** File *********************************

Module: dim3 Editor
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

extern void main_wind_open(void);
extern void main_wind_close(void);
extern void main_wind_initialize(void);
extern void main_wind_shutdown(void);
extern void main_wind_gl_setup(void);
extern void main_wind_draw(void);
extern void main_wind_draw_no_swap(void);
extern void main_wind_click(d3pnt *pnt,bool double_click);
extern void main_wind_scroll_wheel(d3pnt *pnt,int delta);
extern bool main_wind_cursor(void);
extern void main_wind_key_down(char ch);
extern void main_wind_setup(void);

//
// menu
//

extern void menu_fix_enable(void);
extern void menu_update_view(void);
extern bool menu_save_changes_dialog(void);
extern bool menu_event_run(int cmd);

//
// undo
//

extern void undo_initialize(void);
extern void undo_clear(void);
extern void undo_push(void);
extern void undo_pull(void);

//
// tool palette
//

extern void tool_palette_initialize(char *app_name);
extern void tool_palette_shutdown(void);
extern void tool_palette_setup(void);
extern void tool_palette_draw(void);
extern void tool_palette_click(d3pnt *pnt);

extern void tool_default(void);
extern void tool_switch_vertex_mode(void);
extern void tool_switch_drag_mode(void);
extern void tool_switch_grid_mode(void);
extern void tool_switch_node_mode(void);

//
// texture palette
//

extern void texture_palette_setup(void);
extern int texture_palette_get_selected_texture(void);
extern void texture_palette_put_selected_texture(int txt_idx);
extern void texture_palette_draw(texture_type *txt_list);
extern void texture_palette_reset(void);
extern void texture_palette_click(texture_type *txt_list,d3pnt *pnt,bool dbl_click);

//
// common list palette
//

extern void list_palette_initialize(void);
extern void list_palette_shutdown(void);
extern void list_palette_list_initialize(list_palette_type *list,char *title);
extern void list_palette_list_shutdown(list_palette_type *list);
extern void list_palette_set_title(list_palette_type *list,char *title);
extern void list_palette_add_header(list_palette_type *list,int piece_type,char *name);
extern void list_palette_add_item(list_palette_type *list,int piece_type,int piece_idx,char *name,bool selected);
extern void list_palette_add_color(list_palette_type *list,int piece_type,int piece_idx,d3col *col,bool selected);
extern void list_palette_add_string(list_palette_type *list,int id,char *name,char *value);
extern void list_palette_add_string_int(list_palette_type *list,int id,char *name,int value);
extern void list_palette_add_string_float(list_palette_type *list,int id,char *name,float value);
extern void list_palette_add_checkbox(list_palette_type *list,int id,char *name,bool value);
extern void list_palette_add_pick_color(list_palette_type *list,int id,char *name,d3col *col);
extern void list_palette_add_point(list_palette_type *list,int id,char *name,d3pnt *pnt);
extern void list_palette_add_angle(list_palette_type *list,int id,char *name,d3ang *ang);
extern void list_palette_delete_all_items(list_palette_type *list);
extern void list_palette_draw(list_palette_type *list);
extern void list_palette_scroll_up(list_palette_type *list);
extern void list_palette_scroll_down(list_palette_type *list);
extern void list_palette_scroll_wheel(list_palette_type *list,d3pnt *pnt,int move);
extern bool list_palette_click(list_palette_type *list,d3pnt *pnt,bool double_click);

//
// item list palette
//

extern void item_palette_initialize(void);
extern void item_palette_shutdown(void);
extern void item_palette_setup(void);
extern void item_palette_draw(void);
extern void item_palette_reset(void);
extern void item_palette_select(int sel_type,int sel_idx);
extern bool item_palette_delete(void);
extern void item_palette_scroll_wheel(d3pnt *pnt,int move);
extern void item_palette_click(d3pnt *pnt,bool double_click);

//
// property list palette
//

extern void property_palette_fill_mesh(int mesh_idx,int poly_idx);
extern void property_palette_fill_liquid(int liq_idx);
extern void property_palette_fill_spot(int spot_idx);
extern void property_palette_fill_light(int light_idx);
extern void property_palette_fill_sound(int sound_idx);
extern void property_palette_fill_particle(int particle_idx);
extern void property_palette_fill_scenery(int scenery_idx);
extern void property_palette_fill_node(int node_idx);

extern void property_palette_click_mesh(int mesh_idx,int poly_idx,int id);
extern void property_palette_click_liquid(int liq_idx,int id);
extern void property_palette_click_spot(int spot_idx,int id);
extern void property_palette_click_light(int light_idx,int id);
extern void property_palette_click_sound(int sound_idx,int id);
extern void property_palette_click_particle(int particle_idx,int id);
extern void property_palette_click_scenery(int scenery_idx,int id);
extern void property_palette_click_node(int node_idx,int id);

extern void property_palette_initialize(void);
extern void property_palette_shutdown(void);
extern void property_palette_setup(void);
extern void property_palette_draw(void);
extern void property_palette_reset(void);
extern void property_palette_scroll_wheel(d3pnt *pnt,int move);
extern void property_palette_click(d3pnt *pnt,bool double_click);

extern void property_palette_pick_list(char *list,int *idx);
extern void property_palette_pick_group(int *group_idx);
extern void property_palette_pick_spot(char *name);
extern void property_palette_pick_sound(char *name,bool include_none);
extern void property_palette_pick_particle(char *name);
extern void property_palette_pick_node(char *name);

//
// text
//

extern void text_initialize(void);
extern void text_shutdown(void);
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
// dos
//

extern bool file_new_map(void);
extern bool file_open_map(void);
extern void file_save_map(void);
extern void file_close_map(void);

//
// launching
//

extern void launch_engine(void);
extern void launch_map_script_editor(void);
extern void launch_spot_script_editor(char *script_name);

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

//
// auto-generate maps
//

extern int ag_get_styles(char *style_list);
extern bool ag_generate_map(ag_build_setup_type *build_setup);

//
// selection routines
//

extern void select_clear(void);
extern void select_add(int type,int main_idx,int sub_idx);
extern int select_count(void);
extern void select_get(int sel_idx,int *type,int *main_idx,int *sub_idx);
extern void select_switch(int sel_idx,int type,int main_idx,int sub_idx);
extern int select_find(int type,int main_idx,int sub_idx);
extern bool select_check(int type,int main_idx,int sub_idx);
extern bool select_has_type(int type);
extern void select_flip(int type,int main_idx,int sub_idx);
extern void select_remove_type(int type);
extern void select_add_group(int group_idx);
extern void select_add_movement(int movement_idx);
extern void select_add_cinema(int cinema_idx);
extern void select_delete_move_index(int type,int main_idx,int sub_idx);
extern void select_sort(void);
extern void select_duplicate_clear(void);
extern void select_duplicate_add(int type,int main_idx,int sub_idx);
extern void select_duplicate_copy(void);

//
// selection move routines
//

extern void select_get_extent(d3pnt *min,d3pnt *max);
extern void select_get_center(d3pnt *mid);
extern void select_get_angle(d3ang *ang);

//
// groups
//

extern int group_count(int group_idx);
extern void group_set(int group_idx);
extern void group_clear(int group_idx,bool delete_shift);

//
// piece routines
//

extern void piece_duplicate(void);
extern void piece_delete_face(void);
extern void piece_delete(void);
extern void piece_select_more(void);
extern void piece_tesselate(void);
extern void piece_resize(void);
extern void piece_reposition(void);
extern void piece_skew(void);
extern void piece_flip(bool flip_x,bool flip_y,bool flip_z);
extern void piece_rotate(float rot_x,float rot_y,float rot_z);
extern void piece_free_rotate(void);
extern void piece_move(int move_x,int move_y,int move_z);
extern void piece_mesh_select_all_poly(void);
extern void mesh_snap_to_grid(int mesh_idx);
extern void mesh_poly_snap_to_grid(int mesh_idx,int poly_idx);
extern void mesh_vertexes_snap_to_grid(int mesh_idx);
extern void piece_mesh_snap_to_grid(void);
extern void piece_mesh_snap_closest_vertex(void);
extern void piece_mesh_poly_snap_to_grid(void);
extern void piece_mesh_vertexes_snap_to_grid(void);
extern void piece_rotate_uvs(void);
extern void piece_flip_uvs(bool flip_u,bool flip_v);
extern void piece_reset_uvs(bool poly_only);
extern void piece_whole_uvs(bool poly_only);
extern void piece_single_uvs(bool poly_only);
extern void piece_mesh_recalc_normals(void);
extern void piece_mesh_poly_invert_normals(map_mesh_poly_type *poly);
extern void piece_mesh_invert_normals(bool poly_only);
extern void piece_poly_hole(void);
extern void piece_key(char ch);

//
// piece create routines
//

extern bool piece_create_texture_ok(void);
extern void piece_create_get_spot(d3pnt *pnt);
extern void piece_create_spot(void);
extern void piece_create_scenery(void);
extern void piece_create_light(void);
extern void piece_create_sound(void);
extern void piece_create_particle(void);
extern void piece_create_node(void);
extern void piece_create_liquid(void);

//
// piece mesh routines
//

extern void piece_add_obj_mesh(void);
extern void piece_add_obj_mesh_uv(void);
extern void piece_add_height_map_mesh(void);
extern void piece_add_grid_mesh(void);
extern void piece_add_polygon_mesh(void);
extern void piece_combine_mesh(void);
extern void piece_split_mesh(void);
extern void map_mesh_move_all(int x,int y,int z);
extern void map_mesh_reset_uv_all(void);
extern void map_optimize(void);

//
// node routines
//

extern bool node_link_click(int node_idx);
extern void node_path_rebuild(void);
