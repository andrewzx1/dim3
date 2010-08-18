/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Menus

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
#include "dialog.h"
#include "common_view.h"
#include "walk_view.h"
#include "import.h"

extern bool					done;

extern map_type				map;
extern editor_state_type	state;

extern void light_maps_clear(void);

/* =======================================================

      Misc. Dialog Routines
      
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

	undo_initialize();
	menu_fix_enable();
}

/* =======================================================

      Mainline Menu Enable/Disable
      
======================================================= */

void menu_fix_enable(void)
{
	if (!state.map_opened) {
		EnableMenuItem(GetMenuHandle(app_menu_file),1);
		EnableMenuItem(GetMenuHandle(app_menu_file),2);
		DisableMenuItem(GetMenuHandle(app_menu_file),3);
		DisableMenuItem(GetMenuHandle(app_menu_file),5);
		DisableMenuItem(GetMenuHandle(app_menu_file),6);
		DisableMenuItem(GetMenuHandle(app_menu_file),7);
		DisableMenuItem(GetMenuHandle(app_menu_file),9);
	
		DisableMenuItem(GetMenuHandle(app_menu_edit),0);
		DisableMenuItem(GetMenuHandle(app_menu_view),0);
		DisableMenuItem(GetMenuHandle(app_menu_map),0);
		DisableMenuItem(GetMenuHandle(app_menu_mesh),0);
		DisableMenuItem(GetMenuHandle(app_menu_polygon),0);
		DisableMenuItem(GetMenuHandle(app_menu_vertex),0);
		DisableMenuItem(GetMenuHandle(app_menu_groups),0);
	}
	else {
	
			// file menu
			
		DisableMenuItem(GetMenuHandle(app_menu_file),1);
		DisableMenuItem(GetMenuHandle(app_menu_file),2);
		EnableMenuItem(GetMenuHandle(app_menu_file),3);
		EnableMenuItem(GetMenuHandle(app_menu_file),5);
		EnableMenuItem(GetMenuHandle(app_menu_file),6);
		EnableMenuItem(GetMenuHandle(app_menu_file),7);
		EnableMenuItem(GetMenuHandle(app_menu_file),9);
	
			// other menus
			
		EnableMenuItem(GetMenuHandle(app_menu_edit),0);
		EnableMenuItem(GetMenuHandle(app_menu_view),0);
		EnableMenuItem(GetMenuHandle(app_menu_map),0);
		EnableMenuItem(GetMenuHandle(app_menu_groups),0);
        
            // pieces menu
        
		if (select_has_type(mesh_piece)) {
			EnableMenuItem(GetMenuHandle(app_menu_mesh),0);
			if (state.drag_mode==drag_mode_polygon) {
				EnableMenuItem(GetMenuHandle(app_menu_polygon),0);
			}
			else {
				DisableMenuItem(GetMenuHandle(app_menu_polygon),0);
			}
			if (state.drag_mode==drag_mode_vertex) {
				EnableMenuItem(GetMenuHandle(app_menu_vertex),0);
			}
			else {
				DisableMenuItem(GetMenuHandle(app_menu_vertex),0);
			}
		}
		else {
			DisableMenuItem(GetMenuHandle(app_menu_mesh),0);
			DisableMenuItem(GetMenuHandle(app_menu_polygon),0);
			DisableMenuItem(GetMenuHandle(app_menu_vertex),0);
		}
	}
	
	DrawMenuBar();
}

/* =======================================================

      View Menu Check
      
======================================================= */

void menu_update_view(void)
{
	editor_view_type			*view;
	
	view=walk_view_get_current_view();
	
	CheckMenuItem(GetMenuHandle(app_menu_view),8,(!view->ortho));
	CheckMenuItem(GetMenuHandle(app_menu_view),9,view->ortho);
	
	CheckMenuItem(GetMenuHandle(app_menu_view),11,(view->uv_layer==uv_layer_normal));
	CheckMenuItem(GetMenuHandle(app_menu_view),12,(view->uv_layer==uv_layer_light_map));

	CheckMenuItem(GetMenuHandle(app_menu_view),14,(view->clip));

	CheckMenuItem(GetMenuHandle(app_menu_view),16,state.show_liquid);
	CheckMenuItem(GetMenuHandle(app_menu_view),17,state.show_object);
	CheckMenuItem(GetMenuHandle(app_menu_view),18,state.show_lightsoundparticle);
	CheckMenuItem(GetMenuHandle(app_menu_view),19,state.show_node);
	
	if (map.editor_views.count<max_editor_view) {
		EnableMenuItem(GetMenuHandle(app_menu_view),21);
		EnableMenuItem(GetMenuHandle(app_menu_view),22);
	}
	else {
		DisableMenuItem(GetMenuHandle(app_menu_view),21);
		DisableMenuItem(GetMenuHandle(app_menu_view),22);
	}
	
	if (map.editor_views.count>1) {
		EnableMenuItem(GetMenuHandle(app_menu_view),24);
	}
	else {
		DisableMenuItem(GetMenuHandle(app_menu_view),24);
	}
}

