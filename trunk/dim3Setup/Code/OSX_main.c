/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Main

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

WindowRef						wind;
EventHandlerRef					model_wind_event;
EventHandlerUPP					model_wind_upp;

AGLContext						ctx;

extern iface_type				iface;
extern file_path_setup_type		file_path_setup;

/* =======================================================

      Window Resize
      
======================================================= */

void main_wind_event_resize(void)
{
	Rect			box;
	GLint			rect[4];	
	
		// new model view size
		
	aglUpdateContext(ctx);
		
	GetWindowPortBounds(wind,&box);
	
	rect[0]=0;
	rect[1]=0;
	rect[2]=box.right;
	rect[3]=box.bottom;

	aglSetInteger(ctx,AGL_BUFFER_RECT,rect);
	aglEnable(ctx,AGL_BUFFER_RECT);
	
		// resize windows and palettes
	
	main_wind_resize();
	
		// redraw

	main_wind_draw();
}

/* =======================================================

      Window Events
      
======================================================= */

OSStatus main_wind_event_handler(EventHandlerCallRef eventhandler,EventRef event,void *userdata)
{
	unsigned long		modifiers,nclick;
	long				delta;
	char				ch;
	d3pnt				pnt;
	Point				pt;
	EventMouseWheelAxis	axis;
	WindowRef			wref;
	
	switch (GetEventClass(event)) {
	
		case kEventClassWindow:
		
			switch (GetEventKind(event)) {
			
				case kEventWindowDrawContent:
					main_wind_draw();
					return(noErr);
					
				case kEventWindowBoundsChanged:
					main_wind_event_resize();
					return(noErr);

				case kEventWindowClickContentRgn:
					GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,NULL,sizeof(Point),NULL,&pt);
					
					SetPort(GetWindowPort(wind));
					GlobalToLocal(&pt);
					
					pnt.x=pt.h;
					pnt.y=pt.v;
					
					GetEventParameter(event,kEventParamClickCount,typeUInt32,NULL,sizeof(unsigned long),NULL,&nclick);
					main_wind_click(&pnt,(nclick!=1));
					return(noErr);
					
				case kEventWindowClose:
					os_application_quit();
					return(noErr);
					
                case kEventWindowCursorChange:
					main_wind_cursor();
                    return(noErr);
					
			}
			break;
			
		case kEventClassKeyboard:
		
			switch (GetEventKind(event)) {
			
				case kEventRawKeyDown:
				case kEventRawKeyRepeat:
					GetEventParameter(event,kEventParamKeyMacCharCodes,typeChar,NULL,sizeof(char),NULL,&ch);
					main_wind_key(ch);
					main_wind_cursor();
					return(noErr);
				
				case kEventRawKeyUp:
					GetEventParameter(event,kEventParamKeyMacCharCodes,typeChar,NULL,sizeof(char),NULL,&ch);
					main_wind_cursor();
					return(noErr);
                   
                case kEventRawKeyModifiersChanged:
 					GetEventParameter(event,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(unsigned long),NULL,&modifiers);
					main_wind_cursor();
                    return(noErr);
					
			}
			break;
			
		case kEventClassMouse:
		
			switch (GetEventKind(event)) {
			
				case kEventMouseMoved:
					GetEventParameter(event,kEventParamWindowRef,typeWindowRef,NULL,sizeof(WindowRef),NULL,&wref);
					if (wref!=wind) return(noErr);
					
					GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,NULL,sizeof(Point),NULL,&pt);
					
					SetPort(GetWindowPort(wind));
					GlobalToLocal(&pt);
					
					pnt.x=pt.h;
					pnt.y=pt.v;

					main_wind_mouse_move(&pnt);
					
					return(noErr);
			
				case kEventMouseWheelMoved:
					GetEventParameter(event,kEventParamWindowRef,typeWindowRef,NULL,sizeof(WindowRef),NULL,&wref);
					if (wref!=wind) return(noErr);
				
					GetEventParameter(event,kEventParamMouseWheelAxis,typeMouseWheelAxis,NULL,sizeof(EventMouseWheelAxis),NULL,&axis);
					if (axis!=kEventMouseWheelAxisY) return(noErr);
						
					GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,NULL,sizeof(Point),NULL,&pt);
					SetPort(GetWindowPort(wind));
					
					GlobalToLocal(&pt);
					pnt.x=pt.h;
					pnt.y=pt.v;
					
					GetEventParameter(event,kEventParamMouseWheelDelta,typeLongInteger,NULL,sizeof(long),NULL,&delta);
					main_wind_scroll_wheel(&pnt,delta);

					return(noErr);
					
			}
			break;
			
	}

	return(eventNotHandledErr);
}

/* =======================================================

      Model Window
      
======================================================= */

