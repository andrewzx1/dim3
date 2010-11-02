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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// view selection
//

#define view_selection_size						10

//
// tools
//

#define tool_count								37
#define tool_button_size						32

#define tool_file_names							{ \
													"Tool Move Points", \
													"Tool Move Points Together", \
													"Tool Snap Points", \
													"Tool Free Look", \
													"Tool Toggle Mode", \
													"Tool Edit Mesh", \
													"Tool Edit Polygons", \
													"Tool Edit Vertexes", \
													"Tool Combine Meshes", \
													"Tool Split Mesh", \
													"Tool Tesselate Mesh", \
													"Tool No Grid", \
													"Tool Small Grid", \
													"Tool Large Grid", \
													"Tool Auto-Texture Mesh", \
													"Tool Rotate Mode", \
													"Tool Move Mode", \
													"Tool Node Select", \
													"Tool Node Link", \
													"Tool Node Remove Link", \
													"Tool Normals", \
													"Tool Cull", \
													"Tool Edit Map Script", \
													"Tool Run Map", \
													"", \
													"Tool Spot", \
													"Tool Light", \
													"Tool Sound", \
													"Tool Particle", \
													"Tool Scenery", \
													"Tool Node", \
													"Tool Mesh", \
													"Tool Mesh UV", \
													"Tool Height Map", \
													"Tool Grid", \
													"Tool Polygon", \
													"Tool Liquid", \
											}
											
#define tool_separators						{"0001010010010011010010100000000000000"}

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
#define grid_mode_large							2

//
// node modes
//

#define node_mode_select						0
#define node_mode_link							1
#define node_mode_remove_link					2

//
// handle modes
//

#define handle_mode_rotate						0
#define handle_mode_move						1

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

#define view_handle_move_scale					3

//
// clicking
//

#define view_max_box_select_item				256

//
// opengl settings
//

#define view_near_offset						(3*map_enlarge)
#define view_look_snap							10.0f

//
// display sizes
//

#define view_handle_size						12.0f

#define view_handle_length_factor				0.05f
#define view_handle_line_width					2.0f

#define view_sprite_size						(map_enlarge*3)

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
// menus
//

#define app_menu_apple							128
#define app_menu_file							129
#define app_menu_edit							130
#define app_menu_view							131
#define app_menu_map							132
#define app_menu_mesh							133
#define app_menu_polygon						134
#define app_menu_vertex							135
#define app_menu_groups							136

#define kCommandAbout							FOUR_CHAR_CODE('abot')
#define kCommandPreference						FOUR_CHAR_CODE('pref')

#define kCommandFileNew							FOUR_CHAR_CODE('new ')
#define kCommandFileOpen						FOUR_CHAR_CODE('open')
#define kCommandFileClose						FOUR_CHAR_CODE('clse')
#define kCommandFileSave						FOUR_CHAR_CODE('save')
#define kCommandFileAutoGenerateMap				FOUR_CHAR_CODE('auto')
#define kCommandFileQuit						FOUR_CHAR_CODE('quit')

#define kCommandEditUndo						FOUR_CHAR_CODE('undo')
#define kCommandEditDelete						FOUR_CHAR_CODE('pdel')
#define kCommandEditDuplicate					FOUR_CHAR_CODE('pdup')
#define kCommandEditSelectMore					FOUR_CHAR_CODE('selm')

