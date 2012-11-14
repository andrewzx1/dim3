//
// types
//

#include "ray_types.h"

//
// data
//

#define ray_max_scene								32
#define ray_max_scene_light							128
#define ray_max_scene_mesh							5120
#define ray_max_scene_overlay						256
#define ray_max_scene_overlay_quad					256

#define ray_max_material							1024

//
// ray tracing
//

#define ray_max_bounce								8

//
// threading
//

#define ray_render_min_thread_count					8
#define ray_render_max_thread_count					64

//
// worker thread mode
//

#define ray_thread_mode_suspend						0
#define ray_thread_mode_rendering					1
#define ray_thread_mode_shutdown					2

//
// mipmaps
//

#define ray_mipmap_max_level						8
#define ray_mipmap_distance_exponent				0.7f

//
// math defines
//

#define ray_TRIG_PI									3.14159265358979f
#define ray_ANG_to_RAD								(float)(ray_TRIG_PI/180.0f)
#define ray_RAD_to_ANG								(float)(180.0f/ray_TRIG_PI)

//
// bounds struct
//

typedef struct		{
                        ray_point_type				min,max,mid;
                    } ray_bound_type;

//
// scene eye
//
// this contains the eye position in a scene object.
//

typedef struct		{
						float						min_dist,max_dist;
						ray_point_type				pnt;
						ray_matrix_type				matrix;
					} ray_eye_type;

//
// scene meshes
//
// these are the meshes inside of each scene object.  They contain
// vertex, uv, normal, tangent, and triangle lists.
//

typedef struct		{
						int							count;
						ray_point_type				*vertexes;
					} ray_vertex_block;

typedef struct		{
						int							count;
						ray_uv_type					*uvs;
					} ray_uv_block;

typedef struct		{
						int							count;
						ray_vector_type				*normals;
					} ray_normal_block;

typedef struct		{
						int							count;
						ray_vector_type				*tangents;
					} ray_tangent_block;

typedef struct		{
						int							vertex,uv,
													normal,tangent;
					} ray_polygon_index_type;

typedef struct		{
						ray_polygon_index_type		idxs[3];
						ray_vector_type				v1,v2;
						ray_bound_type				bound;
					} ray_trig_type;

typedef struct		{
						int							count;
						ray_trig_type				*trigs;
					} ray_trig_block;

typedef struct		{
						int							material_idx,nvertex,
													mm_level;
						ray_polygon_index_type		idxs[8];
						ray_trig_block				trig_block;
						ray_bound_type				bound;
					} ray_poly_type;

typedef struct		{
						int							count;
						ray_poly_type				*polys;
					} ray_poly_block;

typedef struct		{
						int							id;
						bool						hidden;
						unsigned long				flags;
						unsigned char				light_collide_mask[ray_max_scene_light];
						ray_color_type				tint_col;
						ray_vertex_block			vertex_block;
						ray_uv_block				uv_block;
						ray_normal_block			normal_block;
						ray_tangent_block			tangent_block;
						ray_poly_block				poly_block;
						ray_bound_type				bound;
					} ray_mesh_type;

typedef struct		{
						int							count,next_id;
						ray_mesh_type*				meshes[ray_max_scene_mesh];
					} ray_mesh_list;

//
// scene lights
//
// these contain the lights in a scene.
//

typedef struct		{
						float						cos_sweep;
						bool						on;
						ray_vector_type				vct;
					} ray_light_direction_type;

typedef struct		{
						int							id;
						float						intensity,exponent;
						unsigned char				mesh_collide_mask[ray_max_scene_mesh];
						ray_point_type				pnt;
						ray_color_type				col;
						ray_light_direction_type	direction;
						ray_bound_type				bound;
					} ray_light_type;

typedef struct		{
						int							count,next_id;
						ray_light_type*				lights[ray_max_scene_light];
					} ray_light_list;

//
// scene overlays
//
// these contain the 2D overlays in a scene.
//

typedef struct		{
						ray_2d_point_type			offset_pnt,pnt_size;
						ray_uv_type					uv,uv_size;
						ray_color_type				col;
					} ray_overlay_quad_type;

