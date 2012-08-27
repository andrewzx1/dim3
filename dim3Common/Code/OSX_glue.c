/****************************** File *********************************

Module: Common
Author: Brian Barnes
 Usage: OS X Glue Code

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

#include "glue.h"

extern WindowRef				wind;
extern AGLContext				ctx;

char							os_load_file_ext[32];
EventHandlerRef					os_dialog_wind_event;
EventHandlerUPP					os_dialog_wind_upp;
ControlRef						os_dialog_ctrls[32];
WindowRef						os_dialog_wind;

int								os_dialog_ctrl_count,dialog_file_open_index;
bool							os_dialog_ok;
DataBrowserItemDataUPP			os_dialog_files_item_upp;
DataBrowserItemNotificationUPP	os_dialog_files_notify_upp;
file_path_directory_type		*os_dialog_fpd;
os_dialog_callback_ptr			os_dialog_callback;

/* =======================================================

      Glue Start and End
      
======================================================= */

void os_glue_start(void)
{
}

void os_glue_end(void)
{
}

/* =======================================================

      Paths and Directories
      
======================================================= */

void os_get_support_file_path(char *path,char *app_name)
{
	strcpy(path,"Contents/Resources");
}

void os_create_directory(char *path)
{
   mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);
}

/* =======================================================

      Windows
      
======================================================= */

void os_application_quit(void)
{
	QuitApplicationEventLoop();
}

void os_get_window_box(d3rect *box)
{
	Rect			wbox;
	
	GetWindowPortBounds(wind,&wbox);

	box->lx=wbox.left;
	box->rx=wbox.right;
	box->ty=wbox.top;
	box->by=wbox.bottom;
}

void os_select_window(void)
{
   SelectWindow(wind);
}

void os_set_title_window(char *title)
{
	unsigned char	p_str[256];
	
	CopyCStringToPascal(title,p_str);
	SetWTitle(wind,p_str);
}

void os_swap_gl_buffer(void)
{
	aglSwapBuffers(ctx);
}

/* =======================================================

      Cursors
      
======================================================= */

void os_set_arrow_cursor(void)
{
	SetThemeCursor(kThemeArrowCursor);
}

void os_set_wait_cursor(void)
{
	SetThemeCursor(kThemeWatchCursor);
}

void os_set_hand_cursor(void)
{
   SetThemeCursor(kThemeOpenHandCursor);
}

void os_set_drag_cursor(void)
{
   SetThemeCursor(kThemeClosedHandCursor);
}

void os_set_resize_cursor(void)
{
   SetThemeCursor(kThemeResizeUpDownCursor);
}

void os_set_add_cursor(void)
{
	SetThemeCursor(kThemePlusCursor);
}

void os_set_subtract_cursor(void)
{
	SetThemeCursor(kThemePoofCursor);
}

/* =======================================================

      Menus
      
======================================================= */

void os_menu_create(os_menu_item_type *os_menus)
{

	int						last_idx,menu_id;
	char					last_menu_name[32];
	os_menu_item_type		*menu;
	MenuRef					sub_menu;
	CFStringRef				cf_str;
		
		// clear old menus
		
	ClearMenuBar();

		// create the menu

	menu=os_menus;
	
	last_idx=0;
	menu_id=app_menu_file;
	last_menu_name[0]=0x0;

	while (TRUE) {

		if (menu->menu_name[0]==0x0) break;

			// switching to a new menu

		if (strcmp(menu->menu_name,last_menu_name)!=0) {
			if (last_menu_name[0]!=0x0) InsertMenu(sub_menu,0);
			
			CreateNewMenu(menu_id,0,&sub_menu);
			menu_id++;
			
			cf_str=CFStringCreateWithCString(kCFAllocatorDefault,menu->menu_name,kCFStringEncodingMacRoman);
			SetMenuTitleWithCFString(sub_menu,cf_str);
			CFRelease(cf_str);

			last_idx=0;
			strcpy(last_menu_name,menu->menu_name);
		}

			// add item

		if (menu->item_name[0]!=0x0) {
		
			cf_str=CFStringCreateWithCString(kCFAllocatorDefault,menu->item_name,kCFStringEncodingMacRoman);
			InsertMenuItemTextWithCFString(sub_menu,cf_str,last_idx,0,menu->id);
			CFRelease(cf_str);

				// add in cmd/opt/shift keys

			switch (menu->key_type) {
			
				case os_menu_key_cmd:
					SetMenuItemCommandKey(sub_menu,(last_idx+1),FALSE,menu->key);
					SetMenuItemModifiers(sub_menu,(last_idx+1),kMenuNoModifiers);
					break;

				case os_menu_key_cmd_opt:
					SetMenuItemCommandKey(sub_menu,(last_idx+1),FALSE,menu->key);
					SetMenuItemModifiers(sub_menu,(last_idx+1),kMenuOptionModifier);
					break;

				case os_menu_key_cmd_shift:
					SetMenuItemCommandKey(sub_menu,(last_idx+1),FALSE,menu->key);
					SetMenuItemModifiers(sub_menu,(last_idx+1),kMenuShiftModifier);
					break;
			}

		}

		else {
			InsertMenuItemTextWithCFString(sub_menu,NULL,last_idx,kMenuItemAttrSeparator,0);
		}
		
		last_idx++;

			// next menu item

		menu++;
	}
	
	if (last_menu_name[0]!=0x0) InsertMenu(sub_menu,0);
}