#define kCommandViewFront						FOUR_CHAR_CODE('vw01')
#define kCommandViewLeft						FOUR_CHAR_CODE('vw02')
#define kCommandViewRight						FOUR_CHAR_CODE('vw03')
#define kCommandViewBack						FOUR_CHAR_CODE('vw04')
#define kCommandViewTop							FOUR_CHAR_CODE('vw05')
#define kCommandViewBottom						FOUR_CHAR_CODE('vw06')
#define kCommandViewPerspective					FOUR_CHAR_CODE('vwps')
#define kCommandViewOrtho						FOUR_CHAR_CODE('vwot')
#define kCommandViewUVLayer1					FOUR_CHAR_CODE('uvl1')
#define kCommandViewUVLayer2					FOUR_CHAR_CODE('uvl2')
#define kCommandViewGotoSelect					FOUR_CHAR_CODE('goto')
#define kCommandViewGotoMapCenter				FOUR_CHAR_CODE('gotc')
#define kCommandViewClip						FOUR_CHAR_CODE('vwcp')
#define kCommandViewShowHideLiquids				FOUR_CHAR_CODE('shlq')
#define kCommandViewShowHideSpots				FOUR_CHAR_CODE('shss')
#define kCommandViewShowHideLights				FOUR_CHAR_CODE('shls')
#define kCommandViewShowHideNodes				FOUR_CHAR_CODE('shnd')
#define kCommandViewSplitHorizontal				FOUR_CHAR_CODE('sphz')
#define kCommandViewSplitVertical				FOUR_CHAR_CODE('spvt')
#define kCommandViewRemoveSplit					FOUR_CHAR_CODE('sprm')

#define kCommandMapSettings						FOUR_CHAR_CODE('mset')
#define kCommandMapCinemas						FOUR_CHAR_CODE('cinm')
#define kCommandMapRaiseY						FOUR_CHAR_CODE('mrsy')
#define kCommandMapLowerY						FOUR_CHAR_CODE('mlwy')
#define kCommandMapCenter						FOUR_CHAR_CODE('mctr')
#define kCommandMapResetUV						FOUR_CHAR_CODE('mpuv')
#define kCommandMapOptimize						FOUR_CHAR_CODE('mopt')
#define kCommandMapCreateNormals				FOUR_CHAR_CODE('rmnm')
#define kCommandClearLightMaps					FOUR_CHAR_CODE('clmp')
#define kCommandBuildLightMaps					FOUR_CHAR_CODE('lmap')
#define kCommandRun								FOUR_CHAR_CODE('mrun')

#define kCommandMeshCombine						FOUR_CHAR_CODE('pcmb')
#define kCommandMeshSplit						FOUR_CHAR_CODE('pspt')
#define kCommandMeshTesselate					FOUR_CHAR_CODE('ptsl')
#define kCommandMeshResize						FOUR_CHAR_CODE('mrsz')
#define kCommandMeshReposition					FOUR_CHAR_CODE('mrps')
#define kCommandMeshSkew						FOUR_CHAR_CODE('mskw')
#define kCommandMeshFlipX						FOUR_CHAR_CODE('pfpx')
#define kCommandMeshFlipY						FOUR_CHAR_CODE('pfpy')
#define kCommandMeshFlipZ						FOUR_CHAR_CODE('pfpz')
#define kCommandMeshRotateX						FOUR_CHAR_CODE('prtx')
#define kCommandMeshRotateY						FOUR_CHAR_CODE('prty')
#define kCommandMeshRotateZ						FOUR_CHAR_CODE('prtz')
#define kCommandMeshFreeRotate					FOUR_CHAR_CODE('prtf')
#define kCommandMeshRaiseY						FOUR_CHAR_CODE('prsy')
#define kCommandMeshLowerY						FOUR_CHAR_CODE('plry')
#define kCommandMeshSelectAllPoly				FOUR_CHAR_CODE('msll')
#define kCommandMeshSnapToGrid					FOUR_CHAR_CODE('msnp')
#define kCommandMeshSnapClosestVertex			FOUR_CHAR_CODE('mscv')
#define kCommandMeshResetUV						FOUR_CHAR_CODE('mruv')
#define kCommandMeshWholeUV						FOUR_CHAR_CODE('mrwv')
#define kCommandMeshSingleUV					FOUR_CHAR_CODE('mrss')
#define kCommandMeshCreateNormals				FOUR_CHAR_CODE('rsnm')
#define kCommandMeshInvertNormals				FOUR_CHAR_CODE('isnm')

