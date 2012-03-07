/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: File New Routines

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

#ifdef D3_EDITOR
	#include "dim3editor.h"
#else
	#include "dim3Animator.h"
#endif

#include "resource.h"
#include "interface.h"
#include "win32_dialog.h"

extern HINSTANCE				hinst;
extern HWND						wnd;

char							dialog_file_new_file_name[256],
								dialog_file_new_title[256];

/* =======================================================

      File New Event Handlers
      
======================================================= */

LRESULT CALLBACK dialog_file_new_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch (msg) {

		case WM_INITDIALOG:
			SetWindowText(diag,dialog_file_new_title);

			win32_dialog_set_text(diag,IDC_FILE_NEW_NAME,dialog_file_new_file_name);
			win32_dialog_set_focus(diag,IDC_FILE_NEW_NAME);
			win32_dialog_select_all(diag,IDC_FILE_NEW_NAME);

			return(FALSE);		// return false when keyboard focus has been set

		case WM_COMMAND:

			switch (LOWORD(wparam)) {

				case ID_FILE_NEW_NEW:
					win32_dialog_get_text(diag,IDC_FILE_NEW_NAME,dialog_file_new_file_name,256);
					EndDialog(diag,0);
					return(TRUE);

				case IDCANCEL:
					EndDialog(diag,1);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run File New
      
======================================================= */

bool dialog_file_new_run(char *title,char *file_name)
{
	strcpy(dialog_file_new_title,title);
	strcpy(dialog_file_new_file_name,file_name);

	if (DialogBox(hinst,MAKEINTRESOURCE(IDD_FILE_NEW),wnd,dialog_file_new_proc)!=0) return(FALSE);

	strcpy(file_name,dialog_file_new_file_name);
	return(TRUE);
}