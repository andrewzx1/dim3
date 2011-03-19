/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: HUD Definitions Header

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
// HUD defines
//

#define max_iface_bitmap						256
#define max_iface_text							256
#define max_iface_bar							32
#define max_iface_menu							16
#define max_iface_chooser						128

#define max_iface_particle						128
#define max_iface_ring							64
#define max_iface_mark							128
#define max_iface_halo							32
#define max_iface_crosshair						32
#define max_iface_action						128
#define max_iface_sound							128
#define max_iface_user_shader					128

#define max_hud_text_str_sz						256
#define max_hud_intro_model						16
#define max_tint_color							8
#define max_character							32

#define max_net_bot								16
#define max_net_game							32
#define max_net_option							32

//
// text specials
//

#define text_special_none						0
#define text_special_fps						1
#define text_special_score						2
#define text_special_place						3
#define text_special_spread						4

#define text_special_list_def					{"none","fps","score","place","spread",""}

//
// console
//

#define max_console_line						14
#define max_console_txt_sz						128

typedef struct		{
						int						style;
						char					txt[max_console_txt_sz];
						d3col					color;
					} console_line_type;

//
// color and font settings
//

typedef struct		{
						d3col					dialog_background,dialog_base,dialog_dimmed,dialog_outline,
												control_label,control_text,control_fill,
												control_header,control_outline,
												control_mouse_over,control_hilite,control_disabled,
												button_fill,button_text,button_outline,
												default_tint,tints[max_tint_color];
					} iface_color_type;

typedef struct		{
						int						text_size_small,text_size_medium,text_size_large;
						char					interface_name[name_str_len],hud_name[name_str_len];
					} iface_font_type;

//
// generic animated image
//

typedef struct		{
						int								image_count,image_per_row,msec;
						bool							loop,loop_back;
					} iface_image_animation_type;


//
// bitmaps, text, bars
//

typedef struct		{
						int							start_tick,
													fade_in_tick,life_tick,fade_out_tick;
						bool						on;
					} iface_item_fade_type;
					
typedef struct		{
						int							x_add,y_add,col,row,count;
						bool						on;
					} iface_bitmap_repeat_type;
					
typedef struct		{
						int							x,y,x_size,y_size,image_idx,show_tick;
						float						alpha,rot;
						char						name[name_str_len],filename[file_str_len];
						bool						show,old_show,
													flash,flip_horz,flip_vert,team_tint;
						iface_bitmap_repeat_type	repeat;
						iface_image_animation_type	animate;
						iface_item_fade_type		fade;
					} iface_bitmap_type;

typedef struct		{
						int							nbitmap;
						iface_bitmap_type			*bitmaps;
					} iface_bitmap_list;

typedef struct		{
						int							x,y,size,just,special;
						float						alpha;
						char						name[name_str_len],data[max_hud_text_str_sz];
						bool						show,old_show,has_return;
						d3col						color;
						iface_item_fade_type		fade;
					} iface_text_type;

typedef struct		{
						int							ntext;
						iface_text_type				*texts;
					} iface_text_list;
					
typedef struct		{
						int							x,y,x_size,y_size;
						float						fill_alpha,outline_alpha,value;
						char						name[name_str_len];
						bool						show,old_show,outline,vert;
						d3col						fill_start_color,fill_end_color,outline_color;
					} iface_bar_type;

typedef struct		{
						int							nbar;
						iface_bar_type				*bars;
					} iface_bar_list;

//
// radar
//

#define max_radar_icon							16

typedef struct		{
						int						size,image_idx;
						char					name[name_str_len],bitmap_name[name_str_len];
						bool					rot;
					} iface_radar_icon_type;

typedef struct		{
						int						x,y,display_radius,view_radius,
												background_image_idx,nicon,no_motion_fade;
						bool					on,rot,team_tint;
						char					background_bitmap_name[name_str_len];
						iface_radar_icon_type	icons[max_radar_icon];
					} iface_radar_type;
			
//
// menus
//

#define max_menu_item							24
#define max_menu_item_data_sz					64

typedef struct		{
						int						item_id;
						char					data[max_menu_item_data_sz],sub_menu[name_str_len];
						bool					multiplayer_disable,quit;
					} iface_menu_item_type;
					
typedef struct		{
						int						nitem;
						char					name[name_str_len];
						iface_menu_item_type	items[max_menu_item];
					} iface_menu_type;

typedef struct		{
						int						nmenu;
						iface_menu_type			*menus;
					} iface_menu_list;
					
//
// chooser
//

#define max_chooser_piece						64
#define max_chooser_frame_text_sz				64
#define max_chooser_text_data_sz				256
#define max_chooser_button_text_sz				64
#define max_chooser_sub_txt						16

