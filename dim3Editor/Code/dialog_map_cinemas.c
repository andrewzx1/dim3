/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Cinemas Dialog

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

#define kCinemaList							FOUR_CHAR_CODE('tcin')
#define kCinemaListNameColumn				FOUR_CHAR_CODE('name')
#define kCinemaListLengthColumn				FOUR_CHAR_CODE('clen')

#define kCinemaAddButton					FOUR_CHAR_CODE('acin')
#define kCinemaDeleteButton					FOUR_CHAR_CODE('dcin')

#define kActionList							FOUR_CHAR_CODE('tact')
#define kActionListTimeColumn				FOUR_CHAR_CODE('time')
#define kActionListActorColumn				FOUR_CHAR_CODE('actr')
#define kActionListActionColumn				FOUR_CHAR_CODE('actn')
#define kActionListNodeColumn				FOUR_CHAR_CODE('node')
#define kActionListAnimationColumn			FOUR_CHAR_CODE('anim')

#define kActionAddButton					FOUR_CHAR_CODE('aact')
#define kActionDeleteButton					FOUR_CHAR_CODE('dact')

int							dialog_cinema_current_idx,
							dialog_action_current_idx;
bool						dialog_cinema_ignore_deselect;
WindowRef					dialog_cinemas_wind;

/* =======================================================

      Reset List/Buttons
      
======================================================= */

ControlRef cinemas_list_get_ctrl(void)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=kCinemaList;
	ctrl_id.id=0;
	GetControlByID(dialog_cinemas_wind,&ctrl_id,&ctrl);

	return(ctrl);
}

ControlRef actions_list_get_ctrl(void)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=kActionList;
	ctrl_id.id=0;
	GetControlByID(dialog_cinemas_wind,&ctrl_id,&ctrl);

	return(ctrl);
}

void cinemas_list_reset(int cinema_idx)
{
	ControlRef			ctrl;
	DataBrowserItemID	itemID;
	
	dialog_cinema_current_idx=cinema_idx;
	
		// clear cinemas list
		
	ctrl=cinemas_list_get_ctrl();
		
	dialog_cinema_ignore_deselect=TRUE;
	RemoveDataBrowserItems(ctrl,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	dialog_cinema_ignore_deselect=FALSE;

		// add in cinema list items
	
	AddDataBrowserItems(ctrl,kDataBrowserNoItem,map.cinema.ncinema,NULL,kDataBrowserItemNoProperty);
	
		// select cinema item
		
	if (dialog_cinema_current_idx==-1) return;
		
	itemID=dialog_cinema_current_idx+1;
	SetDataBrowserSelectedItems(ctrl,1,&itemID,kDataBrowserItemsAssign);
	RevealDataBrowserItem(ctrl,itemID,kDataBrowserNoItem,kDataBrowserRevealOnly);
}

void actions_list_reset(int action_idx)
{
	ControlRef			ctrl;
	DataBrowserItemID	itemID;
	
	dialog_action_current_idx=action_idx;
	
		// clear action list
		
	ctrl=actions_list_get_ctrl();
	
	dialog_cinema_ignore_deselect=TRUE;
	RemoveDataBrowserItems(ctrl,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	dialog_cinema_ignore_deselect=FALSE;
	
	if (dialog_cinema_current_idx==-1) return;
	
		// add in action list items
		
	AddDataBrowserItems(ctrl,kDataBrowserNoItem,map.cinema.cinemas[dialog_cinema_current_idx].naction,NULL,kDataBrowserItemNoProperty);
	
		// select action item
		
	if (dialog_action_current_idx==-1) return;
		
	itemID=dialog_action_current_idx+1;
	SetDataBrowserSelectedItems(ctrl,1,&itemID,kDataBrowserItemsAssign);
	RevealDataBrowserItem(ctrl,itemID,kDataBrowserNoItem,kDataBrowserRevealOnly);
}

void cinemas_reset_buttons(void)
{
	ControlRef		delete_cinema_ctrl,
					add_action_ctrl,delete_action_ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=kCinemaDeleteButton;
	ctrl_id.id=0;
	GetControlByID(dialog_cinemas_wind,&ctrl_id,&delete_cinema_ctrl);
	
	ctrl_id.signature=kActionAddButton;
	ctrl_id.id=0;
	GetControlByID(dialog_cinemas_wind,&ctrl_id,&add_action_ctrl);
	
	ctrl_id.signature=kActionDeleteButton;
	ctrl_id.id=0;
	GetControlByID(dialog_cinemas_wind,&ctrl_id,&delete_action_ctrl);
	
	if (dialog_cinema_current_idx==-1) {
		DisableControl(delete_cinema_ctrl);
		DisableControl(add_action_ctrl);
	}
	else {
		EnableControl(delete_cinema_ctrl);
		EnableControl(add_action_ctrl);
	}
	
	if (dialog_action_current_idx==-1) {
		DisableControl(delete_action_ctrl);
	}
	else {
		EnableControl(delete_action_ctrl);
	}
}

/* =======================================================

      Map Cinemas Event Handlers
      
======================================================= */

static pascal OSStatus cinemas_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	int					idx;
	HICommand			cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kCinemaAddButton:
					idx=map_cinema_add(&map);
					if (idx==-1) return(noErr);
					
					if (!dialog_cinema_settings_run(&map.cinema.cinemas[idx])) {
						map_cinema_delete(&map,idx);
						return(noErr);
					}
					
					cinemas_list_reset(idx);
					Draw1Control(cinemas_list_get_ctrl());

					actions_list_reset(-1);
					Draw1Control(actions_list_get_ctrl());
					
					cinemas_reset_buttons();

					return(noErr);
					
				case kCinemaDeleteButton:
					if (dialog_cinema_current_idx==-1) return(noErr);
					
					map_cinema_delete(&map,dialog_cinema_current_idx);
					
					cinemas_list_reset(-1);
					Draw1Control(cinemas_list_get_ctrl());
					
					actions_list_reset(-1);
					Draw1Control(actions_list_get_ctrl());
					
					cinemas_reset_buttons();
					
					return(noErr);
					
				case kActionAddButton:
					idx=map_cinema_add_action(&map,dialog_cinema_current_idx);
					if (idx==-1) return(noErr);
					
					if (!dialog_cinema_action_settings_run(&map.cinema.cinemas[dialog_cinema_current_idx].actions[idx])) {
						map_cinema_delete_action(&map,dialog_cinema_current_idx,idx);
						return(noErr);
					}
					
					actions_list_reset(idx);
					Draw1Control(actions_list_get_ctrl());
					
					cinemas_reset_buttons();

					return(noErr);
					
				case kActionDeleteButton:
					if (dialog_action_current_idx==-1) return(noErr);

					map_cinema_delete_action(&map,dialog_cinema_current_idx,dialog_action_current_idx);
					
					actions_list_reset(-1);
					Draw1Control(actions_list_get_ctrl());
					
					cinemas_reset_buttons();
					
					return(noErr);
				
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_cinemas_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}

	return(eventNotHandledErr);
}

