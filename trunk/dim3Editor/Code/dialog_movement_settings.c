/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Movements Dialog

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

#define kMovementName						FOUR_CHAR_CODE('name')
#define kMovementGroup						FOUR_CHAR_CODE('grop')
#define kMovementReverseGroup				FOUR_CHAR_CODE('rgrp')
#define kMovementAutoStart					FOUR_CHAR_CODE('asrt')
#define kMovementAutoOpen					FOUR_CHAR_CODE('aopn')
#define kMovementAutoOpenStand				FOUR_CHAR_CODE('aoso')
#define kMovementAutoOpenDistance			FOUR_CHAR_CODE('dist')
#define kMovementAutoOpenNeverClose			FOUR_CHAR_CODE('aonc')
#define kMovementLoop						FOUR_CHAR_CODE('loop')

#define kMoveList							FOUR_CHAR_CODE('tmov')
#define kMoveListTimeColumn					FOUR_CHAR_CODE('time')
#define kMoveListMoveColumn					FOUR_CHAR_CODE('move')
#define kMoveListRotateColumn				FOUR_CHAR_CODE('rott')
#define kMoveListSoundColumn				FOUR_CHAR_CODE('sond')

#define kMoveAddButton						FOUR_CHAR_CODE('amov')
#define kMoveDeleteButton					FOUR_CHAR_CODE('dmov')

int							dialog_movement_idx,
							dialog_move_idx;
bool						dialog_movement_ignore_deselect;
WindowRef					dialog_movement_settings_wind;

/* =======================================================

      Reset List/Buttons
      
======================================================= */

ControlRef moves_list_get_ctrl(void)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=kMoveList;
	ctrl_id.id=0;
	GetControlByID(dialog_movement_settings_wind,&ctrl_id,&ctrl);

	return(ctrl);
}

