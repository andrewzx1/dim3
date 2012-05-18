/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto Generator Defines for Editor

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
// constants
//

#define ag_max_shape							128
#define ag_max_shape_point						64
#define ag_max_shape_connector					32
#define ag_max_shape_poly						64

#define ag_max_room								256

#define ag_connector_type_min_x					0
#define ag_connector_type_max_x					1
#define ag_connector_type_min_z					2
#define ag_connector_type_max_z					3

#define ag_floor_left							0
#define ag_floor_right							1
#define ag_floor_top							2
#define ag_floor_bottom							3

//
// textures
//

#define ag_texture_wall							0
#define ag_texture_floor						1
#define ag_texture_additional_floor				2
#define ag_texture_ceiling						3

//
// structures
//

typedef struct		{
						int						npt,v[4];
						bool					floor_flags[4];
					} ag_shape_poly_type;

typedef struct		{
						int						type,dist,v[2];
						bool					prefer;
						d3pnt					top_left;
					} ag_shape_connector_type;

typedef struct		{
						int						nvertex,npoly,nconnector;
						char					name[name_str_len];
						bool					single_floor,spawn_spots;
						d3pnt					vertexes[ag_max_shape_point];
						ag_shape_poly_type		polys[ag_max_shape_poly];
						ag_shape_connector_type	connectors[ag_max_shape_connector];
					} ag_shape_type;

typedef struct		{
						int						shape_idx,mesh_idx;
						bool					clip,multi_story,extra,
												connectors_used[ag_max_shape_connector];
						d3pnt					min,max;
						d3vct					size;
					} ag_room_type;

typedef struct		{
						int						room_min_count,room_max_count,
												room_sz,room_high,
												story_count;
					} ag_size_type;

typedef struct		{
						bool					mirror,auto_connect;
					} ag_option_type;

typedef struct		{
						int						nshape,nroom;
						ag_size_type			size;
						ag_option_type			option;
						ag_shape_type			*shapes;
						ag_room_type			*rooms;
					} ag_state_type;

