/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Server Definitions Header

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
// game states
//

#define gs_running								0
#define gs_intro								1
#define gs_setup_game							2
#define gs_setup_network						3
#define gs_join									4
#define gs_host									5
#define gs_file									6
#define gs_chooser								7
#define gs_title								8
#define gs_story								9
#define gs_movie								10
#define gs_error								11
#define gs_score_limit							12

//
// lists
//

#define max_model_list							128

#define max_obj_list							1024
#define max_weap_list							16
#define max_proj_setup_list						8

#define max_proj_list							256

#define max_effect_list							512
#define max_decal_list							128

#define max_particle_list						128
#define max_ring_list							64

#define max_halo_list							32
#define max_mark_list							128
#define max_crosshair_list						32

//
// maximums
//

#define max_particle_count						256
#define max_particle_trail						16
#define max_particle_variation					4
#define max_particle_group						16

#define max_rain_density						5120

#define max_object_bit_mask						32

#define max_model_light_cache_index				8

//
// binding types
//

#define bt_game									0
#define bt_map									1

//
// object types
//

#define object_type_player						0
#define object_type_remote						1
#define object_type_bot_multiplayer				2
#define object_type_bot_map						3
#define object_type_object						4

//
// input modes
//

#define im_fpp									0
#define im_side_scroll							1
#define im_top_down								2
#define im_fly									3
#define im_thrust								4

//
// effect types
//

#define ef_flash								0
#define ef_particle								1
#define ef_ring									2
#define ef_lightning							3
#define ef_ray									4
#define ef_shake								5

//
// weapon modes
//

#define wm_held									0
#define wm_lower								1
#define wm_raise								2

//
// weapon crosshair types
//

#define ct_center								0
#define ct_bone_tracking						1
#define ct_bone_tracking_resizing				2
#define ct_barrel_tracking						3
#define ct_barrel_tracking_resizing				4

//
// ducking modes
//

#define dm_stand								0
#define dm_duck									1
#define dm_stand_up								2
#define dm_duck_down							3

//
// air modes
//

#define am_up									0
#define am_down									1
#define am_falling								2
#define am_ground								3

//
// liquid modes
//

#define lm_out									0
#define lm_in									1
#define lm_float								2
#define lm_under								3

//
// animation modes
//

#define am_stopped								0
#define am_playing								1
#define am_resting								2

//
// auto walk modes
//

#define aw_none									0
#define aw_node									1
#define aw_object								2
#define aw_object_turn_only						3
#define aw_position								4

//
// shadow settings
//

#define max_shadow_poly							64

//
// node walking
//

#define node_slop_speed_factor					2.5f				// how close to a node do we have to be (compared with speed) to be a hit
#define node_pause_max_angle					45.0f				// angle needed to get out of a pause to turn
#define node_pause_min_angle					25.0f				// angle needed to get out of a pause when already paused

//
// zoom modes
//

#define zoom_mode_off							0
#define zoom_mode_in							1
#define zoom_mode_on							2
#define zoom_mode_out							3

//
// ray collision values
//

#define collide_obj_ray_half_sweep				40.0f
#define collide_obj_ray_spindle_count			5
#define collide_obj_ray_count					(collide_obj_ray_spindle_count*5)
#define collide_obj_ray_spindle_size			((collide_obj_ray_half_sweep*2)/(collide_obj_ray_spindle_count-1))

//
// misc settings
//

#define pd_parent_grace							100					// how long till projectile can strike parent again
#define ci_object_click_angle					20.0f				// angle at which you can click an object

#define liquid_float_slop						0.1f				// how much object height is in float area

//
// dynamic bones
//

#define max_model_dynamic_bone					4

//
// models
//
 
typedef struct		{
						int						tick,mode,
												animate_idx,animate_next_idx,pose_move_idx,
												smooth_animate_idx,smooth_pose_move_idx;
					} model_draw_animation;

