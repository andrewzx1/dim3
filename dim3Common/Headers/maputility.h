/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: External Header

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
// constants
//

extern char light_type_str[][32];

//
// version
//

#define map_current_version									3

//
// map sizes
//

#define map_enlarge											144				// each units of maze equals this many game units
#define map_max_size										500000			// maximum size of map on x/z units

//
// map maximums
//

#define max_mesh											8192			// maximum number of meshes in a map
#define max_liquid											256				// maximum number of liquids in a a map

#define max_map_texture										256				// maximum number of textures in a map

#define max_group											128				// maximum number of mesh groups
#define max_area											128				// maximum number of sight view areas

#define max_map_scenery										1024			// maximum number of ambient scenery
#define max_map_scenery_model_texture_frame					8				// maximum number of changable texture frames

#define max_map_light										256				// maximum number of ambient lights
#define max_map_sound										256				// maximum number of ambient sounds
#define max_map_particle									256				// maximum number of ambient particles

#define max_spot											512				// maximum number of spot in a map
#define max_node											512				// maximum number of nodes in a map
#define max_node_link										16				// maximum number of connected nodes

#define max_movement										64				// maximum number of movements
#define max_movement_move									16				// maximum number of separate moves in a movement

#define max_sort_poly										2048			// maximum number of transparent polys in a single scene

//
// strings
//

#define param_str_len										256				// length of parameter string

//
// physics
//

#define floor_slop											128				// how close to floor to be "standing" on it

#define gravity_start_power									0				// starting gravity
#define gravity_factor										5000			// factor to divide gravity out by when combing with weight

#define gravity_slope_min_y									0.4f			// minimum slope needed to activate gravity
#define gravity_slope_max_y									1.5f			// maximum climbable slope
#define gravity_slope_factor								150.0f			// slope gravity factor
#define gravity_slope_down_cut								0.2f			// amount to count gravity when running with it

#define proj_reflect_slope_max_y							0.5f			// how big a slope before bounces become reflects
#define proj_bounce_min_speed								10.0f			// minimum speed before bounces cancel

//
// per mesh normal creation
//

#define mesh_normal_mode_auto								0
#define mesh_normal_mode_in									1
#define mesh_normal_mode_out								2
#define mesh_normal_mode_in_out								3
#define mesh_normal_mode_lock								4

//
// lights
//

#define lt_normal											0
#define lt_blink											1
#define lt_glow												2
#define lt_pulse											3
#define lt_flicker											4
#define lt_failing											5

#define ld_all												0
#define ld_neg_x											1
#define ld_pos_x											2
#define ld_neg_y											3
#define ld_pos_y											4
#define ld_neg_z											5
#define ld_pos_z											6

//
// media types
//

#define mi_none												0
#define mi_story											1
#define mi_title											2
#define mi_movie											3

//
// sky types
//

#define st_dome_panoramic									0
#define st_dome_hemisphere									1
#define st_cube												2

//
// liquids
//

#define liquid_min_division									5
#define liquid_max_division									20

#define liquid_direction_horizontal							0
#define liquid_direction_vertical							1

//
// group types
//

#define group_type_mesh										0
#define group_type_liquid									1

//
// skills
//

#define skill_easy											0
#define skill_medium										1
#define skill_hard											2

//
// spawn play types
//

#define spawn_always										0
#define spawn_single_player_only							1
#define spawn_multiplayer_only								2

//
// mesh hide types
//

#define mesh_hide_mode_never								0
#define mesh_hide_mode_single_player						1
#define mesh_hide_mode_multiplayer							2

//
// group structure
//

typedef struct		{
						int									type,idx;
					} group_unit_type;

typedef struct		{
						int									count,user_id,
															movement_idx,movement_move_idx;
						bool								on,was_moved,freeze,main_move;
						d3pnt								mov_add,cuml_mov_add;
						d3ang								rot_add,cuml_rot_add;
						attach_type							attach;
					} group_move_type;

typedef struct		{
						int									unit_count;
						char								name[name_str_len];
						d3pnt								center_pnt;
						group_move_type						move;
						group_unit_type						*unit_list;
					} group_type;

//
// mesh structures
//

typedef struct		{
						bool								flat,wall_like;
						d3pnt								min,max,mid;
					} map_mesh_poly_box_type;

