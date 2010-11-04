/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Mesh Scale Dialog

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

extern map_type					map;

bool							dialog_mesh_scale_replace;

/* =======================================================

      Mesh Scale Event Handlers
      
======================================================= */

LRESULT CALLBACK dialog_mesh_scale_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
//	LPNMHDR			hdr;

	switch (msg) {

		case WM_INITDIALOG:
		//	SetWindowText(diag,fp_dialog_name);
		//	dialog_file_open_set(diag);
			return(TRUE);

		case WM_COMMAND:
			
			switch (wparam) {

				case ID_MESH_SCALE_SCALE:
					dialog_mesh_scale_replace=FALSE;
					EndDialog(diag,0);
					return(TRUE);

				case ID_MESH_SCALE_REPLACE:
					dialog_mesh_scale_replace=TRUE;
					EndDialog(diag,0);
					return(TRUE);

			}

			break;

		case WM_NOTIFY:

		//	hdr=(LPNMHDR)lparam;

		//	if ((hdr->idFrom==IDC_FILE_OPEN_TREE) && (hdr->code==NM_DBLCLK)) {
		//		if (!dialog_file_open_get(diag)) return(FALSE);
		//		EndDialog(diag,0);
		//	}

			break;

	}

	return(FALSE);
}



/* =======================================================

      Run Mesh Scale Dialog
      
======================================================= */

bool dialog_mesh_scale_run(d3fpnt *min,d3fpnt *max,bool replace_ok,float old_scale,float *scale)
{
	*scale=1000.0f;
	return(FALSE);

	/*
	bool			ok;

		// run dialog

	dialog_mesh_scale_replace=FALSE;

	ok=(DialogBox(hinst,MAKEINTRESOURCE(IDD_MESH_SCALE),NULL,dialog_mesh_scale_proc)==0);
	
	if (!ok) return(FALSE);




	return(dialog_mesh_scale_replace);
*/
/*

	int						sz;
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_mesh_scale_wind,"MeshScale");

		// set controls
		
	dialog_set_combo(dialog_mesh_scale_wind,kMeshScaleAxis,0,0);
	dialog_set_int(dialog_mesh_scale_wind,kMeshScaleScale,0,(20*map_enlarge));
	dialog_set_focus(dialog_mesh_scale_wind,kMeshScaleScale,0);
	
	dialog_enable(dialog_mesh_scale_wind,kMeshScaleButtonReplace,0,replace_ok);
	
		// show window
	
	ShowWindow(dialog_mesh_scale_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(dialog_mesh_scale_event_proc);
	InstallWindowEventHandler(dialog_mesh_scale_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_mesh_scale_replace=FALSE;
	
	RunAppModalLoopForWindow(dialog_mesh_scale_wind);
	
		// dialog to data
		
	if (dialog_mesh_scale_replace) {
		*scale=old_scale;
		if (*scale==0.0f) *scale=1.0f;
	}
	else {
		*scale=1.0f;
		
		sz=dialog_get_int(dialog_mesh_scale_wind,kMeshScaleScale,0);
		
		switch (dialog_get_combo(dialog_mesh_scale_wind,kMeshScaleAxis,0)) {
			case 0:
				*scale=((float)sz)/(max->x-min->x);
				break;
			case 1:
				*scale=((float)sz)/(max->y-min->y);
				break;
			case 2:
				*scale=((float)sz)/(max->z-min->z);
				break;
		}
		
	}
	
		// close window
		
	DisposeWindow(dialog_mesh_scale_wind);
	*/
}

