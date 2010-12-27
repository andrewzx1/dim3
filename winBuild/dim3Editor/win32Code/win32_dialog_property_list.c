/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property List Dialog

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

int								dialog_property_list_count,
								dialog_property_list_item_sz,dialog_property_list_name_offset,
								dialog_property_idx;
char							*dialog_property_list_ptr;
bool							dialog_property_include_none;

/* =======================================================

      Property List Event Handlers
      
======================================================= */

LRESULT CALLBACK dialog_property_list_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	int				n;
	char			*name;

	switch (msg) {

		case WM_INITDIALOG:
			win32_dialog_enable(diag,ID_PROPERTY_LIST_NONE,dialog_property_include_none);
			for (n=0;n!=dialog_property_list_count;n++) {
				name=(dialog_property_list_ptr+(n*dialog_property_list_item_sz))+dialog_property_list_name_offset;
				win32_dialog_list_add(diag,IDC_PROPERTY_LIST_LIST,name);
			}
			if (dialog_property_idx!=-1) win32_dialog_list_set_value(diag,IDC_PROPERTY_LIST_LIST,dialog_property_idx);
			return(TRUE);

		case WM_COMMAND:

			switch (LOWORD(wparam)) {

				case ID_PROPERTY_LIST_NONE:
					dialog_property_idx=-1;
					EndDialog(diag,0);
					return(TRUE);

				case ID_PROPERTY_LIST_OK:
					EndDialog(diag,0);
					return(TRUE);

				case IDC_PROPERTY_LIST_LIST:
					if ((HIWORD(wparam)==LBN_SELCHANGE) || (HIWORD(wparam)==LBN_DBLCLK)) dialog_property_idx=win32_dialog_list_get_value(diag,IDC_PROPERTY_LIST_LIST);
					if (HIWORD(wparam)==LBN_DBLCLK) EndDialog(diag,0);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Property List
      
======================================================= */

void dialog_property_list_run(char *list_ptr,int list_count,int list_item_sz,int list_name_offset,bool include_none,int *idx)
{
		// remember the list
		
	dialog_property_list_ptr=list_ptr;
	dialog_property_list_count=list_count;
	dialog_property_list_item_sz=list_item_sz;
	dialog_property_list_name_offset=list_name_offset;
	dialog_property_include_none=include_none;
	
		// open the dialog

	dialog_property_idx=*idx;

	DialogBox(hinst,MAKEINTRESOURCE(IDD_PROPERTY_LIST),wnd,dialog_property_list_proc);
	
	*idx=dialog_property_idx;
}

