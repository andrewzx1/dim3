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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// main app
//

extern bool main_app_initialize(void);
extern void main_app_shutdown(void);

//
// main window
//

extern void main_wind_initialize(void);
extern void main_wind_shutdown(void);
extern void main_wind_menu_create(void);
extern void main_wind_menu_dispose(void);
extern void main_wind_menu_update(void);
extern bool main_wind_menu_event_run(int cmd);
extern void main_wind_switch_mode(int mode);
extern void main_wind_gl_setup(void);
extern void main_wind_draw(void);
extern void main_wind_draw_no_swap(void);
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

extern void project_menu_create(void);
extern void project_menu_dispose(void);
extern void project_menu_update(void);
extern bool project_menu_event_run(int cmd);

extern void map_menu_create(void);
extern void map_menu_dispose(void);
extern void map_menu_update(void);
extern bool map_menu_event_run(int cmd);

extern void model_menu_create(void);
extern void model_menu_dispose(void);
extern void model_menu_update(void);
extern bool model_menu_event_run(int cmd);

//
// undo
//

extern void undo_initialize(void);
extern void undo_clear(void);
extern void undo_push(void);
extern void undo_pull(void);

//
// tool palettes
//

extern void project_tool_palette_initialize(void);
extern void project_tool_palette_shutdown(void);
extern void project_tool_palette_set_state(void);
extern void project_tool_palette_click(int idx);
extern void project_tool_default(void);

extern void map_tool_palette_initialize(void);
extern void map_tool_palette_shutdown(void);
extern void map_tool_palette_set_state(void);
extern void map_tool_palette_click(int idx);
extern void map_tool_default(void);

extern void model_tool_palette_initialize(void);
extern void model_tool_palette_shutdown(void);
extern void model_tool_palette_set_state(void);
extern void model_tool_palette_click(int idx);
extern void model_tool_default(void);

//
// texture palette
//

extern int texture_palette_per_page_count(void);
extern int texture_palette_pixel_size(void);
extern int texture_palette_page_list_count(void);
extern int texture_palette_page_list_width(void);
extern void texture_palette_box(d3rect *box);
extern int texture_palette_get_selected_texture(void);
extern void texture_palette_put_selected_texture(int txt_idx);
extern void texture_palette_reset(void);

//
// file list palette
//

extern void file_palette_initialize(void);
extern void file_palette_shutdown(void);
extern void file_palette_fill(void);
extern void file_palette_draw(void);
extern void file_palette_scroll_wheel(d3pnt *pnt,int move);
extern void file_palette_click(d3pnt *pnt,bool double_click);

//
// project property palettes
//

extern void project_palette_initialize(void);
extern void project_palette_shutdown(void);
extern void project_palette_draw(void);
extern void project_palette_reset(void);
extern void project_palette_scroll_wheel(d3pnt *pnt,int move);
extern bool project_palette_delete(void);
extern void project_palette_click(d3pnt *pnt,bool double_click);

extern void project_palette_fill_main(void);
extern void project_palette_fill_project(void);
extern void project_palette_fill_color(void);
extern void project_palette_fill_devices(void);
extern void project_palette_fill_logo(void);
extern void project_palette_fill_title_page(void);
extern void project_palette_fill_singleplayer(void);
extern void project_palette_fill_multiplayer(void);
extern void project_palette_fill_setup(void);
extern void project_palette_fill_progress(void);
extern void project_palette_fill_hud(void);
extern void project_palette_fill_virtual_control(void);
extern void project_palette_fill_radar(void);
extern void project_palette_fill_label(void);
extern void project_palette_fill_menus(void);
extern void project_palette_fill_choosers(void);
extern void project_palette_fill_sounds(void);
extern void project_palette_fill_particles(void);
extern void project_palette_fill_rings(void);
extern void project_palette_fill_halos(void);
extern void project_palette_fill_marks(void);
extern void project_palette_fill_crosshairs(void);
extern void project_palette_fill_actions(void);
extern void project_palette_fill_shaders(void);
extern void project_palette_fill_preload_models(void);

