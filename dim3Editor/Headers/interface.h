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
// menu
//

extern void menu_fix_enable(void);
extern void menu_update_view(void);
extern bool menu_save_changes_dialog(void);
extern bool menu_event_run(int cmd);

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
// main window
//

extern void main_wind_open(void);
extern void main_wind_close(void);
extern void main_wind_resize(void);
extern void main_wind_rotate_view(void);
extern void main_wind_draw(void);
extern void main_wind_center_position_in_map(void);
extern void main_wind_key_cursor(void);
extern void main_wind_key_down(char ch);
extern void main_wind_scroll_wheel(d3pnt *pt,int delta);

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
