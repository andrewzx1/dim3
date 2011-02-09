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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "dialog.h"

extern map_type				map;
extern editor_state_type	state;

/* =======================================================

      Mainline Menu Enable/Disable
      
======================================================= */

void menu_fix_enable(void)
{
	if (!state.map_opened) {
		os_menu_enable_item(app_menu_file,1,TRUE);
		os_menu_enable_item(app_menu_file,2,TRUE);
		os_menu_enable_item(app_menu_file,3,FALSE);
		os_menu_enable_item(app_menu_file,5,FALSE);
		os_menu_enable_item(app_menu_file,7,FALSE);
	
		os_menu_enable_item(app_menu_edit,0,FALSE);
		os_menu_enable_item(app_menu_view,0,FALSE);
		os_menu_enable_item(app_menu_map,0,FALSE);
		os_menu_enable_item(app_menu_mesh,0,FALSE);
		os_menu_enable_item(app_menu_polygon,0,FALSE);
		os_menu_enable_item(app_menu_vertex,0,FALSE);
	}
	else {
	
			// file menu
			
		os_menu_enable_item(app_menu_file,1,FALSE);
		os_menu_enable_item(app_menu_file,2,FALSE);
		os_menu_enable_item(app_menu_file,3,TRUE);
		os_menu_enable_item(app_menu_file,5,TRUE);
		os_menu_enable_item(app_menu_file,7,TRUE);
	
			// other menus
			
		os_menu_enable_item(app_menu_edit,0,TRUE);
		os_menu_enable_item(app_menu_view,0,TRUE);
		os_menu_enable_item(app_menu_map,0,TRUE);
        
            // pieces menu
        
		if (select_has_type(mesh_piece)) {
			os_menu_enable_item(app_menu_mesh,0,TRUE);
			if (state.drag_mode==drag_mode_polygon) {
				os_menu_enable_item(app_menu_polygon,0,TRUE);
			}
			else {
				os_menu_enable_item(app_menu_polygon,0,FALSE);
			}
			if (state.drag_mode==drag_mode_vertex) {
				os_menu_enable_item(app_menu_vertex,0,TRUE);
			}
			else {
				os_menu_enable_item(app_menu_vertex,0,FALSE);
			}
		}
		else {
			os_menu_enable_item(app_menu_mesh,0,FALSE);
			os_menu_enable_item(app_menu_polygon,0,FALSE);
			os_menu_enable_item(app_menu_vertex,0,FALSE);
		}
	}
	
	os_menu_redraw();
}

/* =======================================================

      View Menu Check
      
======================================================= */

void menu_update_view(void)
{
	editor_view_type			*view;
	
	view=view_get_current_view();
	
	os_menu_check_item(app_menu_view,8,(!view->ortho));
	os_menu_check_item(app_menu_view,9,view->ortho);
	
	os_menu_check_item(app_menu_view,11,(view->uv_layer==uv_layer_normal));
	os_menu_check_item(app_menu_view,12,(view->uv_layer==uv_layer_light_map));

	os_menu_check_item(app_menu_view,16,(view->clip));

	os_menu_check_item(app_menu_view,19,state.show_liquid);
	os_menu_check_item(app_menu_view,20,state.show_object);
	os_menu_check_item(app_menu_view,21,state.show_lightsoundparticle);
	os_menu_check_item(app_menu_view,22,state.show_node);
	
	if (map.editor_views.count<max_editor_view) {
		os_menu_enable_item(app_menu_view,24,TRUE);
		os_menu_enable_item(app_menu_view,25,TRUE);
	}
	else {
		os_menu_enable_item(app_menu_view,24,FALSE);
		os_menu_enable_item(app_menu_view,25,FALSE);
	}
	
	if (map.editor_views.count>1) {
		os_menu_enable_item(app_menu_view,26,TRUE);
	}
	else {
		os_menu_enable_item(app_menu_view,26,FALSE);
	}

	os_menu_redraw();
}

/* =======================================================

      Save Changes
      
======================================================= */

bool menu_save_changes_dialog(void)
{
	int			choice;
	
	choice=os_dialog_confirm("Save Changes?","Do you want to save the changes to this map?",TRUE);
	if (choice==1) return(FALSE);
	
	if (choice==0) file_save_map();
	return(TRUE);
}

/* =======================================================

      Menu Events
      
======================================================= */

