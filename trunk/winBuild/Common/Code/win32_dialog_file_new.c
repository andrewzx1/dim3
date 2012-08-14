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

#include "glue.h"
#include "resource.h"
#include "interface.h"
#include "win32_dialog.h"

extern HINSTANCE				hinst;
extern HWND						wnd;

char							dialog_new_map_file_name[256];

// controls

/*
IDD_FILE_NEW DIALOGEX 100, 100, 333, 98
STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | DS_CENTER | WS_POPUP | WS_CAPTION
CAPTION "Create New"
FONT 8, "MS Shell Dlg", 400, 0, 0x1
BEGIN
    PUSHBUTTON      "Cancel",IDCANCEL,221,77,50,14
    DEFPUSHBUTTON   "New",ID_FILE_NEW_NEW,276,77,50,14,WS_GROUP
    LTEXT           "Enter a name for your new map.",IDC_STATIC,7,7,204,9
    RTEXT           "Name:",IDC_STATIC,17,25,32,9
    EDITTEXT        IDC_FILE_NEW_NAME,54,23,229,14,ES_AUTOHSCROLL
    LTEXT           "When a map is created, the map data file is saved in the Data/Maps directory and a default course script is saved in the Data/Scripts/Courses directory.",
                    IDC_STATIC,7,44,319,21
END
*/

#define diag_prop_new_map_name		5000
#define diag_prop_new_map_cancel	5001
#define diag_prop_new_map_ok		5002

os_dialog_ctrl_type		diag_property_new_map_ctrls[]={
							{os_dialog_ctrl_type_text_left,diag_prop_new_map_name,"Enter a name for your new map.",10,10,430,20},
							{os_dialog_ctrl_type_text_right,diag_prop_new_map_name,"Name:",10,10,80,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_new_map_name,"",90,10,340,20},
							{os_dialog_ctrl_type_button,diag_prop_new_map_cancel,"Cancel",275,420,80,25},
							{os_dialog_ctrl_type_default_button,diag_prop_new_map_ok,"OK",365,420,80,25},
							{-1,-1,"",0,0,0,0}
						};

/* =======================================================

      File New Event Handlers
      
======================================================= */
/*
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
*/
/* =======================================================

      Run File New
      
======================================================= */
/*
bool dialog_file_new_run(char *title,char *file_name)
{
	strcpy(dialog_file_new_title,title);
	strcpy(dialog_file_new_file_name,file_name);

	if (DialogBox(hinst,MAKEINTRESOURCE(IDD_FILE_NEW),wnd,dialog_file_new_proc)!=0) return(FALSE);

	strcpy(file_name,dialog_file_new_file_name);
	return(TRUE);
}
*/

bool dialog_property_new_map_proc(int msg_type,int id)
{
	char			str[256];

	switch (msg_type) {

		case os_dialog_msg_type_init:
		//	property_string_get_values(dialog_property_string_value_type,dialog_property_string_value,str);
		//	os_dialog_set_text(diag_prop_string_str,str);
		//	os_dialog_set_focus(diag_prop_string_str,TRUE);
			return(TRUE);

		case os_dialog_msg_type_button:
			if (id==diag_prop_new_map_cancel) {
				os_dialog_close(FALSE);
				return(TRUE);
			}
			if (id==diag_prop_new_map_ok) {
			//	os_dialog_get_text(diag_prop_string_str,str,256);
			//	property_string_set_values(dialog_property_string_value_type,dialog_property_string_value,dialog_property_string_value_len,dialog_property_string_i_min,dialog_property_string_i_max,str);
				os_dialog_close(TRUE);
				return(TRUE);
			}
			break;
	}

	return(FALSE);
}

bool dialog_file_new_run(char *title,char *file_name)
{
	bool				ok;

	strcpy(dialog_new_map_file_name,file_name);
	ok=os_dialog_run(title,450,450,diag_property_new_map_ctrls,dialog_property_new_map_proc);

	return(ok);
}