extern void project_palette_click_main(bool double_click);
extern void project_palette_click_project(bool double_click);
extern void project_palette_click_color(bool double_click);
extern void project_palette_click_devices(bool double_click);
extern void project_palette_click_logo(bool double_click);
extern void project_palette_click_title_page(bool double_click);
extern void project_palette_click_singleplayer(bool double_click);
extern void project_palette_click_multiplayer(bool double_click);
extern void project_palette_click_setup(bool double_click);
extern void project_palette_click_progress(bool double_click);
extern void project_palette_click_hud(bool double_click);
extern void project_palette_click_virtual_control(bool double_click);
extern void project_palette_click_radar(bool double_click);
extern void project_palette_click_label(bool double_click);
extern void project_palette_click_menus(bool double_click);
extern void project_palette_click_choosers(bool double_click);
extern void project_palette_click_sounds(bool double_click);
extern void project_palette_click_particles(bool double_click);
extern void project_palette_click_rings(bool double_click);
extern void project_palette_click_halos(bool double_click);
extern void project_palette_click_marks(bool double_click);
extern void project_palette_click_crosshairs(bool double_click);
extern void project_palette_click_actions(bool double_click);
extern void project_palette_click_shaders(bool double_click);
extern void project_palette_click_preload_models(bool double_click);

extern void project_palette_fill_title_page_button(int intro_button_idx);
extern void project_palette_fill_title_page_model(int intro_model_idx);
extern void project_palette_fill_hud_bitmap(int hud_bitmap_idx);
extern void project_palette_fill_hud_text(int hud_text_idx);
extern void project_palette_fill_hud_bar(int hud_bar_idx);
extern void project_palette_fill_virtual_control_stick(int virtual_control_stick_idx);
extern void project_palette_fill_virtual_control_button(int virtual_control_button_idx);
extern void project_palette_fill_radar_icon(int radar_icon_idx);
extern void project_palette_fill_singleplayer_option(int singleplayer_option_idx);
extern void project_palette_fill_multiplayer_character(int multiplayer_character_idx);
extern void project_palette_fill_multiplayer_game(int multiplayer_game_idx);
extern void project_palette_fill_multiplayer_option(int multiplayer_option_idx);
extern void project_palette_fill_menu(int menu_idx);
extern void project_palette_fill_chooser(int chooser_idx);
extern void project_palette_fill_sound(int sound_idx);
extern void project_palette_fill_particle(int particle_idx);
extern void project_palette_fill_particle_group(int particle_idx);
extern void project_palette_fill_ring(int ring_idx);
extern void project_palette_fill_halo(int halo_idx);
extern void project_palette_fill_mark(int mark_idx);
extern void project_palette_fill_crosshair(int crosshair_idx);
extern void project_palette_fill_action(int action_idx);
extern void project_palette_fill_shader(int shader_idx);

extern void project_palette_click_title_page_button(int intro_button_idx,bool double_click);
extern void project_palette_click_title_page_model(int intro_model_idx,bool double_click);
extern void project_palette_click_hud_bitmap(int hud_bitmap_idx,bool double_click);
extern void project_palette_click_hud_text(int hud_text_idx,bool double_click);
extern void project_palette_click_hud_bar(int hud_bar_idx,bool double_click);
extern void project_palette_click_virtual_control_stick(int virtual_control_stick_idx,bool double_click);
extern void project_palette_click_virtual_control_button(int virtual_control_button_idx,bool double_click);
extern void project_palette_click_radar_icon(int radar_icon_idx,bool double_click);
extern void project_palette_click_singleplayer_option(int option_option_idx,bool double_click);
extern void project_palette_click_multiplayer_character(int multiplayer_character_idx,bool double_click);
extern void project_palette_click_multiplayer_game(int game_game_idx,bool double_click);
extern void project_palette_click_multiplayer_option(int option_option_idx,bool double_click);
extern void project_palette_click_menu(int menu_idx,bool double_click);
extern void project_palette_click_chooser(int chooser_idx,bool double_click);
extern void project_palette_click_sound(int sound_idx,bool double_click);
extern void project_palette_click_particle(int particle_idx,bool double_click);
extern void project_palette_click_particle_group(int particle_idx,bool double_click);
extern void project_palette_click_ring(int ring_idx,bool double_click);
extern void project_palette_click_halo(int halo_idx,bool double_click);
extern void project_palette_click_mark(int mark_idx,bool double_click);
extern void project_palette_click_crosshair(int crosshair_idx,bool double_click);
extern void project_palette_click_action(int action_idx,bool double_click);
extern void project_palette_click_shader(int shader_idx,bool double_click);

