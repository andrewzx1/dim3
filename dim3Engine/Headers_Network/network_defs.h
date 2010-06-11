/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Network Definitions Header

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
// additional network->host conversions
//

#define htonf(x)										(htonl((int)((x)*1000.0f)))
#define ntohf(x)										(((float)((signed int)ntohl(x)))/1000.0f)

//
// networking ports
//

#define net_port_host									11800
#define net_port_host_query								11801
#define net_port_host_broadcast							11802
#define net_port_host_broadcast_reply					11803

//
// low-level socket timing
//

#define socket_no_data_retry							10
#define socket_no_data_u_wait							25

//
// host defines
//

#define host_no_data_u_wait								25
#define host_max_network_error_reject					10

#define host_max_remote_count							24

#define host_client_timeout_msec_rate					10000

//
// client defines
//

#define client_timeout_wait_seconds						5
#define client_message_per_loop_count					5

#define client_communication_update_msec_rate			50
#define client_communication_group_synch_msec_rate		10000
#define client_communication_latency_ping_msec_rate		5000

#define client_communication_timeout_msec				1000

//
// network modes
//

#define net_mode_none									0
#define net_mode_client									1
#define net_mode_host									2
#define net_mode_host_dedicated							3

//
// queue defines
//

#define net_queue_max_message							64

//
// network queues
//

typedef struct		{
						int								count;
						unsigned char					*data;
						SDL_mutex						*lock;
					} net_queue_type;

//
// host player list
//

typedef struct		{
						int								machine_uid,
														remote_uid,
														port;
						unsigned long					ip_addr;
						bool							local,bot,ready;
						d3socket						sock;
					} net_host_player_connect_type;

typedef struct		{
						int								team_idx,tint_color_idx,score;
						char							name[name_str_len],
														draw_name[name_str_len];
						d3pnt							pnt;
						net_host_player_connect_type	connect;
						net_queue_type					queue;
					} net_host_player_type;

//
// network setup structures
//

typedef struct		{
						int								current_map_idx;
						char							name[name_str_len],
														ip_name[256],ip_resolve[64];
					} network_setup_host_type;

typedef struct		{
						int								latency,latency_ping_tick;
						unsigned long					host_ip_addr;
					} network_setup_client_type;

typedef struct		{
						int								machine_uid,
														next_machine_uid,
														next_remote_uid;
					} network_setup_uid;

typedef struct		{
						int								mode,game_idx,option_flags;
						network_setup_host_type			host;
						network_setup_client_type		client;
						network_setup_uid				uid;
					} network_setup_type;

//
// special remote unqiue IDs
//

#define net_player_uid_none								0
#define net_player_uid_host								1
#define net_player_uid_client_start						1000
#define net_player_uid_map_bot_start					2000

//
// messages actions
//

#define net_action_none									0
#define net_action_request_info							1
#define net_action_reply_info							2
#define net_action_request_join							3
#define net_action_reply_join							4
#define net_action_request_game_reset					5
#define net_action_request_ready						6
#define net_action_request_leave						7
#define net_action_request_remote_add					8
#define net_action_request_remote_remove				9
#define net_action_request_remote_death					10
#define net_action_request_remote_update				11
#define net_action_request_remote_telefrag				12
#define net_action_request_remote_chat					13
#define net_action_request_remote_sound					14
#define net_action_request_remote_fire					15
#define net_action_request_remote_pickup				16
#define net_action_request_remote_click					17
#define net_action_request_latency_ping					18
#define net_action_reply_latency_ping					19
#define net_action_request_host_exit					20
#define net_action_request_group_synch					21
#define net_action_reply_group_synch					22
#define net_action_request_game_score_limit				23

//
// remote fire types
//

#define net_remote_fire_type_projectile					0
#define net_remote_fire_type_hit_scan					1
#define net_remote_fire_type_melee						2

//
// network maximums
//

#define net_max_weapon_per_remote						24

//
// team definitions
//

#define net_team_none									0
#define net_team_red									1
#define net_team_blue									2

//
// flags
//

#define net_update_flag_hidden							0x00000001
#define net_update_flag_no_contact_object				0x00000002
#define net_update_flag_no_contact_projectile			0x00000004
#define net_update_flag_no_contact_force				0x00000008
#define net_update_flag_talking							0x00000010

#define net_group_synch_flag_on							0x00000001
#define net_group_synch_flag_freeze						0x00000002
#define net_group_synch_flag_main_move					0x00000004

