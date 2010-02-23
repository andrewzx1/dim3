/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Main Model Window

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "model.h"
#include "window.h"

WindowRef						model_wind;
EventHandlerRef					model_wind_event;
EventHandlerUPP					model_wind_upp;
EventLoopTimerRef				model_timer_event;
EventLoopTimerUPP				model_timer_upp;
ControlRef						magnify_slider;
ControlActionUPP				magnify_proc;
ControlRef						tool_ctrl[tool_count];
IconRef							tool_icon_ref[tool_count];

char							tool_icns_file_name[tool_count][64]=
									{
										"Tool Textured",
										"Tool Mesh",
										"Tool Bones",
										"Tool Texture Bones",
										"Tool Mesh Bones",
										"Tool Mesh Hit Boxes",
										"Tool Show First Mesh",
										"Tool Boxes",
										"Tool Normals",
										"Tool Rotate Mode",
										"Tool Move Mode",
										"Tool Play"
									};
									
char							tool_tooltip_str[tool_count][64]=
									{
										"Edit Textured Model",
										"Edit Mesh",
										"Edit Bones",
										"Edit Textured Model and Bones",
										"Edit Mesh and Bones",
										"Edit Hit Boxes",
										"Always Show First Mesh",
										"Show View Box",
										"Show Normals",
										"Rotate Bones Mode",
										"Stretch Bones Mode",
										"Play Current Animation"
									};

int						draw_type,cur_mesh,cur_bone,cur_pose,cur_animate,
                        shift_x,shift_y,magnify_z,drag_bone_mode,
						gl_view_x_sz,gl_view_y_sz,gl_view_texture_palette_size,
						play_animate_tick[max_model_blend_animation],
						play_animate_blend_idx[max_model_blend_animation],
						play_animate_pose_move_idx[max_model_blend_animation];
float					ang_y,ang_x;
bool					play_animate,play_animate_blend,
						model_view_reset,shift_on,rotate_on,size_on,drag_sel_on,vertex_on,
						model_box_on,model_normal_on,model_bone_drag_on,model_show_first_mesh;
Rect					drag_sel_box;
AGLContext				ctx,texture_ctx;

model_type				model;
model_draw_setup		draw_setup;

extern bool					fileopen;
extern file_path_setup_type	file_path_setup;

/* =======================================================

      Model Window Keyboard Control
      
======================================================= */

