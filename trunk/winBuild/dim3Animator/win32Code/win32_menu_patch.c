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

					{kCommandSettings,300},
					{kCommandHitBoxes,301},
					{kCommandPrepareModel,302},
					{kCommandCalcBoxes,303},
					{kCommandCalcNormals,304},
					{kCommandScaleAll,305},
					{kCommandFlipXAll,306},
					{kCommandFlipYAll,307},
					{kCommandFlipZAll,308},
					{kCommandSwapXZAll,309},
					{kCommandSwapYZAll,310},
					{kCommandCenterXZAll,311},
					{kCommandFloorYAll,312},

					{kCommandFront,400},
					{kCommandLeft,401},
					{kCommandRight,402},
					{kCommandBack,403},
					{kCommandTop,404},
					{kCommandBottom,405},

					{kCommandNewMesh,500},
					{kCommandDuplicateMesh,501},
					{kCommandCopyMesh,502},
					{kCommandDeleteMesh,503},
					{kCommandImportOBJ,504},
					{kCommandReplaceOBJ,505},
					{kCommandInsertXML,506},
					{kCommandScale,507},
					{kCommandFlipX,508},
					{kCommandFlipY,509},
					{kCommandFlipZ,510},
					{kCommandFlipU,511},
					{kCommandFlipV,512},
					{kCommandSwapXZ,513},
					{kCommandSwapYZ,514},
					{kCommandCenterXZ,515},
					{kCommandFloorY,516},

					{kCommandVertexSelectAll,600},
					{kCommandVertexSelectNotAttached,601},
					{kCommandVertexNudge,602},
					{kCommandVertexScale,603},
					{kCommandVertexRotate,604},
					{kCommandVertexInvertNormals,605},
					{kCommandVertexSetNormals,606},
					{kCommandVertexClearBones,607},
					{kCommandVertexHideSelected,608},
					{kCommandVertexHideNonSelected,609},
					{kCommandVertexShowAll,610},
					{kCommandVertexDelete,611},

					{kCommandNewBone,700},
					{kCommandSetBone,701},
					{kCommandNudgeBone,702},
					{kCommandDeleteBone,703},
					{kCommandSelectVertexNearBone,704},

					{kCommandNewPose,800},
					{kCommandDupPose,801},
					{kCommandPreviousPose,802},
					{kCommandNextPose,803},
					{kCommandClearPose,804},
					{kCommandDeletePose,805},
					{kCommandGoToBoneMoveParent,806},

					{kCommandNewAnimate,900},
					{kCommandDupAnimate,901},
					{kCommandDeleteAnimate,902},
					{kCommandResetTimeAnimate,903},
					{kCommandPlayAnimate,904},
					{kCommandPlayBlendAnimate,905},
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
