/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Reposition Dialog

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

#include "dim3editor.h"
#include "resource.h"
#include "win32_dialog.h"

extern HINSTANCE				hinst;
extern HWND						wnd;

extern map_type					map;

d3pnt							dialog_reposition_min,dialog_reposition_max;

/* =======================================================

      Reposition Event Handlers
      
======================================================= */

void dialog_reposition_set(HWND diag)
{
	win32_dialog_set_int(diag,IDC_REPOSITION_X_TOPLEFT,dialog_reposition_min.x);
	win32_dialog_set_int(diag,IDC_REPOSITION_Y_TOPLEFT,dialog_reposition_min.y);
	win32_dialog_set_int(diag,IDC_REPOSITION_Z_TOPLEFT,dialog_reposition_min.z);
	win32_dialog_set_int(diag,IDC_REPOSITION_X_BOTTOMRIGHT,dialog_reposition_max.x);
	win32_dialog_set_int(diag,IDC_REPOSITION_Y_BOTTOMRIGHT,dialog_reposition_max.y);
	win32_dialog_set_int(diag,IDC_REPOSITION_Z_BOTTOMRIGHT,dialog_reposition_max.z);
}

void dialog_reposition_get(HWND diag)
{
	dialog_reposition_min.x=win32_dialog_get_int(diag,IDC_REPOSITION_X_TOPLEFT);
	dialog_reposition_min.y=win32_dialog_get_int(diag,IDC_REPOSITION_Y_TOPLEFT);
	dialog_reposition_min.z=win32_dialog_get_int(diag,IDC_REPOSITION_Z_TOPLEFT);
	dialog_reposition_max.x=win32_dialog_get_int(diag,IDC_REPOSITION_X_BOTTOMRIGHT);
	dialog_reposition_max.y=win32_dialog_get_int(diag,IDC_REPOSITION_Y_BOTTOMRIGHT);
	dialog_reposition_max.z=win32_dialog_get_int(diag,IDC_REPOSITION_Z_BOTTOMRIGHT);
}

LRESULT CALLBACK dialog_reposition_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch (msg) {

		case WM_INITDIALOG:
			dialog_reposition_set(diag);
			return(TRUE);

		case WM_COMMAND:
			
			switch (LOWORD(wparam)) {

				case ID_REPOSITION_OK:
					dialog_reposition_get(diag);
					EndDialog(diag,0);
					return(TRUE);

				case ID_RESIZE_CANCEL:
					EndDialog(diag,-1);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Reposition Dialog
      
======================================================= */

bool dialog_reposition_run(d3pnt *min,d3pnt *max)
{
	memmove(&dialog_reposition_min,min,sizeof(d3pnt));
	memmove(&dialog_reposition_max,max,sizeof(d3pnt));

	if (DialogBox(hinst,MAKEINTRESOURCE(IDD_REPOSITION),wnd,dialog_reposition_proc)!=0) return(FALSE);

	memmove(min,&dialog_reposition_min,sizeof(d3pnt));
	memmove(max,&dialog_reposition_max,sizeof(d3pnt));

	return(TRUE);
}

