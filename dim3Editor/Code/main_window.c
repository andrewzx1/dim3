/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Map Window

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

extern d3rect			tool_palette_box,txt_palette_box;

extern map_type			map;
extern setup_type		setup;

d3rect					main_wind_box;

WindowRef				wind;
EventHandlerRef			main_wind_event;
EventHandlerUPP			main_wind_upp;

AGLContext				ctx;

file_path_setup_type	file_path_setup;
editor_state_type		state;

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
	EventMouseWheelAxis	axis;
	d3pnt				dpt;
	
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
					walk_view_cursor(&dpt);
                    return(noErr);

				case kEventWindowClickContentRgn:
					GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,NULL,sizeof(Point),NULL,&pt);

                    SetPort(GetWindowPort(wind));
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
					
						// click in tool palette
						
					if ((pt.v>=tool_palette_box.ty) && (pt.v<=tool_palette_box.by)) {
						tool_palette_click(&dpt);
						return(noErr);
					}
	
						// click in texture palette
						
					GetEventParameter(event,kEventParamClickCount,typeUInt32,NULL,sizeof(unsigned long),NULL,&nclick);
					
					if ((pt.v>=txt_palette_box.ty) && (pt.v<=txt_palette_box.by)) {
						texture_palette_click(&dpt,(nclick!=1));
						return(noErr);
					}
					
						// click in main window
						
                    if (walk_view_click(&dpt,(nclick!=1))) return(noErr);
                    
                    return(eventNotHandledErr);
					
				case kEventWindowBoundsChanged:
				case kEventWindowResizeCompleted:
					main_wind_resize();
					return(noErr);
				
				case kEventWindowClose:
					if (state.map_opened) {
						if (!menu_save_changes_dialog()) return(noErr);
						file_close_map();
					}
					return(noErr);
					
			}
			break;
			
		case kEventClassKeyboard:
		
			switch (GetEventKind(event)) {
			
				case kEventRawKeyDown:
				case kEventRawKeyRepeat:
					GetEventParameter(event,kEventParamKeyMacCharCodes,typeChar,NULL,sizeof(char),NULL,&ch);
					main_wind_key_down(ch);
					main_wind_key_cursor();
					return(noErr);
				
				case kEventRawKeyUp:
					GetEventParameter(event,kEventParamKeyMacCharCodes,typeChar,NULL,sizeof(char),NULL,&ch);
					main_wind_key_cursor();
					return(noErr);
                   
                case kEventRawKeyModifiersChanged:
 					GetEventParameter(event,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(unsigned long),NULL,&modifiers);
					main_wind_key_cursor();
                    return(noErr);
					
			}
			break;
			
		case kEventClassMouse:
		
			switch (GetEventKind(event)) {
			
				case kEventMouseWheelMoved:
					GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,NULL,sizeof(Point),NULL,&pt);
					SetPort(GetWindowPort(wind));
                    GlobalToLocal(&pt);
					
					dpt.x=pt.h;
					dpt.y=pt.v;
					
					GetEventParameter(event,kEventParamMouseWheelAxis,typeMouseWheelAxis,NULL,sizeof(EventMouseWheelAxis),NULL,&axis);
					if (axis==kEventMouseWheelAxisY) {
						GetEventParameter(event,kEventParamMouseWheelDelta,typeLongInteger,NULL,sizeof(long),NULL,&delta);
						main_wind_scroll_wheel(&dpt,delta);
					}
					return(noErr);
					
			}
			break;
			
	}

	return(eventNotHandledErr);
}

/* =======================================================

      Main Window Viewport Coordinates
      
======================================================= */

void main_wind_setup(void)
{
	Rect			wbox;
	GLint			rect[4];
	
		// setup view box
		
	GetWindowPortBounds(wind,&wbox);

	main_wind_box.lx=wbox.left;
	main_wind_box.rx=wbox.right;
	main_wind_box.ty=wbox.top;
	main_wind_box.by=wbox.bottom;
	
		// buffer rect clipping
		
	rect[0]=0;
	rect[1]=0;
	rect[2]=wbox.right-wbox.left;
	rect[3]=wbox.bottom-wbox.top;
	
	aglSetInteger(ctx,AGL_BUFFER_RECT,rect);
	aglEnable(ctx,AGL_BUFFER_RECT);
}

/* =======================================================

      UI Icon Loader
      
======================================================= */

IconRef main_wind_load_icon_ref(char *name)
{
	char					path[1024];
	FSRef					fsref;
	IconRef					iconRef;

	file_paths_app(&file_path_setup,path,"Contents/Resources",name,"icns");
	FSPathMakeRef((unsigned char*)path,&fsref,NULL);
	
	RegisterIconRefFromFSRef(0,0,&fsref,&iconRef);

	return(iconRef);
}

/* =======================================================

      Open & Close Main Window
      
======================================================= */