typedef struct		{
						int						start_tick,tick;
						float					start_alpha,end_alpha;
						bool					on;
					} model_draw_fade;

typedef struct		{
						int						type,direction,intensity;
						float					exponent;
						bool					on;
						d3col					col;
					} model_draw_light;

typedef struct		{
						int						idx,
												min_dist,max_dist,
												min_size,max_size;
						float					min_alpha,max_alpha;
						bool					on,no_clip_object,no_clip_self;
					} model_draw_halo;
					
typedef struct		{
						int						bone_idx;
						float					resize;
						d3vct					mov,rot;
					} model_draw_dynamic_bone;

typedef struct		{
						int						texture_idx,high,
												px[4],pz[4];
						float					alpha;
						bool					on;
						d3pnt					pnt;
						d3ang					ang;
					} model_draw_shadow;

typedef struct		{
						int						obj_idx,weap_idx,proj_idx,
												net_remote_uid;
						bool					net_sound;
						d3vct					motion_vct;
					} model_draw_connect;

typedef struct		{
						bool					on;
						d3pnt					center;
						d3ang					ang;
					} model_draw_no_rot;

typedef struct		{
						int						size;
						float					fade;
						bool					on;
						d3pnt					pnt,proj_pnt;
					} model_draw_remote_name;

typedef struct		{
						int						start_tick,fade_in_msec,fade_life_msec,fade_out_msec;
						float					alpha;
						bool					on;
					} model_draw_mesh_fade;

typedef struct		{
						int						count,
												indexes[max_model_light_cache_index];
					} model_light_cache;

typedef struct		{
						int						model_idx,spin_tick,
												mesh_mask,render_mesh_mask,
												script_animation_idx,
												script_light_idx,script_halo_idx;
						char					name[name_str_len];
						float					alpha,resize,normal[3];
						double					draw_dist;
						bool					on,flip_x,has_opaque,has_transparent,has_glow,
												bounce,face_forward,player,built_vertex_list;
						unsigned char			cur_texture_frame[max_model_texture];
						d3pnt					pnt,size,center,offset;
						d3ang					rot,spin;
						d3col					tint;
						model_draw_connect		connect;
						model_draw_light		lights[max_model_light];
						model_draw_halo			halos[max_model_halo];
						model_draw_animation	animations[max_model_blend_animation];
						model_draw_mesh_fade	mesh_fades[max_model_mesh];
						model_draw_dynamic_bone	dynamic_bones[max_model_dynamic_bone];
						model_draw_shadow		shadow;
						model_draw_fade			fade;
						model_draw_no_rot		no_rot;
						model_draw_remote_name	remote_name;
						model_light_cache		light_cache;
						model_draw_setup		setup;
					} model_draw;

//
// model list
//

typedef struct		{
						model_type*				models[max_model_list];
					} model_list_type;

//
// map polygon pointer
//

typedef struct		{
						int						mesh_idx,poly_idx;
					} poly_pointer_type;

//
// object sounds
//

typedef struct		{
						int						buffer_idx;
						char					name[name_str_len];
						float					pitch;
						bool					on;
					} obj_snd_ambient;

//
// object size
//

typedef struct		{
						int						x,z,y,radius,
												eye_offset,weight;
					} obj_size;

typedef struct		{
						int						count,eye_offset_end;
						float					resize_end,resize_add,
												eye_offset_add,eye_offset_accum;
						bool					on;
						d3pnt					size_end,offset_end;
						d3vct					size_add,size_accum,
												offset_add,offset_accum;
					} obj_grow;

//
// rigid bodies
//

typedef struct		{
						int						max_drop_y,draw_offset_y;
						float					reset_factor_y,smooth_factor_y,
												max_ang_x,reset_factor_x,smooth_factor_x,
												max_ang_z,reset_factor_z,smooth_factor_z;
						bool					on;
					} obj_rigid_body;

//
// object hits and contacts
//