/* =======================================================

      Cinema List Event Handlers
      
======================================================= */

static pascal OSStatus cinemas_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int					cinema_idx;
	char				str[256];
	CFStringRef			cfstr;

	switch (property) {
		
		case kCinemaListNameColumn:
			cinema_idx=itemID-1;
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,map.cinema.cinemas[cinema_idx].name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kCinemaListLengthColumn:
			cinema_idx=itemID-1;
			
			sprintf(str,"%d msec",map.cinema.cinemas[cinema_idx].len_msec);
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
						
	}

	return(errDataBrowserPropertyNotSupported);
}

static pascal void cinemas_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	int				cinema_idx;
	UInt32			count;

	switch (message) {

		case kDataBrowserItemDoubleClicked:
			cinema_idx=itemID-1;
			if (dialog_cinema_settings_run(&map.cinema.cinemas[cinema_idx])) {
				cinemas_list_reset(cinema_idx);
				Draw1Control(ctrl);
			}
			break;
			
		case kDataBrowserItemSelected:
			dialog_cinema_current_idx=itemID-1;
			
			actions_list_reset(-1);
			Draw1Control(actions_list_get_ctrl());
			cinemas_reset_buttons();
			break;

		case kDataBrowserItemDeselected:
			if (dialog_cinema_ignore_deselect) break;
			
			GetDataBrowserItemCount(ctrl,kDataBrowserNoItem,TRUE,kDataBrowserItemIsSelected,&count);
			if (count!=0) break;
			
			dialog_cinema_current_idx=-1;
			
			actions_list_reset(-1);
			Draw1Control(actions_list_get_ctrl());
			cinemas_reset_buttons();
			break;

	}
}

/* =======================================================

      Action List Event Handlers
      
======================================================= */