typedef struct		{
						int									lx,rx,lz,rz;
					} map_mesh_poly_line_type;

typedef struct		{
						float								y,ang_y,move_x,move_z;
					} map_mesh_poly_slope_type;

typedef struct		{
						int									vertex_offset,decal_stencil_idx,
															txt_frame_offset,
															gl_poly_index_min,gl_poly_index_max,gl_poly_index_offset;
						float								x_shift_offset,y_shift_offset;
						bool								bump_ok,shift_on,shader_on,transparent_on,glow_on;
					} map_mesh_poly_draw_type;

typedef struct		{
						float								x[8],y[8];
					} map_mesh_poly_uv_type;

typedef struct		{
						int									txt_idx,lmap_txt_idx,ptsz,v[8];
						float								x_shift,y_shift;
						char								camera[name_str_len];
						tangent_space_type					tangent_space;
						map_mesh_poly_uv_type				main_uv,lmap_uv;
						map_mesh_poly_box_type				box;
						map_mesh_poly_line_type				line;
						map_mesh_poly_slope_type			slope;
						map_mesh_poly_draw_type				draw;
					} map_mesh_poly_type;

typedef struct		{
						int									all_count,wall_count,floor_count;
						short								*all_idxs,*wall_idxs,*floor_idxs;
					} map_mesh_poly_list;
					
typedef struct		{
						d3pnt								min,max,mid;
					} map_mesh_box_type;

typedef struct		{
						bool								on,pass_through,moveable,
															hilite,climbable,shiftable,
															lock_uv,lock_move,never_obscure,
															rot_independent,shadow,
															no_light_map,skip_light_map_trace,
															touched;
					} map_mesh_flag_type;

typedef struct		{
						int									entry_id,exit_id,base_team;
						bool								entry_on,exit_on,base_on,map_change_on;
						char								map_name[file_str_len],
															map_spot_name[name_str_len],map_spot_type[name_str_len];
					} map_mesh_message_type;

typedef struct		{
						int									vertex_offset;
						bool								moved,cur_ambient_only,
															has_opaque,has_transparent,
															has_shader,has_no_shader,has_glow,
															has_light_map,dist_shader_override;
					} map_mesh_draw_type;
					
typedef struct		{
						int									nvertex,npoly,group_idx,
															hide_mode,normal_mode;
						float								*colors_cache;
						d3pnt								rot_off;
						d3pnt								*vertexes;
						map_mesh_poly_type					*polys;
						map_mesh_poly_list					poly_list;
						map_mesh_box_type					box;
						map_mesh_flag_type					flag;
						map_mesh_message_type				msg;
						map_mesh_draw_type					draw;
					} map_mesh_type;

//
// liquid structures
//

typedef struct		{
						float								x_offset,x_size,
															y_offset,y_size;
					} map_liquid_uv_type;

typedef struct		{
						int									in_harm,drown_harm,drown_tick;
					} map_liquid_harm_type;

typedef struct		{
						int									rate,high,direction,division;
						bool								flat;
					} map_liquid_tide_type;

typedef struct		{
						int									v_cnt,x_sz,z_sz;
					} map_liquid_draw_type;

typedef struct		{
						int									y,depth,lft,rgt,top,bot,group_idx,
															txt_idx,lmap_txt_idx;
						float								speed_alter,tint_alpha,
															x_shift,y_shift;
						bool								never_obscure;
						d3col								col;
						map_liquid_uv_type					main_uv,lmap_uv;
						map_liquid_harm_type				harm;
						map_liquid_tide_type				tide;
						map_liquid_draw_type				draw;
					} map_liquid_type;

//
// mesh and liquid collections
//

typedef struct		{
						int									nmesh,vbo_vertex_count;
						map_mesh_type						*meshes;
					} map_mesh_collection_type;

typedef struct		{
						int									nliquid;
						map_liquid_type						*liquids;
					} map_liquid_collection_type;

//
// map sort lists
//

typedef struct		{
						int									mesh_idx,poly_idx;
						float								dist;
					} map_poly_sort_item_type;

typedef struct		{
						int									count;
						map_poly_sort_item_type				*list;
					} map_poly_sort_type;

