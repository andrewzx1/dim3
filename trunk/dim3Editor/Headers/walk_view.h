/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Functions

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// walk view
//

extern bool walk_view_initialize(void);
extern void walk_view_shutdown(void);
extern void walk_view_setup_default_views(void);
extern void walk_view_split_horizontal(void);
extern void walk_view_split_vertical(void);
extern void walk_view_remove(void);
extern void walk_view_get_pixel_box(editor_view_type *view,d3rect *box);
extern void walk_view_set_viewport_box(d3rect *box,bool erase,bool use_background);
extern void walk_view_set_viewport(editor_view_type *view,bool erase,bool use_background);
extern void walk_view_set_2D_projection(editor_view_type *view);
extern void walk_view_set_3D_projection(editor_view_type *view,int near_z,int far_z,int near_z_offset);
extern editor_view_type* walk_view_get_current_view(void);
extern bool walk_view_point_in_view(editor_view_type *view,d3pnt *pnt);
extern void walk_view_cursor(d3pnt *pnt);
extern void walk_view_key(char ch);
extern void walk_view_get_position(d3pnt *pnt);
extern void walk_view_set_position(d3pnt *pnt);
extern void walk_view_set_position_y_shift(d3pnt *pnt,int y_shift);
extern void walk_view_move_position(d3pnt *pnt);
extern void walk_view_get_angle(d3ang *ang);
extern void walk_view_set_angle(d3ang *ang);
extern void walk_view_turn_angle(d3ang *ang);
extern void walk_view_perspective_ortho(bool on);
extern void walk_view_cull(bool on);
extern int walk_view_get_uv_layer(void);
extern void walk_view_set_uv_layer(int uv_layer);
extern int walk_view_get_magnify_factor(void);
extern void walk_view_set_magnify_factor(int magnify_factor);
extern void walk_view_face_front(void);
extern void walk_view_face_left(void);
extern void walk_view_face_right(void);
extern void walk_view_face_back(void);
extern void walk_view_face_top(void);
extern void walk_view_face_bottom(void);
extern void walk_view_select_view(d3pnt *pnt);
extern bool walk_view_click(d3pnt *pnt,bool dblclick);
extern void walk_view_draw(void);

//
// walk view draw piece
//

extern void walk_view_draw_select_mesh_get_grow_handles(int mesh_idx,int *px,int *py,int *pz);
extern void walk_view_draw_select_liquid_get_grow_handles(int liquid_idx,int *px,int *py,int *pz);
extern void walk_view_draw_view(editor_view_type *view);
extern void walk_view_draw_select(d3pnt *cpt);

//
// walk view click
//

extern bool walk_view_click_rotate_polygon_in_z(int x,int y,int z);
extern void walk_view_click_project_point(editor_view_type *view,int *x,int *y,int *z);
extern int walk_view_get_grid(void);
extern void walk_view_click_grid(d3pnt *pt);
extern bool walk_view_click_snap(int mesh_idx,d3pnt *pt);
extern bool walk_view_click_snap_poly(int mesh_idx,int poly_idx,d3pnt *pt);
extern bool walk_view_click_snap_mesh(int mesh_idx,d3pnt *pt);
extern void walk_view_click_piece(editor_view_type *view,d3pnt *pt,bool dblclick);
extern bool walk_view_click_drag_mesh_handle(editor_view_type *view,d3pnt *pt);
extern bool walk_view_click_drag_mesh(editor_view_type *view,d3pnt *pt);
extern bool walk_view_click_drag_mesh_poly(editor_view_type *view,d3pnt *pt);
extern bool walk_view_click_drag_vertex(editor_view_type *view,d3pnt *pt);
extern bool walk_view_click_drag_liquid_vertex(editor_view_type *view,d3pnt *pt);
extern bool walk_view_click_drag_item(editor_view_type *view,d3pnt *pt);
extern bool walk_view_click_drag_liquid(editor_view_type *view,d3pnt *pt);
extern bool walk_view_click_drag_texture(editor_view_type *view,d3pnt *pt,bool entire_mesh);

//
// walk view draw handle
//

extern void walk_view_sprite_select_size(d3pnt *pnt,int *px,int *py,int *pz);

//
// walk view movement
//

extern void walk_view_mouse_get_scroll_horizontal_axis(d3pnt *pnt,int dist);
extern void walk_view_mouse_get_scroll_vertical_axis(d3pnt *pnt,int dist);
extern void walk_view_mouse_scroll_movement(d3pnt *pnt);
extern void walk_view_mouse_forward_movement(d3pnt *pnt);
extern void walk_view_scroll_wheel_z_movement(int delta);
extern void walk_view_mouse_turn(d3pnt *pnt);

//
// walk view models
//

extern void walk_view_models_start(void);
extern void walk_view_models_close(void);
extern void walk_view_models_reset(void);
extern bool walk_view_model_draw(d3pnt *pnt,d3ang *ang,char *name,short *texture_frame,int frame_count);
extern bool walk_view_model_click_select_size(char *name,d3pnt *pnt,d3ang *ang,int *px,int *py,int *pz);
extern int walk_view_model_rot_y_size(d3pnt *pnt,d3ang *ang,char *name);
extern bool walk_view_model_draw_select(d3pnt *pnt,d3ang *ang,char *name);
