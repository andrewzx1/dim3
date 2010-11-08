/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Cinema Setting Dialog

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

#define kCinemaName							FOUR_CHAR_CODE('name')
#define kCinemaLength						FOUR_CHAR_CODE('clen')
#define kCinemaFreezeInput					FOUR_CHAR_CODE('fzin')
#define kCinemaShowHUD						FOUR_CHAR_CODE('swhd')

#define kActionList							FOUR_CHAR_CODE('tact')
#define kActionListTimeColumn				FOUR_CHAR_CODE('time')
#define kActionListActorColumn				FOUR_CHAR_CODE('actr')
#define kActionListActionColumn				FOUR_CHAR_CODE('actn')
#define kActionListNodeColumn				FOUR_CHAR_CODE('node')
#define kActionListAnimationColumn			FOUR_CHAR_CODE('anim')

#define kActionAddButton					FOUR_CHAR_CODE('aact')
#define kActionSortButton					FOUR_CHAR_CODE('asrt')
#define kActionDeleteButton					FOUR_CHAR_CODE('dact')

int							dialog_cinema_idx,
							dialog_action_idx;
bool						dialog_cinema_ignore_deselect;
WindowRef					dialog_cinema_settings_wind;

/* =======================================================

      Reset List/Buttons
      
======================================================= */

ControlRef actions_list_get_ctrl(void)
{
	ControlRef		ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=kActionList;
	ctrl_id.id=0;
	GetControlByID(dialog_cinema_settings_wind,&ctrl_id,&ctrl);

	return(ctrl);
}

void actions_list_reset(int action_idx)
{
	ControlRef			ctrl;
	DataBrowserItemID	itemID;
	
	dialog_action_idx=action_idx;
	
		// clear action list
		
	ctrl=actions_list_get_ctrl();
	
	dialog_cinema_ignore_deselect=TRUE;
	RemoveDataBrowserItems(ctrl,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	dialog_cinema_ignore_deselect=FALSE;
	
		// add in action list items
		
	AddDataBrowserItems(ctrl,kDataBrowserNoItem,map.cinema.cinemas[dialog_cinema_idx].naction,NULL,kDataBrowserItemNoProperty);
	
		// select action item
		
	if (dialog_action_idx==-1) return;
		
	itemID=dialog_action_idx+1;
	SetDataBrowserSelectedItems(ctrl,1,&itemID,kDataBrowserItemsAssign);
	RevealDataBrowserItem(ctrl,itemID,kDataBrowserNoItem,kDataBrowserRevealOnly);
}

void cinemas_reset_buttons(void)
{
	ControlRef		delete_action_ctrl;
	ControlID		ctrl_id;
	
	ctrl_id.signature=kActionDeleteButton;
	ctrl_id.id=0;
	GetControlByID(dialog_cinema_settings_wind,&ctrl_id,&delete_action_ctrl);
	
	if (dialog_action_idx==-1) {
		DisableControl(delete_action_ctrl);
	}
	else {
		EnableControl(delete_action_ctrl);
	}
}

/* =======================================================

      Map Cinemas Action Sorting
      
======================================================= */

void cinemas_action_sort(void)
{
	int						n,k,idx,count,sz;
	int						*sort_list;
	map_cinema_type			*cinema;
	map_cinema_action_type	*action,*a_ptr;
	
	cinema=&map.cinema.cinemas[dialog_cinema_idx];
	if (cinema->naction<=1) return;
	
		// sort the items
		
	sort_list=(int*)malloc((cinema->naction+1)*sizeof(int));
	if (sort_list==NULL) return;
	
	count=0;
	action=cinema->actions;
	
	for (n=0;n!=cinema->naction;n++) {
	
		idx=count;
		
		for (k=0;k!=count;k++) {
			if (cinema->actions[sort_list[k]].start_msec>action->start_msec) {
				idx=k;
				break;
			}
		}
		
		sz=(count-idx)*sizeof(int);
		if (sz>0) memmove(&sort_list[idx+1],&sort_list[idx],sz);
		
		sort_list[idx]=n;
		count++;

		action++;
	}
	
		// find the new index
		
	for (n=0;n!=cinema->naction;n++) {
		if (sort_list[n]==dialog_action_idx) {
			dialog_action_idx=n;
			break;
		}
	}
	
		// sort the list
		
	a_ptr=(map_cinema_action_type*)malloc(cinema->naction*sizeof(map_cinema_action_type));
	if (a_ptr==NULL) {
		free(sort_list);
		return;
	}
	
	for (n=0;n!=cinema->naction;n++) {
		memmove(&a_ptr[n],&cinema->actions[sort_list[n]],sizeof(map_cinema_action_type));
	}
	
	memmove(cinema->actions,a_ptr,(cinema->naction*sizeof(map_cinema_action_type)));
	free(a_ptr);
		
	free(sort_list);
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
			
				case kActionAddButton:
					idx=map_cinema_add_action(&map,dialog_cinema_idx);
					if (idx==-1) return(noErr);
					
					if (!dialog_cinema_action_settings_run(&map.cinema.cinemas[dialog_cinema_idx].actions[idx])) {
						map_cinema_delete_action(&map,dialog_cinema_idx,idx);
						return(noErr);
					}
					
					actions_list_reset(idx);
					Draw1Control(actions_list_get_ctrl());
					
					cinemas_reset_buttons();

					return(noErr);
					
				case kActionSortButton:
					cinemas_action_sort();
					
					actions_list_reset(dialog_action_idx);
					Draw1Control(actions_list_get_ctrl());
					
					cinemas_reset_buttons();

					return(noErr);
					
				case kActionDeleteButton:
					if (dialog_action_idx==-1) return(noErr);

					map_cinema_delete_action(&map,dialog_cinema_idx,dialog_action_idx);
					
					actions_list_reset(-1);
					Draw1Control(actions_list_get_ctrl());
					
					cinemas_reset_buttons();
					
					return(noErr);
				
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_cinema_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}

	return(eventNotHandledErr);
}

