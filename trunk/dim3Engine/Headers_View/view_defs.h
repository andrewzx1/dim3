/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Definitions Header

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
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
// view maximums
//

#define max_view_render_item								5120

#define max_light_spot										128				// maximum number of lights in a scene
#define max_shader_light									3				// maximum number of lights passed to a shader

//
// shaders
//

#define max_shader_custom_vars								8

#define max_core_shader_data_sz								10240

#define max_core_shader										18

#define gl_core_map_shader_light							0
#define gl_core_map_shader_light_bump						1
#define gl_core_map_shader_light_bump_spec					2
#define gl_core_map_shader_light_map						3
#define gl_core_map_shader_light_map_bump					4
#define gl_core_map_shader_light_map_bump_spec				5
#define gl_core_model_shader_light							6
#define gl_core_model_shader_light_bump						7
#define gl_core_model_shader_light_bump_spec				8
#define gl_core_map_shader_fog_light						9
#define gl_core_map_shader_fog_light_bump					10
#define gl_core_map_shader_fog_light_bump_spec				11
#define gl_core_map_shader_fog_light_map					12
#define gl_core_map_shader_fog_light_map_bump				13
#define gl_core_map_shader_fog_light_map_bump_spec			14
#define gl_core_model_shader_fog_light						15
#define gl_core_model_shader_fog_light_bump					16
#define gl_core_model_shader_fog_light_bump_spec			17

#define max_user_shader										64

#define gl_shader_core_index								1000

//
// input rate
//

#define input_tick_rate										33

//
// stencil constants
//

#define stencil_none										0
#define stencil_poly_start									1
#define stencil_poly_end									255

#define stencil_shadow										1

//
// remote names
//

#define remote_name_max_distance							(map_enlarge*500)	// how far away you can see map names
#define remote_name_min_distance							(map_enlarge*100)	// closest you can see map names in full

//
// render item type
//

#define view_render_type_mesh								0
#define view_render_type_liquid								1
#define view_render_type_object								2
#define view_render_type_projectile							3
#define view_render_type_effect								4

//
// obscure settings
//

#define obscure_grid_division								256
#define obscure_grid_byte_row_size							(obscure_grid_division>>3)
#define obscure_grid_byte_size								(obscure_grid_byte_row_size*obscure_grid_division)

//
// shader variable types
//

#define shader_var_type_int									0
#define shader_var_type_float								1
#define shader_var_type_vec3								2
#define shader_var_type_vec4								3

//
// image structures
//

#define max_view_image										1024
#define max_view_image_bitmap								32

typedef struct		{
						int									msec;
						bitmap_type							bitmap;
					} view_image_bitmap_type;

typedef struct		{
						int									nbitmap,total_msec;
						char								path[1024];
						view_image_bitmap_type				bitmaps[max_view_image_bitmap];
					} view_image_type;

//
// lighting structures
//

typedef struct		{
						int									direction,i_intensity;
						bool								light_map;
						float								exponent,f_x,f_y,f_z,f_intensity;
						double								d_x,d_y,d_z,
															intensity,inv_intensity,
															d_intensity,d_inv_intensity,
															d_col_r,d_col_g,d_col_b;
						d3pnt								pnt;
						d3vct								pnt_eye_space;
						d3col								col;
					} view_light_spot_type;

typedef struct		{
						int									nlight,light_idx[max_shader_light];
						d3col								ambient;
					} view_light_list_type;

//
// join UI structures
//

typedef struct		{
						int									ping_msec,player_count,player_max_count;
						char								ip[256],name[name_str_len],
															game_name[name_str_len],map_name[name_str_len];
					} join_server_info;

//
// halo structures
//
 
typedef struct		{
						int									idx,obj_uid,
															min_dist,max_dist,
															min_pixel_sz,max_pixel_sz,pixel_sz;
						float								min_alpha,max_alpha,alpha;
						bool								in_view,no_clip_object,no_clip_self;
						d3pnt								pnt,proj_pnt;
					} halo_draw_type;

