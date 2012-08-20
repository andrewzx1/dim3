/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Resize Dialog

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

float							dialog_resize_fct_x,dialog_resize_fct_y,dialog_resize_fct_z;

/*
IDD_RESIZE DIALOGEX 0, 0, 200, 84
STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | DS_CENTER | WS_POPUP | WS_CAPTION
CAPTION "Resize"
FONT 8, "MS Shell Dlg", 400, 0, 0x1
BEGIN
    EDITTEXT        IDC_RESIZE_X,33,7,62,12,ES_AUTOHSCROLL
    EDITTEXT        IDC_RESIZE_Y,33,23,62,12,ES_AUTOHSCROLL
    EDITTEXT        IDC_RESIZE_Z,33,39,62,12,ES_AUTOHSCROLL
    PUSHBUTTON      "Cancel",ID_RESIZE_CANCEL,89,63,50,14
    DEFPUSHBUTTON   "OK",ID_RESIZE_OK,143,63,50,14
    LTEXT           "X:",IDC_STATIC,15,8,13,8,0,WS_EX_RIGHT
    LTEXT           "Y:",IDC_STATIC,14,24,14,8,0,WS_EX_RIGHT
    LTEXT           "Z:",IDC_STATIC,11,40,17,8,0,WS_EX_RIGHT
    LTEXT           "%",IDC_STATIC,102,10,13,8
    LTEXT           "%",IDC_STATIC,102,25,13,8
    LTEXT           "%",IDC_STATIC,102,40,13,8
END
*/

// controls

#define diag_prop_pick_scale_type	5000
#define diag_prop_pick_scale_x		5001
#define diag_prop_pick_scale_y		5002
#define diag_prop_pick_scale_z		5003
#define diag_prop_pick_scale_cancel	5004
#define diag_prop_pick_scale_ok		5005

os_dialog_ctrl_type		diag_property_pick_scale_ctrls[]={
							{os_dialog_ctrl_type_text_right,0,"X:",35,38,35,20},
							{os_dialog_ctrl_type_text_right,0,"Y:",35,63,35,20},
							{os_dialog_ctrl_type_text_right,0,"Z:",35,88,35,20},
							{os_dialog_ctrl_type_text_right,0,"%",35,38,35,20},
							{os_dialog_ctrl_type_text_right,0,"%",35,63,35,20},
							{os_dialog_ctrl_type_text_right,0,"Z:",35,88,35,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_pick_scale_x,"",75,35,100,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_pick_scale_y,"",75,60,100,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_pick_scale_z,"",75,85,100,20},
							{os_dialog_ctrl_type_button,diag_prop_pick_scale_cancel,"Cancel",180,110,80,25},
							{os_dialog_ctrl_type_default_button,diag_prop_pick_scale_ok,"OK",270,110,80,25},
							{-1,-1,"",0,0,0,0}
						};

/* =======================================================

      Resize Event Handlers
      
======================================================= */

void dialog_resize_set(HWND diag)
{
	win32_dialog_set_float(diag,IDC_RESIZE_X,100.0f);
	win32_dialog_set_float(diag,IDC_RESIZE_Y,100.0f);
	win32_dialog_set_float(diag,IDC_RESIZE_Z,100.0f);
}

void dialog_resize_get(HWND diag)
{
	dialog_resize_fct_x=win32_dialog_get_float(diag,IDC_RESIZE_X);
	dialog_resize_fct_y=win32_dialog_get_float(diag,IDC_RESIZE_Y);
	dialog_resize_fct_z=win32_dialog_get_float(diag,IDC_RESIZE_Z);
}

LRESULT CALLBACK dialog_resize_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch (msg) {

		case WM_INITDIALOG:
			dialog_resize_set(diag);
			return(TRUE);

		case WM_COMMAND:
			
			switch (LOWORD(wparam)) {

				case ID_RESIZE_OK:
					dialog_resize_get(diag);
					EndDialog(diag,0);
					return(TRUE);

				case ID_RESIZE_CANCEL:
					EndDialog(diag,-1);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Resize Dialog
      
======================================================= */

bool dialog_resize_run(float *fct_x,float *fct_y,float *fct_z)
{
	if (DialogBox(hinst,MAKEINTRESOURCE(IDD_RESIZE),wnd,dialog_resize_proc)!=0) return(FALSE);

	*fct_x=dialog_resize_fct_x;
	*fct_y=dialog_resize_fct_y;
	*fct_z=dialog_resize_fct_z;

	return(TRUE);
}

