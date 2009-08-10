/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Functions Header

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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
// main script routines
//

extern bool scripts_engine_initialize(char *err_str);
extern void scripts_engine_shutdown(void);

extern void scripts_initialize(void);

extern int scripts_find_free(void);
extern int scripts_find_uid(int uid);

extern void script_add_roots(script_type *script);
extern void script_remove_roots(script_type *script);
extern void scripts_clean_up_roots(void);

extern void scripts_clear_last_error(void);
// supergumba -- JS
//extern void scripts_catch_errors(JSContextRef cx,const char *message,JSErrorReport *report);
extern void scripts_get_last_error(char *err_str);

extern void scripts_clear_attach_data(attach_type *attach);

extern bool scripts_execute(attach_type *attach,script_type *script,char *err_str);

extern bool scripts_add(attach_type *attach,char *sub_dir,char *name,char *params,char *err_str);
extern bool scripts_add_console(attach_type *attach,char *sub_dir,char *name,char *params,char *err_str);
extern void scripts_dispose(int uid);

extern void scripts_run(int tick);

extern char* script_parse_defines(char *data,int *len);
extern void script_load_user_defines(void);
extern void script_free_user_defines(void);

extern bool script_load_file(script_type *script,char *script_dir,char *script_name,char *err_str);
extern void script_free_file(script_type *script);

//
// conversions
//

extern JSValueRef script_null_to_value(void);
extern bool script_is_value_null(JSValueRef val);
extern int script_value_to_int(JSValueRef val);
extern JSValueRef script_int_to_value(int i);
extern float script_value_to_float(JSValueRef val);
extern JSValueRef script_float_to_value(float f);
extern bool script_value_to_bool(JSValueRef val);
extern JSValueRef script_bool_to_value(bool b);
extern void script_value_to_string(JSValueRef val,char *str,int len);
extern JSValueRef script_string_to_value(char *str);
extern JSValueRef script_int_array_to_value(int cnt,int *values);
extern JSValueRef script_angle_to_value(float x,float y,float z);
extern JSValueRef script_point_to_value(int x,int y,int z);
extern JSValueRef script_color_to_value(d3col *col);

//
// lookups
//

extern obj_type* script_find_obj_from_uid_arg(JSValueRef arg);
extern weapon_type* script_find_weapon_from_name_arg(obj_type *obj,JSValueRef arg);
extern spot_type* script_find_spot_from_idx_arg(JSValueRef arg);
extern spot_type* script_find_spot_from_name_type(JSValueRef arg_0,JSValueRef arg_1);
extern spot_type* script_find_network_spot(obj_type *obj);
extern node_type* script_find_node_from_idx_arg(JSValueRef arg);
extern map_light_type* script_find_light_from_name(JSValueRef arg);
extern hud_text_type* script_find_text_from_name(JSValueRef arg);
extern hud_bitmap_type* script_find_bitmap_from_name(JSValueRef arg);
extern hud_bar_type* script_find_bar_from_name(JSValueRef arg);
extern int script_find_group_from_name(JSValueRef arg);
extern int script_find_map_movement_from_name(JSValueRef arg);
extern int script_get_attached_object_uid(void);
extern model_draw* script_find_model_draw(void);

//
// initialize script objects
//

extern void script_init_global_map_object(void);
extern void script_init_map_action_object(void);
extern void script_init_map_fog_object(void);
extern void script_init_map_fog_color_object(void);
extern void script_init_map_group_object(void);
extern void script_init_map_info_object(void);
extern void script_init_map_light_color_object(void);
extern void script_init_map_movement_object(void);
extern void script_init_map_node_object(void);
extern void script_init_map_object_object(void);
extern void script_init_map_setting_object(void);
extern void script_init_map_spot_object(void);
extern void script_init_map_light_object(void);

extern void script_init_global_multiplayer_object(void);
extern void script_init_multiplayer_bot_object(void);
extern void script_init_multiplayer_score_object(void);
extern void script_init_multiplayer_setting_object(void);

