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

extern void main_wind_initialize(void);
extern void main_wind_shutdown(void);
extern void main_wind_open(void);
extern void main_wind_close(void);
extern void main_wind_gl_setup(void);
extern void main_wind_draw(void);
extern void main_wind_draw_no_swap(void);
extern void main_wind_play(bool play,bool blend);
extern void main_wind_click(d3pnt *pnt,bool double_click);
extern void main_wind_scroll_wheel(d3pnt *pnt,int delta);
extern void main_wind_mouse_move(d3pnt *pnt);
extern bool main_wind_cursor(void);
extern void main_wind_key(char ch);
extern void main_wind_resize(void);

//
// setup
//

extern bool setup_xml_read(void);
extern bool setup_xml_write(void);

//
// menus
//

extern void menu_update(void);
extern bool menu_save_changes_dialog(void);
extern bool menu_event_run(int cmd);

//
// dos
//

extern void file_new_model(void);
extern void file_open_model(void);
extern bool file_save_model(void);
extern void file_close_model(void);
extern void file_import_mesh_obj(bool replace);
extern void file_insert_mesh_dim3_model(void);

//
// undo
//

extern void undo_initialize(void);
extern void undo_clear(void);
extern void undo_set_bone_move(int pose_idx,int bone_idx);
extern void undo_run(void);

//
// tool palette
//

extern void tool_palette_setup(void);

//
// texture palette
//

extern void texture_palette_setup(void);
extern int texture_palette_get_selected_texture(void);
extern void texture_palette_draw(texture_type *txt_list);

//
// item list palette
//

extern void item_palette_initialize(void);
extern void item_palette_shutdown(void);
extern void item_palette_setup(void);
extern void item_palette_draw(void);
extern void item_palette_state_rebuild(void);
extern void item_palette_scroll_into_view(int item_type,int item_idx);
extern bool item_palette_delete(void);
extern void item_palette_scroll_wheel(d3pnt *pnt,int move);
extern void item_palette_click(d3pnt *pnt,bool double_click);

//
// property list palette
//

extern void property_palette_initialize(void);
extern void property_palette_shutdown(void);
extern void property_palette_setup(void);
extern void property_palette_draw(void);
extern void property_palette_reset(void);
extern void property_palette_scroll_wheel(d3pnt *pnt,int move);
extern void property_palette_click(d3pnt *pnt,bool double_click);

extern void property_palette_add_string_mesh(void *list,int id,char *name,int mesh_idx,bool disabled);
extern void property_palette_add_string_bone(void *list,int id,char *name,int bone_idx,bool disabled);

extern void property_palette_pick_sound(char *name,bool include_none);
extern void property_palette_pick_particle(char *name);
extern void property_palette_pick_ring(char *name);
extern void property_palette_pick_mesh(int *mesh_idx);
extern void property_palette_pick_bone(int *bone_idx);
extern void property_palette_pick_bone_tag(unsigned long *bone_tag);
extern void property_palette_pick_pose(int *pose_idx);
extern void property_palette_pick_shader(char *name);

extern void property_palette_fill_texture(int texture_idx);
extern void property_palette_fill_animator_preference(void);
extern void property_palette_fill_model(void);
extern void property_palette_fill_mesh(int mesh_idx);
extern void property_palette_fill_animation(int animate_idx);
extern void property_palette_fill_pose(int pose_idx);
extern void property_palette_fill_bone(int bone_idx);
extern void property_palette_fill_hit_box(int hit_box_idx);

extern void property_palette_click_texture(int texture_idx,int id);
extern void property_palette_click_animator_preference(int id);
extern void property_palette_click_model(int id);
extern void property_palette_click_mesh(int mesh_idx,int id);
extern void property_palette_click_animation(int animation_idx,int id);
extern void property_palette_click_pose(int pose_idx,int id);
extern void property_palette_click_bone(int bone_idx,int id);
extern void property_palette_click_hit_box(int hit_box_idx,int id);

//
// alt property list palette
//

extern void alt_property_palette_initialize(void);
extern void alt_property_palette_shutdown(void);
extern void alt_property_palette_setup(void);
extern void alt_property_fix_open_state(void);
extern void alt_property_palette_draw(void);
extern void alt_property_palette_scroll_wheel(d3pnt *pnt,int move);
extern void alt_property_palette_click(d3pnt *pnt,bool double_click);

extern void alt_property_palette_fill_animate_pose_move(int animate_idx,int pose_move_idx);
extern void alt_property_palette_fill_pose_bone_move(int pose_idx,int bone_move_idx);

extern void alt_property_palette_click_animate_pose_move(int animate_idx,int pose_move_idx,int id);
extern void alt_property_palette_click_pose_bone_move(int pose_idx,int bone_move_idx,int id);

//
// pieces
//

extern void model_piece_add_mesh(void);
extern void model_piece_duplicate_mesh(int mesh_idx);
extern void model_piece_delete_mesh(int mesh_idx);
extern void model_piece_add_bone(void);
extern void model_piece_delete_bone(int bone_idx);
extern void model_piece_add_pose(void);
extern void model_piece_duplicate_pose(int pose_idx);
extern void model_piece_delete_pose(int pose_idx);
extern void model_piece_add_animate(void);
extern void model_piece_duplicate_animate(int animate_idx);
extern void model_piece_delete_animate(int animate_idx);
extern void model_piece_add_animation_pose_move_particle(int animate_idx,int animate_pose_move_idx);
extern void model_piece_delete_animation_pose_move_particle(int animate_idx,int animate_pose_move_idx,int idx);
extern void model_piece_add_animation_pose_move_ring(int animate_idx,int animate_pose_move_idx);
extern void model_piece_delete_animation_pose_move_ring(int animate_idx,int animate_pose_move_idx,int idx);
extern void model_piece_add_hit_box(void);
extern void model_piece_delete_hit_box(int hit_box_idx);

//
// importing
//

extern bool import_obj(char *path,bool replace,bool *found_normals,char *err_str);
extern void insert_model(char *file_name);

extern void clear_materials(void);
extern int texture_count(void);
extern bool texture_exists(char *material_name);
extern int texture_pick(char *material_name,char *err_str);

//
// models
//

extern void model_wind_get_box(d3rect *box);
extern void model_wind_click(d3pnt *pnt);

extern void draw_model(int mesh_idx);
extern void draw_model_mesh(int mesh_idx);
extern void draw_model_bones(int sel_bone_idx);
extern void draw_model_bone_names(int sel_bone_idx);

extern void draw_model_selected_vertexes(int mesh_idx);
extern void draw_model_selected_trig(int mesh_idx);
extern void draw_model_box_view(void);
extern void draw_model_box_hit_boxes(void);
extern void draw_model_axis(void);
extern void draw_model_normals_vertexes(int mesh_idx);
extern void draw_model_normals_trig(int mesh_idx);

extern void draw_model_gl_setup(int z_offset);
extern void draw_model_gl_setup_2D(void);
extern void draw_model_2D_transform_setup(void);
extern void draw_model_2D_transform(d3fpnt *pnt,d3pnt *tran_pnt);

extern void draw_model_setup_bones_vertexes(int mesh_idx);
extern void draw_model_setup_pose(int pose_idx);
extern void draw_model_wind_pose(int mesh_idx,int pose_idx);
extern void draw_model_wind(int mesh_idx);

