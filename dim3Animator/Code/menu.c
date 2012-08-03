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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
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

extern list_palette_type		property_palette;

os_menu_item_type	animator_menu[]={

							// File menu

						{"File","Save",app_menu_item_FileSave,os_menu_key_cmd,'S'},
						{"File","",0,os_menu_key_none,0x0},
						{"File","Preferences",app_menu_item_FilePreference,os_menu_key_none,0x0},
					#ifndef D3_OS_WINDOWS
						{"File","",0,os_menu_key_none,0x0},
						{"File","Exit",app_menu_item_FileQuit,os_menu_key_none,0x0},
					#endif

							// edit menu

						{"Edit","Undo",app_menu_item_EditUndo,os_menu_key_cmd,'Z'},
						{"Edit","",0,os_menu_key_none,0x0},
						{"Edit","Delete",app_menu_item_EditDelete,os_menu_key_none,0x0},
						{"Edit","Duplicate",app_menu_item_EditDuplicate,os_menu_key_cmd,'D'},
						{"Edit","",0,os_menu_key_none,0x0},
						{"Edit","Select More",app_menu_item_EditSelectMore,os_menu_key_cmd,'M'},

						{"","",-1,os_menu_key_none,0x0},
				};

/* =======================================================

      Create Menu
      
======================================================= */

void menu_create(void)
{
	os_menu_create(animator_menu);
}

void menu_dispose(void)
{
	os_menu_dispose();
}

/* =======================================================

      Menu Enable/Disable
      
======================================================= */