#define kCommandPolygonHole						FOUR_CHAR_CODE('fhl1')
#define kCommandPolygonSnapToGrid				FOUR_CHAR_CODE('psnp')
#define kCommandPolygonRotateUV					FOUR_CHAR_CODE('pyru')
#define kCommandPolygonFlipU					FOUR_CHAR_CODE('pyfu')
#define kCommandPolygonFlipV					FOUR_CHAR_CODE('pyfv')
#define kCommandPolygonInvertNormal				FOUR_CHAR_CODE('inrm')
#define kCommandPolygonResetUV					FOUR_CHAR_CODE('pyuv')
#define kCommandPolygonWholeUV					FOUR_CHAR_CODE('pywv')
#define kCommandPolygonSingleUV					FOUR_CHAR_CODE('pyss')

#define kCommandVertexSnapToGrid				FOUR_CHAR_CODE('vsnp')

#define kCommandGroups							FOUR_CHAR_CODE('grrp')
#define kCommandGroupMovements					FOUR_CHAR_CODE('mmov')

//
// undos
//

#define max_undo_level							8

typedef struct		{
						int						count;
						unsigned char			*data;
					} undo_chunk_type;
					
typedef struct		{
						undo_chunk_type			mesh,liquid,
												spot,scenery,node,
												light,sound,particle;
					} undo_type;

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
						bool					free_look,auto_texture,big_texture,
												flip_horz_movement,flip_vert_movement,
												flip_horz_turn,flip_vert_turn,
												flip_forward_movement;
						char					engine_name[256];
						setup_col_type			col;
					} editor_setup_type;

//
// selections
//

#define select_max_item							1024

typedef struct		{
                        int						type,main_idx,sub_idx;
					} select_item_type;

//
// view picker
//

typedef struct		{
						int						col;
						short					type,main_idx,sub_idx;
						bool					hit;
					} view_picker_type;
							
//
// auto generator
//

#define ag_max_style							32

#define ag_max_shape							128
#define ag_max_shape_point						64
#define ag_max_shape_connector					32
#define ag_max_shape_poly						64

#define ag_max_room								256

#define ag_connector_type_min_x					0
#define ag_connector_type_max_x					1
#define ag_connector_type_min_z					2
#define ag_connector_type_max_z					3

typedef struct		{
						int						nshape,
												shape_list[ag_max_shape];
						char					name[name_str_len];
					} ag_style_type;

typedef struct		{
						int						npt,v[4];
					} ag_shape_poly_type;

typedef struct		{
						int						type,dist,v[2];
						d3pnt					top_left;
					} ag_shape_connector_type;

typedef struct		{
						int						nvertex,npoly,nconnector;
						char					name[name_str_len];
						bool					corridor;
						d3pnt					vertexes[ag_max_shape_point];
						ag_shape_poly_type		polys[ag_max_shape_poly];
						ag_shape_connector_type	connectors[ag_max_shape_connector];
					} ag_shape_type;

typedef struct		{
						int						shape_idx,mesh_idx;
						bool					corridor,clip,multi_story,
												connectors_used[ag_max_shape_connector];
						d3pnt					min,max;
						d3vct					size;
					} ag_room_type;

typedef struct		{
						int						nstyle,nshape,nroom;
						ag_style_type			*styles;
						ag_shape_type			*shapes;
						ag_room_type			*rooms;
					} ag_state_type;

typedef struct		{
						int						style_idx,seed,
												room_count,room_sz,room_high,
												story_count,extra_connect_count;
						bool					mirror;
					} ag_build_setup_type;

//
// editor state struct
//

typedef struct		{
						int						vertex_mode,drag_mode,grid_mode,node_mode,
												handle_mode,view_select_idx,drag_handle_idx;
						bool					map_opened,free_look,select_add,auto_texture,
												select_box_on,
												show_liquid,show_normals,show_node,show_object,
												show_lightsoundparticle;
						d3pnt					select_box_start_pnt,select_box_end_pnt;
					} editor_state_type;
					