extern void project_palette_fill_menu_item(int menu_idx,int menu_item_idx);
extern void project_palette_fill_chooser_piece(int chooser_idx,int chooser_item_idx);
extern void project_palette_fill_group_particle(int particle_idx,int group_particle_idx);

extern void project_palette_click_menu_item(int menu_idx,int menu_item_idx,bool double_click);
extern void project_palette_click_chooser_piece(int chooser_idx,int chooser_item_idx,bool double_click);
extern void project_palette_click_group_particle(int particle_idx,int group_particle_idx,bool double_click);

//
// map property palettes
//

extern void map_palette_fill_texture(int texture_idx);
extern void map_palette_fill_editor_preference(void);
extern void map_palette_fill_main(void);
extern void map_palette_fill_map(void);
extern void map_palette_fill_camera(void);
extern void map_palette_fill_light_media(void);
extern void map_palette_fill_sky_weather(void);
extern void map_palette_fill_cinema(int cinema_idx);
extern void map_palette_fill_group(int group_idx);
extern void map_palette_fill_movement(int movement_idx);
extern void map_palette_fill_mesh(int mesh_idx,int poly_idx);
extern void map_palette_fill_liquid(int liq_idx);
extern void map_palette_fill_spot(int spot_idx);
extern void map_palette_fill_light(int light_idx);
extern void map_palette_fill_sound(int sound_idx);
extern void map_palette_fill_particle(int particle_idx);
extern void map_palette_fill_scenery(int scenery_idx);
extern void map_palette_fill_node(int node_idx);

extern void map_palette_click_texture(int texture_idx,int id,bool double_click);
extern void map_palette_click_editor_preference(int id,bool double_click);
extern void map_palette_click_main(bool double_click);
extern void map_palette_click_map(bool double_click);
extern void map_palette_click_camera(bool double_click);
extern void map_palette_click_light_media(bool double_click);
extern void map_palette_click_sky_weather(bool double_click);
extern void map_palette_click_cinema(bool double_click);
extern void map_palette_click_group(bool double_click);
extern void map_palette_click_movement(bool double_click);
extern void map_palette_click_mesh(int mesh_idx,int poly_idx,bool double_click);
extern void map_palette_click_liquid(int liq_idx,bool double_click);
extern void map_palette_click_spot(int spot_idx,bool double_click);
extern void map_palette_click_light(int light_idx,bool double_click);
extern void map_palette_click_sound(int sound_idx,bool double_click);
extern void map_palette_click_particle(int particle_idx,bool double_click);
extern void map_palette_click_scenery(int scenery_idx,bool double_click);
extern void map_palette_click_node(int node_idx,bool double_click);

extern void map_palette_initialize(void);
extern void map_palette_shutdown(void);
extern void map_palette_draw(void);
extern void map_palette_reset(void);
extern void map_palette_scroll_into_view(int item_id,int item_idx);
extern void map_palette_scroll_wheel(d3pnt *pnt,int move);
extern bool map_palette_delete(void);
extern void map_palette_click(d3pnt *pnt,bool double_click);

extern void map_palette_fill_cinema_action(int cinema_idx,int action_idx);
extern void map_palette_fill_movement_move(int movement_idx,int move_idx);

extern void map_palette_click_cinema_action(int cinema_idx,int action_idx,bool double_click);
extern void map_palette_click_movement_move(int movement_idx,int move_idx,bool double_click);

//
// property palette
//

