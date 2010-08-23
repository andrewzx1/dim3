/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Map Window

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

extern d3rect			txt_palette_box;

extern map_type			map;
extern setup_type		setup;

d3rect					main_wind_box;

WindowRef				mainwind;
EventHandlerRef			main_wind_event;
EventHandlerUPP			main_wind_upp;
ControlRef				tool_ctrl[tool_count],piece_ctrl[piece_count];
IconRef					tool_icon_ref[tool_count],piece_icon_ref[piece_count];
MenuRef					grid_menu;

AGLContext				ctx;

file_path_setup_type	file_path_setup;
editor_state_type		state;

char					tool_icns_file_name[tool_count][64]=
								{
									"Tool Move Points",
									"Tool Move Points Together",
									"Tool Snap Points",
									"Tool Free Look",
									"Tool Toggle Mode",
									"Tool Edit Mesh",
									"Tool Edit Polygons",
									"Tool Edit Vertexes",
									"Tool Combine Meshes",
									"Tool Split Mesh",
									"Tool Tesselate Mesh",
									"Tool Small Grid",
									"Tool Auto-Texture Mesh",
									"Tool Rotate Mode",
									"Tool Move Mode",
									"Tool Node Select",
									"Tool Node Link",
									"Tool Node Remove Link",
									"Tool Normals",
									"Tool Cull",
									"Tool Edit Map Script",
									"Tool Run Map"
								};
								
char					tool_tooltip_str[tool_count][64]=
								{
									"Move Non-Mesh Points Freely\nSwith Mode with Q",
									"Move Non-Mesh Points Together\nSwith Mode with Q",
									"Snap Non-Mesh Points\nSwith Mode with Q",
									"Free Look Lock",
									"Always Add To Selection Lock",
									"Edit Entire Mesh\nSwitch Mode with W or Middle Button",
									"Edit Polygon Only\nSwitch Mode with W or Middle Button",
									"Edit Vertex Only\nSwitch Mode with W or Middle Button",
									"Combine Meshes",
									"Split Meshes",
									"Tesselate Mesh",
									"Grid\nSwitch Mode with E",
									"Auto-Texture Meshes",
									"Handles Rotate Item",
									"Handles Move Item",
									"Node Selects by Click\nSwith Mode with R",
									"Node Links by Click\nSwith Mode with R",
									"Node Removes Links by Click\nSwith Mode with R",
									"Show or Hide Normals",
									"Show or Hide Backface Culling",
									"Edit Map Script",
									"Run Map In Engine"
								};

char					piece_icns_file_name[piece_count][64]=
								{
									"Piece Spot",
									"Piece Light",
									"Piece Sound",
									"Piece Particle",
									"Piece Scenery",
									"Piece Node",
									"Piece Mesh",
									"Piece Mesh UV",
									"Piece Height Map",
									"Piece Grid",
									"Piece Polygon",
									"Piece Liquid",
								};
								
char					piece_tooltip_str[piece_count][64]=
								{
									"Add Spot","Add Light","Add Sound",
									"Add Particle","Add Scenery","Add Node",
									"Add OBJ Mesh","Replace Mesh UV",
									"Add Height Map Mesh","Add Grid Mesh",
									"Add Single Polygon Mesh","Add Liquid Volume",
								};

/* =======================================================

      Tool Window Events
      
======================================================= */

