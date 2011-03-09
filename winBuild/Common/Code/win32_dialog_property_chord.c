/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Property Chord Routines

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

int								dialog_property_chord_value_type;
void							*dialog_property_chord_value;

/* =======================================================

      Property Chord Event Handlers
      
======================================================= */

LRESULT CALLBACK dialog_property_chord_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	bool			three_value;
	char			str_x[256],str_y[256],str_z[256],desc[256];

	switch (msg) {

		case WM_INITDIALOG:

			three_value=property_chord_get_values(dialog_property_chord_value_type,dialog_property_chord_value,str_x,str_y,str_z,desc);
			win32_dialog_set_text(diag,IDC_PROPERTY_CHORD_X,str_x);
			win32_dialog_set_text(diag,IDC_PROPERTY_CHORD_Y,str_y);
			win32_dialog_set_text(diag,IDC_PROPERTY_CHORD_Z,str_z);

			SetWindowText(diag,desc);

			win32_dialog_set_focus(diag,IDC_PROPERTY_CHORD_X);
			win32_dialog_select_all(diag,IDC_PROPERTY_CHORD_X);

			win32_dialog_enable(diag,IDC_PROPERTY_CHORD_Z,three_value);

			return(FALSE);		// return false when keyboard focus has been set

		case WM_COMMAND:

			switch (LOWORD(wparam)) {

				case ID_PROPERTY_CHORD_OK:
					win32_dialog_get_text(diag,IDC_PROPERTY_CHORD_X,str_x,256);
					win32_dialog_get_text(diag,IDC_PROPERTY_CHORD_Y,str_y,256);
					win32_dialog_get_text(diag,IDC_PROPERTY_CHORD_Z,str_z,256);
					property_chord_set_values(dialog_property_chord_value_type,dialog_property_chord_value,str_x,str_y,str_z);
					EndDialog(diag,0);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Property Chord
      
======================================================= */

void dialog_property_chord_run(int value_type,void *value)
{
	dialog_property_chord_value_type=value_type;
	dialog_property_chord_value=value;

	DialogBox(hinst,MAKEINTRESOURCE(IDD_PROPERTY_CHORD),wnd,dialog_property_chord_proc);
}
