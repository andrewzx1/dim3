/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Dialog Play Blended Animation

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
extern animator_state_type		state;

extern HINSTANCE				hinst;
extern HWND						wnd;

/* =======================================================

      Set and Get Bone Combo
      
======================================================= */

void win32_dialog_set_animate_combo(HWND diag,int id,int sel_idx,bool use_none)
{
	int				n;
	
	win32_dialog_combo_clear(diag,id);

	if (use_none) win32_dialog_combo_add(diag,id,"None");
	
	for (n=0;n!=model.nanimate;n++) {
		win32_dialog_combo_add(diag,id,model.animates[n].name);
	}

	if (!use_none) {
		if (sel_idx<0) sel_idx=0;
		win32_dialog_combo_set_value(diag,id,sel_idx);
	}
	else {
		if (sel_idx==-1) {
			win32_dialog_combo_set_value(diag,id,0);
		}
		else {
			win32_dialog_combo_set_value(diag,id,(sel_idx+1));
		}
	}
}

int win32_dialog_get_animate_combo(HWND diag,int id,bool use_none)
{
	int				sel_idx;
	
	sel_idx=win32_dialog_combo_get_value(diag,id);
	if (!use_none) return(sel_idx);

	if (sel_idx==0) return(-1);
	
	return(sel_idx-1);
}

/* =======================================================

      Play Blended Animation Event Handlers
      
======================================================= */

LRESULT CALLBACK dialog_blend_animation_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch (msg) {

		case WM_INITDIALOG:
			win32_dialog_set_animate_combo(diag,IDC_BLEND_ANIMATION_1,state.blend[0].animate_idx,FALSE);
			win32_dialog_set_animate_combo(diag,IDC_BLEND_ANIMATION_2,state.blend[1].animate_idx,TRUE);
			win32_dialog_set_animate_combo(diag,IDC_BLEND_ANIMATION_3,state.blend[2].animate_idx,TRUE);
			win32_dialog_set_animate_combo(diag,IDC_BLEND_ANIMATION_4,state.blend[3].animate_idx,TRUE);
			return(FALSE);		// return false when keyboard focus has been set

		case WM_COMMAND:

			switch (LOWORD(wparam)) {

				case ID_BLEND_ANIMATION_OK:
					state.blend[0].animate_idx=win32_dialog_get_animate_combo(diag,IDC_BLEND_ANIMATION_1,FALSE);
					state.blend[1].animate_idx=win32_dialog_get_animate_combo(diag,IDC_BLEND_ANIMATION_2,TRUE);
					state.blend[2].animate_idx=win32_dialog_get_animate_combo(diag,IDC_BLEND_ANIMATION_3,TRUE);
					state.blend[3].animate_idx=win32_dialog_get_animate_combo(diag,IDC_BLEND_ANIMATION_4,TRUE);

					EndDialog(diag,0);
					return(TRUE);

				case ID_BLEND_ANIMATION_CANCEL:
					EndDialog(diag,-1);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Play Blended Animation
      
======================================================= */

bool dialog_play_blend_animation_run(void)
{
	return(DialogBox(hinst,MAKEINTRESOURCE(IDD_BLEND_ANIMATION),wnd,dialog_blend_animation_proc)==0);
}