void model_wind_key(char ch,bool up)
{
	if (!fileopen) return;
	
		// esc key deselects
		
	if ((ch==0x1B) && (!up)) {
		vertex_clear_sel_mask(cur_mesh);
		draw_model_wind_pose(&model,cur_mesh,cur_pose);
		return;
	}
	
		// space moves around
		
	if (ch==0x20) {
		if (up) {
			shift_on=FALSE;
			SetThemeCursor(kThemeArrowCursor);
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
	
	if (!shift_on) SetThemeCursor(kThemeArrowCursor);
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
	
	SetThemeCursor(kThemeArrowCursor);
}

/* =======================================================

      Window Controls
      
======================================================= */

bool model_wind_control(ControlRef ctrl)
{
	int				i,idx;
	
	idx=-1;
	for (i=0;i!=tool_count;i++) {
		if (ctrl==tool_ctrl[i]) {
			idx=i;
			break;
		}
	}
	
	if (idx==-1) return(FALSE);
	
	switch (idx) {
	
		case 0:
			SetControlValue(tool_ctrl[0],1);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],0);
			SetControlValue(tool_ctrl[3],0);
			SetControlValue(tool_ctrl[4],0);
			SetControlValue(tool_ctrl[5],0);
			draw_type=dt_model;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			break;
			
		case 1:
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],1);
			SetControlValue(tool_ctrl[2],0);
			SetControlValue(tool_ctrl[3],0);
			SetControlValue(tool_ctrl[4],0);
			SetControlValue(tool_ctrl[5],0);
			draw_type=dt_mesh;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			break;
			
		case 2:
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],1);
			SetControlValue(tool_ctrl[3],0);
			SetControlValue(tool_ctrl[4],0);
			SetControlValue(tool_ctrl[5],0);
			draw_type=dt_bones;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			break;
			
		case 3:
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],0);
			SetControlValue(tool_ctrl[3],1);
			SetControlValue(tool_ctrl[4],0);
			SetControlValue(tool_ctrl[5],0);
			draw_type=dt_model_bones;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			break;
			
		case 4:
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],0);
			SetControlValue(tool_ctrl[3],0);
			SetControlValue(tool_ctrl[4],1);
			SetControlValue(tool_ctrl[5],0);
			draw_type=dt_mesh_bones;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			break;
			
		case 5:
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],0);
			SetControlValue(tool_ctrl[3],0);
			SetControlValue(tool_ctrl[4],0);
			SetControlValue(tool_ctrl[5],1);
			draw_type=dt_mesh_hit_boxes;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			break;
			
		case 6:
			model_show_first_mesh=!model_show_first_mesh;
			break;
			
		case 7:
			model_box_on=!model_box_on;
			break;
			
		case 8:
			model_normal_on=!model_normal_on;
			break;
			
		case 9:
			drag_bone_mode=drag_bone_mode_rotate;
			SetControlValue(tool_ctrl[10],1);
			SetControlValue(tool_ctrl[11],0);
			break;
			
		case 10:
			drag_bone_mode=drag_bone_mode_stretch;
			SetControlValue(tool_ctrl[10],0);
			SetControlValue(tool_ctrl[11],1);
			break;
			
		case 11:
			model_wind_play(!play_animate,FALSE);
			break;
	}
	
	draw_model_wind_pose(&model,cur_mesh,cur_pose);
	
	return(TRUE);
}

/* =======================================================

      Magnify Slider Action
      
======================================================= */

void model_wind_magnify_action(ControlRef ctrl,ControlPartCode code)
{
	int				mag_z;
	
	mag_z=((400-GetControlValue(ctrl))-200)*10;
	if (mag_z==magnify_z) return;
	
	magnify_z=mag_z;
	
	draw_model_wind_pose(&model,cur_mesh,cur_pose);
}

void model_wind_set_magnify(int mag_z)
{
	magnify_z=mag_z;
	if (magnify_z<-2000) magnify_z=-2000;
	if (magnify_z>2000) magnify_z=2000;

	SetControlValue(magnify_slider,(400-((magnify_z/10)+200)));
}

/* =======================================================

      Window Background Draw
      
======================================================= */

void model_wind_draw_background(void)
{
	Rect				wbox;
    RGBColor			blackcolor={0x0,0x0,0x0};
	GrafPtr				saveport;
	
		// get draw box
		
	GetPort(&saveport);
	SetPort(GetWindowPort(model_wind));

	GetWindowPortBounds(model_wind,&wbox);
	
		// draw lines
		
	RGBForeColor(&blackcolor);
	
	MoveTo(wbox.left,tool_button_size);
	LineTo(wbox.right,tool_button_size);

	MoveTo((wbox.left+(gl_view_x_sz+0)),(wbox.top+tool_height));
	LineTo((wbox.left+(gl_view_x_sz+0)),(wbox.bottom-info_palette_height));

	SetPort(saveport);
}

/* =======================================================

      Window Resize
      
======================================================= */

