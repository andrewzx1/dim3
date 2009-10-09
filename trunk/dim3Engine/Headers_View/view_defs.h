/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Definitions Header

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
// view maximums
//

#define max_view_image										1024

#define max_view_shader										64
#define max_view_shader_custom_vars							8

#define max_view_render_item								5120

#define max_light_spot										128				// maximum number of lights in a scene
#define max_shader_light									8				// maximum number of lights passed to a shader

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
// sorting types
//

#define view_sort_object									0
#define view_sort_projectile								1

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

typedef struct		{
						char								path[1024];
						bitmap_type							bitmap;
					} view_image_type;

//
// lighting structures
//

typedef struct		{
						int									filter,direction,i_intensity;
						float								exponent,f_x,f_y,f_z;
						double								d_x,d_y,d_z,
															intensity,inv_intensity,
															d_intensity,d_inv_intensity,
															d_col_r,d_col_g,d_col_b;
						d3pnt								pnt;
						d3col								col;
					} view_light_spot_type;

typedef struct		{
						int									nlight,light_idx[max_shader_light];
						float								pos[3*max_shader_light],
															col[3*max_shader_light],												
															intensity[max_shader_light],
															exponent[max_shader_light],
															direction[3*max_shader_light];
					} view_glsl_light_list_type;

//
// join UI structures
//

typedef struct		{
						int									ping_msec,player_count,player_max_count;
						bool								local;
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
					} view_fps_type;

//
// shader structures
//

typedef union		{
						int									i;
						float								f;
						float								vec3[3];
						float								vec4[4];
					} view_shader_value_type;
					
typedef struct		{
						int									var_type;
						char								name[name_str_len];
						view_shader_value_type				value;
					} view_shader_custom_var_type;

typedef struct		{
						int									nvar;
						view_shader_custom_var_type			vars[max_view_shader_custom_vars];
					} view_shader_custom_var_list_type;

typedef struct		{
						GLint								dim3TimeMillisec,dim3FrequencySecond,
															dim3CameraPosition,dim3AmbientColor,
															dim3BumpFactor,dim3SpecularFactor,
															dim3TexColor,dim3LightPosition,
															dim3LightColor,dim3LightIntensity,
															dim3LightExponent,dim3LightDirection,
															dim3TintColor,dim3DarkFactor,dim3Alpha;
					} view_shader_cached_var_loc;

typedef struct		{
						int									start_tick,
															cur_nlight,cur_light_idx[max_shader_light];
						float								cur_dark_factor,cur_alpha;
						char								vertex_name[file_str_len],fragment_name[file_str_len];
						bool								on,per_scene_vars_set,cur_in_hilite;
						d3col								cur_tint_col;
						view_shader_cached_var_loc			var_locs;
						GLhandleARB							vertex_obj,fragment_obj,program_obj;
					} view_shader_code_type;

typedef struct		{
						char								name[name_str_len];
						view_shader_code_type				code_default,
															code_light[max_shader_light+1];
						view_shader_custom_var_list_type	custom_var_list;
					} view_shader_type;

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
						bool								has_area,no_shader,force_camera_obj;
						char								area_mask[max_area];
						view_render_camera_type				camera;
						view_render_draw_list_type			draw_list;
						view_render_light_type				light;
						view_render_halo_type				halo_draw;
					} view_render_type;

//
// count structure
//

typedef struct		{
						int									image,shader;
					} view_count_type;

//
// main view structure
//
 
typedef struct		{
						view_render_type					*render;
						view_count_type						count;
						view_time_type						time;
						view_fps_type						fps;
						view_image_type						*images;
						view_shader_type					*shaders;
						rain_draw_type						*rain_draws;
					} view_type;


