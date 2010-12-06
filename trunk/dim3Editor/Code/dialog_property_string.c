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

#include "dialog.h"

#define kPropertyString				FOUR_CHAR_CODE('vstr')
#define kPropertyDescription		FOUR_CHAR_CODE('vdsc')

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

void dialog_property_string_run(int value_type,void *value,int value_len)
{
	EventHandlerUPP					event_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_property_string_wind,"PropertyString");
	
	switch (value_type) {
	
		case list_string_value_string:
			dialog_set_text(dialog_property_string_wind,kPropertyString,0,(char*)value);
			dialog_set_text(dialog_property_string_wind,kPropertyDescription,0,"Enter a string.");
			break;
			
		case list_string_value_int:
			dialog_set_int(dialog_property_string_wind,kPropertyString,0,*((int*)value));
			dialog_set_text(dialog_property_string_wind,kPropertyDescription,0,"Enter a integer.");
			break;
			
		case list_string_value_positive_int:
			dialog_set_int(dialog_property_string_wind,kPropertyString,0,*((int*)value));
			dialog_set_text(dialog_property_string_wind,kPropertyDescription,0,"Enter a positive integer.");
			break;
			
		case list_string_value_float:
			dialog_set_float(dialog_property_string_wind,kPropertyString,0,*((float*)value));
			dialog_set_text(dialog_property_string_wind,kPropertyDescription,0,"Enter a float.");
			break;

		case list_string_value_positive_float:
			dialog_set_float(dialog_property_string_wind,kPropertyString,0,*((float*)value));
			dialog_set_text(dialog_property_string_wind,kPropertyDescription,0,"Enter a positive float.");
			break;

		case list_string_value_0_to_1_float:
			dialog_set_float(dialog_property_string_wind,kPropertyString,0,*((float*)value));
			dialog_set_text(dialog_property_string_wind,kPropertyDescription,0,"Enter a float between 0.0 and 1.0.");
			break;
			
	}
	
	dialog_set_focus(dialog_property_string_wind,kPropertyString,0);
		
		// show window
	
	ShowWindow(dialog_property_string_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(property_string_event_proc);
	InstallWindowEventHandler(dialog_property_string_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	RunAppModalLoopForWindow(dialog_property_string_wind);
	
		// get the value
		
	switch (value_type) {
	
		case list_string_value_string:
			dialog_get_text(dialog_property_string_wind,kPropertyString,0,(char*)value,value_len);
			break;
			
		case list_string_value_int:
			*((int*)value)=dialog_get_int(dialog_property_string_wind,kPropertyString,0);
			break;
			
		case list_string_value_positive_int:
			*((int*)value)=abs(dialog_get_int(dialog_property_string_wind,kPropertyString,0));
			break;
		
		case list_string_value_float:
			*((float*)value)=dialog_get_float(dialog_property_string_wind,kPropertyString,0);
			break;
			
		case list_string_value_positive_float:
			*((float*)value)=fabs(dialog_get_float(dialog_property_string_wind,kPropertyString,0));
			break;
			
		case list_string_value_0_to_1_float:
			*((float*)value)=dialog_get_float(dialog_property_string_wind,kPropertyString,0);
			if ((*((float*)value))<0.0f) *((float*)value)=0.0f;
			if ((*((float*)value))>1.0f) *((float*)value)=1.0f;
			break;
			
	}
	
		// close window
		
	DisposeWindow(dialog_property_string_wind);
}

