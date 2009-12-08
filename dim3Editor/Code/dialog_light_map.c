/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Light Map Dialog

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "interface.h"
#include "dialog.h"

extern map_type				map;

#define kLightMapSize						FOUR_CHAR_CODE('size')
#define kLightMapQuality					FOUR_CHAR_CODE('qual')
#define kLightMapBlurCount					FOUR_CHAR_CODE('blur')

bool						dialog_light_map_cancel;
WindowRef					dialog_light_map_wind,dialog_light_map_generate_wind;

extern bool light_maps_create(char *err_str);

/* =======================================================

      Height Import Event Handlers
      
======================================================= */

static pascal OSStatus light_map_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_light_map_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_light_map_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_light_map_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Height Import Dialog
      
======================================================= */

bool dialog_light_map_run(void)
{
	int						n,size,quality,blur_count,
							size_list[4]={256,512,1024,2048};
	bool					ok;
	char					err_str[256];
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_light_map_wind,"LightMap");
	
		// set controls
	
	size=0;
	for (n=0;n!=4;n++) {
		if (map.settings.light_map_size==size_list[n]) {
			size=n;
			break;
		}
	}
	
	dialog_set_combo(dialog_light_map_wind,kLightMapSize,0,size);
	dialog_set_value(dialog_light_map_wind,kLightMapQuality,0,map.settings.light_map_quality);
	dialog_set_value(dialog_light_map_wind,kLightMapBlurCount,0,map.settings.light_map_blur_count);
	
		// show window
		
	ShowWindow(dialog_light_map_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(light_map_event_proc);
	InstallWindowEventHandler(dialog_light_map_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_light_map_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_light_map_wind);
	
		// dialog to data
		
	size=size_list[dialog_get_combo(dialog_light_map_wind,kLightMapSize,0)];
	quality=dialog_get_value(dialog_light_map_wind,kLightMapQuality,0);
	blur_count=dialog_get_value(dialog_light_map_wind,kLightMapBlurCount,0);

		// close window
		
	DisposeWindow(dialog_light_map_wind);
	
	if (dialog_light_map_cancel) return(FALSE);
	
		// run the light map generate
		
	map.settings.light_map_size=size;
	map.settings.light_map_quality=quality;
	map.settings.light_map_blur_count=blur_count;
		
	dialog_open(&dialog_light_map_generate_wind,"LightMapGenerate");
	ShowWindow(dialog_light_map_generate_wind);

	SetThemeCursor(kThemeWatchCursor);
	ok=light_maps_create(err_str);
	SetThemeCursor(kThemeArrowCursor);
	
	DisposeWindow(dialog_light_map_generate_wind);
		
	if (!ok) dialog_alert("Can not build light maps",err_str);
	
	return(ok);
}

