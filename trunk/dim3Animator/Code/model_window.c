/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Main Model Window

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

#include "model.h"
#include "interface.h"

WindowRef						wind;
EventHandlerRef					model_wind_event;
EventHandlerUPP					model_wind_upp;
EventLoopTimerRef				model_timer_event;
EventLoopTimerUPP				model_timer_upp;

int								cur_mesh,cur_bone,cur_pose,cur_animate,magnify_z;
bool							shift_on,rotate_on,size_on,drag_sel_on,vertex_on;
d3pnt							shift;
d3ang							ang;

AGLContext						ctx;

model_type						model;
model_draw_setup				draw_setup;
animator_state_type				state;

extern d3rect					model_box;

extern bool						fileopen;
extern file_path_setup_type		file_path_setup;

/* =======================================================

      Model Window Keyboard Control
      
======================================================= */

void model_wind_key(char ch,bool up)
{
	if (!fileopen) return;
	
		// esc key deselects
		
	if ((ch==0x1B) && (!up)) {
		vertex_clear_sel_mask(cur_mesh);
		main_wind_draw();
		return;
	}
	
		// space moves around
		
	if (ch==0x20) {
		if (up) {
			shift_on=FALSE;
			os_set_arrow_cursor();
		}
		else {
			shift_on=TRUE;
			rotate_on=size_on=FALSE;
			SetThemeCursor(kThemeOpenHandCursor);
		}
		return;
	}
}

void key_modifier_model_wind(unsigned long modifiers)
{
	if ((modifiers&cmdKey)!=0) {
		rotate_on=TRUE;
		shift_on=size_on=FALSE;
		SetThemeCursor(kThemeOpenHandCursor);
		return;
	}
	else {
		rotate_on=FALSE;
	}
	
	if ((modifiers&optionKey)!=0) {
		size_on=TRUE;
		shift_on=rotate_on=FALSE;
		SetThemeCursor(kThemeResizeUpDownCursor);
		return;
	}
	else {
		size_on=FALSE;
	}
	
	if ((modifiers&shiftKey)!=0) {
		size_on=shift_on=rotate_on=FALSE;
		SetThemeCursor(kThemePlusCursor);
		return;
	}
	if ((modifiers&controlKey)!=0) {
		size_on=shift_on=rotate_on=FALSE;
		SetThemeCursor(kThemePoofCursor);
		return;
	}
	
	if (!shift_on) os_set_arrow_cursor();
}

void model_wind_reset_modifiers(void)
{
	key_modifier_model_wind(GetCurrentKeyModifiers());
}

/* =======================================================

      Model Window Cursors
      
======================================================= */

void model_wind_cursor(unsigned long modifiers)
{
	if (shift_on) {
		SetThemeCursor(kThemeOpenHandCursor);
		return;
	}
	if ((modifiers&cmdKey)!=0) {
		SetThemeCursor(kThemeOpenHandCursor);
		return;
	}
	if ((modifiers&optionKey)!=0) {
		SetThemeCursor(kThemeResizeUpDownCursor);
		return;
	}
	if ((modifiers&shiftKey)!=0) {
		SetThemeCursor(kThemePlusCursor);
		return;
	}
	if ((modifiers&controlKey)!=0) {
		SetThemeCursor(kThemePoofCursor);
		return;
	}
	
	os_set_arrow_cursor();
}

/* =======================================================

      Window Resize
      
======================================================= */

void model_wind_resize(void)
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
		
	tool_palette_setup();
	texture_palette_setup();
	model_wind_setup();

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

      Main Window Draw
      
======================================================= */