typedef struct		{
						int						health,max_health,start_health,
												health_recover_tick,health_recover_amount,health_recover_count,
												liquid_under_tick,liquid_harm_count,liquid_drown_count,
												respawn_tick;
						float					health_factor;
					} obj_status;
			
typedef struct		{
						int						obj_uid,proj_uid,hit_face,
												liquid_idx;
						bool					object_on,projectile_on,force_on,
												melee,pushable;
						poly_pointer_type		hit_poly,stand_poly,head_poly;
					} obj_contact;
					
typedef struct		{
						int						obj_uid;
						bool					stand;
						d3pnt					pnt;
						d3ang					ang;
					} obj_touch;
					
typedef struct		{
						int						obj_uid,weap_uid,proj_uid,
												damage;
						char					hit_box_name[name_str_len];
						d3pnt					pnt;
						d3ang					ang;
					} obj_hit;
					
typedef struct		{
						int						obj_hit_box_idx,proj_hit_box_idx;
						bool					on;
					} obj_hit_box;
					
typedef struct		{
						int						distance,current_click_obj_uid,
												crosshair_up_idx,crosshair_down_idx;
						char					crosshair_up_name[name_str_len],
												crosshair_down_name[name_str_len];
						bool					on,click_crosshair_on;
					} obj_click;
					
typedef struct		{
						int						obj_uid,item_uid;
						bool					on,ignore,canceled;
					} obj_pickup;
					
typedef struct		{
						bool					on,crushable,invincible,
												in_damage;
					} obj_damage;
					
typedef struct		{
						int						method;
						char					name[name_str_len];
					} obj_weapon_fire;
					
//
// object sighting
//

typedef struct		{
						int						side_division,look_division,distance;
						float					side_angle,look_angle;
					} obj_sight;

//
// object positions
//

typedef struct		{
						int						last_y_change;
						d3vct					vct;
						d3ang					ang;
					} obj_motion;

typedef struct		{
                        float					speed,
												max_crawl_speed,max_walk_speed,max_run_speed,max_air_speed,
												accelerate,decelerate,air_accelerate,air_decelerate,
												slop,seek_ang;
                        bool					moving,running,reverse,seeking;
                    } obj_movement;
					
typedef struct		{
						float					walk_speed,walk_motion_speed,
												run_speed,run_motion_speed,
												crawl_speed,crawl_motion_speed,
												air_speed,air_motion_speed,
												key_speed,top_down_ang_offset;
						bool					ignore_mouse,only_when_moving,restrict_player_turning;
						d3ang					ang_add,ang_to,fix_ang_add;
					} obj_turn;

typedef struct		{
						int						obj_idx;
						d3ang					ang;
					} obj_face;

typedef struct		{
						float					speed,max_speed;
						bool					drag;
						d3vct					vct;
					} obj_thrust;
						
typedef struct		{
						float					up_angle,down_angle,speed,
												ang_add;
						bool					effect_weapons;
					} obj_look;
					
typedef struct		{
						bool					x,y,z;
					} obj_lock;
					
typedef struct		{
						int						high,smooth_offset;
						float					smooth_factor;
						bool					on;
					} obj_bump;
					
typedef struct		{
						int						mesh_idx;
						float					factor;
					} obj_bounce;

typedef struct		{
						int						spawn_mesh_idx,cur_mesh_idx;
					} obj_mesh;

//
// jumping, ducking, climbing, falling, liquids
//

typedef struct		{
						int						mode,y_add,y_move,y_change;
						bool					on;
					} obj_duck;
					
typedef struct		{
						int						y_add;
						bool					on;
					} obj_jump;

typedef struct		{
						float					y_ang;
						bool					on,step_off_ready;
						poly_pointer_type		poly_ptr;
					} obj_climb;
					
typedef struct		{
						int						dist,damage_minimum_height;
						float					damage_factor;
						bool					change;
					} obj_fall;

typedef struct		{
						int						mode,bob_y_move;
					} obj_liquid;

