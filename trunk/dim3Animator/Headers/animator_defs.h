/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Main Defines for Animator

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
// tool bars
//

#define tool_button_size				32
#define tool_count						20


#define tool_file_names					{ \
											"Tool Textured", \
											"Tool Mesh", \
											"Tool Bones", \
											"Tool Hit Boxes", \
											"Tool Edit Mesh", \
											"Tool Edit Polygons", \
											"Tool Edit Vertexes", \
											"Tool Boxes", \
											"Tool Normals", \
											"Tool Show Bone Names", \
											"Tool Select Vertex With Bone", \
											"Tool Rotate Mode", \
											"Tool Move Mode", \
											"", \
											"Tool Play Stop", \
											"Tool Play", \
											"Tool Play Blend", \
											"Tool Play Slow", \
											"Tool Play Previous", \
											"Tool Play Next" \
										}

#define tool_separators					{"00001001001100010010"}

#define tool_tip_names					{ \
											"Show Textured Model", \
											"Show Model Mesh", \
											"Show Model Bones", \
											"Show Model Hit Boxes", \
											"Select Whole Mesh", \
											"Select Polygons", \
											"Select Vertexes", \
											"Show View Box", \
											"Show Normals", \
											"Show Bone Names", \
											"Select Attached Vertexes When Selecting Bones", \
											"Bone Rotate Mode", \
											"Bone Move Mode", \
											"", \
											"Stop Playing Animation", \
											"Play Animation", \
											"Play Multiple Blended Animations", \
											"Play Animation Slowly", \
											"Previous Pose in Animation", \
											"Next Pose in Animation" \
										}

//
// Menus
//

#define app_menu_apple					128
#define app_menu_file					129
#define app_menu_edit					130
#define app_menu_model					131
#define app_menu_view					132
#define app_menu_mesh					133
#define app_menu_vertex					134
#define app_menu_bone					135
#define app_menu_pose					136
#define app_menu_animate				137

#define app_menu_item_About						5000

#define app_menu_item_FileSave					5100
#define app_menu_item_FilePreference			5101
#define app_menu_item_FileQuit					5102

#define app_menu_item_EditUndo					5200
#define app_menu_item_EditSelectMore			5201

#define app_menu_item_PrepareModel				5300
#define app_menu_item_CalcBoxes					5301
#define app_menu_item_CalcNormals				5302
#define app_menu_item_ScaleAll					5303
#define app_menu_item_FlipXAll					5304
#define app_menu_item_FlipYAll					5305
#define app_menu_item_FlipZAll					5306
#define app_menu_item_SwapXZAll					5307
#define app_menu_item_SwapYZAll					5308
#define app_menu_item_CenterXZAll				5309
#define app_menu_item_FloorYAll					5310
#define app_menu_item_AddHitBox					5311

#define app_menu_item_Front						5400
#define app_menu_item_Left						5401
#define app_menu_item_Right						5402
#define app_menu_item_Back						5403
#define app_menu_item_Top						5404
#define app_menu_item_Bottom					5405

#define app_menu_item_NewMesh					5500
#define app_menu_item_DuplicateMesh				5501
#define app_menu_item_ImportOBJ					5502
#define app_menu_item_ReplaceOBJ				5503
#define app_menu_item_InsertXML					5504
#define app_menu_item_Scale						5505
#define app_menu_item_FlipX						5506
#define app_menu_item_FlipY						5507
#define app_menu_item_FlipZ						5508
#define app_menu_item_FlipU						5509
#define app_menu_item_FlipV						5510
#define app_menu_item_SwapXZ					5511
#define app_menu_item_SwapYZ					5512
#define app_menu_item_CenterXZ					5513
#define app_menu_item_FloorY					5514
#define app_menu_item_MeshTessellate			5515

