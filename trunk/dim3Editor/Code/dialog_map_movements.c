/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Movements Dialog

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

#define kMovementList						FOUR_CHAR_CODE('tmvm')
#define kMovementListNameColumn				FOUR_CHAR_CODE('name')
#define kMovementListGroupColumn			FOUR_CHAR_CODE('grop')
#define kMovementListReverseGroupColumn		FOUR_CHAR_CODE('rgop')

#define kMovementAddButton					FOUR_CHAR_CODE('amvm')
#define kMovementDeleteButton				FOUR_CHAR_CODE('dmvm')

#define kMoveList							FOUR_CHAR_CODE('tmov')
#define kMoveListTimeColumn					FOUR_CHAR_CODE('time')
#define kMoveListMoveColumn					FOUR_CHAR_CODE('move')
#define kMoveListRotateColumn				FOUR_CHAR_CODE('rott')
#define kMoveListSoundColumn				FOUR_CHAR_CODE('sond')

#define kMoveAddButton						FOUR_CHAR_CODE('amov')
#define kMoveDeleteButton					FOUR_CHAR_CODE('dmov')

int							dialog_movement_current_idx,
							dialog_move_current_idx;
bool						dialog_movement_ignore_deselect;
WindowRef					dialog_movements_wind;

/* =======================================================

      Reset List/Buttons
      
======================================================= */

ControlRef movements_list_get_ctrl(void)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=kMovementList;
	ctrl_id.id=0;
	GetControlByID(dialog_movements_wind,&ctrl_id,&ctrl);

	return(ctrl);
}

ControlRef moves_list_get_ctrl(void)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=kMoveList;
	ctrl_id.id=0;
	GetControlByID(dialog_movements_wind,&ctrl_id,&ctrl);

	return(ctrl);
}