void main_wind_open(void)
{
	int							wid,high;
	Rect						wbox,box;
	GLint						attrib[]={AGL_RGBA,AGL_DOUBLEBUFFER,AGL_DEPTH_SIZE,24,AGL_ALL_RENDERERS,AGL_NONE};
	AGLPixelFormat				pf;
	GLint						rect[4];
	EventTypeSpec	wind_events[]={	{kEventClassWindow,kEventWindowDrawContent},
									{kEventClassWindow,kEventWindowBoundsChanged},
									{kEventClassWindow,kEventWindowClickContentRgn},
									{kEventClassWindow,kEventWindowClose},
									{kEventClassWindow,kEventWindowCursorChange},
									{kEventClassKeyboard,kEventRawKeyUp},
									{kEventClassKeyboard,kEventRawKeyDown},
									{kEventClassKeyboard,kEventRawKeyModifiersChanged},
									{kEventClassMouse,kEventMouseMoved},
									{kEventClassMouse,kEventMouseWheelMoved}};
	
    GetAvailableWindowPositioningBounds(GetMainDevice(),&wbox);

	wid=list_palette_tree_sz*3;
	high=(wbox.bottom-wbox.top)-60;

	box.left=((wbox.right-wbox.left)-wid)/2;
	box.top=60;
	box.right=box.left+wid;
	box.bottom=box.top+high;
	
	CreateNewWindow(kDocumentWindowClass,kWindowStandardDocumentAttributes|kWindowLiveResizeAttribute|kWindowStandardHandlerAttribute|kWindowInWindowMenuAttribute,&box,&wind);
	SetWTitle(wind,"\pSetup");

		// show window
		
	ShowWindow(wind);
	SetPort(GetWindowPort(wind));
	
		// OpenGL context
		
	pf=aglChoosePixelFormat(NULL,0,attrib);
	
	ctx=aglCreateContext(pf,NULL);
	aglSetDrawable(ctx,(AGLDrawable)GetWindowPort(wind));
	aglSetCurrentContext(ctx);
	aglDestroyPixelFormat(pf);
	
	GetWindowPortBounds(wind,&box);
		
	rect[0]=0;
	rect[1]=0;
	rect[2]=box.right;
	rect[3]=box.bottom;

	aglSetInteger(ctx,AGL_BUFFER_RECT,rect);
	aglEnable(ctx,AGL_BUFFER_RECT);

	main_wind_gl_setup();
	
		// init window
		
	main_wind_initialize();

		// events
		
	model_wind_upp=NewEventHandlerUPP(main_wind_event_handler);
	InstallEventHandler(GetWindowEventTarget(wind),model_wind_upp,GetEventTypeCount(wind_events),wind_events,NULL,&model_wind_event);
}

void main_wind_close(void)
{
		// remove events
		
	RemoveEventHandler(model_wind_event);
	DisposeEventHandlerUPP(model_wind_upp);

		// close window
		
	main_wind_shutdown();
	
		// close OpenGL contexts
		
	aglSetCurrentContext(NULL);
	
	aglSetDrawable(ctx,NULL);
	aglDestroyContext(ctx);
	
		// close window

	DisposeWindow(wind);
}

/* =======================================================

      Main Event Loop
      
======================================================= */

OSStatus menu_event_callback(EventHandlerCallRef eventhandler,EventRef event,void *userdata)
{
	HICommand		cmd;
	
	GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
	
	if (menu_event_run(cmd.commandID)) return(noErr);

	return(eventNotHandledErr);
}

void main_loop(void)
{
	EventHandlerRef		menu_event;
	EventHandlerUPP		menu_upp;
	EventTypeSpec		app_menu_events[]={{kEventClassCommand,kEventProcessCommand}};
	
	menu_upp=NewEventHandlerUPP(menu_event_callback);						   
	InstallEventHandler(GetApplicationEventTarget(),menu_upp,GetEventTypeCount(app_menu_events),app_menu_events,NULL,&menu_event);
	
	RunApplicationEventLoop();

	RemoveEventHandler(menu_event);
	DisposeEventHandlerUPP(menu_upp);
}

/* =======================================================

      Start Menu
      
======================================================= */

void menu_start(void)
{
	IBNibRef				nib;
	CFStringRef				cf_str;
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"dim3 Setup",kCFStringEncodingMacRoman);
	CreateNibReference(cf_str,&nib);
	CFRelease(cf_str);
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"MenuBar",kCFStringEncodingMacRoman);
	SetMenuBarFromNib(nib,cf_str);
	CFRelease(cf_str);
	
	DisposeNibReference(nib);
}

/* =======================================================

      Infamous Main Code
      
======================================================= */

int main(int argc,char *argv[])
{
	os_glue_start();
	os_set_arrow_cursor();
	
		// setup and read XML
		
	if (!file_paths_setup(&file_path_setup)) {
		os_dialog_alert("Error","No data folder found");
		os_glue_end();
		return(0);
	}
	
	if (!iface_initialize(&iface,&file_path_setup)) {
		os_dialog_alert("Error","Out of Memory");
		os_glue_end();
		return(0);
	}
	
	iface_read(&iface);
	
		// run setup
		
	main_wind_open();
	menu_start();
	main_wind_initialize();
	
	main_wind_draw();
	
	main_loop();
	
	main_wind_shutdown();
	main_wind_close();
		
		// shutdown and save
		
	iface_shutdown(&iface);

	os_glue_end();
    
    return(0);
}
