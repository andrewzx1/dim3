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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// view selection
//

#define view_selection_size						10

//
// tools
//

#define tool_count								38
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
													"Tool Node Duplicate", \
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
											
#define tool_separators						{"00010100100100110100010100000000000000"}

#define tool_tip_names							{ \
													"Move Vertexes and Meshes Freely", \
													"Move Equal Vertexes Together", \
													"Snap Vertexes and Meshes", \
													"Free Look Mode", \
													"Multi-Select Mode", \
													"Select Meshes", \
													"Select Polygons", \
													"Select Vertexes", \
													"Combine Meshes", \
													"Split Mesh", \
													"Tesselate Mesh", \
													"No Grid", \
													"Small Grid", \
													"Large Grid", \
													"Auto-Texture Mesh", \
													"Rotate Mode", \
													"Move Mode", \
													"Click Node To Select", \
													"Click Node To Duplicate, Drag and Auto-Link", \
													"Click Node To Add Link From Selected Node", \
													"Click Node To Remove Link From Selected Node", \
													"Show Normals", \
													"Show Obscured (Green) and Culled (Gray) Polygons", \
													"Edit Map Script", \
													"Run Map In Engine", \
													"", \
													"Add Spot", \
													"Add Light", \
													"Add Sound", \
													"Add Particle", \
													"Add Scenery", \
													"Add Node", \
													"Add Imported Mesh", \
													"Replace UVs on an Existing Mesh", \
													"Add Imported Height Map", \
													"Add Grid", \
													"Add Polygon", \
													"Add Liquid", \
											}

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
#define node_mode_duplicate						1
#define node_mode_link							2
#define node_mode_remove_link					3

//
// handle modes
//

#define handle_mode_rotate						0
#define handle_mode_move						1

//
// selection types
//

#define map_setting_piece						0
#define map_camera_piece						1
#define map_light_media_piece					2
#define map_sky_weather_piece					3

#define mesh_piece								10
#define liquid_piece							11
#define node_piece								12
#define spot_piece								13
#define scenery_piece							14
#define light_piece								15
#define sound_piece								16
#define particle_piece							17

#define group_piece								20
#define movement_piece							21
#define cinema_piece							22

//
// movement scales
//

#define move_mouse_min_scale					40
#define move_mouse_distance_ratio				1000

#define move_mouse_turn_reduce_scale			5.0f

#define move_mouse_drag_distance_ratio			1000

#define move_scroll_wheel_scale					300

#define move_key_scale							30
#define move_key_shift_reduce_scale				10

#define view_texture_shift_factor				100.0f

//
// opengl settings
//

#define view_near_offset						430
#define view_look_snap							10.0f

//
// display sizes
//

#define view_handle_size						12.0f
#define view_skew_handle_size					8.0f

#define view_handle_length_min					150.0f
#define view_handle_length_factor				0.05f
#define view_handle_line_width					2.0f

#define view_sprite_size						400

#define view_snap_clip_size_factor				150

//
// light maps
//

#define light_map_texture_block_size						8

#define light_map_max_textures								64
#define light_map_max_solid_color							64
#define light_map_render_margin								5

#define light_map_texture_min_rect_size						16

#define light_map_blur_count								3

#define light_map_quality_very_low							0
#define light_map_quality_low								1
#define light_map_quality_medium							2
#define light_map_quality_high								3
#define light_map_quality_very_high							4

#define light_map_quality_values							{(10.0f/10000.0f),(30.0f/10000.0f),(50.0f/10000.0f),(75.0f/10000.0f),(100.0f/10000.0f)}

typedef struct		{
						int									x_shift,y_shift;
						unsigned char						col[3];
					} light_map_solid_color_cache_type;

typedef struct		{
						int									solid_color_cache_count;
						unsigned char						*block,*pixel_data,
															*pixel_touch;
						light_map_solid_color_cache_type	*solid_color_cache;
					} light_map_texture_type;
					
typedef struct		{
						int									ptsz,txt_idx,
															mesh_idx,poly_idx,liquid_idx,
															x[8],y[8],
															x_shift,y_shift,
															x_sz,y_sz;
						bool								solid_color;
						d3pnt								pt[8];
						d3rect								box;
					} light_map_poly_type;
					
typedef struct		{
						unsigned char						*data;
					} light_map_map_texture_alpha_type;

//
// uv layers
//

#define uv_layer_normal							0
#define uv_layer_light_map						1

//
// normals
//

#define normal_vector_scale						300.0f

//
// import modes
//

#define import_mode_new							0
#define import_mode_replace						1
#define import_mode_replace_all					2

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

#define app_menu_item_About						5000

#define app_menu_item_FileSave					5100
#define app_menu_item_FilePreference			5101
#define app_menu_item_FileQuit					5102

#define app_menu_item_EditUndo					5200
#define app_menu_item_EditDelete				5201
#define app_menu_item_EditDuplicate				5202
#define app_menu_item_EditSelectMore			5203