bool menu_event_run(int cmd)
{
	switch (cmd) {
		
			// about

        case kCommandAbout:
            dialog_about_run();
            return(TRUE);
            
			// file menu
			
		case kCommandFileNew:
			file_new_map();
			return(TRUE);
			
		case kCommandFileOpen:
			file_open_map();
			return(TRUE);
			
		case kCommandFileClose:
			if (state.map_opened) {
				if (!menu_save_changes_dialog()) return(TRUE);
				file_close_map();
			}
			return(TRUE);

		case kCommandFileSave:
			file_save_map();
			return(TRUE);

        case kCommandFilePreference:
			state.in_preference=!state.in_preference;
			property_palette_reset();
			main_wind_draw();
            return(TRUE);

		case kCommandFileQuit:
			if (state.map_opened) {
				if (!menu_save_changes_dialog()) return(TRUE);
				file_close_map();
			}
			os_application_quit();
			return(TRUE);
			
			// edit menu
			
		case kCommandEditUndo:
			undo_pull();
			return(TRUE);
			
		case kCommandEditDelete:
			piece_delete();
			return(TRUE);
			
		case kCommandEditDuplicate:
			piece_duplicate();
			return(TRUE);
			
		case kCommandEditSelectMore:
			piece_select_more();
			return(TRUE);
			
			// view menu
			
		case kCommandViewFront:
			view_face_front();
			main_wind_draw();
			break;
			
		case kCommandViewLeft:
			view_face_left();
			main_wind_draw();
			break;
			
		case kCommandViewRight:
			view_face_right();
			main_wind_draw();
			break;
			
		case kCommandViewBack:
			view_face_back();
			main_wind_draw();
			break;
			
		case kCommandViewTop:
			view_face_top();
			main_wind_draw();
			break;
			
		case kCommandViewBottom:
			view_face_bottom();
			main_wind_draw();
			break;

		case kCommandViewPerspective:
			view_perspective_ortho(FALSE);
			menu_update_view();
			main_wind_draw();
			return(TRUE);
			
		case kCommandViewOrtho:
			view_perspective_ortho(TRUE);
			menu_update_view();
			main_wind_draw();
			return(TRUE);
			
		case kCommandViewUVLayer1:
			view_set_uv_layer(uv_layer_normal);
			menu_update_view();
			main_wind_draw();
			return(TRUE);
			
		case kCommandViewUVLayer2:
			view_set_uv_layer(uv_layer_light_map);
			menu_update_view();
			main_wind_draw();
			return(TRUE);
			
		case kCommandViewGotoSelect:
			view_goto_select();
			main_wind_draw();
			return(TRUE);
			
		case kCommandViewGotoMapCenter:
			view_goto_map_center();
			main_wind_draw();
			return(TRUE);
			
		case kCommandViewClip:
			view_flip_clip();
			menu_update_view();
			main_wind_draw();
			return(TRUE);
			
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
			view_split_horizontal();
			menu_update_view();
			main_wind_draw();
			break;
			
		case kCommandViewSplitVertical:
			view_split_vertical();
			menu_update_view();
			main_wind_draw();
			break;
			
		case kCommandViewRemoveSplit:
			view_split_remove();
			menu_update_view();
			main_wind_draw();
			break;
			
			// map menu

		case kCommandMapAddCinema:
			piece_create_cinema();
			return(TRUE);

		case kCommandMapAddGroup:
			piece_create_group();
			return(TRUE);

		case kCommandMapAddMovement:
			piece_create_movement();
			return(TRUE);
			
		case kCommandMapRaiseY:
			map_mesh_move_all(0,-(map_enlarge*10),0);
			main_wind_draw();
			return(TRUE);
			
		case kCommandMapLowerY:
			map_mesh_move_all(0,(map_enlarge*10),0);
			main_wind_draw();
			return(TRUE);
			
		case kCommandMapCenter:
			map_center(&map);
			main_wind_draw();
			return(TRUE);
			
		case kCommandMapResetUV:
			map_mesh_reset_uv_all();
			main_wind_draw();
			return(TRUE);
			
		case kCommandMapCreateNormals:
			map_recalc_normals(&map,FALSE);
			main_wind_draw();
			return(TRUE);
			
		case kCommandClearLightMaps:
			light_maps_clear();
			view_set_uv_layer(uv_layer_normal);
			menu_update_view();
			main_wind_draw();
			return(TRUE);
			
		case kCommandBuildLightMaps:
			if (light_maps_create()) {
				view_set_uv_layer(uv_layer_light_map);
				menu_update_view();
				main_wind_draw();
			}
			return(TRUE);
			
		case kCommandRun:
			launch_engine();
			return(TRUE);
			            
			// mesh menu

		case kCommandMeshCombine:
			piece_combine_mesh();
			main_wind_draw();
			return(TRUE);
			
		case kCommandMeshSplit:
			piece_split_mesh();
			main_wind_draw();
			return(TRUE);
			
		case kCommandMeshTesselate:
			piece_tesselate();
			return(TRUE);
			
		case kCommandMeshResize:
			piece_resize();
			return(TRUE);
			
		case kCommandMeshReposition:
			piece_reposition();
			return(TRUE);
			
		case kCommandMeshFlipX:
			piece_flip(TRUE,FALSE,TRUE);
			return(TRUE);
			
		case kCommandMeshFlipY:
			piece_flip(FALSE,TRUE,FALSE);
			return(TRUE);
			
		case kCommandMeshFlipZ:
			piece_flip(FALSE,FALSE,TRUE);
			return(TRUE);
			
		case kCommandMeshRotateX:
			piece_rotate(90.0f,0.0f,0.0f);
			return(TRUE);
			
		case kCommandMeshRotateY:
			piece_rotate(0.0f,90.0f,0.0f);
			return(TRUE);
			
		case kCommandMeshRotateZ:
			piece_rotate(0.0f,0.0f,90.0f);
			return(TRUE);

		case kCommandMeshFreeRotate:
			piece_free_rotate();
			return(TRUE);

		case kCommandMeshRaiseY:
			piece_move(0,-100,0);
			main_wind_draw();
			return(TRUE);
            
		case kCommandMeshLowerY:
			piece_move(0,100,0);
            main_wind_draw();
 			return(TRUE);

		case kCommandMeshSelectAllPoly:
			piece_mesh_select_all_poly();
			main_wind_draw();
			return(TRUE);
			
		case kCommandMeshSnapToGrid:
			piece_mesh_snap_to_grid();
			main_wind_draw();
			return(TRUE);
			
		case kCommandMeshSnapClosestVertex:
			piece_mesh_snap_closest_vertex();
			main_wind_draw();
			return(TRUE);
			
		case kCommandMeshResetUV:
			piece_reset_uvs(FALSE);
			main_wind_draw();
			return(TRUE);
			
		case kCommandMeshWholeUV:
			piece_whole_uvs(FALSE);
			main_wind_draw();
			return(TRUE);
			
		case kCommandMeshSingleUV:
			piece_single_uvs(FALSE);
			main_wind_draw();
			return(TRUE);
			
		case kCommandMeshCreateNormals:
			piece_mesh_recalc_normals();
			main_wind_draw();
			return(TRUE);
			
		case kCommandMeshInvertNormals:
			piece_mesh_invert_normals(FALSE);
			main_wind_draw();
			return(TRUE);
			
			// polygon menu
			
		case kCommandPolygonHole:
			piece_poly_hole();
			return(TRUE);
			
		case kCommandPolygonSnapToGrid:
			piece_mesh_poly_snap_to_grid();
			main_wind_draw();
			return(TRUE);
			
		case kCommandPolygonRotateUV:
			piece_rotate_uvs();
			main_wind_draw();
			return(TRUE);
			
		case kCommandPolygonFlipU:
			piece_flip_uvs(TRUE,FALSE);
			main_wind_draw();
			return(TRUE);
			
		case kCommandPolygonFlipV:
			piece_flip_uvs(FALSE,TRUE);
			main_wind_draw();
			return(TRUE);
			
		case kCommandPolygonInvertNormal:
			piece_mesh_invert_normals(TRUE);
			main_wind_draw();
			return(TRUE);

		case kCommandPolygonResetUV:
			piece_reset_uvs(TRUE);
			main_wind_draw();
			return(TRUE);
			
		case kCommandPolygonWholeUV:
			piece_whole_uvs(TRUE);
			main_wind_draw();
			return(TRUE);
			
		case kCommandPolygonSingleUV:
			piece_single_uvs(TRUE);
			main_wind_draw();
			return(TRUE);
			
			// vertex menu
			
		case kCommandVertexSnapToGrid:
			piece_mesh_vertexes_snap_to_grid();
			main_wind_draw();
			return(TRUE);

	}
	
	return(FALSE);
}