void movements_list_reset(int movement_idx)
{
	ControlRef			ctrl;
	DataBrowserItemID	itemID;
	
	dialog_movement_current_idx=movement_idx;
	
		// clear movements list
		
	ctrl=movements_list_get_ctrl();
		
	dialog_movement_ignore_deselect=TRUE;
	RemoveDataBrowserItems(ctrl,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	dialog_movement_ignore_deselect=FALSE;

		// add in movement list items
	
	AddDataBrowserItems(ctrl,kDataBrowserNoItem,map.nmovement,NULL,kDataBrowserItemNoProperty);
	
		// select movement item
		
	if (dialog_movement_current_idx==-1) return;
		
	itemID=dialog_movement_current_idx+1;
	SetDataBrowserSelectedItems(ctrl,1,&itemID,kDataBrowserItemsAssign);
	RevealDataBrowserItem(ctrl,itemID,kDataBrowserNoItem,kDataBrowserRevealOnly);
}

void moves_list_reset(int move_idx)
{
	ControlRef			ctrl;
	DataBrowserItemID	itemID;
	
	dialog_move_current_idx=move_idx;
	
		// clear move list
		
	ctrl=moves_list_get_ctrl();
	
	dialog_movement_ignore_deselect=TRUE;
	RemoveDataBrowserItems(ctrl,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	dialog_movement_ignore_deselect=FALSE;
	
	if (dialog_movement_current_idx==-1) return;
	
		// add in move list items
		
	AddDataBrowserItems(ctrl,kDataBrowserNoItem,map.movements[dialog_movement_current_idx].nmove,NULL,kDataBrowserItemNoProperty);
	
		// select move item
		
	if (dialog_move_current_idx==-1) return;
		
	itemID=dialog_move_current_idx+1;
	SetDataBrowserSelectedItems(ctrl,1,&itemID,kDataBrowserItemsAssign);
	RevealDataBrowserItem(ctrl,itemID,kDataBrowserNoItem,kDataBrowserRevealOnly);
}

void movements_reset_buttons(void)
{
	ControlRef		add_movement_ctrl,delete_movement_ctrl,
					add_move_ctrl,delete_move_ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=kMovementAddButton;
	ctrl_id.id=0;
	GetControlByID(dialog_movements_wind,&ctrl_id,&add_movement_ctrl);
	
	ctrl_id.signature=kMovementDeleteButton;
	ctrl_id.id=0;
	GetControlByID(dialog_movements_wind,&ctrl_id,&delete_movement_ctrl);
	
	ctrl_id.signature=kMoveAddButton;
	ctrl_id.id=0;
	GetControlByID(dialog_movements_wind,&ctrl_id,&add_move_ctrl);
	
	ctrl_id.signature=kMoveDeleteButton;
	ctrl_id.id=0;
	GetControlByID(dialog_movements_wind,&ctrl_id,&delete_move_ctrl);
	
	if (map.nmovement>=max_movement) {
		DisableControl(add_movement_ctrl);
	}
	else {
		EnableControl(add_movement_ctrl);
	}
	
	if (dialog_movement_current_idx==-1) {
		DisableControl(delete_movement_ctrl);
		DisableControl(add_move_ctrl);
	}
	else {
		EnableControl(delete_movement_ctrl);
		EnableControl(add_move_ctrl);
	}
	
	if (dialog_move_current_idx==-1) {
		DisableControl(delete_move_ctrl);
	}
	else {
		EnableControl(delete_move_ctrl);
	}
}

/* =======================================================

      Map Movements Event Handlers
      
======================================================= */

static pascal OSStatus movements_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	int					idx;
	HICommand			cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kMovementAddButton:
					if (map.ngroup==0) {
						dialog_alert("Can Not Create Movement","You need at least one segment group to create a movement.");
						return(noErr);
					}
					
					idx=map_movement_add(&map);
					if (idx==-1) return(noErr);
					
					if (!dialog_movement_settings_run(&map.movements[idx])) {
						map_movement_delete(&map,idx);
						return(noErr);
					}
					
					movements_list_reset(idx);
					Draw1Control(movements_list_get_ctrl());

					moves_list_reset(-1);
					Draw1Control(moves_list_get_ctrl());
					
					movements_reset_buttons();

					return(noErr);
					
				case kMovementDeleteButton:
					if (dialog_movement_current_idx==-1) return(noErr);
					
					map_movement_delete(&map,dialog_movement_current_idx);
					
					movements_list_reset(-1);
					Draw1Control(movements_list_get_ctrl());
					
					moves_list_reset(-1);
					Draw1Control(moves_list_get_ctrl());
					
					movements_reset_buttons();
					
					return(noErr);
					
				case kMoveAddButton:
					idx=map_movement_move_add(&map,dialog_movement_current_idx);
					if (idx==-1) return(noErr);
					
					if (!dialog_movement_move_settings_run(&map.movements[dialog_movement_current_idx].moves[idx])) {
						map_movement_move_delete(&map,dialog_movement_current_idx,idx);
						return(noErr);
					}
					
					moves_list_reset(idx);
					Draw1Control(moves_list_get_ctrl());
					
					movements_reset_buttons();

					return(noErr);
					
				case kMoveDeleteButton:
					if (dialog_move_current_idx==-1) return(noErr);

					map_movement_move_delete(&map,dialog_movement_current_idx,dialog_move_current_idx);
					
					moves_list_reset(-1);
					Draw1Control(moves_list_get_ctrl());
					
					movements_reset_buttons();
					
					return(noErr);
				
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_movements_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Movement List Event Handlers
      
======================================================= */

static pascal OSStatus movements_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int					movement_idx;
	char				str[256];
	CFStringRef			cfstr;

	switch (property) {
		
		case kMovementListNameColumn:
			movement_idx=itemID-1;
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,map.movements[movement_idx].name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kMovementListGroupColumn:
			movement_idx=itemID-1;
			
			strcpy(str,map.groups[map.movements[movement_idx].group_idx].name);
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kMovementListReverseGroupColumn:
			movement_idx=itemID-1;
			
			strcpy(str,map.groups[map.movements[movement_idx].reverse_group_idx].name);
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
						
	}

	return(errDataBrowserPropertyNotSupported);
}

static pascal void movements_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	int				movement_idx;
	UInt32			count;
	
	switch (message) {

		case kDataBrowserItemDoubleClicked:
			movement_idx=itemID-1;
			if (dialog_movement_settings_run(&map.movements[movement_idx])) {
				movements_list_reset(movement_idx);
				Draw1Control(ctrl);
			}
			break;
			
		case kDataBrowserItemSelected:
			dialog_movement_current_idx=itemID-1;
			
			moves_list_reset(-1);
			Draw1Control(moves_list_get_ctrl());
			movements_reset_buttons();
			break;

		case kDataBrowserItemDeselected:
			if (dialog_movement_ignore_deselect) break;
			
			GetDataBrowserItemCount(ctrl,kDataBrowserNoItem,TRUE,kDataBrowserItemIsSelected,&count);
			if (count!=0) break;
			
			dialog_movement_current_idx=-1;
			
			moves_list_reset(-1);
			Draw1Control(moves_list_get_ctrl());
			movements_reset_buttons();
			break;

	}
}

/* =======================================================

      Move List Event Handlers
      
======================================================= */

