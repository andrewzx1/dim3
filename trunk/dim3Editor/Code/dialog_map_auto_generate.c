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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "interface.h"
#include "dialog.h"
#include "import.h"

#define kMapGenerateTabCount						3

#define kMapGenerateTab								FOUR_CHAR_CODE('tabb')

#define kMapGenerateSeed							FOUR_CHAR_CODE('seed')
#define kMapGenerateType							FOUR_CHAR_CODE('type')
#define kMapGenerateFlow							FOUR_CHAR_CODE('blck')
#define kMapGenerateMapSize							FOUR_CHAR_CODE('mpsz')

#define kMapGenerateCorridorType					FOUR_CHAR_CODE('tcor')
#define kMapGenerateDoorType						FOUR_CHAR_CODE('tdor')
#define kMapGenerateStairType						FOUR_CHAR_CODE('tstp')
#define kMapGenerateCeilingType						FOUR_CHAR_CODE('tcel')
#define kMapGenerateSecondStory						FOUR_CHAR_CODE('stry')
#define kMapGenerateWindow							FOUR_CHAR_CODE('wind')
#define kMapGenerateFrame							FOUR_CHAR_CODE('frme')
#define kMapGenerateRail							FOUR_CHAR_CODE('rail')

#define kMapGenerateLightBoost						FOUR_CHAR_CODE('brgt')
#define kMapGenerateLightMap						FOUR_CHAR_CODE('lmap')

#define kMapGenerateDoorSound						FOUR_CHAR_CODE('dsnd')
#define kMapGenerateLiftSound						FOUR_CHAR_CODE('lsnd')

#define kMapGenerateTexturePortalWall				FOUR_CHAR_CODE('pwtx')
#define kMapGenerateTexturePortalWall2				FOUR_CHAR_CODE('pw2x')
#define kMapGenerateTexturePortalFloor				FOUR_CHAR_CODE('pftx')
#define kMapGenerateTexturePortalCeiling1			FOUR_CHAR_CODE('pc2x')
#define kMapGenerateTexturePortalCeiling2			FOUR_CHAR_CODE('pctx')
#define kMapGenerateTextureCorridorWallCeiling		FOUR_CHAR_CODE('crtx')
#define kMapGenerateTextureCorridorFloor			FOUR_CHAR_CODE('crtf')
#define kMapGenerateTextureSecondStory				FOUR_CHAR_CODE('sstx')
#define kMapGenerateTextureFrame					FOUR_CHAR_CODE('frtx')
#define kMapGenerateTextureDoor						FOUR_CHAR_CODE('drtx')
#define kMapGenerateTextureWindow					FOUR_CHAR_CODE('wwtx')
#define kMapGenerateTextureStep						FOUR_CHAR_CODE('sttx')
#define kMapGenerateTextureRamp						FOUR_CHAR_CODE('rmtx')
#define kMapGenerateTextureColumn					FOUR_CHAR_CODE('cltx')
#define kMapGenerateTextureColumnBase				FOUR_CHAR_CODE('cbtx')
#define kMapGenerateTextureRail						FOUR_CHAR_CODE('rltx')

#define kMapGenerateNewSeed							FOUR_CHAR_CODE('nsed')

extern auto_generate_settings_type			ag_settings;

bool						dialog_map_generate_cancel;
WindowRef					dialog_map_generate_wind;

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
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

