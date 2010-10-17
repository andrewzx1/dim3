/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Line

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
#include "common_view.h"
#include "dialog.h"

bool							done;

extern file_path_setup_type		file_path_setup;
extern editor_state_type		state;

extern bool setup_xml_read(void);

/* =======================================================

      Main Event Loop
      
======================================================= */

OSStatus menu_event_callback(EventHandlerCallRef eventhandler,EventRef event,void *userdata)
{
	HICommand		cmd;
	
	GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
	
	if (menu_event_run(cmd.commandID)) return(noErr);

	return(eventNotHandledErr);
}

void doloop(void)
{
	EventHandlerRef		menu_event;
	EventHandlerUPP		menu_upp;
	EventTypeSpec		app_menu_events[]={{kEventClassCommand,kEventProcessCommand}};
	
	menu_upp=NewEventHandlerUPP(menu_event_callback);						   
	InstallEventHandler(GetApplicationEventTarget(),menu_upp,GetEventTypeCount(app_menu_events),app_menu_events,NULL,&menu_event);
	
	RunApplicationEventLoop();

	RemoveEventHandler(menu_event);
	DisposeEventHandlerUPP(menu_upp);
}

/* =======================================================

      Initialization Code
      
======================================================= */

// supergumba -- some of this is ANCIENT and probably deperciated, look it up and fix
void doinit(void)
{
	RegisterAppearanceClient();
	
	SetThemeCursor(kThemeArrowCursor);
	
	MoreMasterPointers(128);
	FlushEvents(everyEvent,0);

	HMSetTagDelay(250);
    
	state.map_opened=FALSE;
	
		// setup paths

	file_paths_setup(&file_path_setup);
	
		// setup XML
		
	setup_xml_read();
}

void doshutdown(void)
{
	UnregisterAppearanceClient();
}

/* =======================================================

      Start Menu
      
======================================================= */

void menu_start(void)
{
	IBNibRef				nib;
	CFStringRef				cf_str;
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"dim3 Editor",kCFStringEncodingMacRoman);
	CreateNibReference(cf_str,&nib);
	CFRelease(cf_str);
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"MenuBar",kCFStringEncodingMacRoman);
	SetMenuBarFromNib(nib,cf_str);
	CFRelease(cf_str);

	DisposeNibReference(nib);
}

/* =======================================================

      Infamous Main Code
      
======================================================= */

int main(int argc,char *argv[])
{
	doinit();
	
	menu_start();
	
	undo_initialize();
	menu_fix_enable();
	
	file_open_map();
	doloop();
	file_close_map();
	
	doshutdown();
	
    return(0);
}
