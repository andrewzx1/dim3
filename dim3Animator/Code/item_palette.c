/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Item Palette

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
#include "ui_common.h"
#include "interface.h"

extern model_type				model;
extern model_draw_setup			draw_setup;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern bool						list_palette_open;

list_palette_type				item_palette;

/* =======================================================

      Item Palette Setup
      
======================================================= */

void item_palette_initialize(void)
{
	list_palette_list_initialize(&item_palette,"Model Items",FALSE);

	item_palette.item_type=0;
	item_palette.item_idx=-1;
}

void item_palette_shutdown(void)
{
	list_palette_list_shutdown(&item_palette);
}

/* =======================================================

      Item Palette Fill
      
======================================================= */

void item_palette_fill(void)
{
	int			n;
	char		str[256],str_tag[8];

	list_palette_delete_all_items(&item_palette);

		// map

	list_palette_add_header(&item_palette,item_model,"Model");
	list_palette_add_item(&item_palette,item_model,0,"Settings",(state.cur_item==item_model),FALSE);

		// meshes

	list_palette_add_header_count(&item_palette,item_mesh,"Meshes",model.nmesh);

	for (n=0;n!=model.nmesh;n++) {
		list_palette_add_item(&item_palette,item_mesh,n,model.meshes[n].name,((state.cur_item==item_mesh)&&(state.cur_mesh_idx==n)),FALSE);
	}

		// animations

	list_palette_add_header_count(&item_palette,item_animate,"Animations",model.nanimate);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=model.nanimate;n++) {
		list_palette_add_item(&item_palette,item_animate,n,model.animates[n].name,((state.cur_item==item_animate)&&(state.cur_animate_idx==n)),FALSE);
	}

	list_palette_sort(&item_palette);

		// poses

	list_palette_add_header_count(&item_palette,item_pose,"Poses",model.npose);
	list_palette_add_item(&item_palette,item_neutral_pose,0,"[Neutral]",((state.cur_item==item_neutral_pose)&&(state.cur_pose_idx==-1)),FALSE);

	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=model.npose;n++) {
		list_palette_add_item(&item_palette,item_pose,n,model.poses[n].name,((state.cur_item==item_pose)&&(state.cur_pose_idx==n)),FALSE);
	}

	list_palette_sort(&item_palette);

		// bones

	list_palette_add_header_count(&item_palette,item_bone,"Bones",model.nbone);

	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=model.nbone;n++) {
		memmove(str_tag,&model.bones[n].tag,4);
		str_tag[4]=0x0;
		sprintf(str,"%s (%s)",model.bones[n].name,str_tag);
		list_palette_add_item(&item_palette,item_bone,n,str,((state.cur_item==item_bone)&&(state.cur_bone_idx==n)),FALSE);
	}

	list_palette_sort(&item_palette);

		// hit boxes

	list_palette_add_header_count(&item_palette,item_hit_box,"Hit Boxes",model.nhit_box);
	list_palette_sort_mark_start(&item_palette);

	for (n=0;n!=model.nhit_box;n++) {
		list_palette_add_item(&item_palette,item_hit_box,n,model.hit_boxes[n].name,((state.cur_item==item_hit_box)&&(state.cur_hit_box_idx==n)),FALSE);
	}

	list_palette_sort(&item_palette);
}

/* =======================================================

      Item Palette Draw
      
======================================================= */

void item_palette_draw(void)
{
	if (list_palette_get_level()!=0) return;
	
	item_palette_fill();
	list_palette_draw(&item_palette);
}

/* =======================================================

      Item Palette Reset
      
======================================================= */

void item_palette_state_rebuild(void)
{
	main_wind_draw();
}

void item_palette_scroll_into_view(int item_type,int item_idx)
{
	item_palette_fill();
	list_palette_scroll_item_into_view(&item_palette,item_type,item_idx);
}

/* =======================================================

      Item Palette Delete
      
======================================================= */

bool item_palette_delete(void)
{
		// anything to delete?

	if ((item_palette.item_type==-1) || (item_palette.item_idx==-1)) return(FALSE);

	switch (item_palette.item_type) {

		case item_mesh:
			if (os_dialog_confirm("Delete Mesh","Is it okay to delete this mesh?",FALSE)!=0) return(FALSE);
			model_piece_delete_mesh(item_palette.item_idx);
			item_palette_state_rebuild();
			return(TRUE);

		case item_bone:
			if (os_dialog_confirm("Delete Bone","Is it okay to delete this bone?",FALSE)!=0) return(FALSE);
			model_piece_delete_bone(item_palette.item_idx);
			item_palette_state_rebuild();
			return(TRUE);

		case item_pose:
			if (os_dialog_confirm("Delete Pose","Is it okay to delete this pose?",FALSE)!=0) return(FALSE);
			model_piece_delete_pose(item_palette.item_idx);
			item_palette_state_rebuild();
			return(TRUE);

		case item_animate:
			if (os_dialog_confirm("Delete Animation","Is it okay to delete this animation?",FALSE)!=0) return(FALSE);
			model_piece_delete_animate(item_palette.item_idx);
			item_palette_state_rebuild();
			return(TRUE);

		case item_hit_box:
			if (os_dialog_confirm("Delete Hit Box","Is it okay to delete this hit box?",FALSE)!=0) return(FALSE);
			model_piece_delete_hit_box(item_palette.item_idx);
			item_palette_state_rebuild();
			return(TRUE);

	}

	return(FALSE);
}

/* =======================================================

      Item Palette Scroll Wheel
      
======================================================= */

void item_palette_scroll_wheel(d3pnt *pnt,int move)
{
	if (list_palette_get_level()==0) list_palette_scroll_wheel(&item_palette,pnt,move);
}

/* =======================================================

      Item Palette Click
      
======================================================= */

bool item_palette_click(d3pnt *pnt,bool double_click)
{
	bool					old_open;

	if (list_palette_get_level()!=0) return(FALSE);
	
		// check if open changes
	
	old_open=list_palette_open;

		// click

	if (!list_palette_click(&item_palette,pnt,double_click)) {
		if (old_open!=list_palette_open) item_palette_state_rebuild();
		return(TRUE);
	}

	if (item_palette.item_idx==-1) return(TRUE);
	
		// turn off preferences

	state.in_preference=FALSE;

		// handle click

	switch (item_palette.item_type) {

		case item_model:
			state.cur_item=item_model;
			break;

		case item_mesh:
			state.cur_item=item_mesh;
			state.cur_mesh_idx=item_palette.item_idx;
			break;

		case item_animate:
			state.cur_item=item_animate;
			state.cur_animate_idx=item_palette.item_idx;
			state.cur_animate_pose_move_idx=-1;
			break;

		case item_pose:
			state.cur_item=item_pose;
			state.cur_pose_idx=item_palette.item_idx;
			state.cur_pose_bone_move_idx=-1;
			break;
			
		case item_neutral_pose:
			state.cur_item=item_neutral_pose;
			state.cur_pose_idx=-1;
			state.cur_pose_bone_move_idx=-1;
			break;

		case item_bone:
			state.cur_item=item_bone;
			state.cur_bone_idx=item_palette.item_idx;
			break;

		case item_hit_box:
			state.cur_item=item_hit_box;
			state.cur_hit_box_idx=item_palette.item_idx;
			break;
	}

		// netural pose has no properties

	if (item_palette.item_type==item_neutral_pose) {
		list_palette_set_level(0);
		main_wind_draw();
		return(TRUE);
	}

		// switch to properties

	property_palette_reset();
	list_palette_set_level(1);
	main_wind_draw();
	
	return(TRUE);
}

