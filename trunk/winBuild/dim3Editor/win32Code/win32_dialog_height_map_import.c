/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Height Map Import Dialog

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

int								dialog_height_import_div_cnt,dialog_height_import_size,dialog_height_import_high;

/* =======================================================

      Height Map Import Event Handlers
      
======================================================= */

void dialog_height_import_set(HWND diag)
{
	win32_dialog_set_int(diag,IDC_HEIGHT_MAP_IMPORT_DIVISIONS,25);
	win32_dialog_set_int(diag,IDC_HEIGHT_MAP_IMPORT_SIZE,100000);
	win32_dialog_set_int(diag,IDC_HEIGHT_MAP_IMPORT_HEIGHT,8000);
}

void dialog_height_import_get(HWND diag)
{
	dialog_height_import_div_cnt=win32_dialog_get_int(diag,IDC_HEIGHT_MAP_IMPORT_DIVISIONS);
	dialog_height_import_size=win32_dialog_get_int(diag,IDC_HEIGHT_MAP_IMPORT_SIZE);
	dialog_height_import_high=win32_dialog_get_int(diag,IDC_HEIGHT_MAP_IMPORT_HEIGHT);
}

LRESULT CALLBACK dialog_height_import_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch (msg) {

		case WM_INITDIALOG:
			dialog_height_import_set(diag);
			return(TRUE);

		case WM_COMMAND:
			
			switch (LOWORD(wparam)) {

				case ID_HEIGHT_MAP_IMPORT_OK:
					dialog_height_import_get(diag);
					EndDialog(diag,0);
					return(TRUE);

				case ID_HEIGHT_MAP_IMPORT_CANCEL:
					EndDialog(diag,-1);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Height Map Import Dialog
      
======================================================= */

bool dialog_height_import_run(int *div_cnt,int *size,int *high)
{
	if (DialogBox(hinst,MAKEINTRESOURCE(IDD_HEIGHT_MAP_IMPORT),wnd,dialog_height_import_proc)!=0) return(FALSE);

	*div_cnt=dialog_height_import_div_cnt;
	*size=dialog_height_import_size;
	*high=dialog_height_import_high;

	return(TRUE);
}

