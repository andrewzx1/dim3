/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Skew Dialog

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

#include "interface.h"
#include "dialog.h"

extern map_type				map;

#define kSkewAxis							FOUR_CHAR_CODE('axis')
#define kSkewDirection						FOUR_CHAR_CODE('sdir')
#define kSkewSize							FOUR_CHAR_CODE('size')

bool						dialog_skew_cancel;
WindowRef					dialog_skew_wind;

/* =======================================================

      Free Rotate Event Handlers
      
======================================================= */

static pascal OSStatus dialog_skew_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_skew_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_skew_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_skew_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Free Rotate Dialog
      
======================================================= */

bool dialog_skew_run(int *axis,int *dir,int *size)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_skew_wind,"Skew");

		// set controls
		
	dialog_set_combo(dialog_skew_wind,kSkewAxis,0,0);
	dialog_set_combo(dialog_skew_wind,kSkewDirection,0,0);
	dialog_set_int(dialog_skew_wind,kSkewSize,0,0);
	dialog_set_focus(dialog_skew_wind,kSkewSize,0);
	
		// show window
	
	ShowWindow(dialog_skew_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(dialog_skew_event_proc);
	InstallWindowEventHandler(dialog_skew_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_skew_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_skew_wind);
	
		// dialog to data
		
	if (!dialog_skew_cancel) {
		*axis=dialog_get_combo(dialog_skew_wind,kSkewAxis,0);
		*dir=dialog_get_combo(dialog_skew_wind,kSkewDirection,0);
		*size=dialog_get_int(dialog_skew_wind,kSkewSize,0);
	}

		// close window
		
	DisposeWindow(dialog_skew_wind);
	
	return(!dialog_skew_cancel);
}

