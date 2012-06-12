/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Dos

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

extern model_type				model;
extern model_draw_setup			draw_setup;
extern file_path_setup_type		file_path_setup;
extern animator_state_type		state;

extern list_palette_type		file_palette,property_palette;

/* =======================================================

      Reset State
      
======================================================= */

void file_reset_state(void)
{
	int				n;

		// if there is an open model,
		// change the palette opens

	if (state.model_open) {
		os_set_title_window(state.model_file_name);

		file_palette.open=FALSE;
		property_palette.open=TRUE;
		property_palette_reset();
	}
	else {
		os_set_title_window("dim3 Editor");
	}
	
		// setup state

	state.texture=TRUE;
	state.mesh=FALSE;
	state.bone=FALSE;
    state.hit_box=FALSE;
	state.normal=FALSE;
	state.view_box=FALSE;
	state.bone_names=TRUE;
	state.sel_vertex_with_bone=FALSE;

	state.ang.x=0;
    state.ang.y=180;
	
	state.shift.x=0;
	state.shift.y=0;
	
	state.select_mode=select_mode_vertex;
	state.drag_bone_mode=drag_bone_mode_rotate;
	
	state.drag_sel_on=FALSE;
	state.magnify_z=3000;

	state.cur_item=item_model;
	state.cur_mesh_idx=0;
	state.cur_bone_idx=-1;

	state.cur_pose_idx=-1;
	
	state.cur_animate_idx=-1;
	if (model.nanimate!=0) state.cur_animate_idx=0;
	
	state.cur_animate_pose_move_idx=-1;
	state.cur_animate_pose_move_particle_idx=-1;
	state.cur_animate_pose_move_ring_idx=-1;
	state.cur_pose_bone_move_idx=-1;
	state.cur_hit_box_idx=-1;

		// clear selections and states

	main_wind_play(play_mode_stop);
	
	state.texture_edit_idx=-1;
	state.in_preference=FALSE;
	
	vertex_mask_clear_sel(state.cur_mesh_idx);
	vertex_mask_clear_hide(state.cur_mesh_idx);

	poly_mask_clear_sel(state.cur_mesh_idx);
	poly_mask_clear_hide(state.cur_mesh_idx);

	for (n=0;n!=max_model_blend_animation;n++) {
		state.blend[n].animate_idx=-1;
	}

	for (n=0;n!=max_model_mesh;n++) {
		state.show_mesh[n]=FALSE;
	}

	undo_clear();
	menu_update();

		// make sure window is
		// seleted

    os_select_window();

	main_wind_draw();
}

/* =======================================================

      New Model
      
======================================================= */

bool file_new_model(void)
{
	char		file_name[256],err_str[256],
				base_path[1024],path[1024];

		// close model

	if (!file_close_model()) return(FALSE);
	
		// get name
	
	strcpy(file_name,"NewModel");
	if (!dialog_file_new_run("New Model",file_name)) {
		file_reset_state();
		return(FALSE);
	}

		// create model

	os_set_wait_cursor();
		
	model_setup(&file_path_setup,mipmap_mode_none,FALSE,FALSE);
	model_new(&model,file_name);
	
	model.nmesh=1;
	strcpy(model.meshes[0].name,"Default");
	
	model.view_box.size.x=model.view_box.size.y=model.view_box.size.z=100;

	os_select_window();
	
	os_set_wait_cursor();
	
		// create new folders
		
	file_paths_data_default(&file_path_setup,base_path,"Models",NULL,NULL);
		
	strcat(base_path,"/");
	strcat(base_path,file_name);
	os_create_directory(base_path);
	
	sprintf(path,"%s/Textures",base_path);
	os_create_directory(path);

        // write the XML
	
	if (!model_save(&model,err_str)) {
		os_set_arrow_cursor();
		os_dialog_alert("dim3 Animator could not save model.",err_str);
		file_reset_state();
		return(FALSE);
	}
	
	os_set_arrow_cursor();
	
		// finish
		
	state.model_open=TRUE;
	strcpy(state.model_file_name,file_name);
	
	file_reset_state();
	
	return(TRUE);
}