void moves_list_reset(int move_idx)
{
	ControlRef			ctrl;
	DataBrowserItemID	itemID;
	
	dialog_move_idx=move_idx;
	
		// clear move list
		
	ctrl=moves_list_get_ctrl();
	
	dialog_movement_ignore_deselect=TRUE;
	RemoveDataBrowserItems(ctrl,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	dialog_movement_ignore_deselect=FALSE;
	
	if (dialog_movement_idx==-1) return;
	
		// add in move list items
		
	AddDataBrowserItems(ctrl,kDataBrowserNoItem,map.movement.movements[dialog_movement_idx].nmove,NULL,kDataBrowserItemNoProperty);
	
		// select move item
		
	if (dialog_move_idx==-1) return;
		
	itemID=dialog_move_idx+1;
	SetDataBrowserSelectedItems(ctrl,1,&itemID,kDataBrowserItemsAssign);
	RevealDataBrowserItem(ctrl,itemID,kDataBrowserNoItem,kDataBrowserRevealOnly);
}

void movements_reset_buttons(void)
{
	ControlRef		delete_move_ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=kMoveDeleteButton;
	ctrl_id.id=0;
	GetControlByID(dialog_movement_settings_wind,&ctrl_id,&delete_move_ctrl);
	
	if (dialog_move_idx==-1) {
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
								
				case kMoveAddButton:
					idx=map_movement_move_add(&map,dialog_movement_idx);
					if (idx==-1) return(noErr);
					
					if (!dialog_movement_move_settings_run(&map.movement.movements[dialog_movement_idx].moves[idx])) {
						map_movement_move_delete(&map,dialog_movement_idx,idx);
						return(noErr);
					}
					
					moves_list_reset(idx);
					Draw1Control(moves_list_get_ctrl());
					
					movements_reset_buttons();

					return(noErr);
					
				case kMoveDeleteButton:
					if (dialog_move_idx==-1) return(noErr);

					map_movement_move_delete(&map,dialog_movement_idx,dialog_move_idx);
					
					moves_list_reset(-1);
					Draw1Control(moves_list_get_ctrl());
					
					movements_reset_buttons();
					
					return(noErr);
				
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_movement_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
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
			
			move=&map.movement.movements[dialog_movement_idx].moves[move_idx];
			sprintf(str,"%d msec",move->msec);
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kMoveListMoveColumn:
			move_idx=itemID-1;
			
			move=&map.movement.movements[dialog_movement_idx].moves[move_idx];
			sprintf(str,"%d,%d,%d",move->mov.x,move->mov.y,move->mov.z);
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kMoveListRotateColumn:
			move_idx=itemID-1;
			
			move=&map.movement.movements[dialog_movement_idx].moves[move_idx];
			sprintf(str,"%.2f,%.2f,%.2f",move->rot.x,move->rot.y,move->rot.z);
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kMoveListSoundColumn:
			move_idx=itemID-1;
			
			move=&map.movement.movements[dialog_movement_idx].moves[move_idx];
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
			if (dialog_movement_move_settings_run(&map.movement.movements[dialog_movement_idx].moves[move_idx])) {
				moves_list_reset(move_idx);
				Draw1Control(ctrl);
			}
			break;
			
		case kDataBrowserItemSelected:
			dialog_move_idx=itemID-1;
			movements_reset_buttons();
			break;

		case kDataBrowserItemDeselected:
			if (dialog_movement_ignore_deselect) break;
			
			GetDataBrowserItemCount(ctrl,kDataBrowserNoItem,TRUE,kDataBrowserItemIsSelected,&count);
			if (count!=0) break;
			
			dialog_move_idx=-1;
			movements_reset_buttons();
			break;

	}
}

/* =======================================================

      Run Map Movements
      
======================================================= */

int dialog_map_movements_run(int movement_idx)
{
	int								n;
	ControlRef						ctrl;
	DataBrowserCallbacks			dbcall_moves;
	EventHandlerUPP					event_upp;
	DataBrowserItemDataUPP			list_moves_item_upp;
	DataBrowserItemNotificationUPP	list_moves_notify_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	movement_type					*movement;
	
		// new or existing movement?
		
	if (movement_idx==-1) {
		if (map.movement.nmovement==max_movement) {
			os_dialog_alert("Movements","Reached maximum number of movements");
			return(-1);
		}
		
		movement_idx=map.movement.nmovement;
		map.movement.nmovement++;
	}
	
	dialog_movement_idx=movement_idx;
	movement=&map.movement.movements[movement_idx];
	
		// open the dialog
		
	dialog_open(&dialog_movement_settings_wind,"MovementSettings");
	
		// setup the move list
		
	ctrl=moves_list_get_ctrl();
	
	dbcall_moves.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall_moves);
	
	list_moves_item_upp=NewDataBrowserItemDataUPP(&moves_list_item_proc);
	dbcall_moves.u.v1.itemDataCallback=list_moves_item_upp;

	list_moves_notify_upp=NewDataBrowserItemNotificationUPP(&moves_list_notify_proc);
	dbcall_moves.u.v1.itemNotificationCallback=list_moves_notify_upp;
	
	SetDataBrowserCallbacks(ctrl,&dbcall_moves);
	
		// set controls

	dialog_set_text(dialog_movement_settings_wind,kMovementName,0,movement->name);
	
	dialog_clear_combo(dialog_movement_settings_wind,kMovementGroup,0);
	dialog_clear_combo(dialog_movement_settings_wind,kMovementReverseGroup,0);
	dialog_add_combo_item(dialog_movement_settings_wind,kMovementReverseGroup,0,"None",0);
	
	for (n=0;n!=map.group.ngroup;n++) {
		dialog_add_combo_item(dialog_movement_settings_wind,kMovementGroup,0,map.group.groups[n].name,0);
		dialog_add_combo_item(dialog_movement_settings_wind,kMovementReverseGroup,0,map.group.groups[n].name,0);
	}
	
	dialog_set_combo(dialog_movement_settings_wind,kMovementGroup,0,movement->group_idx);
	dialog_set_combo(dialog_movement_settings_wind,kMovementReverseGroup,0,(movement->reverse_group_idx+1));
	
	dialog_set_boolean(dialog_movement_settings_wind,kMovementAutoStart,0,movement->auto_start);
	dialog_set_boolean(dialog_movement_settings_wind,kMovementAutoOpen,0,movement->auto_open);
	dialog_set_int(dialog_movement_settings_wind,kMovementAutoOpenDistance,0,movement->auto_open_distance);
	dialog_set_boolean(dialog_movement_settings_wind,kMovementAutoOpenStand,0,movement->auto_open_stand);
	dialog_set_boolean(dialog_movement_settings_wind,kMovementAutoOpenNeverClose,0,movement->auto_open_never_close);
	dialog_set_boolean(dialog_movement_settings_wind,kMovementLoop,0,movement->loop);
	
	dialog_set_focus(dialog_movement_settings_wind,kMovementName,0);

		// reset lists
	
	moves_list_reset(-1);
	movements_reset_buttons();

		// show window
	
	ShowWindow(dialog_movement_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(movements_event_proc);
	InstallWindowEventHandler(dialog_movement_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	RunAppModalLoopForWindow(dialog_movement_settings_wind);
	
		// get controls
		
	dialog_get_text(dialog_movement_settings_wind,kMovementName,0,movement->name,name_str_len);
	movement->group_idx=dialog_get_combo(dialog_movement_settings_wind,kMovementGroup,0);
	movement->reverse_group_idx=dialog_get_combo(dialog_movement_settings_wind,kMovementReverseGroup,0)-1;
	movement->auto_start=dialog_get_boolean(dialog_movement_settings_wind,kMovementAutoStart,0);
	movement->auto_open=dialog_get_boolean(dialog_movement_settings_wind,kMovementAutoOpen,0);
	movement->auto_open_distance=dialog_get_int(dialog_movement_settings_wind,kMovementAutoOpenDistance,0);
	movement->auto_open_stand=dialog_get_boolean(dialog_movement_settings_wind,kMovementAutoOpenStand,0);
	movement->auto_open_never_close=dialog_get_boolean(dialog_movement_settings_wind,kMovementAutoOpenNeverClose,0);
	movement->loop=dialog_get_boolean(dialog_movement_settings_wind,kMovementLoop,0);

		// close window
		
	DisposeDataBrowserItemDataUPP(list_moves_item_upp);
	DisposeDataBrowserItemNotificationUPP(list_moves_notify_upp);
	
	DisposeWindow(dialog_movement_settings_wind);
	
	return(TRUE);
}



