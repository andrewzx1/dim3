/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Main Defines for Setup

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
// Menus
//

#define app_menu_apple							128
#define app_menu_file							129

#define kCommandAbout							FOUR_CHAR_CODE('abot')

#define kCommandFileSave						FOUR_CHAR_CODE('save')
#define kCommandFileQuit						FOUR_CHAR_CODE('quit')

//
// Item types
//

#define item_interface							0
#define item_interface_project					1
#define item_interface_color					2
#define item_interface_device					3
#define item_interface_logo						4
#define item_interface_title_page				5
#define item_interface_singleplayer				6
#define item_interface_multiplayer				7
#define item_interface_setup					8
#define item_interface_progress					9
#define item_interface_hud						10
#define item_interface_virtual_control			11
#define item_interface_radar					12
#define item_interface_label					13
#define item_interface_menu						14
#define item_interface_chooser					15
#define item_interface_sound					16
#define item_interface_particle					17
#define item_interface_ring						18
#define item_interface_halo						19
#define item_interface_mark						20
#define item_interface_crosshair				21
#define item_interface_action					22
#define item_interface_shader					23
#define item_interface_preload_models			24

//
// intro button types
//

#define item_intro_button_game_new				0
#define item_intro_button_game_load				1
#define item_intro_button_game_setup			2
#define item_intro_button_multiplayer_host		3
#define item_intro_button_multiplayer_join		4
#define item_intro_button_credit				5
#define item_intro_button_quit					6

#define item_intro_button_simple_save_start		10
#define item_intro_button_simple_save_erase		20

//
// setup state
//

typedef struct		{
						int						cur_item,cur_device,
												cur_intro_button_idx,cur_intro_model_idx,
												cur_hud_bitmap_idx,cur_hud_text_idx,cur_hud_bar_idx,
												cur_virtual_control_stick_idx,cur_virtual_control_button_idx,
												cur_radar_icon_idx,cur_singleplayer_option_idx,cur_multiplayer_character_idx,
												cur_menu_idx,cur_menu_item_idx,cur_chooser_idx,cur_chooser_piece_idx,
												cur_multiplayer_game_idx,cur_multiplayer_option_idx,
												cur_sound_idx,cur_particle_idx,cur_group_particle_idx,cur_ring_idx,
												cur_halo_idx,cur_mark_idx,cur_crosshair_idx,
												cur_action_idx,cur_shader_idx;
					} setup_state_type;