//
// auto-walk and watch
//
					
typedef struct		{
						int						mode,obj_uid,node_event_id,
												node_from_idx,node_dest_idx,
												node_seek_idx,node_last_seek_idx;
						bool					pause_for_turn;
						d3pnt					pnt;
					} obj_auto_walk;

typedef struct		{
						float					ang;
						bool					on,ray_trace;
					} obj_watch_restrict;
					
typedef struct		{
						int						dist,obj_uid,base_team;
						float					restrict_ang;
						char					obj_flags[max_obj_list],sound_name[name_str_len];
						bool					on;
						obj_watch_restrict		watch_restrict;
					} obj_watch;
		
//
// vehicles
//

typedef struct		{
						int						attach_obj_uid;
						bool					on;
						d3pnt					attach_offset;
					} obj_vehicle;

//
// radar
//

typedef struct		{
						int						icon_idx,fade_start_tick;
						char					icon[name_str_len];
						bool					on,motion_only,always_visible;
					} obj_radar;

//
// forces
//

typedef struct		{
						float					gravity;
						d3vct					vct;
					} obj_force;

typedef struct		{
						int						size;
					} obj_kickback;

//
// remote players
//

typedef struct		{
						float					turn_y;
						d3pnt					move;
					} obj_remote_predict;
					
typedef struct		{
						int						uid,last_update;
						bool					talking;
						obj_remote_predict		predict;
					} obj_remote;

typedef struct		{
						int						kill,death,suicide,goal,
												score,place;
					} obj_score;

//
// scenery objects
//

typedef struct		{
						int						idx;
						bool					on;
					} obj_scenery;

//
// object weapons
//
 
typedef struct		{
						int						mode,swap_tick,bounce_y,
												current_idx,next_idx;
					} obj_held_weapon;
					
typedef struct		{
						int						x,y,sz,aim_obj_uid,
												alt_tick,gl_id;
						float					alpha;
						bool					on;
						d3col					color;
					} obj_crosshair_draw;
					
typedef struct		{
						int						x,y,old_camera_mode,start_tick;
						bool					on,sway_reset;
						d3ang					sway_ang,sway_next;
					} obj_zoom_draw;

//
// object debug
//

typedef struct		{
						d3pnt					collide_spt[collide_obj_ray_count],
												collide_ept[collide_obj_ray_count];
					} obj_debug;

//
// weapon/project common structures
//
 
typedef struct		{
						int						radius,distance,damage,
												force;
						char					strike_pose_name[name_str_len],
												object_strike_pose_name[name_str_len];
						bool					fall_off;
						model_tag				strike_bone_tag,object_strike_bone_tag;
					} melee_type;

//
// projectile setup/projectile common structures
//

typedef struct		{
						int						hit_tick;
						float					bounce_min_move,bounce_reduce;
						bool					bounce,reflect;
					} proj_action_type;

//
// projectile setup
//

typedef struct		{
						int						max_dist;
						bool					on;
					} proj_setup_hitscan_type;
					
typedef struct		{
						int						force;
						bool					on;
					} proj_setup_push_type;
					
typedef struct		{
						int						idx,size;
						char					name[name_str_len];
						float					alpha;
						bool					on;
					} proj_setup_mark_type;
			
typedef struct		{
						int						idx,obj_idx,weap_idx,script_uid,
												damage,decel_grace;
						char					name[name_str_len];
						float					speed,decel_speed,decel_min_speed,
												inherit_motion_factor;
						bool					collision,reset_angle;
						obj_size				size;
						model_draw				draw;
						melee_type				melee;
						proj_action_type		action;
						proj_setup_hitscan_type	hitscan;
						proj_setup_push_type	push;
						proj_setup_mark_type	mark;
						attach_type				attach;
					} proj_setup_type;

typedef struct		{
						proj_setup_type*		proj_setups[max_proj_setup_list];
					} proj_setup_list_type;

//
// weapons
//