extern void model_palette_initialize(void);
extern void model_palette_shutdown(void);
extern void model_palette_draw(void);
extern void model_palette_reset(void);
extern void model_palette_scroll_into_view(int item_type,int item_idx);
extern void model_palette_scroll_wheel(d3pnt *pnt,int move);
extern bool model_palette_delete(void);
extern void model_palette_click(d3pnt *pnt,bool double_click);

extern void model_palette_fill_texture(int texture_idx);
extern void model_palette_fill_animator_preference(void);
extern void model_palette_fill_main(void);
extern void model_palette_fill_model(void);
extern void model_palette_fill_mesh(int mesh_idx);
extern void model_palette_fill_animation(int animate_idx);
extern void model_palette_fill_pose(int pose_idx);
extern void model_palette_fill_bone(int bone_idx,int pose_idx);
extern void model_palette_fill_hit_box(int hit_box_idx);

extern void model_palette_click_texture(int texture_idx,int id,bool double_click);
extern void model_palette_click_animator_preference(int id,bool double_click);
extern void model_palette_click_main(bool double_click);
extern void model_palette_click_model(bool double_click);
extern void model_palette_click_mesh(int mesh_idx,bool double_click);
extern void model_palette_click_animation(int animation_idx,bool double_click);
extern void model_palette_click_pose(int pose_idx,bool double_click);
extern void model_palette_click_bone(int bone_idx,int pose_idx,bool double_click);
extern void model_palette_click_hit_box(int hit_box_idx,bool double_click);

extern void model_palette_fill_animate_pose_move(int animate_idx,int pose_move_idx);
extern void model_palette_fill_pose_bone_move(int pose_idx,int bone_move_idx);

extern void model_palette_click_animate_pose_move(int animate_idx,int pose_move_idx,bool double_click);
extern void model_palette_click_pose_bone_move(int pose_idx,int bone_move_idx,bool double_click);

extern void model_palette_fill_animate_pose_move_particle(int animate_idx,int pose_move_idx,int particle_idx);
extern void model_palette_fill_animate_pose_move_ring(int animate_idx,int pose_move_idx,int ring_idx);

extern void model_palette_click_animate_pose_move_particle(int animate_idx,int pose_move_idx,int particle_idx,bool double_click);
extern void model_palette_click_animate_pose_move_ring(int animate_idx,int pose_move_idx,int ring_idx,bool double_click);

//
// dos
//

extern bool file_new_map(void);
extern bool file_open_map(char *file_name);
extern void file_save_map(void);
extern bool file_close_map(void);

//
// launching
//

extern void launch_engine(void);
extern void launch_map_script_editor(void);
extern void launch_spot_script_editor(spot_type *spot);

//
// light maps
//

extern void light_map_texture_map_folder_name(char *map_name);
extern float light_map_ray_trace_triangle(d3pnt *spt,d3vct *vct,int *x,int *y,int *z);
extern bool light_maps_create(void);
extern void light_maps_clear(void);

//
// liquid reflection maps
//

extern bool liquid_reflection_maps_create(void);

//
// auto-generate
//

extern void auto_generate_map(void);

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
extern void select_duplicate_backup(void);
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
extern void piece_tesselate(bool mesh);
extern void piece_resize(void);
extern void piece_force_grid(void);
extern void piece_flip(bool flip_x,bool flip_y,bool flip_z);
extern void piece_rotate(d3ang *ang);
extern void piece_free_rotate(void);
extern void piece_move(int move_x,int move_y,int move_z);
extern void piece_mesh_select_all_poly(void);
extern void piece_rotate_uvs(void);
extern void piece_flip_uvs(bool flip_u,bool flip_v);
extern void piece_reset_uvs(bool poly_only);
extern void piece_whole_uvs(bool poly_only);
extern void piece_single_uvs(bool poly_only);
extern void piece_mesh_recalc_normals(bool poly_only);
extern void piece_mesh_poly_invert_normals(map_mesh_poly_type *poly);
extern void piece_mesh_invert_normals(bool poly_only);
extern void piece_mesh_set_normals_in_out(bool out);
extern void piece_poly_hole(void);
extern void piece_liquid_reset_size(map_liquid_type *liq);
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
extern void piece_create_cinema(void);
extern void piece_create_group(void);
extern void piece_create_movement(void);

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

