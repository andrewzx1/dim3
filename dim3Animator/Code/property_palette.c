/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette

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
#include "ui_common.h"
#include "interface.h"

extern model_type				model;
extern model_draw_setup			draw_setup;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;
extern iface_type				iface;

char							property_bone_list[max_model_bone][name_str_len+1];

list_palette_type				property_palette;

/* =======================================================

      Property Palette Setup
      
======================================================= */

void property_palette_initialize(void)
{
	list_palette_list_initialize(&property_palette,"No Properties",FALSE,FALSE,FALSE);

	property_palette.item_pane.click.id=0;
	property_palette.item_pane.click.idx=-1;
	property_palette.item_pane.click.item=NULL;
}

void property_palette_shutdown(void)
{
	list_palette_list_shutdown(&property_palette);
}

/* =======================================================

      Property Palette Fill
      
======================================================= */

void property_palette_fill_level_0(void)
{
	property_palette_fill_main();
}

void property_palette_fill_level_1(void)
{
	switch (state.cur_item) {

		case item_model:
			property_palette_fill_model();
			break;

		case item_mesh:
			property_palette_fill_mesh(state.cur_mesh_idx);
			break;

		case item_animate:
			property_palette_fill_animation(state.cur_animate_idx);
			break;

		case item_pose:
			property_palette_fill_pose(state.cur_pose_idx);
			break;

		case item_bone:
			property_palette_fill_bone(state.cur_bone_idx,state.cur_pose_idx);
			break;

		case item_hit_box:
			property_palette_fill_hit_box(state.cur_hit_box_idx);
			break;

	}
}

void property_palette_fill_level_2(void)
{
	switch (state.cur_item) {

		case item_animate:
			property_palette_fill_animate_pose_move(state.cur_animate_idx,state.cur_animate_pose_move_idx);
			break;

		case item_pose:
			property_palette_fill_pose_bone_move(state.cur_pose_idx,state.cur_pose_bone_move_idx);
			break;

	}
}

void property_palette_fill_level_3(void)
{
	switch (state.cur_item) {

		case item_animate:
			if ((state.cur_animate_idx==-1) || (state.cur_animate_pose_move_idx==-1)) break;

			if (state.cur_animate_pose_move_particle_idx!=-1) {
				property_palette_fill_animate_pose_move_particle(state.cur_animate_idx,state.cur_animate_pose_move_idx,state.cur_animate_pose_move_particle_idx);
				break;
			}

			if (state.cur_animate_pose_move_ring_idx!=-1) {
				property_palette_fill_animate_pose_move_ring(state.cur_animate_idx,state.cur_animate_pose_move_idx,state.cur_animate_pose_move_ring_idx);
				break;
			}

			break;

	}
}

/* =======================================================

      Property Palette Draw
      
======================================================= */

void property_palette_draw(void)
{
		// delete the properties

	list_palette_delete_all_items(&property_palette);

		// if texture window is up,
		// put in texture properties

	if (state.texture_edit_idx!=-1) {
		property_palette_fill_texture(state.texture_edit_idx);
		list_palette_draw(&property_palette);
		return;
	}

		// if preference window is up,
		// put in preference properties

	if (state.in_preference) {
		property_palette_fill_animator_preference();
		list_palette_draw(&property_palette);
		return;
	}

		// fill by level

	switch (list_palette_get_level(&property_palette)) {
		case 0:
			property_palette_fill_level_0();
			break;
		case 1:
			property_palette_fill_level_1();
			break;
		case 2:
			property_palette_fill_level_2();
			break;
		case 3:
			property_palette_fill_level_3();
			break;
	}

	list_palette_draw(&property_palette);
}

/* =======================================================

      Property Palette Scrolling
      
======================================================= */

void property_palette_reset(void)
{
	property_palette.item_pane.scroll_offset=0;
}

void property_palette_scroll_into_view(int item_type,int item_idx)
{
		// can only scroll into view
		// on main page

	if (list_palette_get_level(&property_palette)!=0) return;

	property_palette_fill_level_0();
	list_palette_scroll_item_into_view(&property_palette,item_type,item_idx);
}

void property_palette_scroll_wheel(d3pnt *pnt,int move)
{
	list_palette_scroll_wheel(&property_palette,pnt,move);
}

/* =======================================================

      Property Palette Delete
      
======================================================= */

bool property_palette_delete(void)
{
		// only delete on first level

	if (list_palette_get_level(&property_palette)!=0) return(FALSE);

		// anything to delete?

	if ((property_palette.item_pane.click.id==-1) || (property_palette.item_pane.click.idx==-1)) return(FALSE);

	switch (property_palette.item_pane.click.id) {

		case item_mesh:
			if (os_dialog_confirm("Delete Mesh","Is it okay to delete this mesh?",FALSE)!=0) return(FALSE);
			model_piece_delete_mesh(property_palette.item_pane.click.idx);
			return(TRUE);

		case item_bone:
			if (os_dialog_confirm("Delete Bone","Is it okay to delete this bone?",FALSE)!=0) return(FALSE);
			model_piece_delete_bone(property_palette.item_pane.click.idx);
			return(TRUE);

		case item_pose:
			if (os_dialog_confirm("Delete Pose","Is it okay to delete this pose?",FALSE)!=0) return(FALSE);
			model_piece_delete_pose(property_palette.item_pane.click.idx);
			return(TRUE);

		case item_animate:
			if (os_dialog_confirm("Delete Animation","Is it okay to delete this animation?",FALSE)!=0) return(FALSE);
			model_piece_delete_animate(property_palette.item_pane.click.idx);
			return(TRUE);

		case item_hit_box:
			if (os_dialog_confirm("Delete Hit Box","Is it okay to delete this hit box?",FALSE)!=0) return(FALSE);
			model_piece_delete_hit_box(property_palette.item_pane.click.idx);
			return(TRUE);

	}

	return(FALSE);
}

