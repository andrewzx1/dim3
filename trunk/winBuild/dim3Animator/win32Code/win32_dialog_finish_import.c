/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Dialog Import Finish

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

#include "dim3Animator.h"

#include "resource.h"
#include "interface.h"
#include "ui_common.h"
#include "win32_dialog.h"

extern HINSTANCE				hinst;
extern HWND						wnd;

int								dialog_import_finish_high;

/* =======================================================

      Import Finish Event Handlers
      
======================================================= */

LRESULT CALLBACK dialog_import_finish_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch (msg) {

		case WM_INITDIALOG:
			win32_dialog_set_int(diag,IDC_IMPORT_FINISH_HEIGHT,1000);
			win32_dialog_set_focus(diag,IDC_IMPORT_FINISH_HEIGHT);
			win32_dialog_select_all(diag,IDC_IMPORT_FINISH_HEIGHT);
			return(FALSE);		// return false when keyboard focus has been set

		case WM_COMMAND:

			switch (LOWORD(wparam)) {

				case ID_IMPORT_FINISH_OK:
					dialog_import_finish_high=win32_dialog_get_int(diag,IDC_IMPORT_FINISH_HEIGHT);
					EndDialog(diag,0);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Import Finish
      
======================================================= */

int dialog_import_finish_run(void)
{
	DialogBox(hinst,MAKEINTRESOURCE(IDD_IMPORT_FINISH),wnd,dialog_import_finish_proc);

	return(dialog_import_finish_high);
}