static pascal OSStatus moves_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int					move_idx;
	char				str[256];
	movement_move_type	*move;
	CFStringRef			cfstr;

	switch (property) {
		
		case kMoveListTimeColumn:
			move_idx=itemID-1;
			
			move=&map.movements[dialog_movement_current_idx].moves[move_idx];
			sprintf(str,"%d msec",move->msec);
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kMoveListMoveColumn:
			move_idx=itemID-1;
			
			move=&map.movements[dialog_movement_current_idx].moves[move_idx];
			sprintf(str,"%d,%d,%d",move->mov.x,move->mov.y,move->mov.z);
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kMoveListRotateColumn:
			move_idx=itemID-1;
			
			move=&map.movements[dialog_movement_current_idx].moves[move_idx];
			sprintf(str,"%.2f,%.2f,%.2f",move->rot.x,move->rot.y,move->rot.z);
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kMoveListSoundColumn:
			move_idx=itemID-1;
			
			move=&map.movements[dialog_movement_current_idx].moves[move_idx];
			if (move->sound_name[0]==0x0) return(noErr);
			sprintf(str,"%s @ %.2f",move->sound_name,move->sound_pitch);
				
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
						
	}

	return(errDataBrowserPropertyNotSupported);
}

static pascal void moves_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	int				move_idx;
	UInt32			count;

	switch (message) {

		case kDataBrowserItemDoubleClicked:
			move_idx=itemID-1;			
			if (dialog_movement_move_settings_run(&map.movements[dialog_movement_current_idx].moves[move_idx])) {
				moves_list_reset(move_idx);
				Draw1Control(ctrl);
			}
			break;
			
		case kDataBrowserItemSelected:
			dialog_move_current_idx=itemID-1;
			movements_reset_buttons();
			break;

		case kDataBrowserItemDeselected:
			if (dialog_movement_ignore_deselect) break;
			
			GetDataBrowserItemCount(ctrl,kDataBrowserNoItem,TRUE,kDataBrowserItemIsSelected,&count);
			if (count!=0) break;
			
			dialog_move_current_idx=-1;
			movements_reset_buttons();
			break;

	}
}

/* =======================================================

      Run Map Movements
      
======================================================= */

bool dialog_map_movements_run(void)
{
	ControlRef						ctrl;
	DataBrowserCallbacks			dbcall_movements,dbcall_moves;
	EventHandlerUPP					event_upp;
	DataBrowserItemDataUPP			list_movements_item_upp,list_moves_item_upp;
	DataBrowserItemNotificationUPP	list_movements_notify_upp,list_moves_notify_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_movements_wind,"MapMovements");
	
		// setup the movement list
		
	ctrl=movements_list_get_ctrl();
	
	dbcall_movements.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall_movements);
	
	list_movements_item_upp=NewDataBrowserItemDataUPP(&movements_list_item_proc);
	dbcall_movements.u.v1.itemDataCallback=list_movements_item_upp;

	list_movements_notify_upp=NewDataBrowserItemNotificationUPP(&movements_list_notify_proc);
	dbcall_movements.u.v1.itemNotificationCallback=list_movements_notify_upp;
	
	SetDataBrowserCallbacks(ctrl,&dbcall_movements);

		// setup the move list
		
	ctrl=moves_list_get_ctrl();
	
	dbcall_moves.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall_moves);
	
	list_moves_item_upp=NewDataBrowserItemDataUPP(&moves_list_item_proc);
	dbcall_moves.u.v1.itemDataCallback=list_moves_item_upp;

	list_moves_notify_upp=NewDataBrowserItemNotificationUPP(&moves_list_notify_proc);
	dbcall_moves.u.v1.itemNotificationCallback=list_moves_notify_upp;
	
	SetDataBrowserCallbacks(ctrl,&dbcall_moves);

		// reset lists
	
	movements_list_reset(-1);
	moves_list_reset(-1);
	movements_reset_buttons();

		// show window
	
	ShowWindow(dialog_movements_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(movements_event_proc);
	InstallWindowEventHandler(dialog_movements_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	RunAppModalLoopForWindow(dialog_movements_wind);
	
		// close window
		
	DisposeDataBrowserItemDataUPP(list_movements_item_upp);
	DisposeDataBrowserItemNotificationUPP(list_movements_notify_upp);
	
	DisposeDataBrowserItemDataUPP(list_moves_item_upp);
	DisposeDataBrowserItemNotificationUPP(list_moves_notify_upp);
	
	DisposeWindow(dialog_movements_wind);
	
	return(TRUE);
}



