/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Video Definitions Header

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

//
// vertex objects
//

#define view_vertex_object_count						32

//
// back renderers
//

#define back_render_texture_pixel_size					256

//
// shadows
//

#define max_shadow_render								24
#define shadow_max_trigs								10240
#define shadow_max_vertexes								10240
#define shadow_max_polys								128
#define shadow_max_ray_trace_vertexes					10240
#define shadow_infinite_light_adjust					100

typedef struct			{
							int							type,idx;
							d3pnt						pnt;
							d3ang						ang;
						} shadow_render_item_type;

typedef struct			{
							double						intensity;
							d3pnt						pnt;
						} shadow_render_light_type;

typedef struct			{
							int							start_idx,count_idx;
							poly_pointer_type			ptr;
						} shadow_render_poly_type;

typedef struct			{
							int							npoly,nindex,nvertex;
							unsigned short				*indexes;
							float						*vp,*cp;
							shadow_render_item_type		cur_item;
							shadow_render_light_type	cur_light;
							shadow_render_poly_type		*polys;
						} shadow_render_type;

//
// screen sizes
//

#define max_screen_size									64

typedef struct		{
						int								wid,high;
					} screen_size_type;
					
//
// render info
//
 
#define max_fps											100

typedef struct		{
						int								view_x,view_y,
														monitor_x_sz,monitor_y_sz,
														monitor_refresh_rate,
														texture_unit_count,texture_max_size,
														nscreen_size;
						char							name[64],ext_string[8192];
						screen_size_type				screen_sizes[max_screen_size];
					} render_info_type;

//
// fonts
//

#define tx_left											0
#define tx_center										1
#define tx_right										2