void model_wind_resize(void)
{
	Rect			box,cbox;
	
		// new model view size
		
	aglUpdateContext(ctx);
	aglUpdateContext(texture_ctx);
		
	GetWindowPortBounds(model_wind,&box);
	
	gl_view_x_sz=(box.right-box.left)-total_list_width;
	if (gl_view_x_sz<model_view_min_size) gl_view_x_sz=model_view_min_size;
	
	gl_view_texture_palette_size=(box.right-box.left)/max_model_texture;
	
	gl_view_y_sz=(box.bottom-box.top)-(tool_height+gl_view_texture_palette_size+info_palette_height);

		// resize controls

	box.left+=gl_view_x_sz;
	box.bottom-=info_palette_height;
		
	resize_pose_controls(&box);
	resize_bone_controls(&box);
	resize_animate_controls(&box);
	resize_mesh_controls(&box);
	resize_vertex_controls(&box);
	
	cbox.left=box.right-250;
	cbox.right=box.right-5;
	cbox.bottom=(box.top+tool_height)-5;
	cbox.top=box.top+5;
	
	MoveControl(magnify_slider,cbox.left,cbox.top);
	SizeControl(magnify_slider,(cbox.right-cbox.left),(cbox.bottom-cbox.top));
	
		// redraw

	model_wind_draw_background();
	draw_model_wind_pose(&model,cur_mesh,cur_pose);
	texture_palette_draw();
	info_palette_draw();
	
	DrawControls(model_wind);
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
	Point				pt;
	EventMouseWheelAxis	axis;
	ControlRef			ctrl;
	
	switch (GetEventClass(event)) {
	
		case kEventClassWindow:
		
			switch (GetEventKind(event)) {
			
				case kEventWindowDrawContent:
					model_wind_draw_background();
					draw_model_wind_pose(&model,cur_mesh,cur_pose);
					DrawControls(model_wind);
					return(noErr);
					
				case kEventWindowBoundsChanged:
					model_wind_resize();
					return(noErr);

				case kEventWindowClickContentRgn:
					GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,NULL,sizeof(Point),NULL,&pt);
					
					SetPort(GetWindowPort(model_wind));
					GlobalToLocal(&pt);

						// clicking in toolbar or right side controls

					if (pt.v<=tool_height) return(eventNotHandledErr);
					if (pt.h>=gl_view_x_sz) return(eventNotHandledErr);
					
						// clicking in palettes
						
					GetEventParameter(event,kEventParamClickCount,typeUInt32,NULL,sizeof(unsigned long),NULL,&nclick);
					 
					if ((pt.h>=0) && (pt.h<=gl_view_x_sz) && (pt.v>=(tool_height+gl_view_y_sz)) && (pt.v<=(tool_height+gl_view_y_sz+gl_view_texture_palette_size))) {
						texture_palette_click(pt,(nclick!=1));
						return(noErr);
					}
					
						// clicking in model view
						
					model_wind_offset_click(&pt);
					
					GetEventParameter(event,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(unsigned long),NULL,&modifiers);
					GetEventParameter(event,kEventParamMouseButton,typeMouseButton,NULL,sizeof(unsigned short),NULL,&btn);
					
					if (btn==kEventMouseButtonTertiary) {		// shortcut for mesh mode change
						model_wind_switch_mesh_mode();
						draw_model_wind_pose(&model,cur_mesh,cur_pose);
						return(noErr);
					}
					
					click_model_wind(pt,modifiers);
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
					SetPort(GetWindowPort(model_wind));
					GlobalToLocal(&pt);
					if (pt.v<=tool_height) return(eventNotHandledErr);
					if (pt.h>=gl_view_x_sz) return(eventNotHandledErr);
					
						// zoom
						
					GetEventParameter(event,kEventParamMouseWheelAxis,typeMouseWheelAxis,NULL,sizeof(EventMouseWheelAxis),NULL,&axis);
					if (axis!=kEventMouseWheelAxisY) return(noErr);
					GetEventParameter(event,kEventParamMouseWheelDelta,typeLongInteger,NULL,sizeof(long),NULL,&delta);
					model_wind_set_magnify(magnify_z-(delta*20));
					draw_model_wind_pose(&model,cur_mesh,cur_pose);
					return(noErr);
					
			}
			break;
			
		case kEventClassControl:
		
			GetEventParameter(event,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&ctrl);
			if (model_wind_control(ctrl)) return(noErr);
			
			return(eventNotHandledErr);
			
	}

	return(eventNotHandledErr);
}

