/****************************** File *********************************

Module: dim3 Math Utility
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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

/* =======================================================

      Version
      
======================================================= */

#define dim3_version					"3.0b21"

/* =======================================================

      Base Definitions
      
======================================================= */

//
// lengths
//

#define name_str_len					32
#define file_str_len					256

//
// dim3 base types
//

typedef struct		{
						int				x,y,z;
					} d3pnt;

typedef struct		{
						int				lx,rx,ty,by;
					} d3rect;
					
typedef struct		{
						float			x,y,z;
					} d3fpnt;
					
typedef struct		{
						float			x,y,z;
					} d3vct;
					
typedef struct		{
						float			x,y,z;
					} d3ang;
					
typedef struct		{
						float			r,g,b;
					} d3col;

typedef struct		{
						float			x,y;
					} d3uv;

/* =======================================================

      Math
      
======================================================= */

//
// Defines
//

#define TRIG_PI					3.14159265358979f
#define D_TRIG_PI				3.14159265358979
#define D_INV_TRIG_PI			0.31830988618379
#define ANG_to_RAD				(float)(TRIG_PI/180.0f)
#define RAD_to_ANG				(float)(180.0f/TRIG_PI)

//
// Matrix Structure
//

typedef struct		{
                        float			data[4][4];
                    } matrix_type;

//
// Functions
//

extern void time_start(void);
extern int time_get(void);

extern void random_reset(void);
extern int random_int(int max);
extern float random_float(float max);
extern bool random_boolean(void);
extern int random_int_spread(int spread);

extern void matrix_identity(matrix_type *mat);
extern void matrix_multiply(matrix_type *mat_1,matrix_type *mat_2);
extern void matrix_vertex_multiply(matrix_type *mat,float *x,float *y,float *z);
extern void matrix_vertex_multiply_ignore_transform(matrix_type *mat,float *x,float *y,float *z);
extern void matrix_angle_multiply(matrix_type *mat,float *x,float *y,float *z);
extern void matrix_rotate_x(matrix_type *mat,float ang);
extern void matrix_rotate_y(matrix_type *mat,float ang);
extern void matrix_rotate_z(matrix_type *mat,float ang);
extern void matrix_rotate_xy(matrix_type *mat,float x_ang,float y_ang);
extern void matrix_rotate_xyz(matrix_type *mat,float x_ang,float y_ang,float z_ang);
extern void matrix_rotate_zyx(matrix_type *mat,float x_ang,float y_ang,float z_ang);
extern void matrix_rotate_xzy(matrix_type *mat,float x_ang,float y_ang,float z_ang);
extern bool matrix_has_rotation(matrix_type *mat);
extern void matrix_translate(matrix_type *mat,float x,float y,float z);
extern void matrix_average(matrix_type *mat,int nmatrix,matrix_type *mats);

extern void vector_normalize(d3vct *v);
extern void vector_create(d3vct *v,int x1,int y1,int z1,int x2,int y2,int z2);
extern void vector_scalar_multiply(d3vct *v,d3vct *v1,float f);
extern void vector_add(d3vct *v,d3vct *v1,d3vct *v2);
extern void vector_subtract(d3vct *v,d3vct *v1,d3vct *v2);
extern void vector_cross_product(d3vct *v,d3vct *v1,d3vct *v2);
extern float vector_dot_product(d3vct *v1,d3vct *v2);

extern void fast_trig_setup(void);
extern float fast_atan2f(float y,float x);
extern float fast_sinf(float x);
extern float fast_cosf(float x);

extern float angle_add(float ang1,float ang2);
extern float angle_dif(float ang1,float ang2,bool *cwise);
extern void angle_get_movement(float ang,int mv,int *xadd,int *zadd);
extern void angle_get_movement_float(float ang,float fmv,float *xadd,float *zadd);
extern void angle_add_movement(float ang,int mv,int *x,int *z);
extern float angle_find(int x,int z,int tox,int toz);
extern float angle_turn_toward(float ang,float rang,float mv);
extern float angle_to_normal(int x,int z,int tox,int toz);