//
// import routines
//

extern bool import_obj(char *path,char *err_str);

//
// node routines
//

extern bool node_link_click(int node_idx);
extern int node_duplicate_and_drag(int org_node_idx);
extern void node_path_rebuild(void);

//
// view general
//

extern bool view_initialize(void);
extern void view_shutdown(void);
extern void view_setup_default_views(void);
extern void view_split_horizontal(void);
extern void view_split_vertical(void);
extern void view_split_remove(void);
extern void view_center_all(bool reset_ang);
extern void view_get_pixel_box(editor_view_type *view,d3rect *box);
extern void view_set_viewport_box(d3rect *box,bool erase,bool use_background);
extern void view_set_viewport(editor_view_type *view,bool erase,bool use_background);
extern float view_get_lookat_x_angle(editor_view_type *view);
extern void view_set_2D_projection(editor_view_type *view);
extern void view_set_3D_projection(editor_view_type *view,int near_z,int far_z,int near_z_offset);
extern void view_setup_project_point(void);
extern void view_project_point(editor_view_type *view,d3pnt *pnt);
extern bool view_project_point_in_z(d3pnt *pnt);
extern editor_view_type* view_get_current_view(void);
extern bool view_point_in_view(editor_view_type *view,d3pnt *pnt);
extern bool view_cursor(d3pnt *pnt);
extern void view_key(char ch);
extern void view_get_position(d3pnt *pnt);
extern void view_set_position(d3pnt *pnt);
extern void view_set_position_y_shift(d3pnt *pnt,int y_shift);
extern void view_move_position(d3pnt *pnt);
extern void view_get_angle(d3ang *ang);
extern void view_set_angle(d3ang *ang);
extern void view_turn_angle(d3ang *ang);
extern void view_perspective_ortho(bool on);
extern void view_cull(bool on);
extern void view_clip(bool on);
extern void view_goto_select(void);
extern void view_goto_map_center(void);
extern void view_goto_map_center_all(void);
extern void view_flip_clip(void);
extern int view_get_uv_layer(void);
extern void view_set_uv_layer(int uv_layer);
extern void view_no_rot(bool on);
extern void view_face_front(void);
extern void view_face_left(void);
extern void view_face_right(void);
extern void view_face_back(void);
extern void view_face_top(void);
extern void view_face_bottom(void);
extern void view_select_view(d3pnt *pnt);
extern void view_scroll_wheel(d3pnt *pnt,int delta);
extern bool view_click(d3pnt *pnt,bool double_click);
extern void view_draw(void);

//
// view VBO
//

extern void view_vbo_mesh_rebuild(int mesh_idx);
extern void view_vbo_mesh_initialize(int mesh_idx);
extern void view_vbo_mesh_free(int mesh_idx);
extern void view_vbo_map_initialize(void);
extern void view_vbo_map_free(void);

//
// view draw piece
//

extern void view_cull_setup_frustum_clipping_planes(void);
extern bool view_cull_boundbox_in_frustum(d3pnt *min,d3pnt *max);
extern bool view_clip_poly(editor_view_type *view,map_mesh_poly_type *poly);
extern bool view_clip_liquid(editor_view_type *view,map_liquid_type *liq);
extern void view_draw_select_mesh_get_grow_handles(int mesh_idx,d3pnt *pts);
extern void view_draw_select_liquid_get_grow_handles(int liquid_idx,d3pnt *pts);
extern void view_draw_view(editor_view_type *view);
extern void view_draw_select(editor_view_type *view);

//
// view picking
//

extern bool view_pick_list_start(editor_view_type *view,bool in_2D,int count);
extern void view_pick_list_end(editor_view_type *view,d3pnt *pnt,int *type,int *main_idx,int *sub_idx);
extern bool view_pick_list_multiple_setup(editor_view_type *view);
extern int view_pick_list_multiple_pick(editor_view_type *view,d3pnt *start_pnt,d3pnt *end_pnt,view_picker_type *pick_list);
extern void view_pick_list_multiple_end(void);
extern void view_pick_list_add(int type,int main_idx,int sub_idx);
extern void view_pick_list_add_cube(d3pnt *v_pnts,int type,int main_idx,int sub_idx);
extern void view_pick_list_add_handle(d3pnt *pnt,int type,int main_idx,int sub_idx);
extern void view_pick_list_add_2D_handle(d3pnt *pnt,int type,int main_idx,int sub_idx);

