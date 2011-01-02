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
// inteface defs
//

#define model_view_min_size				512

#define mesh_palette_width				64

#define list_width						170
#define total_list_width				810

//
// tool bars
//

#define tool_button_size				32
#define tool_count						12


#define tool_file_names					{ \
											"Tool Textured", \
											"Tool Mesh", \
											"Tool Bones", \
											"Tool Hit Boxes", \
											"Tool Edit Polygons", \
											"Tool Edit Vertexes", \
											"Tool Boxes", \
											"Tool Normals", \
											"Tool Show First Mesh", \
											"Tool Rotate Mode", \
											"Tool Move Mode", \
											"Tool Play" \
										}

#define tool_separators					{"000010100101"}


//
// Menus
//

#define applemenu						128
#define filemenu						129
#define editmenu						130
#define modelmenu						131
#define viewmenu						132
#define meshmenu						133
#define vertexmenu						134
#define bonemenu						135
#define posemenu						136
#define animatemenu						137

#define modeltransformmenu				141
#define meshimportmenu					140
#define meshtransformmenu				141
#define poseblendingmenu				142

#define kCommandAbout					FOUR_CHAR_CODE('abot')

#define kCommandNew						FOUR_CHAR_CODE('newB')
#define kCommandOpen					FOUR_CHAR_CODE('opnB')
#define kCommandClose					FOUR_CHAR_CODE('clse')
#define kCommandSave					FOUR_CHAR_CODE('savB')

#define kCommandUndo					FOUR_CHAR_CODE('undo')

#define kCommandSettings				FOUR_CHAR_CODE('sets')
#define kCommandHitBoxes				FOUR_CHAR_CODE('hbox')
#define kCommandPrepareModel			FOUR_CHAR_CODE('pmdl')
#define kCommandCalcBoxes				FOUR_CHAR_CODE('cbox')
#define kCommandCalcNormals				FOUR_CHAR_CODE('cnrm')
#define kCommandScaleAll				FOUR_CHAR_CODE('scal')
#define kCommandFlipXAll				FOUR_CHAR_CODE('fxal')
#define kCommandFlipZAll				FOUR_CHAR_CODE('fzal')
#define kCommandFlipYAll				FOUR_CHAR_CODE('fyal')
#define kCommandSwapXZAll				FOUR_CHAR_CODE('spal')
#define kCommandSwapYZAll				FOUR_CHAR_CODE('syal')
#define kCommandCenterXZAll				FOUR_CHAR_CODE('ctal')
#define kCommandFloorYAll				FOUR_CHAR_CODE('fral')

#define kCommandFront					FOUR_CHAR_CODE('vfnt')
#define kCommandLeft					FOUR_CHAR_CODE('vlft')
#define kCommandRight					FOUR_CHAR_CODE('vrgt')
#define kCommandBack					FOUR_CHAR_CODE('vbck')
#define kCommandTop						FOUR_CHAR_CODE('vtop')
#define kCommandBottom					FOUR_CHAR_CODE('vbtm')

#define kCommandNewMesh					FOUR_CHAR_CODE('nmsh')
#define kCommandDuplicateMesh			FOUR_CHAR_CODE('dpmh')
#define kCommandCopyMesh				FOUR_CHAR_CODE('cmsh')
#define kCommandDeleteMesh				FOUR_CHAR_CODE('dmsh')
#define kCommandImportOBJ				FOUR_CHAR_CODE('OBJi')
#define kCommandReplaceOBJ				FOUR_CHAR_CODE('OBJr')
#define kCommandInsertXML				FOUR_CHAR_CODE('opDM')
#define kCommandScale					FOUR_CHAR_CODE('scle')
#define kCommandFlipX					FOUR_CHAR_CODE('flpx')
#define kCommandFlipZ					FOUR_CHAR_CODE('flpz')
#define kCommandFlipY					FOUR_CHAR_CODE('flpy')
#define kCommandFlipU					FOUR_CHAR_CODE('flpu')
#define kCommandFlipV					FOUR_CHAR_CODE('flpv')
#define kCommandSwapXZ					FOUR_CHAR_CODE('spxz')
#define kCommandSwapYZ					FOUR_CHAR_CODE('spyz')
#define kCommandCenterXZ				FOUR_CHAR_CODE('ctxz')
#define kCommandFloorY					FOUR_CHAR_CODE('flry')