extern void rotate_point(int *x,int *y,int *z,int cx,int cy,int cz,float ang_x,float ang_y,float ang_z);
extern void rotate_polygon(int ptsz,int *x,int *y,int *z,int cx,int cy,int cz,float ang_x,float ang_y,float ang_z);
extern void rotate_point_center(int *x,int *y,int *z,float ang_x,float ang_y,float ang_z);
extern void rotate_polygon_center(int ptsz,int *x,int *y,int *z,float ang_x,float ang_y,float ang_z);
extern void rotate_2D_point(int *x,int *z,int cx,int cz,float ang_y);
extern void rotate_2D_point_center(int *x,int *z,float ang_y);
extern void rotate_2D_line(int *lx,int *lz,int *rx,int *rz,int cx,int cz,float ang_y);
extern void rotate_2D_polygon(int ptsz,int *x,int *z,int cx,int cz,float ang_y);

extern void line_get_movement(int x1,int y1,int z1,int x2,int y2,int z2,int mv,int *xadd,int *yadd,int *zadd);
extern bool line_2D_get_intersect(int x0,int y0,int x1,int y1,int x2,int y2,int x3,int y3,int *x,int *y);
extern bool line_2D_test_intersect(int x0,int y0,int x1,int y1,int x2,int y2,int x3,int y3);
extern void line_2D_find_angle_hit(int x,int z,float ang,int lx,int lz,int rx,int rz,int *kx,int *kz);
extern void line_2D_find_inside_infinite(int x,int z,int wid,int lx,int lz,int rx,int rz,int *klx,int *klz,int *krx,int *krz);
extern bool line_2D_all_points_in_line(int ptsz,int *px,int *pz,float slop);

extern void polygon_get_box(int ptsz,int *px,int *py,int *pz,int *p_min_x,int *p_max_x,int *p_min_y,int *p_max_y,int *p_min_z,int *p_max_z);
extern void polygon_find_center(int ptsz,int *px,int *py,int *pz,int *mx,int *my,int *mz);
extern void polgyon_enlarge(int ptsz,int *px,int *py,int *pz,int *ex,int *ey,int *ez,int factor);
extern void polygon_2D_get_box(int ptsz,int *px,int *py,int *p_lft,int *p_rgt,int *p_top,int *p_bot);
extern bool polygon_2D_point_inside(int ptsz,int *px,int *py,int x,int y);
extern bool polygon_2D_polygon_points_inside(int ptsz,int *px,int *py,int ptsz_2,int *px_2,int *py_2);
extern bool polygon_2D_polygon_lines_intersect(int ptsz,int *px,int *py,int ptsz_2,int *px_2,int *py_2);
extern void polygon_2D_collision_setup(int ptsz,int *px,int *py);
extern bool polygon_2D_collision_line(int lx,int lz,int rx,int rz);
extern bool polygon_2D_collision_bound_box(int lft,int rgt,int top,int bot);
extern bool polygon_2D_collision_bound_polygon(int ptsz,int *x,int *y,int lft,int rgt,int top,int bot);
extern bool polygon_2D_collision_polygon(int ptsz,int *x,int *y);
extern float polygon_get_slope_y(int ptsz,int *px,int *py,int *pz,float *ang_y);
extern int polygon_find_y(int ptsz,int *px,int *py,int *pz,int kx,int kz);
extern int polygon_find_y_outside_point(int ptsz,int *px,int *py,int *pz,int kx,int kz);

extern int area_2D_trig(int *px,int *py);
extern int area_2D_polygon(int ptsz,int *px,int *py);

extern int distance_get(int x,int y,int z,int tox,int toy,int toz);
extern bool distance_check(int x,int y,int z,int tox,int toy,int toz,int max_distance);
extern int distance_2D_get(int x,int z,int tox,int toz);