void os_menu_dispose(void)
{
	ClearMenuBar();
}

void os_menu_enable_item(int menu_idx,int item_idx,bool enable)
{
	if (enable) {
		EnableMenuItem(GetMenuRef(menu_idx),item_idx);
	}
	else {
		DisableMenuItem(GetMenuRef(menu_idx),item_idx);
	}
}

void os_menu_check_item(int menu_idx,int item_idx,bool check)
{
	CheckMenuItem(GetMenuRef(menu_idx),item_idx,check);
}

void os_menu_redraw(void)
{
	DrawMenuBar();
}

/* =======================================================

      Keys
      
======================================================= */

bool os_key_space_down(void)
{
	KeyMap			map;
	unsigned char	*c;
	
	GetKeys(map);
	c=(unsigned char*)map;
	
	return((c[6]&0x02)!=0);
}

bool os_key_option_down(void)
{
	return((GetCurrentKeyModifiers()&optionKey)!=0);
}

bool os_key_control_down(void)
{
	return((GetCurrentKeyModifiers()&controlKey)!=0);
}

bool os_key_command_down(void)
{
	return((GetCurrentKeyModifiers()&cmdKey)!=0);
}

bool os_key_shift_down(void)
{
	return(((GetCurrentKeyModifiers()&shiftKey)!=0)||((GetCurrentKeyModifiers()&alphaLock)!=0));
}

/* =======================================================

      Mouse
      
======================================================= */

void os_get_cursor(d3pnt *pnt)
{
	Point			ui_pt;
	
	GetMouse(&ui_pt);
	GlobalToLocal(&ui_pt);

	pnt->x=ui_pt.h;
	pnt->y=ui_pt.v;
}

bool os_button_down(void)
{
	return(Button());
}

bool os_track_mouse_location(d3pnt *pnt,d3rect *offset_box)
{
	Point					ui_pt;
	MouseTrackingResult		track;

	TrackMouseLocation(NULL,&ui_pt,&track);
	
	if (offset_box==NULL) {
		pnt->x=ui_pt.h;
		pnt->y=ui_pt.v;
	}
	else {
		pnt->x=ui_pt.h-offset_box->lx;
		pnt->y=ui_pt.v-offset_box->ty;
	}
	
	return(track==kMouseTrackingMouseReleased);
}

/* =======================================================

      Alerts
      
======================================================= */

void os_dialog_alert(char *title,char *msg)
{
	CFStringRef			cf_title_str,cf_msg_str;
	CFOptionFlags		resp;

	os_set_arrow_cursor();
	
	cf_title_str=CFStringCreateWithCString(kCFAllocatorDefault,title,kCFStringEncodingMacRoman);
	cf_msg_str=CFStringCreateWithCString(kCFAllocatorDefault,msg,kCFStringEncodingMacRoman);

	CFUserNotificationDisplayAlert(0,kCFUserNotificationStopAlertLevel,NULL,NULL,NULL,cf_title_str,cf_msg_str,NULL,NULL,NULL,&resp);
	
	CFRelease(cf_title_str);
	CFRelease(cf_msg_str);
}