void main_wind_control_tool(int tool_idx)
{
	short		menu_idx;
	
	switch (tool_idx) {
	
			// vertex mode
			
		case 0:
			state.vertex_mode=vertex_mode_none;
			SetControlValue(tool_ctrl[0],1);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],0);
			break;
			
		case 1:
			state.vertex_mode=vertex_mode_lock;
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],1);
			SetControlValue(tool_ctrl[2],0);
			break;
			
		case 2:
			state.vertex_mode=vertex_mode_snap;
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],1);
			break;
			
			// free look and selection toggle
			
		case 3:
			state.free_look=!state.free_look;
			break;

		case 4:
			state.select_add=!state.select_add;
			break;
			
			// drag mode buttons
			
		case 5:
			state.drag_mode=drag_mode_mesh;
			SetControlValue(tool_ctrl[5],1);
			SetControlValue(tool_ctrl[6],0);
			SetControlValue(tool_ctrl[7],0);
			palette_reset();
			break;
			
		case 6:
			state.drag_mode=drag_mode_polygon;
			SetControlValue(tool_ctrl[5],0);
			SetControlValue(tool_ctrl[6],1);
			SetControlValue(tool_ctrl[7],0);
			palette_reset();
			break;
			
		case 7:
			state.drag_mode=drag_mode_vertex;
			SetControlValue(tool_ctrl[5],0);
			SetControlValue(tool_ctrl[6],0);
			SetControlValue(tool_ctrl[7],1);
			palette_reset();
			break;
			
			// mesh polygons
			
		case 8:
			SetControlValue(tool_ctrl[tool_idx],0);
			piece_combine_mesh();
			break;
			
		case 9:
			SetControlValue(tool_ctrl[tool_idx],0);
			piece_split_mesh();
			break;

		case 10:
			SetControlValue(tool_ctrl[tool_idx],0);
			piece_tesselate();
			break;
			
			
			// grid and auto-texture
			
		case 11:
			GetBevelButtonMenuValue(tool_ctrl[tool_idx],&menu_idx);
			state.grid_mode=((int)menu_idx)-1;
			SetControlValue(tool_ctrl[tool_idx],0);
			break;
			
		case 12:
			state.auto_texture=!state.auto_texture;
			break;
			
			// handle modes
			
		case 13:
			state.handle_mode=handle_mode_rotate;
			SetControlValue(tool_ctrl[13],1);
			SetControlValue(tool_ctrl[14],0);
			break;
			
		case 14:
			state.handle_mode=handle_mode_move;
			SetControlValue(tool_ctrl[13],0);
			SetControlValue(tool_ctrl[14],1);
			break;
			
			// node editing
			
		case 15:
			state.show_node=TRUE;
			menu_update_view();
			state.node_mode=node_mode_select;
			SetControlValue(tool_ctrl[15],1);
			SetControlValue(tool_ctrl[16],0);
			SetControlValue(tool_ctrl[17],0);
			break;
			
		case 16:
			state.show_node=TRUE;
			menu_update_view();
			state.node_mode=node_mode_link;
			SetControlValue(tool_ctrl[15],0);
			SetControlValue(tool_ctrl[16],1);
			SetControlValue(tool_ctrl[17],0);
			break;
			
		case 17:
			state.show_node=TRUE;
			menu_update_view();
			state.node_mode=node_mode_remove_link;
			SetControlValue(tool_ctrl[15],0);
			SetControlValue(tool_ctrl[16],0);
			SetControlValue(tool_ctrl[17],1);
			break;
			
			// normals
			
		case 18:
			state.show_normals=!state.show_normals;
			break;
			
		case 19:
			walk_view_cull(GetControlValue(tool_ctrl[tool_idx])!=0);
			break;
			
			// script and run buttons
			
		case 20:
			SetControlValue(tool_ctrl[tool_idx],0);
			launch_map_script_editor();
			break;
			
		case 21:
			SetControlValue(tool_ctrl[tool_idx],0);
			launch_engine();
			break;
			
	}
	
	menu_fix_enable();
	main_wind_draw();
}

void main_wind_control_piece(int piece_idx)
{
	switch (piece_idx) {

		case 0:
			piece_create_spot();
			break;
			
		case 1:
			piece_create_light();
			break;
			
		case 2:
			piece_create_sound();
			break;
			
		case 3:
			piece_create_particle();
			break;
			
		case 4:
			piece_create_scenery();
			break;
			
		case 5:
			piece_create_node();
			break;
			
		case 6:
			piece_add_obj_mesh();
			break;
			
		case 7:
			piece_add_obj_mesh_uv();
			break;
			
		case 8:
			piece_add_height_map_mesh();
			break;
			
		case 9:
			piece_add_grid_mesh();
			break;
			
		case 10:
			piece_add_polygon_mesh();
			break;
			
		case 11:
			piece_create_liquid();
			break;
			
	}
	
	SetControlValue(piece_ctrl[piece_idx],0);
	
	palette_reset();
}

