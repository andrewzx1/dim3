/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: String Properties

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
#include "ui_common.h"
#include "osx_dialog.h"

#define kPropertyString				FOUR_CHAR_CODE('vstr')

WindowRef						dialog_property_string_wind;

/* =======================================================

      Property String Event Handlers
      
======================================================= */

static pascal OSStatus property_string_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_property_string_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Property String
      
======================================================= */

void dialog_property_string_run(int value_type,void *value,int value_len,int i_min,int i_max)
{
	char					str[256],desc[256];
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_property_string_wind,"PropertyString");
	
	property_string_get_values(value_type,value,i_min,i_max,str,desc);
	dialog_set_text(dialog_property_string_wind,kPropertyString,0,str);
	dialog_set_title(dialog_property_string_wind,desc);
	
	dialog_set_focus(dialog_property_string_wind,kPropertyString,0);
		
		// show window
	
	ShowWindow(dialog_property_string_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(property_string_event_proc);
	InstallWindowEventHandler(dialog_property_string_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	RunAppModalLoopForWindow(dialog_property_string_wind);
	
		// get the value
		
	dialog_get_text(dialog_property_string_wind,kPropertyString,0,str,256);
	property_string_set_values(value_type,value,value_len,i_min,i_max,str);
	
		// close window
		
	DisposeWindow(dialog_property_string_wind);
}

