/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Menus

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

#include "interface.h"
#include "dialog.h"

char							filename[256];

extern model_type				model;
extern model_draw_setup			draw_setup;
extern file_path_setup_type		file_path_setup;
extern animator_state_type		state;

/* =======================================================

      App Windows
      
======================================================= */

void windows_start(void)
{
	int				n;
	
	state.texture=TRUE;
	state.mesh=FALSE;
	state.bone=FALSE;
    state.hit_box=FALSE;
	state.normal=FALSE;
	state.view_box=FALSE;
	state.first_mesh=FALSE;
	
	state.select_mode=select_mode_vertex;
	state.drag_bone_mode=drag_bone_mode_rotate;
	
	state.sel_trig_idx=-1;
	
	state.cur_mesh_idx=0;
	state.cur_bone_idx=-1;
	state.cur_pose_idx=-1;
	
	state.playing=FALSE;
	state.play_animate_blend=FALSE;
	
	state.sel_trig_idx=-1;
	
	for (n=0;n!=max_model_blend_animation;n++) {
		state.blend[n].animate_idx=-1;
	}
	
	model_wind_open();
	
	os_select_window();
}

void windows_end(void)
{
	model_wind_close();
}

/* =======================================================

      XML Creation/Saving
      
======================================================= */

bool create_binary(void)
{
	char			base_path[1024],path[1024];
	
	os_set_wait_cursor();
	
		// base path
		
	file_paths_data_default(&file_path_setup,base_path,"Models",NULL,NULL);
    
        // create the new folders
		
	strcat(base_path,"/");
	strcat(base_path,filename);
	
	mkdir(base_path,S_IRWXU|S_IRWXG|S_IRWXO);
	
	sprintf(path,"%s/Textures",base_path);
	mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);

        // write the binary
	
	model_save(&model);
	
	os_set_arrow_cursor();
    
	return(TRUE);
}

bool save_binary(void)
{
	bool				ok;
	
	os_set_wait_cursor();
	
	ok=model_save(&model);
	os_set_arrow_cursor();
	
	if (!ok) os_dialog_alert("dim3 Animator could not save model.","The disk might be locked or a folder might be missing.\n\nIf you are running dim3 directly from the DMG file, then you need to move the files to your harddrive (DMGs are read-only).");
	
	return(ok);
}

/* =======================================================

      Open Model XML
      
======================================================= */

void reset_model_open(void)
{
	state.ang.x=0;
    state.ang.y=180;
	
	state.shift.x=0;
	state.shift.y=0;
	
	vertex_clear_sel_mask(state.cur_mesh_idx);
	vertex_clear_hide_mask(state.cur_mesh_idx);
	
	state.cur_mesh_idx=0;
	state.cur_bone_idx=-1;
	
	state.cur_pose_idx=-1;
	if (model.npose!=0) state.cur_pose_idx=0;
	
	state.cur_animate_idx=-1;
	if (model.nanimate!=0) state.cur_animate_idx=0;
	
	os_set_title_window(filename);
	
    main_wind_draw();

    reset_pose_list();
	reset_bone_list();
    reset_animate_list();
	reset_mesh_list();
	reset_vertex_tab();
	
	undo_clear();
	
	menu_fix_enable();
}

void new_model_xml(void)
{
	char		fname[256];
	
		// get name
	
	strcpy(fname,"NewModel");
	
	if (!dialog_file_new_run("New Model",fname)) return;
	
	strcpy(filename,fname);
	
		// create model

	os_set_wait_cursor();
		
	model_setup(&file_path_setup,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE);
	model_new(&model,filename);
	
	model.nmesh=1;
	strcpy(model.meshes[0].name,"Default");
	
	model.view_box.size.x=model.view_box.size.y=model.view_box.size.z=100;

	windows_start();
	
	if (!create_binary()) {
		windows_end();
		os_set_arrow_cursor();
		return;
	}
	
	os_set_arrow_cursor();
	
		// finish
		
	state.model_open=TRUE;
	
	reset_model_open();
}