int os_dialog_confirm(char *title,char *msg,bool include_cancel)
{
	CFStringRef			cf_title_str,cf_msg_str,
						cf_butt_1_str,cf_butt_2_str,cf_butt_3_str;
	CFOptionFlags		resp;

	os_set_arrow_cursor();
	
	cf_title_str=CFStringCreateWithCString(kCFAllocatorDefault,title,kCFStringEncodingMacRoman);
	cf_msg_str=CFStringCreateWithCString(kCFAllocatorDefault,msg,kCFStringEncodingMacRoman);

	cf_butt_1_str=CFStringCreateWithCString(kCFAllocatorDefault,"Yes",kCFStringEncodingMacRoman);
	if (include_cancel) {
		cf_butt_2_str=CFStringCreateWithCString(kCFAllocatorDefault,"Cancel",kCFStringEncodingMacRoman);
	}
	else {
		cf_butt_2_str=NULL;
	}
	cf_butt_3_str=CFStringCreateWithCString(kCFAllocatorDefault,"No",kCFStringEncodingMacRoman);
	
	CFUserNotificationDisplayAlert(0,kCFUserNotificationNoteAlertLevel,NULL,NULL,NULL,cf_title_str,cf_msg_str,cf_butt_1_str,cf_butt_2_str,cf_butt_3_str,&resp);
	
	CFRelease(cf_butt_1_str);
	if (include_cancel) CFRelease(cf_butt_2_str);
	CFRelease(cf_butt_3_str);
	
	CFRelease(cf_title_str);
	CFRelease(cf_msg_str);
	
	return(resp);
}

/* =======================================================

      Load a File
      
======================================================= */

void os_load_file_event_proc(const NavEventCallbackMessage callBackSelector,NavCBRecPtr callBackParms,NavCallBackUserData callBackUD)
{
}

Boolean os_load_file_filter(AEDesc *theItem,void *info,void *callBackUD,NavFilterModes filterMode)
{
    char					*c,filename[256];
    NavFileOrFolderInfo		*filefolder;
    AEDesc					desc;
	HFSUniStr255			uniname;
    CFStringRef				cfstr;
    FSRef					fref;
    
    if ((theItem->descriptorType!=typeFSS) && (theItem->descriptorType!=typeFSRef)) return(FALSE);

    filefolder=(NavFileOrFolderInfo*)info;
    if (filefolder->isFolder) return(TRUE);

	AECoerceDesc(theItem,typeFSRef,&desc);
	AEGetDescData(&desc,(void*)&fref,sizeof(FSRef));
    AEDisposeDesc(&desc);

    FSGetCatalogInfo(&fref,kFSCatInfoNone,NULL,&uniname,NULL,NULL);
	cfstr=CFStringCreateWithCharacters(kCFAllocatorDefault,uniname.unicode,uniname.length);
	CFStringGetCString(cfstr,filename,256,kCFStringEncodingMacRoman);
    CFRelease(cfstr);
    
    c=strchr(filename,'.');
    if (c==NULL) return(FALSE);
    
    return(strcasecmp((c+1),os_load_file_ext)==0);
}

bool os_load_file(char *title,char *path,char *ext)
{
    NavDialogCreationOptions	navoption;
    NavReplyRecord				navreply;
	NavEventUPP					navevent;
    NavObjectFilterUPP			navfilter;
	AEKeyword					keyword;
	DescType					typecode;
    Size						sz;
	CFStringRef					cf_title_str;
    NavDialogRef				diagref;
	FSRef						fsref;
	
	strcpy(os_load_file_ext,ext);
	
		// get the file
		
	NavGetDefaultDialogCreationOptions(&navoption);
	navoption.optionFlags-=kNavDontAddTranslateItems;
	navoption.optionFlags-=kNavAllowPreviews;
	
	cf_title_str=CFStringCreateWithCString(kCFAllocatorDefault,title,kCFStringEncodingMacRoman);
	navoption.windowTitle=cf_title_str;

	navevent=NewNavEventUPP(os_load_file_event_proc);
	navfilter=NewNavObjectFilterUPP(os_load_file_filter);
	NavCreateGetFileDialog(&navoption,NULL,navevent,NULL,navfilter,NULL,&diagref);
	NavDialogRun(diagref);
 
 	NavDialogGetReply(diagref,&navreply);
	NavDialogDispose(diagref);
	DisposeNavEventUPP(navevent);
    DisposeNavObjectFilterUPP(navfilter);
  
	CFRelease(cf_title_str);

	if (!navreply.validRecord) {
		NavDisposeReply(&navreply);
        return(FALSE);
    }
    
	AEGetNthPtr(&(navreply.selection),1,typeFSRef,&keyword,&typecode,(void*)&fsref,sizeof(FSRef),&sz);
    NavDisposeReply(&navreply);
	
    FSRefMakePath(&fsref,(unsigned char*)path,1024);
	
	return(TRUE);
}

