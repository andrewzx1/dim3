/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: File Open Dialogs

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

#include "dialog.h"

#define kFileOpenList				FOUR_CHAR_CODE('list')
#define kFileOpenListNameColumn		FOUR_CHAR_CODE('name')
#define kFileOpenButton				FOUR_CHAR_CODE('open')

int								fp_file_index;
char							fp_file_name[256];
bool							fp_cancel;
file_path_directory_type		*fpd;
WindowRef						dialog_file_open_wind;

extern file_path_setup_type		file_path_setup;

/* =======================================================

      File Open Event Handlers
      
======================================================= */

static pascal OSStatus file_open_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandOpen:
					QuitAppModalLoopForWindow(dialog_file_open_wind);
					return(noErr);
					
				case kHICommandCancel:
					fp_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_file_open_wind);
					return(noErr);
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      File List Event Handlers
      
======================================================= */

int file_open_list_build_items_for_parent(int parent_idx,DataBrowserItemID *items)
{
	int					n,count;
	
	count=0;
	
	for (n=0;n!=fpd->nfile;n++) {
		if (fpd->files[n].parent_idx==parent_idx) {
			items[count++]=n+1;
		}
	}

	return(count);
}

static pascal OSStatus file_open_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int				idx;
	CFStringRef		cfstr;
	
	switch (property) {

		case kFileOpenListNameColumn:
			idx=itemID-1;
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,fpd->files[idx].file_name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kDataBrowserItemIsContainerProperty:
			idx=itemID-1;
			SetDataBrowserItemDataBooleanValue(itemData,fpd->files[idx].is_dir);
			return(noErr);
						
	}

	return(errDataBrowserPropertyNotSupported);
}

static pascal void file_open_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	int					idx;
	bool				enable;
	UInt32				count;
	DataBrowserItemID	items[file_paths_max_directory_file];
	
	switch (message) {
	
		case kDataBrowserItemDoubleClicked:
			fp_file_index=itemID-1;
			if (!fpd->files[fp_file_index].is_dir) QuitAppModalLoopForWindow(dialog_file_open_wind);
			break;

		case kDataBrowserItemSelected:
			fp_file_index=itemID-1;
			break;
			
		case kDataBrowserItemDeselected:
			if (fp_file_index==(itemID-1)) fp_file_index=-1;
			break;
			
		case kDataBrowserContainerOpened:
			idx=itemID-1;
			count=file_open_list_build_items_for_parent(idx,items);
			AddDataBrowserItems(ctrl,itemID,count,items,kDataBrowserItemNoProperty);
			break;
	}
	
		// enable open button
		
	enable=FALSE;
	if (fp_file_index!=-1) {
		enable=!fpd->files[fp_file_index].is_dir;
	}

	dialog_enable(dialog_file_open_wind,kFileOpenButton,0,enable);
}

/* =======================================================

      Run File Open
      
======================================================= */

bool dialog_file_open_run(char *dialog_name,char *search_path,char *extension,char *required_file_name,char *file_name)
{
	int								n,count;
	CFStringRef						cfstr;
	ControlRef						ctrl;
	ControlID						ctrl_id;
	DataBrowserCallbacks			dbcall;
	EventHandlerUPP					event_upp;
	DataBrowserItemID				items[file_paths_max_directory_file];
	DataBrowserItemDataUPP			list_item_upp;
	DataBrowserItemNotificationUPP	list_notify_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_file_open_wind,"FileOpen");
	
	cfstr=CFStringCreateWithCString(kCFAllocatorDefault,dialog_name,kCFStringEncodingMacRoman);
	SetWindowTitleWithCFString(dialog_file_open_wind,cfstr);
	CFRelease(cfstr);
	
		// scan for files
		
	if (extension!=NULL) {
		fpd=file_paths_read_directory_data(&file_path_setup,search_path,extension);
	}
	else {
		fpd=file_paths_read_directory_data_dir(&file_path_setup,search_path,required_file_name);
	}
	
		// setup the list
		
	ctrl_id.signature=kFileOpenList;
	ctrl_id.id=0;
	GetControlByID(dialog_file_open_wind,&ctrl_id,&ctrl);
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	list_item_upp=NewDataBrowserItemDataUPP(&file_open_list_item_proc);
	dbcall.u.v1.itemDataCallback=list_item_upp;

	list_notify_upp=NewDataBrowserItemNotificationUPP(&file_open_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=list_notify_upp;
	
	SetDataBrowserCallbacks(ctrl,&dbcall);
	
	SetDataBrowserListViewDisclosureColumn(ctrl,kFileOpenListNameColumn,FALSE);

	count=file_open_list_build_items_for_parent(-1,items);
	AddDataBrowserItems(ctrl,kDataBrowserNoItem,count,items,kDataBrowserItemNoProperty);
	
		// always start with top level items open
		
	for (n=0;n!=count;n++) {
		OpenDataBrowserContainer(ctrl,items[n]);
	}
	
		// disable open
		
	dialog_enable(dialog_file_open_wind,kFileOpenButton,0,FALSE);
		
		// show window
	
	ShowWindow(dialog_file_open_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(file_open_event_proc);
	InstallWindowEventHandler(dialog_file_open_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	fp_cancel=FALSE;
	
	RunAppModalLoopForWindow(dialog_file_open_wind);
	
		// get selected file
		
	if (!fp_cancel) file_paths_get_complete_path_from_index(fpd,fp_file_index,file_name);
	
		// close window
		
	DisposeDataBrowserItemDataUPP(list_item_upp);
	DisposeDataBrowserItemNotificationUPP(list_notify_upp);
	DisposeWindow(dialog_file_open_wind);
	
		// clear up memory
		
	file_paths_close_directory(fpd);

	return(!fp_cancel);
}

