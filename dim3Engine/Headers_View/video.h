/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Video Functions Header

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
	
	// init and shutdown
	
extern bool gl_in_window_mode(void);
extern void gl_setup_context(void);
extern bool gl_initialize(int screen_wid,int screen_high,bool lock_fps_refresh,int fsaa_mode,bool reset,char *err_str);
extern void gl_shutdown(void);

	// drawables
	
extern bool gl_is_size_widescreen(int wid,int high);
extern bool gl_is_screen_widescreen(void);
extern bool gl_screen_shot(int lft_x,int top_y,int wid,int high,bool thumbnail,char *path);

	// settings
	
extern void gl_check_initialize(void);
extern bool gl_check_frame_buffer_ok(void);
extern bool gl_check_fsaa_ok(void);
extern bool gl_check_texture_compress_ok(void);
extern bool gl_check_texture_anisotropic_filter_ok(void);
extern bool gl_check_texture_compress_ok(void);
extern bool gl_check_texture_generate_mipmaps_ok(void);
extern bool gl_check_texture_rectangle_ok(void);
extern bool gl_check_shader_ok(void);

	// lights

extern void gl_lights_compile(int tick);
extern view_light_spot_type* gl_light_find_closest_light(double x,double y,double z);
extern void gl_lights_calc_ambient_color(d3col *col);
extern void gl_lights_calc_color(double x,double y,double z,float *cf);
extern void gl_lights_calc_color_light_cache(int count,int *indexes,bool skip_light_map,double x,double y,double z,float *cf);
extern void gl_lights_build_poly_light_list(int mesh_idx,map_mesh_poly_type *poly,view_light_list_type *light_list);
extern void gl_lights_build_liquid_light_list(map_liquid_type *liq,view_light_list_type *light_list);
extern void gl_lights_build_model_light_list(model_draw *draw,view_light_list_type *light_list);

	// vbos

extern void view_create_vertex_objects(void);
extern void view_dispose_vertex_objects(void);

extern void view_init_map_vertex_object(int sz);
extern float* view_bind_map_map_vertex_object(void);
extern void view_bind_map_vertex_object(void);
extern void view_unmap_map_vertex_object(void);
extern void view_unbind_map_vertex_object(void);
extern void view_init_map_index_object(int sz);
extern unsigned int* view_bind_map_map_index_object(void);
extern void view_bind_map_index_object(void);
extern void view_unmap_map_index_object(void);
extern void view_unbind_map_index_object(void);

extern float* view_bind_map_liquid_vertex_object(int sz);
extern void view_unmap_liquid_vertex_object(void);
extern void view_unbind_liquid_vertex_object(void);
extern unsigned int* view_bind_map_liquid_index_object(int sz);
extern void view_unmap_liquid_index_object(void);
extern void view_unbind_liquid_index_object(void);

extern float* view_bind_map_sky_vertex_object(int sz);
extern void view_bind_sky_vertex_object(void);
extern void view_unmap_sky_vertex_object(void);
extern void view_unbind_sky_vertex_object(void);

extern float* view_bind_map_next_vertex_object(int sz);
extern void view_unmap_current_vertex_object(void);
extern void view_unbind_current_vertex_object(void);
extern unsigned short* view_bind_map_next_index_object(int sz);
extern void view_unmap_current_index_object(void);
extern void view_unbind_current_index_object(void);

extern void view_draw_next_vertex_object_2D_tint_screen(void);
extern void view_draw_next_vertex_object_2D_color_poly(int x0,int y0,d3col *col0,int x1,int y1,d3col *col1,int x2,int y2,d3col *col2,int x3,int y3,d3col *col3,float alpha);
extern void view_draw_next_vertex_object_2D_color_quad(d3col *col,float alpha,int lft,int rgt,int top,int bot);
extern void view_draw_next_vertex_object_2D_color_trig(d3col *col,float alpha,int lft,int rgt,int top,int bot,bool up);
extern void view_draw_next_vertex_object_2D_line(d3col *col,float alpha,int x0,int y0,int x1,int y1);
extern void view_draw_next_vertex_object_2D_line_poly(d3col *col,float alpha,int x0,int y0,int x1,int y1,int x2,int y2,int x3,int y3);
extern void view_draw_next_vertex_object_2D_line_quad(d3col *col,float alpha,int lft,int rgt,int top,int bot);
extern void view_draw_next_vertex_object_2D_line_trig(d3col *col,float alpha,int lft,int rgt,int top,int bot,bool up);
extern void view_draw_next_vertex_object_3D_line(d3col *col,float alpha,int x0,int y0,int z0,int x1,int y1,int z1);
extern void view_draw_next_vertex_object_3D_line_cube(d3col *col,float alpha,int *px,int *py,int *pz);
extern void view_draw_next_vertex_object_2D_texture_quad(GLuint gl_id,d3col *col,float alpha,int lft,int rgt,int top,int bot,float gx,float gy);
extern void view_draw_next_vertex_object_2D_texture_quad_rectangle(GLuint gl_id,float alpha,int lft,int rgt,int top,int bot,int pixel_wid,int pixel_high);

	// shaders