typedef struct		{
						int						count,count_dual,
												init_count,max_count,
												clip_count,init_clip_count,max_clip_count,
												last_reload_tick,last_reload_dual_tick;
						bool					reset,use_ammo,use_clips;
					} weap_ammo_type;
					
typedef struct		{
						int						size,life_msec;
					} weap_kickback_type;

typedef struct		{
						float					look_reset,turn_reset;
						d3ang					ang,
												min_ang,max_ang,reset_ang;
					} weap_recoil_type;

typedef struct		{
						int						type,min_size,max_size,distance,
												bone_idx,fire_idx;
						char					fire_name[name_str_len];
						bool					on;
						d3col					col,empty_col,pickup_col;
						model_tag				bone_tag;
					} weap_crosshair_type;
					
typedef struct		{
						int						method,last_fire_tick,last_fire_dual_tick;
						bool					cancel;
					} weap_fire_type;

typedef struct		{
						int						distance,obj_uid;
						bool					on,opponent_only;
						d3col					col;
					} weap_target_type;
					
typedef struct		{
						int						mode,mask_idx,tick,step_count,current_step;
						float					fov_min,fov_max,sway_factor,crawl_sway_factor,
												turn_factor,crawl_turn_factor,look_factor;
						char					mask_name[name_str_len];
						bool					on,show_weapon;
					} weap_zoom_type;
					
typedef struct		{
						int						raise_tick,lower_tick,select_shift;
						float					bounce_ang,bounce_speed;
						d3pnt					shift;
						d3ang					ang;
					} weap_hand_type;

typedef struct		{
						int						repeat_tick,next_repeat_tick;
						char					fire_pose_name[name_str_len],
												object_fire_pose_name[name_str_len];
						bool					repeat_on,repeat_ok;
						model_tag				fire_bone_tag,barrel_bone_tag,
												object_fire_bone_tag;
					} weap_proj_type;
					
typedef struct		{
						int						hand_offset;
						bool					on,active,in_dual;
					} weap_dual_type;
					
typedef struct		{
						int						idx,obj_idx;
						char					name[name_str_len];
						bool					hidden,fail_in_liquid;
						model_draw				draw,draw_dual;
						melee_type				melee;
						weap_ammo_type			ammo,alt_ammo;
						weap_kickback_type		kickback;
						weap_recoil_type		recoil;
						weap_crosshair_type		crosshair;
						weap_fire_type			fire;
						weap_dual_type			dual;
						weap_target_type		target;
						weap_zoom_type			zoom;
						weap_hand_type			hand;
						weap_proj_type			proj;
						proj_setup_list_type	proj_setup_list;
						attach_type				attach;
					} weapon_type;

typedef struct		{
						weapon_type*			weaps[max_weap_list];
					} weapon_list_type;

//
// objects
//

typedef struct		{
						int						idx,type,bind,next_spawn_sub_event,
												team_idx,tint_color_idx,character_idx,
												count,input_mode,air_mode,camera_z_adjust,
												stand_obj_uid,damage_obj_uid,item_count,
												last_move_animation_event,last_turn_animation_event;
						char					name[name_str_len],spawn_spot_name[name_str_len];
						bool					hidden,suspend,fly,slope_gravity,
												side_step,crawl,single_speed,no_slide,
												open_doors,input_freeze,hide_all_weapons,
												find_on,in_collide_event,
												quick_reverse,death_trigger,death_telefrag;
						d3pnt					pnt,org_pnt,last_pnt;
						d3ang					ang,view_ang,org_ang,last_ang;
						obj_motion				motion;
                        obj_movement			forward_move,side_move,vert_move;
						obj_lock				lock;
						obj_turn				turn;
						obj_face				face;
						obj_thrust				thrust;
						obj_mesh				mesh;
						obj_look				look;
						obj_size				size;
						obj_grow				grow;
						obj_sight				sight;
						obj_rigid_body			rigid_body;
						obj_bump				bump;
						obj_bounce				bounce;
						obj_duck				duck;
						obj_jump				jump;
						obj_climb				climb;
						obj_fall				fall;
						obj_liquid				liquid;
						obj_auto_walk			auto_walk;
						obj_watch				watch;
						obj_vehicle				vehicle;
						obj_radar				radar;
						obj_force				force;
						obj_kickback			kickback;
						obj_status				status;
						obj_contact				contact;
						obj_touch				touch;
						obj_hit					hit;
						obj_hit_box				hit_box;
						obj_click				click;
						obj_pickup				pickup;
						obj_damage				damage;
						obj_weapon_fire			weapon_fire;
						obj_held_weapon			held_weapon;
						obj_crosshair_draw		crosshair_draw;
						obj_zoom_draw			zoom_draw;
						melee_type				melee;
						model_draw				draw;
						obj_snd_ambient			ambient;
						attach_type				attach;
						obj_remote				remote;
						obj_score				score;
						obj_scenery				scenery;
						obj_debug				debug;
						weapon_list_type		weap_list;
					} obj_type;

