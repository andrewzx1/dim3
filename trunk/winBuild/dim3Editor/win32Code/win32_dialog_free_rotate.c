/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Free Rotate Dialog

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

#include "dim3editor.h"
#include "resource.h"
#include "win32_dialog.h"

extern HINSTANCE				hinst;
extern HWND						wnd;

extern map_type					map;

float							dialog_free_rotate_rot_x,dialog_free_rotate_rot_y,dialog_free_rotate_rot_z;

/* =======================================================

      Free Rotate Event Handlers
      
======================================================= */

void dialog_free_rotate_set(HWND diag)
{
	win32_dialog_set_float(diag,IDC_FREE_ROTATE_X_ROT,0.0f);
	win32_dialog_set_float(diag,IDC_FREE_ROTATE_Y_ROT,0.0f);
	win32_dialog_set_float(diag,IDC_FREE_ROTATE_Z_ROT,0.0f);
}

void dialog_free_rotate_get(HWND diag)
{
	dialog_free_rotate_rot_x=win32_dialog_get_float(diag,IDC_FREE_ROTATE_X_ROT);
	dialog_free_rotate_rot_y=win32_dialog_get_float(diag,IDC_FREE_ROTATE_Y_ROT);
	dialog_free_rotate_rot_z=win32_dialog_get_float(diag,IDC_FREE_ROTATE_Z_ROT);
}

LRESULT CALLBACK dialog_free_rotate_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch (msg) {

		case WM_INITDIALOG:
			dialog_free_rotate_set(diag);
			return(TRUE);

		case WM_COMMAND:
			
			switch (LOWORD(wparam)) {

				case ID_FREE_ROTATE_OK:
					dialog_free_rotate_get(diag);
					EndDialog(diag,0);
					return(TRUE);

				case ID_FREE_ROTATE_CANCEL:
					EndDialog(diag,-1);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Free Rotate Dialog
      
======================================================= */

bool dialog_free_rotate_run(float *rot_x,float *rot_y,float *rot_z)
{
	if (DialogBox(hinst,MAKEINTRESOURCE(IDD_FREE_ROTATE),wnd,dialog_free_rotate_proc)!=0) return(FALSE);

	*rot_x=dialog_free_rotate_rot_x;
	*rot_y=dialog_free_rotate_rot_y;
	*rot_z=dialog_free_rotate_rot_z;

	return(TRUE);
}

