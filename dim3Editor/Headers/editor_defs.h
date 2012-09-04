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
// app modes
//

#define app_mode_project						0
#define app_mode_map							1
#define app_mode_model							2

//
// view selection
//

#define view_selection_size						10

//
// tools
//

#define tool_max_tools							64
#define tool_button_size						32

typedef struct		{
						char						file_name[64];
						bool						separator;
						char						tool_tip[64];
						bitmap_type					bitmap;
					} tool_palette_tool_type;

typedef struct		{
						bool						disabled,selected;
					} tool_palette_state_type;

typedef struct		{
						int							count,push_idx;
						tool_palette_tool_type		tools[tool_max_tools];
						tool_palette_state_type		state[tool_max_tools];
					} tool_palette_type;

//
// map tool palette
//

#define map_tool_count					38

#define map_tool_palette_def			{ \
											{"Map/Tool Move Points",FALSE,"Move Vertexes and Meshes Freely"}, \
											{"Map/Tool Move Points Together",FALSE,"Move Equal Vertexes Together"}, \
											{"Map/Tool Snap Points",FALSE,"Snap Vertexes and Meshes"}, \
											{"Map/Tool Free Look",TRUE,"Free Look Mode"}, \
											{"Map/Tool Toggle Mode",FALSE,"Multi-Select Mode"}, \
											{"Map/Tool Edit Mesh",TRUE,"Select Meshes"}, \
											{"Map/Tool Edit Polygons",FALSE,"Select Polygons"}, \
											{"Map/Tool Edit Vertexes",FALSE,"Select Vertexes"}, \
											{"Map/Tool Combine Meshes",TRUE,"Combine Meshes"}, \
											{"Map/Tool Split Mesh",FALSE,"Split Mesh"}, \
											{"Map/Tool Tesselate Mesh",FALSE,"Tesselate Mesh"}, \
											{"Map/Tool No Grid",TRUE,"No Grid"}, \
											{"Map/Tool Small Grid",FALSE,"Small Grid"}, \
											{"Map/Tool Large Grid",FALSE,"Large Grid"}, \
											{"Map/Tool Auto-Texture Mesh",TRUE,"Auto-Texture Mesh"}, \
											{"Map/Tool Rotate Mode",TRUE,"Rotate Mode"}, \
											{"Map/Tool Move Mode",FALSE,"Move Mode"}, \
											{"Map/Tool Node Select",TRUE,"Click Node To Select"}, \
											{"Map/Tool Node Duplicate",FALSE,"Click Node To Duplicate, Drag and Auto-Link"}, \
											{"Map/Tool Node Link",FALSE,"Click Node To Add Link From Selected Node"}, \
											{"Map/Tool Node Remove Link",FALSE,"Click Node To Remove Link From Selected Node"}, \
											{"Map/Tool Normals",TRUE,"Show Normals"}, \
											{"Map/Tool Cull",FALSE,"Show Obscured (Green) and Culled (Gray) Polygons"}, \
											{"Map/Tool Edit Map Script",TRUE,"Edit Map Script"}, \
											{"Map/Tool Run Map",FALSE,"Run Map In Engine"}, \
											{"",FALSE,""}, \
											{"Map/Tool Spot",FALSE,"Add Spot"}, \
											{"Map/Tool Light",FALSE,"Add Light"}, \
											{"Map/Tool Sound",FALSE,"Add Sound"}, \
											{"Map/Tool Particle",FALSE,"Add Particle"}, \
											{"Map/Tool Scenery",FALSE,"Add Scenery"}, \
											{"Map/Tool Node",FALSE,"Add Node"}, \
											{"Map/Tool Mesh",FALSE,"Add Imported Mesh"}, \
											{"Map/Tool Mesh UV",FALSE,"Replace UVs on an Existing Mesh"}, \
											{"Map/Tool Height Map",FALSE,"Add Imported Height Map"}, \
											{"Map/Tool Grid",FALSE,"Add Grid"}, \
											{"Map/Tool Polygon",FALSE,"Add Polygon"}, \
											{"Map/Tool Liquid",FALSE,"Add Liquid"}, \
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
// project menus
//

#define app_menu_apple							128
#define app_menu_file							129

#define app_menu_item_About						5000

#define app_menu_item_Save						5100
#define app_menu_item_Quit						5101

//
// map menus
//

