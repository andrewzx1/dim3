#include "dim3Editor.h"

	// remapping for menu items

int				win32_menu_remap[][2]={
					{kCommandFileNew,100},
					{kCommandFileOpen,101},
					{kCommandFileClose,102},
					{kCommandFileSave,103},
					{kCommandFileAutoGenerateMap,104},
					{kCommandFileQuit,105},

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

					{kCommandMapSettings,400},
					{kCommandMapCinemas,401},
					{kCommandMapRaiseY,402},
					{kCommandMapLowerY,403},
					{kCommandMapCenter,404},
					{kCommandMapResetUV,405},
					{kCommandMapOptimize,406},
					{kCommandMapCreateNormals,407},
					{kCommandClearLightMaps,408},
					{kCommandBuildLightMaps,409},
					{kCommandRun,410},

					{kCommandMeshCombine,500},
					{kCommandMeshSplit,501},
					{kCommandMeshTesselate,502},
					{kCommandMeshResize,503},
					{kCommandMeshReposition,504},
					{kCommandMeshSkew,505},
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