/* =======================================================

      Save Changes
      
======================================================= */

bool menu_save_changes_dialog(void)
{
	int			choice;
	
	choice=dialog_confirm("Save Changes?","Do you want to save the changes to this map?","Save","Cancel","Don't Save");
	if (choice==1) return(FALSE);
	
	if (choice==0) file_save_map();
	return(TRUE);
}

/* =======================================================

      Menu Events
      
======================================================= */

OSStatus menu_event_callback(EventHandlerCallRef eventhandler,EventRef event,void *userdata)
{
	HICommand		cmd;
	
	GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
	
	switch (cmd.commandID) {
		
        case kCommandAbout:
            dialog_about_run();
            return(noErr);
			
        case kCommandPreference:
            dialog_preference_run();
			main_wind_draw();
            return(noErr);
            
		case kHICommandQuit:
			if (state.map_opened) {
				if (!menu_save_changes_dialog()) return(noErr);
				file_close_map();
			}
			QuitApplicationEventLoop();
			return(noErr);
	
			// file menu
			
		case kCommandNew:
			file_new_map();
			return(noErr);
			
		case kCommandOpen:
			file_open_map();
			return(noErr);
			
		case kCommandClose:
			if (state.map_opened) {
				if (!menu_save_changes_dialog()) return(noErr);
				file_close_map();
			}
			return(noErr);

		case kCommandSave:
			file_save_map();
			return(noErr);
			
		case kCommandAutoGenerateMap:
			auto_generate_map();
			return(noErr);
			
			// edit menu
			
		case kCommandUndo:
			undo_pull();
			return(noErr);
			
		case kCommandDelete:
			piece_delete();
			return(noErr);
			
		case kCommandDuplicate:
			piece_duplicate();
			return(noErr);
			
		case kCommandSelectMore:
			piece_select_more();
			return(noErr);
			
			// view menu
			
		case kCommandViewFront:
			walk_view_face_front();
			main_wind_draw();
			break;
			
		case kCommandViewLeft:
			walk_view_face_left();
			main_wind_draw();
			break;
			
		case kCommandViewRight:
			walk_view_face_right();
			main_wind_draw();
			break;
			
		case kCommandViewBack:
			walk_view_face_back();
			main_wind_draw();
			break;
			
		case kCommandViewTop:
			walk_view_face_top();
			main_wind_draw();
			break;
			
		case kCommandViewBottom:
			walk_view_face_bottom();
			main_wind_draw();
			break;

		case kCommandViewPerspective:
			walk_view_perspective_ortho(FALSE);
			menu_update_view();
			main_wind_draw();
			return(noErr);
			
		case kCommandViewOrtho:
			walk_view_perspective_ortho(TRUE);
			menu_update_view();
			main_wind_draw();
			return(noErr);
			
		case kCommandViewUVLayer1:
			walk_view_set_uv_layer(uv_layer_normal);
			menu_update_view();
			main_wind_draw();
			return(noErr);
			
		case kCommandViewUVLayer2:
			walk_view_set_uv_layer(uv_layer_light_map);
			menu_update_view();
			main_wind_draw();
			return(noErr);
			
		case kCommandViewClip:
			walk_view_flip_clip();
			menu_update_view();
			main_wind_draw();
			return(noErr);
			
		case kCommandViewShowHideLiquids:
			select_remove_type(liquid_piece);
			state.show_liquid=!state.show_liquid;
			menu_update_view();
			main_wind_draw();
			break;

		case kCommandViewShowHideSpots:
			select_remove_type(spot_piece);
			select_remove_type(scenery_piece);
			state.show_object=!state.show_object;
			menu_update_view();
			main_wind_draw();
			break;
			
		case kCommandViewShowHideLights:
			select_remove_type(light_piece);
			select_remove_type(sound_piece);
			select_remove_type(particle_piece);
			state.show_lightsoundparticle=!state.show_lightsoundparticle;
			menu_update_view();
			main_wind_draw();
			break;
			
		case kCommandViewShowHideNodes:
			select_remove_type(node_piece);
			state.show_node=!state.show_node;
			menu_update_view();
			main_wind_draw();
			break;

		case kCommandViewSplitHorizontal:
			walk_view_split_horizontal();
			menu_update_view();
			main_wind_draw();
			break;
			
		case kCommandViewSplitVertical:
			walk_view_split_vertical();
			menu_update_view();
			main_wind_draw();
			break;
			
		case kCommandViewRemoveSplit:
			walk_view_remove();
			menu_update_view();
			main_wind_draw();
			break;
			
			// map menu

		case kCommandMapSettings:
			dialog_map_settings_run();
			return(noErr);
			
		case kCommandMapRaiseY:
			map_mesh_move_all(0,-(map_enlarge*10),0);
			main_wind_draw();
			return(noErr);
			
		case kCommandMapLowerY:
			map_mesh_move_all(0,(map_enlarge*10),0);
			main_wind_draw();
			return(noErr);
			
		case kCommandMapCenter:
			map_center(&map);
			main_wind_draw();
			return(noErr);
			
		case kCommandMapResetUV:
			map_mesh_reset_uv_all();
			main_wind_draw();
			return(noErr);
			
		case kCommandMapOptimize:
			map_optimize();
			main_wind_draw();
			return(noErr);
			
		case kCommandMapCreateNormals:
			map_recalc_normals(&map,FALSE);
			main_wind_draw();
			return(noErr);
			
		case kCommandClearLightMaps:
			light_maps_clear();
			walk_view_set_uv_layer(uv_layer_normal);
			menu_update_view();
			main_wind_draw();
			return(noErr);
			
		case kCommandBuildLightMaps:
			if (dialog_light_map_run()) {
				walk_view_set_uv_layer(uv_layer_light_map);
				menu_update_view();
				main_wind_draw();
			}
			return(noErr);
			
		case kCommandRun:
			launch_engine();
			return(noErr);
			            
			// mesh menu

		case kCommandMeshCombine:
			piece_combine_mesh();
			main_wind_draw();
			return(noErr);
			
		case kCommandMeshSplit:
			piece_split_mesh();
			main_wind_draw();
			return(noErr);
			
		case kCommandMeshTesselate:
			piece_tesselate();
			return(noErr);
			
		case kCommandMeshResize:
			piece_resize();
			return(noErr);
			
		case kCommandMeshReposition:
			piece_reposition();
			return(noErr);
			
		case kCommandMeshFlipX:
			piece_flip(TRUE,FALSE,TRUE);
			return(noErr);
			
		case kCommandMeshFlipY:
			piece_flip(FALSE,TRUE,FALSE);
			return(noErr);
			
		case kCommandMeshFlipZ:
			piece_flip(FALSE,FALSE,TRUE);
			return(noErr);
			
		case kCommandMeshRotateX:
			piece_rotate(90.0f,0.0f,0.0f);
			return(noErr);
			
		case kCommandMeshRotateY:
			piece_rotate(0.0f,90.0f,0.0f);
			return(noErr);
			
		case kCommandMeshRotateZ:
			piece_rotate(0.0f,0.0f,90.0f);
			return(noErr);

		case kCommandMeshFreeRotate:
			piece_free_rotate();
			return(noErr);

		case kCommandMeshRaiseY:
			piece_move(0,-100,0);
			main_wind_draw();
			return(noErr);
            
		case kCommandMeshLowerY:
			piece_move(0,100,0);
            main_wind_draw();
 			return(noErr);

		case kCommandMeshSelectAllPoly:
			piece_mesh_select_all_poly();
			main_wind_draw();
			return(noErr);
			
		case kCommandMeshSnapToGrid:
			piece_mesh_snap_to_grid();
			main_wind_draw();
			return(noErr);
			
		case kCommandMeshSnapClosestVertex:
			piece_mesh_snap_closest_vertex();
			main_wind_draw();
			return(noErr);
			
		case kCommandMeshResetUV:
			piece_reset_uvs(FALSE);
			main_wind_draw();
			return(noErr);
			
		case kCommandMeshWholeUV:
			piece_whole_uvs(FALSE);
			main_wind_draw();
			return(noErr);
			
		case kCommandMeshSingleUV:
			piece_single_uvs(FALSE);
			main_wind_draw();
			return(noErr);
			
		case kCommandMeshCreateNormals:
			piece_mesh_recalc_normals();
			main_wind_draw();
			return(noErr);
			
		case kCommandMeshInvertNormals:
			piece_mesh_invert_normals(FALSE);
			main_wind_draw();
			return(noErr);
			
			
			// polygon menu
			
		case kCommandPolygonHole:
			piece_poly_hole();
			return(noErr);
			
		case kCommandPolygonSnapToGrid:
			piece_mesh_poly_snap_to_grid();
			main_wind_draw();
			return(noErr);
			
		case kCommandPolygonRotateUV:
			piece_rotate_uvs();
			main_wind_draw();
			return(noErr);
			
		case kCommandPolygonFlipU:
			piece_flip_uvs(TRUE,FALSE);
			main_wind_draw();
			return(noErr);
			
		case kCommandPolygonFlipV:
			piece_flip_uvs(FALSE,TRUE);
			main_wind_draw();
			return(noErr);
			
		case kCommandPolygonInvertNormal:
			piece_mesh_invert_normals(TRUE);
			main_wind_draw();
			return(noErr);

		case kCommandPolygonResetUV:
			piece_reset_uvs(TRUE);
			main_wind_draw();
			return(noErr);
			
		case kCommandPolygonWholeUV:
			piece_whole_uvs(TRUE);
			main_wind_draw();
			return(noErr);
			
		case kCommandPolygonSingleUV:
			piece_single_uvs(TRUE);
			main_wind_draw();
			return(noErr);
			
			// vertex menu
			
		case kCommandVertexSnapToGrid:
			piece_mesh_vertexes_snap_to_grid();
			main_wind_draw();
			return(noErr);
			
			// group menu
			
		case kCommandGroups:
			dialog_map_groups_run();
			return(noErr);

		case kCommandGroupMovements:
			dialog_map_movements_run();
			return(noErr);

	}
	
	return(eventNotHandledErr);
}

