/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Normal Dialog

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
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

#include "dialog.h"

#define kXValue						FOUR_CHAR_CODE('xxxx')
#define kZValue						FOUR_CHAR_CODE('zzzz')
#define kYValue						FOUR_CHAR_CODE('yyyy')

bool					dialog_set_normal_cancel;
WindowRef				dialog_set_normal_wind;

/* =======================================================

      Nudge/Rotate Event Handlers
      
======================================================= */

static pascal OSStatus set_normal_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_set_normal_wind);
					return(noErr);
					
				case kHICommandCancel:
					dialog_set_normal_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_set_normal_wind);
					return(noErr);
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Nudge/Rotate Dialog
      
======================================================= */

bool dialog_set_normal_run(d3vct *normal)
{
	EventHandlerUPP					event_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_set_normal_wind,"NormalPick");
	
		// setup the controls
		
	dialog_set_float(dialog_set_normal_wind,kXValue,0,normal->x);
	dialog_set_float(dialog_set_normal_wind,kYValue,0,normal->y);
	dialog_set_float(dialog_set_normal_wind,kZValue,0,normal->z);
	
	dialog_set_focus(dialog_set_normal_wind,kXValue,0);
	
		// show window
	
	ShowWindow(dialog_set_normal_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(set_normal_event_proc);
	InstallWindowEventHandler(dialog_set_normal_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_set_normal_cancel=FALSE;
	
	RunAppModalLoopForWindow(dialog_set_normal_wind);
	
	if (!dialog_set_normal_cancel) {
		normal->x=dialog_get_int(dialog_set_normal_wind,kXValue,0);
		normal->y=dialog_get_int(dialog_set_normal_wind,kYValue,0);
		normal->z=dialog_get_int(dialog_set_normal_wind,kZValue,0);
	}
	
		// close window

	DisposeWindow(dialog_set_normal_wind);
	
	return(!dialog_set_normal_cancel);
}