//
// scenery, light, sound and particle structures
//
					
typedef struct		{
						short								texture_frame[max_map_scenery_model_texture_frame];
						float								resize;
						char								model_name[name_str_len],animation_name[name_str_len];
						bool								contact_object_on,contact_projectile_on,
															contact_hit_box,face_forward,
															shadow;
						d3pnt								pnt;
						d3ang								ang;
					} map_scenery_type;

typedef struct		{
						int									type,direction,intensity;
						float								exponent;
						bool								on,light_map;
						char								name[name_str_len];
						d3pnt								pnt;
						d3col								col;
					} map_light_type;
					
typedef struct		{
						int									buffer_idx;
                        float								pitch;
						char								name[name_str_len];
						bool								on;
						d3pnt								pnt;
					} map_sound_type;
					
typedef struct		{
                        int									spawn_tick,slop_tick,next_spawn_tick,
															particle_idx;
						char								name[name_str_len];
						bool								on;
						d3pnt								pnt;
					} map_particle_type;
                    
//
// script spots
//

typedef struct		{
						int									skill,spawn;
						char								name[name_str_len],
															type[name_str_len],
															script[name_str_len],
															display_model[name_str_len],
															params[param_str_len],
															attach_name[name_str_len],
															attach_type[name_str_len],
															attach_script[file_str_len],
															attach_params[param_str_len];
						bool								attach,random_hit;
						d3pnt								pnt;
						d3ang								ang;
					} spot_type;

//
// nodes
//

typedef struct		{
						bool								render;
						GLuint								txt_id;
					} node_back_render_type;

typedef struct		{
						int									idx,event_id;
						short								link[max_node_link],
															path_hint[max_node];
						char								name[name_str_len];
						bool								follow_camera;
						d3pnt								pnt;
						d3ang								ang;
						node_back_render_type				back_render;
					} node_type;

//
// movement structures
//
 
typedef struct		{
						int									msec,user_id,sound_buffer_idx;
						char								sound_name[name_str_len];
						float								sound_pitch;
						d3pnt								mov;
						d3ang								rot;
					} movement_move_type;

typedef struct		{
						int									nmove,group_idx,reverse_group_idx,
															auto_open_distance;
						char								name[name_str_len];
						bool								auto_start,auto_open,auto_open_stand,
															auto_open_never_close,
															loop,started,opened,reverse;
						movement_move_type					moves[max_movement_move];
					} movement_type;

//
// map media and music
//

typedef struct		{
						int									type;
						char								name[name_str_len],title_sound_name[name_str_len];
					} map_media_type;
					
typedef struct		{
						int									fade_msec;
						char								name[name_str_len];
					} map_music_type;
					
//
// map info and sky structures
//

typedef struct		{
						char								name[name_str_len],
															title[name_str_len],
															author[name_str_len],
															host_name[name_str_len],
															player_start_name[name_str_len],
															player_start_type[name_str_len],
															load_path[1024];
						bool								in_load;
					} map_info_type;

typedef struct		{
						int									quality,size,
															pixel_border_count,blur_count;
					} map_settings_light_map_type;

typedef struct		{
						int									optimize_distance;
						float								gravity,gravity_max_power,gravity_max_speed,
															resistance,txt_scale_x,txt_scale_y;
						bool								editor_link_always_start;
						char								network_game_list[256];
						map_settings_light_map_type			light_map;
					} map_settings_type;
					
typedef struct		{
						float								sound_pitch,light_map_boost;
						char								sound_name[name_str_len];
						bool								light_ignore_mesh;
						d3col								light_color;
					} map_ambient_type;					

typedef struct		{
						int									density,radius,height,speed,
															line_width,line_length,slant_add,
															slant_time_msec,slant_change_msec;
						float								alpha;
						bool								on,reset;
						d3col								start_color,end_color;
					} map_rain_type;					
					
typedef struct		{
						int									fill;
						float								x_scroll_fact,y_scroll_fact;
						bool								on;
					} map_background_type;					
					
typedef struct		{
						int									type,radius,dome_y,
															fill,bottom_fill,north_fill,south_fill,east_fill,west_fill;
						float								txt_fact,txt_x_shift,txt_y_shift;
						bool								on,dome_mirror;
					} map_sky_type;
					
