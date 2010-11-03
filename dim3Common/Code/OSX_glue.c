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

extern WindowRef				wind;
extern AGLContext				ctx;

char							os_load_file_ext[32];

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

/* =======================================================

      Menus
      
======================================================= */

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
	return((GetCurrentKeyModifiers()&shiftKey)!=0);
}

/* =======================================================

      Mouse
      
======================================================= */

bool os_button_down(void)
{
	return(Button());
}

bool os_track_mouse_location(d3pnt *pt,d3rect *offset_box)
{
	Point					uipt;
	MouseTrackingResult		track;

	TrackMouseLocation(NULL,&uipt,&track);
	
	if (offset_box==NULL) {
		pt->x=uipt.h;
		pt->y=uipt.v;
	}
	else {
		pt->x=uipt.h-offset_box->lx;
		pt->y=uipt.v-offset_box->ty;
	}
	
	return(track==kMouseTrackingMouseReleased);
}

/* =======================================================

      Alerts
      
======================================================= */

int dialog_alert(char *title,char *msg)
{
	CFStringRef			cf_title_str,cf_msg_str;
	CFOptionFlags		resp;

	os_set_arrow_cursor();
	
	cf_title_str=CFStringCreateWithCString(kCFAllocatorDefault,title,kCFStringEncodingMacRoman);
	cf_msg_str=CFStringCreateWithCString(kCFAllocatorDefault,msg,kCFStringEncodingMacRoman);

	CFUserNotificationDisplayAlert(0,kCFUserNotificationStopAlertLevel,NULL,NULL,NULL,cf_title_str,cf_msg_str,NULL,NULL,NULL,&resp);
	
	CFRelease(cf_title_str);
	CFRelease(cf_msg_str);
	
	return(resp);
}

int dialog_confirm(char *title,char *msg)
{
	CFStringRef			cf_title_str,cf_msg_str,
						cf_butt_1_str,cf_butt_2_str,cf_butt_3_str;
	CFOptionFlags		resp;

	os_set_arrow_cursor();
	
	cf_title_str=CFStringCreateWithCString(kCFAllocatorDefault,title,kCFStringEncodingMacRoman);
	cf_msg_str=CFStringCreateWithCString(kCFAllocatorDefault,msg,kCFStringEncodingMacRoman);

	cf_butt_1_str=CFStringCreateWithCString(kCFAllocatorDefault,"Yes",kCFStringEncodingMacRoman);
	cf_butt_2_str=CFStringCreateWithCString(kCFAllocatorDefault,"Cancel",kCFStringEncodingMacRoman);
	cf_butt_3_str=CFStringCreateWithCString(kCFAllocatorDefault,"No",kCFStringEncodingMacRoman);
	
	CFUserNotificationDisplayAlert(0,kCFUserNotificationNoteAlertLevel,NULL,NULL,NULL,cf_title_str,cf_msg_str,cf_butt_1_str,cf_butt_2_str,cf_butt_3_str,&resp);
	
	CFRelease(cf_butt_1_str);
	CFRelease(cf_butt_2_str);
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

bool os_load_file(char *path,char *ext)
{
    NavDialogCreationOptions	navoption;
    NavReplyRecord				navreply;
	NavEventUPP					navevent;
    NavObjectFilterUPP			navfilter;
	AEKeyword					keyword;
	DescType					typecode;
    Size						sz;
    NavDialogRef				diagref;
	FSRef						fsref;
	
	strcpy(os_load_file_ext,ext);
	
		// get the file
		
	NavGetDefaultDialogCreationOptions(&navoption);
	navoption.optionFlags-=kNavDontAddTranslateItems;
	navoption.optionFlags-=kNavAllowPreviews;

	navevent=NewNavEventUPP(os_load_file_event_proc);
	navfilter=NewNavObjectFilterUPP(os_load_file_filter);
	NavCreateGetFileDialog(&navoption,NULL,navevent,NULL,navfilter,NULL,&diagref);
	NavDialogRun(diagref);
 
 	NavDialogGetReply(diagref,&navreply);
	NavDialogDispose(diagref);
	DisposeNavEventUPP(navevent);
    DisposeNavObjectFilterUPP(navfilter);
    
	if (!navreply.validRecord) {
		NavDisposeReply(&navreply);
        return(FALSE);
    }
    
	AEGetNthPtr(&(navreply.selection),1,typeFSRef,&keyword,&typecode,(void*)&fsref,sizeof(FSRef),&sz);
    NavDisposeReply(&navreply);
	
    FSRefMakePath(&fsref,(unsigned char*)path,1024);
	
	return(TRUE);
}