/* =======================================================

      Color Picker
      
======================================================= */

void os_pick_color(d3col *col)
{
	RGBColor		color;
	Point			pt;
	
	color.red=(int)(col->r*(float)0xFFFF);
	color.green=(int)(col->g*(float)0xFFFF);
	color.blue=(int)(col->b*(float)0xFFFF);
	
	pt.h=pt.v=-1;
	
	if (GetColor(pt,"\pChoose the Light Color:",&color,&color)) {
		col->r=((float)color.red/(float)0xFFFF);
		col->g=((float)color.green/(float)0xFFFF);
		col->b=((float)color.blue/(float)0xFFFF);
	}
}

/* =======================================================

      Launch Process
      
======================================================= */

bool os_launch_process(char *path,bool text_editor)
{
	FSRef					fsref;
	CFURLRef				cf_url;

	if (FSPathMakeRef((unsigned char*)path,&fsref,NULL)!=0) return(FALSE);
	
	cf_url=CFURLCreateFromFSRef(kCFAllocatorDefault,&fsref);
	LSOpenCFURLRef(cf_url,NULL);
	CFRelease(cf_url);

	return(TRUE);
}

/* =======================================================

      Dialogs
      
======================================================= */

OSStatus os_dialog_wind_event_callback(EventHandlerCallRef eventhandler,EventRef event,void *userdata)
{
	HICommand			cmd;
	
	switch (GetEventClass(event)) {
	
		case kEventClassCommand:
		
			switch (GetEventKind(event)) {
			
                case kEventCommandProcess:
					GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
					(*os_dialog_callback)(os_dialog_msg_type_command,(int)cmd.commandID);
                    return(noErr);

			}
	}
	
	return(eventNotHandledErr);
}