extern void clip_line_to_2D_horizontal_line(int clip_z,int *x,int *y,int *z,int x1,int y1,int z1,int x2,int y2,int z2);
extern void clip_line_to_2D_vertical_line(int clip_x,int *x,int *y,int *z,int x1,int y1,int z1,int x2,int y2,int z2);
extern void clip_extend_line_to_2D_horizontal_line(int clip_z,int *x,int *y,int *z,int x1,int y1,int z1,int x2,int y2,int z2);
extern void clip_extend_line_to_2D_vertical_line(int clip_x,int *x,int *y,int *z,int x1,int y1,int z1,int x2,int y2,int z2);
extern int clip_polygon_to_2D_rect(int ptsz,int *x,int *y,int *z,int lx,int rx,int tz,int bz);
extern int clip_extend_polygon_to_2D_rect(int ptsz,int *x,int *y,int *z,int lx,int rx,int tz,int bz);

extern float acceleration_calculate(float factor,float acceleration);

extern void string_convert_float(char *str,float f);
extern void string_safe_strcat(char *str,char *cat_str,int buf_len);

/* =======================================================

      Files and Paths
      
======================================================= */

//
// defines
//

#define file_paths_max_directory_file		1024

//
// file path structure
//

typedef struct	{
					char							app_name[256],
													proj_name[256],
													path_base[1024],
													path_app[1024],
													path_data[1024],
													path_data_2[1024];
				} file_path_setup_type;
				
//
// file directory structures
//

typedef struct	{
					int								parent_idx;
					bool							is_dir;
					char							file_name[file_str_len];
				} file_path_directory_file_type;

typedef struct	{
					int								nfile;
					file_path_directory_file_type	files[file_paths_max_directory_file];
				} file_path_directory_type;

//
// file path routines
//

extern bool file_paths_setup(file_path_setup_type *file_path_setup);

extern void file_paths_data(file_path_setup_type *file_path_setup,char *path,char *sub_path,char *file_name,char *ext_name);
extern void file_paths_data_default(file_path_setup_type *file_path_setup,char *path,char *sub_path,char *file_name,char *ext_name);
extern void file_paths_app(file_path_setup_type *file_path_setup,char *path,char *sub_path,char *file_name,char *ext_name);
extern void file_paths_base(file_path_setup_type *file_path_setup,char *path,char *file_name,char *ext_name);

extern void file_paths_documents(file_path_setup_type *file_path_setup,char *path,char *sub_path,char *file_name,char *ext_name);
extern bool file_paths_documents_exist(file_path_setup_type *file_path_setup,char *path,char *sub_path,char *file_name,char *ext_name);
extern void file_paths_preferences(char *path,char *file_name,char *ext_name);

extern file_path_directory_type* file_paths_read_directory_data(file_path_setup_type *file_path_setup,char *sub_path,char *ext_name);
extern file_path_directory_type* file_paths_read_directory_data_dir(file_path_setup_type *file_path_setup,char *sub_path,char *required_file_name);
extern file_path_directory_type* file_paths_read_directory_document(file_path_setup_type *file_path_setup,char *sub_path,char *ext_name);
extern void file_paths_close_directory(file_path_directory_type *fpd);

extern int file_paths_count_children(file_path_directory_type *fpd,int index);
extern void file_paths_get_complete_path_from_index(file_path_directory_type *fpd,int idx,char *file_name);

extern int file_paths_project_hash(char *path);

//
// zipping
//

extern unsigned char* zip_compress(unsigned char *data,unsigned long data_sz,unsigned long *p_compress_sz,char *err_str);
extern unsigned char* zip_expand(unsigned char *compress_data,unsigned long compress_data_sz,unsigned long *p_data_sz,char *err_str);

/* =======================================================

      XML
      
======================================================= */

extern bool xml_new_file(void);
extern bool xml_open_file(char *path);
extern bool xml_save_file(char *path);
extern void xml_close_file(void);
extern char* xml_get_stream(int *sz);

extern int xml_findfirstchild(char *name,int parent);
extern int xml_findrootchild(char *name);
extern int xml_findnextchild(int curtag);
extern int xml_getparent(int n);
extern int xml_countchildren(int n);
extern int xml_findflat(char *name,int offset);
extern int xml_findflatinparent(char *name,int parent,int offset);

