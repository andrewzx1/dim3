/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: win32 Editor Menu Patch

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

#include "dim3Editor.h"
#include "resource.h"

	// remapping for menu items

int				win32_menu_remap[][2]={
					{kCommandFileSave,ID_FILE_SAVE},
					{kCommandFilePreference,ID_FILE_PREFERENCE},
					{kCommandFileQuit,ID_FILE_EXIT},

					{kCommandEditUndo,ID_FILE_UNDO},
					{kCommandEditDelete,ID_FILE_DELETE},
					{kCommandEditDuplicate,ID_FILE_DUPLICATE},
					{kCommandEditSelectMore,ID_EDIT_SELECT_MORE},

					{kCommandViewFront,ID_VIEW_FRONT},
					{kCommandViewLeft,ID_VIEW_LEFT},
					{kCommandViewRight,ID_VIEW_RIGHT},
					{kCommandViewBack,ID_VIEW_BACK},
					{kCommandViewTop,ID_VIEW_TOP},
					{kCommandViewBottom,ID_VIEW_BOTTOM},
					{kCommandViewPerspective,ID_VIEW_PERSPECTIVE},
					{kCommandViewOrtho,ID_VIEW_ORTHO},
					{kCommandViewUVLayer1,ID_VIEW_NORMAL_LAYER},
					{kCommandViewUVLayer2,ID_VIEW_LIGHT_MAP_LAYER},
					{kCommandViewGotoSelect,ID_VIEW_GOTO_SELECTION},
					{kCommandViewGotoMapCenter,ID_VIEW_GOTO_MAP_CENTER},
					{kCommandViewClip,ID_VIEW_CLIPPING},
					{kCommandViewShowHideLiquids,ID_VIEW_LIQUIDS},
					{kCommandViewShowHideSpots,ID_VIEW_SPOT_SCENERY},
					{kCommandViewShowHideLights,ID_VIEW_LIGHT_SOUND_PARTICLE},
					{kCommandViewShowHideNodes,ID_VIEW_NODES},
					{kCommandViewSplitHorizontal,ID_VIEW_SPLIT_HORIZONTAL},
					{kCommandViewSplitVertical,ID_VIEW_SPLIT_VERTICAL},
					{kCommandViewRemoveSplit,ID_VIEW_REMOVE_PANEL},

					{kCommandMapRaiseY,ID_MAP_RAISE_Y},
					{kCommandMapLowerY,ID_MAP_LOWER_Y},
					{kCommandMapCenter,ID_MAP_CENTER},
					{kCommandMapResetUV,ID_MAP_RESET_UV},
					{kCommandMapResetNormals,ID_MAP_RECALC_NORMAL},
					{kCommandMapSortMeshPolys,ID_MAP_SORT_MESH_POLYS},
					{kCommandClearLightMaps,ID_MAP_CLEAR_LIGHT_MAP},
					{kCommandBuildLightMaps,ID_MAP_BUILD_LIGHT_MAP},
					{kCommandBuildLiquidReflectionMaps,ID_MAP_BUILD_LIQUID_REFLECTION_MAP},
					{kCommandRun,ID_MAP_RUN},

					{kCommandMeshCombine,ID_MESH_COMBINE},
					{kCommandMeshSplit,ID_MESH_SPLIT},
					{kCommandMeshTesselate,ID_MESH_TESSELATE},
					{kCommandMeshResize,ID_MESH_RESIZE},
					{kCommandMeshReposition,ID_MESH_REPOSITION},
					{kCommandMeshResizeTexture,ID_MESH_RESIZE_TEXTURE},
					{kCommandMeshFlipX,ID_MESH_FLIP_X},
					{kCommandMeshFlipY,ID_MESH_FLIP_Y},
					{kCommandMeshFlipZ,ID_MESH_FLIP_Z},
					{kCommandMeshRotateX,ID_MESH_ROTATE_X},
					{kCommandMeshRotateY,ID_MESH_ROTATE_Y},
					{kCommandMeshRotateZ,ID_MESH_ROTATE_Z},
					{kCommandMeshFreeRotate,ID_MESH_FREE_ROTATE},
					{kCommandMeshRaiseY,ID_MESH_RAISE_Y},
					{kCommandMeshLowerY,ID_MESH_LOWER_Y},
					{kCommandMeshSelectAllPoly,ID_MESH_SELECT_ALL_POLYGONS},
					{kCommandMeshSnapToGrid,ID_MESH_SNAP_TO_GRID},
					{kCommandMeshSnapClosestVertex,ID_MESH_SNAP_TO_CLOSEST_VERTEX},
					{kCommandMeshResetUV,ID_MESH_RESET_UV},
					{kCommandMeshWholeUV,ID_MESH_WHOLE_UV},
					{kCommandMeshSingleUV,ID_MESH_SINGLE_STAMP_UV},
					{kCommandMeshCreateNormals,ID_MESH_RECALC_NORMAL},
					{kCommandMeshInvertNormals,ID_MESH_INVERT_NORMAL},
					{kCommandMeshSetNormalsOut,ID_MESH_SET_NORMAL_OUT},
					{kCommandMeshSetNormalsIn,ID_MESH_SET_NORMAL_IN},

					{kCommandPolygonHole,ID_POLYGON_PUNCH_HOLE},
					{kCommandPolyTesselate,ID_POLYGON_TESSELATE},
					{kCommandPolygonSnapToGrid,ID_POLYGON_SNAP_TO_GRID},
					{kCommandPolygonRotateUV,ID_POLYGON_ROTATE_UV},
					{kCommandPolygonFlipU,ID_POLYGON_FLIP_U},
					{kCommandPolygonFlipV,ID_POLYGON_FLIP_V},
					{kCommandPolygonRecalcNormal,ID_POLYGON_RECALC_NORMAL},
					{kCommandPolygonInvertNormal,ID_POLYGON_INVERT_NORMAL},
					{kCommandPolygonResetUV,ID_POLYGON_RESET_UV},
					{kCommandPolygonWholeUV,ID_POLYGON_WHILE_UV},
					{kCommandPolygonSingleUV,ID_POLYGON_SINGLE_STAMP_UV},

					{kCommandVertexSnapToGrid,ID_VERTEX_SNAP_TO_GRID},

					{-1,-1},
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