void open_model_xml(void)
{
	char		file_name[256];
	
	os_set_arrow_cursor();

    if (!dialog_file_open_run("Open a Model","Models",NULL,"Mesh.xml",file_name)) return;
	
		// open model
		
	os_set_wait_cursor();

	windows_start();
    
	model_setup(&file_path_setup,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE);
	model_open(&model,file_name,TRUE);
    	
	os_set_arrow_cursor();
	
		// finish
		
	state.model_open=TRUE;
	strcpy(filename,file_name);
	
	reset_model_open();
}

void close_model_xml(void)
{
	int				n;
	
	if (!state.model_open) return;
	
	model_close(&model);
	
	for (n=0;n!=max_model_mesh;n++) {
		model.meshes[n].nvertex=0;
		model.meshes[n].ntrig=0;
	}
	
	model.nbone=0;
	model.npose=0;
	model.nanimate=0;
	
	for (n=0;n!=max_model_light;n++) {
		model.tags.light_bone_tag[n]=FOUR_CHAR_CODE('\?\?\?\?');
	}
	
	for (n=0;n!=max_model_halo;n++) {
		model.tags.halo_bone_tag[n]=FOUR_CHAR_CODE('\?\?\?\?');
	}
	
	model.tags.name_bone_tag=FOUR_CHAR_CODE('\?\?\?\?');
	
	windows_end();
	
	state.model_open=FALSE;
}

/* =======================================================

      Import Meshes
      
======================================================= */

void import_mesh_obj(void)
{
	float			scale;
	char			path[1024],err_str[256];
	bool			found_normals;
    
	os_set_arrow_cursor();
	if (!os_load_file(path,"obj")) return;
	
	if (state.cur_mesh_idx==-1) state.cur_mesh_idx=0;
	model.meshes[state.cur_mesh_idx].nvertex=model.meshes[state.cur_mesh_idx].ntrig=0;
	
	if (!import_obj(path,&found_normals,err_str)) {
		os_dialog_alert("Could not import .OBJ file",err_str);
		return;
	}

	vertex_delete_unused_vertexes(state.cur_mesh_idx);
	
	dialog_import_finish_run(&model,&scale);
	model_scale(&model,state.cur_mesh_idx,scale,scale,scale);

	model_center_xz(&model,state.cur_mesh_idx);
	model_floor(&model,state.cur_mesh_idx);
	model_recalc_boxes(&model);
    model_recalc_normals(&model,found_normals);
	
	reset_vertex_tab();
	reset_pose_list();
	reset_bone_list();
	
	main_wind_draw();
}

void import_mesh_lightwave(void)
{
	float			scale;
	char			path[1024],err_str[256];
    
	os_set_arrow_cursor();
	if (!os_load_file(path,"lwo")) return;
	
	if (state.cur_mesh_idx==-1) state.cur_mesh_idx=0;
	model.meshes[state.cur_mesh_idx].nvertex=model.meshes[state.cur_mesh_idx].ntrig=0;
	
	if (!import_lightwave(path,err_str)) {
		os_dialog_alert("Could not import .LWO file",err_str);
		return;
	}
	
	vertex_delete_unused_vertexes(state.cur_mesh_idx);
	
	dialog_import_finish_run(&model,&scale);
	model_scale(&model,state.cur_mesh_idx,scale,scale,scale);
	
    model_center_xz(&model,state.cur_mesh_idx);
    model_floor(&model,state.cur_mesh_idx);
    model_recalc_boxes(&model);
    model_recalc_normals(&model,FALSE);
	
	reset_vertex_tab();
	reset_pose_list();
	reset_bone_list();
	
    main_wind_draw();
}

void import_mesh_c4d_xml(void)
{
	char			path[1024],err_str[256];
	float			scale;
    
	os_set_arrow_cursor();
	if (!os_load_file(path,"xml")) return;
	
	if (state.cur_mesh_idx==-1) state.cur_mesh_idx=0;
	model.meshes[state.cur_mesh_idx].nvertex=model.meshes[state.cur_mesh_idx].ntrig=0;
	
	if (!import_c4d_xml(path,err_str)) {
		os_dialog_alert("Could not import .XML file",err_str);
		return;
	}
	
	vertex_delete_unused_vertexes(state.cur_mesh_idx);
	
	dialog_import_finish_run(&model,&scale);
	model_scale_all(&model,scale,scale,scale);

    model_center_xz_all(&model);
    model_floor_all(&model);
    model_recalc_boxes(&model);
    model_recalc_normals(&model,FALSE);
	
	reset_vertex_tab();
	reset_pose_list();
	reset_bone_list();
	
	main_wind_draw();
}

