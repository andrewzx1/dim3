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

#include "glue.h"
#include "interface.h"

extern map_type				map;

#define kOBJImportType							FOUR_CHAR_CODE('impt')
#define kOBJImportAxis							FOUR_CHAR_CODE('axis')
#define kOBJImportScale							FOUR_CHAR_CODE('scle')

bool						dialog_obj_import_cancel;
WindowRef					dialog_obj_import_wind;

/* =======================================================

      OBJ Import Event Handlers
      
======================================================= */

static pascal OSStatus dialog_obj_import_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_obj_import_wind);
					return(noErr);
					
				case kHICommandCancel:
					dialog_obj_import_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_obj_import_wind);
					return(noErr);					
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run OBJ Import Dialog
      
======================================================= */

int dialog_obj_import_run(int *scale_axis,int *scale_unit)
{
	int						import_mode;
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_obj_import_wind,"OBJImport");

		// set controls
		
	dialog_set_combo(dialog_obj_import_wind,kOBJImportType,0,0);

	dialog_set_combo(dialog_obj_import_wind,kOBJImportAxis,0,0);
	dialog_set_int(dialog_obj_import_wind,kOBJImportScale,0,5000);
	dialog_set_focus(dialog_obj_import_wind,kOBJImportScale,0);
	
		// show window
	
	ShowWindow(dialog_obj_import_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(dialog_obj_import_event_proc);
	InstallWindowEventHandler(dialog_obj_import_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_obj_import_cancel=FALSE;
	
	RunAppModalLoopForWindow(dialog_obj_import_wind);
	
		// dialog to data
		
	import_mode=dialog_get_combo(dialog_obj_import_wind,kOBJImportType,0);
	
	*scale_axis=dialog_get_combo(dialog_obj_import_wind,kOBJImportAxis,0);
	*scale_unit=dialog_get_int(dialog_obj_import_wind,kOBJImportScale,0);
	
		// close window
		
	DisposeWindow(dialog_obj_import_wind);
	
	if (dialog_obj_import_cancel) return(-1);
	
	return(import_mode);
}

