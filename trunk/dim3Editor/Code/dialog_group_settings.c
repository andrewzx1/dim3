/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Group Setting Dialog

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

#include "interface.h"
#include "dialog.h"

extern map_type				map;

#define kGroupName							FOUR_CHAR_CODE('name')
#define kGroupCount							FOUR_CHAR_CODE('attc')

#define kGroupClear							FOUR_CHAR_CODE('cler')

int							dialog_groups_idx;
bool						dialog_group_settings_cancel;
WindowRef					dialog_group_settings_wind;

/* =======================================================

      Group Setting Event Handlers
      
======================================================= */

static pascal OSStatus group_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kGroupClear:
					group_clear(dialog_groups_idx,FALSE);
					dialog_set_int(dialog_group_settings_wind,kGroupCount,0,0);
					dialog_redraw(dialog_group_settings_wind,kGroupCount,0);
					return(noErr);
				
				case kHICommandCancel:
					dialog_group_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_group_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_group_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Group Setting
      
======================================================= */

int dialog_group_settings_run(int group_idx)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	group_type				*group;
	
		// new or existing group?
		
	if (group_idx==-1) {
		if (map.group.ngroup==max_group) {
			os_dialog_alert("Groups","Reached maximum number of groups");
			return(-1);
		}
		
		group_idx=map.group.ngroup;
		map.group.ngroup++;
	}
	
	dialog_groups_idx=group_idx;
	group=&map.group.groups[group_idx];
	
		// open the dialog
		
	dialog_open(&dialog_group_settings_wind,"GroupSettings");

		// set controls
		
	dialog_set_text(dialog_group_settings_wind,kGroupName,0,group->name);
	dialog_set_int(dialog_group_settings_wind,kGroupCount,0,group_count(group_idx));
	
		// show window
	
	ShowWindow(dialog_group_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(group_setting_event_proc);
	InstallWindowEventHandler(dialog_group_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_group_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_group_settings_wind);
	
		// dialog to data
		
	if (!dialog_group_settings_cancel) {
		dialog_get_text(dialog_group_settings_wind,kGroupName,0,group->name,name_str_len);
	}

		// close window
		
	DisposeWindow(dialog_group_settings_wind);
	
	return(group_idx);
}

