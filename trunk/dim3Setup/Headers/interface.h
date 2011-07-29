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
extern bool menu_save(void);
extern bool menu_event_run(int cmd);

//
// item list palette
//

extern void item_palette_initialize(void);
extern void item_palette_shutdown(void);
extern void item_palette_draw(void);
extern bool item_palette_delete(void);
extern void item_palette_scroll_wheel(d3pnt *pnt,int move);
extern bool item_palette_click(d3pnt *pnt,bool double_click);

//
// property list palette
//

extern void property_palette_initialize(void);
extern void property_palette_shutdown(void);
extern void property_palette_draw(void);
extern void property_palette_reset(void);
extern void property_palette_scroll_wheel(d3pnt *pnt,int move);
extern bool property_palette_click(d3pnt *pnt,bool double_click);

extern void property_palette_pick_sound(char *name,bool include_none);
extern void property_palette_pick_particle(char *name);

extern void property_palette_fill_settings(void);
extern void property_palette_fill_intro(void);
extern void property_palette_fill_setup(void);
extern void property_palette_fill_hud(void);
extern void property_palette_fill_radar(void);
extern void property_palette_fill_multiplayer(void);
extern void property_palette_fill_menu(void);
extern void property_palette_fill_chooser(void);
extern void property_palette_fill_sounds(void);
extern void property_palette_fill_particles(void);
extern void property_palette_fill_rings(void);
extern void property_palette_fill_halos(void);
extern void property_palette_fill_marks(void);
extern void property_palette_fill_crosshairs(void);
extern void property_palette_fill_actions(void);
extern void property_palette_fill_shaders(void);

extern void property_palette_click_settings(int id,bool double_click);
extern void property_palette_click_intro(int id,bool double_click);
extern void property_palette_click_setup(int id,bool double_click);
extern void property_palette_click_hud(int id,bool double_click);
extern void property_palette_click_radar(int id,bool double_click);
extern void property_palette_click_multiplayer(int id,bool double_click);
extern void property_palette_click_menu(int id,bool double_click);
extern void property_palette_click_chooser(int id,bool double_click);
extern void property_palette_click_sounds(int id,bool double_click);
extern void property_palette_click_particles(int id,bool double_click);
extern void property_palette_click_rings(int id,bool double_click);
extern void property_palette_click_halos(int id,bool double_click);
extern void property_palette_click_marks(int id,bool double_click);
extern void property_palette_click_crosshairs(int id,bool double_click);
extern void property_palette_click_actions(int id,bool double_click);
extern void property_palette_click_shaders(int id,bool double_click);

//
// alt property list palette
//

extern void alt_property_palette_initialize(void);
extern void alt_property_palette_shutdown(void);
extern void alt_property_palette_draw(void);
extern void alt_property_palette_scroll_wheel(d3pnt *pnt,int move);
extern bool alt_property_palette_click(d3pnt *pnt,bool double_click);

extern void alt_property_palette_fill_intro_button(int intro_button_idx);
extern void alt_property_palette_fill_intro_model(int intro_model_idx);
extern void alt_property_palette_fill_hud_bitmap(int hud_bitmap_idx);
extern void alt_property_palette_fill_hud_text(int hud_text_idx);
extern void alt_property_palette_fill_hud_bar(int hud_bar_idx);
extern void alt_property_palette_fill_radar_icon(int radar_icon_idx);
extern void alt_property_palette_fill_multiplayer_character(int multiplayer_character_idx);
extern void alt_property_palette_fill_multiplayer_game(int multiplayer_game_idx);
extern void alt_property_palette_fill_multiplayer_option(int multiplayer_option_idx);
extern void alt_property_palette_fill_menu(int menu_idx);
extern void alt_property_palette_fill_chooser(int chooser_idx);
extern void alt_property_palette_fill_sound(int sound_idx);
extern void alt_property_palette_fill_particle(int particle_idx);
extern void alt_property_palette_fill_particle_group(int particle_idx);
extern void alt_property_palette_fill_ring(int ring_idx);
extern void alt_property_palette_fill_halo(int halo_idx);
extern void alt_property_palette_fill_mark(int mark_idx);
extern void alt_property_palette_fill_crosshair(int crosshair_idx);
extern void alt_property_palette_fill_action(int action_idx);
extern void alt_property_palette_fill_shader(int shader_idx);

extern void alt_property_palette_click_intro_button(int intro_button_idx,int id,bool double_click);
extern void alt_property_palette_click_intro_model(int intro_model_idx,int id,bool double_click);
extern void alt_property_palette_click_hud_bitmap(int hud_bitmap_idx,int id,bool double_click);
extern void alt_property_palette_click_hud_text(int hud_text_idx,int id,bool double_click);
extern void alt_property_palette_click_hud_bar(int hud_bar_idx,int id,bool double_click);
extern void alt_property_palette_click_radar_icon(int radar_icon_idx,int id,bool double_click);
extern void alt_property_palette_click_multiplayer_character(int multiplayer_character_idx,int id,bool double_click);
extern void alt_property_palette_click_multiplayer_game(int game_character_idx,int id,bool double_click);
extern void alt_property_palette_click_multiplayer_option(int option_character_idx,int id,bool double_click);
extern void alt_property_palette_click_menu(int menu_idx,int id,bool double_click);
extern void alt_property_palette_click_chooser(int chooser_idx,int id,bool double_click);
extern void alt_property_palette_click_sound(int sound_idx,int id,bool double_click);
extern void alt_property_palette_click_particle(int particle_idx,int id,bool double_click);
extern void alt_property_palette_click_particle_group(int particle_idx,int id,bool double_click);
extern void alt_property_palette_click_ring(int ring_idx,int id,bool double_click);
extern void alt_property_palette_click_halo(int halo_idx,int id,bool double_click);
extern void alt_property_palette_click_mark(int mark_idx,int id,bool double_click);
extern void alt_property_palette_click_crosshair(int crosshair_idx,int id,bool double_click);
extern void alt_property_palette_click_action(int action_idx,int id,bool double_click);
extern void alt_property_palette_click_shader(int shader_idx,int id,bool double_click);

//
// alt2 property list palette
//

extern void alt2_property_palette_initialize(void);
extern void alt2_property_palette_shutdown(void);
extern void alt2_property_palette_draw(void);
extern void alt2_property_palette_scroll_wheel(d3pnt *pnt,int move);
extern bool alt2_property_palette_click(d3pnt *pnt,bool double_click);

extern void alt2_property_palette_fill_menu_item(int menu_idx,int menu_item_idx);
extern void alt2_property_palette_fill_chooser_piece(int chooser_idx,int chooser_item_idx);
extern void alt2_property_palette_fill_group_particle(int particle_idx,int group_particle_idx);

extern void alt2_property_palette_click_menu_item(int menu_idx,int menu_item_idx,int id,bool double_click);
extern void alt2_property_palette_click_chooser_piece(int chooser_idx,int chooser_item_idx,int id,bool double_click);
extern void alt2_property_palette_click_group_particle(int particle_idx,int group_particle_idx,int id,bool double_click);

//
// dialogs
//

extern void dialog_about_run(void);
extern bool dialog_file_open_run(char *title,char *search_path,char *extension,char *required_file_name,char *file_name);
