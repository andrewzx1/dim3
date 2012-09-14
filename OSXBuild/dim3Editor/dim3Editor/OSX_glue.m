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

#import "AppDelegate.h"
#import "View.h"

#include "glue.h"

char							os_load_file_ext[32];

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

void os_get_support_file_path(char *path)
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
	[NSApp terminate:nil];
}

inline NSWindow* os_cocoa_get_window()
{
#ifndef D3_OS_IPHONE
	return([[NSApp delegate] window]);
#else
	return([[UIApplication sharedApplication].delegate window]);
#endif
}

inline View* os_cocoa_get_view()
{
	NSWindow		*wind;
	
	wind=os_cocoa_get_window();
	return([wind contentView]);
}

void os_get_window_box(d3rect *box)
{
	View			*view;
	NSRect			rect;
	
	view=os_cocoa_get_view();
	rect=[view frame];
	
	box->lx=(int)rect.origin.x;
	box->ty=(int)rect.origin.y;
	box->rx=box->lx+(int)rect.size.width;
	box->by=box->ty+(int)rect.size.height;
}

void os_select_window(void)
{
	NSWindow		*wind;
	
	wind=os_cocoa_get_window();
	[wind makeKeyAndOrderFront:nil];
}

void os_set_title_window(char *title)
{
	NSWindow		*wind;
	NSString		*ns_str;
	
	wind=os_cocoa_get_window();
	
	ns_str=[[NSString alloc] initWithUTF8String:title];
	[wind setTitle:ns_str];
	
	[ns_str release];
}

void os_swap_gl_buffer(void)
{
	View			*view;
	
	view=os_cocoa_get_view();
	[view openGLFlush];
}

/* =======================================================

      Cursors
      
======================================================= */

void os_set_arrow_cursor(void)
{
	[[NSCursor arrowCursor] set];
}

void os_set_wait_cursor(void)
{
	// cocoa handles this automatically
}

void os_set_hand_cursor(void)
{
	[[NSCursor openHandCursor] set];
}

void os_set_drag_cursor(void)
{
	[[NSCursor closedHandCursor] set];
}

void os_set_resize_cursor(void)
{
	[[NSCursor arrowCursor] set];
}

void os_set_add_cursor(void)
{
	[[NSCursor crosshairCursor] set];
}

void os_set_subtract_cursor(void)
{
	[[NSCursor disappearingItemCursor] set];
}

/* =======================================================

      Menus
      
======================================================= */

void os_menu_create(os_menu_item_type *os_menus)
{
// [[NSApp mainMenu] setAutoenablesItems:NO];
}

void os_menu_dispose(void)
{
}

void os_menu_enable_item(int menu_idx,int item_idx,bool enable)
{
	NSMenu		*main_menu,*sub_menu;
	NSMenuItem	*item;
	
	main_menu=[NSApp mainMenu];
	sub_menu=[[main_menu itemAtIndex:menu_idx] submenu];
	item=[sub_menu itemAtIndex:item_idx];
	
	[item setEnabled:enable];
}

void os_menu_check_item(int menu_idx,int item_idx,bool check)
{
	NSMenu		*main_menu,*sub_menu;
	NSMenuItem	*item;
	
	main_menu=[NSApp mainMenu];
	sub_menu=[[main_menu itemAtIndex:menu_idx] submenu];
	item=[sub_menu itemAtIndex:item_idx];
	
	if (check) {
		[item setState:NSOnState];
	}
	else {
		[item setState:NSOffState];
	}
}

/* =======================================================

      Keys
      
======================================================= */

bool os_key_space_down(void)
{
/*
	KeyMap			map;
	unsigned char	*c;
	
	GetKeys(map);
	c=(unsigned char*)map;
	
	return((c[6]&0x02)!=0);
	*/
	return(FALSE);
}

bool os_key_option_down(void)
{
//	return((GetCurrentEventKeyModifiers()&optionKey)!=0);
	return(FALSE);
}

bool os_key_control_down(void)
{
//	return((GetCurrentEventKeyModifiers()&controlKey)!=0);
	return(FALSE);
}

bool os_key_command_down(void)
{
//	return((GetCurrentEventKeyModifiers()&cmdKey)!=0);
	return(FALSE);
}

bool os_key_shift_down(void)
{
//	return(((GetCurrentEventKeyModifiers()&shiftKey)!=0)||((GetCurrentEventKeyModifiers()&alphaLock)!=0));
	return(FALSE);
}

