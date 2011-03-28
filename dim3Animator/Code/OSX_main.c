/****************************** File *********************************

Module: dim3 Animator
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
EventLoopTimerRef				model_timer_event;
EventLoopTimerUPP				model_timer_upp;

AGLContext						ctx;

extern iface_type				iface;
extern file_path_setup_type		file_path_setup;
extern animator_state_type		state;

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
					if (!menu_save_changes_dialog()) return(noErr);
					file_close_model();
					main_wind_play(FALSE,FALSE);
					menu_update();
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

void main_wind_event_timer(EventLoopTimerRef inTimer,void *inUserData)
{
	if (state.playing) main_wind_draw();
}

/* =======================================================

      Model Window
      
======================================================= */

void main_wind_open(void)
{
	Rect						wbox,box;
	GLint						attrib[]={AGL_NO_RECOVERY,AGL_RGBA,AGL_DOUBLEBUFFER,AGL_WINDOW,AGL_ACCELERATED,AGL_PIXEL_SIZE,24,AGL_DEPTH_SIZE,24,AGL_NONE};
	CGDirectDisplayID			cg_display_id;
	CGOpenGLDisplayMask			cg_display_mask;
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
	
	SetRect(&box,wbox.left,(wbox.top+22),wbox.right,wbox.bottom);
	CreateNewWindow(kDocumentWindowClass,kWindowStandardDocumentAttributes|kWindowLiveResizeAttribute|kWindowStandardHandlerAttribute|kWindowInWindowMenuAttribute,&box,&wind);
	SetWTitle(wind,"\pModel");

		// show window
		
	ShowWindow(wind);
	SetPort(GetWindowPort(wind));
	
		// OpenGL context
		
	cg_display_id=CGMainDisplayID();
	cg_display_mask=CGDisplayIDToOpenGLDisplayMask(cg_display_id);
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
	
		// init window
		
	main_wind_initialize();

		// events
		
	model_wind_upp=NewEventHandlerUPP(main_wind_event_handler);
	InstallEventHandler(GetWindowEventTarget(wind),model_wind_upp,GetEventTypeCount(wind_events),wind_events,NULL,&model_wind_event);
		
	model_timer_upp=NewEventLoopTimerUPP(main_wind_event_timer);
	InstallEventLoopTimer(GetCurrentEventLoop(),0,0.01,model_timer_upp,NULL,&model_timer_event);
}

void main_wind_close(void)
{
		// remove events
		
	RemoveEventLoopTimer(model_timer_event);
	DisposeEventLoopTimerUPP(model_timer_upp);
	
	RemoveEventHandler(model_wind_event);
	DisposeEventHandlerUPP(model_wind_upp);

		// close window
		
	main_wind_shutdown();
	
		// close OpenGL contexts
		
	aglSetCurrentContext(NULL);
	aglSetWindowRef(ctx,NULL);
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
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"dim3 Animator",kCFStringEncodingMacRoman);
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
	state.model_open=FALSE;
	
		// initialize
		
	os_glue_start();
	os_set_arrow_cursor();
	
	if (!file_paths_setup(&file_path_setup)) {
		os_dialog_alert("Error","No data folder found");
		os_glue_end();
		return(0);
	}
	
	if (!iface_initialize(&iface,&file_path_setup)) {
		os_dialog_alert("Error","Out of memory");
		os_glue_end();
		return(0);
	}

	iface_read(&iface);
		
	setup_xml_read();

	menu_start();
	
		// main loop
		
    file_open_model();
	main_loop();
	file_close_model();
	
		// shutdown
		
	iface_shutdown(&iface);
	os_glue_end();
    
    return(0);
}
