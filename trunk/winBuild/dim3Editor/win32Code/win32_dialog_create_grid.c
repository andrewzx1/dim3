/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Create Grid Dialog

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

int								dialog_create_grid_x_div,dialog_create_grid_y_div,dialog_create_grid_z_div;

/* =======================================================

      Create Grid Event Handlers
      
======================================================= */

void dialog_create_grid_set(HWND diag)
{
	win32_dialog_set_int(diag,IDC_CREATE_GRID_X_DIVISIONS,10);
	win32_dialog_set_int(diag,IDC_CREATE_GRID_Y_DIVISIONS,10);
	win32_dialog_set_int(diag,IDC_CREATE_GRID_Z_DIVISIONS,10);
}

void dialog_create_grid_get(HWND diag)
{
	dialog_create_grid_x_div=win32_dialog_get_int(diag,IDC_CREATE_GRID_X_DIVISIONS);
	dialog_create_grid_y_div=win32_dialog_get_int(diag,IDC_CREATE_GRID_Y_DIVISIONS);
	dialog_create_grid_z_div=win32_dialog_get_int(diag,IDC_CREATE_GRID_Z_DIVISIONS);
}

LRESULT CALLBACK dialog_create_grid_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch (msg) {

		case WM_INITDIALOG:
			dialog_create_grid_set(diag);
			return(TRUE);

		case WM_COMMAND:
			
			switch (LOWORD(wparam)) {

				case ID_CREATE_GRID_OK:
					dialog_create_grid_get(diag);
					EndDialog(diag,0);
					return(TRUE);

				case ID_CREATE_GRID_CANCEL:
					EndDialog(diag,-1);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Create Grid Dialog
      
======================================================= */

bool dialog_create_grid_mesh_run(int *xdiv,int *ydiv,int *zdiv)
{
	if (DialogBox(hinst,MAKEINTRESOURCE(IDD_CREATE_GRID),wnd,dialog_create_grid_proc)!=0) return(FALSE);

	*xdiv=dialog_create_grid_x_div;
	*ydiv=dialog_create_grid_y_div;
	*zdiv=dialog_create_grid_z_div;

	return(TRUE);
}

