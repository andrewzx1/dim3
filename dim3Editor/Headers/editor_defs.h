/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Defines for Editor

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
// UI sizes
//

#define tool_count								19
#define piece_count								12

#define tool_button_size						32

#define tool_spot_menu_id						160
#define tool_light_menu_id						161
#define tool_sound_menu_id						162
#define tool_particle_menu_id					163
#define tool_scenery_menu_id					164
#define tool_node_menu_id						165
#define tool_area_menu_id						166
#define tool_grid_menu_id						167

#define toolbar_high							32

#define view_selection_size						10

//
// vertex modes
//

#define vertex_mode_none						0
#define vertex_mode_lock						1
#define vertex_mode_snap						2

//
// drag modes
//

#define drag_mode_mesh							0
#define drag_mode_polygon						1
#define drag_mode_vertex						2

//
// grid modes
//

#define grid_mode_none							0
#define grid_mode_small							1
#define grid_mode_medium						2
#define grid_mode_large							3

//
// node modes
//

#define node_mode_select						0
#define node_mode_link							1
#define node_mode_remove_link					2

//
// selection types
//

#define mesh_piece								100
#define liquid_piece							101
#define node_piece								102
#define spot_piece								103
#define scenery_piece							104
#define light_piece								105
#define sound_piece								106
#define particle_piece							107

//
// movement scales
//

#define move_mouse_scale						30
#define move_mouse_turn_reduce_scale			5.0f

#define move_scroll_wheel_scale					300

#define move_key_scale							30
#define move_key_shift_reduce_scale				10

//
// clicking
//

#define walk_view_max_z_click					1000000

//
// walk view opengl settings
//

#define walk_view_near_offset					(3*map_enlarge)

#define walk_view_handle_size					12.0f
#define walk_view_handle_length					40.0f
#define walk_view_handle_line_width				2.0f

//
// light maps
//

#define light_map_texture_block_size			8
#define light_map_quality_to_pixel_factor		10000.0f

#define uv_layer_normal							0
#define uv_layer_light_map						1

//
// normals
//

#define normal_vector_scale						300.0f

//
// setups
//

typedef struct		{
						d3col					mesh_line,mesh_sel,poly_sel,poly_cull,
												background;
					} setup_col_type;

typedef struct		{
						int						anisotropic_mode,mipmap_mode,
												duplicate_offset,snap_size,
												clip_distance;
						bool					auto_texture,flip_movement;
						char					engine_name[256];
						setup_col_type			col;
					} setup_type;

//
// selections
//

#define select_max_item							1024

typedef struct		{
                        int						type,main_idx,sub_idx;
					} select_item_type;

//
// picker struct
//

typedef struct		{
						short					type,main_idx,sub_idx;
						unsigned char			col[3];
					} view_picker_type;

//
// mode struct
//

typedef struct		{
						int						vertex_mode,drag_mode,grid_mode,node_mode,
												drag_handle_idx;
						bool					map_opened,select_toggle_mode,auto_texture,
												show_liquid,show_normals,show_node,show_object,
												show_lightsoundparticle;
					} editor_state_type;
					