//
// rain structures
//

typedef struct		{
						int									x,z,y,by,
															x_off,z_off;
					} rain_draw_type;

//
// timing structures
//

typedef struct		{
						int									input_tick,draw_tick,draw_time,
															run_tick;
					} view_time_type;

//
// fps type
//

typedef struct		{
						int									tick,count,last_time;
						float								total;
						bool								first_skip;
					} view_fps_type;

//
// shader structures
//

typedef union		{
						int									i;
						float								f;
						float								vec3[3];
						float								vec4[4];
					} shader_value_type;
					
typedef struct		{
						int									var_type;
						char								name[name_str_len];
						shader_value_type					value;
					} shader_custom_var_type;

typedef struct		{
						GLint								position,color,
															intensity,exponent,
															direction,inLightMap;					
					} shader_cached_var_light_loc;
					
typedef struct		{
						GLint								dim3FrequencySecond,
															dim3CameraPosition,dim3AmbientColor,
															dim3LightMapBoost,dim3ShineFactor,
															dim3TintColor,dim3Alpha,
															dim3Tangent,dim3Normal,
															dim3VertexTangent,dim3VertexNormal;
						shader_cached_var_light_loc			dim3Lights[max_shader_light];
					} shader_cached_var_loc;

typedef struct		{
						int									light_map,direction;
						float								intensity,exponent;
						d3fpnt								position;
						d3col								color;
					} shader_current_var_light_value;

typedef struct		{
						float								alpha,shine_factor;
						d3col								tint_col;
						d3vct								tangent,normal;
						shader_current_var_light_value		lights[max_shader_light];
					} shader_current_var_value;

typedef struct		{
						int									start_tick;
						char								name[64],
															vertex_name[64],fragment_name[64];
						bool								per_scene_vars_set,normal_vertex_attrib_active;
						GLhandleARB							vertex_obj,fragment_obj,program_obj;
						shader_cached_var_loc				var_locs;
						shader_current_var_value			var_values;
					} shader_type;

//
// view counts
//

typedef struct		{
						int									poly,liquid,model,shadow,effect;
					} view_count_type;

//
// view rendering structure
//

#define view_list_item_flag_model_in_view					0x1
#define view_list_item_flag_shadow_in_view					0x2

typedef struct		{
						int									under_liquid_idx,z_adjust;
						float								fov;
						bool								flip;
						d3pnt								pnt;
						d3ang								ang;
					} view_render_camera_type;
	
typedef struct		{
						int									flag;
						short								type,idx;
						double								dist;
					} view_render_draw_list_item_type;
					
typedef struct		{
						int									count;
						view_render_draw_list_item_type		items[max_view_render_item];
					} view_render_draw_list_type;

typedef struct		{
						int									count;
						view_light_spot_type				spots[max_light_spot];
					} view_render_light_type;

typedef struct		{
						int									count,in_view_count;
						halo_draw_type						halos[max_light_spot];
					} view_render_halo_type;

typedef struct		{
						bool								force_camera_obj;
						view_render_camera_type				camera;
						view_render_draw_list_type			draw_list;
						view_render_light_type				light;
						view_render_halo_type				halo_draw;
					} view_render_type;

//
// obscuring
//

typedef struct		{
						int									x_start,z_start,
															x_size,z_size;
						unsigned char						*grid;
					} view_obscure_type;

//
// menus and debug
//

typedef struct		{
						int									fade_start_tick,menu_idx,
															click_item_idx;
						bool								fade_in,fade_out,active,
															mouse_down;
					} view_menu_type;

typedef struct		{
						bool								on;
					} view_debug_type;

//
// main view structure
//
 
typedef struct		{
						bool								shader_on;
						view_count_type						count;
						view_render_type					*render;
						view_time_type						time;
						view_fps_type						fps;
						view_image_type						*images;
						rain_draw_type						*rain_draws;
						view_obscure_type					obscure;
						view_menu_type						menu;
						view_debug_type						debug;
					} view_type;