extern void script_init_global_camera_object(void);
extern void script_init_camera_angle_object(void);
extern void script_init_camera_chase_object(void);
extern void script_init_camera_chase_angle_object(void);
extern void script_init_camera_chase_offset_object(void);
extern void script_init_camera_chase_slop_object(void);
extern void script_init_camera_plane_object(void);
extern void script_init_camera_position_object(void);
extern void script_init_camera_setting_object(void);
extern void script_init_camera_state_object(void);
extern void script_init_camera_static_position_object(void);

extern void script_init_global_interface_object(void);
extern void script_init_interface_bar_object(void);
extern void script_init_interface_bitmap_object(void);
extern void script_init_interface_console_object(void);
extern void script_init_interface_fade_object(void);
extern void script_init_interface_interaction_object(void);
extern void script_init_interface_radar_object(void);
extern void script_init_interface_screen_object(void);
extern void script_init_interface_text_object(void);

extern void script_init_global_data_object(void);

extern void script_init_global_sound_object(void);

extern void script_init_global_spawn_object(void);

extern void script_init_global_utility_object(void);
extern void script_init_utility_angle_object(void);
extern void script_init_utility_pack_object(void);
extern void script_init_utility_point_object(void);
extern void script_init_utility_random_object(void);

extern void script_init_game_setting_object(void);
extern void script_init_game_score_object(void);
extern void script_init_game_join_object(void);

extern void script_init_model_object(void);
extern void script_init_model_animation_object(void);
extern void script_init_model_bone_object(void);
extern void script_init_model_fill_object(void);
extern void script_init_model_halo_object(void);
extern void script_init_model_light_object(void);
extern void script_init_model_light_color_object(void);
extern void script_init_model_mesh_object(void);
extern void script_init_model_offset_object(void);
extern void script_init_model_rotate_object(void);
extern void script_init_model_shadow_object(void);
extern void script_init_model_spin_object(void);

extern void script_init_event_object(void);

extern void script_init_obj_setting_object(void);
extern void script_init_obj_position_object(void);
extern void script_init_obj_angle_object(void);
extern void script_init_obj_lock_object(void);
extern void script_init_obj_status_object(void);
extern void script_init_obj_motion_vector_object(void);
extern void script_init_obj_motion_angle_object(void);
extern void script_init_obj_forward_speed_object(void);
extern void script_init_obj_side_speed_object(void);
extern void script_init_obj_vert_speed_object(void);
extern void script_init_obj_turn_speed_object(void);
extern void script_init_obj_object_speed_object(void);
extern void script_init_obj_thrust_object(void);
extern void script_init_obj_look_object(void);
extern void script_init_obj_sight_object(void);
extern void script_init_obj_health_object(void);
extern void script_init_obj_size_object(void);
extern void script_init_obj_rigid_body_object(void);
extern void script_init_obj_vehicle_object(void);
extern void script_init_obj_radar_object(void);
extern void script_init_obj_score_object(void);
extern void script_init_obj_touch_object(void);
extern void script_init_obj_touch_position_object(void);
extern void script_init_obj_touch_angle_object(void);
extern void script_init_obj_hit_object(void);
extern void script_init_obj_hit_position_object(void);
extern void script_init_obj_hit_angle_object(void);
extern void script_init_obj_click_object(void);
extern void script_init_obj_held_object(void);
extern void script_init_obj_pickup_object(void);
extern void script_init_obj_watch_object(void);
extern void script_init_obj_weapon_object(void);
extern void script_init_obj_weapon_fire_object(void);
extern void script_init_obj_melee_object(void);

extern void script_init_weap_setting_object(void);
extern void script_init_weap_hand_object(void);
extern void script_init_weap_hand_position_object(void);
extern void script_init_weap_hand_angle_object(void);
extern void script_init_weap_projectile_object(void);
extern void script_init_weap_crosshair_object(void);
extern void script_init_weap_crosshair_color_object(void);
extern void script_init_weap_crosshair_empty_color_object(void);
extern void script_init_weap_crosshair_pickup_color_object(void);
extern void script_init_weap_ammo_object(void);
extern void script_init_weap_alt_ammo_object(void);
extern void script_init_weap_kickback_object(void);
extern void script_init_weap_recoil_object(void);
extern void script_init_weap_melee_object(void);
extern void script_init_weap_fire_object(void);
extern void script_init_weap_dual_object(void);
extern void script_init_weap_target_object(void);
extern void script_init_weap_target_color_object(void);
extern void script_init_weap_zoom_object(void);

