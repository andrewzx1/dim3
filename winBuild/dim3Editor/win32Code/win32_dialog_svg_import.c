/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: SVG Import Dialog

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

int								dialog_svg_import_layer_high;
float							dialog_svg_import_scale;

/* =======================================================

      SVG Import Event Handlers
      
======================================================= */

void dialog_svg_import_set(HWND diag)
{
	win32_dialog_set_float(diag,IDC_SVG_IMPORT_SCALE,dialog_svg_import_scale);
	win32_dialog_set_int(diag,IDC_SVG_IMPORT_LAYER_HEIGHT,dialog_svg_import_layer_high);
}

void dialog_svg_import_get(HWND diag)
{
	dialog_svg_import_scale=win32_dialog_get_float(diag,IDC_SVG_IMPORT_SCALE);
	dialog_svg_import_layer_high=win32_dialog_get_int(diag,IDC_SVG_IMPORT_LAYER_HEIGHT);
}

LRESULT CALLBACK dialog_svg_import_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch (msg) {

		case WM_INITDIALOG:
			dialog_svg_import_set(diag);
			return(TRUE);

		case WM_COMMAND:
			
			switch (LOWORD(wparam)) {

				case ID_SVG_IMPORT_OK:
					dialog_svg_import_get(diag);
					EndDialog(diag,0);
					return(TRUE);

				case ID_SVG_IMPORT_CANCEL:
					EndDialog(diag,-1);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run SVG Import Dialog
      
======================================================= */

bool dialog_svg_import_run(float *scale,int *high)
{
	dialog_svg_import_scale=100.0f;
	dialog_svg_import_layer_high=5000;

	if (DialogBox(hinst,MAKEINTRESOURCE(IDD_SVG_IMPORT),wnd,dialog_svg_import_proc)!=0) return(FALSE);

	*scale=dialog_svg_import_scale;
	*high=dialog_svg_import_layer_high;

	return(TRUE);
}