extern bool xml_get_attribute_raw(int n,char *name,char *value,int valuesz);
extern void xml_get_attribute_text(int n,char *name,char *value,int valuesz);
extern int xml_get_attribute_int(int n,char *name);
extern int xml_get_attribute_int_default(int n,char *name,int def_int);
extern int xml_get_attribute_int_array(int n,char *name,int *value,int count);
extern int xml_get_attribute_short_array(int n,char *name,short *value,int count);
extern float xml_get_attribute_float(int n,char *name);
extern float xml_get_attribute_float_default(int n,char *name,float def_float);
extern int xml_get_attribute_float_array(int n,char *name,float *value,int count);
extern bool xml_get_attribute_boolean(int n,char *name);
extern bool xml_get_attribute_boolean_default_true(int n,char *name);
extern int xml_get_attribute_list(int n,char *name,char *list);
extern bool xml_get_attribute_2_coord_int(int n,char *name,int *x,int *y);
extern bool xml_get_attribute_2_coord_float(int n,char *name,float *x,float *y);
extern bool xml_get_attribute_2_coord_float_default(int n,char *name,float *x,float *y,float def_x,float def_y);
extern bool xml_get_attribute_3_coord_int(int n,char *name,int *x,int *y,int *z);
extern bool xml_get_attribute_3_coord_float(int n,char *name,float *x,float *y,float *z);
extern bool xml_get_attribute_4_coord_float(int n,char *name,float *r,float *g,float *b,float *a);
extern bool xml_get_attribute_color(int n,char *name,d3col *col);

extern bool xml_add_tagstart(char *name);
extern bool xml_add_tagend(bool single);
extern bool xml_add_tagclose(char *name);

extern void xml_add_attribute_text(char *name,char *value);
extern void xml_add_attribute_int(char *name,int value);
extern void xml_add_attribute_int_array(char *name,int *value,int count,bool removenegone);
extern void xml_add_attribute_short_array(char *name,short *value,int count,bool removenegone);
extern void xml_add_attribute_float(char *name,float value);
extern void xml_add_attribute_float_array(char *name,float *value,int count);
extern void xml_add_attribute_boolean(char *name,bool value);
extern void xml_add_attribute_list(char *name,char *list,int value);
extern void xml_add_attribute_2_coord_int(char *name,int x,int y);
extern void xml_add_attribute_2_coord_float(char *name,float x,float y);
extern void xml_add_attribute_3_coord_int(char *name,int x,int y,int z);
extern void xml_add_attribute_3_coord_float(char *name,float x,float y,float z);
extern void xml_add_attribute_4_coord_float(char *name,float r,float g,float b,float a);
extern void xml_add_attribute_color(char *name,d3col *col);

extern void xml_key_read_int(int setup_tag,char *name,int *value);
extern void xml_key_read_float(int setup_tag,char *name,float *value);
extern void xml_key_read_text(int setup_tag,char *name,char *value,int value_sz);
extern void xml_key_read_boolean(int setup_tag,char *name,bool *value);
extern void xml_key_read_color(int setup_tag,char *name,d3col *value);
extern void xml_key_write_int(char *name,int value);
extern void xml_key_write_float(char *name,float value);
extern void xml_key_write_text(char *name,char *value);
extern void xml_key_write_boolean(char *name,bool value);
extern void xml_key_write_color(char *name,d3col *value);

/* =======================================================

      Bitmap
      
======================================================= */

//
// alpha modes
//

#define alpha_mode_none							0
#define alpha_mode_cut_out						1
#define alpha_mode_transparent					2

//
// anisotropic modes
//

#define anisotropic_mode_none					0
#define anisotropic_mode_low					1
#define anisotropic_mode_medium					2
#define anisotropic_mode_high					3

#define anisotropic_mode_setup_list_def			{"None","Low","Medium","High",""}

//
// mipmap modes
//

#define mipmap_mode_none						0
#define mipmap_mode_bilinear					1
#define mipmap_mode_trilinear					2

#define mipmap_mode_setup_list_def				{"None","Bilinear","Trilinear",""}

//
// texture quality modes
//

#define texture_quality_mode_high				0
#define texture_quality_mode_normal				1
#define texture_quality_mode_low				2

#define texture_quality_mode_setup_list_def		{"High","Normal","Low",""}