typedef struct		{
						int									count,inner_radius,outer_radius,
															high,drop,texture_idx;
						float								speed,txt_x_fact,txt_y_fact,
															alpha;
						bool								on,use_solid_color;
						d3col								col;
					} map_fog_type;

//
// area structures
//

typedef struct		{
						int									lft,rgt,top,bot;
						d3col								col;
					} map_area_type;

//
// main map structure
//

typedef struct		{

						int									nlight,nsound,nparticle,
															nspot,nnode,nscenery,nmovement,
															ngroup,narea,start_game_tick;
													
						map_info_type						info;
						
						map_ambient_type					ambient;
						map_background_type					background;
						map_sky_type						sky;
						map_fog_type						fog;
						map_rain_type						rain;
						
						map_settings_type					settings;
						map_media_type						media;
						map_music_type						music;
						
						texture_type						*textures;
						
						map_scenery_type					*sceneries;
						map_light_type						*lights;
						map_sound_type						*sounds;
						map_particle_type					*particles;
						spot_type							*spots;
						node_type							*nodes;
						
						movement_type						*movements;
						group_type							*groups;
						map_area_type						*areas;
	
						map_mesh_collection_type			mesh;
						map_liquid_collection_type			liquid;
						
					} map_type;

//
// functions
//

extern void map_setup(file_path_setup_type *file_path_setup,int anisotropic_mode,int mipmap_mode,bool compress_on,bool in_engine,bool glsl_ok);
extern bool map_new(map_type *map,char *name);
extern bool map_open(map_type *map,char *name);
extern bool map_reload(map_type *map);
extern bool map_save(map_type *map);
extern void map_close(map_type *map);
extern void map_refresh_textures(map_type *map);

extern void map_prepare_mesh_poly(map_mesh_type *mesh,map_mesh_poly_type *poly);
extern void map_prepare_mesh_box(map_mesh_type *mesh);
extern void map_prepare(map_type *map);
extern void map_center(map_type *map);

extern int map_count_texture_frames(map_type *map,int txt);
extern void map_setup_animated_textures(map_type *map,int tick);

extern bool map_check_game_type(char *game_type,char *map_name,char *info_name);

extern bool map_create_vertex_lists(map_type *map);
extern void map_dispose_vertex_lists(map_type *map);

extern int map_count_spot(map_type *map,char *name,char *type);
extern int map_find_spot(map_type *map,char *name,char *type);
extern void map_find_random_spot_clear(map_type *map,char *name,char *type);
extern int map_find_random_spot(map_type *map,char *name,char *type);
extern void map_spot_clear_attach(map_type *map);
extern void map_spot_attach_object(spot_type *spot,char *name,char *type,char *script,char *params);

extern int map_find_light(map_type *map,char *name);

extern int map_node_to_node_distance(map_type *map,int from_idx,int to_idx);
extern int map_find_node(map_type *map,char *name);
extern int map_find_nearest_node_by_point(map_type *map,d3pnt *pnt);
extern int map_find_nearest_node_in_path(map_type *map,int from_idx,char *name,int *dist);
extern int map_find_random_node(map_type *map,char *name);
extern int map_find_next_node_in_path(map_type *map,int from_idx,int to_idx);

extern int map_mesh_add(map_type *map);
extern bool map_mesh_delete(map_type *map,int mesh_idx);
extern bool map_mesh_set_vertex_count(map_type *map,int mesh_idx,int nvertex);
extern bool map_mesh_set_poly_count(map_type *map,int mesh_idx,int npoly);
extern int map_mesh_duplicate(map_type *map,int mesh_idx);
extern int map_mesh_count_total_poly(map_type *map);
extern int map_mesh_add_poly(map_type *map,int mesh_idx,int ptsz,int *x,int *y,int *z,float *gx,float *gy,int txt_idx);
extern bool map_mesh_delete_poly(map_type *map,int mesh_idx,int poly_idx);
extern bool map_mesh_delete_unused_vertexes(map_type *map,int mesh_idx);
extern void map_mesh_calculate_extent(map_type *map,int mesh_idx,d3pnt *min,d3pnt *max);
extern void map_mesh_calculate_center(map_type *map,int mesh_idx,d3pnt *mpt);
extern void map_mesh_calculate_uv_center(map_type *map,int mesh_idx,float *gx,float *gy);

