/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Line

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

WindowRef						wind;
EventHandlerRef					main_wind_event;
EventHandlerUPP					main_wind_upp;

AGLContext						ctx;

extern iface_type				iface;
extern file_path_setup_type		file_path_setup;
extern editor_state_type		state;

/* =======================================================

      Resizing
      
======================================================= */

void main_wind_event_resize(void)
{
	Rect			wbox;
	GLint			rect[4];
	
		// fix all views and palettes
		
	aglUpdateContext(ctx);
	
		// update buffer rect
		
	GetWindowPortBounds(wind,&wbox);
		
	rect[0]=0;
	rect[1]=0;
	rect[2]=wbox.right-wbox.left;
	rect[3]=wbox.bottom-wbox.top;
	
	aglSetInteger(ctx,AGL_BUFFER_RECT,rect);
	aglEnable(ctx,AGL_BUFFER_RECT);
	
		// fix views and redraw
		
	main_wind_resize();
	main_wind_draw();
}

/* =======================================================

      Main Window Events
      
======================================================= */

OSStatus main_wind_event_callback(EventHandlerCallRef eventhandler,EventRef event,void *userdata)
{
	unsigned long		modifiers,nclick;
	unsigned short		btn;
	long				delta;
	char				ch;
	Point				pt;
	Rect				wbox;
	d3pnt				dpt;
	EventMouseWheelAxis	axis;
	WindowRef			wref;
	
	switch (GetEventClass(event)) {
	
		case kEventClassWindow:
		
			switch (GetEventKind(event)) {
			
				case kEventWindowDrawContent:
					main_wind_draw();
					DrawControls(wind);
					return(noErr);
					
                case kEventWindowCursorChange:
					GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,NULL,sizeof(Point),NULL,&pt);
                    GlobalToLocal(&pt);
 					dpt.x=pt.h;
					dpt.y=pt.v;
					view_cursor(&dpt);
                    return(noErr);

				case kEventWindowClickContentRgn:
					GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,NULL,sizeof(Point),NULL,&pt);

                    SetPortWindowPort(wind);
					GlobalToLocal(&pt);
					
					GetWindowPortBounds(wind,&wbox);
					
					dpt.x=pt.h;
					dpt.y=pt.v;
					
						// middle button vertex change mode
						
					GetEventParameter(event,kEventParamMouseButton,typeMouseButton,NULL,sizeof(unsigned short),NULL,&btn);
					
					if (btn==kEventMouseButtonTertiary) {
						tool_switch_drag_mode();
						return(noErr);
					}
					
						// regular clicking
					
					GetEventParameter(event,kEventParamClickCount,typeUInt32,NULL,sizeof(unsigned long),NULL,&nclick);
					main_wind_click(&dpt,(nclick!=1));
					
					return(noErr);

				case kEventWindowBoundsChanged:
				case kEventWindowResizeCompleted:
					main_wind_event_resize();
					return(noErr);
				
				case kEventWindowClose:
					if (!file_close_map()) return(noErr);
					os_application_quit();
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
					
					SetPortWindowPort(wind);
					GlobalToLocal(&pt);
					
					dpt.x=pt.h;
					dpt.y=pt.v;

					main_wind_mouse_move(&dpt);
					
					return(noErr);
			
				case kEventMouseWheelMoved:
					GetEventParameter(event,kEventParamWindowRef,typeWindowRef,NULL,sizeof(WindowRef),NULL,&wref);
					if (wref!=wind) return(noErr);

					GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,NULL,sizeof(Point),NULL,&pt);
					SetPortWindowPort(wind);
                    GlobalToLocal(&pt);
					
					dpt.x=pt.h;
					dpt.y=pt.v;
					
					GetEventParameter(event,kEventParamMouseWheelAxis,typeMouseWheelAxis,NULL,sizeof(EventMouseWheelAxis),NULL,&axis);
					if (axis!=kEventMouseWheelAxisY) return(noErr);
					
					GetEventParameter(event,kEventParamMouseWheelDelta,typeLongInteger,NULL,sizeof(long),NULL,&delta);
					
					main_wind_scroll_wheel(&dpt,delta);
					return(noErr);
					
			}
			break;
			
	}

	return(eventNotHandledErr);
}

