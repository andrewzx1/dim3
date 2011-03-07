/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Chord Properties

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

#define kPropertyX				FOUR_CHAR_CODE('xstr')
#define kPropertyY				FOUR_CHAR_CODE('ystr')
#define kPropertyZ				FOUR_CHAR_CODE('zstr')

WindowRef						dialog_property_chord_wind;

/* =======================================================

      Property Chord Event Handlers
      
======================================================= */

static pascal OSStatus property_chord_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_property_chord_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Property Chord
      
======================================================= */

void dialog_property_chord_run(int value_type,void *value)
{
	char					str_x[256],str_y[256],str_z[256],desc[256];
	bool					three_value;
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_property_chord_wind,"PropertyChord");
	
	three_value=property_chord_get_values(value_type,value,str_x,str_y,str_z,desc);
	dialog_set_text(dialog_property_chord_wind,kPropertyX,0,str_x);
	dialog_set_text(dialog_property_chord_wind,kPropertyY,0,str_y);
	dialog_set_text(dialog_property_chord_wind,kPropertyZ,0,str_z);
	
	dialog_set_title(dialog_property_chord_wind,desc);
	
	dialog_enable(dialog_property_chord_wind,kPropertyZ,0,three_value);
	dialog_set_focus(dialog_property_chord_wind,kPropertyX,0);
		
		// show window
	
	ShowWindow(dialog_property_chord_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(property_chord_event_proc);
	InstallWindowEventHandler(dialog_property_chord_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	RunAppModalLoopForWindow(dialog_property_chord_wind);
	
		// get the value
		
	dialog_get_text(dialog_property_chord_wind,kPropertyX,0,str_x,256);
	dialog_get_text(dialog_property_chord_wind,kPropertyY,0,str_y,256);
	dialog_get_text(dialog_property_chord_wind,kPropertyZ,0,str_z,256);
	property_chord_set_values(value_type,value,str_x,str_y,str_z);
	
		// close window
		
	DisposeWindow(dialog_property_chord_wind);
}