/* =======================================================

      Mouse
      
======================================================= */

void os_get_cursor(d3pnt *pnt)
{
/*
	Point			ui_pt;
	
	GetMouse(&ui_pt);
	GlobalToLocal(&ui_pt);

	pnt->x=ui_pt.h;
	pnt->y=ui_pt.v;
	*/
}

bool os_button_down(void)
{
//	return(Button());
	return(FALSE);
}

bool os_track_mouse_location(d3pnt *pnt,d3rect *offset_box)
{
/*
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
	*/
	return(FALSE);
}

/* =======================================================

      Alerts
      
======================================================= */

void os_dialog_alert(char *title,char *msg)
{
	NSString			*ns_title_str,*ns_msg_str;

	os_set_arrow_cursor();
	
	ns_title_str=[[NSString alloc] initWithUTF8String:title];
	ns_msg_str=[[NSString alloc] initWithUTF8String:msg];
	
	NSRunAlertPanel(ns_title_str,ns_msg_str,nil,nil,nil);
	
	[ns_title_str release];
	[ns_msg_str release];
}

int os_dialog_confirm(char *title,char *msg,bool include_cancel)
{
	NSString		*ns_title_str,*ns_msg_str,
					*ns_butt_1_str,*ns_butt_2_str;
	NSInteger		resp;

	os_set_arrow_cursor();
	
	ns_title_str=[[NSString alloc] initWithUTF8String:title];
	ns_msg_str=[[NSString alloc] initWithUTF8String:msg];

	ns_butt_1_str=@"Yes";
	if (include_cancel) {
		ns_butt_2_str=@"Cancel";
	}
	else {
		ns_butt_2_str=nil;
	}
	
	resp=NSRunAlertPanel(ns_title_str,ns_msg_str,ns_butt_1_str,ns_butt_2_str,@"No");
	
	[ns_butt_1_str release];
	if (include_cancel) [ns_butt_2_str release];
	
	[ns_title_str release];
	[ns_msg_str release];
	
	switch (resp) {
		case NSAlertDefaultReturn:
			return(0);
		case NSAlertAlternateReturn:
			return(1);
		default:
			return(2);
	}
}

/* =======================================================

      Load a File
      
======================================================= */

Boolean os_load_file_filter(AEDesc *theItem,void *info,void *callBackUD,NavFilterModes filterMode)
{
/*
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
	*/
	return(TRUE);
}

bool os_load_file(char *title,char *path,char *ext)
{
/*
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
	*/
	
	return(TRUE);
}

/* =======================================================

      Color Picker
      
======================================================= */

void os_pick_color(d3col *col)
{
/*
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
	*/
}

/* =======================================================

      Launch Process
      
======================================================= */

bool os_launch_process(char *path,bool text_editor)
{
/*
	FSRef					fsref;
	CFURLRef				cf_url;

	if (FSPathMakeRef((unsigned char*)path,&fsref,NULL)!=0) return(FALSE);
	
	cf_url=CFURLCreateFromFSRef(kCFAllocatorDefault,&fsref);
	LSOpenCFURLRef(cf_url,NULL);
	CFRelease(cf_url);

	return(TRUE);
	*/
	return(FALSE);
}

/* =======================================================

      OS Dialogs
      
======================================================= */

bool os_dialog_run(char *title,int wid,int high,os_dialog_ctrl_type *ctrls,void *callback)
{
	return(FALSE);
}

void os_dialog_close(bool ok)
{
}

void os_dialog_set_text(int id,char *value)
{
}

void os_dialog_get_text(int id,char *value,int value_len)
{
}

void os_dialog_set_int(int id,int i)
{
}

int os_dialog_get_int(int id)
{
	return(0);
}

void os_dialog_set_float(int id,float f)
{
}

float os_dialog_get_float(int id)
{
	return(0.0f);
}

void os_dialog_set_bool(int id,bool value)
{
}

bool os_dialog_get_bool(int id)
{
	return(FALSE);
}

void os_dialog_combo_clear(int id)
{
}

void os_dialog_combo_add(int id,char *str)
{
}

void os_dialog_combo_set_value(int id,int value)
{
}

int os_dialog_combo_get_value(int id)
{
	return(0);
}

void os_dialog_tree_add(int id,file_path_directory_type *fpd)
{
}

int os_dialog_tree_get_value(int id)
{
	return(0);
}

void os_dialog_set_focus(int id,bool select_all)
{
}

void os_dialog_enable(int id,bool enable)
{
}