//
// view click
//

extern int view_get_grid(void);
extern void view_click_grid(d3pnt *pt);
extern void view_force_grid(int mesh_idx,bool pos_only);
extern bool view_click_snap(int mesh_idx,int liquid_idx,d3pnt *pt);
extern bool view_click_snap_poly(int mesh_idx,int poly_idx,d3pnt *pt);
extern void view_click_snap_mesh(d3pnt *old_dpt,d3pnt *mpt);
extern void view_click_piece_map_pick_start(editor_view_type *view);
extern void view_click_piece(editor_view_type *view,d3pnt *pt,bool double_click);
extern void view_click_fix_cascade_size(int mesh_idx);
extern bool view_click_drag_mesh_handle(editor_view_type *view,d3pnt *pt);
extern bool view_click_drag_mesh(editor_view_type *view,d3pnt *pt);
extern bool view_click_drag_mesh_poly(editor_view_type *view,d3pnt *pt);
extern bool view_click_drag_vertex(editor_view_type *view,d3pnt *pt);
extern bool view_click_drag_texture_uv(editor_view_type *view,d3pnt *pt,bool entire_mesh);
extern bool view_click_drag_liquid_vertex(editor_view_type *view,d3pnt *pt);
extern bool view_click_drag_liquid(editor_view_type *view,d3pnt *pt);
extern bool view_click_drag_item(editor_view_type *view,d3pnt *pt);
extern bool view_click_box_select(editor_view_type *view,d3pnt *pt);

//
// view draw handle
//

extern bool view_handle_create_rot_handle(editor_view_type *view,d3pnt *pnt,d3ang *ang,d3pnt *center_pnt,d3pnt *hand_pnt);
extern void view_get_sprite_vertexes(d3pnt *pnt,d3ang *ang,d3pnt *v_pnts);

//
// view movement
//

extern void view_mouse_get_scroll_horizontal_axis(editor_view_type *view,d3pnt *pnt,int dist);
extern void view_mouse_get_scroll_vertical_axis(editor_view_type *view,d3pnt *pnt,int dist);
extern void view_mouse_scroll_movement(editor_view_type *view,d3pnt *pnt);
extern void view_mouse_forward_movement(editor_view_type *view,d3pnt *pnt);
extern void view_key_forward_movement(editor_view_type *view,int dir);
extern void view_key_side_movement(editor_view_type *view,int dir);
extern void view_key_vert_movement(editor_view_type *view,int dir);
extern void view_mouse_get_forward_axis(editor_view_type *view,d3pnt *pnt,int dist);
extern void view_mouse_turn(editor_view_type *view,d3pnt *pnt);

//
// view models
//

extern void view_models_start(void);
extern void view_models_close(void);
extern void view_models_reset(void);
extern void view_get_model_size(char *model_name,d3pnt *size);
extern bool view_model_draw(d3pnt *pnt,d3ang *ang,char *name,float resize,int *texture_frame,int frame_count);
extern void view_model_cube_vertexes(char *name,d3pnt *pnt,d3ang *ang,float resize,d3pnt *v_pnts);

//
// dialogs
//

extern bool dialog_new_map_run(char *file_name);
extern bool dialog_file_open_run(char *title,char *search_path,char *extension,char *required_file_name,char *file_name);
extern bool dialog_scale_run(d3fpnt *scale);
extern bool dialog_reposition_run(d3pnt *min,d3pnt *max);
extern bool dialog_free_rotate_run(d3ang *ang);
extern bool dialog_grid_run(d3pnt *divs,bool *sides);
extern bool dialog_obj_import_run(int *import_mode,int *scale_axis,int *scale_unit,bool *force_grid);
extern bool dialog_height_import_run(int *div_cnt,int *size,int *high);