#define app_menu_item_ViewFront					5300
#define app_menu_item_ViewLeft					5301
#define app_menu_item_ViewRight					5302
#define app_menu_item_ViewBack					5303
#define app_menu_item_ViewTop					5304
#define app_menu_item_ViewBottom				5305
#define app_menu_item_ViewPerspective			5306
#define app_menu_item_ViewOrtho					5307
#define app_menu_item_ViewUVLayer1				5308
#define app_menu_item_ViewUVLayer2				5309
#define app_menu_item_ViewGotoSelect			5310
#define app_menu_item_ViewGotoMapCenter			5311
#define app_menu_item_ViewClip					5312
#define app_menu_item_ViewShowHideLiquids		5313
#define app_menu_item_ViewShowHideSpots			5314
#define app_menu_item_ViewShowHideLights		5315
#define app_menu_item_ViewShowHideNodes			5316
#define app_menu_item_ViewShowHideMovements		5317
#define app_menu_item_ViewSplitHorizontal		5318
#define app_menu_item_ViewSplitVertical			5319
#define app_menu_item_ViewRemoveSplit			5320

#define app_menu_item_MapRaiseY					5400
#define app_menu_item_MapLowerY					5401
#define app_menu_item_MapCenter					5402
#define app_menu_item_MapResetUV				5403
#define app_menu_item_MapResetNormals			5404
#define app_menu_item_MapSortMeshPolys			5405
#define app_menu_item_ClearLightMaps			5406
#define app_menu_item_BuildLightMaps			5407
#define app_menu_item_BuildLiquidReflectionMaps	5408
#define app_menu_item_AutoGenerate				5409
#define app_menu_item_Run						5410

#define app_menu_item_MeshCombine				5500
#define app_menu_item_MeshSplit					5501
#define app_menu_item_MeshTesselate				5502
#define app_menu_item_MeshResize				5503
#define app_menu_item_MeshReposition			5504
#define app_menu_item_MeshForceGrid				5506
#define app_menu_item_MeshResizeTexture			5507
#define app_menu_item_MeshFlipX					5508
#define app_menu_item_MeshFlipY					5509
#define app_menu_item_MeshFlipZ					5510
#define app_menu_item_MeshRotateX				5511
#define app_menu_item_MeshRotateY				5512
#define app_menu_item_MeshRotateZ				5513
#define app_menu_item_MeshFreeRotate			5514
#define app_menu_item_MeshRaiseY				5515
#define app_menu_item_MeshLowerY				5516
#define app_menu_item_MeshSelectAllPoly			5517
#define app_menu_item_MeshSnapToGrid			5518
#define app_menu_item_MeshSnapClosestVertex		5519
#define app_menu_item_MeshResetUV				5520
#define app_menu_item_MeshWholeUV				5521
#define app_menu_item_MeshSingleUV				5522
#define app_menu_item_MeshCreateNormals			5523
#define app_menu_item_MeshInvertNormals			5524
#define app_menu_item_MeshSetNormalsOut			5525
#define app_menu_item_MeshSetNormalsIn			5526

#define app_menu_item_PolygonHole				5600
#define app_menu_item_PolyTesselate				5601
#define app_menu_item_PolygonSnapToGrid			5602
#define app_menu_item_PolygonRotateUV			5603
#define app_menu_item_PolygonFlipU				5604
#define app_menu_item_PolygonFlipV				5605
#define app_menu_item_PolygonRecalcNormal		5606
#define app_menu_item_PolygonInvertNormal		5607
#define app_menu_item_PolygonResetUV			5608
#define app_menu_item_PolygonWholeUV			5609
#define app_menu_item_PolygonSingleUV			5610

#define app_menu_item_VertexSnapToGrid			5700

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
												light,sound,particle,
												selection;
					} undo_type;

//
// setups
//

typedef struct		{
						d3col					mesh_line,mesh_sel,poly_sel,poly_cull,
												background;
					} editor_setup_col_type;

typedef struct		{
						int						duplicate_offset,snap_size,
												clip_distance;
						bool					free_look,auto_texture,big_texture,
												show_tangent_binormal,
												flip_horz_movement,flip_vert_movement,
												flip_horz_turn,flip_vert_turn,
												flip_forward_movement;
						char					engine_name[256];
						d3vct					import_normal_factor;
						editor_setup_col_type	col;
					} editor_setup_type;

//
// selections
//

#define select_max_item							5120

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
// editor draw sorting
//

#define view_mesh_sort_max_mesh					5000

typedef struct		{
						int						idx,dist;
					} view_mesh_sort_list_type;

//
// editor state struct
//

typedef struct		{
						int						vertex_mode,drag_mode,grid_mode,node_mode,
												handle_mode,view_select_idx,drag_handle_idx,
												texture_edit_idx,
												cur_no_sel_piece_type,cur_group_idx,
												cur_cinema_idx,cur_cinema_action_idx,
												cur_movement_idx,cur_movement_move_idx;
						bool					map_open,
												free_look,select_add,auto_texture,select_box_on,
												show_liquid,show_normals,show_movements,
												show_node,show_object,show_lightsoundparticle,
												in_preference;
						char					map_file_name[file_str_len];
						d3pnt					select_box_start_pnt,select_box_end_pnt;
					} editor_state_type;
					