#define app_menu_apple							128
#define app_menu_file							129
#define app_menu_edit							130
#define app_menu_view							131
#define app_menu_map							132
#define app_menu_mesh							133
#define app_menu_polygon						134

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
#define app_menu_item_MeshForceGrid				5506
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
#define app_menu_item_MeshResetUV				5520
#define app_menu_item_MeshWholeUV				5521
#define app_menu_item_MeshSingleUV				5522
#define app_menu_item_MeshCreateNormals			5523
#define app_menu_item_MeshInvertNormals			5524
#define app_menu_item_MeshSetNormalsOut			5525
#define app_menu_item_MeshSetNormalsIn			5526

#define app_menu_item_PolygonHole				5600
#define app_menu_item_PolyTesselate				5601
#define app_menu_item_PolygonRotateUV			5603
#define app_menu_item_PolygonFlipU				5604
#define app_menu_item_PolygonFlipV				5605
#define app_menu_item_PolygonRecalcNormal		5606
#define app_menu_item_PolygonInvertNormal		5607
#define app_menu_item_PolygonResetUV			5608
#define app_menu_item_PolygonWholeUV			5609
#define app_menu_item_PolygonSingleUV			5610

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
// Item types
//

#define item_interface							0
#define item_interface_project					1
#define item_interface_color					2
#define item_interface_device					3
#define item_interface_logo						4
#define item_interface_title_page				5
#define item_interface_singleplayer				6
#define item_interface_multiplayer				7
#define item_interface_setup					8
#define item_interface_progress					9
#define item_interface_hud						10
#define item_interface_virtual_control			11
#define item_interface_radar					12
#define item_interface_label					13
#define item_interface_menu						14
#define item_interface_chooser					15
#define item_interface_sound					16
#define item_interface_particle					17
#define item_interface_ring						18
#define item_interface_halo						19
#define item_interface_mark						20
#define item_interface_crosshair				21
#define item_interface_action					22
#define item_interface_shader					23
#define item_interface_preload_models			24

//
// intro button types
//

#define item_intro_button_game_new				0
#define item_intro_button_game_load				1
#define item_intro_button_game_setup			2
#define item_intro_button_multiplayer_host		3
#define item_intro_button_multiplayer_join		4
#define item_intro_button_credit				5
#define item_intro_button_quit					6

#define item_intro_button_simple_save_start		10
#define item_intro_button_simple_save_erase		20

//
// preference structures
//

typedef struct		{
						d3col					mesh_line,mesh_sel,poly_sel,poly_cull,
												background;
					} map_pref_col_type;

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
						map_pref_col_type		col;
					} map_pref_type;

typedef struct		{
						d3col					mesh_line,mesh_sel,
												background;
					} model_pref_col_type;

typedef struct		{
						bool					show_tangent_binormal;
						model_pref_col_type		col;
					} model_pref_type;

typedef struct		{
						map_pref_type			map;
						model_pref_type			model;
					} app_pref_type;

//
// state structures
//

typedef struct		{
						int						cur_item,cur_device,
												cur_intro_button_idx,cur_intro_model_idx,
												cur_hud_bitmap_idx,cur_hud_text_idx,cur_hud_bar_idx,
												cur_virtual_control_stick_idx,cur_virtual_control_button_idx,
												cur_radar_icon_idx,cur_singleplayer_option_idx,cur_multiplayer_character_idx,
												cur_menu_idx,cur_menu_item_idx,cur_chooser_idx,cur_chooser_piece_idx,
												cur_multiplayer_game_idx,cur_multiplayer_option_idx,
												cur_sound_idx,cur_particle_idx,cur_group_particle_idx,cur_ring_idx,
												cur_halo_idx,cur_mark_idx,cur_crosshair_idx,
												cur_action_idx,cur_shader_idx;
					} project_state_type;

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
					} map_state_type;

typedef struct		{
						int						tick,animate_idx,pose_move_idx;
					} model_state_blend_type;
					
typedef struct		{
						int						cur_item,cur_mesh_idx,cur_bone_idx,
												cur_pose_idx,cur_animate_idx,cur_hit_box_idx,
												cur_animate_pose_move_idx,cur_pose_bone_move_idx,
												cur_animate_pose_move_particle_idx,
												cur_animate_pose_move_ring_idx,
												texture_edit_idx,drag_bone_mode,select_mode,
												play_mode,magnify_z;
						bool					model_open,texture,mesh,bone,hit_box,
												normal,view_box,bone_names,sel_vertex_with_bone,
												drag_sel_on,in_preference,show_mesh[max_model_mesh];
						char					model_file_name[256];
						d3pnt					shift;
						d3ang					ang;
						d3rect					drag_sel_box;
						model_state_blend_type	blend[max_model_blend_animation];
					} model_state_type;

typedef struct		{
						int						mode;
						project_state_type		proj;
						map_state_type			map;
						model_state_type		model;
					} app_state_type;