/* =======================================================

      Action List Event Handlers
      
======================================================= */

static pascal OSStatus actions_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int						action_idx;
	char					str[256];
	char					actor_type_str[][32]={"Camera","Player","Object","Movement","Particle","HUD Text","HUD Bitmap"},
							action_type_str[][32]={"None","Place","Move","Stop","Show","Hide"};
	map_cinema_action_type	*action;
	CFStringRef				cfstr;

	switch (property) {
		
		case kActionListTimeColumn:
			action_idx=itemID-1;
			
			action=&map.cinema.cinemas[dialog_cinema_idx].actions[action_idx];
			if (action->action==cinema_action_move) {
				sprintf(str,"%d - %d msec",action->start_msec,action->end_msec);
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
			action=&map.cinema.cinemas[dialog_cinema_idx].actions[action_idx];
			
			if ((action->actor_type!=cinema_actor_camera) && (action->actor_type!=cinema_actor_player)) {
				sprintf(str,"%s: %s",actor_type_str[action->actor_type],action->actor_name);
			}
			else {
				strcpy(str,actor_type_str[action->actor_type]);
			}
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kActionListActionColumn:
			action_idx=itemID-1;
			action=&map.cinema.cinemas[dialog_cinema_idx].actions[action_idx];
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,action_type_str[action->action],kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kActionListNodeColumn:
			action_idx=itemID-1;
			action=&map.cinema.cinemas[dialog_cinema_idx].actions[action_idx];
				
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,action->node_name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kActionListAnimationColumn:
			action_idx=itemID-1;
			action=&map.cinema.cinemas[dialog_cinema_idx].actions[action_idx];
			
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
			if (dialog_cinema_action_settings_run(&map.cinema.cinemas[dialog_cinema_idx].actions[action_idx])) {
				actions_list_reset(action_idx);
				Draw1Control(ctrl);
			}
			break;
			
		case kDataBrowserItemSelected:
			dialog_action_idx=itemID-1;
			cinemas_reset_buttons();
			break;

		case kDataBrowserItemDeselected:
			if (dialog_cinema_ignore_deselect) break;
			
			GetDataBrowserItemCount(ctrl,kDataBrowserNoItem,TRUE,kDataBrowserItemIsSelected,&count);
			if (count!=0) break;
			
			dialog_action_idx=-1;
			cinemas_reset_buttons();
			break;

	}
}

/* =======================================================

      Run Map Cinemas
      
======================================================= */

int dialog_cinema_settings_run(int cinema_idx)
{
	ControlRef						ctrl;
	DataBrowserCallbacks			dbcall_actions;
	EventHandlerUPP					event_upp;
	DataBrowserItemDataUPP			list_actions_item_upp;
	DataBrowserItemNotificationUPP	list_actions_notify_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	map_cinema_type					*cinema;
	
		// new or existing cinema?
		
	if (cinema_idx==-1) {
		dialog_cinema_idx=map_cinema_add(&map);
		if (dialog_cinema_idx==-1) {
			os_dialog_alert("Cinema","Out of Memory -- could not create a new cinema");
			return(-1);
		}
	}
	else {
		dialog_cinema_idx=cinema_idx;
	}
	
		// open the dialog
		
	dialog_open(&dialog_cinema_settings_wind,"CinemaSettings");
	
		// set controls
		
	cinema=&map.cinema.cinemas[dialog_cinema_idx];
		
	dialog_set_text(dialog_cinema_settings_wind,kCinemaName,0,cinema->name);
	dialog_set_int(dialog_cinema_settings_wind,kCinemaLength,0,cinema->len_msec);
	dialog_set_boolean(dialog_cinema_settings_wind,kCinemaFreezeInput,0,cinema->freeze_input);
	dialog_set_boolean(dialog_cinema_settings_wind,kCinemaShowHUD,0,cinema->show_hud);
	
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
	
	actions_list_reset(-1);
	cinemas_reset_buttons();
	
	dialog_set_focus(dialog_cinema_settings_wind,kCinemaName,0);

		// show window
	
	ShowWindow(dialog_cinema_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(cinemas_event_proc);
	InstallWindowEventHandler(dialog_cinema_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	RunAppModalLoopForWindow(dialog_cinema_settings_wind);
	
		// get controls
		
	dialog_get_text(dialog_cinema_settings_wind,kCinemaName,0,cinema->name,name_str_len);
	cinema->len_msec=dialog_get_int(dialog_cinema_settings_wind,kCinemaLength,0);
	cinema->freeze_input=dialog_get_boolean(dialog_cinema_settings_wind,kCinemaFreezeInput,0);
	cinema->show_hud=dialog_get_boolean(dialog_cinema_settings_wind,kCinemaShowHUD,0);
	
		// close window
		
	DisposeDataBrowserItemDataUPP(list_actions_item_upp);
	DisposeDataBrowserItemNotificationUPP(list_actions_notify_upp);
	
	DisposeWindow(dialog_cinema_settings_wind);
	
	return(dialog_cinema_idx);
}