bool os_dialog_run(char *title,int wid,int high,os_dialog_ctrl_type *ctrls,void *callback)
{
	int								idx;
	unsigned char					p_str[256];
	os_dialog_ctrl_type				*ctrl;
	HIRect							wbox;
	Rect							box;
	MenuRef							ctrl_menu;
	CFStringRef						cf_str;
	ControlFontStyleRec				font_rec;
	DataBrowserListViewColumnDesc	db_col;
	EventTypeSpec					diag_events[]={{kEventClassCommand,kEventCommandProcess}};
	
		// setup callback

	os_dialog_callback=callback;
	
		// create the window
		
	HIWindowGetAvailablePositioningBounds(CGMainDisplayID(),kHICoordSpaceScreenPixel,&wbox);
	
	box.left=(wbox.origin.x+(wbox.size.width/2))-(wid/2);
	box.top=(wbox.origin.y+(wbox.size.height/2))-(high/2);
	box.right=box.left+wid;
	box.bottom=box.top+high;

	CreateNewWindow(kMovableModalWindowClass,kWindowStandardHandlerAttribute,&box,&os_dialog_wind);
	
	CopyCStringToPascal(title,p_str);
	SetWTitle(os_dialog_wind,p_str);
	
		// remember if we've added
		// a file browser
		
	os_dialog_files_item_upp=NULL;
	os_dialog_files_notify_upp=NULL;
	
		// add controls
		
	idx=0;
	
	while (TRUE) {
		ctrl=&ctrls[idx];
		if (ctrl->type==-1) break;
		
		box.left=ctrl->x;
		box.top=ctrl->y;
		box.right=box.left+ctrl->wid;
		box.bottom=box.top+ctrl->high;
		
		cf_str=CFStringCreateWithCString(kCFAllocatorDefault,ctrl->str,kCFStringEncodingMacRoman);

		switch (ctrl->type) {

			case os_dialog_ctrl_type_button:
				CreatePushButtonControl(os_dialog_wind,&box,cf_str,&os_dialog_ctrls[idx]);
				break;
				
			case os_dialog_ctrl_type_default_button:
				CreatePushButtonControl(os_dialog_wind,&box,cf_str,&os_dialog_ctrls[idx]);
				SetWindowDefaultButton(os_dialog_wind,os_dialog_ctrls[idx]);
				break;
				
			case os_dialog_ctrl_type_text_left:
				box.bottom=box.top+16;
				CreateStaticTextControl(os_dialog_wind,&box,cf_str,NULL,&os_dialog_ctrls[idx]);
				break;
			
			case os_dialog_ctrl_type_text_right:
				box.bottom=box.top+16;
				font_rec.flags=kControlUseJustMask;
				font_rec.just=-1;
				CreateStaticTextControl(os_dialog_wind,&box,cf_str,&font_rec,&os_dialog_ctrls[idx]);
				break;
				
			case os_dialog_ctrl_type_text_edit:
				box.left+=5;
				box.right+=5;
				box.bottom=box.top+16;
				CreateEditUnicodeTextControl(os_dialog_wind,&box,cf_str,FALSE,NULL,&os_dialog_ctrls[idx]);
				break;
				
			case os_dialog_ctrl_type_combo:
				box.bottom=box.top+20;
				CreateNewMenu(ctrl->id,0,&ctrl_menu);
				AddResource((char**)ctrl_menu,'MENU',ctrl->id,"\pMenu");
				CreatePopupButtonControl(os_dialog_wind,&box,CFSTR(""),ctrl->id,FALSE,0,teFlushDefault,0,&os_dialog_ctrls[idx]);
				SetControlPopupMenuHandle(os_dialog_ctrls[idx],ctrl_menu);
 				break;
				
			case os_dialog_ctrl_type_checkbox:
				box.bottom=box.top+16;
				CreateCheckBoxControl(os_dialog_wind,&box,cf_str,0,TRUE,&os_dialog_ctrls[idx]);
				break;
				
			case os_dialog_ctrl_type_files:
				CreateDataBrowserControl(os_dialog_wind,&box,kDataBrowserListView,&os_dialog_ctrls[idx]);
				SetDataBrowserSelectionFlags(os_dialog_ctrls[idx],kDataBrowserSelectOnlyOne);
				
				db_col.headerBtnDesc.version=kDataBrowserListViewLatestHeaderDesc;
				db_col.headerBtnDesc.minimumWidth=ctrl->wid-20;
				db_col.headerBtnDesc.maximumWidth=ctrl->wid-20;
				db_col.headerBtnDesc.titleOffset=0;
				db_col.headerBtnDesc.titleString=CFSTR("Files");
				db_col.headerBtnDesc.initialOrder=kDataBrowserOrderIncreasing;
				db_col.headerBtnDesc.btnFontStyle.flags=0;
				db_col.headerBtnDesc.btnContentInfo.contentType=kControlNoContent;
				
				db_col.propertyDesc.propertyID=FOUR_CHAR_CODE('list');
				db_col.propertyDesc.propertyType=kDataBrowserTextType;
				db_col.propertyDesc.propertyFlags=kDataBrowserListViewSelectionColumn;
				
				AddDataBrowserListViewColumn(os_dialog_ctrls[idx],&db_col,0);
				break;
	
		}
		
		SetControlCommandID(os_dialog_ctrls[idx],ctrl->id);
		
		CFRelease(cf_str);
	
		idx++;
	}
	
	os_dialog_ctrl_count=idx;
	
		// init dialog
		
	(*os_dialog_callback)(os_dialog_msg_type_init,0);
	
		// show window
		
	ShowWindow(os_dialog_wind);
	
		// run events
		
	os_dialog_wind_upp=NewEventHandlerUPP(os_dialog_wind_event_callback);
	InstallEventHandler(GetWindowEventTarget(os_dialog_wind),os_dialog_wind_upp,GetEventTypeCount(diag_events),diag_events,NULL,&os_dialog_wind_event);
		
	RunAppModalLoopForWindow(os_dialog_wind);
	
	RemoveEventHandler(os_dialog_wind_event);
	DisposeEventHandlerUPP(os_dialog_wind_upp);
	
		// if there was a file list,
		// then dispose that
		
	if (os_dialog_files_item_upp!=NULL)  DisposeDataBrowserItemDataUPP(os_dialog_files_item_upp);
	if (os_dialog_files_notify_upp!=NULL) DisposeDataBrowserItemNotificationUPP(os_dialog_files_notify_upp);
	
		// dispose window
		
	DisposeWindow(os_dialog_wind);
	
	return(os_dialog_ok);
}