/* =======================================================

      Open Model
      
======================================================= */

bool file_open_model(char *file_name)
{
		// close model

	if (!file_close_model()) return(FALSE);
	
		// open model
		
	os_set_wait_cursor();
	
	os_select_window();
   
	model_setup(&file_path_setup,mipmap_mode_none,FALSE,FALSE);
	if (!model_open(&model,file_name,TRUE)) {
		os_set_arrow_cursor();
		os_dialog_alert("Animator","There was a problem loading the model file.");
		file_reset_state();
		return(FALSE);
	}
    	
	os_set_arrow_cursor();

		// finish
		
	state.model_open=TRUE;
	strcpy(state.model_file_name,file_name);
	
	file_reset_state();

	return(TRUE);
}

/* =======================================================

      Save Model
      
======================================================= */

bool file_save_model(void)
{
	char			err_str[256];
	bool			ok;
	
	os_set_wait_cursor();
	
	ok=model_save(&model,err_str);
	os_set_arrow_cursor();
	
	if (!ok) os_dialog_alert("dim3 Animator could not save model.",err_str);
	
	return(ok);
}

/* =======================================================

      Close Model
      
======================================================= */

bool file_close_model(void)
{
	int			n,choice;
	
		// if no model open, just
		// return OK to close

	if (!state.model_open) return(TRUE);

		// check for save

	choice=os_dialog_confirm("Save Changes?","Do you want to save the changes to this model?",TRUE);
	if (choice==1) return(FALSE);
	
	if (choice==0) file_save_model();

		// close model
	
	model_close(&model);
	
	for (n=0;n!=max_model_mesh;n++) {
		model.meshes[n].nvertex=0;
		model.meshes[n].npoly=0;
	}
	
	model.nbone=0;
	model.npose=0;
	model.nanimate=0;
	model.nhit_box=0;
	
	for (n=0;n!=max_model_light;n++) {
		model.bone_connect.light_bone_idx[n]=-1;
	}
	
	for (n=0;n!=max_model_halo;n++) {
		model.bone_connect.halo_bone_idx[n]=-1;
	}
	
	model.bone_connect.name_bone_idx=-1;

		// reset state

	state.model_open=FALSE;
	file_reset_state();

	return(TRUE);
}

/* =======================================================

      Import Meshes
      
======================================================= */

void file_import_mesh_obj(bool replace)
{
	char			path[1024],err_str[256];
	bool			found_normals;
    
	os_set_arrow_cursor();
	if (!os_load_file("Select an OBJ to Import",path,"obj")) return;
	
	if (state.cur_mesh_idx==-1) state.cur_mesh_idx=0;
	
	if (!import_obj(path,replace,&found_normals,err_str)) {
		os_dialog_alert("OBJ Import",err_str);
		return;
	}

		// finish setup
		
	vertex_mask_clear_sel(state.cur_mesh_idx);
	vertex_mask_clear_hide(state.cur_mesh_idx);

	poly_mask_clear_sel(state.cur_mesh_idx);
	poly_mask_clear_hide(state.cur_mesh_idx);
	
	main_wind_draw();
}

/* =======================================================

      Insert Meshes
      
======================================================= */

void file_insert_mesh_dim3_model(void)
{
	char			file_name[256];
	
	os_set_arrow_cursor();

    if (!dialog_file_open_run("Open a Model","Models",NULL,"Mesh.xml;Model.xml",file_name)) return;
	
	if (state.cur_mesh_idx==-1) state.cur_mesh_idx=0;
	
	os_set_wait_cursor();
	
	insert_model(file_name);
	
    model_calculate_parents(&model);
    model_center_xz(&model,state.cur_mesh_idx);
    model_floor(&model,state.cur_mesh_idx);
    model_recalc_boxes(&model);
	
	os_set_arrow_cursor();
	
	main_wind_draw();
}