typedef struct		{
						int							count;
						ray_overlay_quad_type*		quads[ray_max_scene_overlay_quad];
					} ray_overlay_quad_list;

typedef struct		{
						int							id,material_idx,mm_level;
						bool						hidden;
						ray_2d_point_type			pnt,pnt_size;
						ray_overlay_quad_list		quad_list;
					} ray_overlay_type;

typedef struct		{
						int							count,next_id;
						ray_overlay_type*			overlays[ray_max_scene_overlay];
					} ray_overlay_list;

//
// the scene
//
// the scene is the main rendering object for a openRL operation.  Scenes
// contain a drawing buffer/target, lights, meshes, and eye position
//

typedef struct		{
						int								count,
														indexes[ray_max_scene_mesh];
					} ray_scene_draw_mesh_index_block;

typedef struct		{
						int								target,format,
														wid,high;
						unsigned long					*data;
					} ray_scene_buffer_type;

typedef struct		{
						bool							render_done,shutdown_done;
						void							*parent_scene;			// this is a pointer back to the parent structure, need by threading
						ray_thread						thread;
						ray_2d_point_type				pixel_start,pixel_end;
						ray_scene_draw_mesh_index_block	draw_mesh_index_block;
					} ray_draw_scene_thread_info;

typedef struct		{
						ray_mutex						scene_lock,thread_lock;	// thread_lock only needed for pthread con waits
						ray_cond						thread_cond;			// thread_cond only needed for pthread con waits
						ray_draw_scene_thread_info		thread_info[ray_render_max_thread_count];
					} ray_scene_render_type;

typedef struct		{
						int								id,thread_mode;
						ray_eye_type					eye;
						ray_color_type					ambient_col;
						ray_light_list					light_list;
						ray_mesh_list					mesh_list;
						ray_overlay_list				overlay_list;
						ray_scene_buffer_type			buffer;
						ray_scene_render_type			render;
						ray_scene_draw_mesh_index_block	draw_mesh_index_block;
					} ray_scene_type;

typedef struct		{
						int								count,next_id,
														thread_mode;
						ray_scene_type*					scenes[ray_max_scene];
					} ray_scene_list;

//
// materials
//
// materials are global to all scenes within the openRL, and can be used
// by any scene
//

typedef struct		{
						unsigned char					*color,*normal,
														*specular,*glow;
					} ray_material_mipmap_data_type;

typedef struct		{
						int								wid,high;
						float							wid_scale,high_scale;
						ray_material_mipmap_data_type	data;
					} ray_material_mipmap_type;

typedef struct		{
						int								count;
						ray_material_mipmap_type		mipmaps[ray_mipmap_max_level];
					} ray_material_mipmap_list;

typedef struct		{
						int								id,wid,high,
														alpha_type;
						float							shine_factor,glow_factor,
														refract_factor;
						bool							no_alpha;
						ray_material_mipmap_list		mipmap_list;
					} ray_material_type;

typedef struct		{
						int								count,next_id;
						ray_material_type*				materials[ray_max_material];
					} ray_material_list;

//
// rendering pixel data
//

typedef struct		{
						ray_vector_type					normal,tangent,binormal;
					} ray_material_pixel_surface_type;
typedef struct		{
						bool							on;
						ray_color_type					rgb;
					} ray_material_pixel_col_type;

typedef struct		{
						float							shine_factor;
						ray_material_pixel_surface_type	surface;
						ray_material_pixel_col_type		color,normal,specular,glow;
					} ray_material_pixel_type;

//
// global ray
//
// this structure is the global structure that contains all the structures
// within an OpenRL engine
//

typedef struct		{
						int							thread_count;
					} ray_settings_type;

typedef struct		{
						ray_settings_type			settings;
						ray_scene_list				scene_list;
						ray_material_list			material_list;
					} ray_global_type;

//
// internal structures
//

typedef struct		{
						int							mesh_idx,poly_idx;
					} ray_collision_skip_type;

typedef struct		{
						int							count;
						ray_collision_skip_type		skips[ray_max_bounce];
					} ray_collision_skip_block;

