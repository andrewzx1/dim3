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
#import "DialogView.h"

#include "glue.h"

int								os_menu_add_count;
char							os_load_file_ext[32];

NSWindow						*diagWindow;
DialogView						*diagView;
NSControl						*diagControls[64];

/* =======================================================

      Glue Start and End
      
======================================================= */

void os_glue_start(void)
{
	os_menu_add_count=0;
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
	[NSApp stop:nil];
}

inline NSWindow* os_cocoa_get_window()
{
#ifndef D3_OS_IPHONE
	return(((AppDelegate*)[NSApp delegate])->window);
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

void os_menu_create_single_menu(char *name,NSMenu *menu)
{
	NSString			*menu_name;
	NSMenuItem			*mbar_item;
	
		// create menu item to add to menu bar
		
	menu_name=[[NSString alloc] initWithUTF8String:name];
	mbar_item=[[NSMenuItem alloc] initWithTitle:menu_name action:NULL keyEquivalent:@""];
	[menu_name release];
	
		// add to menu bar
		
	[mbar_item setSubmenu:menu];
	[[NSApp mainMenu] addItem:mbar_item];
	[mbar_item release];
	
	os_menu_add_count++;
}

void os_menu_create(os_menu_item_type *os_menus)
{
	char					c,last_menu_name[32],key_str[4];
	os_menu_item_type		*menu;
	NSString				*menu_name,*item_name,*key_equiv;
	NSMenu					*ns_menu;
	NSMenuItem				*ns_item;

		// create the menu

	menu=os_menus;
	
	ns_menu=NULL;
	last_menu_name[0]=0x0;
	
	os_menu_add_count=0;

	while (TRUE) {
		if (menu->menu_name[0]==0x0) break;

			// switching to a new menu

		if (strcmp(menu->menu_name,last_menu_name)!=0) {
			if (ns_menu!=NULL) {
				os_menu_create_single_menu(last_menu_name,ns_menu);
				[ns_menu release];
			}
			
			strcpy(last_menu_name,menu->menu_name);
			
			menu_name=[[NSString alloc] initWithUTF8String:last_menu_name];
			ns_menu=[[NSMenu alloc] initWithTitle:menu_name];
			[menu_name release];
			
			[ns_menu setAutoenablesItems:NO];
		}

			// add item
			// if menu doesn't exist, create it
			
		if (menu->item_name[0]!=0x0) {
		
			item_name=[[NSString alloc] initWithUTF8String:menu->item_name];
			
			c=menu->key;
			if ((c>='A') && (c<='Z')) c+=('a'-'A');

			key_str[0]=c;		// will be 0x0 if no key
			key_str[1]=0x0;
			key_equiv=[[NSString alloc] initWithUTF8String:key_str];
			
			ns_item=[[NSMenuItem alloc] initWithTitle:item_name action:@selector(menuCommand:) keyEquivalent:key_equiv];
			
			[key_equiv release];
			[item_name release];
			
			switch (menu->key_type) {

				case os_menu_key_cmd:
					[ns_item setKeyEquivalentModifierMask:NSCommandKeyMask];
					break;

				case os_menu_key_cmd_opt:
					[ns_item setKeyEquivalentModifierMask:NSCommandKeyMask|NSAlternateKeyMask];
					break;

				case os_menu_key_cmd_shift:
					[ns_item setKeyEquivalentModifierMask:NSCommandKeyMask|NSShiftKeyMask];
					break;

			}
			
			[ns_item setTarget:[NSApp delegate]];
			[ns_item setTag:menu->id];
			[ns_item setEnabled:YES];
			
			[ns_menu addItem:ns_item];
			[ns_item release];
		}
		else {
			[ns_menu addItem:[NSMenuItem separatorItem]];
		}

			// next menu item

		menu++;
	}

	if (ns_menu!=NULL) {
		os_menu_create_single_menu(last_menu_name,ns_menu);
		[ns_menu release];
	}
}

void os_menu_dispose(void)
{
	int			n;
	NSMenu		*main_menu;
	
	main_menu=[NSApp mainMenu];

	for (n=0;n!=os_menu_add_count;n++) {
		[main_menu removeItemAtIndex:1];
	}
	
	os_menu_add_count=0;
}

void os_menu_enable_item(int menu_idx,int item_idx,bool enable)
{
	NSMenu		*main_menu,*sub_menu;
	NSMenuItem	*item;
	
		// get over app menu
		
	menu_idx++;
	
		// enable entire menu
		
	if (item_idx==0) {
		main_menu=[NSApp mainMenu];
		item=[main_menu itemAtIndex:menu_idx];
		[item setEnabled:enable];
		return;
	}
	
		// enable single item
	
	main_menu=[NSApp mainMenu];
	sub_menu=[[main_menu itemAtIndex:menu_idx] submenu];
	item=[sub_menu itemAtIndex:(item_idx-1)];
	
	[item setEnabled:enable];
}

void os_menu_check_item(int menu_idx,int item_idx,bool check)
{
	NSMenu		*main_menu,*sub_menu;
	NSMenuItem	*item;
	
		// get over app menu
		
	menu_idx++;
	
		// get the item
	
	main_menu=[NSApp mainMenu];
	sub_menu=[[main_menu itemAtIndex:menu_idx] submenu];
	item=[sub_menu itemAtIndex:(item_idx-1)];
	
		// check item
		
	[item setState:(check?NSOnState:NSOffState)];
}

/* =======================================================

      Keys
      
======================================================= */

bool os_key_space_down(void)
{
	return(CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState,kVK_Space));
}

