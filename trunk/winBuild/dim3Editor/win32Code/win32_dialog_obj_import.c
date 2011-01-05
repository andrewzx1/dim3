/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: OBJ Import Dialog

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

int								dialog_obj_import_type,dialog_obj_import_axis,dialog_obj_import_unit;

/* =======================================================

      Mesh Scale Event Handlers
      
======================================================= */

void dialog_obj_import_set(HWND diag)
{
	win32_dialog_combo_add(diag,IDC_OBJ_IMPORT_TYPE,"Import New Mesh(es)");
	win32_dialog_combo_add(diag,IDC_OBJ_IMPORT_TYPE,"Replace Selected Mesh with Imported Mesh(es)");
	win32_dialog_combo_add(diag,IDC_OBJ_IMPORT_TYPE,"Replace All Previously Imported Mesh(es) with New Copy");
	win32_dialog_combo_set_value(diag,IDC_OBJ_IMPORT_TYPE,0);

	win32_dialog_combo_add(diag,IDC_OBJ_IMPORT_AXIS,"X");
	win32_dialog_combo_add(diag,IDC_OBJ_IMPORT_AXIS,"Y");
	win32_dialog_combo_add(diag,IDC_OBJ_IMPORT_AXIS,"Z");
	win32_dialog_combo_set_value(diag,IDC_OBJ_IMPORT_AXIS,0);

	win32_dialog_set_int(diag,IDC_OBJ_IMPORT_UNITS,(20*map_enlarge));
}

void dialog_obj_import_get(HWND diag)
{
	dialog_obj_import_type=win32_dialog_combo_get_value(diag,IDC_OBJ_IMPORT_TYPE);
	dialog_obj_import_axis=win32_dialog_combo_get_value(diag,IDC_OBJ_IMPORT_AXIS);
	dialog_obj_import_unit=win32_dialog_get_int(diag,IDC_OBJ_IMPORT_UNITS);
}

LRESULT CALLBACK dialog_obj_import_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch (msg) {

		case WM_INITDIALOG:
			dialog_obj_import_set(diag);
			return(TRUE);

		case WM_COMMAND:
			
			switch (LOWORD(wparam)) {

				case ID_OBJ_IMPORT_OK:
					dialog_obj_import_get(diag);
					EndDialog(diag,0);
					return(TRUE);

				case ID_OBJ_IMPORT_CANCEL:
					EndDialog(diag,-1);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Mesh Scale Dialog
      
======================================================= */

int dialog_obj_import_run(int *scale_axis,int *scale_unit)
{
	dialog_obj_import_type=0;

	if (DialogBox(hinst,MAKEINTRESOURCE(IDD_OBJ_IMPORT),wnd,dialog_obj_import_proc)!=0) return(-1);

	*scale_axis=dialog_obj_import_axis;
	*scale_unit=dialog_obj_import_unit;

	return(dialog_obj_import_type);
}

