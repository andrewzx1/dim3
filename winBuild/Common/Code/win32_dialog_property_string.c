/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Property String Routines

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

#ifdef D3_EDITOR
	#include "dim3editor.h"
#endif
#ifdef D3_ANIMATOR
	#include "dim3Animator.h"
#endif
#ifdef D3_SETUP
	#include "dim3Setup.h"
#endif

#include "resource.h"
#include "interface.h"
#include "ui_common.h"
#include "win32_dialog.h"

extern HINSTANCE				hinst;
extern HWND						wnd;

int								dialog_property_string_value_type,
								dialog_property_string_value_len,
								dialog_property_string_i_min,
								dialog_property_string_i_max;
void							*dialog_property_string_value;

/* =======================================================

      Property String Event Handlers
      
======================================================= */

LRESULT CALLBACK dialog_property_string_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	char			str[256],desc[256];

	switch (msg) {

		case WM_INITDIALOG:

			property_string_get_values(dialog_property_string_value_type,dialog_property_string_value,dialog_property_string_i_min,dialog_property_string_i_max,str,desc);
			win32_dialog_set_text(diag,IDC_PROPERTY_STRING_STRING,str);
			SetWindowText(diag,desc);

			win32_dialog_set_focus(diag,IDC_PROPERTY_STRING_STRING);
			win32_dialog_select_all(diag,IDC_PROPERTY_STRING_STRING);

			return(FALSE);		// return false when keyboard focus has been set

		case WM_COMMAND:

			switch (LOWORD(wparam)) {

				case ID_PROPERTY_STRING_OK:
					win32_dialog_get_text(diag,IDC_PROPERTY_STRING_STRING,str,256);
					property_string_set_values(dialog_property_string_value_type,dialog_property_string_value,dialog_property_string_value_len,dialog_property_string_i_min,dialog_property_string_i_max,str);
					EndDialog(diag,0);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Property String
      
======================================================= */

void dialog_property_string_run(int value_type,void *value,int value_len,int i_min,int i_max)
{
	dialog_property_string_value_type=value_type;
	dialog_property_string_value=value;
	dialog_property_string_value_len=value_len;
	dialog_property_string_i_min=i_min;
	dialog_property_string_i_max=i_max;

	DialogBox(hinst,MAKEINTRESOURCE(IDD_PROPERTY_STRING),wnd,dialog_property_string_proc);
}