static pascal OSStatus map_generate_setting_tab_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	int				event_class,event_kind;
	HICommand		cmd;
	
	event_class=GetEventClass(event);
	event_kind=GetEventKind(event);
	
	if ((event_class==kEventClassCommand) && (event_kind==kEventProcessCommand)) {
			
		GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
		
		switch (cmd.commandID) {
				
				case kMapGenerateNewSeed:
					dialog_set_int(dialog_map_generate_wind,kMapGenerateSeed,0,(int)time(NULL));
					dialog_redraw(dialog_map_generate_wind,kMapGenerateSeed,0);
					return(noErr);
				
		}
			
		return(eventNotHandledErr);
	}
	
	if ((event_class==kEventClassControl) && (event_kind==kEventControlHit)) {
		dialog_switch_tab(dialog_map_generate_wind,kMapGenerateTab,0,kMapGenerateTabCount);
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Map Auto-Generate Setting
      
======================================================= */

bool dialog_map_auto_generate_setting_run(bool first)
{
	ControlRef				ctrl;
	ControlID				ctrl_id;
	EventHandlerUPP			event_upp,tab_event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}},
							tab_event_list[]={{kEventClassCommand,kEventProcessCommand},
											  {kEventClassControl,kEventControlHit}};
	
		// open the dialog
		
	dialog_open(&dialog_map_generate_wind,"MapGenerate");
	
		// tab
		
	dialog_set_tab(dialog_map_generate_wind,kMapGenerateTab,0,0,kMapGenerateTabCount);
	
	ctrl_id.signature=kMapGenerateTab;
	ctrl_id.id=0;
	GetControlByID(dialog_map_generate_wind,&ctrl_id,&ctrl);
	
	tab_event_upp=NewEventHandlerUPP(map_generate_setting_tab_proc);
	InstallControlEventHandler(ctrl,tab_event_upp,GetEventTypeCount(tab_event_list),tab_event_list,dialog_map_generate_wind,NULL);
	
		// old settings
		
	if (!first) {
		dialog_set_int(dialog_map_generate_wind,kMapGenerateSeed,0,ag_settings.seed);
		
		dialog_set_value(dialog_map_generate_wind,kMapGenerateMapSize,0,ag_settings.map.map_sz);
		dialog_set_combo(dialog_map_generate_wind,kMapGenerateType,0,ag_settings.type);
		dialog_set_combo(dialog_map_generate_wind,kMapGenerateFlow,0,ag_settings.flow);

		dialog_set_combo(dialog_map_generate_wind,kMapGenerateCorridorType,0,ag_settings.corridor_type);
		dialog_set_combo(dialog_map_generate_wind,kMapGenerateDoorType,0,ag_settings.door_type);
		dialog_set_combo(dialog_map_generate_wind,kMapGenerateStairType,0,ag_settings.stair_type);
		dialog_set_combo(dialog_map_generate_wind,kMapGenerateCeilingType,0,ag_settings.ceiling_type);
		
		dialog_set_value(dialog_map_generate_wind,kMapGenerateLightBoost,0,(int)(ag_settings.light_boost*1000.0f));
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateLightMap,0,ag_settings.light_map);
		
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateSecondStory,0,ag_settings.second_story);
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateWindow,0,ag_settings.window);
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateFrame,0,ag_settings.frame);
		dialog_set_boolean(dialog_map_generate_wind,kMapGenerateRail,0,ag_settings.rails);
		
		dialog_set_text(dialog_map_generate_wind,kMapGenerateDoorSound,0,ag_settings.sound.door);
		dialog_set_text(dialog_map_generate_wind,kMapGenerateLiftSound,0,ag_settings.sound.lift);
	
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalWall,0,FALSE,ag_settings.texture.portal_wall);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalWall2,0,FALSE,ag_settings.texture.portal_wall_2);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalFloor,0,FALSE,ag_settings.texture.portal_floor);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalCeiling1,0,FALSE,ag_settings.texture.portal_ceiling_1);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalCeiling2,0,FALSE,ag_settings.texture.portal_ceiling_2);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorWallCeiling,0,FALSE,ag_settings.texture.corridor_wall_ceiling);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorFloor,0,FALSE,ag_settings.texture.corridor_floor);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureSecondStory,0,FALSE,ag_settings.texture.second_story);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureFrame,0,FALSE,ag_settings.texture.frame);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureDoor,0,FALSE,ag_settings.texture.door);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureWindow,0,FALSE,ag_settings.texture.window);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureStep,0,FALSE,ag_settings.texture.steps);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureRamp,0,FALSE,ag_settings.texture.ramp);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureColumn,0,FALSE,ag_settings.texture.column);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureColumnBase,0,FALSE,ag_settings.texture.column_base);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureRail,0,FALSE,ag_settings.texture.rails);
	}
	else {
	
			// random seed
			
		dialog_set_int(dialog_map_generate_wind,kMapGenerateSeed,0,(int)time(NULL));
	
			// always need to set textures
			
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalWall,0,FALSE,0);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalWall2,0,FALSE,1);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalFloor,0,FALSE,2);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalCeiling1,0,FALSE,3);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalCeiling2,0,FALSE,4);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorWallCeiling,0,FALSE,5);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorFloor,0,FALSE,6);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureSecondStory,0,FALSE,7);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureFrame,0,FALSE,8);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureDoor,0,FALSE,9);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureWindow,0,FALSE,10);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureStep,0,FALSE,11);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureRamp,0,FALSE,12);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureColumn,0,FALSE,13);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureColumnBase,0,FALSE,14);
		dialog_fill_texture_combo(dialog_map_generate_wind,kMapGenerateTextureRail,0,FALSE,15);
	}
	
		// show window
	
	ShowWindow(dialog_map_generate_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_generate_setting_event_proc);
	InstallWindowEventHandler(dialog_map_generate_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_map_generate_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_map_generate_wind);
	
		// dialog to data
		
	ag_settings.seed=dialog_get_int(dialog_map_generate_wind,kMapGenerateSeed,0);
	ag_settings.type=dialog_get_combo(dialog_map_generate_wind,kMapGenerateType,0);
	ag_settings.flow=dialog_get_combo(dialog_map_generate_wind,kMapGenerateFlow,0);

	ag_settings.map.map_sz=dialog_get_value(dialog_map_generate_wind,kMapGenerateMapSize,0);

	ag_settings.corridor_type=dialog_get_combo(dialog_map_generate_wind,kMapGenerateCorridorType,0);
	ag_settings.door_type=dialog_get_combo(dialog_map_generate_wind,kMapGenerateDoorType,0);
	ag_settings.stair_type=dialog_get_combo(dialog_map_generate_wind,kMapGenerateStairType,0);
	ag_settings.ceiling_type=dialog_get_combo(dialog_map_generate_wind,kMapGenerateCeilingType,0);
	
	ag_settings.light_boost=((float)dialog_get_value(dialog_map_generate_wind,kMapGenerateLightBoost,0))/1000.0f;
	ag_settings.light_map=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateLightMap,0);

	ag_settings.second_story=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateSecondStory,0);
	ag_settings.window=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateWindow,0);
	ag_settings.frame=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateFrame,0);
	ag_settings.rails=dialog_get_boolean(dialog_map_generate_wind,kMapGenerateRail,0);
	
	dialog_get_text(dialog_map_generate_wind,kMapGenerateDoorSound,0,ag_settings.sound.door,name_str_len);
	dialog_get_text(dialog_map_generate_wind,kMapGenerateLiftSound,0,ag_settings.sound.lift,name_str_len);
	
	ag_settings.texture.portal_wall=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalWall,0,FALSE);
	ag_settings.texture.portal_wall_2=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalWall2,0,FALSE);
	ag_settings.texture.portal_floor=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalFloor,0,FALSE);
	ag_settings.texture.portal_ceiling_1=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalCeiling1,0,FALSE);
	ag_settings.texture.portal_ceiling_2=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTexturePortalCeiling2,0,FALSE);
	ag_settings.texture.corridor_wall_ceiling=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorWallCeiling,0,FALSE);
	ag_settings.texture.corridor_floor=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureCorridorFloor,0,FALSE);
	ag_settings.texture.second_story=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureSecondStory,0,FALSE);
	ag_settings.texture.frame=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureFrame,0,FALSE);
	ag_settings.texture.door=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureDoor,0,FALSE);
	ag_settings.texture.window=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureWindow,0,FALSE);
	ag_settings.texture.steps=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureStep,0,FALSE);
	ag_settings.texture.ramp=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureRamp,0,FALSE);
	ag_settings.texture.column=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureColumn,0,FALSE);
	ag_settings.texture.column_base=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureColumnBase,0,FALSE);
	ag_settings.texture.rails=dialog_get_texture_combo(dialog_map_generate_wind,kMapGenerateTextureRail,0,FALSE);
	
		// close window
		
	DisposeWindow(dialog_map_generate_wind);
	
	return(!dialog_map_generate_cancel);
}