//
// network headers and messages
//

#define net_max_msg_size								2048

typedef struct		{
						short							remote_uid,
														action,len;
					} network_header;

//
// info messages
//

typedef struct		{
						short							bot,score;
						char							name[name_str_len];
					} network_reply_info_player;

typedef struct		{
						short							count,max_count;
						network_reply_info_player		players[host_max_remote_count];
					} network_reply_info_player_list;

typedef struct		{
						int								option_flags;
						char							host_name[name_str_len],host_ip_resolve[16],
														proj_name[name_str_len],
														game_name[name_str_len],
														map_name[name_str_len];
						network_reply_info_player_list	player_list;
					} network_reply_info;

//
// join remotes and bots
//

typedef struct		{
						int								pnt_x,pnt_y,pnt_z;
						short							remote_uid,bot,score,
														team_idx,tint_color_idx;
						char							name[name_str_len],
														draw_name[name_str_len];
					} network_reply_join_remote;

typedef struct		{
						short							count;
						network_reply_join_remote		remotes[host_max_remote_count];
					} network_reply_join_remote_list;

//
// joining messages
//

typedef struct		{
						int								hash;
						short							tint_color_idx;
						char							name[name_str_len],
														draw_name[name_str_len],
														vers[name_str_len];
					} network_request_join;

typedef struct		{
						int								map_tick;
						short							machine_uid,remote_uid,
														team_idx;
						char							deny_reason[64];
						network_reply_join_remote_list	remote_list;
					} network_reply_join;

//
// game reset messages
//

typedef struct		{
						char							map_name[name_str_len];
					} network_request_game_reset;

//
// in game messages
//
				
typedef struct		{
						short							remote_killer_obj_uid,telefrag;
					} network_request_remote_death;

typedef struct		{
						int								model_tick;
						short							model_mode,
														model_animate_idx,model_animate_next_idx,model_pose_move_idx,
														model_smooth_animate_idx,model_smooth_pose_move_idx;
					} network_request_animation;			// used as part of network_request_update

typedef struct		{
						int								fp_mov_x,fp_mov_y,fp_mov_z,
														fp_rot_x,fp_rot_y,fp_rot_z,
														fp_resize;
						short							bone_idx;
					} network_request_dynamic_bone;			// used as part of network_request_update

typedef struct		{
						int								flags,pnt_x,pnt_y,pnt_z,
														fp_ang_x,fp_ang_y,fp_ang_z,
														fp_predict_move_x,fp_predict_move_y,fp_predict_move_z,
														fp_predict_turn_y,model_mesh_mask;
						short							offset_x,offset_y,offset_z,
														score,health;
						unsigned char					model_cur_texture_frame[max_model_texture];
						network_request_animation		animation[max_model_blend_animation];
						network_request_dynamic_bone	dynamic_bones[max_model_dynamic_bone];
					} network_request_remote_update;
					
typedef struct		{
						char							str[64];
					} network_request_remote_chat;
					
typedef struct		{
						int								pnt_x,pnt_y,pnt_z,fp_pitch;
						char							name[name_str_len];
					} network_request_remote_sound;

typedef struct		{
						int								pt_x,pt_y,pt_z,
														fp_ang_x,fp_ang_y,fp_ang_z;
						short							fire_type,radius,distance,damage,force;
						char							weap_name[name_str_len],proj_setup_name[name_str_len];
					} network_request_remote_fire;

typedef struct		{
						short							hidden,ammo_count,clip_count,
														alt_ammo_count,alt_clip_count;
					} network_request_remote_ammo;

typedef struct		{
						int								pt_x,pt_y,pt_z;
						short							health;
						network_request_remote_ammo		ammos[net_max_weapon_per_remote];
					} network_request_remote_pickup;

typedef struct		{
						int								pt_x,pt_y,pt_z,
														fp_ang_x,fp_ang_y,fp_ang_z;
					} network_request_remote_click;

//
// map synch messages
//

typedef struct		{
						int								flags,
														mov_add_x,mov_add_y,mov_add_z,
														cuml_mov_add_x,cuml_mov_add_y,cuml_mov_add_z,
														fp_rot_add_x,fp_rot_add_y,fp_rot_add_z,
														fp_cuml_rot_add_x,fp_cuml_rot_add_y,fp_cuml_rot_add_z;
						short							group_idx,movement_idx,movement_move_idx,
														count,user_id;
					} network_reply_group_synch;
