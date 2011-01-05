/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: win32 Dialog Utilities

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
#else
	#include "dim3Animator.h"
#endif

#include "resource.h"

/* =======================================================

      Dialog Utilities
      
======================================================= */

void win32_dialog_set_text(HWND diag,int id,char *value)
{
	SendDlgItemMessage(diag,id,WM_SETTEXT,0,(LPARAM)value);
}

void win32_dialog_get_text(HWND diag,int id,char *value,int value_len)
{
	char			str[256];

	SendDlgItemMessage(diag,id,WM_GETTEXT,256,(LPARAM)str);

	strncpy(value,str,value_len);
	value[value_len-1]=0x0;
}

void win32_dialog_set_int(HWND diag,int id,int value)
{
	char			str[32];

	sprintf(str,"%d",value);
	SendDlgItemMessage(diag,id,WM_SETTEXT,0,(LPARAM)str);
}

int win32_dialog_get_int(HWND diag,int id)
{
	char			str[32];

	SendDlgItemMessage(diag,id,WM_GETTEXT,32,(LPARAM)str);
	return(atoi(str));
}

void win32_dialog_set_float(HWND diag,int id,float value)
{
	char			str[32];

	sprintf(str,"%.2f",value);
	SendDlgItemMessage(diag,id,WM_SETTEXT,0,(LPARAM)str);
}

float win32_dialog_get_float(HWND diag,int id)
{
	char			str[32];

	SendDlgItemMessage(diag,id,WM_GETTEXT,32,(LPARAM)str);
	return((float)atof(str));
}

void win32_dialog_combo_add(HWND diag,int id,char *str)
{
	SendDlgItemMessage(diag,id,CB_ADDSTRING,0,(LPARAM)str);
}

void win32_dialog_combo_set_value(HWND diag,int id,int value)
{
	SendDlgItemMessage(diag,id,CB_SETCURSEL,value,0);
}

int win32_dialog_combo_get_value(HWND diag,int id)
{
	return(SendDlgItemMessage(diag,id,CB_GETCURSEL,0,0));
}

void win32_dialog_list_add(HWND diag,int id,char *str)
{
	SendDlgItemMessage(diag,id,LB_ADDSTRING,0,(LPARAM)str);
}

void win32_dialog_list_set_value(HWND diag,int id,int value)
{
	SendDlgItemMessage(diag,id,LB_SETCURSEL,value,0);
}

int win32_dialog_list_get_value(HWND diag,int id)
{
	return(SendDlgItemMessage(diag,id,LB_GETCURSEL,0,0));
}

void win32_dialog_enable(HWND diag,int id,bool enable)
{
	EnableWindow(GetDlgItem(diag,id),enable);
}

void win32_dialog_set_focus(HWND diag,int id)
{
	SetFocus(GetDlgItem(diag,id));
}

int win32_dialog_select_all(HWND diag,int id)
{
	return(SendDlgItemMessage(diag,id,EM_SETSEL,0,-1));
}


