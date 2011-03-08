/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Dialog Set Normal

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

extern HINSTANCE				hinst;
extern HWND						wnd;

d3vct							dialog_set_normal_vct;

/* =======================================================

      Set Normal Event Handlers
      
======================================================= */

LRESULT CALLBACK dialog_set_normal_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch (msg) {

		case WM_INITDIALOG:
			win32_dialog_set_float(diag,IDC_SET_NORMAL_X,dialog_set_normal_vct.x);
			win32_dialog_set_float(diag,IDC_SET_NORMAL_Y,dialog_set_normal_vct.y);
			win32_dialog_set_float(diag,IDC_SET_NORMAL_Z,dialog_set_normal_vct.z);

			win32_dialog_set_focus(diag,IDC_SET_NORMAL_X);
			return(FALSE);		// return false when keyboard focus has been set

		case WM_COMMAND:

			switch (LOWORD(wparam)) {

				case ID_SET_NORMAL_OK:

					dialog_set_normal_vct.x=win32_dialog_get_float(diag,IDC_SET_NORMAL_X);
					dialog_set_normal_vct.y=win32_dialog_get_float(diag,IDC_SET_NORMAL_Y);
					dialog_set_normal_vct.z=win32_dialog_get_float(diag,IDC_SET_NORMAL_Z);
					vector_normalize(&dialog_set_normal_vct);

					EndDialog(diag,0);
					return(TRUE);

				case ID_SET_NORMAL_CANCEL:
					EndDialog(diag,-1);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Set Normal
      
======================================================= */

bool dialog_set_normal_run(d3vct *vct)
{
	memmove(&dialog_set_normal_vct,vct,sizeof(d3vct));

	if (DialogBox(hinst,MAKEINTRESOURCE(IDD_SET_NORMAL),wnd,dialog_set_normal_proc)!=0) return(FALSE);

	memmove(vct,&dialog_set_normal_vct,sizeof(d3vct));

	return(TRUE);
}