void menu_update(void)
{
	if (!state.model_open) {
		os_menu_enable_item(app_menu_file,1,FALSE);
   
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
		os_menu_enable_item(app_menu_file,1,TRUE);
	
		os_menu_enable_item(app_menu_edit,0,TRUE);
		os_menu_enable_item(app_menu_model,0,TRUE);
		os_menu_enable_item(app_menu_view,0,TRUE);
		os_menu_enable_item(app_menu_mesh,0,TRUE);
		os_menu_enable_item(app_menu_vertex,0,TRUE);
		os_menu_enable_item(app_menu_bone,0,TRUE);
		os_menu_enable_item(app_menu_pose,0,TRUE);
		os_menu_enable_item(app_menu_animate,0,TRUE);
	}
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

      Menu Events
      
======================================================= */

bool menu_event_run(int cmd)
{
	int				major_bone_idx,minor_bone_idx,parent_idx;
	float			fx,fy,fz,bone_factor;
	
	switch (cmd) {
	
			// apple menu
   
        case app_menu_item_About:
			dialog_about_run();
            return(TRUE);
		
			// file menu

		case app_menu_item_FileSave:
			file_save_model();
			return(TRUE);

		case app_menu_item_FilePreference:
			state.in_preference=!state.in_preference;
			property_palette_reset();
			list_palette_set_level(&property_palette,0);
			main_wind_draw();
			return(TRUE);

		case app_menu_item_FileQuit:
			if (file_close_model()) {
				os_application_quit();
			}
			return(TRUE);
			
			// edit menu

		case app_menu_item_EditUndo:
			undo_run();
			main_wind_draw();
			return(TRUE);

		case app_menu_item_EditSelectMore:
			poly_mask_select_more(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
			// model menu
        
		case app_menu_item_PrepareModel:
            prepare_model();
            main_wind_draw();
			return(TRUE);
			
		case app_menu_item_CalcBoxes:
            model_recalc_boxes(&model);
            main_wind_draw();
			return(TRUE);
            
		case app_menu_item_CalcNormals:
            model_recalc_normals(&model,FALSE);
            main_wind_draw();
			return(TRUE);
           
		case app_menu_item_ScaleAll:
			if (!dialog_scale_run(&model,&fx,&fy,&fz)) return(TRUE);
			model_scale_all(&model,fx,fy,fz);
            main_wind_draw();
			return(TRUE);
			
		case app_menu_item_FlipXAll:
			model_flip_all(&model,TRUE,FALSE,FALSE);
            main_wind_draw();
			return(TRUE);
			
		case app_menu_item_FlipYAll:
			model_flip_all(&model,FALSE,TRUE,FALSE);
            main_wind_draw();
			return(TRUE);

		case app_menu_item_FlipZAll:
			model_flip_all(&model,FALSE,FALSE,TRUE);
            main_wind_draw();
			return(TRUE);
			
		case app_menu_item_SwapXZAll:
			model_swap_xz_all(&model);
            main_wind_draw();
			return(TRUE);
			
		case app_menu_item_SwapYZAll:
			model_swap_yz_all(&model);
            main_wind_draw();
			return(TRUE);
            
		case app_menu_item_CenterXZAll:
			model_center_xz_all(&model);
            main_wind_draw();
			return(TRUE);
            
		case app_menu_item_FloorYAll:
			model_floor_all(&model);
            main_wind_draw();
			return(TRUE);

		case app_menu_item_AddHitBox:
			model_piece_add_hit_box();
            main_wind_draw();
			return(TRUE);
						
			// view menu
					
		case app_menu_item_Front:
			state.ang.x=0;
			state.ang.y=180;
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_Left:
			state.ang.x=0;
			state.ang.y=270;
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_Right:
			state.ang.x=0;
			state.ang.y=90;
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_Back:
			state.ang.x=0;
			state.ang.y=0;
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_Top:
			state.ang.x=270;
			state.ang.y=0;
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_Bottom:
			state.ang.x=90;
			state.ang.y=0;
			main_wind_draw();
			return(TRUE);
						
			// mesh menu
			
		case app_menu_item_NewMesh:
			model_piece_add_mesh();
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_DuplicateMesh:
			if (state.cur_mesh_idx==-1) return(TRUE);
			model_piece_duplicate_mesh(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_ImportOBJ:
			main_wind_play(play_mode_stop);
			file_import_mesh_obj(FALSE);
			return(TRUE);
			
		case app_menu_item_ReplaceOBJ:
			main_wind_play(play_mode_stop);
			file_import_mesh_obj(TRUE);
			return(TRUE);
			
		case app_menu_item_InsertXML:
			main_wind_play(play_mode_stop);
			file_insert_mesh_dim3_model();
			return(TRUE);
			
		case app_menu_item_Scale:
			if (!dialog_scale_run(&model,&fx,&fy,&fz)) return(TRUE);
			model_scale(&model,state.cur_mesh_idx,fx,fy,fz);
            main_wind_draw();
			return(TRUE);
			
		case app_menu_item_FlipX:
			model_flip(&model,state.cur_mesh_idx,TRUE,FALSE,FALSE);
            main_wind_draw();
			return(TRUE);

		case app_menu_item_FlipY:
			model_flip(&model,state.cur_mesh_idx,FALSE,TRUE,FALSE);
            main_wind_draw();
			return(TRUE);
			
		case app_menu_item_FlipZ:
			model_flip(&model,state.cur_mesh_idx,FALSE,FALSE,TRUE);
            main_wind_draw();
			return(TRUE);
			
		case app_menu_item_FlipU:
			model_flip_uv(&model,state.cur_mesh_idx,TRUE,FALSE);
            main_wind_draw();
			return(TRUE);
			
		case app_menu_item_FlipV:
			model_flip_uv(&model,state.cur_mesh_idx,FALSE,TRUE);
            main_wind_draw();
			return(TRUE);
			
		case app_menu_item_SwapXZ:
			model_swap_xz(&model,state.cur_mesh_idx);
            main_wind_draw();
			return(TRUE);
			
		case app_menu_item_SwapYZ:
			model_swap_yz(&model,state.cur_mesh_idx);
            main_wind_draw();
			return(TRUE);
            
		case app_menu_item_CenterXZ:
			model_center_xz(&model,state.cur_mesh_idx);
            main_wind_draw();
			return(TRUE);
            
		case app_menu_item_FloorY:
			model_floor(&model,state.cur_mesh_idx);
            main_wind_draw();
			return(TRUE);

		case app_menu_item_MeshTessellate:
			polygon_tessellate(state.cur_mesh_idx,FALSE);
			main_wind_draw();
			return(TRUE);
			
			// vertex menu
			
		case app_menu_item_VertexSelectAll:
			state.select_mode=select_mode_vertex;
			vertex_mask_set_sel_all(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_VertexSelectNotAttached:
			state.select_mode=select_mode_vertex;
			vertex_mask_set_sel_no_bone(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_VertexInvertNormals:
			vertex_invert_normals(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_VertexSetNormals:
			vertex_set_normals(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);

		case app_menu_item_VertexSetNormalsOut:
			vertex_set_normals_in_out(state.cur_mesh_idx,TRUE);
			main_wind_draw();
			return(TRUE);

		case app_menu_item_VertexSetNormalsIn:
			vertex_set_normals_in_out(state.cur_mesh_idx,FALSE);
			main_wind_draw();
			return(TRUE);
		
		case app_menu_item_VertexClearBones:
			state.select_mode=select_mode_vertex;
			vertex_clear_bone_attachments_sel_vertexes(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_VertexAutoBones:
			state.select_mode=select_mode_vertex;
			vertex_auto_bone_attachments(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
				
		case app_menu_item_VertexHideSelected:
			vertex_mask_hide_set_sel_vertexes(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_VertexHideNonSelected:
			vertex_mask_hide_set_non_sel_vertexes(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_VertexShowAll:
			vertex_mask_hide_show_all_vertexes(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_VertexDelete:
			state.select_mode=select_mode_vertex;
			vertex_delete_sel_vertex(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_VertexPolygonDelete:
			if (state.select_mode!=select_mode_polygon) return(TRUE);
			vertex_delete_sel_poly(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_VertexCollapseSelected:
			if (state.select_mode==select_mode_mesh) return(TRUE);
			vertex_collapse_selected(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);

		case app_menu_item_VertexCollapseSimilar:
			vertex_collapse_similar(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);

		case app_menu_item_VertexMakeQuad:
			polygon_make_quad(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);

		case app_menu_item_VertexTessellatePoly:
			polygon_tessellate(state.cur_mesh_idx,TRUE);
			main_wind_draw();
			return(TRUE);
			
			// bone menu
			
		case app_menu_item_NewBone:
			model_piece_add_bone();
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_SetBone:
			if (!dialog_set_vertex_bone_run(&major_bone_idx,&minor_bone_idx,&bone_factor)) return(TRUE);
			vertex_mask_set_sel_vertex_to_bone(state.cur_mesh_idx,major_bone_idx,minor_bone_idx,bone_factor);
			main_wind_draw();
			return(TRUE);

		case app_menu_item_GoToParentBone:
			if (state.cur_bone_idx==-1) return(TRUE);
			
			parent_idx=model.bones[state.cur_bone_idx].parent_idx;
			if (parent_idx!=-1) state.cur_bone_idx=parent_idx;

			main_wind_draw();
			return(TRUE);
			
			// pose menu
			
		case app_menu_item_NewPose:
			model_piece_add_pose();
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_DupPose:
			if (state.cur_pose_idx==-1) return(TRUE);
			model_piece_duplicate_pose(state.cur_pose_idx);
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_ClearPose:
			if (state.cur_pose_idx==-1) return(TRUE);

			model_pose_clear(&model,state.cur_pose_idx);
			return(TRUE);
			
			// animation menu
			
		case app_menu_item_NewAnimate:
			model_piece_add_animate();
			main_wind_draw();
			return(TRUE);
			
		case app_menu_item_DupAnimate:
			if (state.cur_animate_idx==-1) return(TRUE);
			model_piece_duplicate_animate(state.cur_animate_idx);
			main_wind_draw();
			return(TRUE);
            
		case app_menu_item_ResetTimeAnimate:
			if (state.cur_animate_idx==-1) return(TRUE);
			main_wind_play(play_mode_stop);

			dialog_animation_reset_time_run(state.cur_animate_idx);
			return(TRUE);
			
		case app_menu_item_PlayAnimate:
			main_wind_play(play_mode_normal);
			return(TRUE);
			
		case app_menu_item_PlayBlendAnimate:
			if (dialog_play_blend_animation_run()) {
				main_wind_play(play_mode_blend);
			}
			return(TRUE);
						
	}
	
	return(FALSE);
}