//
// texture maximums
//

#define max_texture_frame						32				// maximum number of frames in a texture
#define max_texture_frame_mask					0x1F			// mask for maximum number of textures frames

//
// tangent space
//

typedef struct		{
						d3vct					tangent,normal;
					} tangent_space_type;

//
// bitmap structures
//

typedef struct		{
						int						wid,high,alpha_mode;
						unsigned char			*pixel_data;
						GLuint					gl_id;
					} bitmap_type;

//
// glow structure
//

typedef struct		{
						int						rate;
						float					min,max,current_color;
					} texture_glow_type;

//
// scaling structure (only used for Editor)
//

typedef struct		{
						float					x,y;
						bool					on,lock_offset;
					} texture_scale_type;
					
//
// animation structures
//

typedef struct		{
						int						current_frame,total_wait,
												wait[max_texture_frame],
												end_tick[max_texture_frame];
						bool					on;
					} texture_animate_type;

//
// texture frames
//

typedef struct		{
						char					name[file_str_len];
						bitmap_type				bitmap,bumpmap,specularmap,glowmap,
												combinemap;
					} texture_frame_type;

//
// texture structure
//

typedef struct		{
						int						shader_idx;
						float					shine_factor;
						bool					additive,pixelated,compress,
												keep_quality;
						char					material_name[name_str_len],
												shader_name[name_str_len];
						texture_animate_type	animate;
						texture_glow_type		glow;
						texture_scale_type		scale;
						texture_frame_type		frames[max_texture_frame];
					} texture_type;

//
// functions
//

extern void bitmap_new(bitmap_type *bitmap);
extern bool bitmap_open(bitmap_type *bitmap,char *path,int anisotropic_mode,int mipmap_mode,int texture_quality_mode,bool compress,bool rectangle,bool pixelated,bool scrub_black_to_alpha,bool keep_pixel_data);
extern bool bitmap_color(bitmap_type *bitmap,d3col *col);
extern bool bitmap_data(bitmap_type *bitmap,unsigned char *data,int wid,int high,bool alpha_channel,int anisotropic_mode,int mipmap_mode,bool compress,bool rectangle);
extern void bitmap_combine(bitmap_type *combinemap,bitmap_type *bitmap,bitmap_type *bumpmap,bitmap_type *specularmap,int anisotropic_mode,int mipmap_mode,bool compress);
extern void bitmap_free_pixel_data(bitmap_type *bitmap);
extern void bitmap_close(bitmap_type *bitmap);

extern void bitmap_texture_setup_animation(texture_type *texture,int texture_count,int tick);
extern void bitmap_texture_recalc_animation(texture_type *texture);

extern void bitmap_texture_clear(texture_type *texture);
extern void bitmap_texture_read_xml(texture_type *texture,int main_tag,bool read_scale);
extern void bitmap_texture_write_xml(texture_type *texture,int frame_count,bool write_scale);

extern unsigned char* bitmap_load_png_data(char *path,int *p_wid,int *p_high,bool *alpha_channel);
extern bool bitmap_write_png_data(unsigned char *data,int wid,int high,bool alpha_channel,char *path);
extern bool bitmap_check(char *path,char *err_str);
extern bool bitmap_copy(char *srce_path,char *dest_path);

/* =======================================================

      Bitmap Fonts
      
======================================================= */

typedef struct			{
							int							char_box_wid,char_box_high,char_real_high,
														char_baseline,char_per_line;
							float						gl_xoff,gl_xadd,gl_yoff,gl_yadd,char_size[90];
							bitmap_type					bitmap;
						} texture_font_size_type;

typedef struct			{
							char						name[256];
							texture_font_size_type		size_24,size_48;
						} texture_font_type;

extern void bitmap_text_initialize(texture_font_type *d3_font);
extern void bitmap_text_shutdown(texture_font_type *d3_font);

/* =======================================================

      Global Normal Modes
      
======================================================= */

#define normal_mode_none						0
#define normal_mode_out							1
#define normal_mode_in							2

/* =======================================================

      Interface
      
======================================================= */

//
// iface main defines
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
// color and font settings
//