void main_wind_draw_play_calc_animation(int cur_tick,int animate_idx,int blend_idx,bool non_blend_setup)
{
	int						first_pose,last_pose,tick,nxt_tick,
							pose_move_1_idx,pose_move_2_idx,msec;
	float					pose_factor;
	model_animate_type		*animate;
	model_pose_move_type	*pose_move_1,*pose_move_2;

		// calc animation
		
	animate=&model.animates[animate_idx];
	
	first_pose=0;
	if (animate->loop_start!=-1) first_pose=animate->loop_start;

	last_pose=animate->npose_move-1;
	if (animate->loop_end!=-1) last_pose=animate->loop_end;
	
	tick=state.blend[blend_idx].tick;
	pose_move_1_idx=state.blend[blend_idx].pose_move_idx;
	
	msec=animate->pose_moves[pose_move_1_idx].msec;
	nxt_tick=tick+msec;
	
	while (nxt_tick<cur_tick) {				// skip ahead till we find correct pose
		pose_move_1_idx++;
		if (pose_move_1_idx>last_pose) pose_move_1_idx=first_pose;
		
		msec=animate->pose_moves[pose_move_1_idx].msec;

		tick=nxt_tick;
		nxt_tick=tick+msec;
	}
	
	state.blend[blend_idx].tick=tick;
	state.blend[blend_idx].pose_move_idx=pose_move_1_idx;
	
	pose_move_2_idx=pose_move_1_idx+1;
	if (pose_move_2_idx>last_pose) pose_move_2_idx=first_pose;
	
		// get the pose moves
		
	pose_move_1=&animate->pose_moves[pose_move_1_idx];
	pose_move_2=&animate->pose_moves[pose_move_2_idx];
	
		// setup the poses
	
	draw_setup.poses[blend_idx].idx_1=pose_move_1->pose_idx;
	draw_setup.poses[blend_idx].idx_2=pose_move_2->pose_idx;
	
	pose_factor=(float)(nxt_tick-cur_tick)/(float)msec;
	
    draw_setup.poses[blend_idx].factor=pose_factor;
	draw_setup.poses[blend_idx].acceleration=pose_move_1->acceleration;
	
		// non-blended setup
		
	if (non_blend_setup) {
		draw_setup.ang.x=0;
		draw_setup.ang.y=0;
		draw_setup.ang.z=0;
		
		draw_setup.sway.x=pose_move_2->sway.x+((pose_move_1->sway.x-pose_move_2->sway.x)*pose_factor);
		draw_setup.sway.y=pose_move_2->sway.y+((pose_move_1->sway.y-pose_move_2->sway.y)*pose_factor);
		draw_setup.sway.z=pose_move_2->sway.z+((pose_move_1->sway.z-pose_move_2->sway.z)*pose_factor);
		draw_setup.move.x=pose_move_2->mov.x+((pose_move_1->mov.x-pose_move_2->mov.x)*pose_factor);
		draw_setup.move.y=pose_move_2->mov.y+((pose_move_1->mov.y-pose_move_2->mov.y)*pose_factor);
		draw_setup.move.z=pose_move_2->mov.z+((pose_move_1->mov.z-pose_move_2->mov.z)*pose_factor);
	}
}

void main_wind_draw_play(void)
{
	int					n,cur_tick;
	
		// if no current animation, just do no pose for animated textures
		
	if ((cur_animate==-1) && (!state.play_animate_blend)) {
		main_wind_draw();
		return;
	}
	
		// clear the draw setup
		
	model_draw_setup_clear(&model,&draw_setup);
	
		// calc the pose
		
	cur_tick=time_get();
	
	if (!state.play_animate_blend) {
		main_wind_draw_play_calc_animation(cur_tick,cur_animate,0,TRUE);
	}
	else {
		for (n=0;n!=max_model_blend_animation;n++) {
			if (state.blend[n].animate_idx!=-1) {
				main_wind_draw_play_calc_animation(cur_tick,state.blend[n].animate_idx,n,(n==0));
			}
		}
	}
		
		// global draw setup
	
	draw_model_wind(&model,cur_mesh,&draw_setup);
}