#define chooser_piece_type_text					0
#define chooser_piece_type_item					1
#define chooser_piece_type_model				2
#define chooser_piece_type_button				3

typedef struct		{
						int								size,just;
						char							str[max_chooser_text_data_sz];
					} iface_chooser_text_type;

typedef struct		{
						char							file[file_str_len];
					} iface_chooser_item_type;

typedef struct		{
						float							resize;
						char							model_name[name_str_len],animate_name[name_str_len];
						d3ang							rot;
					} iface_chooser_model_type;

typedef struct		{
						char							name[max_chooser_button_text_sz];
					} iface_chooser_button_type;

typedef union		{
						iface_chooser_text_type			text;
						iface_chooser_item_type			item;
						iface_chooser_model_type		model;
						iface_chooser_button_type		button;
					} iface_chooser_piece_data_type;

typedef struct		{
						int								type,id,
														x,y,wid,high;
						bool							clickable;
						char							goto_name[name_str_len];
						iface_chooser_piece_data_type	data;
					} iface_chooser_piece_type;

typedef struct		{
						int								x,y,wid,high;
						char							title[max_chooser_frame_text_sz];
						bool							on;
						d3col							background_col;
					} iface_chooser_frame_type;
					
typedef struct		{
						int								ok_id,cancel_id;
					} iface_chooser_key_type;
					
typedef struct		{
						int								npiece;
						char							name[name_str_len];
						iface_chooser_frame_type		frame;
						iface_chooser_key_type			key;
						iface_chooser_piece_type		pieces[max_chooser_piece];
					} iface_chooser_type;

typedef struct		{
						int								nchooser;
						iface_chooser_type				*choosers;
					} iface_chooser_list;

//
// progress
//

typedef struct		{
						int								lx,rx,ty,by,text_size;
						bool							outline;
						d3col							base_color_start,base_color_end,
														hilite_color_start,hilite_color_end,
														text_color,outline_color;
					} iface_progress_type;
					
//
// intro
//

#define max_simple_save_spot							5

typedef struct		{
						int								x,y,wid,high,element_id;
						bool							on;
					} iface_intro_button_type;

typedef struct		{
						int								x,y,text_size;
					} iface_intro_simple_save_desc_type;

typedef struct		{
						iface_intro_button_type				button_start,button_erase;
						iface_intro_simple_save_desc_type	desc;
					} iface_intro_simple_save_type;
			
typedef struct		{
						int								x,y;
						float							resize;
						char							model_name[name_str_len],animate_name[name_str_len];
						d3ang							rot;
					} iface_intro_model_type;

typedef struct		{
						int								nmodel;
						iface_intro_model_type			models[max_hud_intro_model];
					} iface_intro_model_list;

typedef struct		{
						char							music[name_str_len];
						iface_intro_button_type			button_game_new,button_game_load,button_game_setup,
														button_game_new_easy,button_game_new_medium,button_game_new_hard,button_game_new_cancel,
														button_multiplayer_host,button_multiplayer_join,button_multiplayer_setup,
														button_credit,button_quit;
						iface_intro_simple_save_type	simple_save[max_simple_save_spot];
						iface_intro_model_list			model;
					} iface_intro_type;

//
// hud fades
//

typedef struct		{
						int								title_msec,
														map_msec;
					} iface_fade_type;

//
// particles
//

#define max_particle_count								256
#define max_particle_trail								16
#define max_particle_variation							4
#define max_particle_group								16

typedef struct		{
						int								shift;
						char							name[name_str_len];
					} iface_particle_group_piece_type;

typedef struct		{
						int								count;
						bool							on;
						iface_particle_group_piece_type	particles[max_particle_group];
					} iface_particle_group_type;

typedef struct		{
						d3pnt							pt;
						d3vct							vct;
					} iface_particle_piece_type;

typedef struct		{
						int								image_idx,count,trail_count,
														spread_offset,life_msec,
														start_pixel_size,end_pixel_size,
														current_variation_idx;
						float							start_gravity,gravity_add,
														start_alpha,end_alpha,reduce_pixel_fact,
														trail_step,ambient_factor;
						char							name[name_str_len],bitmap_name[name_str_len];
						bool							reverse,blend_add,globe,team_tint;
						d3pnt							pt;
						d3ang							rot;
						d3vct							vct,rot_accel;
						d3col							start_color,end_color;
						iface_image_animation_type		animate;
						iface_particle_piece_type		pieces[max_particle_variation][max_particle_count];
						iface_particle_group_type		group;
					} iface_particle_type;

typedef struct		{
						int								nparticle;
						iface_particle_type				*particles;
					} iface_particle_list;

//
// rings
//