/* =======================================================

      Insert Meshes
      
======================================================= */

void insert_mesh_dim3_model(void)
{
	char			file_name[256];
	
	os_set_arrow_cursor();

    if (!dialog_file_open_run("Open a Model","Models",NULL,"Mesh.xml",file_name)) return;
	
	if (state.cur_mesh_idx==-1) state.cur_mesh_idx=0;
	
	os_set_wait_cursor();
	
	insert_model(file_name);
	
    model_calculate_parents(&model);
    model_center_xz(&model,state.cur_mesh_idx);
    model_floor(&model,state.cur_mesh_idx);
    model_recalc_boxes(&model);
	
	os_set_arrow_cursor();
	
	reset_vertex_tab();
	reset_pose_list();
	reset_bone_list();
	reset_animate_list();
	
	main_wind_draw();
}

/* =======================================================

      Menu Enable/Disable
      
======================================================= */

void menu_fix_enable(void)
{
	if (!state.model_open) {
		EnableMenuItem(GetMenuHandle(filemenu),1);
		EnableMenuItem(GetMenuHandle(filemenu),2);
		DisableMenuItem(GetMenuHandle(filemenu),3);
		DisableMenuItem(GetMenuHandle(filemenu),5);
    
		DisableMenuItem(GetMenuHandle(editmenu),0);
		DisableMenuItem(GetMenuHandle(modelmenu),0);
		DisableMenuItem(GetMenuHandle(viewmenu),0);
		DisableMenuItem(GetMenuHandle(meshmenu),0);
		DisableMenuItem(GetMenuHandle(vertexmenu),0);
		DisableMenuItem(GetMenuHandle(bonemenu),0);
		DisableMenuItem(GetMenuHandle(posemenu),0);
		DisableMenuItem(GetMenuHandle(animatemenu),0);
	}
	else {
		DisableMenuItem(GetMenuRef(filemenu),1);
		DisableMenuItem(GetMenuRef(filemenu),2);
		EnableMenuItem(GetMenuRef(filemenu),3);
		EnableMenuItem(GetMenuRef(filemenu),5);
	
		EnableMenuItem(GetMenuRef(editmenu),0);
		EnableMenuItem(GetMenuRef(modelmenu),0);
		EnableMenuItem(GetMenuRef(viewmenu),0);
		EnableMenuItem(GetMenuRef(meshmenu),0);
		EnableMenuItem(GetMenuRef(vertexmenu),0);
		EnableMenuItem(GetMenuRef(bonemenu),0);
		EnableMenuItem(GetMenuRef(posemenu),0);
		EnableMenuItem(GetMenuRef(animatemenu),0);
	}
	
	DrawMenuBar();
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

void redraw_model(void)
{
 	reset_vertex_tab();
	reset_pose_list();
	reset_bone_list();
    reset_animate_list();
	reset_mesh_list();
	
	main_wind_draw();
}

/* =======================================================

      Save Changes
      
======================================================= */

bool menu_save_changes_dialog(void)
{
	int			choice;
	
	choice=os_dialog_confirm("Save Changes?","Do you want to save the changes to this model?",TRUE);
	if (choice==1) return(FALSE);
	
	if (choice==0) save_binary();
	return(TRUE);
}

/* =======================================================

      Menu Events
      
======================================================= */

bool menu_event_run(int cmd)
{
	int				x,y,z,idx,major_bone_idx,minor_bone_idx,parent_idx,old_cur_animate;
	float			fx,fy,fz,bone_factor;
	bool			nudge_children,nudge_vertexes;
	
	switch (cmd) {
	
			// apple menu
   
        case kCommandAbout:
			dialog_about_run();
            return(TRUE);
		
		case kHICommandQuit:
			if (state.model_open) {
				if (!menu_save_changes_dialog()) return(TRUE);
				close_model_xml();
			}
			QuitApplicationEventLoop();
			return(TRUE);
	
			// file menu

		case kCommandNew:
			model_wind_play(FALSE,FALSE);
			new_model_xml();
			return(TRUE);

		case kCommandOpen:
			model_wind_play(FALSE,FALSE);
			open_model_xml();
			return(TRUE);
			
		case kCommandClose:
			if (!menu_save_changes_dialog()) return(TRUE);
			close_model_xml();
			model_wind_play(FALSE,FALSE);
			menu_fix_enable();
			return(TRUE);
			
		case kCommandSave:
			save_binary();
			return(TRUE);
			
			// edit menu

		case kCommandUndo:
			undo_run();
			main_wind_draw();
			return(TRUE);
			
			// model menu
        
 		case kCommandSettings:
			dialog_model_settings_run();
            redraw_model();
			return(TRUE);
			
		case kCommandHitBoxes:
			dialog_hit_box_settings_run();
            redraw_model();
			return(TRUE);
		
		case kCommandPrepareModel:
            prepare_model();
            redraw_model();
			return(TRUE);
			
		case kCommandCalcBoxes:
            model_recalc_boxes(&model);
            redraw_model();
			return(TRUE);
            
		case kCommandCalcNormals:
            model_recalc_normals(&model,FALSE);
            redraw_model();
			return(TRUE);
           
		case kCommandScaleAll:
			if (!dialog_scale_run(&model,&fx,&fz,&fy)) return(TRUE);
			model_scale_all(&model,fx,fy,fz);
            model_calculate_parents(&model);
            redraw_model();
			return(TRUE);
			
		case kCommandFlipXAll:
			model_flip_all(&model,TRUE,FALSE,FALSE);
            model_calculate_parents(&model);
            redraw_model();
			return(TRUE);

		case kCommandFlipZAll:
			model_flip_all(&model,FALSE,FALSE,TRUE);
            model_calculate_parents(&model);
            redraw_model();
			return(TRUE);
			
		case kCommandFlipYAll:
			model_flip_all(&model,FALSE,TRUE,FALSE);
            model_calculate_parents(&model);
            redraw_model();
			return(TRUE);
			
		case kCommandSwapXZAll:
			model_swap_xz_all(&model);
            model_calculate_parents(&model);
            redraw_model();
			return(TRUE);
			
		case kCommandSwapYZAll:
			model_swap_yz_all(&model);
            model_calculate_parents(&model);
            redraw_model();
			return(TRUE);
            
		case kCommandCenterXZAll:
			model_center_xz_all(&model);
            model_calculate_parents(&model);
            redraw_model();
			return(TRUE);
            
		case kCommandFloorYAll:
			model_floor_all(&model);
            model_calculate_parents(&model);
            redraw_model();
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
			idx=model_mesh_add(&model);
			if (idx==-1) return(TRUE);
			
			if (!dialog_mesh_info_run(&model.meshes[idx])) {
				model_mesh_delete(&model,idx);
				return(TRUE);
			}
			
			state.cur_mesh_idx=idx;

			main_wind_draw();
			reset_mesh_list();
			return(TRUE);
			
		case kCommandDuplicateMesh:
			idx=model_mesh_duplicate(&model,state.cur_mesh_idx);
			if (idx==-1) return(TRUE);
			
			if (!dialog_mesh_info_run(&model.meshes[idx])) {
				model_mesh_delete(&model,idx);
				return(TRUE);
			}
			
			state.cur_mesh_idx=idx;

			reset_mesh_list();
			main_wind_draw();
			return(TRUE);
			
		case kCommandCopyMesh:
			idx=dialog_copy_mesh_run();
			if (idx==-1) return(TRUE);
			if (idx!=state.cur_mesh_idx) model_mesh_copy(&model,idx,state.cur_mesh_idx);
			redraw_model();
			reset_mesh_list();
			reset_vertex_tab();
			return(TRUE);
			
		case kCommandDeleteMesh:
			model_mesh_delete(&model,state.cur_mesh_idx);
			state.cur_mesh_idx=0;
			reset_mesh_list();
			main_wind_draw();
			return(TRUE);
			
		case kCommandImportOBJ:
			model_wind_play(FALSE,FALSE);
			import_mesh_obj();
			return(TRUE);
			
		case kCommandImportLWO:
			model_wind_play(FALSE,FALSE);
			import_mesh_lightwave();
			return(TRUE);
			
		case kCommandImportC4DXML:
			model_wind_play(FALSE,FALSE);
			import_mesh_c4d_xml();
			return(TRUE);
			
		case kCommandInsertXML:
			model_wind_play(FALSE,FALSE);
			insert_mesh_dim3_model();
			return(TRUE);
			
		case kCommandScale:
			if (!dialog_scale_run(&model,&fx,&fz,&fy)) return(TRUE);
			model_scale(&model,state.cur_mesh_idx,fx,fy,fz);
            redraw_model();
			return(TRUE);
			
		case kCommandFlipX:
			model_flip(&model,state.cur_mesh_idx,TRUE,FALSE,FALSE);
            model_calculate_parents(&model);
            redraw_model();
			return(TRUE);

		case kCommandFlipZ:
			model_flip(&model,state.cur_mesh_idx,FALSE,FALSE,TRUE);
            model_calculate_parents(&model);
            redraw_model();
			return(TRUE);
			
		case kCommandFlipY:
			model_flip(&model,state.cur_mesh_idx,FALSE,TRUE,FALSE);
            model_calculate_parents(&model);
            redraw_model();
			return(TRUE);
			
		case kCommandFlipU:
			model_flip_uv(&model,state.cur_mesh_idx,TRUE,FALSE);
            redraw_model();
			return(TRUE);
			
		case kCommandFlipV:
			model_flip_uv(&model,state.cur_mesh_idx,FALSE,TRUE);
            redraw_model();
			return(TRUE);
			
		case kCommandSwapXZ:
			model_swap_xz(&model,state.cur_mesh_idx);
            model_calculate_parents(&model);
            redraw_model();
			return(TRUE);
			
		case kCommandSwapYZ:
			model_swap_yz(&model,state.cur_mesh_idx);
            model_calculate_parents(&model);
            redraw_model();
			return(TRUE);
            
		case kCommandCenterXZ:
			model_center_xz(&model,state.cur_mesh_idx);
            model_calculate_parents(&model);
            redraw_model();
			return(TRUE);
            
		case kCommandFloorY:
			model_floor(&model,state.cur_mesh_idx);
            model_calculate_parents(&model);
            redraw_model();
			return(TRUE);
			
			// vertex menu
			
		case kCommandVertexSelectAll:
			vertex_set_sel_mask_all(state.cur_mesh_idx);
			main_wind_draw();
			hilite_vertex_rows();
			return(TRUE);
			
		case kCommandVertexSelectNotAttached:
			vertex_set_sel_mask_no_bone(state.cur_mesh_idx);
			main_wind_draw();
			hilite_vertex_rows();
			return(TRUE);
			
		case kCommandVertexNudge:
			if (!dialog_nudge_rotate_run(&x,&z,&y,"NudgePick",0)) return(TRUE);
			vertex_move_sel_vertexes(state.cur_mesh_idx,x,y,z);
			model_calculate_parents(&model);
			main_wind_draw();
			return(TRUE);
			
		case kCommandVertexScale:
			if (!dialog_scale_run(&model,&fx,&fz,&fy)) return(TRUE);
			vertex_scale_sel_vertexes(state.cur_mesh_idx,fx,fy,fz);
			model_calculate_parents(&model);
			main_wind_draw();
			return(TRUE);
			
		case kCommandVertexRotate:
			if (!dialog_nudge_rotate_run(&x,&z,&y,"RotatePick",0)) return(TRUE);
			vertex_rotate_sel_vertexes(state.cur_mesh_idx,(float)x,(float)y,(float)z);
			model_calculate_parents(&model);
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
		
		case kCommandVertexClearBones:
			vertex_clear_bone_attachments_sel_vertexes(state.cur_mesh_idx);
			reset_vertex_tab();
			main_wind_draw();
			return(TRUE);
				
		case kCommandVertexHideSelected:
			vertex_hide_mask_set_sel_vertexes(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case kCommandVertexHideNonSelected:
			vertex_hide_mask_set_non_sel_vertexes(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case kCommandVertexShowAll:
			vertex_hide_mask_show_all_vertexes(state.cur_mesh_idx);
			main_wind_draw();
			return(TRUE);
			
		case kCommandVertexDelete:
			vertex_delete_sel_vertex(state.cur_mesh_idx);
			model_calculate_parents(&model);
			redraw_model();
			reset_vertex_tab();
			return(TRUE);
			
			// bone menu
			
		case kCommandNewBone:
 			vertex_find_center_sel_vertexes(state.cur_mesh_idx,&x,&y,&z);
			
			idx=model_bone_add(&model,x,y,z);
			if (idx==-1) return(TRUE);
			
			if (!dialog_bone_settings_run(&model.bones[idx])) {
				model_bone_delete(&model,idx);
				return(TRUE);
			}
			
			vertex_set_sel_vertex_to_bone(state.cur_mesh_idx,idx,-1,1.0f);
			state.cur_bone_idx=idx;

			reset_pose_list();
			reset_bone_list();
			main_wind_draw();
			return(TRUE);
			
		case kCommandSetBone:
			if (!dialog_set_vertex_bone_run(&major_bone_idx,&minor_bone_idx,&bone_factor)) return(TRUE);
			vertex_set_sel_vertex_to_bone(state.cur_mesh_idx,major_bone_idx,minor_bone_idx,bone_factor);
			reset_vertex_tab();
			main_wind_draw();
			return(TRUE);
			
		case kCommandNudgeBone:
			if (state.cur_bone_idx==-1) return(TRUE);
			
			if (!dialog_bone_nudge_run(&x,&z,&y,&nudge_children,&nudge_vertexes)) return(TRUE);
			model_bone_move(&model,state.cur_bone_idx,x,y,z,nudge_children,nudge_vertexes);
			model_calculate_parents(&model);
			main_wind_draw();
			return(TRUE);
			
		case kCommandDeleteBone:
			if (state.cur_bone_idx==-1) return(TRUE);
			
 			model_bone_delete(&model,state.cur_bone_idx);
			state.cur_bone_idx=-1;
			reset_vertex_tab();
			reset_pose_list();
			reset_bone_list();
			return(TRUE);
			
		case kCommandSelectVertexNearBone:
			if (state.cur_bone_idx!=-1) return(TRUE);
			vertex_set_sel_mask_near_bone(state.cur_mesh_idx,state.cur_bone_idx,0.10);
			main_wind_draw();
			return(TRUE);
			
			// pose menu
			
		case kCommandNewPose:
 			if (model.nbone==0) {
				os_dialog_alert("Can't Create Pose","You need to have at least one bone before creating a pose.");
				return(TRUE);
			}
			
			idx=model_pose_add(&model);
			if (idx==-1) return(TRUE);
			
			if (!dialog_pose_settings_run(&model.poses[idx])) {
				model_pose_delete(&model,idx);
				return(TRUE);
			}
			
			state.cur_pose_idx=idx;
			state.cur_bone_idx=-1;

			reset_pose_list();
			reset_bone_list();
			main_wind_draw();
			return(TRUE);
			
		case kCommandDupPose:
			if (state.cur_pose_idx==-1) return(TRUE);

			idx=model_pose_duplicate(&model,state.cur_pose_idx);
			if (idx==-1) return(TRUE);
			
			if (!dialog_pose_settings_run(&model.poses[idx])) {
				model_pose_delete(&model,idx);
				return(TRUE);
			}
			
			state.cur_pose_idx=idx;
			state.cur_bone_idx=-1;

			reset_pose_list();
			reset_bone_list();
			main_wind_draw();
			return(TRUE);
			
		case kCommandPreviousPose:
			if (state.cur_pose_idx>0) state.cur_pose_idx--;
			reset_pose_list();
			reset_bone_list();
			return(TRUE);
			
		case kCommandNextPose:
			if (state.cur_pose_idx<(model.npose-1)) state.cur_pose_idx++;
			reset_pose_list();
			reset_bone_list();
			return(TRUE);
			
		case kCommandClearPose:
			if (state.cur_pose_idx==-1) return(TRUE);

			model_pose_clear(&model,state.cur_pose_idx);
			reset_pose_list();
			reset_bone_list();
			return(TRUE);
			
        case kCommandDeletePose:
			if (state.cur_pose_idx==-1) return(TRUE);

			if (model_check_pose_in_animation(&model,state.cur_pose_idx)) {
				os_dialog_alert("Can't Delete Pose","This pose is being used in an animation.");
				return(TRUE);
			}
			
            model_pose_delete(&model,state.cur_pose_idx);
			
			state.cur_pose_idx=-1;
			state.cur_bone_idx=-1;
			
			reset_pose_list();
			reset_bone_list();
            return(TRUE);
			
		case kCommandGoToBoneMoveParent:
			if ((state.cur_pose_idx==-1) || (state.cur_bone_idx==-1)) return(TRUE);
			
			parent_idx=model.bones[state.cur_bone_idx].parent_idx;
			if (parent_idx!=-1) {
				state.cur_bone_idx=parent_idx;
				reset_pose_list();
				reset_bone_list();
			}

			return(TRUE);
			
			// blending menu
			
		case kCommandBlendSetSkipAll:
			if (state.cur_pose_idx==-1) return(TRUE);
			
			model_pose_blend_set_all(&model,state.cur_pose_idx,TRUE);
			reset_pose_list();
			reset_bone_list();
			return(TRUE);
			
		case kCommandBlendSetSkipNone:
			if (state.cur_pose_idx==-1) return(TRUE);
			
			model_pose_blend_set_all(&model,state.cur_pose_idx,FALSE);
			reset_pose_list();
			reset_bone_list();
			return(TRUE);
			
		case kCommandBlendFlipSkip:
 			if ((state.cur_pose_idx==-1) || (state.cur_bone_idx==-1)) return(TRUE);
			
			model.poses[state.cur_pose_idx].bone_moves[state.cur_bone_idx].skip_blended=!model.poses[state.cur_pose_idx].bone_moves[state.cur_bone_idx].skip_blended;
			reset_pose_list();
			reset_bone_list();
			return(TRUE);
						
			// animation menu
			
		case kCommandNewAnimate:
			if (model.npose==0) {
				os_dialog_alert("Can't Create Animation","You need to have at least one pose before creating an animation.");
				return(TRUE);
			}
			
			model_wind_play(FALSE,FALSE);
			
			idx=model_animate_add(&model);
			if (idx==-1) return(TRUE);
			
			old_cur_animate=state.cur_animate_idx;
			state.cur_animate_idx=idx;
			
			if (!dialog_animation_settings_run(idx)) {
				model_animate_delete(&model,idx);
				state.cur_animate_idx=old_cur_animate;
				reset_animate_list();
				return(TRUE);
			}

			reset_animate_list();
			main_wind_draw();
			return(TRUE);
			
		case kCommandDupAnimate:
			if (state.cur_animate_idx==-1) return(TRUE);
			model_wind_play(FALSE,FALSE);
 
			idx=model_animate_duplicate(&model,state.cur_animate_idx);
			if (idx==-1) return(TRUE);
			
			old_cur_animate=state.cur_animate_idx;
			state.cur_animate_idx=idx;
			
			if (!dialog_animation_settings_run(idx)) {
				model_animate_delete(&model,idx);
				state.cur_animate_idx=old_cur_animate;
				reset_animate_list();
				return(TRUE);
			}

			reset_animate_list();
			main_wind_draw();
			return(TRUE);
            
		case kCommandDeleteAnimate:
			if (state.cur_animate_idx==-1) return(TRUE);
			model_wind_play(FALSE,FALSE);

			model_animate_delete(&model,state.cur_animate_idx);
			if (model.nanimate==0) {
				state.cur_animate_idx=-1;
			}
			else {
				state.cur_animate_idx=0;
			}
			reset_animate_list();
			main_wind_draw();
			return(TRUE);
			
		case kCommandResetTimeAnimate:
			if (state.cur_animate_idx==-1) return(TRUE);
			model_wind_play(FALSE,FALSE);

			dialog_animation_reset_time_run(state.cur_animate_idx);
			reset_animate_list();
			return(TRUE);
			
		case kCommandPlayAnimate:
			model_wind_play(!state.playing,FALSE);
			return(TRUE);
			
		case kCommandPlayBlendAnimate:
			if (dialog_play_blend_animation_run()) {
				model_wind_play(TRUE,TRUE);
			}
			return(TRUE);
						
	}
	
	return(FALSE);
}