void main_wind_open(void)
{
	Rect						wbox;
	GLint						attrib[]={AGL_NO_RECOVERY,AGL_RGBA,AGL_DOUBLEBUFFER,AGL_ACCELERATED,AGL_PIXEL_SIZE,24,AGL_ALPHA_SIZE,8,AGL_DEPTH_SIZE,16,AGL_STENCIL_SIZE,8,AGL_NONE};
	GDHandle					gdevice;
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
												{kEventClassMouse,kEventMouseWheelMoved}};
	
        // open window
        
    GetAvailableWindowPositioningBounds(GetMainDevice(),&wbox);

	SetRect(&wbox,wbox.left,(wbox.top+25),wbox.right,wbox.bottom);
	CreateNewWindow(kDocumentWindowClass,kWindowCloseBoxAttribute|kWindowCollapseBoxAttribute|kWindowFullZoomAttribute|kWindowResizableAttribute|kWindowLiveResizeAttribute|kWindowStandardHandlerAttribute|kWindowInWindowMenuAttribute,&wbox,&wind);

		// show window before additional setup
		
	ShowWindow(wind);
   
	SetPort(GetWindowPort(wind));
		
	TextFont(FMGetFontFamilyFromName("\pMonaco"));
	TextSize(10);
	
		// opengl setup
		
	gdevice=GetMainDevice();

	pf=aglChoosePixelFormat(&gdevice,1,attrib);
	ctx=aglCreateContext(pf,NULL);
	aglSetCurrentContext(ctx);
	aglDestroyPixelFormat(pf);

	glDisable(GL_SMOOTH);
	glDisable(GL_DITHER);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_DEPTH_TEST);
	
	aglSetDrawable(ctx,(AGLDrawable)GetWindowPort(wind));

		// setup view sizes
	
	tool_palette_initialize();
	tool_palette_setup();
	
	texture_palette_setup();
	
	main_wind_setup();
	walk_view_initialize();
   
        // misc setup
        
	state.vertex_mode=vertex_mode_none;
	state.drag_mode=drag_mode_mesh;
	state.grid_mode=grid_mode_small;
	state.handle_mode=handle_mode_rotate;
	
	state.free_look=TRUE;
	state.select_add=FALSE;
	
	state.drag_handle_idx=-1;
	
	menu_update_view();
	
        // events
    
	main_wind_upp=NewEventHandlerUPP(main_wind_event_callback);
	InstallEventHandler(GetWindowEventTarget(wind),main_wind_upp,GetEventTypeCount(wind_events),wind_events,NULL,&main_wind_event);
	
		// start palettes
		
	palette_initialize();
}

void main_wind_close(void)
{
		// close the palettes
		
	palette_shutdown();
	
        // close views
		
	walk_view_shutdown();
	tool_palette_shutdown();
	
		// gl shutdown
		
	aglSetCurrentContext(NULL);
	aglSetDrawable(ctx,NULL);
	aglDestroyContext(ctx);
	
        // dispose of events and window
        
	RemoveEventHandler(main_wind_event);
	DisposeEventHandlerUPP(main_wind_upp);
	DisposeWindow(wind);
}

/* =======================================================

      Resizing
      
======================================================= */

void main_wind_resize(void)
{
	Rect			wbox;
	CGrafPtr		saveport;
	
		// erase window
		
	GetPort(&saveport);
	SetPort(GetWindowPort(wind));
    
	GetWindowPortBounds(wind,&wbox);
	EraseRect(&wbox);
	
	SetPort(saveport);

		// fix all views and palettes
		
	aglUpdateContext(ctx);

	tool_palette_setup();
	texture_palette_setup();
	
	main_wind_setup();
	DrawControls(wind);

	main_wind_draw();
}

/* =======================================================

      View Drawing
      
======================================================= */

void main_wind_draw(void)
{
		// clear gl buffer
		
	glDisable(GL_SCISSOR_TEST);
	
	glClearColor(1.0f,1.0f,1.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	walk_view_draw();

		// palettes
		
	tool_palette_draw();
	texture_palette_draw();
	
		// swap GL buffer
		
	aglSwapBuffers(ctx);
}

/* =======================================================

      Center Position in Map
	        
======================================================= */

void main_wind_center_position_in_map(void)
{
	int					n;
	d3pnt				pnt;
	d3ang				ang;
	
		// view angles
		
	ang.x=0.0f;
	ang.y=0.0f;
	ang.z=0.0f;
	
	walk_view_set_angle(&ang);
	
		// look for player spot first
		
	for (n=0;n!=map.nspot;n++) {
		if ((strcasecmp(map.spots[n].name,"start")==0) || (strcasecmp(map.spots[n].script,"player")==0)) {
			walk_view_set_position_y_shift(&map.spots[n].pnt,-(map_enlarge*20));
			return;
		}
	}

		// otherwise do first mesh with vertexes
		
	for (n=0;n!=map.mesh.nmesh;n++) {
		if (map.mesh.meshes[n].nvertex!=0) {
			map_mesh_calculate_center(&map,n,&pnt);
			walk_view_set_position(&pnt);
			return;
		}
	}
	
		// just center in total map size
		
	pnt.x=map_max_size/2;
	pnt.y=map_max_size/2;
	pnt.z=map_max_size/2;
	walk_view_set_position(&pnt);
}

/* =======================================================

      Key States
      
======================================================= */

void main_wind_key_cursor(void)
{
	Point			pt;
	d3pnt			dpt;
	
	GetMouse(&pt);
	GlobalToLocal(&pt);
	
	dpt.x=pt.h;
	dpt.y=pt.v;
	
	walk_view_cursor(&dpt);
}

void main_wind_key_down(char ch)
{
		// special tool keys
		
	if ((ch=='q') || (ch=='Q')) {
		tool_switch_vertex_mode();
		return;
	}
	
	if ((ch=='w') || (ch=='W')) {
		tool_switch_drag_mode();
		return;
	}
	
	if ((ch=='e') || (ch=='E')) {
		tool_switch_grid_mode();
		return;
	}
	
	if ((ch=='r') || (ch=='R')) {
		tool_switch_node_mode();
		return;
	}
	
		// panel keys
		
	walk_view_key(ch);
}

/* =======================================================

      Scroll Wheel
      
======================================================= */

void main_wind_scroll_wheel(d3pnt *pt,int delta)
{
	editor_view_type			*view;
	
		// switch to proper pane
		
	walk_view_select_view(pt);

		// run wheel change
	
	view=walk_view_get_current_view();
	walk_view_scroll_wheel_z_movement(view,delta);
}