bool os_key_option_down(void)
{
	return(CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState,kVK_Option));
}

bool os_key_control_down(void)
{
	return(CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState,kVK_Control));
}

bool os_key_command_down(void)
{
	return(CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState,kVK_Command));
}

bool os_key_shift_down(void)
{
	return(CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState,kVK_Shift));
}

/* =======================================================

      Mouse
      
======================================================= */

void os_get_cursor(d3pnt *pnt)
{
	NSWindow		*wnd;
	NSView			*view;
	NSPoint			wnd_pnt;
	
	wnd=os_cocoa_get_window();
	view=os_cocoa_get_view();
	
	wnd_pnt=[view convertPoint:[wnd mouseLocationOutsideOfEventStream] fromView:nil];

	pnt->x=(int)wnd_pnt.x;
	pnt->y=(int)([view frame].size.height-wnd_pnt.y);
}

bool os_button_down(void)
{
	return(([NSEvent pressedMouseButtons]&0x1)!=0x0);
}

bool os_track_mouse_location(d3pnt *pnt,d3rect *offset_box)
{
	NSWindow		*wnd;
	NSView			*view;
	NSEvent			*event;
	NSPoint			wnd_pnt;
	
	wnd=os_cocoa_get_window();
	view=os_cocoa_get_view();
	
	event=[wnd nextEventMatchingMask:NSLeftMouseUpMask|NSLeftMouseDraggedMask];
	wnd_pnt=[view convertPoint:[event locationInWindow] fromView:nil];
	
	pnt->x=(int)wnd_pnt.x;
	pnt->y=(int)([view frame].size.height-wnd_pnt.y);

	if (offset_box!=NULL) {
		pnt->x-=offset_box->lx;
		pnt->y-=offset_box->ty;
	}
	
	return([event type]==NSLeftMouseUp);
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
	NSColorPanel		*colPanel;
	
	colPanel=[NSColorPanel sharedColorPanel];
	[colPanel makeKeyAndOrderFront:NSApp];
	
	while (TRUE) {
	
		event=[wnd nextEventMatchingMask:NSLeftMouseUpMask|NSLeftMouseDraggedMask];
	*/
	

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
	int						idx,deskWid,deskHigh;
	NSUInteger				styleMask;
	NSString				*nsTitle;
	NSRect					deskTopRect,frame,windRect;
	os_dialog_ctrl_type		*ctrl;
	NSButton	*btn;		// supergumba
	
		// get desktop window

	deskTopRect=[[NSScreen mainScreen] visibleFrame];
	deskWid=deskTopRect.size.width;
	deskHigh=deskTopRect.size.height;
	
		// create the dialog window
	
	frame=NSMakeRect(((deskWid-wid)>>1),(high-((deskHigh-high)>>1)),wid,high);
	styleMask=NSTitledWindowMask;
	windRect=[NSWindow contentRectForFrameRect:frame styleMask:styleMask];
	
	diagWindow=[[NSWindow alloc] initWithContentRect:windRect styleMask:styleMask backing:NSBackingStoreBuffered defer:false];

	nsTitle=[[NSString alloc] initWithUTF8String:title];
	[diagWindow setTitle:nsTitle];
	[nsTitle release];
	
		// create the view
	
	diagView=[[DialogView alloc] initWithFrame:windRect];
	[diagWindow setContentView:diagView];
	[diagWindow setDelegate:diagView];
	
		// get high from content view
		
	high=(int)[diagView frame].size.height;
	
		// add the controls
		
	idx=0;
	
	while (TRUE) {
		ctrl=&ctrls[idx];
		if (ctrl->type==-1) break;
		
		frame=NSMakeRect(ctrl->x,(high-ctrl->y),ctrl->wid,ctrl->high);
		nsTitle=[[NSString alloc] initWithUTF8String:ctrl->str];

		switch (ctrl->type) {

			case os_dialog_ctrl_type_button:
			case os_dialog_ctrl_type_default_button:
				btn=[[NSButton alloc] initWithFrame:frame];
				[[diagWindow contentView] addSubview:btn];
				[btn setTitle:nsTitle];
				[btn setButtonType:NSMomentaryLightButton];
				[btn setBezelStyle:NSRoundedBezelStyle];
				
				/* supergumba
					[my setTarget:self];
					[my setAction:@selector(invisible)];
				*/
				
				if (ctrl->type==os_dialog_ctrl_type_default_button) [diagWindow setDefaultButtonCell:[btn cell]];
				
				diagControls[idx]=btn;
				break;
		
		}
		
		[nsTitle release];
		
		idx++;
	}
	
		// display the window

	[diagWindow makeKeyAndOrderFront:NSApp];
	
		// run the modal loop
		
		/*
		modalSession=[NSApp beginModalSessionForWindow:conversionWindow];
[NSApp runModalForWindow:conversionWindow];
	[NSApp endModalSession:session];
*/

	return(TRUE);
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



