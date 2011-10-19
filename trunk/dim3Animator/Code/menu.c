/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Menus

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

#ifdef D3_PCH
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "interface.h"
#include "ui_common.h"

extern model_type				model;
extern model_draw_setup			draw_setup;
extern file_path_setup_type		file_path_setup;
extern animator_state_type		state;

/* =======================================================

      Menu Enable/Disable
      
======================================================= */

void menu_update(void)
{
	if (!state.model_open) {
		os_menu_enable_item(app_menu_file,1,TRUE);
		os_menu_enable_item(app_menu_file,2,TRUE);
		os_menu_enable_item(app_menu_file,3,FALSE);
		os_menu_enable_item(app_menu_file,5,FALSE);
    
		os_menu_enable_item(app_menu_edit,0,FALSE);
		os_menu_enable_item(app_menu_model,0,FALSE);
		os_menu_enable_item(app_menu_view,0,FALSE);
		os_menu_enable_item(app_menu_mesh,0,FALSE);
		os_menu_enable_item(app_menu_vertex,0,FALSE);
		os_menu_enable_item(app_menu_bone,0,FALSE);
		os_menu_enable_item(app_menu_pose,0,FALSE);
		os_menu_enable_item(app_menu_animate,0,FALSE);
	}
	else {
		os_menu_enable_item(app_menu_file,1,FALSE);
		os_menu_enable_item(app_menu_file,2,FALSE);
		os_menu_enable_item(app_menu_file,3,TRUE);
		os_menu_enable_item(app_menu_file,5,TRUE);
	
		os_menu_enable_item(app_menu_edit,0,TRUE);
		os_menu_enable_item(app_menu_model,0,TRUE);
		os_menu_enable_item(app_menu_view,0,TRUE);
		os_menu_enable_item(app_menu_mesh,0,TRUE);
		os_menu_enable_item(app_menu_vertex,0,TRUE);
		os_menu_enable_item(app_menu_bone,0,TRUE);
		os_menu_enable_item(app_menu_pose,0,TRUE);
		os_menu_enable_item(app_menu_animate,0,TRUE);
	}
	
	os_menu_redraw();
}

/* =======================================================

      Misc Menu Routines
      
======================================================= */

void prepare_model(void)
{
    model_calculate_parents(&model);
    model_center_xz_all(&model);
    model_floor_all(&model);
    model_recalc_boxes(&model);
}

/* =======================================================

      Save Changes
      
======================================================= */

bool menu_save_changes_dialog(void)
{
	int			choice;
	
	choice=os_dialog_confirm("Save Changes?","Do you want to save the changes to this model?",TRUE);
	if (choice==1) return(FALSE);
	
	if (choice==0) file_save_model();
	return(TRUE);
}

/* =======================================================

      Menu Events
      
======================================================= */

