/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: win32 Animator Menu Patch

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

#include "dim3Animator.h"
#include "resource.h"

	// remapping for menu items

int				win32_menu_remap[][2]={
					{kCommandFileNew,ID_FILE_NEW},
					{kCommandFileOpen,ID_FILE_OPEN},
					{kCommandFileClose,ID_FILE_CLOSE},
					{kCommandFileSave,ID_FILE_SAVE},
					{kCommandFilePreference,ID_FILE_PREFERENCE},
					{kCommandFileQuit,ID_FILE_EXIT},

					{kCommandEditUndo,ID_EDIT_UNDO},
					{kCommandEditSelectMore,ID_EDIT_SELECT_MORE},

					{kCommandPrepareModel,ID_MODEL_PREPARE_MODEL},
					{kCommandCalcBoxes,ID_MODEL_CALC_BOXES},
					{kCommandCalcNormals,ID_MODEL_CALC_NORMAL},
					{kCommandScaleAll,ID_MODEL_SCALE},
					{kCommandFlipXAll,ID_MODEL_FLIP_X},
					{kCommandFlipYAll,ID_MODEL_FLIP_Y},
					{kCommandFlipZAll,ID_MODEL_FLIP_Z},
					{kCommandSwapXZAll,ID_MODEL_SWAP_XZ},
					{kCommandSwapYZAll,ID_MODEL_SWAP_YZ},
					{kCommandCenterXZAll,ID_MODEL_CENTER_XZ},
					{kCommandFloorYAll,ID_MODEL_FLOOR_Y},
					{kCommandAddHitBox,ID_MODEL_ADD_HIT_BOX},

					{kCommandFront,ID_VIEW_FRONT},
					{kCommandLeft,ID_VIEW_LEFT},
					{kCommandRight,ID_VIEW_RIGHT},
					{kCommandBack,ID_VIEW_BACK},
					{kCommandTop,ID_VIEW_TOP},
					{kCommandBottom,ID_VIEW_BOTTOM},

					{kCommandNewMesh,ID_MESH_ADD},
					{kCommandDuplicateMesh,ID_MESH_DUPLICATE},
					{kCommandImportOBJ,ID_MESH_IMPORT_OBJ},
					{kCommandReplaceOBJ,ID_MESH_REPLACE_OBJ},
					{kCommandInsertXML,ID_MESH_ADD_MESH},
					{kCommandScale,ID_MESH_SCALE},
					{kCommandFlipX,ID_MESH_FLIP_X},
					{kCommandFlipY,ID_MESH_FLIP_Y},
					{kCommandFlipZ,ID_MESH_FLIP_Z},
					{kCommandFlipU,ID_MESH_FLIP_U},
					{kCommandFlipV,ID_MESH_FLIP_V},
					{kCommandSwapXZ,ID_MESH_SWAP_XZ},
					{kCommandSwapYZ,ID_MESH_SWAP_YZ},
					{kCommandCenterXZ,ID_MESH_CENTER_XZ},
					{kCommandFloorY,ID_MESH_FLOOR_Y},
					{kCommandMeshTessellate,ID_MESH_TESSELLATE},

					{kCommandVertexSelectAll,ID_VERTEX_SELECT_ALL},
					{kCommandVertexSelectNotAttached,ID_VERTEX_SELECT_NOT_ATTACHED},
					{kCommandVertexInvertNormals,ID_VERTEX_INVERT_NORMAL},
					{kCommandVertexSetNormals,ID_VERTEX_SET_NORMAL},
					{kCommandVertexSetNormalsOut,ID_VERTEX_SET_NORMAL_OUT},
					{kCommandVertexSetNormalsIn,ID_VERTEX_SET_NORMAL_IN},
					{kCommandVertexClearBones,ID_VERTEX_CLEAR_BONE},
					{kCommandVertexAutoBones,ID_VERTEX_AUTO_SET_BONE},
					{kCommandVertexHideSelected,ID_VERTEX_HIDE_SELECTED},
					{kCommandVertexHideNonSelected,ID_VERTEX_HIDE_NON_SELECTED},
					{kCommandVertexShowAll,ID_VERTEX_SHOW_ALL},
					{kCommandVertexCollapseSelected,ID_VERTEX_COLLAPSE_SELECTED},
					{kCommandVertexCollapseSimilar,ID_VERTEX_COLLAPSE_SIMILAR},
					{kCommandVertexDelete,ID_VERTEX_DELETE},
					{kCommandVertexPolygonDelete,ID_VERTEX_DELETE_POLYGON},
					{kCommandVertexMakeQuad,ID_VERTEX_MAKE_QUAD},
					{kCommandVertexTessellatePoly,ID_VERTEX_TESSELLATE_POLYGON},

					{kCommandNewBone,ID_BONE_ADD},
					{kCommandSetBone,ID_BONE_SET},
					{kCommandGoToParentBone,ID_BONE_GOTO_PARENT},

					{kCommandNewPose,ID_POSE_NEW},
					{kCommandDupPose,ID_POSE_DUPLICATE},
					{kCommandClearPose,ID_POSE_CLEAR},

					{kCommandNewAnimate,ID_ANIMATION_NEW},
					{kCommandDupAnimate,ID_ANIMATION_DUPLICATE},
					{kCommandResetTimeAnimate,ID_ANIMATION_SCALE_TIME},
					{kCommandPlayAnimate,ID_ANIMATION_PLAY},
					{kCommandPlayBlendAnimate,ID_ANIMATION_PLAY_BLENDED},

					{-1,-1}
				};

/* =======================================================

      Menu Lookups
      
======================================================= */

int os_win32_menu_lookup(int id)
{
	int			idx;

	idx=0;

	while (TRUE) {
		if (win32_menu_remap[idx][1]==-1) return(-1);
		if (win32_menu_remap[idx][1]==id) return(win32_menu_remap[idx][0]);
		idx++;
	}

	return(-1);
}