extern void script_init_proj_setting_object(void);
extern void script_init_proj_position_object(void);
extern void script_init_proj_origin_object(void);
extern void script_init_proj_motion_vector_object(void);
extern void script_init_proj_motion_angle_object(void);
extern void script_init_proj_speed_object(void);
extern void script_init_proj_hit_object(void);
extern void script_init_proj_action_object(void);
extern void script_init_proj_push_object(void);
extern void script_init_proj_size_object(void);
extern void script_init_proj_mark_object(void);
extern void script_init_proj_melee_object(void);

//
// free script objects
//

extern void script_free_global_map_object(void);
extern void script_free_map_action_object(void);
extern void script_free_map_fog_object(void);
extern void script_free_map_fog_color_object(void);
extern void script_free_map_group_object(void);
extern void script_free_map_info_object(void);
extern void script_free_map_light_color_object(void);
extern void script_free_map_movement_object(void);
extern void script_free_map_node_object(void);
extern void script_free_map_object_object(void);
extern void script_free_map_setting_object(void);
extern void script_free_map_spot_object(void);
extern void script_free_map_light_object(void);

extern void script_free_global_multiplayer_object(void);
extern void script_free_multiplayer_bot_object(void);
extern void script_free_multiplayer_score_object(void);
extern void script_free_multiplayer_setting_object(void);

extern void script_free_global_camera_object(void);
extern void script_free_camera_angle_object(void);
extern void script_free_camera_chase_object(void);
extern void script_free_camera_chase_angle_object(void);
extern void script_free_camera_chase_offset_object(void);
extern void script_free_camera_chase_slop_object(void);
extern void script_free_camera_plane_object(void);
extern void script_free_camera_position_object(void);
extern void script_free_camera_setting_object(void);
extern void script_free_camera_state_object(void);
extern void script_free_camera_static_position_object(void);

extern void script_free_global_interface_object(void);
extern void script_free_interface_bar_object(void);
extern void script_free_interface_bitmap_object(void);
extern void script_free_interface_console_object(void);
extern void script_free_interface_fade_object(void);
extern void script_free_interface_interaction_object(void);
extern void script_free_interface_radar_object(void);
extern void script_free_interface_screen_object(void);
extern void script_free_interface_text_object(void);

extern void script_free_global_data_object(void);

extern void script_free_global_sound_object(void);

extern void script_free_global_spawn_object(void);

extern void script_free_global_utility_object(void);
extern void script_free_utility_angle_object(void);
extern void script_free_utility_pack_object(void);
extern void script_free_utility_point_object(void);
extern void script_free_utility_random_object(void);

extern void script_free_game_setting_object(void);
extern void script_free_game_score_object(void);
extern void script_free_game_join_object(void);

extern void script_free_model_object(void);
extern void script_free_model_animation_object(void);
extern void script_free_model_bone_object(void);
extern void script_free_model_fill_object(void);
extern void script_free_model_halo_object(void);
extern void script_free_model_light_object(void);
extern void script_free_model_light_color_object(void);
extern void script_free_model_mesh_object(void);
extern void script_free_model_offset_object(void);
extern void script_free_model_rotate_object(void);
extern void script_free_model_shadow_object(void);
extern void script_free_model_spin_object(void);

extern void script_free_event_object(void);

