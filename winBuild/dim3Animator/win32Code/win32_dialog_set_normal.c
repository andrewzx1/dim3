/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Dialog Set Normal

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

#include "glue.h"
#include "resource.h"
#include "interface.h"
#include "ui_common.h"
#include "win32_dialog.h"

extern HINSTANCE				hinst;
extern HWND						wnd;

d3vct							dialog_set_normal_vct;

// controls

#define diag_prop_set_normal_x		0
#define diag_prop_set_normal_y		1
#define diag_prop_set_normal_z		2
#define diag_prop_set_normal_ok		3
#define diag_prop_set_normal_cancel	4

os_dialog_ctrl_type		diag_property_set_normal_ctrls[]={
							{os_dialog_ctrl_type_text_edit,diag_prop_set_normal_x,"",5,10,145,20,TRUE,TRUE},
							{os_dialog_ctrl_type_text_edit,diag_prop_set_normal_y,"",155,10,145,20,FALSE,FALSE},
							{os_dialog_ctrl_type_text_edit,diag_prop_set_normal_z,"",305,10,145,20,FALSE,FALSE},
							{os_dialog_ctrl_type_default_button,diag_prop_chord_ok,"OK",370,40,80,25,FALSE,FALSE},
							{os_dialog_ctrl_type_default_button,diag_prop_chord_cancel,"Cancel",280,40,80,25,FALSE,FALSE},
							{-1,-1,"",0,0,0,0,FALSE,FALSE}
						};

/* =======================================================

      Set Normal Event Handlers
      
======================================================= */

LRESULT CALLBACK dialog_set_normal_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch (msg) {

		case WM_INITDIALOG:
			win32_dialog_set_float(diag,IDC_SET_NORMAL_X,dialog_set_normal_vct.x);
			win32_dialog_set_float(diag,IDC_SET_NORMAL_Y,dialog_set_normal_vct.y);
			win32_dialog_set_float(diag,IDC_SET_NORMAL_Z,dialog_set_normal_vct.z);

			win32_dialog_set_focus(diag,IDC_SET_NORMAL_X);
			return(FALSE);		// return false when keyboard focus has been set

		case WM_COMMAND:

			switch (LOWORD(wparam)) {

				case ID_SET_NORMAL_OK:

					dialog_set_normal_vct.x=win32_dialog_get_float(diag,IDC_SET_NORMAL_X);
					dialog_set_normal_vct.y=win32_dialog_get_float(diag,IDC_SET_NORMAL_Y);
					dialog_set_normal_vct.z=win32_dialog_get_float(diag,IDC_SET_NORMAL_Z);
					vector_normalize(&dialog_set_normal_vct);

					EndDialog(diag,0);
					return(TRUE);

				case ID_SET_NORMAL_CANCEL:
					EndDialog(diag,-1);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Set Normal
      
======================================================= */

bool dialog_set_normal_run(d3vct *vct)
{
	memmove(&dialog_set_normal_vct,vct,sizeof(d3vct));

		// run dialog

//	os_dialog_create(desc,455,200,diag_property_set_normal_ctrls,dialog_property_set_normal_proc);

	memmove(vct,&dialog_set_normal_vct,sizeof(d3vct));

	return(TRUE);
}