void main_wind_control(ControlRef ctrl)
{
	int				n;
			
	for (n=0;n!=tool_count;n++) {
		if (ctrl==tool_ctrl[n]) {
			main_wind_control_tool(n);
			return;
		}
	}

		
	for (n=0;n!=piece_count;n++) {
		if (ctrl==piece_ctrl[n]) {
			main_wind_control_piece(n);
			return;
		}
	}
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
	EventMouseWheelAxis	axis;
	ControlRef			ctrl;
	d3pnt				dpt;
	
	switch (GetEventClass(event)) {
	
		case kEventClassWindow:
		
			switch (GetEventKind(event)) {
			
				case kEventWindowDrawContent:
					main_wind_draw();
					DrawControls(mainwind);
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

                    SetPort(GetWindowPort(mainwind));
                    GlobalToLocal(&pt);
					
					GetWindowPortBounds(mainwind,&wbox);
					
					dpt.x=pt.h;
					dpt.y=pt.v;
					
						// click in toolbars
						// supergumba -- will need some editing (possibly) when adding right side properties

					if (pt.v<toolbar_high) return(eventNotHandledErr);

						// middle button vertex change mode
						
					GetEventParameter(event,kEventParamMouseButton,typeMouseButton,NULL,sizeof(unsigned short),NULL,&btn);
					
					if (btn==kEventMouseButtonTertiary) {
						main_wind_tool_switch_drag_mode();
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
					SetPort(GetWindowPort(mainwind));
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
			
		case kEventClassControl:
			GetEventParameter(event,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&ctrl);
			main_wind_control(ctrl);
			return(noErr);
			
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
		
	GetWindowPortBounds(mainwind,&wbox);

	main_wind_box.lx=0;
	main_wind_box.rx=wbox.right;
	main_wind_box.ty=toolbar_high;
	main_wind_box.by=wbox.bottom;
	
		// buffer rect clipping
		
	rect[0]=0;
	rect[1]=0;
	rect[2]=wbox.right-wbox.left;
	rect[3]=(wbox.bottom-wbox.top)-toolbar_high;
	
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
	int							n,menu_id,behavior;
	Rect						wbox,box;
	GLint						attrib[]={AGL_NO_RECOVERY,AGL_RGBA,AGL_DOUBLEBUFFER,AGL_ACCELERATED,AGL_PIXEL_SIZE,24,AGL_ALPHA_SIZE,8,AGL_DEPTH_SIZE,16,AGL_STENCIL_SIZE,8,AGL_NONE};
	GDHandle					gdevice;
	AGLPixelFormat				pf;
	ControlButtonContentInfo	icon_info;
	HMHelpContentRec			tag;
	CFStringRef					cf_str;
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
												{kEventClassMouse,kEventMouseWheelMoved},
												{kEventClassControl,kEventControlHit}};
	
        // open window
        
    GetAvailableWindowPositioningBounds(GetMainDevice(),&wbox);

	SetRect(&wbox,wbox.left,(wbox.top+25),wbox.right,wbox.bottom);
	CreateNewWindow(kDocumentWindowClass,kWindowCloseBoxAttribute|kWindowCollapseBoxAttribute|kWindowFullZoomAttribute|kWindowResizableAttribute|kWindowLiveResizeAttribute|kWindowStandardHandlerAttribute|kWindowInWindowMenuAttribute,&wbox,&mainwind);
	
		// toolbar controls
		
	SetRect(&box,0,0,tool_button_size,tool_button_size);
    
	for (n=0;n!=tool_count;n++) {
		
			// grid tool has menu
			
		menu_id=0;
		behavior=kControlBehaviorToggles;
		
		if (n==11) {
			CreateNewMenu(tool_grid_menu_id,0,&grid_menu);
			InsertMenu(grid_menu,kInsertHierarchicalMenu);
	
			cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"No Grid",kCFStringEncodingMacRoman);
			AppendMenuItemTextWithCFString(grid_menu,cf_str,0,FOUR_CHAR_CODE('gd00'),NULL);
			CFRelease(cf_str);
			cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"Small Grid",kCFStringEncodingMacRoman);
			AppendMenuItemTextWithCFString(grid_menu,cf_str,0,FOUR_CHAR_CODE('gd01'),NULL);
			CFRelease(cf_str);
			cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"Medium Grid",kCFStringEncodingMacRoman);
			AppendMenuItemTextWithCFString(grid_menu,cf_str,0,FOUR_CHAR_CODE('gd02'),NULL);
			CFRelease(cf_str);
			cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"Large Grid",kCFStringEncodingMacRoman);
			AppendMenuItemTextWithCFString(grid_menu,cf_str,0,FOUR_CHAR_CODE('gd03'),NULL);
			CFRelease(cf_str);
			
			menu_id=tool_grid_menu_id;
			behavior=kControlBehaviorPushbutton;
		}
	
			// create button
			
		tool_icon_ref[n]=main_wind_load_icon_ref(tool_icns_file_name[n]);
		icon_info.contentType=kControlContentIconRef;
		icon_info.u.iconRef=tool_icon_ref[n];
		
		CreateBevelButtonControl(mainwind,&box,NULL,kControlBevelButtonSmallBevel,behavior,&icon_info,menu_id,0,0,&tool_ctrl[n]);
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
		if ((n==2) || (n==4) || (n==7) || (n==10) || (n==12) || (n==14) || (n==17) || (n==19)) OffsetRect(&box,3,0);
	}
	

		// piece buttons
		
	box.left=wbox.right-(piece_count*tool_button_size);
	box.right=box.left+tool_button_size;
	
	for (n=0;n!=piece_count;n++) {
				
			// create button
			
		piece_icon_ref[n]=main_wind_load_icon_ref(piece_icns_file_name[n]);
		icon_info.contentType=kControlContentIconRef;
		icon_info.u.iconRef=piece_icon_ref[n];
		
		CreateBevelButtonControl(mainwind,&box,NULL,kControlBevelButtonSmallBevel,kControlBehaviorPushbutton,&icon_info,0,0,0,&piece_ctrl[n]);
		SetBevelButtonGraphicAlignment(piece_ctrl[n],kControlBevelButtonAlignCenter,0,0);

			// create tooltip
			
		tag.version=kMacHelpVersion;
		tag.tagSide=kHMDefaultSide;
		SetRect(&tag.absHotRect,0,0,0,0);
		tag.content[kHMMinimumContentIndex].contentType=kHMCFStringContent;
		tag.content[kHMMinimumContentIndex].u.tagCFString=CFStringCreateWithCString(NULL,piece_tooltip_str[n],kCFStringEncodingMacRoman);
		tag.content[kHMMaximumContentIndex].contentType=kHMNoContent;
		
		HMSetControlHelpContent(piece_ctrl[n],&tag);
		
		OffsetRect(&box,tool_button_size,0);
    }

		// show window before additional setup
		
	ShowWindow(mainwind);
   
	SetPort(GetWindowPort(mainwind));
		
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
	
	aglSetDrawable(ctx,(AGLDrawable)GetWindowPort(mainwind));

		// setup view sizes
		
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
	InstallEventHandler(GetWindowEventTarget(mainwind),main_wind_upp,GetEventTypeCount(wind_events),wind_events,NULL,&main_wind_event);
	
		// start palettes
		
	palette_initialize();
}

