/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Mesh Scale Dialog

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

extern HINSTANCE				hinst;
extern HWND						wnd;

extern map_type					map;

int								dialog_mesh_scale_axis,dialog_mesh_scale_unit;
bool							dialog_mesh_scale_replace;

extern void win32_dialog_set_int(HWND diag,int id,int value);
extern int win32_dialog_get_int(HWND diag,int id);
extern void win32_dialog_combo_add(HWND diag,int id,char *str);
extern void win32_dialog_combo_set_value(HWND diag,int id,int value);
extern int win32_dialog_combo_get_value(HWND diag,int id);

/* =======================================================

      Mesh Scale Event Handlers
      
======================================================= */

void dialog_mesh_scale_set(HWND diag)
{
	win32_dialog_combo_add(diag,IDC_MESH_SCALE_AXIS,"X");
	win32_dialog_combo_add(diag,IDC_MESH_SCALE_AXIS,"Y");
	win32_dialog_combo_add(diag,IDC_MESH_SCALE_AXIS,"Z");
	win32_dialog_combo_set_value(diag,IDC_MESH_SCALE_AXIS,0);
	win32_dialog_set_int(diag,IDC_MESH_SCALE_UNITS,(20*map_enlarge));
}

void dialog_mesh_scale_get(HWND diag)
{
	dialog_mesh_scale_axis=win32_dialog_combo_get_value(diag,IDC_MESH_SCALE_AXIS);
	dialog_mesh_scale_unit=win32_dialog_get_int(diag,IDC_MESH_SCALE_UNITS);
}

LRESULT CALLBACK dialog_mesh_scale_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch (msg) {

		case WM_INITDIALOG:
			dialog_mesh_scale_set(diag);
			return(TRUE);

		case WM_COMMAND:
			
			switch (LOWORD(wparam)) {

				case ID_MESH_SCALE_SCALE:
					dialog_mesh_scale_replace=FALSE;
					dialog_mesh_scale_get(diag);
					EndDialog(diag,0);
					return(TRUE);

				case ID_MESH_SCALE_REPLACE:
					dialog_mesh_scale_replace=TRUE;
					dialog_mesh_scale_get(diag);
					EndDialog(diag,0);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Mesh Scale Dialog
      
======================================================= */

bool dialog_mesh_scale_run(bool replace_ok,int *scale_axis,int *scale_unit)
{
	dialog_mesh_scale_replace=FALSE;

	DialogBox(hinst,MAKEINTRESOURCE(IDD_MESH_SCALE),wnd,dialog_mesh_scale_proc);

	*scale_axis=dialog_mesh_scale_axis;
	*scale_unit=dialog_mesh_scale_unit;

	return(dialog_mesh_scale_replace);
}