#define app_menu_item_VertexSelectAll			5600
#define app_menu_item_VertexSelectNotAttached	5601
#define app_menu_item_VertexInvertNormals		5602
#define app_menu_item_VertexSetNormals			5603
#define app_menu_item_VertexSetNormalsOut		5604
#define app_menu_item_VertexSetNormalsIn		5605
#define app_menu_item_VertexClearBones			5606
#define app_menu_item_VertexAutoBones			5607
#define app_menu_item_VertexHideSelected		5608
#define app_menu_item_VertexHideNonSelected		5609
#define app_menu_item_VertexShowAll				5610
#define app_menu_item_VertexDelete				5611
#define app_menu_item_VertexPolygonDelete		5612
#define app_menu_item_VertexCollapseSelected	5613
#define app_menu_item_VertexCollapseSimilar		5614
#define app_menu_item_VertexMakeQuad			5615
#define app_menu_item_VertexTessellatePoly		5616

#define app_menu_item_NewBone					5700
#define app_menu_item_SetBone					5701
#define app_menu_item_SelectVertexWithBone		5702
#define app_menu_item_GoToParentBone			5703

#define app_menu_item_NewPose					5800
#define app_menu_item_DupPose					5801
#define app_menu_item_ClearPose					5802

#define app_menu_item_NewAnimate				5900
#define app_menu_item_DupAnimate				5901
#define app_menu_item_ResetTimeAnimate			5902
#define app_menu_item_PlayAnimate				5903
#define app_menu_item_PlayBlendAnimate			5904

//
// vertex and poly masks
//

#define animator_max_vertex				10240
#define animator_max_poly				10240

#define animator_mask_flag_sel			0x1
#define animator_mask_flag_hide			0x2

//
// Item types
//

#define item_model						0
#define item_mesh						1
#define item_animate					2
#define item_pose						3
#define item_neutral_pose				4
#define item_bone						5
#define item_hit_box					6

#define item_pose_move_particle			10
#define item_pose_move_ring				11

//
// Draw Types
//

#define dt_model						0
#define dt_mesh							1
#define dt_bones						2
#define dt_model_bones					3
#define dt_mesh_bones					4
#define dt_mesh_hit_boxes				5

//
// Normal Modes
//

#define normal_mode_none				0
#define normal_mode_vertex				1
#define normal_mode_face				2

//
// Drag Modes
//

#define drag_bone_mode_rotate			0
#define drag_bone_mode_stretch			1

//
// Select Modes
//

#define select_mode_mesh				0
#define select_mode_polygon				1
#define select_mode_vertex				2

//
// Drag Handle Types
//

#define drag_handle_none				-1
#define drag_handle_x					0
#define drag_handle_y					1
#define drag_handle_z					2

//
// drawing sizes
//

#define draw_poly_select_line_size		2.0f
#define draw_vertex_handle_size			8.0f

#define draw_model_normal_size			3.0f
#define draw_model_normal_len			50.0f

//
// play types
//

#define play_mode_stop					0
#define play_mode_normal				1
#define play_mode_blend					2
#define play_mode_slow					3
#define play_mode_prev					4
#define play_mode_next					5

//
// setups
//

typedef struct		{
						d3col						mesh_line,mesh_sel,
													background;
					} animator_setup_col_type;

typedef struct		{
						bool						show_tangent_binormal;
						animator_setup_col_type		col;
					} animator_setup_type;

//
// animator state
//

typedef struct		{
						int							tick,animate_idx,pose_move_idx;
					} play_blend_type;
					
typedef struct		{
						int							cur_item,cur_mesh_idx,cur_bone_idx,
													cur_pose_idx,cur_animate_idx,cur_hit_box_idx,
													cur_animate_pose_move_idx,cur_pose_bone_move_idx,
													cur_animate_pose_move_particle_idx,
													cur_animate_pose_move_ring_idx,
													texture_edit_idx,drag_bone_mode,select_mode,
													play_mode,magnify_z;
						bool						model_open,texture,mesh,bone,hit_box,
													normal,view_box,bone_names,sel_vertex_with_bone,
													drag_sel_on,in_preference,show_mesh[max_model_mesh];
						char						model_file_name[256];
						d3pnt						shift;
						d3ang						ang;
						d3rect						drag_sel_box;
						play_blend_type				blend[max_model_blend_animation];
					} animator_state_type;