/* =======================================================

      Model Timer
      
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
		
	play_animate=FALSE;
	
		// setup animation
		
	play_animate_blend=blend;
	
	time_start();
	tick=time_get();

	for (n=0;n!=max_model_blend_animation;n++) {
		play_animate_pose_move_idx[n]=0;
		play_animate_tick[n]=tick;
	}
	
	model_view_reset=FALSE;
	
		// turn on/off animation
		
	play_animate=play;
	
	model_wind_reset_tools();
}

void model_wind_play_calc_animation(int cur_tick,int animate_idx,int blend_idx,bool non_blend_setup)
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
	
	tick=play_animate_tick[blend_idx];
	pose_move_1_idx=play_animate_pose_move_idx[blend_idx];
	
	msec=animate->pose_moves[pose_move_1_idx].msec;
	nxt_tick=tick+msec;
	
	while (nxt_tick<cur_tick) {				// skip ahead till we find correct pose
		pose_move_1_idx++;
		if (pose_move_1_idx>last_pose) pose_move_1_idx=first_pose;
		
		msec=animate->pose_moves[pose_move_1_idx].msec;

		tick=nxt_tick;
		nxt_tick=tick+msec;
	}
	
	play_animate_tick[blend_idx]=tick;
	play_animate_pose_move_idx[blend_idx]=pose_move_1_idx;
	
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

void model_wind_timer(EventLoopTimerRef inTimer,void *inUserData)
{
	int					n,cur_tick;
	
		// if not playing, then do model view resets
		
	if (!play_animate) {
		if (model_view_reset) {
			model_view_reset=FALSE;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
		}
		return;
	}
	
		// if no current animation, just do no pose for animated textures
		
	if ((cur_animate==-1) && (!play_animate_blend)) {
		draw_model_wind_pose(&model,cur_mesh,-1);
		return;
	}
	
		// clear the draw setup
		
	model_draw_setup_clear(&model,&draw_setup);
	
		// calc the pose
		
	cur_tick=time_get();
	
	if (!play_animate_blend) {
		model_wind_play_calc_animation(cur_tick,cur_animate,0,TRUE);
	}
	else {
		for (n=0;n!=max_model_blend_animation;n++) {
			if (play_animate_blend_idx[n]!=-1) {
				model_wind_play_calc_animation(cur_tick,play_animate_blend_idx[n],n,(n==0));
			}
		}
	}
		
		// global draw setup
	
	draw_model_wind(&model,cur_mesh,&draw_setup);
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
	int							n;
	Rect						wbox,box;
	GLint						attrib[]={AGL_RGBA,AGL_DOUBLEBUFFER,AGL_DEPTH_SIZE,16,AGL_ALL_RENDERERS,AGL_NONE};
	AGLPixelFormat				pf;
	ControlButtonContentInfo	icon_info;
	HMHelpContentRec			tag;
	EventTypeSpec	wind_events[]={	{kEventClassWindow,kEventWindowDrawContent},
									{kEventClassWindow,kEventWindowBoundsChanged},
									{kEventClassWindow,kEventWindowClickContentRgn},
									{kEventClassWindow,kEventWindowClose},
									{kEventClassWindow,kEventWindowCursorChange},
									{kEventClassKeyboard,kEventRawKeyUp},
									{kEventClassKeyboard,kEventRawKeyDown},
									{kEventClassKeyboard,kEventRawKeyModifiersChanged},
									{kEventClassMouse,kEventMouseWheelMoved},
									{kEventClassControl,kEventControlHit}};
	
    GetAvailableWindowPositioningBounds(GetMainDevice(),&wbox);
	
	SetRect(&box,wbox.left,(wbox.top+25),wbox.right,wbox.bottom);
	CreateNewWindow(kDocumentWindowClass,kWindowStandardDocumentAttributes|kWindowLiveResizeAttribute|kWindowStandardHandlerAttribute|kWindowInWindowMenuAttribute,&box,&model_wind);
	SetWTitle(model_wind,"\pModel");
	ShowWindow(model_wind);
	
		// set font
		
	SetPort(GetWindowPort(model_wind));
	TextFont(FMGetFontFamilyFromName("\pMonaco"));
	TextSize(10);
	
		// setup and cursors
		
	cur_animate=-1;
	model_wind_play(FALSE,FALSE);
	
	shift_on=FALSE;
	rotate_on=FALSE;
	size_on=FALSE;
	drag_sel_on=FALSE;
	
		// get gl sizes
		
	GetWindowPortBounds(model_wind,&box);
	
	gl_view_x_sz=(wbox.right-wbox.left)-total_list_width;
	if (gl_view_x_sz<model_view_min_size) gl_view_x_sz=model_view_min_size;
	
	gl_view_texture_palette_size=(wbox.right-wbox.left)/max_model_texture;
	
	gl_view_y_sz=(box.bottom-box.top)-(tool_height+gl_view_texture_palette_size+info_palette_height);
	
		// toolbar buttons
			
	SetRect(&box,0,0,tool_button_size,tool_button_size);
	
	for (n=0;n!=tool_count;n++) {
	
			// create button
			
		tool_icon_ref[n]=main_wind_load_icon_ref(tool_icns_file_name[n]);
		icon_info.contentType=kControlContentIconRef;
		icon_info.u.iconRef=tool_icon_ref[n];
			
		CreateBevelButtonControl(model_wind,&box,NULL,kControlBevelButtonSmallBevel,kControlBehaviorToggles,&icon_info,0,0,0,&tool_ctrl[n]);
		SetBevelButtonGraphicAlignment(tool_ctrl[n],kControlBevelButtonAlignCenter,0,0);
	
			// create tooltip
			
		tag.version=kMacHelpVersion;
		tag.tagSide=kHMDefaultSide;
		SetRect(&tag.absHotRect,0,0,0,0);
		tag.content[kHMMinimumContentIndex].contentType=kHMCFStringContent;
		tag.content[kHMMinimumContentIndex].u.tagCFString=CFStringCreateWithCString(NULL,tool_tooltip_str[n],kCFStringEncodingMacRoman);
		tag.content[kHMMaximumContentIndex].contentType=kHMNoContent;
		
		HMSetControlHelpContent(tool_ctrl[n],&tag);

			// next button position
			
		OffsetRect(&box,tool_button_size,0);
		if ((n==5) || (n==9) || (n==11)) OffsetRect(&box,5,0);
	}
	
		// magnify slider
		
	GetWindowPortBounds(model_wind,&box);

	box.left=box.right-250;
	box.right-=5;
	box.bottom=(box.top+tool_height)-5;
	box.top+=5;
	
	magnify_proc=NewControlActionUPP(model_wind_magnify_action);
	CreateSliderControl(model_wind,&box,200,0,400,kControlSliderDoesNotPoint,0,TRUE,magnify_proc,&magnify_slider);
		
		// dragging for bones window
		
	SetAutomaticControlDragTrackingEnabledForWindow(model_wind,TRUE);
	
		// controls
		
	GetWindowPortBounds(model_wind,&box);

	box.left+=gl_view_x_sz;
	box.bottom-=info_palette_height;
	
	start_pose_controls(model_wind,&box);
	start_bone_controls(model_wind,&box);
    start_animate_controls(model_wind,&box);
	start_mesh_controls(model_wind,&box);
	start_vertex_controls(model_wind,&box);
	
		// model OpenGL contexts
		
	pf=aglChoosePixelFormat(NULL,0,attrib);
	
	ctx=aglCreateContext(pf,NULL);
	aglSetDrawable(ctx,(AGLDrawable)GetWindowPort(model_wind));
	
	texture_ctx=aglCreateContext(pf,ctx);
	aglSetDrawable(texture_ctx,(AGLDrawable)GetWindowPort(model_wind));
	
	aglSetCurrentContext(ctx);
	
	aglDestroyPixelFormat(pf);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glClearColor(0.9f,0.9f,0.9f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
		// get the controls draw

	DrawControls(model_wind);
	model_wind_draw_background();

		// events
		
	model_wind_upp=NewEventHandlerUPP(model_wind_event_handler);
	InstallEventHandler(GetWindowEventTarget(model_wind),model_wind_upp,GetEventTypeCount(wind_events),wind_events,NULL,&model_wind_event);
		
	model_timer_upp=NewEventLoopTimerUPP(model_wind_timer);
	InstallEventLoopTimer(GetCurrentEventLoop(),0,0.01,model_timer_upp,NULL,&model_timer_event);
}

void model_wind_close(void)
{
	int				n;
	
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
	
	DisposeControl(magnify_slider);
	DisposeControlActionUPP(magnify_proc);
		
	for (n=0;n!=tool_count;n++) {
		DisposeControl(tool_ctrl[n]);
		ReleaseIconRef(tool_icon_ref[n]);
	}

		// close OpenGL contexts
		
	aglSetCurrentContext(NULL);
	
	aglSetDrawable(texture_ctx,NULL);
	aglDestroyContext(texture_ctx);
	
	aglSetDrawable(ctx,NULL);
	aglDestroyContext(ctx);
	
		// close window

	DisposeWindow(model_wind);
}

/* =======================================================

      Window Click Offsets
      
======================================================= */