#define kCommandVertexSelectAll			FOUR_CHAR_CODE('vsla')
#define kCommandVertexSelectNotAttached	FOUR_CHAR_CODE('vsna')
#define kCommandVertexNudge				FOUR_CHAR_CODE('vnud')
#define kCommandVertexScale				FOUR_CHAR_CODE('vscl')
#define kCommandVertexRotate			FOUR_CHAR_CODE('vrot')
#define kCommandVertexInvertNormals		FOUR_CHAR_CODE('vinm')
#define kCommandVertexSetNormals		FOUR_CHAR_CODE('vsnm')
#define kCommandVertexClearBones		FOUR_CHAR_CODE('vcba')
#define kCommandVertexHideSelected		FOUR_CHAR_CODE('vhsl')
#define kCommandVertexHideNonSelected	FOUR_CHAR_CODE('vhns')
#define kCommandVertexShowAll			FOUR_CHAR_CODE('vsal')
#define kCommandVertexDelete			FOUR_CHAR_CODE('vdel')

#define kCommandNewBone					FOUR_CHAR_CODE('nbne')
#define kCommandSetBone					FOUR_CHAR_CODE('sbn2')
#define kCommandNudgeBone				FOUR_CHAR_CODE('bnud')
#define kCommandDeleteBone				FOUR_CHAR_CODE('dbne')
#define kCommandSelectVertexNearBone	FOUR_CHAR_CODE('svbn')

#define kCommandNewPose					FOUR_CHAR_CODE('npse')
#define kCommandDupPose					FOUR_CHAR_CODE('dpse')
#define kCommandPreviousPose			FOUR_CHAR_CODE('prvp')
#define kCommandNextPose				FOUR_CHAR_CODE('nxtp')
#define kCommandClearPose				FOUR_CHAR_CODE('cpse')
#define kCommandDeletePose				FOUR_CHAR_CODE('kpse')
#define kCommandGoToBoneMoveParent		FOUR_CHAR_CODE('pmbn')

#define kCommandBlendSetSkipAll			FOUR_CHAR_CODE('bsas')
#define kCommandBlendSetSkipNone		FOUR_CHAR_CODE('bsns')
#define kCommandBlendFlipSkip			FOUR_CHAR_CODE('bfsk')

#define kCommandNewAnimate				FOUR_CHAR_CODE('nani')
#define kCommandDupAnimate				FOUR_CHAR_CODE('dani')
#define kCommandDeleteAnimate			FOUR_CHAR_CODE('kani')
#define kCommandResetTimeAnimate		FOUR_CHAR_CODE('artm')
#define kCommandPlayAnimate				FOUR_CHAR_CODE('plya')
#define kCommandPlayBlendAnimate		FOUR_CHAR_CODE('plyb')

//
// Misc UI Constants
//

#define kMeshNameDBColumn				1050

#define kVertexPosDBColumn				1060
#define kVertexBoneDBColumn				1061

#define kBoneTagDBColumn				1070
#define kBonePosDBColumn				1071

#define kPoseNameDBColumn				1080
#define kPoseBoneDBColumn				1081
#define kPoseBoneRotDBColumn			1082
#define kPoseBoneMoveDBColumn			1083
#define kPoseBoneAccDBColumn			1084
#define kPoseBoneBlendDBColumn			1085

#define kAnimateNameDBColumn			1090
#define kAnimatePoseDBColumn			1091
#define kAnimateSwayMoveDBColumn		1092
#define kAnimateOtherDBColumn			1093

#define kHitBoxNameDBColumn				1100

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

#define select_mode_polygon				0
#define select_mode_vertex				1

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

#define draw_trig_select_line_size		4.0f
#define draw_vertex_handle_size			8.0f

#define draw_model_normal_size			3.0f
#define draw_model_normal_len			50.0f

//
// animator state
//

typedef struct		{
						int					tick,animate_idx,pose_move_idx;
					} play_blend_type;
					
typedef struct		{
						int					cur_mesh_idx,cur_bone_idx,
											cur_pose_idx,cur_animate_idx,
											drag_bone_mode,select_mode,
											sel_trig_idx,magnify_z;
						bool				model_open,
											texture,mesh,bone,hit_box,
											normal,view_box,first_mesh,
											playing,play_animate_blend,
											drag_sel_on;
						d3pnt				shift;
						d3ang				ang;
						d3rect				drag_sel_box;
						play_blend_type		blend[max_model_blend_animation];
					} animator_state_type;