typedef struct		{
						d3col							dialog_background,dialog_base,dialog_dimmed,dialog_outline,
														control_label,control_text,control_fill,
														control_header,control_outline,
														control_mouse_over,control_hilite,control_disabled,
														button_fill,button_text,button_outline,
														default_tint,tints[max_tint_color];
					} iface_color_type;

typedef struct		{
						int								text_size_mini,text_size_small,text_size_medium,text_size_large;
						char							interface_name[name_str_len],hud_name[name_str_len];
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

#define tx_left											0
#define tx_center										1
#define tx_right										2

#define iface_text_mini_wid_factor						0.015f
#define iface_text_small_wid_factor						0.02f
#define iface_text_medium_wid_factor					0.025f
#define iface_text_large_wid_factor						0.038f

typedef struct		{
						int								start_tick,
														fade_in_tick,life_tick,fade_out_tick;
						bool							on;
					} iface_item_fade_type;
					
typedef struct		{
						int								x_add,y_add,col,row,count;
						bool							on;
					} iface_bitmap_repeat_type;
					
typedef struct		{
						int								x,y,x_size,y_size,image_idx,show_tick;
						float							alpha,rot;
						char							name[name_str_len],filename[file_str_len];
						bool							show,old_show,
														flash,flip_horz,flip_vert,team_tint;
						iface_bitmap_repeat_type		repeat;
						iface_image_animation_type		animate;
						iface_item_fade_type			fade;
					} iface_bitmap_type;

typedef struct		{
						int								nbitmap;
						iface_bitmap_type				*bitmaps;
					} iface_bitmap_list;

typedef struct		{
						int								x,y,size,just,special;
						float							alpha;
						char							name[name_str_len],data[max_hud_text_str_sz];
						bool							show,old_show,has_return;
						d3col							color;
						iface_item_fade_type			fade;
					} iface_text_type;

typedef struct		{
						int								ntext;
						iface_text_type					*texts;
					} iface_text_list;
					
typedef struct		{
						int								x,y,x_size,y_size;
						float							fill_alpha,outline_alpha,value;
						char							name[name_str_len];
						bool							show,old_show,outline,vert;
						d3col							fill_start_color,fill_end_color,outline_color;
					} iface_bar_type;

typedef struct		{
						int								nbar;
						iface_bar_type					*bars;
					} iface_bar_list;

//
// radar
//

#define max_radar_icon									16

typedef struct		{
						int								size,image_idx;
						char							name[name_str_len],bitmap_name[name_str_len];
						bool							rot;
					} iface_radar_icon_type;

typedef struct		{
						int								x,y,display_radius,view_radius,
														background_image_idx,nicon,no_motion_fade;
						bool							on,rot,team_tint;
						char							background_bitmap_name[name_str_len];
						iface_radar_icon_type			icons[max_radar_icon];
					} iface_radar_type;
			
//
// menus
//

#define max_menu_item									24
#define max_menu_item_data_sz							64

typedef struct		{
						int								item_id;
						char							data[max_menu_item_data_sz],sub_menu[name_str_len];
						bool							multiplayer_disable,quit;
					} iface_menu_item_type;
					
typedef struct		{
						int								nitem;
						char							name[name_str_len];
						iface_menu_item_type			items[max_menu_item];
					} iface_menu_type;

typedef struct		{
						int								nmenu;
						iface_menu_type					*menus;
					} iface_menu_list;
					
//
// chooser
//

#define max_chooser_piece								64
#define max_chooser_frame_text_sz						64
#define max_chooser_text_data_sz						256
#define max_chooser_button_text_sz						64
#define max_chooser_sub_txt								16

#define chooser_piece_type_text							0
#define chooser_piece_type_item							1
#define chooser_piece_type_model						2
#define chooser_piece_type_button						3

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
						bool							clickable,used_in_override;
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
						char							name[name_str_len],template_name[name_str_len];
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
						iface_intro_model_list			model_list;
					} iface_intro_type;

