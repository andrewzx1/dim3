/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Resize Texture Dialog

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

float							dialog_resize_texture_fct_u,dialog_resize_texture_fct_v;

/* =======================================================

      Resize Texture Event Handlers
      
======================================================= */

void dialog_resize_texture_set(HWND diag)
{
	win32_dialog_set_float(diag,IDC_RESIZE_TEXTURE_U,100.0f);
	win32_dialog_set_float(diag,IDC_RESIZE_TEXTURE_V,100.0f);
}

void dialog_resize_texture_get(HWND diag)
{
	dialog_resize_texture_fct_u=win32_dialog_get_float(diag,IDC_RESIZE_TEXTURE_U);
	dialog_resize_texture_fct_v=win32_dialog_get_float(diag,IDC_RESIZE_TEXTURE_V);
}

LRESULT CALLBACK dialog_resize_texture_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch (msg) {

		case WM_INITDIALOG:
			dialog_resize_texture_set(diag);
			return(TRUE);

		case WM_COMMAND:
			
			switch (LOWORD(wparam)) {

				case ID_RESIZE_TEXTURE_OK:
					dialog_resize_texture_get(diag);
					EndDialog(diag,0);
					return(TRUE);

				case ID_RESIZE_TEXTURE_CANCEL:
					EndDialog(diag,-1);
					return(TRUE);

			}

			break;

	}

	return(FALSE);
}

/* =======================================================

      Run Resize Texture Dialog
      
======================================================= */

bool dialog_resize_texture_run(float *fct_u,float *fct_v)
{
	if (DialogBox(hinst,MAKEINTRESOURCE(IDD_RESIZE_TEXTURE),wnd,dialog_resize_texture_proc)!=0) return(FALSE);

	*fct_u=dialog_resize_texture_fct_u;
	*fct_v=dialog_resize_texture_fct_v;

	return(TRUE);
}