void model_wind_offset_click(Point *pt)
{
	pt->v-=tool_height;
}

/* =======================================================

      Model Window Controls
      
======================================================= */

void model_wind_set_title(char *title)
{
	unsigned char		p_str[256];
	
	CopyCStringToPascal(title,p_str);
	SetWTitle(model_wind,p_str);
}

void model_wind_reset_tools(void)
{
	SetControlValue(tool_ctrl[0],(draw_type==dt_model)?1:0);
	SetControlValue(tool_ctrl[1],(draw_type==dt_mesh)?1:0);
	SetControlValue(tool_ctrl[2],(draw_type==dt_bones)?1:0);
	SetControlValue(tool_ctrl[3],(draw_type==dt_model_bones)?1:0);
	SetControlValue(tool_ctrl[4],(draw_type==dt_mesh_bones)?1:0);
	SetControlValue(tool_ctrl[5],(draw_type==dt_mesh_hit_boxes)?1:0);
    
	SetControlValue(tool_ctrl[6],model_show_first_mesh?1:0);
	SetControlValue(tool_ctrl[7],model_box_on?1:0);
	SetControlValue(tool_ctrl[8],model_normal_on?1:0);
	
	SetControlValue(tool_ctrl[9],(drag_bone_mode==drag_bone_mode_rotate)?1:0);
	SetControlValue(tool_ctrl[10],(drag_bone_mode==drag_bone_mode_stretch)?1:0);

	SetControlValue(tool_ctrl[11],play_animate?1:0);
}

void model_wind_switch_mesh_mode(void)
{
	draw_type++;
	if (draw_type>dt_mesh_bones) draw_type=dt_model;
	
	SetControlValue(tool_ctrl[0],(draw_type==dt_model)?1:0);
	SetControlValue(tool_ctrl[1],(draw_type==dt_mesh)?1:0);
	SetControlValue(tool_ctrl[2],(draw_type==dt_bones)?1:0);
	SetControlValue(tool_ctrl[3],(draw_type==dt_model_bones)?1:0);
	SetControlValue(tool_ctrl[4],(draw_type==dt_mesh_bones)?1:0);
	SetControlValue(tool_ctrl[5],(draw_type==dt_mesh_hit_boxes)?1:0);
}

