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

extern void property_palette_fill_settings(void);
extern void property_palette_fill_intro(void);
extern void property_palette_fill_hud(void);
extern void property_palette_fill_sounds(void);
extern void property_palette_fill_particles(void);
extern void property_palette_fill_rings(void);
extern void property_palette_fill_halos(void);
extern void property_palette_fill_marks(void);
extern void property_palette_fill_crosshairs(void);
extern void property_palette_fill_actions(void);

extern void property_palette_click_settings(int id);
extern void property_palette_click_intro(int id);
extern void property_palette_click_hud(int id);
extern void property_palette_click_sounds(int id);
extern void property_palette_click_particles(int id);
extern void property_palette_click_rings(int id);
extern void property_palette_click_halos(int id);
extern void property_palette_click_marks(int id);
extern void property_palette_click_crosshairs(int id);
extern void property_palette_click_actions(int id);

//
// alt property list palette
//

extern void alt_property_palette_initialize(void);
extern void alt_property_palette_shutdown(void);
extern void alt_property_palette_setup(void);
extern void alt_property_palette_draw(void);
extern void alt_property_palette_scroll_wheel(d3pnt *pnt,int move);
extern void alt_property_palette_click(d3pnt *pnt,bool double_click);

extern void alt_property_palette_fill_intro_button(int intro_button_idx);
extern void alt_property_palette_fill_intro_model(int intro_model_idx);

extern void alt_property_palette_click_intro_button(int intro_button_idx,int id);
extern void alt_property_palette_click_intro_model(int intro_model_idx,int id);

//
// dialogs
//

extern void dialog_about_run(void);
extern bool dialog_file_open_run(char *title,char *search_path,char *extension,char *required_file_name,char *file_name);
