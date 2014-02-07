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

#define ag_max_room								256

#define ag_max_position_room_try				8

#define ag_connector_type_min_x					0
#define ag_connector_type_max_x					1
#define ag_connector_type_min_z					2
#define ag_connector_type_max_z					3

#define ag_story_place_none						0
#define ag_story_place_left						1
#define ag_story_place_right					2
#define ag_story_place_top						3
#define ag_story_place_bottom					4
#define ag_story_place_all						5

//
// connection sides
//

#define ag_connect_side_top						0
#define ag_connect_side_bottom					1
#define ag_connect_side_left					2
#define ag_connect_side_right					3

//
// generation constants
//

#define ag_story_change_chance					30			// out of 100

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
						int						p1_idx,p2_idx;
						bool					on;
					} ag_room_flat_type;

typedef struct		{
						ag_room_flat_type		lft,rgt,top,bot;
					} ag_room_flat_side_type;

typedef struct		{
						int						nvertex,mesh_idx;
						d3pnt					min,max;
						d3pnt					vertexes[8];
						d3vct					size;
						ag_room_flat_side_type	flat;
					} ag_room_type;

typedef struct		{
						int						room_count,story_count;
					} ag_option_type;

typedef struct		{
						int						seed;
						ag_option_type			option;
						ag_room_type			*rooms;
					} ag_state_type;

