/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: File Open Routines

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

extern HINSTANCE				hinst;
extern HWND						wnd;

int								dialog_property_string_value_type,
								dialog_property_string_value_len;
void							*dialog_property_string_value;

extern void win32_dialog_set_text(HWND diag,int id,char *value);
extern void win32_dialog_get_text(HWND diag,int id,char *value,int value_len);
extern void win32_dialog_set_int(HWND diag,int id,int value);
extern int win32_dialog_get_int(HWND diag,int id);
extern void win32_dialog_set_float(HWND diag,int id,float value);
extern float win32_dialog_get_float(HWND diag,int id);
extern void win32_dialog_set_focus(HWND diag,int id);

/* =======================================================

      Property String Event Handlers
      
======================================================= */

LRESULT CALLBACK dialog_property_string_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch (msg) {

		case WM_INITDIALOG:
			switch (dialog_property_string_value_type) {

				case list_string_value_string:
					win32_dialog_set_text(diag,IDC_PROPERTY_STRING_STRING,(char*)dialog_property_string_value);
					win32_dialog_set_text(diag,IDC_PROPERTY_STRING_DESCRIPTION,"Enter a string.");
					break;
					
				case list_string_value_int:
					win32_dialog_set_int(diag,IDC_PROPERTY_STRING_STRING,*((int*)dialog_property_string_value));
					win32_dialog_set_text(diag,IDC_PROPERTY_STRING_DESCRIPTION,"Enter a integer.");
					break;
					
				case list_string_value_positive_int:
					win32_dialog_set_int(diag,IDC_PROPERTY_STRING_STRING,*((int*)dialog_property_string_value));
					win32_dialog_set_text(diag,IDC_PROPERTY_STRING_DESCRIPTION,"Enter a positive integer.");
					break;
					
				case list_string_value_float:
					win32_dialog_set_float(diag,IDC_PROPERTY_STRING_STRING,*((float*)dialog_property_string_value));
					win32_dialog_set_text(diag,IDC_PROPERTY_STRING_DESCRIPTION,"Enter a float.");
					break;

				case list_string_value_positive_float:
					win32_dialog_set_float(diag,IDC_PROPERTY_STRING_STRING,*((float*)dialog_property_string_value));
					win32_dialog_set_text(diag,IDC_PROPERTY_STRING_DESCRIPTION,"Enter a positive float.");
					break;

				case list_string_value_0_to_1_float:
					win32_dialog_set_float(diag,IDC_PROPERTY_STRING_STRING,*((float*)dialog_property_string_value));
					win32_dialog_set_text(diag,IDC_PROPERTY_STRING_DESCRIPTION,"Enter a float between 0.0 and 1.0.");
					break;
					
			}
	
			win32_dialog_set_focus(diag,IDC_PROPERTY_STRING_STRING);
			return(FALSE);		// return false when keyboard focus has been set

		case WM_COMMAND:

			switch (LOWORD(wparam)) {

				case ID_PROPERTY_STRING_OK:

					switch (dialog_property_string_value_type) {
					
						case list_string_value_string:
							win32_dialog_get_text(diag,IDC_PROPERTY_STRING_STRING,(char*)dialog_property_string_value,dialog_property_string_value_len);
							break;
							
						case list_string_value_int:
							*((int*)dialog_property_string_value)=win32_dialog_get_int(diag,IDC_PROPERTY_STRING_STRING);
							break;
							
						case list_string_value_positive_int:
							*((int*)dialog_property_string_value)=abs(win32_dialog_get_int(diag,IDC_PROPERTY_STRING_STRING));
							break;
						
						case list_string_value_float:
							*((float*)dialog_property_string_value)=win32_dialog_get_float(diag,IDC_PROPERTY_STRING_STRING);
							break;
							
						case list_string_value_positive_float:
							*((float*)dialog_property_string_value)=(float)fabs(win32_dialog_get_float(diag,IDC_PROPERTY_STRING_STRING));
							break;
							
						case list_string_value_0_to_1_float:
							*((float*)dialog_property_string_value)=win32_dialog_get_float(diag,IDC_PROPERTY_STRING_STRING);
							if ((*((float*)dialog_property_string_value))<0.0f) *((float*)dialog_property_string_value)=0.0f;
							if ((*((float*)dialog_property_string_value))>1.0f) *((float*)dialog_property_string_value)=1.0f;
							break;
							
					}

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

void dialog_property_string_run(int value_type,void *value,int value_len)
{
	dialog_property_string_value_type=value_type;
	dialog_property_string_value=value;
	dialog_property_string_value_len=value_len;

	DialogBox(hinst,MAKEINTRESOURCE(IDD_PROPERTY_STRING),wnd,dialog_property_string_proc);
}

