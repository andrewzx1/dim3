/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Main

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
#include "interface.h"
#include "dialog.h"

WindowRef						wind;
EventHandlerRef					model_wind_event;
EventHandlerUPP					model_wind_upp;
EventLoopTimerRef				model_timer_event;
EventLoopTimerUPP				model_timer_upp;

AGLContext						ctx;

extern model_type				model;
extern model_draw_setup			draw_setup;
extern file_path_setup_type		file_path_setup;
extern animator_state_type		state;

extern d3rect					model_box;

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
	rect[2]=model_box.rx;
	rect[3]=box.bottom;

	aglSetInteger(ctx,AGL_BUFFER_RECT,rect);
	aglEnable(ctx,AGL_BUFFER_RECT);
	
		// resize windows and palettes
	
	main_wind_resize();

		// resize controls

	box.left=model_box.rx;
		
	resize_pose_controls(&box);
	resize_bone_controls(&box);
	resize_animate_controls(&box);
	resize_mesh_controls(&box);
	resize_vertex_controls(&box);
	
		// redraw

	main_wind_draw();
	
	DrawControls(wind);
}

/* =======================================================

      Window Events
      
======================================================= */

OSStatus main_wind_event_handler(EventHandlerCallRef eventhandler,EventRef event,void *userdata)
{
	unsigned long		modifiers,nclick;
	unsigned short		btn;
	long				delta;
	char				ch;
	d3pnt				pnt;
	Point				pt;
	EventMouseWheelAxis	axis;
	
	switch (GetEventClass(event)) {
	
		case kEventClassWindow:
		
			switch (GetEventKind(event)) {
			
				case kEventWindowDrawContent:
					main_wind_draw();
					DrawControls(wind);
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
					
						// clicking in right side controls
						
					if (pt.h>=model_box.rx) return(eventNotHandledErr);

						// clicking in toolbar

					if (pt.v<=model_box.ty) {
						tool_palette_click(&pnt);
						return(noErr);
					}
					
						// clicking in palettes
						
					GetEventParameter(event,kEventParamClickCount,typeUInt32,NULL,sizeof(unsigned long),NULL,&nclick);
					 
					if (pt.v>=model_box.by) {
						texture_palette_click(model.textures,&pnt,(nclick!=1));
						return(noErr);
					}
					
						// clicking in model view
						
					GetEventParameter(event,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(unsigned long),NULL,&modifiers);
					GetEventParameter(event,kEventParamMouseButton,typeMouseButton,NULL,sizeof(unsigned short),NULL,&btn);
					
					click_model_wind(&pnt,modifiers);
					
					return(noErr);
					
				case kEventWindowClose:
					if (!menu_save_changes_dialog()) return(noErr);
					close_model_xml();
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
			
				case kEventMouseWheelMoved:
				
						// are we over model window?
						
					GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,NULL,sizeof(Point),NULL,&pt);
					SetPort(GetWindowPort(wind));
					
					GlobalToLocal(&pt);
					if (pt.h>=model_box.rx) return(eventNotHandledErr);
					if (pt.v<=model_box.ty) return(eventNotHandledErr);
					if (pt.v>=model_box.by) return(eventNotHandledErr);
					
						// zoom
						
					GetEventParameter(event,kEventParamMouseWheelAxis,typeMouseWheelAxis,NULL,sizeof(EventMouseWheelAxis),NULL,&axis);
					if (axis!=kEventMouseWheelAxisY) return(noErr);
					GetEventParameter(event,kEventParamMouseWheelDelta,typeLongInteger,NULL,sizeof(long),NULL,&delta);
					state.magnify_z+=(delta*20);
					main_wind_draw();
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
									{kEventClassMouse,kEventMouseWheelMoved}};
	
    GetAvailableWindowPositioningBounds(GetMainDevice(),&wbox);
	
	SetRect(&box,wbox.left,(wbox.top+25),wbox.right,wbox.bottom);
	CreateNewWindow(kDocumentWindowClass,kWindowStandardDocumentAttributes|kWindowLiveResizeAttribute|kWindowStandardHandlerAttribute|kWindowInWindowMenuAttribute,&box,&wind);
	SetWTitle(wind,"\pModel");
	ShowWindow(wind);
	
		// set font
		
	SetPort(GetWindowPort(wind));
	TextFont(FMGetFontFamilyFromName("\pMonaco"));
	TextSize(10);
	
		// get gl sizes
		
	GetWindowPortBounds(wind,&box);

		// dragging for bones window
		
	SetAutomaticControlDragTrackingEnabledForWindow(wind,TRUE);
	
		// model OpenGL contexts
		
	pf=aglChoosePixelFormat(NULL,0,attrib);
	
	ctx=aglCreateContext(pf,NULL);
	aglSetDrawable(ctx,(AGLDrawable)GetWindowPort(wind));
	aglSetCurrentContext(ctx);
	aglDestroyPixelFormat(pf);
	
	main_wind_gl_setup();
	
		// init window
		
	main_wind_initialize();
	
		// box from the controls
		
	GetWindowPortBounds(wind,&box);
		
	rect[0]=0;
	rect[1]=0;
	rect[2]=model_box.rx;
	rect[3]=box.bottom;

	aglSetInteger(ctx,AGL_BUFFER_RECT,rect);
	aglEnable(ctx,AGL_BUFFER_RECT);
	
		// controls
		
	box.left+=model_box.rx;
	
	start_pose_controls(wind,&box);
	start_bone_controls(wind,&box);
    start_animate_controls(wind,&box);
	start_mesh_controls(wind,&box);
	start_vertex_controls(wind,&box);

	DrawControls(wind);

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
	
		// remove controls
		
	end_pose_controls();
	end_bone_controls();
    end_animate_controls();
    end_mesh_controls();
	end_vertex_controls();

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
	os_set_arrow_cursor();
	
	state.model_open=FALSE;
	file_paths_setup(&file_path_setup);
	
	menu_start();
	
    open_model_xml();
	main_loop();
	close_model_xml();
    
    return(0);
}