extern void script_free_obj_setting_object(void);
extern void script_free_obj_position_object(void);
extern void script_free_obj_angle_object(void);
extern void script_free_obj_lock_object(void);
extern void script_free_obj_status_object(void);
extern void script_free_obj_motion_vector_object(void);
extern void script_free_obj_motion_angle_object(void);
extern void script_free_obj_forward_speed_object(void);
extern void script_free_obj_side_speed_object(void);
extern void script_free_obj_vert_speed_object(void);
extern void script_free_obj_turn_speed_object(void);
extern void script_free_obj_object_speed_object(void);
extern void script_free_obj_thrust_object(void);
extern void script_free_obj_look_object(void);
extern void script_free_obj_sight_object(void);
extern void script_free_obj_health_object(void);
extern void script_free_obj_size_object(void);
extern void script_free_obj_rigid_body_object(void);
extern void script_free_obj_vehicle_object(void);
extern void script_free_obj_radar_object(void);
extern void script_free_obj_score_object(void);
extern void script_free_obj_touch_object(void);
extern void script_free_obj_touch_position_object(void);
extern void script_free_obj_touch_angle_object(void);
extern void script_free_obj_hit_object(void);
extern void script_free_obj_hit_position_object(void);
extern void script_free_obj_hit_angle_object(void);
extern void script_free_obj_click_object(void);
extern void script_free_obj_held_object(void);
extern void script_free_obj_pickup_object(void);
extern void script_free_obj_watch_object(void);
extern void script_free_obj_weapon_object(void);
extern void script_free_obj_weapon_fire_object(void);
extern void script_free_obj_melee_object(void);

extern void script_free_weap_setting_object(void);
extern void script_free_weap_hand_object(void);
extern void script_free_weap_hand_position_object(void);
extern void script_free_weap_hand_angle_object(void);
extern void script_free_weap_projectile_object(void);
extern void script_free_weap_crosshair_object(void);
extern void script_free_weap_crosshair_color_object(void);
extern void script_free_weap_crosshair_empty_color_object(void);
extern void script_free_weap_crosshair_pickup_color_object(void);
extern void script_free_weap_ammo_object(void);
extern void script_free_weap_alt_ammo_object(void);
extern void script_free_weap_kickback_object(void);
extern void script_free_weap_recoil_object(void);
extern void script_free_weap_melee_object(void);
extern void script_free_weap_fire_object(void);
extern void script_free_weap_dual_object(void);
extern void script_free_weap_target_object(void);
extern void script_free_weap_target_color_object(void);
extern void script_free_weap_zoom_object(void);

extern void script_free_proj_setting_object(void);
extern void script_free_proj_position_object(void);
extern void script_free_proj_origin_object(void);
extern void script_free_proj_motion_vector_object(void);
extern void script_free_proj_motion_angle_object(void);
extern void script_free_proj_speed_object(void);
extern void script_free_proj_hit_object(void);
extern void script_free_proj_action_object(void);
extern void script_free_proj_push_object(void);
extern void script_free_proj_size_object(void);
extern void script_free_proj_mark_object(void);
extern void script_free_proj_melee_object(void);

//
// add script objects
//

extern JSObjectRef script_add_global_map_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_map_action_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_map_fog_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_map_fog_color_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_map_group_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_map_info_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_map_light_color_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_map_movement_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_map_node_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_map_object_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_map_setting_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_map_spot_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_map_light_object(JSObjectRef parent_obj);

extern JSObjectRef script_add_global_multiplayer_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_multiplayer_bot_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_multiplayer_score_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_multiplayer_setting_object(JSObjectRef parent_obj);

extern JSObjectRef script_add_global_camera_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_camera_angle_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_camera_chase_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_camera_chase_angle_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_camera_chase_offset_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_camera_chase_slop_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_camera_plane_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_camera_position_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_camera_setting_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_camera_state_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_camera_static_position_object(JSObjectRef parent_obj);

extern JSObjectRef script_add_global_interface_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_interface_bar_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_interface_bitmap_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_interface_console_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_interface_fade_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_interface_interaction_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_interface_radar_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_interface_screen_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_interface_text_object(JSObjectRef parent_obj);

extern JSObjectRef script_add_global_data_object(JSObjectRef parent_obj);

extern JSObjectRef script_add_global_sound_object(JSObjectRef parent_obj);

extern JSObjectRef script_add_global_spawn_object(JSObjectRef parent_obj);

