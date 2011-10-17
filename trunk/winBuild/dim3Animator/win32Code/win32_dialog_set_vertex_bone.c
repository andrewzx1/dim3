/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Dialog Set Vertex Bone

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

#include "resource.h"
#include "interface.h"
#include "ui_common.h"
#include "win32_dialog.h"

extern model_type				model;

extern HINSTANCE				hinst;
extern HWND						wnd;

int								dialog_set_vertex_bone_major_bone_idx,dialog_set_vertex_bone_minor_bone_idx;
float							dialog_set_vertex_bone_factor;

/* =======================================================

      Set and Get Bone Combo
      
======================================================= */

void win32_dialog_set_bone_combo(HWND diag,int id,int bone_idx)
{
	int				n;
	
	win32_dialog_combo_clear(diag,id);

	win32_dialog_combo_add(diag,id,"None");
	
	for (n=0;n!=model.nbone;n++) {
		win32_dialog_combo_add(diag,id,model.bones[n].name);
	}
	
	if (bone_idx==-1) {
		win32_dialog_combo_set_value(diag,id,0);
	}
	else {
		win32_dialog_combo_set_value(diag,id,(bone_idx+1));
	}
}

int win32_dialog_get_bone_combo(HWND diag,int id)
{
	int				sel_idx;
	
	sel_idx=win32_dialog_combo_get_value(diag,id);
	if (sel_idx==0) return(-1);
	
	return(sel_idx-1);
}

/* =======================================================

      Set Vertex Bone Event Handlers
      
======================================================= */

LRESULT CALLBACK dialog_set_vertex_bone_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch (msg) {

		case WM_INITDIALOG:
			win32_dialog_set_bone_combo(diag,IDC_SET_VERTEX_BONE_MAJOR_BONE,-1);
			win32_dialog_set_bone_combo(diag,IDC_SET_VERTEX_BONE_MINOR_BONE,-1);
			win32_dialog_set_int(diag,IDC_SET_VERTEX_BONE_FACTOR,100);
			return(FALSE);		// return false when keyboard focus has been set

		case WM_COMMAND:

			switch (LOWORD(wparam)) {

				case ID_SET_VERTEX_BONE_OK:
					dialog_set_vertex_bone_major_bone_idx=win32_dialog_get_bone_combo(diag,IDC_SET_VERTEX_BONE_MAJOR_BONE);
					dialog_set_vertex_bone_minor_bone_idx=win32_dialog_get_bone_combo(diag,IDC_SET_VERTEX_BONE_MINOR_BONE);
					dialog_set_vertex_bone_factor=((float)win32_dialog_get_int(diag,IDC_SET_VERTEX_BONE_FACTOR))/100.0f;
					EndDialog(diag,0);
					return(TRUE);

				case ID_SET_VERTEX_BONE_CANCEL:
					EndDialog(diag,-1);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Set Vertex Bone
      
======================================================= */

bool dialog_set_vertex_bone_run(int *major_bone_idx,int *minor_bone_idx,float *factor)
{
	if (DialogBox(hinst,MAKEINTRESOURCE(IDD_SET_VERTEX_BONE),wnd,dialog_set_vertex_bone_proc)!=0) return(FALSE);

	*major_bone_idx=dialog_set_vertex_bone_major_bone_idx;
	*minor_bone_idx=dialog_set_vertex_bone_minor_bone_idx;
	*factor=dialog_set_vertex_bone_factor;

	return(TRUE);
}