void os_dialog_close(bool ok)
{
	os_dialog_ok=ok;
	
	QuitAppModalLoopForWindow(os_dialog_wind);
}

/* =======================================================

      Dialog Edit Controls
      
======================================================= */

ControlRef os_dialog_get_control_ref_from_id(int id)
{
	int				n;
	UInt32			ctrl_id;
	
	for (n=0;n!=os_dialog_ctrl_count;n++) {
		GetControlCommandID(os_dialog_ctrls[n],&ctrl_id);
		if (((int)ctrl_id)==id) return(os_dialog_ctrls[n]);
	}
	
	return(os_dialog_ctrls[0]);
}

void os_dialog_set_text(int id,char *value)
{
	if (value!=NULL) {
		SetControlData(os_dialog_get_control_ref_from_id(id),kControlNoPart,kControlEditTextTextTag,strlen(value),value);
	}
	else {
		SetControlData(os_dialog_get_control_ref_from_id(id),kControlNoPart,kControlEditTextTextTag,0,NULL);
	}
}

void os_dialog_get_text(int id,char *value,int value_len)
{
	memset(value,0x0,value_len);
	GetControlData(os_dialog_get_control_ref_from_id(id),kControlNoPart,kControlEditTextTextTag,value_len,value,NULL);
	value[value_len-1]=0x0;
}

void os_dialog_set_int(int id,int i)
{
	char			str[256];

	sprintf(str,"%d",i);
	os_dialog_set_text(id,str);
}

int os_dialog_get_int(int id)
{
	char			str[256];

	os_dialog_get_text(id,str,256);
	return(atoi(str));
}

void os_dialog_set_float(int id,float f)
{
	char			str[256];

	sprintf(str,"%.2f",f);
	os_dialog_set_text(id,str);
}

float os_dialog_get_float(int id)
{
	char			str[256];

	os_dialog_get_text(id,str,256);
	return((float)atof(str));
}

void os_dialog_set_bool(int id,bool value)
{
	SetControl32BitValue(os_dialog_get_control_ref_from_id(id),(value?1:0));
}

bool os_dialog_get_bool(int id)
{
	return(GetControl32BitValue(os_dialog_get_control_ref_from_id(id))!=0);
}

/* =======================================================

      Dialog Combo Controls
      
======================================================= */

void os_dialog_combo_clear(int id)
{
	ControlRef		ctrl;
	MenuRef			menu;
	
	ctrl=os_dialog_get_control_ref_from_id(id);
	menu=GetControlPopupMenuHandle(ctrl);
	
	DeleteMenuItems(menu,1,CountMenuItems(menu));
	SetControl32BitMaximum(ctrl,0);
}

void os_dialog_combo_add(int id,char *str)
{
	int				cnt;
	ControlRef		ctrl;
	MenuRef			menu;
	CFStringRef		cf_str;
	
	ctrl=os_dialog_get_control_ref_from_id(id);
	menu=GetControlPopupMenuHandle(ctrl);
	
	cnt=CountMenuItems(menu);
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
	AppendMenuItemTextWithCFString(menu,cf_str,0,(6000+cnt),NULL);
	CFRelease(cf_str);
	
	SetControl32BitMaximum(ctrl,(cnt+1));
}

void os_dialog_combo_set_value(int id,int value)
{
	ControlRef		ctrl;
	
	ctrl=os_dialog_get_control_ref_from_id(id);
	SetControl32BitValue(ctrl,(value+1));
}