static pascal OSStatus actions_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int						action_idx;
	char					str[256];
	char					action_type_str[][32]={"None","Place","Move","Show","Hide"};
	map_cinema_action_type	*action;
	CFStringRef				cfstr;

	switch (property) {
		
		case kActionListTimeColumn:
			action_idx=itemID-1;
			
			action=&map.cinema.cinemas[dialog_cinema_current_idx].actions[action_idx];
			if (action->action==cinema_action_move) {
				sprintf(str,"%d to %d msec",action->start_msec,action->end_msec);
			}
			else {
				sprintf(str,"%d msec",action->start_msec);
			}
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kActionListActorColumn:
			action_idx=itemID-1;
			action=&map.cinema.cinemas[dialog_cinema_current_idx].actions[action_idx];
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,action->actor_name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kActionListActionColumn:
			action_idx=itemID-1;
			action=&map.cinema.cinemas[dialog_cinema_current_idx].actions[action_idx];
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,action_type_str[action->action],kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kActionListNodeColumn:
			action_idx=itemID-1;
			action=&map.cinema.cinemas[dialog_cinema_current_idx].actions[action_idx];
				
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,action->node_name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kActionListAnimationColumn:
			action_idx=itemID-1;
			
			action=&map.cinema.cinemas[dialog_cinema_current_idx].actions[action_idx];
			
			str[0]=0x0;
			if (action->animation_name[0]!=0x0) {
				if (action->next_animation_name[0]==0x0) {
					strcpy(str,action->animation_name);
				}
				else {
					sprintf(str,"%s \\ %s",action->animation_name,action->next_animation_name);
				}
			}
				
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
						
	}

	return(errDataBrowserPropertyNotSupported);
}

static pascal void actions_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	int				action_idx;
	UInt32			count;

	switch (message) {

		case kDataBrowserItemDoubleClicked:
			action_idx=itemID-1;			
			if (dialog_cinema_action_settings_run(&map.cinema.cinemas[dialog_cinema_current_idx].actions[action_idx])) {
				actions_list_reset(action_idx);
				Draw1Control(ctrl);
			}
			break;
			
		case kDataBrowserItemSelected:
			dialog_action_current_idx=itemID-1;
			cinemas_reset_buttons();
			break;

		case kDataBrowserItemDeselected:
			if (dialog_cinema_ignore_deselect) break;
			
			GetDataBrowserItemCount(ctrl,kDataBrowserNoItem,TRUE,kDataBrowserItemIsSelected,&count);
			if (count!=0) break;
			
			dialog_action_current_idx=-1;
			cinemas_reset_buttons();
			break;

	}
}

/* =======================================================

      Run Map Cinemas
      
======================================================= */

bool dialog_map_cinemas_run(void)
{
	ControlRef						ctrl;
	DataBrowserCallbacks			dbcall_cinemas,dbcall_actions;
	EventHandlerUPP					event_upp;
	DataBrowserItemDataUPP			list_cinemas_item_upp,list_actions_item_upp;
	DataBrowserItemNotificationUPP	list_cinemas_notify_upp,list_actions_notify_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_cinemas_wind,"MapCinemas");
	
		// setup the movement list
		
	ctrl=cinemas_list_get_ctrl();
	
	dbcall_cinemas.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall_cinemas);
	
	list_cinemas_item_upp=NewDataBrowserItemDataUPP(&cinemas_list_item_proc);
	dbcall_cinemas.u.v1.itemDataCallback=list_cinemas_item_upp;

	list_cinemas_notify_upp=NewDataBrowserItemNotificationUPP(&cinemas_list_notify_proc);
	dbcall_cinemas.u.v1.itemNotificationCallback=list_cinemas_notify_upp;
	
	SetDataBrowserCallbacks(ctrl,&dbcall_cinemas);

		// setup the move list
		
	ctrl=actions_list_get_ctrl();
	
	dbcall_actions.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall_actions);
	
	list_actions_item_upp=NewDataBrowserItemDataUPP(&actions_list_item_proc);
	dbcall_actions.u.v1.itemDataCallback=list_actions_item_upp;

	list_actions_notify_upp=NewDataBrowserItemNotificationUPP(&actions_list_notify_proc);
	dbcall_actions.u.v1.itemNotificationCallback=list_actions_notify_upp;
	
	SetDataBrowserCallbacks(ctrl,&dbcall_actions);

		// reset lists
	
	cinemas_list_reset(-1);
	actions_list_reset(-1);
	cinemas_reset_buttons();

		// show window
	
	ShowWindow(dialog_cinemas_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(cinemas_event_proc);
	InstallWindowEventHandler(dialog_cinemas_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	RunAppModalLoopForWindow(dialog_cinemas_wind);
	
		// close window
		
	DisposeDataBrowserItemDataUPP(list_cinemas_item_upp);
	DisposeDataBrowserItemNotificationUPP(list_cinemas_notify_upp);
	
	DisposeDataBrowserItemDataUPP(list_actions_item_upp);
	DisposeDataBrowserItemNotificationUPP(list_actions_notify_upp);
	
	DisposeWindow(dialog_cinemas_wind);
	
	return(TRUE);
}



