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
// connection types
//

#define ag_connect_type_normal					0
#define ag_connect_type_door					1
#define ag_connect_type_arch					2
#define ag_connect_type_stairs					3

//
// stair directions
//

#define ag_stair_dir_top						0
#define ag_stair_dir_bottom						1
#define ag_stair_dir_left						2
#define ag_stair_dir_right						3

//
// stair types
//

#define ag_stair_type_one_side					0
#define ag_stair_type_middle					1
#define ag_stair_type_full						2

//
// decorations
//

#define ag_decoration_type_none					0
#define ag_decoration_type_columns				1
#define ag_decoration_type_box_stacks			2
#define ag_decoration_type_equipment			3
#define ag_decoration_type_trench				4

#define ag_decoration_type_vertex_none			0
#define ag_decoration_type_vertex_column_circle	1
#define ag_decoration_type_vertex_column_square	2

#define ag_dectoration_column_config_square		0
#define ag_dectoration_column_config_diamond	1
#define ag_dectoration_column_config_circle		2

//
// textures
//

#define ag_texture_wall							0
#define ag_texture_floor						1
#define ag_texture_ceiling						2
#define ag_texture_connect						3
#define ag_texture_door							4
#define ag_texture_decoration_pillar			5
#define ag_texture_decoration_box				6
#define ag_texture_equipment					7

#define ag_texture_required_count				8			// number of textures that must be present to run

//
// dimensions and positions
//

#define ag_flat_connector_min_width				10000

#define ag_size_room_width_start				20000
#define ag_size_room_width_extra				10000

#define ag_map_bottom_start_y					20000
#define ag_size_room_high						10000
#define ag_size_floor_high						1000

#define ag_size_door_min_width					25000
#define ag_size_door_margin						400

#define ag_size_stair_length_start				25000
#define ag_size_stair_length_extra				10000
#define ag_size_stair_count						15

#define ag_size_pillar_wid						1500
#define ag_size_bump_high						400

#define ag_size_stack_box						2500
#define ag_count_stack_box_start				2
#define ag_count_stack_box_extra				2
#define ag_count_stack_box_row_start			3
#define ag_count_stack_box_row_extra			3
#define ag_count_stack_box_row_min				2

#define ag_size_column_normal_wid				1500
#define ag_size_column_vertex_wid_start			2000
#define ag_size_column_vertex_wid_extra			1000

#define ag_size_equipment_separate_width		1000
#define ag_size_equipment_high_start			5000
#define ag_size_equipment_high_extra			2000

#define ag_size_trench_high						800

#define ag_size_light_width_start				30000
#define ag_size_light_width_extra				15000
#define ag_size_light_fixture_wid				500

#define ag_window_min_wid						10000
#define ag_window_min_high						10000

#define ag_window_depth							2500

#define ag_story_walkway_width					10000

//
// structures
//

typedef struct		{
						int							p1_idx,p2_idx;
						bool						on;
					} ag_room_flat_type;

typedef struct		{
						ag_room_flat_type			lft,rgt,top,bot;
					} ag_room_flat_side_type;

typedef struct		{
						int							connect_type,
													other_room_idx,wid,
													stair_dir;
						bool						on;
						d3pnt						min,max;
					} ag_room_connect_box_type;

typedef struct		{
						int							nvertex,mesh_idx;
						bool						second_story,require_top_floor;
						d3pnt						min,max;
						d3pnt						vertexes[8];
						d3vct						size;
						ag_room_flat_side_type		flat;
						ag_room_connect_box_type	connect_box;
					} ag_room_type;

typedef struct		{
						int							seed,nroom,
													current_door_idx;
						ag_room_type				*rooms;
					} ag_state_type;

