/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Auto-Generate Setting Dialogs

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

#define kAutoGenerateStyle							FOUR_CHAR_CODE('styl')
#define kAutoGenerateMirror							FOUR_CHAR_CODE('mirr')
#define kAutoGenerateSeed							FOUR_CHAR_CODE('seed')

#define kAutoGenerateRoomCount						FOUR_CHAR_CODE('nrom')
#define kAutoGenerateRoomSize						FOUR_CHAR_CODE('nsiz')
#define kAutoGenerateRoomHeight						FOUR_CHAR_CODE('nhgh')
#define kAutoGenerateStoryCount						FOUR_CHAR_CODE('nsct')
#define kAutoGenerateMergeCount						FOUR_CHAR_CODE('nmct')

#define kMapGenerateNewSeed							FOUR_CHAR_CODE('nsed')

bool						dialog_map_generate_cancel;
WindowRef					dialog_map_generate_wind;

bool						dialog_map_generate_first=TRUE;
ag_build_setup_type			ag_build_setup;

map_type					map;

/* =======================================================

      Map Generate Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_generate_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand					cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandCancel:
					dialog_map_generate_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_map_generate_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_map_generate_wind);
					return(noErr);
					
				case kMapGenerateNewSeed:
					dialog_set_int(dialog_map_generate_wind,kAutoGenerateSeed,0,(int)time(NULL));
					dialog_redraw(dialog_map_generate_wind,kAutoGenerateSeed,0);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Map Auto-Generate Setting
      
======================================================= */

bool dialog_map_auto_generate_setting_run(void)
{
	int						n,nstyle;
	char					style_names[128][name_str_len];
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// auto generation requires 4 textures
		
	for (n=0;n!=4;n++) {
		if (map.textures[n].frames[0].name[0]==0x0) {
			os_dialog_alert("Auto Generator","Auto Generator requires first 4 texture slots to be filled.");
			return(FALSE);
		}
	}
		
			// first time, create defaults
		
	if (dialog_map_generate_first) {
		dialog_map_generate_first=FALSE;
		
		ag_build_setup.style_idx=0;
		ag_build_setup.seed=(int)time(NULL);
		ag_build_setup.mirror=FALSE;
		ag_build_setup.room_count=20;
		ag_build_setup.room_sz=400;
		ag_build_setup.room_high=60;
		ag_build_setup.story_count=1;
		ag_build_setup.merge_count=2;
	}
											  
		// open the dialog
		
	dialog_open(&dialog_map_generate_wind,"AutoGenerateMap");
	
		// settings
		
	nstyle=ag_get_styles((char*)style_names);
	dialog_clear_combo(dialog_map_generate_wind,kAutoGenerateStyle,0);
	
	for (n=0;n!=nstyle;n++) {
		dialog_add_combo_item(dialog_map_generate_wind,kAutoGenerateStyle,0,style_names[n],0);
	}
	
	dialog_set_combo(dialog_map_generate_wind,kAutoGenerateStyle,0,ag_build_setup.style_idx);
	dialog_set_boolean(dialog_map_generate_wind,kAutoGenerateMirror,0,ag_build_setup.mirror);
	dialog_set_int(dialog_map_generate_wind,kAutoGenerateSeed,0,ag_build_setup.seed);

	dialog_set_value(dialog_map_generate_wind,kAutoGenerateRoomCount,0,ag_build_setup.room_count);
	dialog_set_value(dialog_map_generate_wind,kAutoGenerateRoomSize,0,ag_build_setup.room_sz);
	dialog_set_value(dialog_map_generate_wind,kAutoGenerateRoomHeight,0,ag_build_setup.room_high);
	dialog_set_value(dialog_map_generate_wind,kAutoGenerateStoryCount,0,ag_build_setup.story_count);
	dialog_set_value(dialog_map_generate_wind,kAutoGenerateMergeCount,0,ag_build_setup.merge_count);

		// show window
	
	ShowWindow(dialog_map_generate_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_generate_setting_event_proc);
	InstallWindowEventHandler(dialog_map_generate_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_map_generate_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_map_generate_wind);
	
		// dialog to data
		
	ag_build_setup.style_idx=dialog_get_combo(dialog_map_generate_wind,kAutoGenerateStyle,0);
	ag_build_setup.mirror=dialog_get_boolean(dialog_map_generate_wind,kAutoGenerateMirror,0);
	ag_build_setup.seed=dialog_get_int(dialog_map_generate_wind,kAutoGenerateSeed,0);

	ag_build_setup.room_count=dialog_get_value(dialog_map_generate_wind,kAutoGenerateRoomCount,0);
	ag_build_setup.room_sz=dialog_get_value(dialog_map_generate_wind,kAutoGenerateRoomSize,0);
	ag_build_setup.room_high=dialog_get_value(dialog_map_generate_wind,kAutoGenerateRoomHeight,0);
	ag_build_setup.story_count=dialog_get_value(dialog_map_generate_wind,kAutoGenerateStoryCount,0);
	ag_build_setup.merge_count=dialog_get_value(dialog_map_generate_wind,kAutoGenerateMergeCount,0);

		// close window
		
	DisposeWindow(dialog_map_generate_wind);
	
	if (dialog_map_generate_cancel) return(FALSE);
	
		// generate map

	select_clear();
	undo_clear();

		// run the auto-generate
		
	os_set_wait_cursor();
	ag_generate_map(&ag_build_setup);
	os_set_arrow_cursor();
	
		// redraw the map
	
	view_center_all(FALSE);		
	main_wind_draw();
	
	texture_palette_reset();
	menu_fix_enable();

	return(TRUE);
}