void main_wind_draw(void)
{
		// clear gl buffer
		
	glDisable(GL_SCISSOR_TEST);
	
	glClearColor(1.0f,1.0f,1.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
		// model
		
	if (!state.playing) {
		draw_model_wind_pose(&model,cur_mesh,cur_pose);
	}
	else {
		main_wind_draw_play();
	}

		// palettes
		
	tool_palette_draw();
	texture_palette_draw(model.textures);
	
		// swap GL buffer
		
	aglSwapBuffers(ctx);
}

void main_wind_draw_no_swap(void)
{
		// clear gl buffer
		
	glDisable(GL_SCISSOR_TEST);
	
	glClearColor(1.0f,1.0f,1.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
		// model
		
	if (!state.playing) {
		draw_model_wind_pose(&model,cur_mesh,cur_pose);
	}
	else {
		main_wind_draw_play();
	}

		// palettes
		
	tool_palette_draw();
	texture_palette_draw(model.textures);
}

/* =======================================================

      Info Window Events
      
======================================================= */

OSStatus model_wind_event_handler(EventHandlerCallRef eventhandler,EventRef event,void *userdata)
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
					model_wind_resize();
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
					model_wind_reset_modifiers();
					
					return(noErr);
					
				case kEventWindowClose:
					if (!menu_save_changes_dialog()) return(noErr);
					close_model_xml();
					model_wind_play(FALSE,FALSE);
					fix_menus();
					return(noErr);
					
                case kEventWindowCursorChange:
					GetEventParameter(event,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(unsigned long),NULL,&modifiers);
					model_wind_cursor(modifiers);
                    return(noErr);
					
			}
			break;
			
		case kEventClassKeyboard:
		
			switch (GetEventKind(event)) {
			
				case kEventRawKeyUp:
					GetEventParameter(event,kEventParamKeyMacCharCodes,typeChar,NULL,sizeof(char),NULL,&ch);
					model_wind_key(ch,TRUE);
					return(noErr);
					
				case kEventRawKeyDown:
					GetEventParameter(event,kEventParamKeyMacCharCodes,typeChar,NULL,sizeof(char),NULL,&ch);
					model_wind_key(ch,FALSE);
					return(noErr);
					
				case kEventRawKeyModifiersChanged:
					GetEventParameter(event,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(unsigned long),NULL,&modifiers);
					key_modifier_model_wind(modifiers);
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
					magnify_z+=(delta*20);
					main_wind_draw();
					return(noErr);
					
			}
			break;
			
	}

	return(eventNotHandledErr);
}

/* =======================================================

      Model Playering and Timer
      
======================================================= */

void model_wind_play(bool play,bool blend)
{
	int					n,tick;
	
		// good animation?
		
	if (play) {
		if (cur_animate==-1) return;
		if (model.animates[cur_animate].npose_move==0) return;
	}
	
		// always turn off animation until setup is complete
		// as animation is on a timer
		
	state.playing=FALSE;
	
		// setup animation
		
	state.play_animate_blend=blend;
	
	time_start();
	tick=time_get();

	for (n=0;n!=max_model_blend_animation;n++) {
		state.blend[n].pose_move_idx=0;
		state.blend[n].tick=tick;
	}
	
		// turn on/off animation
		
	state.playing=play;
	
	if (fileopen) main_wind_draw();
}

void model_wind_timer(EventLoopTimerRef inTimer,void *inUserData)
{
	if (state.playing) main_wind_draw();
}

/* =======================================================

      UI Icon Loader
      
======================================================= */

IconFamilyHandle main_wind_load_ui_icon(char *name)
{
	char						path[1024];
	FSRef						fsref;
	IconFamilyHandle			iconfamily;

	file_paths_app(&file_path_setup,path,"Contents/Resources",name,"icns");
	FSPathMakeRef((unsigned char*)path,&fsref,NULL);
	
	ReadIconFromFSRef(&fsref,&iconfamily);

	return(iconfamily);
}

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

      Model Window
      
======================================================= */

void model_wind_open(void)
{
	Rect						wbox,box;
	GLint						attrib[]={AGL_RGBA,AGL_DOUBLEBUFFER,AGL_DEPTH_SIZE,16,AGL_ALL_RENDERERS,AGL_NONE};
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
	
		// setup and cursors
		
	shift_on=FALSE;
	rotate_on=FALSE;
	size_on=FALSE;
	drag_sel_on=FALSE;
	
	magnify_z=3000;
	
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
	
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glClearColor(0.9f,0.9f,0.9f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
		// setup models and palettes
		
	tool_palette_initialize("Animator");
	tool_palette_setup();
	
	texture_palette_setup();
	
	model_wind_setup();
	
		// vertex masks
		
	vertex_mask_initialize();
	
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
	
		// animation setup
		
	cur_animate=-1;
	model_wind_play(FALSE,FALSE);

		// events
		
	model_wind_upp=NewEventHandlerUPP(model_wind_event_handler);
	InstallEventHandler(GetWindowEventTarget(wind),model_wind_upp,GetEventTypeCount(wind_events),wind_events,NULL,&model_wind_event);
		
	model_timer_upp=NewEventLoopTimerUPP(model_wind_timer);
	InstallEventLoopTimer(GetCurrentEventLoop(),0,0.01,model_timer_upp,NULL,&model_timer_event);
}

void model_wind_close(void)
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

		// close tool palette
		
	tool_palette_shutdown();
	
		// free vertex masks
		
	vertex_mask_shutdown();
	
		// close OpenGL contexts
		
	aglSetCurrentContext(NULL);
	
	aglSetDrawable(ctx,NULL);
	aglDestroyContext(ctx);
	
		// close window

	DisposeWindow(wind);
}

