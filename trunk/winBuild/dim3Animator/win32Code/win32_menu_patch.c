/****************************** File *********************************

Module: dim3 Editor
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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "dim3Animator.h"

	// remapping for menu items

int				win32_menu_remap[][2]={
					{kCommandNew,100},
					{kCommandOpen,101},
					{kCommandClose,102},
					{kCommandSave,103},
					{kCommandFileQuit,104},

					{kCommandUndo,200},

					{kCommandPrepareModel,300},
					{kCommandCalcBoxes,301},
					{kCommandCalcNormals,302},
					{kCommandScaleAll,303},
					{kCommandFlipXAll,304},
					{kCommandFlipYAll,305},
					{kCommandFlipZAll,306},
					{kCommandSwapXZAll,307},
					{kCommandSwapYZAll,308},
					{kCommandCenterXZAll,309},
					{kCommandFloorYAll,310},
					{kCommandAddHitBox,311},

					{kCommandFront,400},
					{kCommandLeft,401},
					{kCommandRight,402},
					{kCommandBack,403},
					{kCommandTop,404},
					{kCommandBottom,405},

					{kCommandNewMesh,500},
					{kCommandDuplicateMesh,501},
					{kCommandImportOBJ,502},
					{kCommandReplaceOBJ,503},
					{kCommandInsertXML,504},
					{kCommandScale,505},
					{kCommandFlipX,506},
					{kCommandFlipY,507},
					{kCommandFlipZ,508},
					{kCommandFlipU,509},
					{kCommandFlipV,510},
					{kCommandSwapXZ,511},
					{kCommandSwapYZ,512},
					{kCommandCenterXZ,513},
					{kCommandFloorY,514},

					{kCommandVertexSelectAll,600},
					{kCommandVertexSelectNotAttached,601},
					{kCommandVertexInvertNormals,602},
					{kCommandVertexSetNormals,603},
					{kCommandVertexClearBones,604},
					{kCommandVertexAutoBones,605},
					{kCommandVertexHideSelected,606},
					{kCommandVertexHideNonSelected,607},
					{kCommandVertexShowAll,608},
					{kCommandVertexDelete,609},

					{kCommandNewBone,700},
					{kCommandSetBone,701},
					{kCommandGoToParentBone,702},

					{kCommandNewPose,800},
					{kCommandDupPose,801},
					{kCommandClearPose,804},

					{kCommandNewAnimate,900},
					{kCommandDupAnimate,901},
					{kCommandAddPoseMoveParticle,902},
					{kCommandDeletePoseMoveParticle,903},
					{kCommandAddPoseMoveRing,904},
					{kCommandDeletePoseMoveRing,905},
					{kCommandResetTimeAnimate,906},
					{kCommandPlayAnimate,907},
					{kCommandPlayBlendAnimate,908},
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