/* =======================================================

      Open & Close Main Window
      
======================================================= */

void main_wind_open(void)
{
	HIRect						wbox;
	Rect						box;
	GLint						attrib[]={AGL_NO_RECOVERY,AGL_RGBA,AGL_DOUBLEBUFFER,AGL_WINDOW,AGL_ACCELERATED,AGL_PIXEL_SIZE,24,AGL_ALPHA_SIZE,8,AGL_DEPTH_SIZE,24,AGL_NONE};
	GLint						rect[4];
	CGDirectDisplayID			cg_display_id;
	CGOpenGLDisplayMask			cg_display_mask;
	AGLPixelFormat				pf;
	EventTypeSpec				wind_events[]={	{kEventClassWindow,kEventWindowDrawContent},
												{kEventClassWindow,kEventWindowCursorChange},
												{kEventClassWindow,kEventWindowClickContentRgn},
												{kEventClassWindow,kEventWindowBoundsChanged},
												{kEventClassWindow,kEventWindowResizeCompleted},
												{kEventClassWindow,kEventWindowClose},
												{kEventClassKeyboard,kEventRawKeyDown},
												{kEventClassKeyboard,kEventRawKeyUp},
												{kEventClassKeyboard,kEventRawKeyRepeat},
												{kEventClassKeyboard,kEventRawKeyModifiersChanged},
												{kEventClassMouse,kEventMouseMoved},
												{kEventClassMouse,kEventMouseWheelMoved}};
	
 		// get display
		
	cg_display_id=CGMainDisplayID();
	cg_display_mask=CGDisplayIDToOpenGLDisplayMask(cg_display_id);
					
		// create window
		
	HIWindowGetAvailablePositioningBounds(cg_display_id,kHICoordSpaceScreenPixel,&wbox);
	
	box.left=wbox.origin.x;
	box.top=wbox.origin.y+22;
	box.right=wbox.size.width;
	box.bottom=wbox.size.height+22;

	CreateNewWindow(kDocumentWindowClass,kWindowCloseBoxAttribute|kWindowCollapseBoxAttribute|kWindowFullZoomAttribute|kWindowResizableAttribute|kWindowLiveResizeAttribute|kWindowStandardHandlerAttribute|kWindowInWindowMenuAttribute,&box,&wind);

		// show window
		
	ShowWindow(wind);
	SetPortWindowPort(wind);
	
		// opengl setup
		
	pf=aglCreatePixelFormat(attrib);
	ctx=aglCreateContext(pf,NULL);
	aglSetCurrentContext(ctx);
	aglDestroyPixelFormat(pf);

	main_wind_gl_setup();
	
	aglSetWindowRef(ctx,wind);
	
		// buffer rect clipping
		
	GetWindowPortBounds(wind,&box);
		
	rect[0]=0;
	rect[1]=0;
	rect[2]=box.right;
	rect[3]=box.bottom;
	
	aglSetInteger(ctx,AGL_BUFFER_RECT,rect);
	aglEnable(ctx,AGL_BUFFER_RECT);

		// setup main window
		
	main_wind_initialize();
	
        // events
    
	main_wind_upp=NewEventHandlerUPP(main_wind_event_callback);
	InstallEventHandler(GetWindowEventTarget(wind),main_wind_upp,GetEventTypeCount(wind_events),wind_events,NULL,&main_wind_event);
}

void main_wind_close(void)
{
        // main window shutdown
		
	main_wind_shutdown();
		
		// gl shutdown
		
	aglSetCurrentContext(NULL);
	aglSetWindowRef(ctx,NULL);
	aglDestroyContext(ctx);
	
        // dispose of events and window
        
	RemoveEventHandler(main_wind_event);
	DisposeEventHandlerUPP(main_wind_upp);
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
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"dim3 Editor",kCFStringEncodingMacRoman);
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
	menu_start();
	
		// initialize
		
	if (!main_app_initialize()) return(0);

		// main loop
		
	main_wind_open();
	main_wind_draw();
	main_loop();
	main_wind_close();
	
		// shutdown
		
	main_app_shutdown();
	
    return(0);
}
