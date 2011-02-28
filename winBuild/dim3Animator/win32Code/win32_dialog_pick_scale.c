/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Dialog Pick Scale

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

int								dialog_import_finish_high;
float							dialog_pick_scale_x,dialog_pick_scale_y,dialog_pick_scale_z;

/* =======================================================

      Pick Scale Event Handlers
      
======================================================= */

LRESULT CALLBACK dialog_pick_scale_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch (msg) {

		case WM_INITDIALOG:
			win32_dialog_combo_add(diag,IDC_PICK_SCALE_TYPE,"Percentage (Ex: 200% = 2x, 50% = 1/2)");
			win32_dialog_combo_add(diag,IDC_PICK_SCALE_TYPE,"To Specific Model Units");
			win32_dialog_combo_set_value(diag,IDC_PICK_SCALE_TYPE,0);

			win32_dialog_set_float(diag,IDC_PICK_SCALE_X,100.0f);
			win32_dialog_set_float(diag,IDC_PICK_SCALE_Y,100.0f);
			win32_dialog_set_float(diag,IDC_PICK_SCALE_Z,100.0f);
			return(FALSE);		// return false when keyboard focus has been set

		case WM_COMMAND:

			switch (LOWORD(wparam)) {

				case ID_PICK_SCALE_OK:

					switch (win32_dialog_combo_get_value(diag,IDC_PICK_SCALE_TYPE)) {
						case 0:
							dialog_pick_scale_x=win32_dialog_get_float(diag,IDC_PICK_SCALE_X)/100.0f;		// reuse pick scale for return
							dialog_pick_scale_y=win32_dialog_get_float(diag,IDC_PICK_SCALE_Y)/100.0f;
							dialog_pick_scale_z=win32_dialog_get_float(diag,IDC_PICK_SCALE_Z)/100.0f;
							break;
						case 1:
							dialog_pick_scale_x=win32_dialog_get_float(diag,IDC_PICK_SCALE_X)/dialog_pick_scale_x;
							dialog_pick_scale_y=win32_dialog_get_float(diag,IDC_PICK_SCALE_Y)/dialog_pick_scale_y;
							dialog_pick_scale_z=win32_dialog_get_float(diag,IDC_PICK_SCALE_Z)/dialog_pick_scale_z;
							break;
					}

					EndDialog(diag,0);
					return(TRUE);

				case ID_PICK_SCALE_CANCEL:
					EndDialog(diag,-1);
					return(TRUE);

				case IDC_PICK_SCALE_TYPE:
					if (HIWORD(wparam)!=LBN_SELCHANGE) return(TRUE);

					switch (win32_dialog_combo_get_value(diag,IDC_PICK_SCALE_TYPE)) {
						case 0:
							win32_dialog_set_float(diag,IDC_PICK_SCALE_X,100.0f);
							win32_dialog_set_float(diag,IDC_PICK_SCALE_Y,100.0f);
							win32_dialog_set_float(diag,IDC_PICK_SCALE_Z,100.0f);
							break;
						case 1:
							win32_dialog_set_float(diag,IDC_PICK_SCALE_X,dialog_pick_scale_x);
							win32_dialog_set_float(diag,IDC_PICK_SCALE_Y,dialog_pick_scale_y);
							win32_dialog_set_float(diag,IDC_PICK_SCALE_Z,dialog_pick_scale_z);
							break;
					}

					return(TRUE);
			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Pick Scale
      
======================================================= */

bool dialog_scale_run(model_type *mdl,float *x,float *y,float *z)
{
	int					minx,maxx,minz,maxz,miny,maxy;

		// find total size of model
		
	model_get_vertex_extent_all(mdl,&minx,&maxx,&minz,&maxz,&miny,&maxy);
	dialog_pick_scale_x=(float)abs(maxx-minx);
	dialog_pick_scale_y=(float)abs(maxy-miny);
	dialog_pick_scale_z=(float)abs(maxz-minz);

		// run dialog

	if (DialogBox(hinst,MAKEINTRESOURCE(IDD_PICK_SCALE),wnd,dialog_pick_scale_proc)!=0) return(FALSE);

		// setup value

	*x=dialog_pick_scale_x;
	*y=dialog_pick_scale_y;
	*z=dialog_pick_scale_z;

	return(TRUE);
}
