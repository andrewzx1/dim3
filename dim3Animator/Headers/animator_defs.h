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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
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

#define kCommandAbout					FOUR_CHAR_CODE('abot')

#define kCommandFileNew					FOUR_CHAR_CODE('newB')
#define kCommandFileOpen				FOUR_CHAR_CODE('opnB')
#define kCommandFileClose				FOUR_CHAR_CODE('clse')
#define kCommandFileSave				FOUR_CHAR_CODE('savB')
#define kCommandFilePreference			FOUR_CHAR_CODE('pref')
#define kCommandFileQuit				FOUR_CHAR_CODE('quit')

#define kCommandEditUndo				FOUR_CHAR_CODE('undo')
#define kCommandEditSelectMore			FOUR_CHAR_CODE('selm')

#define kCommandPrepareModel			FOUR_CHAR_CODE('pmdl')
#define kCommandCalcBoxes				FOUR_CHAR_CODE('cbox')
#define kCommandCalcNormals				FOUR_CHAR_CODE('cnrm')
#define kCommandScaleAll				FOUR_CHAR_CODE('scal')
#define kCommandFlipXAll				FOUR_CHAR_CODE('fxal')
#define kCommandFlipYAll				FOUR_CHAR_CODE('fyal')
#define kCommandFlipZAll				FOUR_CHAR_CODE('fzal')
#define kCommandSwapXZAll				FOUR_CHAR_CODE('spal')
#define kCommandSwapYZAll				FOUR_CHAR_CODE('syal')
#define kCommandCenterXZAll				FOUR_CHAR_CODE('ctal')
#define kCommandFloorYAll				FOUR_CHAR_CODE('fral')
#define kCommandAddHitBox				FOUR_CHAR_CODE('hbox')

#define kCommandFront					FOUR_CHAR_CODE('vfnt')
#define kCommandLeft					FOUR_CHAR_CODE('vlft')
#define kCommandRight					FOUR_CHAR_CODE('vrgt')
#define kCommandBack					FOUR_CHAR_CODE('vbck')
#define kCommandTop						FOUR_CHAR_CODE('vtop')
#define kCommandBottom					FOUR_CHAR_CODE('vbtm')

#define kCommandNewMesh					FOUR_CHAR_CODE('nmsh')
#define kCommandDuplicateMesh			FOUR_CHAR_CODE('dpmh')
#define kCommandImportOBJ				FOUR_CHAR_CODE('OBJi')
#define kCommandReplaceOBJ				FOUR_CHAR_CODE('OBJr')
#define kCommandInsertXML				FOUR_CHAR_CODE('opDM')
#define kCommandScale					FOUR_CHAR_CODE('scle')
#define kCommandFlipX					FOUR_CHAR_CODE('flpx')
#define kCommandFlipY					FOUR_CHAR_CODE('flpy')
#define kCommandFlipZ					FOUR_CHAR_CODE('flpz')
#define kCommandFlipU					FOUR_CHAR_CODE('flpu')
#define kCommandFlipV					FOUR_CHAR_CODE('flpv')
#define kCommandSwapXZ					FOUR_CHAR_CODE('spxz')
#define kCommandSwapYZ					FOUR_CHAR_CODE('spyz')
#define kCommandCenterXZ				FOUR_CHAR_CODE('ctxz')
#define kCommandFloorY					FOUR_CHAR_CODE('flry')
#define kCommandMeshTessellate			FOUR_CHAR_CODE('tsms')

#define kCommandVertexSelectAll			FOUR_CHAR_CODE('vsla')
#define kCommandVertexSelectNotAttached	FOUR_CHAR_CODE('vsna')
#define kCommandVertexInvertNormals		FOUR_CHAR_CODE('vinm')
#define kCommandVertexSetNormals		FOUR_CHAR_CODE('vsnm')
#define kCommandVertexSetNormalsOut		FOUR_CHAR_CODE('vsno')
#define kCommandVertexSetNormalsIn		FOUR_CHAR_CODE('vsni')
#define kCommandVertexClearBones		FOUR_CHAR_CODE('vcba')
#define kCommandVertexAutoBones			FOUR_CHAR_CODE('vsba')
#define kCommandVertexHideSelected		FOUR_CHAR_CODE('vhsl')
#define kCommandVertexHideNonSelected	FOUR_CHAR_CODE('vhns')
#define kCommandVertexShowAll			FOUR_CHAR_CODE('vsal')
#define kCommandVertexDelete			FOUR_CHAR_CODE('vdel')
#define kCommandVertexPolygonDelete		FOUR_CHAR_CODE('pdel')
#define kCommandVertexCollapseSelected	FOUR_CHAR_CODE('csvt')
#define kCommandVertexCollapseSimilar	FOUR_CHAR_CODE('csml')
#define kCommandVertexMakeQuad			FOUR_CHAR_CODE('vmqd')
#define kCommandVertexTessellatePoly	FOUR_CHAR_CODE('vtpy')

#define kCommandNewBone					FOUR_CHAR_CODE('nbne')
#define kCommandSetBone					FOUR_CHAR_CODE('sbn2')
#define kCommandSelectVertexWithBone	FOUR_CHAR_CODE('svwb')
#define kCommandGoToParentBone			FOUR_CHAR_CODE('gpbn')

#define kCommandNewPose					FOUR_CHAR_CODE('npse')
#define kCommandDupPose					FOUR_CHAR_CODE('dpse')
#define kCommandClearPose				FOUR_CHAR_CODE('cpse')

#define kCommandNewAnimate				FOUR_CHAR_CODE('nani')
#define kCommandDupAnimate				FOUR_CHAR_CODE('dani')
#define kCommandResetTimeAnimate		FOUR_CHAR_CODE('artm')
#define kCommandPlayAnimate				FOUR_CHAR_CODE('plya')
#define kCommandPlayBlendAnimate		FOUR_CHAR_CODE('plyb')

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
						char						file_name[256];
						d3pnt						shift;
						d3ang						ang;
						d3rect						drag_sel_box;
						play_blend_type				blend[max_model_blend_animation];
					} animator_state_type;