void main_wind_close(void)
{
	int			n;
	
		// close the palettes
		
	palette_shutdown();
	
		// gl shutdown
		
	aglSetCurrentContext(NULL);
	aglSetDrawable(ctx,NULL);
	aglDestroyContext(ctx);
	
        // close walkview icons
        
	walk_view_shutdown();
	
		// dispose controls

	for (n=0;n!=tool_count;n++) {
		DisposeControl(tool_ctrl[n]);
		ReleaseIconRef(tool_icon_ref[n]);
	}
	
	for (n=0;n!=piece_count;n++) {
		DisposeControl(piece_ctrl[n]);
		ReleaseIconRef(piece_icon_ref[n]);
	}
	
		// tool menus
		
	DeleteMenu(tool_grid_menu_id);
	DisposeMenu(grid_menu);
	
        // dispose of events and window
        
	RemoveEventHandler(main_wind_event);
	DisposeEventHandlerUPP(main_wind_upp);
	DisposeWindow(mainwind);
}

void main_wind_set_title(char *file_name)
{
	char			*c,wname[256];
	unsigned char	p_str[256];
	
	strcpy(wname,file_name);
	c=strchr(wname,'.');
	if (c!=NULL) *c=0x0;
	
	CopyCStringToPascal(wname,p_str);
	SetWTitle(mainwind,p_str);
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
	SetPort(GetWindowPort(mainwind));
    
	GetWindowPortBounds(mainwind,&wbox);
	EraseRect(&wbox);
	
	SetPort(saveport);

		// fix all views and palettes
		
	aglUpdateContext(ctx);

	texture_palette_setup();
	
	main_wind_setup();
	DrawControls(mainwind);

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

		// texture window
		
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
		main_wind_tool_switch_vertex_mode();
		return;
	}
	
	if ((ch=='w') || (ch=='W')) {
		main_wind_tool_switch_drag_mode();
		return;
	}
	
	if ((ch=='e') || (ch=='E')) {
		main_wind_tool_switch_grid_mode();
		return;
	}
	
	if ((ch=='r') || (ch=='R')) {
		main_wind_tool_switch_node_mode();
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

/* =======================================================

      Values and States
      
======================================================= */

void main_wind_tool_reset(void)
{
	editor_view_type		*view;
	
		// vertex mode
		
	SetControlValue(tool_ctrl[0],(state.vertex_mode==vertex_mode_none)?1:0);
	SetControlValue(tool_ctrl[1],(state.vertex_mode==vertex_mode_lock)?1:0);
	SetControlValue(tool_ctrl[2],(state.vertex_mode==vertex_mode_snap)?1:0);
	
		// select toggle mode
		
	SetControlValue(tool_ctrl[3],state.free_look?1:0);
	SetControlValue(tool_ctrl[4],state.select_add?1:0);
	
		// drag mode
		
	SetControlValue(tool_ctrl[5],(state.drag_mode==drag_mode_mesh)?1:0);
	SetControlValue(tool_ctrl[6],(state.drag_mode==drag_mode_polygon)?1:0);
	SetControlValue(tool_ctrl[7],(state.drag_mode==drag_mode_vertex)?1:0);
	
		// auto-texture
		
	SetControlValue(tool_ctrl[12],state.auto_texture?1:0);
	
		// handle mode
		
	SetControlValue(tool_ctrl[13],state.handle_mode==handle_mode_rotate);
	SetControlValue(tool_ctrl[14],state.handle_mode==handle_mode_move);

		// node mode
		
	SetControlValue(tool_ctrl[15],(state.node_mode==node_mode_select)?1:0);
	SetControlValue(tool_ctrl[16],(state.node_mode==node_mode_link)?1:0);
	SetControlValue(tool_ctrl[17],(state.node_mode==node_mode_remove_link)?1:0);
	
		// normals
		
	view=walk_view_get_current_view();
		
	SetControlValue(tool_ctrl[18],(state.show_normals)?1:0);
	SetControlValue(tool_ctrl[19],(view->cull)?1:0);
	
		// grid mode
		
	SetBevelButtonMenuValue(tool_ctrl[11],(state.grid_mode+1));
}

void main_wind_tool_default(void)
{
	state.free_look=setup.free_look;
	state.select_add=FALSE;
	
    state.vertex_mode=vertex_mode_none;
	state.drag_mode=drag_mode_mesh;
	state.grid_mode=grid_mode_small;
	state.node_mode=node_mode_select;
	state.handle_mode=handle_mode_rotate;
	
	state.auto_texture=setup.auto_texture;
	
	state.show_liquid=TRUE;
	state.show_object=TRUE;
	state.show_lightsoundparticle=TRUE;
	state.show_node=FALSE;
	
	state.show_normals=FALSE;
    
	menu_update_view();
    main_wind_tool_reset();
}

void main_wind_tool_switch_vertex_mode(void)
{
	state.vertex_mode++;
	if (state.vertex_mode>vertex_mode_snap) state.vertex_mode=vertex_mode_none;
	
	main_wind_tool_reset();
}

void main_wind_tool_switch_drag_mode(void)
{
	state.drag_mode++;
	if (state.drag_mode>drag_mode_vertex) state.drag_mode=drag_mode_mesh;

	main_wind_tool_reset();
	main_wind_draw();
}

void main_wind_tool_switch_grid_mode(void)
{
	state.grid_mode++;
	if (state.grid_mode>grid_mode_large) state.grid_mode=grid_mode_none;

	main_wind_tool_reset();
}

void main_wind_tool_switch_node_mode(void)
{
	state.node_mode++;
	if (state.node_mode>node_mode_remove_link) state.node_mode=node_mode_select;

	main_wind_tool_reset();
}

/* =======================================================

      OS Specific Utility Routines
      
======================================================= */

void os_get_icon_file_path(char *path)
{
	strcpy(path,"Contents/Resources/Icons");
}

void os_get_window_box(d3rect *box)
{
	Rect			wbox;
	
	GetWindowPortBounds(mainwind,&wbox);

	box->lx=wbox.left;
	box->rx=wbox.right;
	box->ty=wbox.top;
	box->by=wbox.bottom;
}

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

void os_enable_menu_item_undo(bool enable)
{
	if (enable) {
		EnableMenuItem(GetMenuRef(app_menu_edit),1);
	}
	else {
		DisableMenuItem(GetMenuRef(app_menu_edit),1);
	}
}

bool os_button_down(void)
{
	return(Button());
}

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

