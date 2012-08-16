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

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "ui_common.h"

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

      Run New Map
      
======================================================= */

bool dialog_property_new_map_proc(int msg_type,int id)
{
	switch (msg_type) {

		case os_dialog_msg_type_init:
			os_dialog_set_text(diag_prop_new_map_name,"New Map");
			os_dialog_set_focus(diag_prop_new_map_name,TRUE);
			return(TRUE);

		case os_dialog_msg_type_button:
			if (id==diag_prop_new_map_cancel) {
				os_dialog_close(FALSE);
				return(TRUE);
			}
			if (id==diag_prop_new_map_ok) {
				os_dialog_get_text(diag_prop_new_map_name,dialog_new_map_file_name,256);
				os_dialog_close(TRUE);
				return(TRUE);
			}
			break;
	}

	return(FALSE);
}

bool dialog_new_map_run(char *file_name)
{
	bool				ok;

	ok=os_dialog_run("Create New Map",450,450,diag_property_new_map_ctrls,dialog_property_new_map_proc);
	if (ok) strcpy(file_name,dialog_new_map_file_name);

	return(ok);
}
