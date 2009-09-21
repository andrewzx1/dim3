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

#define htonf(x)									(htonl((int)((x)*1000.0f)))
#define ntohf(x)									(((float)((signed int)ntohl(x)))/1000.0f)

//
// networking ports
//

#define net_port_host								11800
#define net_port_host_query							11801
#define net_port_host_broadcast						11802
#define net_port_host_broadcast_reply				11803

//
// low-level socket timing
//

#define socket_no_data_retry						10
#define socket_no_data_u_wait						25

//
// host defines
//

#define host_no_data_u_wait							25
#define host_max_network_error_reject				10

#define host_max_remote_count						24

#define host_client_timeout_msec_rate				10000

//
// client defines
//

#define client_timeout_wait_seconds					5
#define client_message_per_loop_count				5

#define client_communication_update_msec_rate		50
#define client_communication_group_synch_msec_rate	10000
#define client_communication_latency_ping_msec_rate	5000

#define client_communication_timeout_msec			1000

//
// queue defines
//

#define net_queue_max_data_size						10240
#define net_queue_chunk_read_size					1024

//
// read queues
//

typedef struct		{
						int							len;
						unsigned char				*data;
						SDL_mutex					*lock;
					} net_queue_type;

//
// network headers, messages, and tags
//
// the tag is used to make sure that we are really into a dim3 message block
// to work around any garbles or corruptions in the transmission
//

#define net_header_tag_size							8
#define net_header_tag								"dim3blck"

#define net_max_msg_size							2048

typedef struct		{
						short						len,action,from_remote_uid;
					} network_header;

//
// host player list
//

typedef struct		{
						d3socket					sock;
						int							remote_uid,team_idx,
													tint_color_idx,character_idx,
													score;
						char						name[name_str_len];
						bool						ready,bot;
						d3pnt						pnt;
					} net_host_player_type;

//
// setup structures
//

typedef struct		{
						char						name[name_str_len],
													ip_name[256],ip_resolve[64],
													map_name[name_str_len];
						bool						hosting;
					} network_setup_host_type;

typedef struct		{
						int							latency,latency_ping_tick;
						char						joined_ip[32],game_name2[name_str_len];
						bool						joined;
					} network_setup_client_type;

typedef struct		{
						int							game_idx,option_flags;
						network_setup_host_type		host;
						network_setup_client_type	client;
					} network_setup_type;

//
// special remote unqiue IDs
//

#define net_remote_uid_host							0
#define net_remote_uid_none							1
#define net_remote_uid_client_start					1000
#define net_remote_uid_monster_start				2000

//
// messages actions
//

#define net_action_none								0
#define net_action_request_info						1
#define net_action_reply_info						2
#define net_action_request_join						3
#define net_action_reply_join						4
#define net_action_request_game_reset				5
#define net_action_request_ready					6
#define net_action_request_team						7
#define net_action_request_leave					8
#define net_action_request_remote_add				9
#define net_action_request_remote_remove			10
#define net_action_request_remote_update			11
#define net_action_request_remote_death				12
#define net_action_request_remote_telefrag			13
#define net_action_request_remote_chat				14
#define net_action_request_remote_sound				15
#define net_action_request_remote_fire				16
#define net_action_request_remote_pickup			17
#define net_action_request_remote_click				18
#define net_action_request_latency_ping				19
#define net_action_reply_latency_ping				20
#define net_action_request_host_exit				21
#define net_action_request_group_synch				22
#define net_action_reply_group_synch				23
#define net_action_request_game_score_limit			24

//
// remote fire types
//

#define net_remote_fire_type_projectile				0
#define net_remote_fire_type_hit_scan				1
#define net_remote_fire_type_melee					2

//
// network maximums
//

#define net_max_weapon_per_remote					24

//
// team definitions
//

#define net_team_none								0
#define net_team_red								1
#define net_team_blue								2

//
// flags
//

#define net_update_flag_hidden						0x00000001
#define net_update_flag_no_contact_object			0x00000002
#define net_update_flag_no_contact_projectile		0x00000004
#define net_update_flag_no_contact_force			0x00000008
#define net_update_flag_talking						0x00000010

#define net_group_synch_flag_on						0x00000001
#define net_group_synch_flag_freeze					0x00000002
#define net_group_synch_flag_main_move				0x00000004

//
// join remotes and bots
//

typedef struct		{
						int							pnt_x,pnt_y,pnt_z;
						short						uid,bot,score,
													team_idx,tint_color_idx,character_idx;
						char						name[name_str_len];
					} network_request_object_add;

typedef struct		{
						short						count;
						network_request_object_add	objects[host_max_remote_count];
					} network_reply_join_remotes;

//
// joining messages
//

typedef struct		{
						short						player_count,player_max_count;
						char						host_name[name_str_len],host_ip_resolve[16],
													proj_name[name_str_len],
													game_name[name_str_len],
													map_name[name_str_len];
					} network_reply_info;
					
typedef struct		{
						int							hash;
						short						tint_color_idx,character_idx;
						char						name[name_str_len],vers[name_str_len];
					} network_request_join;

typedef struct		{
						int							map_tick,option_flags;
						short						join_uid,remote_count,bot_count;
						char						game_name[name_str_len],map_name[name_str_len],
													deny_reason[64];
						network_reply_join_remotes	remotes;
					} network_reply_join;
	
typedef struct		{
						short						team_idx;
					} network_request_team;

//
// in game messages
//
				
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
														vehicle_map_spawn_idx,
														score,health;
						unsigned char					model_cur_texture_frame[max_model_texture];
						network_request_animation		animation[max_model_blend_animation];
						network_request_dynamic_bone	dynamic_bones[max_model_dynamic_bone];
					} network_request_remote_update;

typedef struct		{
						short							kill_remote_uid,telefrag;
					} network_request_remote_death;
					
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