int os_dialog_combo_get_value(int id)
{
	ControlRef		ctrl;
	
	ctrl=os_dialog_get_control_ref_from_id(id);	
	return(GetControl32BitValue(ctrl)-1);
}

/* =======================================================

      Dialog File Controls
      
======================================================= */

int file_open_list_build_items_for_parent(int parent_idx,DataBrowserItemID *items)
{
	int					n,count;
	
	count=0;
	
	for (n=0;n!=os_dialog_fpd->nfile;n++) {
		if (os_dialog_fpd->files[n].parent_idx==parent_idx) {
			items[count++]=n+1;
		}
	}

	return(count);
}

static pascal OSStatus os_dialog_files_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int				idx;
	CFStringRef		cfstr;
	
	switch (property) {

		case FOUR_CHAR_CODE('list'):
			idx=itemID-1;
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,os_dialog_fpd->files[idx].file_name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kDataBrowserItemIsContainerProperty:
			idx=itemID-1;
			SetDataBrowserItemDataBooleanValue(itemData,os_dialog_fpd->files[idx].is_dir);
			return(noErr);
						
	}

	return(errDataBrowserPropertyNotSupported);
}

static pascal void os_dialog_files_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	int					idx;
	UInt32				count;
	DataBrowserItemID	items[file_paths_max_directory_file];
	
	switch (message) {
	
		case kDataBrowserItemDoubleClicked:
			dialog_file_open_index=itemID-1;
			if (!os_dialog_fpd->files[dialog_file_open_index].is_dir) (*os_dialog_callback)(os_dialog_msg_type_tree_double_click,dialog_file_open_index);
			break;

		case kDataBrowserItemSelected:
			dialog_file_open_index=itemID-1;
			(*os_dialog_callback)(os_dialog_msg_type_tree_change,dialog_file_open_index);
			break;
			
		case kDataBrowserItemDeselected:
			if (dialog_file_open_index==(itemID-1)) {
				dialog_file_open_index=-1;
				(*os_dialog_callback)(os_dialog_msg_type_tree_change,dialog_file_open_index);
			}
			break;
			
		case kDataBrowserContainerOpened:
			idx=itemID-1;
			count=file_open_list_build_items_for_parent(idx,items);
			AddDataBrowserItems(ctrl,itemID,count,items,kDataBrowserItemNoProperty);
			break;
	}
}

void os_dialog_tree_add(int id,file_path_directory_type *fpd)
{
	int							n,count;
	ControlRef					ctrl;
	DataBrowserItemID			items[file_paths_max_directory_file];
	DataBrowserCallbacks		dbcall;
	
		// remember list
		
	os_dialog_fpd=fpd;

		// setup the list
		
	ctrl=os_dialog_get_control_ref_from_id(id);
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	os_dialog_files_item_upp=NewDataBrowserItemDataUPP(&os_dialog_files_item_proc);
	dbcall.u.v1.itemDataCallback=os_dialog_files_item_upp;

	os_dialog_files_notify_upp=NewDataBrowserItemNotificationUPP(&os_dialog_files_notify_proc);
	dbcall.u.v1.itemNotificationCallback=os_dialog_files_notify_upp;
	
	SetDataBrowserCallbacks(ctrl,&dbcall);
	
	SetDataBrowserListViewDisclosureColumn(ctrl,FOUR_CHAR_CODE('list'),FALSE);

	count=file_open_list_build_items_for_parent(-1,items);
	AddDataBrowserItems(ctrl,kDataBrowserNoItem,count,items,kDataBrowserItemNoProperty);
	
		// always start with top level items open
		
	for (n=0;n!=count;n++) {
		OpenDataBrowserContainer(ctrl,items[n]);
	}
	
	dialog_file_open_index=-1;
}

int os_dialog_tree_get_value(int id)
{
	return(dialog_file_open_index);
}

/* =======================================================

      Dialog Misc
      
======================================================= */

void os_dialog_set_focus(int id,bool select_all)
{
	SetKeyboardFocus(os_dialog_wind,os_dialog_get_control_ref_from_id(id),kControlFocusNextPart);
}

void os_dialog_enable(int id,bool enable)
{
	if (enable) {
		EnableControl(os_dialog_get_control_ref_from_id(id));
	}
	else {
		DisableControl(os_dialog_get_control_ref_from_id(id));
	}
}