extern void gl_shader_code_clear(shader_type *shader);
extern bool gl_shader_code_compile(shader_type *shader,char *vertex_data,char *fragment_data,char *err_str);
extern void gl_shader_code_shutdown(shader_type *shader);
extern void gl_shader_attach_map(void);
extern void gl_shader_attach_model(model_type *mdl);
extern void gl_shader_draw_scene_initialize_code(shader_type *shader);
extern void gl_shader_draw_scene_initialize(void);
extern void gl_shader_draw_start(void);
extern void gl_shader_draw_end(void);
extern void gl_shader_texture_override(GLuint gl_id,float alpha);
extern void gl_shader_draw_execute(bool map_shader,texture_type *texture,int txt_idx,int frame,int lmap_txt_idx,bool light_map,float alpha,view_light_list_type *light_list,d3pnt *pnt,d3col *tint_col,tangent_space_type *tangent_space,model_draw_vbo_offset_type *vbo_offset);

	// core shaders

extern bool gl_core_shader_initialize(char *err_str);
extern void gl_core_shader_shutdown(void);
extern void gl_core_shader_draw_scene_initialize(void);
extern shader_type* gl_core_shader_find_ptr(int nlight,bool map_shader,texture_type *texture,bool light_map);

	// user shaders

extern bool gl_user_shader_initialize(char *err_str);
extern void gl_user_shader_shutdown(void);
extern void gl_user_shader_draw_scene_initialize(void);
extern int gl_user_shader_find(char *name);

	// full screen shaders

extern void gl_fs_shader_map_start(void);
extern void gl_fs_shader_map_end(void);
extern bool gl_fs_shader_start(char *shader_name,int life_msec,char *err_str);
extern void gl_fs_shader_end(void);
extern void gl_fs_shader_render_begin(void);
extern void gl_fs_shader_render_finish(void);

	// back renderers
	
extern void gl_back_render_initialize(void);
extern void gl_back_render_shutdown(void);
extern void gl_back_render_map_start(void);
extern void gl_back_render_map_end(void);
extern void gl_back_render_frame_start(void);
extern bool gl_back_render_get_texture(char *node_name,GLuint *txt_id,float *alpha);

	// view
	
extern void gl_frame_clear(bool in_view);
extern void gl_frame_swap(void);
extern void gl_3D_view(void);
extern void gl_3D_rotate(d3pnt *pnt,d3ang *ang);
extern void gl_2D_view_screen(void);
extern void gl_2D_view_interface(void);
extern void gl_setup_project(void);
extern bool gl_project_in_view_z(int x,int y,int z);
extern void gl_project_point(int *x,int *y,int *z);
extern void gl_project_poly(int ptsz,int *x,int *y,int *z);
extern float gl_project_get_depth(int x,int y,int z);
extern void gl_unproject_point(float fx,float fy,float fz,int *x,int *y,int *z);
extern void gl_project_to_eye_coordinates(float *x,float *y,float *z);
extern void gl_project_fix_rotation(int *x,int *y,int *z);

	// textures
	
extern void gl_texture_initialize(void);
extern void gl_texture_shutdown(void);
extern void gl_texture_bind(int unit,GLuint txt_id);
extern void gl_texture_clear(int unit);
extern void gl_texture_opaque_start(void);
extern void gl_texture_opaque_end(void);
extern void gl_texture_opaque_set(GLuint txt_id);
extern void gl_texture_opaque_light_map_start(void);
extern void gl_texture_opaque_light_map_end(void);
extern void gl_texture_opaque_light_map_set(GLuint txt_id,GLuint lmap_txt_id);
extern void gl_texture_transparent_start(void);
extern void gl_texture_transparent_end(void);
extern void gl_texture_transparent_set(GLuint txt_id,float alpha);
extern void gl_texture_transparent_light_map_start(void);
extern void gl_texture_transparent_light_map_end(void);
extern void gl_texture_transparent_light_map_set(GLuint txt_id,GLuint lmap_txt_id,float alpha);
extern void gl_texture_glow_start(void);
extern void gl_texture_glow_end(void);
extern void gl_texture_glow_set(GLuint txt_id,GLuint glow_id,float glow_color);
extern void gl_texture_decal_start(void);
extern void gl_texture_decal_end(void);
extern void gl_texture_decal_set(GLuint txt_id,float r,float g,float b,float alpha);
extern void gl_texture_simple_start(void);
extern void gl_texture_simple_end(void);
extern void gl_texture_simple_set(GLuint txt_id,bool clamp,float r,float g,float b,float alpha);
extern void gl_texture_movie_start(GLuint txt_id);
extern void gl_texture_movie_end(void);

	// text

extern void gl_text_initialize(void);
extern void gl_text_shutdown(void);
extern int gl_text_get_char_height(int text_size);
extern int gl_text_get_string_width(int text_font,int text_size,char *str);
extern void gl_text_start(int text_font,int text_size);
extern void gl_text_end(void);
extern void gl_text_draw(int x,int y,char *txt,int just,bool vcenter,d3col *col,float alpha);

	// images

extern void view_images_initialize(void);
extern void view_images_shutdown(void);
extern int view_images_load_single(char *path,bool rectangle,bool simple);
extern void view_images_free_single(int idx);
extern void view_images_cached_load(void);
extern void view_images_cached_free(void);
extern bool view_images_is_empty(int idx);
extern bitmap_type* view_images_get_bitmap(int idx);
extern unsigned long view_images_get_gl_id(int idx);