typedef struct		{
						obj_type*				objs[max_obj_list];
					} obj_list_type;

//
// projectiles
//

typedef struct		{
						int						idx,obj_idx,weap_idx,proj_setup_idx,
												count,parent_grace,decel_grace,
												start_tick;
						float					speed,decel_speed,decel_min_speed,gravity_add;
						bool					on,script_dispose,stick,reset_angle,flag_melee_hit;
						d3pnt					pnt,org_pnt,last_pnt;
						d3ang					ang;
						obj_size				size;
						obj_motion				motion;
						obj_force				force;
						obj_contact				contact;
						model_draw				draw;
						proj_action_type		action;
						attach_type				attach;
					} proj_type;

typedef struct		{
						proj_type*				projs[max_proj_list];
					} proj_list_type;

//
// effects
//

typedef struct		{
						int						intensity,mid_tick;
						float					exponent;
						d3col					col;
					} flash_effect_data;

typedef struct		{
						bool					on;
						d3ang					ang;
					} particle_rotate;
	
typedef struct		{
						int						obj_uid,weap_uid,proj_uid,
												bone_idx;
						d3vct					vct;
					} particle_motion;

typedef struct		{
						int						particle_idx,variation_idx;
						d3col					tint;
						particle_rotate			rot;
						particle_motion			motion;
					} particle_effect_data;
					
typedef struct		{
						int						ring_idx;
						d3ang					ang;
						d3col					tint;
					} ring_effect_data;
					
typedef struct		{
						int						wid,line_offset;
						float					varient;
						d3pnt					start_pnt,end_pnt;
						d3col					col;
					} lightning_effect_data;
					
typedef struct		{
						int						wid;
						d3pnt					start_pnt,end_pnt;
						d3col					col;
					} ray_effect_data;
					
typedef struct		{
						int						start_size,end_size;
						float					start_alpha,end_alpha;
						d3col					col;
					} globe_effect_data;
					
typedef struct		{
						int						distance;
					} shake_effect_data;
					
typedef union		{
						flash_effect_data		flash;
						particle_effect_data	particle;
						ring_effect_data		ring;
						lightning_effect_data	lightning;
						ray_effect_data			ray;
						globe_effect_data		globe;
						shake_effect_data		shake;
					} effect_data;
	
typedef struct		{
						int						effecttype,mesh_idx,size,
												start_tick,life_tick;
						bool					on;
						d3pnt					pnt;
						effect_data				data;
					} effect_type;

typedef struct		{
						effect_type*			effects[max_effect_list];
					} effect_list_type;

//
// decal structures
//

typedef struct		{
						int						mark_idx,mesh_idx,poly_idx,
												start_tick,x[4],z[4],y[4];
                        float					alpha;
						bool					on,in_view;
						d3col					tint;
					} decal_type;

typedef struct		{
						decal_type*				decals[max_decal_list];
					} decal_list_type;