typedef struct		{
						int								image_idx,life_msec,
														start_outer_size,end_outer_size,
														start_inner_size,end_inner_size;
						float							start_alpha,end_alpha;
						char							name[name_str_len],bitmap_name[name_str_len];
						bool							blend_add,team_tint;
						d3ang							ang,rot;
						d3vct							vct,rot_accel;
						d3col							start_color,end_color;
						iface_image_animation_type		animate;
					} iface_ring_type;

typedef struct		{
						int								nring;
						iface_ring_type					*rings;
					} iface_ring_list;

//
// marks
//

typedef struct		{
						int								image_idx,
														fade_in_msec,life_msec,fade_out_msec,total_msec;
						char							name[name_str_len],bitmap_name[name_str_len];
						bool							no_rotate,no_transparent,no_opaque,hilite,blend_add,team_tint;
						d3col							color;
						iface_image_animation_type		animate;
					} iface_mark_type;

typedef struct		{
						int								nmark;
						iface_mark_type					*marks;
					} iface_mark_list;

//
// halos
//

typedef struct		{
						int								image_idx;
						char							name[name_str_len],bitmap_name[name_str_len];
					} iface_halo_type;

typedef struct		{
						int								nhalo;
						iface_halo_type					*halos;
					} iface_halo_list;

//
// crosshair
//

typedef struct		{
						int								image_idx;
						char							name[name_str_len],bitmap_name[name_str_len];
					} iface_crosshair_type;

typedef struct		{
						int								ncrosshair;
						iface_crosshair_type			*crosshairs;
					} iface_crosshair_list;
					
//
// actions
//

typedef struct		{
						char							display_name[name_str_len];
						bool							show;
					} iface_action_display_type;

typedef struct		{
						iface_action_display_type		*action_displays;
					} iface_action_display_list;

//
// sounds
//

typedef struct		{
						int								min_dist,max_dist;
						char							name[name_str_len],file_name[file_str_len];
					} iface_sound_type;

typedef struct		{
						int								nsound;
						iface_sound_type				*sounds;
					} iface_sound_list;

//
// shaders
//

typedef struct		{
						char							name[name_str_len],
														vert_name[file_str_len],
														frag_name[file_str_len];
					} iface_shader_type;

typedef struct		{
						int								nshader;
						iface_shader_type				*shaders;
					} iface_shader_list;

//
// simple save structures
//

typedef struct		{
						int								save_id;
						char							desc[64];
					} hud_simple_save_type;

typedef struct		{
						hud_simple_save_type			saves[max_simple_save_spot];
					} hud_simple_save_list;

//
// chat structures
//

typedef struct		{
						int								x,y,
														last_add_life_sec,next_life_sec;
					} iface_chat_type;
					
//
// network structures
//

typedef struct		{
						char							name[name_str_len],model_name[name_str_len],
														param[name_str_len];
						float							interface_resize;
						d3pnt							interface_offset;
					} hud_character_item_type;

typedef struct		{
						int								ncharacter;
						hud_character_item_type			characters[max_character];
					} hud_character_type;
	
typedef struct		{
						char							name[name_str_len];
					} hud_net_bot_type;

typedef struct		{
						bool							on;
						hud_net_bot_type				bots[max_net_bot];
					} hud_net_bots_type;

typedef struct		{
						char							name[name_str_len];
						bool							use_teams,monsters;
					} hud_net_game_type;

typedef struct		{
						int								ngame;
						hud_net_game_type				games[max_net_game];
					} hud_net_games_type;

typedef struct		{
						char							name[name_str_len],descript[64];
					} hud_net_option_type;

typedef struct		{
						int								noption;
						hud_net_option_type				options[max_net_option];
					} hud_net_options_type;
					
typedef struct		{
						int								port;
						char							host[64],url[256];
					} hud_net_news_type;

//
// main iface structure
//
 
typedef struct		{
						int								scale_x,scale_y;
						char							project_name[name_str_len],
														click_sound[name_str_len];
						bool							debug,skill;
						iface_color_type				color;
						iface_font_type					font;
						iface_progress_type				progress;
						iface_intro_type				intro;
						iface_fade_type					fade;
						iface_bitmap_list				bitmap_list;
						iface_text_list					text_list;
						iface_bar_list					bar_list;
						iface_menu_list					menu_list;
						iface_chooser_list				chooser_list;
						iface_particle_list				particle_list;
						iface_ring_list					ring_list;
						iface_mark_list					mark_list;
						iface_halo_list					halo_list;
						iface_crosshair_list			crosshair_list;
						iface_action_display_list		action_display_list;
						iface_sound_list				sound_list;
						iface_shader_list				shader_list;
						iface_radar_type				radar;
						iface_chat_type					chat;
						hud_character_type				character;
						hud_simple_save_list			simple_save_list;
						hud_net_bots_type				net_bot;
						hud_net_games_type				net_game;
						hud_net_options_type			net_option;
						hud_net_news_type				net_news;
					} iface_type;
					