//
// fades
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
						int								image_idx,chain_idx,
														count,trail_count,
														spread_offset,life_msec,
														start_pixel_size,end_pixel_size,
														current_variation_idx;
						float							start_gravity,gravity_add,
														start_alpha,end_alpha,reduce_pixel_fact,
														trail_step,ambient_factor;
						char							name[name_str_len],bitmap_name[name_str_len],
														chain_name[name_str_len];
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
						int								image_idx,
														min_dist,max_dist,
														min_size,max_size;
						float							min_alpha,max_alpha;
						bool							no_clip_object,no_clip_self;
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
// configurable control names
//

#define control_names			{"move_forward","move_backward","turn_left","turn_right", \
								 "sidestep_left","sidestep_right","run", \
								 "jump","duck","up","down","enter-exit", \
								 "next_weapon","previous_weapon", \
								 "fire_0","fire_1","fire_2","fire_3", \
								 "zoom","zoom_increase","zoom_decrease", \
								 "target_next","target_previous", \
								 "weapon_0","weapon_1","weapon_2","weapon_3","weapon_4", \
								 "weapon_5","weapon_6","weapon_7","weapon_8","weapon_9", \
								 "weapon_10","weapon_11","weapon_12","weapon_13","weapon_14", \
								 "weapon_15","weapon_16","weapon_17","weapon_18","weapon_19", \
								 "game_0","game_1","game_2","game_3","game_4", \
								 "game_5","game_6","game_7","game_8","game_9", \
								 "game_10","game_11","game_12","game_13","game_14", \
								 "game_15","game_16","game_17","game_18","game_19", \
								 "player_0","player_1","player_2","player_3","player_4", \
								 "player_5","player_6","player_7","player_8","player_9", \
								 "player_10","player_11","player_12","player_13","player_14", \
								 "player_15","player_16","player_17","player_18","player_19", \
								 "console","menu","quick_save","save","load", \
								 "message","network_score", \
								 "debug_status","debug_game","debug_screenshot", \
								 ""}

//
// configurable controls
//

#define ncontrol					93				// total number of controls

#define nc_move_forward				0
#define nc_move_backward			1
#define nc_turn_left				2
#define nc_turn_right				3

#define nc_sidestep_left			4
#define nc_sidestep_right			5

#define nc_run						6

#define nc_jump						7
#define nc_duck						8

#define nc_up						9
#define nc_down						10

#define nc_enter_exit				11

#define nc_next_weapon				12
#define nc_previous_weapon			13

#define nc_fire_0					14
#define nc_fire_1					15
#define nc_fire_2					16
#define nc_fire_3					17

#define nc_weapon_zoom				18
#define nc_weapon_zoom_increase		19
#define nc_weapon_zoom_decrease		20

#define nc_weapon_target_next		21
#define nc_weapon_target_previous	22

#define nc_weapon_0					23
#define nc_weapon_1					24
#define nc_weapon_2					25
#define nc_weapon_3					26
#define nc_weapon_4					27
#define nc_weapon_5					28
#define nc_weapon_6					29
#define nc_weapon_7					30
#define nc_weapon_8					31
#define nc_weapon_9					32
#define nc_weapon_10				33
#define nc_weapon_11				34
#define nc_weapon_12				35
#define nc_weapon_13				36
#define nc_weapon_14				37
#define nc_weapon_15				38
#define nc_weapon_16				39
#define nc_weapon_17				40
#define nc_weapon_18				41
#define nc_weapon_19				42

#define nc_game_0					43
#define nc_game_1					44
#define nc_game_2					45
#define nc_game_3					46
#define nc_game_4					47
#define nc_game_5					48
#define nc_game_6					49
#define nc_game_7					50
#define nc_game_8					51
#define nc_game_9					52
#define nc_game_10					53
#define nc_game_11					54
#define nc_game_12					55
#define nc_game_13					56
#define nc_game_14					57
#define nc_game_15					58
#define nc_game_16					59
#define nc_game_17					60
#define nc_game_18					61
#define nc_game_19					62