extern JSObjectRef script_add_global_utility_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_utility_angle_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_utility_pack_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_utility_point_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_utility_random_object(JSObjectRef parent_obj);

extern JSObjectRef script_add_game_setting_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_game_score_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_game_join_object(JSObjectRef parent_obj);

extern JSObjectRef script_add_model_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_model_animation_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_model_bone_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_model_fill_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_model_halo_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_model_light_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_model_light_color_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_model_mesh_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_model_offset_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_model_rotate_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_model_shadow_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_model_spin_object(JSObjectRef parent_obj);

extern JSObjectRef script_add_event_object(JSObjectRef parent_obj);

extern JSObjectRef script_add_obj_setting_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_position_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_angle_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_lock_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_status_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_motion_vector_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_motion_angle_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_forward_speed_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_side_speed_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_vert_speed_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_turn_speed_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_object_speed_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_thrust_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_look_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_sight_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_health_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_size_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_rigid_body_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_vehicle_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_radar_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_score_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_touch_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_touch_position_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_touch_angle_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_hit_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_hit_position_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_hit_angle_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_click_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_held_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_pickup_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_watch_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_weapon_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_weapon_fire_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_obj_melee_object(JSObjectRef parent_obj);

extern JSObjectRef script_add_weap_setting_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_hand_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_hand_position_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_hand_angle_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_projectile_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_crosshair_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_crosshair_color_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_crosshair_empty_color_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_crosshair_pickup_color_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_ammo_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_alt_ammo_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_kickback_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_recoil_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_melee_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_fire_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_dual_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_target_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_target_color_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_weap_zoom_object(JSObjectRef parent_obj);

extern JSObjectRef script_add_proj_setting_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_proj_position_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_proj_origin_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_proj_motion_vector_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_proj_motion_angle_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_proj_speed_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_proj_hit_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_proj_action_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_proj_push_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_proj_size_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_proj_mark_object(JSObjectRef parent_obj);
extern JSObjectRef script_add_proj_melee_object(JSObjectRef parent_obj);

//
// event posting, chaining
//

extern void scripts_setup_events(int script_uid);
extern bool scripts_post_event(attach_type *attach,int main_event,int sub_event,int id,char *err_str);
extern void scripts_post_event_console(attach_type *attach,int main_event,int sub_event,int id);
extern bool scripts_chain(attach_type *attach,char *func_name,char *err_str);
extern void scripts_chain_console(attach_type *attach,char *func_name);
extern bool scripts_direct_call(attach_type *attach,char *func_name,int arg_count,JSValueRef *args,JSValueRef *rval);

extern void script_initialize_classes(void);
extern void script_release_classes(void);
extern JSClassRef script_create_class(const char *name,JSObjectGetPropertyCallback getter,JSObjectSetPropertyCallback setter);
extern void script_free_class(JSClassRef cls);
extern bool script_add_global_object(script_type *script,char *err_str);
extern JSObjectRef script_create_main_object(attach_type *attach);
extern JSObjectRef script_create_child_object(JSObjectRef parent_obj,JSClassRef cls,char *name,script_js_property *props,script_js_function *funcs);
extern JSValueRef script_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,script_js_property *props);
extern bool script_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,script_js_property *props);

extern void script_globals_initialize(void);
extern int script_find_global(char *name,int script_uid);
extern void script_set_global_by_index(int idx,JSValueRef val);
extern bool script_set_global(char *name,int script_uid,JSValueRef val);
extern JSValueRef script_get_global(char *name,int script_uid);
extern bool script_add_global(char *name,int script_uid,JSValueRef val);
extern void script_delete_global(char *name,int script_uid);

extern void script_state_save(void);
extern void script_state_load(void);

extern void timers_initialize(void);
extern int timers_find(attach_type *attach,int mode);
extern bool timers_add(attach_type *attach,int freq,int user_id,char *chain_func_name,int mode);
extern void timers_remove(int idx);
extern void timers_clear(attach_type *attach,int mode);
extern void timers_script_dispose(int script_uid);
extern void timers_run(void);