bool menu_event_run(int cmd)
{
	int				major_bone_idx,minor_bone_idx,parent_idx;
	float			fx,fy,fz,bone_factor;
	
	switch (cmd) {
	
			// apple menu
   
        case kCommandAbout:
			dialog_about_run();
            return(TRUE);
		
			// file menu

		case kCommandFileNew:
			main_wind_play(FALSE,FALSE);
			file_new_model();
			return(TRUE);

		case kCommandFileOpen:
			main_wind_play(FALSE,FALSE);
			file_open_model();
			return(TRUE);
			
		case kCommandFileClose:
			if (!menu_save_changes_dialog()) return(TRUE);
			file_close_model();
			main_wind_play(FALSE,FALSE);
			menu_update();
			return(TRUE);
			
		case kCommandFileSave:
			file_save_model();
			return(TRUE);

		case kCommandFilePreference:
			state.in_preference=!state.in_preference;
			property_palette_reset();
			list_palette_set_level(1);
			main_wind_draw();
			return(TRUE);

		case kCommandFileQuit:
			if (state.model_open) {
				if (!menu_save_changes_dialog()) return(TRUE);
				file_close_model();
			}
			os_application_quit();
			return(TRUE);
			
			// edit menu

		case kCommandEditUndo:
			undo_run();
			main_wind_draw();
			return(TRUE);

		case kCommandEditSelectMore:
			poly_mask_select_more(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
			// model menu
        
		case kCommandPrepareModel:
            prepare_model();
            main_wind_draw();
			return(TRUE);
			
		case kCommandCalcBoxes:
            model_recalc_boxes(&model);
            main_wind_draw();
			return(TRUE);
            
		case kCommandCalcNormals:
            model_recalc_normals(&model,FALSE);
            main_wind_draw();
			return(TRUE);
           
		case kCommandScaleAll:
			if (!dialog_scale_run(&model,&fx,&fy,&fz)) return(TRUE);
			model_scale_all(&model,fx,fy,fz);
            main_wind_draw();
			return(TRUE);
			
		case kCommandFlipXAll:
			model_flip_all(&model,TRUE,FALSE,FALSE);
            main_wind_draw();
			return(TRUE);
			
		case kCommandFlipYAll:
			model_flip_all(&model,FALSE,TRUE,FALSE);
            main_wind_draw();
			return(TRUE);

		case kCommandFlipZAll:
			model_flip_all(&model,FALSE,FALSE,TRUE);
            main_wind_draw();
			return(TRUE);
			
		case kCommandSwapXZAll:
			model_swap_xz_all(&model);
            main_wind_draw();
			return(TRUE);
			
		case kCommandSwapYZAll:
			model_swap_yz_all(&model);
            main_wind_draw();
			return(TRUE);
            
		case kCommandCenterXZAll:
			model_center_xz_all(&model);
            main_wind_draw();
			return(TRUE);
            
		case kCommandFloorYAll:
			model_floor_all(&model);
            main_wind_draw();
			return(TRUE);

		case kCommandAddHitBox:
			model_piece_add_hit_box();
            main_wind_draw();
			return(TRUE);
						
			// view menu
					
		case kCommandFront:
			state.ang.x=0;
			state.ang.y=180;
			main_wind_draw();
			return(TRUE);
			
		case kCommandLeft:
			state.ang.x=0;
			state.ang.y=270;
			main_wind_draw();
			return(TRUE);
			
		case kCommandRight:
			state.ang.x=0;
			state.ang.y=90;
			main_wind_draw();
			return(TRUE);
			
		case kCommandBack:
			state.ang.x=0;
			state.ang.y=0;
			main_wind_draw();
			return(TRUE);
			
		case kCommandTop:
			state.ang.x=270;
			state.ang.y=0;
			main_wind_draw();
			return(TRUE);
			
		case kCommandBottom:
			state.ang.x=90;
			state.ang.y=0;
			main_wind_draw();
			return(TRUE);
						
			// mesh menu
			
		case kCommandNewMesh:
			model_piece_add_mesh();
			main_wind_draw();
			return(TRUE);
			
		case kCommandDuplicateMesh:
			if (state.cur_mesh_idx==-1) return(TRUE);
			model_piece_duplicate_mesh(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case kCommandImportOBJ:
			main_wind_play(FALSE,FALSE);
			file_import_mesh_obj(FALSE);
			return(TRUE);
			
		case kCommandReplaceOBJ:
			main_wind_play(FALSE,FALSE);
			file_import_mesh_obj(TRUE);
			return(TRUE);
			
		case kCommandInsertXML:
			main_wind_play(FALSE,FALSE);
			file_insert_mesh_dim3_model();
			return(TRUE);
			
		case kCommandScale:
			if (!dialog_scale_run(&model,&fx,&fy,&fz)) return(TRUE);
			model_scale(&model,state.cur_mesh_idx,fx,fy,fz);
            main_wind_draw();
			return(TRUE);
			
		case kCommandFlipX:
			model_flip(&model,state.cur_mesh_idx,TRUE,FALSE,FALSE);
            main_wind_draw();
			return(TRUE);

		case kCommandFlipY:
			model_flip(&model,state.cur_mesh_idx,FALSE,TRUE,FALSE);
            main_wind_draw();
			return(TRUE);
			
		case kCommandFlipZ:
			model_flip(&model,state.cur_mesh_idx,FALSE,FALSE,TRUE);
            main_wind_draw();
			return(TRUE);
			
		case kCommandFlipU:
			model_flip_uv(&model,state.cur_mesh_idx,TRUE,FALSE);
            main_wind_draw();
			return(TRUE);
			
		case kCommandFlipV:
			model_flip_uv(&model,state.cur_mesh_idx,FALSE,TRUE);
            main_wind_draw();
			return(TRUE);
			
		case kCommandSwapXZ:
			model_swap_xz(&model,state.cur_mesh_idx);
            main_wind_draw();
			return(TRUE);
			
		case kCommandSwapYZ:
			model_swap_yz(&model,state.cur_mesh_idx);
            main_wind_draw();
			return(TRUE);
            
		case kCommandCenterXZ:
			model_center_xz(&model,state.cur_mesh_idx);
            main_wind_draw();
			return(TRUE);
            
		case kCommandFloorY:
			model_floor(&model,state.cur_mesh_idx);
            main_wind_draw();
			return(TRUE);
			
			// vertex menu
			
		case kCommandVertexSelectAll:
			vertex_mask_set_sel_all(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case kCommandVertexSelectNotAttached:
			vertex_mask_set_sel_no_bone(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case kCommandVertexInvertNormals:
			vertex_invert_normals(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case kCommandVertexSetNormals:
			vertex_set_normals(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);

		case kCommandVertexSetNormalsOut:
			vertex_set_normals_in_out(state.cur_mesh_idx,TRUE);
			main_wind_draw();
			return(TRUE);

		case kCommandVertexSetNormalsIn:
			vertex_set_normals_in_out(state.cur_mesh_idx,FALSE);
			main_wind_draw();
			return(TRUE);
		
		case kCommandVertexClearBones:
			vertex_clear_bone_attachments_sel_vertexes(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case kCommandVertexAutoBones:
			vertex_auto_bone_attachments(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
				
		case kCommandVertexHideSelected:
			vertex_mask_hide_set_sel_vertexes(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case kCommandVertexHideNonSelected:
			vertex_mask_hide_set_non_sel_vertexes(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case kCommandVertexShowAll:
			vertex_mask_hide_show_all_vertexes(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case kCommandVertexDelete:
			vertex_delete_sel_vertex(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
			// bone menu
			
		case kCommandNewBone:
			model_piece_add_bone();
			main_wind_draw();
			return(TRUE);
			
		case kCommandSetBone:
			if (!dialog_set_vertex_bone_run(&major_bone_idx,&minor_bone_idx,&bone_factor)) return(TRUE);
			vertex_mask_set_sel_vertex_to_bone(state.cur_mesh_idx,major_bone_idx,minor_bone_idx,bone_factor);
			main_wind_draw();
			return(TRUE);

		case kCommandGoToParentBone:
			if (state.cur_bone_idx==-1) return(TRUE);
			
			parent_idx=model.bones[state.cur_bone_idx].parent_idx;
			if (parent_idx!=-1) state.cur_bone_idx=parent_idx;

			main_wind_draw();
			return(TRUE);
			
			// pose menu
			
		case kCommandNewPose:
			model_piece_add_pose();
			main_wind_draw();
			return(TRUE);
			
		case kCommandDupPose:
			if (state.cur_pose_idx==-1) return(TRUE);
			model_piece_duplicate_pose(state.cur_pose_idx);
			main_wind_draw();
			return(TRUE);
			
		case kCommandClearPose:
			if (state.cur_pose_idx==-1) return(TRUE);

			model_pose_clear(&model,state.cur_pose_idx);
			return(TRUE);
			
			// animation menu
			
		case kCommandNewAnimate:
			model_piece_add_animate();
			main_wind_draw();
			return(TRUE);
			
		case kCommandDupAnimate:
			if (state.cur_animate_idx==-1) return(TRUE);
			model_piece_duplicate_animate(state.cur_animate_idx);
			main_wind_draw();
			return(TRUE);
            
		case kCommandResetTimeAnimate:
			if (state.cur_animate_idx==-1) return(TRUE);
			main_wind_play(FALSE,FALSE);

			dialog_animation_reset_time_run(state.cur_animate_idx);
			return(TRUE);
			
		case kCommandPlayAnimate:
			main_wind_play(!state.playing,FALSE);
			return(TRUE);
			
		case kCommandPlayBlendAnimate:
			if (dialog_play_blend_animation_run()) {
				main_wind_play(TRUE,TRUE);
			}
			return(TRUE);
						
	}
	
	return(FALSE);
}


