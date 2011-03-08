/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Dialog Scale Animation

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
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

int								dialog_animate_idx;

/* =======================================================

      Animate Time Reset Utility
	        
======================================================= */

int dialog_animation_reset_time_get_total_time(void)
{
	int						n,tot_time;
	model_animate_type		*animate;
	
	animate=&model.animates[dialog_animate_idx];
	
	tot_time=0;
	
	for (n=0;n!=animate->npose_move;n++) {
		tot_time+=animate->pose_moves[n].msec;
	}
	
	return(tot_time);
}

void dialog_animate_reset_change(int tot_time)
{
	int						n,org_tot_time;
	model_animate_type		*animate;

	org_tot_time=dialog_animation_reset_time_get_total_time();
	if (org_tot_time==tot_time) return;
	
	animate=&model.animates[dialog_animate_idx];
	
	for (n=0;n!=animate->npose_move;n++) {
		animate->pose_moves[n].msec=(animate->pose_moves[n].msec*tot_time)/org_tot_time;
	}
}

/* =======================================================

      Scale Animation Event Handlers
      
======================================================= */

LRESULT CALLBACK dialog_scale_animation_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	int				tot_time;

	switch (msg) {

		case WM_INITDIALOG:
			win32_dialog_set_int(diag,IDC_SCALE_ANIMTION_TIME,dialog_animation_reset_time_get_total_time());

			win32_dialog_set_focus(diag,IDC_SCALE_ANIMTION_TIME);
			win32_dialog_select_all(diag,IDC_SCALE_ANIMTION_TIME);
			return(FALSE);		// return false when keyboard focus has been set

		case WM_COMMAND:

			switch (LOWORD(wparam)) {

				case ID_SCALE_ANIMATION_OK:
					tot_time=win32_dialog_get_int(diag,IDC_SCALE_ANIMTION_TIME);
					dialog_animate_reset_change(tot_time);

					EndDialog(diag,0);
					return(TRUE);

				case ID_SCALE_ANIMATION_CANCEL:
					EndDialog(diag,-1);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Scale Animation
      
======================================================= */

bool dialog_animation_reset_time_run(int animate_idx)
{
	dialog_animate_idx=animate_idx;

	return(DialogBox(hinst,MAKEINTRESOURCE(IDD_SCALE_ANIMATION),wnd,dialog_scale_animation_proc)==0);
}
