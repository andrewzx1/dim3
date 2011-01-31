/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: SVG Import

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

#define kSVGImportScale						FOUR_CHAR_CODE('scle')
#define kSVGImportHigh						FOUR_CHAR_CODE('high')

bool						dialog_svg_import_cancel;
WindowRef					dialog_svg_import_wind;

/* =======================================================

      SVG Import Event Handlers
      
======================================================= */

static pascal OSStatus dialog_svg_import_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_svg_import_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_svg_import_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_svg_import_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run SVG Import Dialog
      
======================================================= */

bool dialog_svg_import_run(float *scale,int *high)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_svg_import_wind,"SVGImport");

		// set controls
		
	dialog_set_float(dialog_svg_import_wind,kSVGImportScale,0,100.0f);
	dialog_set_int(dialog_svg_import_wind,kSVGImportHigh,0,5000);
	
	dialog_set_focus(dialog_svg_import_wind,kSVGImportScale,0);
	
		// show window
	
	ShowWindow(dialog_svg_import_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(dialog_svg_import_event_proc);
	InstallWindowEventHandler(dialog_svg_import_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_svg_import_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_svg_import_wind);
	
		// dialog to data
		
	if (!dialog_svg_import_cancel) {
		*scale=(float)dialog_get_int(dialog_svg_import_wind,kSVGImportScale,0);
		*high=(float)dialog_get_int(dialog_svg_import_wind,kSVGImportHigh,0);
	}

		// close window
		
	DisposeWindow(dialog_svg_import_wind);
	
	return(!dialog_svg_import_cancel);
}

