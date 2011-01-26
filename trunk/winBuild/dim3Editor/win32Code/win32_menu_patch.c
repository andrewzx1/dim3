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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "dim3Editor.h"

	// remapping for menu items

int				win32_menu_remap[][2]={
					{kCommandFileNew,100},
					{kCommandFileOpen,101},
					{kCommandFileClose,102},
					{kCommandFileSave,103},
					{kCommandFileQuit,104},

					{kCommandEditUndo,200},
					{kCommandEditDelete,201},
					{kCommandEditDuplicate,202},
					{kCommandEditSelectMore,203},

					{kCommandViewFront,300},
					{kCommandViewLeft,301},
					{kCommandViewRight,302},
					{kCommandViewBack,303},
					{kCommandViewTop,304},
					{kCommandViewBottom,305},
					{kCommandViewPerspective,306},
					{kCommandViewOrtho,307},
					{kCommandViewUVLayer1,308},
					{kCommandViewUVLayer2,309},
					{kCommandViewGotoSelect,310},
					{kCommandViewGotoMapCenter,311},
					{kCommandViewClip,312},
					{kCommandViewShowHideLiquids,313},
					{kCommandViewShowHideSpots,314},
					{kCommandViewShowHideLights,315},
					{kCommandViewShowHideNodes,316},
					{kCommandViewSplitHorizontal,317},
					{kCommandViewSplitVertical,318},
					{kCommandViewRemoveSplit,319},

					{kCommandMapAddCinema,400},
					{kCommandMapAddGroup,401},
					{kCommandMapAddMovement,402},
					{kCommandMapRaiseY,403},
					{kCommandMapLowerY,404},
					{kCommandMapCenter,405},
					{kCommandMapResetUV,406},
					{kCommandMapCreateNormals,407},
					{kCommandClearLightMaps,408},
					{kCommandBuildLightMaps,409},
					{kCommandRun,410},

					{kCommandMeshCombine,500},
					{kCommandMeshSplit,501},
					{kCommandMeshTesselate,502},
					{kCommandMeshResize,503},
					{kCommandMeshReposition,504},
					{kCommandMeshFlipX,506},
					{kCommandMeshFlipY,507},
					{kCommandMeshFlipZ,508},
					{kCommandMeshRotateX,509},
					{kCommandMeshRotateY,510},
					{kCommandMeshRotateZ,511},
					{kCommandMeshFreeRotate,512},
					{kCommandMeshRaiseY,513},
					{kCommandMeshLowerY,514},
					{kCommandMeshSelectAllPoly,515},
					{kCommandMeshSnapToGrid,516},
					{kCommandMeshSnapClosestVertex,517},
					{kCommandMeshResetUV,518},
					{kCommandMeshWholeUV,519},
					{kCommandMeshSingleUV,520},
					{kCommandMeshCreateNormals,521},
					{kCommandMeshInvertNormals,522},

					{kCommandPolygonHole,600},
					{kCommandPolygonSnapToGrid,601},
					{kCommandPolygonRotateUV,602},
					{kCommandPolygonFlipU,603},
					{kCommandPolygonFlipV,604},
					{kCommandPolygonInvertNormal,605},
					{kCommandPolygonResetUV,606},
					{kCommandPolygonWholeUV,607},
					{kCommandPolygonSingleUV,608},

					{kCommandVertexSnapToGrid,700},

					{kCommandGroups,800},
					{kCommandGroupMovements,801},
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