/* =======================================================

      Property Palette Click For Levels
      
======================================================= */

void property_palette_click_level_0(bool double_click)
{
	property_palette_click_main(double_click);
}

void property_palette_click_level_1(bool double_click)
{
	switch (state.cur_item) {

		case item_model:
			property_palette_click_model(double_click);
			break;

		case item_mesh:
			property_palette_click_mesh(state.cur_mesh_idx,double_click);
			break;

		case item_animate:
			property_palette_click_animation(state.cur_animate_idx,double_click);
			break;

		case item_pose:
			property_palette_click_pose(state.cur_pose_idx,double_click);
			break;

		case item_bone:
			property_palette_click_bone(state.cur_bone_idx,state.cur_pose_idx,double_click);
			break;

		case item_hit_box:
			property_palette_click_hit_box(state.cur_hit_box_idx,double_click);
			break;

	}
}

void property_palette_click_level_2(bool double_click)
{
	switch (state.cur_item) {

		case item_animate:
			property_palette_click_animate_pose_move(state.cur_animate_idx,state.cur_animate_pose_move_idx,double_click);
			break;

		case item_pose:
			property_palette_click_pose_bone_move(state.cur_pose_idx,state.cur_pose_bone_move_idx,double_click);
			break;

	}
}

void property_palette_click_level_3(bool double_click)
{
	switch (state.cur_item) {

		case item_animate:
			if ((state.cur_animate_idx==-1) || (state.cur_animate_pose_move_idx==-1)) break;

			if (state.cur_animate_pose_move_particle_idx!=-1) {
				property_palette_click_animate_pose_move_particle(state.cur_animate_idx,state.cur_animate_pose_move_idx,state.cur_animate_pose_move_particle_idx,double_click);
				break;
			}

			if (state.cur_animate_pose_move_ring_idx!=-1) {
				property_palette_click_animate_pose_move_ring(state.cur_animate_idx,state.cur_animate_pose_move_idx,state.cur_animate_pose_move_ring_idx,double_click);
				break;
			}

			break;

	}
}

/* =======================================================

      Property Palette Click MainLine
      
======================================================= */

void property_palette_click(d3pnt *pnt,bool double_click)
{
	bool				old_open;

		// check if open changes
	
	old_open=list_palette_is_open(&property_palette);

		// click

	if (!list_palette_click(&property_palette,pnt,double_click)) {
		if (old_open!=list_palette_is_open(&property_palette)) main_wind_draw();
		return;
	}

		// if texture window is up, texture properties

	if (state.texture_edit_idx!=-1) {
		property_palette_click_texture(state.texture_edit_idx,property_palette.item_pane.click.id,double_click);
		main_wind_draw();
		return;
	}

		// if preference window is up, preference properties

	if (state.in_preference) {
		property_palette_click_animator_preference(property_palette.item_pane.click.id,double_click);
		main_wind_draw();
		return;
	}

		// click by level

	switch (list_palette_get_level(&property_palette)) {
		case 0:
			property_palette_click_level_0(double_click);
			break;
		case 1:
			property_palette_click_level_1(double_click);
			break;
		case 2:
			property_palette_click_level_2(double_click);
			break;
		case 3:
			property_palette_click_level_3(double_click);
			break;
	}
	
	main_wind_draw();
}

/* =======================================================

      Property Palette String Utilities
      
======================================================= */

void property_palette_add_string_mesh(void *list,int id,char *name,int mesh_idx,bool disabled)
{
	list_palette_type			*p_list;

	p_list=(list_palette_type*)list;

	if (mesh_idx==-1) {
		list_palette_add_string(p_list,id,name,"",FALSE);
		return;
	}
	
	list_palette_add_string(p_list,id,name,model.meshes[mesh_idx].name,disabled);
}

void property_palette_add_string_bone(void *list,int id,char *name,int bone_idx,bool disabled)
{
	list_palette_type			*p_list;

	p_list=(list_palette_type*)list;

	if (bone_idx==-1) {
		list_palette_add_string(p_list,id,name,"",FALSE);
		return;
	}
	
	list_palette_add_string(p_list,id,name,model.bones[bone_idx].name,disabled);
}

/* =======================================================

      Property Palette List Utilities
      
======================================================= */

void property_palette_pick_bone(int *bone_idx,int circular_check_bone_idx)
{
	int				n;

	for (n=0;n!=model.nbone;n++) {

		if (circular_check_bone_idx!=-1) {
			if ((n==circular_check_bone_idx) || (model_check_bone_circular(&model,&model.bones[circular_check_bone_idx]))) {
				property_bone_list[n][0]='~';
				strcpy((char*)&property_bone_list[n][1],model.bones[n].name);
				continue;
			}
		}

		strcpy(property_bone_list[n],model.bones[n].name);
	}

	list_palette_start_picking_mode(&property_palette,"Pick a Bone",(char*)property_bone_list,model.nbone,(name_str_len+1),0,TRUE,FALSE,bone_idx,NULL);
}

