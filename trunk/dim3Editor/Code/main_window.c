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

extern d3rect			txt_palette_box;

extern map_type			map;
extern setup_type		setup;

d3rect					main_wind_box;

WindowRef				mainwind;
EventHandlerRef			main_wind_event;
EventHandlerUPP			main_wind_upp;
ControlRef				tool_ctrl[tool_count],piece_ctrl[piece_count],magnify_slider;
ControlActionUPP		magnify_proc;
IconRef					tool_icon_ref[tool_count],piece_icon_ref[piece_count];
MenuRef					grid_menu,spot_menu,light_menu,sound_menu,particle_menu,
						scenery_menu,node_menu,area_menu;

AGLContext				ctx;

file_path_setup_type	file_path_setup;
editor_state_type		state;

int						spot_combo_lookup[max_spot],
						light_combo_lookup[max_map_light],
						sound_combo_lookup[max_map_sound],
						particle_combo_lookup[max_map_particle],
						scenery_combo_lookup[max_map_scenery],
						node_combo_lookup[max_node];

char					tool_icns_file_name[tool_count][64]=
								{
									"Tool Move Points",
									"Tool Move Points Together",
									"Tool Snap Points",
									"Tool Toggle Mode",
									"Tool Edit Mesh",
									"Tool Edit Polygons",
									"Tool Edit Vertexes",
									"Tool Combine Meshes",
									"Tool Split Mesh",
									"Tool Tesselate Mesh",
									"Tool Small Grid",
									"Tool Auto-Texture Mesh",
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
									"Select Toggle Mode",
									"Edit Entire Mesh\nSwitch Mode with W or Middle Button",
									"Edit Polygon Only\nSwitch Mode with W or Middle Button",
									"Edit Vertex Only\nSwitch Mode with W or Middle Button",
									"Combine Meshes",
									"Split Meshes",
									"Tesselate Mesh",
									"Grid\nSwitch Mode with E",
									"Auto-Texture Meshes",
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
			
			// select toggle mode
			
		case 3:
			state.select_toggle_mode=!state.select_toggle_mode;
			break;
			
			// drag mode buttons
			
		case 4:
			state.drag_mode=drag_mode_mesh;
			SetControlValue(tool_ctrl[4],1);
			SetControlValue(tool_ctrl[5],0);
			SetControlValue(tool_ctrl[6],0);
			palette_reset();
			break;
			
		case 5:
			state.drag_mode=drag_mode_polygon;
			SetControlValue(tool_ctrl[4],0);
			SetControlValue(tool_ctrl[5],1);
			SetControlValue(tool_ctrl[6],0);
			palette_reset();
			break;
			
		case 6:
			state.drag_mode=drag_mode_vertex;
			SetControlValue(tool_ctrl[4],0);
			SetControlValue(tool_ctrl[5],0);
			SetControlValue(tool_ctrl[6],1);
			palette_reset();
			break;
			
			// mesh polygons
			
		case 7:
			SetControlValue(tool_ctrl[tool_idx],0);
			piece_combine_mesh();
			break;
			
		case 8:
			SetControlValue(tool_ctrl[tool_idx],0);
			piece_split_mesh();
			break;

		case 9:
			SetControlValue(tool_ctrl[tool_idx],0);
			piece_tesselate();
			break;
			
			
			// grid and auto-texture
			
		case 10:
			GetBevelButtonMenuValue(tool_ctrl[tool_idx],&menu_idx);
			state.grid_mode=((int)menu_idx)-1;
			SetControlValue(tool_ctrl[tool_idx],0);
			break;
			
		case 11:
			state.auto_texture=!state.auto_texture;
			break;
			
			// node editing
			
		case 12:
			state.show_node=TRUE;
			menu_update_view();
			state.node_mode=node_mode_select;
			SetControlValue(tool_ctrl[12],1);
			SetControlValue(tool_ctrl[13],0);
			SetControlValue(tool_ctrl[14],0);
			break;
			
		case 13:
			state.show_node=TRUE;
			menu_update_view();
			state.node_mode=node_mode_link;
			SetControlValue(tool_ctrl[12],0);
			SetControlValue(tool_ctrl[13],1);
			SetControlValue(tool_ctrl[14],0);
			break;
			
		case 14:
			state.show_node=TRUE;
			menu_update_view();
			state.node_mode=node_mode_remove_link;
			SetControlValue(tool_ctrl[12],0);
			SetControlValue(tool_ctrl[13],0);
			SetControlValue(tool_ctrl[14],1);
			break;
			
			// normals
			
		case 15:
			state.show_normals=!state.show_normals;
			break;
			
		case 16:
			walk_view_cull(GetControlValue(tool_ctrl[tool_idx])!=0);
			break;
			
			// script and run buttons
			
		case 17:
			SetControlValue(tool_ctrl[tool_idx],0);
			launch_map_script_editor();
			break;
			
		case 18:
			SetControlValue(tool_ctrl[tool_idx],0);
			launch_engine();
			break;
			
	}
	
	menu_fix_enable();
	main_wind_draw();
}

