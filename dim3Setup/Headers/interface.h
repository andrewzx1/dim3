/****************************** File *********************************

Module: dim3 Setup
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
extern void main_wind_draw_no_swap(void);
extern void main_wind_draw(void);
extern void main_wind_play(bool play,bool blend);
extern void main_wind_click(d3pnt *pnt,bool double_click);
extern void main_wind_scroll_wheel(d3pnt *pnt,int delta);
extern void main_wind_mouse_move(d3pnt *pnt);
extern bool main_wind_cursor(void);
extern void main_wind_key(char ch);
extern void main_wind_resize(void);

//
// menus
//

extern void menu_update(void);
extern bool menu_event_run(int cmd);

//
// item list palette
//

extern void item_palette_initialize(void);
extern void item_palette_shutdown(void);
extern void item_palette_setup(void);
extern void item_palette_draw(void);
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
// dialogs
//

extern void dialog_about_run(void);