typedef struct		{
						int							mesh_idx,poly_idx,trig_idx;
						float						max_t,t,u,v;
						bool						in_bounce;
						ray_collision_skip_block	skip_block;
					} ray_collision_type;

//
// internal interfaces
//

extern unsigned long ray_create_ulong_color_from_float(ray_color_type *col);
extern unsigned long ray_create_ulong_color_from_float_no_alpha(ray_color_type *col);
extern void ray_create_float_color_from_ulong(unsigned long ul,ray_color_type *rgb);
extern void ray_create_float_color_from_ulong_no_alpha(unsigned long ul,ray_color_type *rgb);
extern unsigned long ray_create_ulong_black(void);

extern inline void ray_vector_normalize(ray_vector_type *v);
extern inline void ray_vector_create_from_points(ray_vector_type *v,ray_point_type *p1,ray_point_type *p2);
extern inline void ray_vector_scalar_multiply(ray_vector_type *sm,ray_vector_type *v,float f);
extern inline void ray_vector_add(ray_vector_type *v,ray_vector_type *v1,ray_vector_type *v2);
extern inline void ray_vector_cross_product(ray_vector_type *cp,ray_vector_type *v1,ray_vector_type *v2);
extern inline float ray_vector_dot_product(ray_vector_type *v1,ray_vector_type *v2);
extern inline void ray_vector_find_line_point_for_T(ray_point_type *p,ray_vector_type *v,float t,ray_point_type *lp);

extern void ray_to_bound(ray_point_type *p,ray_vector_type *v,ray_bound_type *bnd);
extern bool ray_bound_bound_collision(ray_bound_type *bnd_1,ray_bound_type *bnd_2);
extern bool ray_bound_ray_collision(ray_point_type *p,ray_vector_type *v,ray_bound_type *bnd);

extern float ray_distance_between_points(ray_point_type *p1,ray_point_type *p2);
extern unsigned char* ray_bitmap_reduction(int factor,int wid,int high,unsigned char *data);

extern void ray_scene_3D_to_2D_point(ray_scene_type *scene,ray_point_type *pnt_3d,ray_2d_point_type *pnt_2d);

extern void ray_scene_clear_threads(ray_scene_type *scene);
extern void ray_scene_wait_shutdown_threads(ray_scene_type *scene);
extern void ray_scene_resume_threads(ray_scene_type *scene,int mode);
extern void ray_scene_release_threads(ray_scene_type *scene);
extern bool ray_scene_create_threads(ray_scene_type *scene);

extern void ray_precalc_mesh_bounds(ray_mesh_type *mesh);
extern void ray_precalc_polygon_bounds(ray_mesh_type *mesh,ray_poly_type *poly);
extern void ray_precalc_triangle_bounds(ray_mesh_type *mesh,ray_trig_type *trig);
extern void ray_precalc_light_bounds(ray_light_type *light);
extern void ray_precalc_triangle_vectors(ray_mesh_type *mesh,ray_trig_type *trig);
extern void ray_precalc_render_scene_setup(ray_scene_type *scene);
extern void ray_precalc_render_scene_thread_setup(ray_scene_type *scene,ray_draw_scene_thread_info *thread_info);

extern void ray_get_material_rgb(ray_scene_type *scene,ray_point_type *eye_pnt,ray_point_type *trig_pnt,ray_collision_type *collision,ray_material_pixel_type *pixel);
extern float ray_get_material_alpha(ray_scene_type *scene,ray_point_type *eye_pnt,ray_point_type *trig_pnt,ray_collision_type *collision);
extern void ray_get_material_normal(ray_scene_type *scene,ray_point_type *eye_pnt,ray_point_type *trig_pnt,ray_collision_type *collision,ray_vector_type *normal);
extern bool ray_get_overlay_rgb(ray_scene_type *scene,int x,int y,ray_color_type *col);
extern void ray_overlay_setup_all(ray_scene_type *scene);

extern int ray_scene_get_index(int sceneId);
extern int ray_material_get_index(int materialId);

extern void ray_render_clear_threads(ray_scene_type *scene);

#ifndef WIN32
	extern void* ray_render_thread(void *arg);
#else
	extern unsigned __stdcall ray_render_thread(void *arg);
#endif