void main_wind_control_piece(int piece_idx)
{
	int					idx;
	short				menu_idx;
	
	switch (piece_idx) {

		case 0:
			GetBevelButtonMenuValue(piece_ctrl[0],&menu_idx);
			
				// create a spot
				
			if (menu_idx==1) {
				piece_create_spot();
				break;
			}
			
				// find a spot
			
			if (menu_idx>2) {
				state.show_object=TRUE;
				
				idx=spot_combo_lookup[menu_idx-3];
				walk_view_set_position_y_shift(&map.spots[idx].pnt,-500);
				
				select_clear();
				select_add(spot_piece,idx,-1);
				
				main_wind_draw();	
			}
			break;
			
		case 1:
			GetBevelButtonMenuValue(piece_ctrl[1],&menu_idx);
			
				// create a light
				
			if (menu_idx==1) {
				piece_create_light();
				break;
			}
			
				// find a light
			
			if (menu_idx>2) {
				state.show_lightsoundparticle=TRUE;
				
				idx=light_combo_lookup[menu_idx-3];
				walk_view_set_position_y_shift(&map.lights[idx].pnt,-500);
				
				select_clear();
				select_add(light_piece,idx,-1);
				
				main_wind_draw();	
			}
			break;
			
		case 2:
			GetBevelButtonMenuValue(piece_ctrl[2],&menu_idx);
			
				// create a sound
				
			if (menu_idx==1) {
				piece_create_sound();
				break;
			}
			
				// find a sound
			
			if (menu_idx>2) {
				state.show_lightsoundparticle=TRUE;
				
				idx=sound_combo_lookup[menu_idx-3];
				walk_view_set_position_y_shift(&map.sounds[idx].pnt,-500);			
				
				select_clear();
				select_add(sound_piece,idx,-1);
				
				main_wind_draw();	
			}
			break;
			
		case 3:
			GetBevelButtonMenuValue(piece_ctrl[3],&menu_idx);
			
				// create a particle
				
			if (menu_idx==1) {
				piece_create_particle();
				break;
			}
			
				// find a particle
			
			if (menu_idx>2) {
				state.show_lightsoundparticle=TRUE;
				
				idx=particle_combo_lookup[menu_idx-3];
				walk_view_set_position_y_shift(&map.particles[idx].pnt,-500);			
				
				select_clear();
				select_add(particle_piece,idx,-1);
				
				main_wind_draw();	
			}
			break;
			
		case 4:
			GetBevelButtonMenuValue(piece_ctrl[4],&menu_idx);
			
				// create a spot
				
			if (menu_idx==1) {
				piece_create_scenery();
				break;
			}
			
				// find a spot
			
			if (menu_idx>2) {
				state.show_object=TRUE;
				
				idx=scenery_combo_lookup[menu_idx-3];
				walk_view_set_position_y_shift(&map.sceneries[idx].pnt,-500);			
				
				select_clear();
				select_add(scenery_piece,idx,-1);
				
				main_wind_draw();	
			}
			break;
			
		case 5:
			GetBevelButtonMenuValue(piece_ctrl[5],&menu_idx);
			
				// create a node
				
			if (menu_idx==1) {
				piece_create_node();
				break;
			}
			
				// find a node
			
			if (menu_idx>2) {
				state.show_node=TRUE;
				
				idx=node_combo_lookup[menu_idx-3];
				walk_view_set_position_y_shift(&map.nodes[idx].pnt,-500);
				
				select_clear();
				select_add(node_piece,idx,-1);
				
				main_wind_draw();	
			}
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

      Magnify Slider Action
      
======================================================= */

void main_wind_magnify_action(ControlRef ctrl,ControlPartCode code)
{
	int				magnify_factor;
	
	magnify_factor=GetControlValue(ctrl);
	if (magnify_factor==walk_view_get_magnify_factor()) return;
	
	walk_view_set_magnify_factor(magnify_factor);
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
		
		if (n==10) {
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
		if ((n==2) || (n==6) || (n==9) || (n==11) || (n==14) || (n==16)) OffsetRect(&box,3,0);
	}
	

		// piece buttons
		
	OffsetRect(&box,10,0);
	
	for (n=0;n!=piece_count;n++) {
	
			// misc menus
			
		menu_id=0;
		
		switch (n) {
		
				// spot menu
			
			case 0:
				CreateNewMenu(tool_spot_menu_id,kMenuAttrExcludesMarkColumn,&spot_menu);
				InsertMenu(spot_menu,kInsertHierarchicalMenu);
				menu_id=tool_spot_menu_id;
				break;
				
				// light menu
			
			case 1:
				CreateNewMenu(tool_light_menu_id,kMenuAttrExcludesMarkColumn,&light_menu);
				InsertMenu(light_menu,kInsertHierarchicalMenu);
				menu_id=tool_light_menu_id;
				break;
				
				// sound menu
			
			case 2:
				CreateNewMenu(tool_sound_menu_id,kMenuAttrExcludesMarkColumn,&sound_menu);
				InsertMenu(sound_menu,kInsertHierarchicalMenu);
				menu_id=tool_sound_menu_id;
				break;
				
				// particle menu
			
			case 3:
				CreateNewMenu(tool_particle_menu_id,kMenuAttrExcludesMarkColumn,&particle_menu);
				InsertMenu(particle_menu,kInsertHierarchicalMenu);
				menu_id=tool_particle_menu_id;
				break;
				
				// scenery menu
			
			case 4:
				CreateNewMenu(tool_scenery_menu_id,kMenuAttrExcludesMarkColumn,&scenery_menu);
				InsertMenu(scenery_menu,kInsertHierarchicalMenu);
				menu_id=tool_scenery_menu_id;
				break;
				
		
				// node menu
			
			case 5:
				CreateNewMenu(tool_node_menu_id,kMenuAttrExcludesMarkColumn,&node_menu);
				InsertMenu(node_menu,kInsertHierarchicalMenu);
				menu_id=tool_node_menu_id;
				break;
				
				// area menu
			
			case 12:
				CreateNewMenu(tool_area_menu_id,kMenuAttrExcludesMarkColumn,&area_menu);
				InsertMenu(area_menu,kInsertHierarchicalMenu);
				menu_id=tool_area_menu_id;
				break;
		}
			
			// create button
			
		piece_icon_ref[n]=main_wind_load_icon_ref(piece_icns_file_name[n]);
		icon_info.contentType=kControlContentIconRef;
		icon_info.u.iconRef=piece_icon_ref[n];
		
		CreateBevelButtonControl(mainwind,&box,NULL,kControlBevelButtonSmallBevel,kControlBehaviorPushbutton,&icon_info,menu_id,0,0,&piece_ctrl[n]);
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
	
		// magnify slider

	box.left+=10;
	box.right=(wbox.right-5);
	box.top+=5;
	box.bottom-=5;
	
	magnify_proc=NewControlActionUPP(main_wind_magnify_action);
	CreateSliderControl(mainwind,&box,magnify_factor_default,magnify_factor_min,magnify_factor_max,kControlSliderDoesNotPoint,0,TRUE,magnify_proc,&magnify_slider);

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
	state.select_toggle_mode=FALSE;
	
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
	
	DisposeControl(magnify_slider);
	DisposeControlActionUPP(magnify_proc);
	
		// tool menus
		
	DeleteMenu(tool_grid_menu_id);
	DisposeMenu(grid_menu);
	
	DeleteMenu(tool_spot_menu_id);
	DisposeMenu(spot_menu);
	
	DeleteMenu(tool_light_menu_id);
	DisposeMenu(light_menu);
	
	DeleteMenu(tool_sound_menu_id);
	DisposeMenu(sound_menu);
	
	DeleteMenu(tool_particle_menu_id);
	DisposeMenu(particle_menu);
	
	DeleteMenu(tool_scenery_menu_id);
	DisposeMenu(scenery_menu);

	DeleteMenu(tool_node_menu_id);
	DisposeMenu(node_menu);
	
	DeleteMenu(tool_area_menu_id);
	DisposeMenu(area_menu);

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

void main_wind_resize_buttons(void)
{
	Rect			wbox,box;
	
	GetWindowPortBounds(mainwind,&wbox);
		
	GetControlBounds(magnify_slider,&box);
	box.right=wbox.right-5;
		
	SetControlBounds(magnify_slider,&box);
}

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
	main_wind_resize_buttons();
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
		// switch to proper pane
		
	walk_view_select_view(pt);

		// run wheel change
	
	walk_view_scroll_wheel_z_movement(delta);
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
		
	SetControlValue(tool_ctrl[3],state.select_toggle_mode?1:0);
	
		// drag mode
		
	SetControlValue(tool_ctrl[4],(state.drag_mode==drag_mode_mesh)?1:0);
	SetControlValue(tool_ctrl[5],(state.drag_mode==drag_mode_polygon)?1:0);
	SetControlValue(tool_ctrl[6],(state.drag_mode==drag_mode_vertex)?1:0);
	
		// auto-texture
		
	SetControlValue(tool_ctrl[11],state.auto_texture?1:0);
	
		// node mode
		
	SetControlValue(tool_ctrl[12],(state.node_mode==node_mode_select)?1:0);
	SetControlValue(tool_ctrl[13],(state.node_mode==node_mode_link)?1:0);
	SetControlValue(tool_ctrl[14],(state.node_mode==node_mode_remove_link)?1:0);
	
		// normals
		
	view=walk_view_get_current_view();
		
	SetControlValue(tool_ctrl[15],(state.show_normals)?1:0);
	SetControlValue(tool_ctrl[16],(view->cull)?1:0);
	
		// grid mode
		
	SetBevelButtonMenuValue(tool_ctrl[10],(state.grid_mode+1));
}

void main_wind_tool_default(void)
{
    state.vertex_mode=vertex_mode_none;
	state.drag_mode=drag_mode_mesh;
	state.grid_mode=grid_mode_small;
	state.node_mode=node_mode_select;
	
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

      Combos
      
======================================================= */

void main_wind_tool_fill_spot_combo(void)
{
	int					n,cnt,idx;
	bool				spot_hit[max_spot];
	CFStringRef			cf_str;
	
		// delete old items
		
	DeleteMenuItems(spot_menu,1,CountMenuItems(spot_menu));
	
		// add item
		
	AppendMenuItemTextWithCFString(spot_menu,CFSTR("Add Spot"),0,0,NULL);
	AppendMenuItemTextWithCFString(spot_menu,NULL,kMenuItemAttrSeparator,0,NULL);
	
		// spots
	
	cnt=0;
	
	for (n=0;n<map.nspot;n++) {
		spot_hit[n]=FALSE;
	}

	while (TRUE) {
	
		idx=-1;
		
		for (n=0;n<map.nspot;n++) {
			if ((map.spots[n].name[0]!=0x0) && (!spot_hit[n])) {
				if (idx==-1) {
					idx=n;
				}
				else {
					if (strcasecmp(map.spots[idx].name,map.spots[n].name)>0) idx=n;
				}
			}
		}
		
		if (idx==-1) break;
		
		cf_str=CFStringCreateWithCString(kCFAllocatorDefault,map.spots[idx].name,kCFStringEncodingMacRoman);
		AppendMenuItemTextWithCFString(spot_menu,cf_str,0,0,NULL);
		CFRelease(cf_str);
		
		spot_combo_lookup[cnt++]=idx;
		spot_hit[idx]=TRUE;
	}
}

void main_wind_tool_fill_light_combo(void)
{
	int					n,cnt,idx;
	bool				light_hit[max_map_light];
	CFStringRef			cf_str;
	
		// delete old items
		
	DeleteMenuItems(light_menu,1,CountMenuItems(light_menu));
	
		// add item
		
	AppendMenuItemTextWithCFString(light_menu,CFSTR("Add Light"),0,0,NULL);
	AppendMenuItemTextWithCFString(light_menu,NULL,kMenuItemAttrSeparator,0,NULL);
	
		// lights
	
	cnt=0;
	
	for (n=0;n<map.nlight;n++) {
		light_hit[n]=FALSE;
	}

	while (TRUE) {
	
		idx=-1;
		
		for (n=0;n<map.nlight;n++) {
			if ((map.lights[n].name[0]!=0x0) && (!light_hit[n])) {
				if (idx==-1) {
					idx=n;
				}
				else {
					if (strcasecmp(map.lights[idx].name,map.lights[n].name)>0) idx=n;
				}
			}
		}
		
		if (idx==-1) break;
		
		cf_str=CFStringCreateWithCString(kCFAllocatorDefault,map.lights[idx].name,kCFStringEncodingMacRoman);
		AppendMenuItemTextWithCFString(light_menu,cf_str,0,0,NULL);
		CFRelease(cf_str);
		
		light_combo_lookup[cnt++]=idx;
		light_hit[idx]=TRUE;
	}
}

void main_wind_tool_fill_sound_combo(void)
{
	int					n,cnt,idx;
	bool				sound_hit[max_map_sound];
	CFStringRef			cf_str;
	
		// delete old items
		
	DeleteMenuItems(sound_menu,1,CountMenuItems(sound_menu));
	
		// add item
		
	AppendMenuItemTextWithCFString(sound_menu,CFSTR("Add Sound"),0,0,NULL);
	AppendMenuItemTextWithCFString(sound_menu,NULL,kMenuItemAttrSeparator,0,NULL);
	
		// sound
	
	cnt=0;
	
	for (n=0;n<map.nsound;n++) {
		sound_hit[n]=FALSE;
	}

	while (TRUE) {
	
		idx=-1;
		
		for (n=0;n<map.nsound;n++) {
			if ((map.sounds[n].name[0]!=0x0) && (!sound_hit[n])) {
				if (idx==-1) {
					idx=n;
				}
				else {
					if (strcasecmp(map.sounds[idx].name,map.sounds[n].name)>0) idx=n;
				}
			}
		}
		
		if (idx==-1) break;
		
		cf_str=CFStringCreateWithCString(kCFAllocatorDefault,map.sounds[idx].name,kCFStringEncodingMacRoman);
		AppendMenuItemTextWithCFString(sound_menu,cf_str,0,0,NULL);
		CFRelease(cf_str);
		
		sound_combo_lookup[cnt++]=idx;
		sound_hit[idx]=TRUE;
	}
}

void main_wind_tool_fill_particle_combo(void)
{
	int					n,cnt,idx;
	bool				particle_hit[max_map_particle];
	CFStringRef			cf_str;
	
		// delete old items
		
	DeleteMenuItems(particle_menu,1,CountMenuItems(particle_menu));
	
		// add item
		
	AppendMenuItemTextWithCFString(particle_menu,CFSTR("Add Particle"),0,0,NULL);
	AppendMenuItemTextWithCFString(particle_menu,NULL,kMenuItemAttrSeparator,0,NULL);
	
		// particles
	
	cnt=0;
	
	for (n=0;n<map.nparticle;n++) {
		particle_hit[n]=FALSE;
	}

	while (TRUE) {
	
		idx=-1;
		
		for (n=0;n<map.nparticle;n++) {
			if ((map.particles[n].name[0]!=0x0) && (!particle_hit[n])) {
				if (idx==-1) {
					idx=n;
				}
				else {
					if (strcasecmp(map.particles[idx].name,map.particles[n].name)>0) idx=n;
				}
			}
		}
		
		if (idx==-1) break;
		
		cf_str=CFStringCreateWithCString(kCFAllocatorDefault,map.particles[idx].name,kCFStringEncodingMacRoman);
		AppendMenuItemTextWithCFString(particle_menu,cf_str,0,0,NULL);
		CFRelease(cf_str);
		
		particle_combo_lookup[cnt++]=idx;
		particle_hit[idx]=TRUE;
	}
}

void main_wind_tool_fill_scenery_combo(void)
{
	int					n,cnt,idx;
	bool				scenery_hit[max_spot];
	CFStringRef			cf_str;
	
		// delete old items
		
	DeleteMenuItems(scenery_menu,1,CountMenuItems(scenery_menu));
	
		// add item
		
	AppendMenuItemTextWithCFString(scenery_menu,CFSTR("Add Scenery"),0,0,NULL);
	AppendMenuItemTextWithCFString(scenery_menu,NULL,kMenuItemAttrSeparator,0,NULL);
	
		// scenery
	
	cnt=0;
	
	for (n=0;n<map.nscenery;n++) {
		scenery_hit[n]=FALSE;
	}

	while (TRUE) {
	
		idx=-1;
		
		for (n=0;n<map.nscenery;n++) {
			if ((map.sceneries[n].model_name[0]!=0x0) && (!scenery_hit[n])) {
				if (idx==-1) {
					idx=n;
				}
				else {
					if (strcasecmp(map.sceneries[idx].model_name,map.sceneries[n].model_name)>0) idx=n;
				}
			}
		}
		
		if (idx==-1) break;
		
		cf_str=CFStringCreateWithCString(kCFAllocatorDefault,map.sceneries[idx].model_name,kCFStringEncodingMacRoman);
		AppendMenuItemTextWithCFString(scenery_menu,cf_str,0,0,NULL);
		CFRelease(cf_str);
		
		scenery_combo_lookup[cnt++]=idx;
		scenery_hit[idx]=TRUE;
	}
}

void main_wind_tool_fill_node_combo(void)
{
	int					n,cnt,idx;
	bool				node_hit[max_node];
	CFStringRef			cf_str;
	
		// delete old items
		
	DeleteMenuItems(node_menu,1,CountMenuItems(node_menu));
	
		// add item
		
	AppendMenuItemTextWithCFString(node_menu,CFSTR("Add Node"),0,0,NULL);
	AppendMenuItemTextWithCFString(node_menu,NULL,kMenuItemAttrSeparator,0,NULL);
	
		// nodes
		
	cnt=0;
	
	for (n=0;n<map.nnode;n++) {
		node_hit[n]=FALSE;
	}

	while (TRUE) {
	
		idx=-1;
		
		for (n=0;n<map.nnode;n++) {
			if ((map.nodes[n].name[0]!=0x0) && (!node_hit[n])) {
				if (idx==-1) {
					idx=n;
				}
				else {
					if (strcasecmp(map.nodes[idx].name,map.nodes[n].name)>0) idx=n;
				}
			}
		}
		
		if (idx==-1) break;
		
		cf_str=CFStringCreateWithCString(kCFAllocatorDefault,map.nodes[idx].name,kCFStringEncodingMacRoman);
		AppendMenuItemTextWithCFString(node_menu,cf_str,0,0,NULL);
		CFRelease(cf_str);
		
		node_combo_lookup[cnt++]=idx;
		node_hit[idx]=TRUE;
	}
}

void main_wind_tool_fill_tool_combos(void)
{
	main_wind_tool_fill_spot_combo();
	main_wind_tool_fill_light_combo();
	main_wind_tool_fill_sound_combo();
	main_wind_tool_fill_particle_combo();
	main_wind_tool_fill_scenery_combo();
	main_wind_tool_fill_node_combo();
}

/* =======================================================

      OS Specific Utility Routines
      
======================================================= */

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

