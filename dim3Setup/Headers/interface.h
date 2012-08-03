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

extern void menu_create(void);
extern void menu_dispose(void);
extern void menu_update(void);
extern bool menu_quit_save(void);
extern bool menu_event_run(int cmd);

//
// property list palette
//

extern void property_palette_initialize(void);
extern void property_palette_shutdown(void);
extern void property_palette_draw(void);
extern void property_palette_reset(void);
extern void property_palette_scroll_wheel(d3pnt *pnt,int move);
extern bool property_palette_delete(void);
extern void property_palette_click(d3pnt *pnt,bool double_click);

extern void property_palette_fill_main(void);
extern void property_palette_fill_project(void);
extern void property_palette_fill_color(void);
extern void property_palette_fill_devices(void);
extern void property_palette_fill_logo(void);
extern void property_palette_fill_title_page(void);
extern void property_palette_fill_singleplayer(void);
extern void property_palette_fill_multiplayer(void);
extern void property_palette_fill_setup(void);
extern void property_palette_fill_progress(void);
extern void property_palette_fill_hud(void);
extern void property_palette_fill_virtual_control(void);
extern void property_palette_fill_radar(void);
extern void property_palette_fill_label(void);
extern void property_palette_fill_menus(void);
extern void property_palette_fill_choosers(void);
extern void property_palette_fill_sounds(void);
extern void property_palette_fill_particles(void);
extern void property_palette_fill_rings(void);
extern void property_palette_fill_halos(void);
extern void property_palette_fill_marks(void);
extern void property_palette_fill_crosshairs(void);
extern void property_palette_fill_actions(void);
extern void property_palette_fill_shaders(void);
extern void property_palette_fill_preload_models(void);

extern void property_palette_click_main(bool double_click);
extern void property_palette_click_project(bool double_click);
extern void property_palette_click_color(bool double_click);
extern void property_palette_click_devices(bool double_click);
extern void property_palette_click_logo(bool double_click);
extern void property_palette_click_title_page(bool double_click);
extern void property_palette_click_singleplayer(bool double_click);
extern void property_palette_click_multiplayer(bool double_click);
extern void property_palette_click_setup(bool double_click);
extern void property_palette_click_progress(bool double_click);
extern void property_palette_click_hud(bool double_click);
extern void property_palette_click_virtual_control(bool double_click);
extern void property_palette_click_radar(bool double_click);
extern void property_palette_click_label(bool double_click);
extern void property_palette_click_menus(bool double_click);
extern void property_palette_click_choosers(bool double_click);
extern void property_palette_click_sounds(bool double_click);
extern void property_palette_click_particles(bool double_click);
extern void property_palette_click_rings(bool double_click);
extern void property_palette_click_halos(bool double_click);
extern void property_palette_click_marks(bool double_click);
extern void property_palette_click_crosshairs(bool double_click);
extern void property_palette_click_actions(bool double_click);
extern void property_palette_click_shaders(bool double_click);
extern void property_palette_click_preload_models(bool double_click);

extern void property_palette_fill_title_page_button(int intro_button_idx);
extern void property_palette_fill_title_page_model(int intro_model_idx);
extern void property_palette_fill_hud_bitmap(int hud_bitmap_idx);
extern void property_palette_fill_hud_text(int hud_text_idx);
extern void property_palette_fill_hud_bar(int hud_bar_idx);
extern void property_palette_fill_virtual_control_stick(int virtual_control_stick_idx);
extern void property_palette_fill_virtual_control_button(int virtual_control_button_idx);
extern void property_palette_fill_radar_icon(int radar_icon_idx);
extern void property_palette_fill_singleplayer_option(int singleplayer_option_idx);
extern void property_palette_fill_multiplayer_character(int multiplayer_character_idx);
extern void property_palette_fill_multiplayer_game(int multiplayer_game_idx);
extern void property_palette_fill_multiplayer_option(int multiplayer_option_idx);
extern void property_palette_fill_menu(int menu_idx);
extern void property_palette_fill_chooser(int chooser_idx);
extern void property_palette_fill_sound(int sound_idx);
extern void property_palette_fill_particle(int particle_idx);
extern void property_palette_fill_particle_group(int particle_idx);
extern void property_palette_fill_ring(int ring_idx);
extern void property_palette_fill_halo(int halo_idx);
extern void property_palette_fill_mark(int mark_idx);
extern void property_palette_fill_crosshair(int crosshair_idx);
extern void property_palette_fill_action(int action_idx);
extern void property_palette_fill_shader(int shader_idx);

extern void property_palette_click_title_page_button(int intro_button_idx,bool double_click);
extern void property_palette_click_title_page_model(int intro_model_idx,bool double_click);
extern void property_palette_click_hud_bitmap(int hud_bitmap_idx,bool double_click);
extern void property_palette_click_hud_text(int hud_text_idx,bool double_click);
extern void property_palette_click_hud_bar(int hud_bar_idx,bool double_click);
extern void property_palette_click_virtual_control_stick(int virtual_control_stick_idx,bool double_click);
extern void property_palette_click_virtual_control_button(int virtual_control_button_idx,bool double_click);
extern void property_palette_click_radar_icon(int radar_icon_idx,bool double_click);
extern void property_palette_click_singleplayer_option(int option_option_idx,bool double_click);
extern void property_palette_click_multiplayer_character(int multiplayer_character_idx,bool double_click);
extern void property_palette_click_multiplayer_game(int game_game_idx,bool double_click);
extern void property_palette_click_multiplayer_option(int option_option_idx,bool double_click);
extern void property_palette_click_menu(int menu_idx,bool double_click);
extern void property_palette_click_chooser(int chooser_idx,bool double_click);
extern void property_palette_click_sound(int sound_idx,bool double_click);
extern void property_palette_click_particle(int particle_idx,bool double_click);
extern void property_palette_click_particle_group(int particle_idx,bool double_click);
extern void property_palette_click_ring(int ring_idx,bool double_click);
extern void property_palette_click_halo(int halo_idx,bool double_click);
extern void property_palette_click_mark(int mark_idx,bool double_click);
extern void property_palette_click_crosshair(int crosshair_idx,bool double_click);
extern void property_palette_click_action(int action_idx,bool double_click);
extern void property_palette_click_shader(int shader_idx,bool double_click);

extern void property_palette_fill_menu_item(int menu_idx,int menu_item_idx);
extern void property_palette_fill_chooser_piece(int chooser_idx,int chooser_item_idx);
extern void property_palette_fill_group_particle(int particle_idx,int group_particle_idx);

extern void property_palette_click_menu_item(int menu_idx,int menu_item_idx,bool double_click);
extern void property_palette_click_chooser_piece(int chooser_idx,int chooser_item_idx,bool double_click);
extern void property_palette_click_group_particle(int particle_idx,int group_particle_idx,bool double_click);

//
// dialogs
//

extern void dialog_about_run(void);
extern bool dialog_file_open_run(char *title,char *search_path,char *extension,char *required_file_name,char *file_name);