#define nc_player_0					63
#define nc_player_1					64
#define nc_player_2					65
#define nc_player_3					66
#define nc_player_4					67
#define nc_player_5					68
#define nc_player_6					69
#define nc_player_7					70
#define nc_player_8					71
#define nc_player_9					72
#define nc_player_10				73
#define nc_player_11				74
#define nc_player_12				75
#define nc_player_13				76
#define nc_player_14				77
#define nc_player_15				78
#define nc_player_16				79
#define nc_player_17				80
#define nc_player_18				81
#define nc_player_19				82

#define nc_console					83
#define nc_menu						84
#define nc_quick_save				85
#define nc_save						86
#define nc_load						87

#define nc_message					88
#define nc_score					89

#define nc_debug_status				90
#define nc_debug_game				91
#define nc_debug_screenshot			92

//
// control ranges
//

#define nc_respawn_start			0
#define nc_respawn_end				17

#define nc_fire_start				14
#define nc_fire_end					17

#define nc_weapon_start				23
#define nc_weapon_end				42

#define nc_game_start				43
#define nc_game_end					62

#define nc_player_start				63
#define nc_player_end				82

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
					} iface_simple_save_type;

typedef struct		{
						iface_simple_save_type			saves[max_simple_save_spot];
					} iface_simple_save_list;

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
					} iface_character_item_type;

typedef struct		{
						int								ncharacter;
						iface_character_item_type		characters[max_character];
					} iface_character_type;
	
typedef struct		{
						char							name[name_str_len];
					} iface_net_bot_type;

typedef struct		{
						bool							on;
						iface_net_bot_type				bots[max_net_bot];
					} iface_net_bots_type;

typedef struct		{
						char							name[name_str_len],descript[64];
					} iface_net_option_type;

typedef struct		{
						int								noption;
						iface_net_option_type			options[max_net_option];
					} iface_net_options_type;
					
typedef struct		{
						int								port;
						char							host[64],url[256];
					} iface_net_news_type;

//
// network game setup
//

typedef struct		{
						char							player_script[file_str_len],
														bot_script[file_str_len];
					} iface_net_game_script_type;

typedef struct		{
						bool							coop_spot,spawn_spot,team_spot;
					} iface_net_game_spawn_type;

typedef struct		{
						int								kill,death,suicide,goal;
					} iface_net_game_score_type;
					
typedef struct		{
						char							name[name_str_len];
						bool							use_teams,monsters;
						iface_net_game_script_type		script;
						iface_net_game_spawn_type		spawn;
						iface_net_game_score_type		score;
					} iface_net_game_type;

typedef struct		{
						int								ngame;
						iface_net_game_type				games[max_net_game];
					} iface_net_games_type;


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
						iface_simple_save_list			simple_save_list;
						iface_character_type			character;
						iface_net_bots_type				net_bot;
						iface_net_games_type			net_game;
						iface_net_options_type			net_option;
						iface_net_news_type				net_news;
					} iface_type;
					
extern bool iface_initialize(iface_type *iface,file_path_setup_type *path_setup);
extern void iface_shutdown(iface_type *iface);

extern void iface_default_settings(iface_type *iface);

extern iface_bitmap_type* iface_bitmaps_find(iface_type *iface,char *name);
extern iface_text_type* iface_texts_find(iface_type *iface,char *name);
extern iface_bar_type* iface_bars_find(iface_type *iface,char *name);
extern int iface_menu_find_idx(iface_type *iface,char *name);
extern int iface_chooser_find_idx(iface_type *iface,char *name);
extern int iface_chooser_find_piece_idx(iface_chooser_type *chooser,int id);
extern void iface_text_set(iface_text_type *text,char *data);
extern int iface_crosshair_find(iface_type *iface,char *name);
extern int iface_halo_find(iface_type *iface,char *name);
extern void iface_text_set(iface_text_type *text,char *data);
extern void iface_bitmaps_hide_all(iface_type *iface);
extern void iface_texts_hide_all(iface_type *iface);
extern void iface_bars_hide_all(iface_type *iface);

extern void iface_read(iface_type *iface);
extern void iface_read_settings_project_name(file_path_setup_type *path_setup);
extern bool iface_write(iface_type *iface);

extern void simple_save_xml_read(iface_type *iface);
extern void simple_save_xml_write(iface_type *iface);