extern int map_mesh_find(map_type *map,d3pnt *pnt);
extern int map_mesh_find_closest(map_type *map,d3pnt *pnt);
extern int map_mesh_find_always(map_type *map,d3pnt *pnt);
extern double map_mesh_calculate_distance(map_mesh_type *mesh,d3pnt *pnt);

extern bool map_mesh_create_colors_cache(map_mesh_type *mesh);

extern int map_mesh_combine(map_type *map,int mesh_1_idx,int mesh_2_idx);
extern int map_mesh_combine_small(map_type *map,int poly_threshold);
extern void map_mesh_move(map_type *map,int mesh_idx,d3pnt *mov_pnt);
extern void map_mesh_resize(map_type *map,int mesh_idx,d3pnt *min,d3pnt *max);
extern void map_mesh_flip(map_type *map,int mesh_idx,bool flip_x,bool flip_y,bool flip_z);
extern void map_mesh_rotate(map_type *map,int mesh_idx,d3pnt *center_pnt,d3ang *rot_ang);
extern bool map_mesh_tesselate(map_type *map,int mesh_idx);
extern bool map_mesh_poly_punch_hole(map_type *map,int mesh_idx,int poly_idx,d3pnt *extrude_pnt);
extern void map_mesh_poly_run_shifts(map_type *map,int tick);
extern void map_mesh_get_poly_uv_as_box(map_type *map,int mesh_idx,int poly_idx,bool light_map_uv,float *x_txtoff,float *y_txtoff,float *x_txtfact,float *y_txtfact);
extern void map_mesh_set_poly_uv_as_box(map_type *map,int mesh_idx,int poly_idx,bool light_map_uv,float x_txtoff,float y_txtoff,float x_txtfact,float y_txtfact);
extern void map_mesh_rotate_poly_uv(map_type *map,int mesh_idx,int poly_idx,int rot_ang);
extern void map_mesh_flip_poly_uv(map_type *map,int mesh_idx,int poly_idx,bool flip_u,bool flip_v);
extern void map_mesh_reset_poly_uv(map_type *map,int mesh_idx,int poly_idx);
extern void map_mesh_reset_uv(map_type *map,int mesh_idx);
extern void map_mesh_whole_poly_uv(map_type *map,int mesh_idx,int poly_idx);
extern void map_mesh_whole_uv(map_type *map,int mesh_idx);
extern void map_mesh_single_poly_uv(map_type *map,int mesh_idx,int poly_idx);
extern void map_mesh_single_uv(map_type *map,int mesh_idx);

extern int map_liquid_add(map_type *map);
extern bool map_liquid_delete(map_type *map,int liquid_idx);
extern int map_liquid_duplicate(map_type *map,int liquid_idx);
extern void map_liquid_calculate_center(map_type *map,int liquid_idx,d3pnt *pt);
extern double map_liquid_calculate_distance(map_liquid_type *liq,d3pnt *pnt);

extern void map_liquid_move(map_type *map,int liquid_idx,int x,int y,int z);
extern void map_liquid_reset_uv(map_type *map,int liquid_idx);

extern bool map_group_create_unit_list(map_type *map);
extern void map_group_dispose_unit_list(map_type *map);
extern void map_group_get_center(map_type *map,int group_idx,d3pnt *center_pnt);

extern int map_movement_find(map_type *map,char *name);
extern int map_movement_add(map_type *map);
extern void map_movement_delete(map_type *map,int movement_idx);
extern int map_movement_move_add(map_type *map,int movement_idx);
extern void map_movement_move_delete(map_type *map,int movement_idx,int move_idx);

extern bool map_add_texture_frame(map_type *map,int txt,char *bitmap_name);
extern bool map_delete_texture_frame(map_type *map,int txt);
extern bool map_replace_texture(map_type *map,int txt,char *bitmap_name);
extern bool map_delete_texture(map_type *map,int start_txt,int end_txt);

extern void map_recalc_normals_mesh(map_mesh_type *mesh,bool only_tangent_binormal);
extern void map_recalc_normals(map_type *map,bool only_tangent_binormal);