//
// ray trace structures
//

#define poly_ray_trace_hit_mode_all						0
#define poly_ray_trace_hit_mode_wall_only				1
#define poly_ray_trace_hit_mode_floor_only				2

#define poly_ray_trace_origin_unknown					0
#define poly_ray_trace_origin_object					1
#define poly_ray_trace_origin_projectile				2

#define ray_trace_face_hit_neg_z						0
#define ray_trace_face_hit_pos_z						1
#define ray_trace_face_hit_neg_x						2
#define ray_trace_face_hit_pos_x						3
#define ray_trace_face_hit_neg_y						4
#define ray_trace_face_hit_pos_y						5

#define ray_trace_max_check_item						512

#define ray_trace_check_item_object						0
#define ray_trace_check_item_projectile					1
#define ray_trace_check_item_mesh_poly					2


typedef struct		{
						int								uid,ignore_uid,hit_face;
						bool							on;
					} ray_trace_obj_proj_type;

typedef struct		{
						int								hit_mode,origin;
						ray_trace_obj_proj_type			obj,proj;
						poly_pointer_type				poly;
					} ray_trace_contact_type;

typedef struct		{
						int								type,index,index_2;
					} ray_trace_check_item_type;

//
// timing structures
//

typedef struct		{
						int								run_tick,
														network_update_tick,
														network_group_synch_tick,
														network_latency_ping_tick;
						bool							paused;
					} server_time_type;

//
// generic image animation structure
//

typedef struct		{
						int								image_count,image_per_row,msec;
						bool							loop,loop_back;
					} image_animation_type;
					
//
// particle structures
//

typedef struct		{
						int								shift;
						char							name[name_str_len];
					} particle_group_particle_type;

typedef struct		{
						int								count;
						bool							on;
						particle_group_particle_type	particles[max_particle_group];
					} particle_group_type;

typedef struct		{
						d3pnt							pt;
						d3vct							vct;
					} particle_piece_type;

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
						image_animation_type			animate;
						particle_piece_type				pieces[max_particle_variation][max_particle_count];
						particle_group_type				group;
					} particle_type;

typedef struct		{
						particle_type*					particles[max_particle_list];
					} particle_list_type;

//
// ring structures
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
						image_animation_type			animate;
					} ring_type;

typedef struct		{
						ring_type*						rings[max_ring_list];
					} ring_list_type;

//
// misc settings structures
//
					
typedef struct		{
						int								image_idx;
						char							name[name_str_len],bitmap_name[name_str_len];
					} halo_type;

typedef struct		{
						halo_type*						halos[max_halo_list];
					} halo_list_type;
					
typedef struct		{
						int								image_idx,
														fade_in_msec,life_msec,fade_out_msec,total_msec;
						char							name[name_str_len],bitmap_name[name_str_len];
						bool							no_rotate,no_transparent,no_opaque,hilite,blend_add,team_tint;
						d3col							color;
						image_animation_type			animate;
					} mark_type;

typedef struct		{
						mark_type*						marks[max_mark_list];
					} mark_list_type;

typedef struct		{
						int								image_idx;
						char							name[name_str_len],bitmap_name[name_str_len];
					} crosshair_type;

typedef struct		{
						crosshair_type*					crosshairs[max_crosshair_list];
					} crosshair_list_type;
					
//
// Main Server Structure
//
 
 typedef struct		{
						int								state,next_state,last_state,
														skill,player_obj_idx;
						bool							game_open,map_open,map_change,skip_media;
						server_time_type				time;
						obj_list_type					obj_list;
						proj_list_type					proj_list;
						model_list_type					model_list;
						effect_list_type				effect_list;
						decal_list_type					decal_list;
						particle_list_type				particle_list;
						ring_list_type					ring_list;
						halo_list_type					halo_list;
						mark_list_type					mark_list;
						crosshair_list_type				crosshair_list;
					} server_type;


