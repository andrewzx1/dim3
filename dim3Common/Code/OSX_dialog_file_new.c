/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: File New Dialog

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
#include "osx_dialog.h"

#define kFileNewText			FOUR_CHAR_CODE('name')

bool							dialog_file_new_cancel;
WindowRef						dialog_file_new_wind;

/* =======================================================

      File New Event Handlers
      
======================================================= */

static pascal OSStatus file_new_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandNew:
					QuitAppModalLoopForWindow(dialog_file_new_wind);
					return(noErr);
					
				case kHICommandCancel:
					dialog_file_new_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_file_new_wind);
					return(noErr);
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run File New
      
======================================================= */
/*
bool dialog_file_new_run(char *title,char *file_name)
{
	EventHandlerUPP					event_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_file_new_wind,"FileNew");
	dialog_set_title(dialog_file_new_wind,title);
	
	dialog_set_text(dialog_file_new_wind,kFileNewText,0,file_name);
	dialog_set_focus(dialog_file_new_wind,kFileNewText,0);
	
		// show window
	
	ShowWindow(dialog_file_new_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(file_new_event_proc);
	InstallWindowEventHandler(dialog_file_new_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_file_new_cancel=FALSE;
	
	RunAppModalLoopForWindow(dialog_file_new_wind);
	
	if (!dialog_file_new_cancel) {
		dialog_get_text(dialog_file_new_wind,kFileNewText,0,file_name,256);
	}
	
		// close window

	DisposeWindow(dialog_file_new_wind);
	
	return(!dialog_file_new_cancel);
}
*/
