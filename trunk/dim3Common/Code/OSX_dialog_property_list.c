/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: List Properties

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

#include "glue.h"
#include "interface.h"
#include "ui_common.h"
#include "osx_dialog.h"

#define kPropertyList				FOUR_CHAR_CODE('list')
#define kPropertyListNameColumn		FOUR_CHAR_CODE('name')

#define kPropertyListNone			FOUR_CHAR_CODE('none')

int								dialog_property_list_item_sz,dialog_property_list_name_offset,
								dialog_property_idx;
char							*dialog_property_list_ptr;
WindowRef						dialog_property_list_wind;

/* =======================================================

      Property List Event Handlers
      
======================================================= */

static pascal OSStatus property_list_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kPropertyListNone:
					dialog_property_idx=-1;
					QuitAppModalLoopForWindow(dialog_property_list_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_property_list_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Property List Event Handlers
      
======================================================= */

static pascal OSStatus property_list_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int				idx;
	char			*name;
	CFStringRef		cfstr;
	
	switch (property) {

		case kPropertyListNameColumn:
			idx=itemID-1;
			name=(dialog_property_list_ptr+(idx*dialog_property_list_item_sz))+dialog_property_list_name_offset;
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
						
	}

	return(errDataBrowserPropertyNotSupported);
}

static pascal void property_list_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	switch (message) {
	
		case kDataBrowserItemDoubleClicked:
			dialog_property_idx=itemID-1;
			QuitAppModalLoopForWindow(dialog_property_list_wind);
			break;

		case kDataBrowserItemSelected:
			dialog_property_idx=itemID-1;
			break;

	}
}

/* =======================================================

      Run Property List
      
======================================================= */

void dialog_property_list_run(char *title,char *list_ptr,int list_count,int list_item_sz,int list_name_offset,bool include_none,int *idx)
{
	ControlRef						ctrl;
	ControlID						ctrl_id;
	DataBrowserCallbacks			dbcall;
	EventHandlerUPP					event_upp;
	DataBrowserItemID				itemID;
	DataBrowserItemDataUPP			list_item_upp;
	DataBrowserItemNotificationUPP	list_notify_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// remember the list
		
	dialog_property_list_ptr=list_ptr;
	dialog_property_list_item_sz=list_item_sz;
	dialog_property_list_name_offset=list_name_offset;
	
		// open the dialog
		
	dialog_open(&dialog_property_list_wind,"PropertyList");
	dialog_set_title(dialog_property_list_wind,title);
	
		// setup the list
		
	ctrl_id.signature=kPropertyList;
	ctrl_id.id=0;
	GetControlByID(dialog_property_list_wind,&ctrl_id,&ctrl);
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	list_item_upp=NewDataBrowserItemDataUPP(&property_list_list_item_proc);
	dbcall.u.v1.itemDataCallback=list_item_upp;

	list_notify_upp=NewDataBrowserItemNotificationUPP(&property_list_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=list_notify_upp;
	
	SetDataBrowserCallbacks(ctrl,&dbcall);

	AddDataBrowserItems(ctrl,kDataBrowserNoItem,list_count,NULL,kDataBrowserItemNoProperty);
	
		// selected item
		
	if ((*idx)!=-1) {
		itemID=(*idx)+1;
		SetDataBrowserSelectedItems(ctrl,1,&itemID,kDataBrowserItemsAssign);
		RevealDataBrowserItem(ctrl,itemID,kDataBrowserNoItem,kDataBrowserRevealOnly);
	}
	
		// none control
		
	dialog_enable(dialog_property_list_wind,kPropertyListNone,0,include_none);
		
		// show window
	
	ShowWindow(dialog_property_list_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(property_list_event_proc);
	InstallWindowEventHandler(dialog_property_list_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_property_idx=*idx;
	
	RunAppModalLoopForWindow(dialog_property_list_wind);
	
		// close window
		
	DisposeDataBrowserItemDataUPP(list_item_upp);
	DisposeDataBrowserItemNotificationUPP(list_notify_upp);
	DisposeWindow(dialog_property_list_wind);
	
		// return value
		
	*idx=dialog_property_idx;
}

