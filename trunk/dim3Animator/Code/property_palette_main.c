/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette Main

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

#define kModelPropertyMeshAdd				0
#define kModelPropertyAnimateAdd			1
#define kModelPropertyPoseAdd				2
#define kModelPropertyBoneAdd				3
#define kModelPropertyHitBoxAdd				4

extern model_type				model;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Main
      
======================================================= */

void property_palette_fill_main(void)
{
	int			n;

	list_palette_delete_all_items(&property_palette);

		// map

	list_palette_add_header(&property_palette,item_model,"Model");
	list_palette_add_item(&property_palette,item_model,0,"Settings",(state.cur_item==item_model),FALSE);

		// meshes

	list_palette_add_header_button(&property_palette,kModelPropertyMeshAdd,"Meshes",list_button_plus);

	for (n=0;n!=model.nmesh;n++) {
		list_palette_add_item(&property_palette,item_mesh,n,model.meshes[n].name,((state.cur_item==item_mesh)&&(state.cur_mesh_idx==n)),FALSE);
	}

		// animations

	list_palette_add_header_button(&property_palette,kModelPropertyAnimateAdd,"Animations",list_button_plus);
	list_palette_sort_mark_start(&property_palette);

	for (n=0;n!=model.nanimate;n++) {
		list_palette_add_item(&property_palette,item_animate,n,model.animates[n].name,((state.cur_item==item_animate)&&(state.cur_animate_idx==n)),FALSE);
	}

	list_palette_sort(&property_palette);

		// poses

	list_palette_add_header_button(&property_palette,kModelPropertyPoseAdd,"Poses",list_button_plus);
	list_palette_add_item(&property_palette,item_neutral_pose,0,"[Neutral]",((state.cur_item==item_neutral_pose)&&(state.cur_pose_idx==-1)),FALSE);

	list_palette_sort_mark_start(&property_palette);

	for (n=0;n!=model.npose;n++) {
		list_palette_add_item(&property_palette,item_pose,n,model.poses[n].name,((state.cur_item==item_pose)&&(state.cur_pose_idx==n)),FALSE);
	}

	list_palette_sort(&property_palette);

		// bones

	list_palette_add_header_button(&property_palette,kModelPropertyBoneAdd,"Bones",list_button_plus);

	list_palette_sort_mark_start(&property_palette);

	for (n=0;n!=model.nbone;n++) {
		list_palette_add_item(&property_palette,item_bone,n,model.bones[n].name,((state.cur_item==item_bone)&&(state.cur_bone_idx==n)),FALSE);
	}

	list_palette_sort(&property_palette);

		// hit boxes

	list_palette_add_header_button(&property_palette,kModelPropertyHitBoxAdd,"Hit Boxes",list_button_plus);
	list_palette_sort_mark_start(&property_palette);

	for (n=0;n!=model.nhit_box;n++) {
		list_palette_add_item(&property_palette,item_hit_box,n,model.hit_boxes[n].name,((state.cur_item==item_hit_box)&&(state.cur_hit_box_idx==n)),FALSE);
	}

	list_palette_sort(&property_palette);
}

/* =======================================================

      Property Palette Click Main
      
======================================================= */

void property_palette_click_main(bool double_click)
{
		// adding

	switch (property_palette.item_id) {

		case kModelPropertyMeshAdd:
			model_piece_add_mesh();
			return;

		case kModelPropertyAnimateAdd:
			model_piece_add_animate();
			return;

		case kModelPropertyPoseAdd:
			model_piece_add_pose();
			return;

		case kModelPropertyBoneAdd:
			model_piece_add_bone();
			return;

		case kModelPropertyHitBoxAdd:
			model_piece_add_hit_box();
			return;

	}

		// regular clicks

	if (property_palette.item_idx==-1) return;

		// handle click

	switch (property_palette.item_type) {

		case item_model:
			state.cur_item=item_model;
			break;

		case item_mesh:
			state.cur_item=item_mesh;
			state.cur_mesh_idx=property_palette.item_idx;
			break;

		case item_animate:
			state.cur_item=item_animate;
			state.cur_animate_idx=property_palette.item_idx;
			state.cur_animate_pose_move_idx=-1;
			state.cur_animate_pose_move_particle_idx=-1;
			state.cur_animate_pose_move_ring_idx=-1;
			break;

		case item_pose:
			state.cur_item=item_pose;
			state.cur_pose_idx=property_palette.item_idx;
			state.cur_pose_bone_move_idx=-1;
			break;
			
		case item_neutral_pose:
			state.cur_item=item_neutral_pose;
			state.cur_pose_idx=-1;
			state.cur_pose_bone_move_idx=-1;
			break;

		case item_bone:
			state.cur_item=item_bone;
			state.cur_bone_idx=property_palette.item_idx;
			break;

		case item_hit_box:
			state.cur_item=item_hit_box;
			state.cur_hit_box_idx=property_palette.item_idx;
			break;
	}

		// netural pose has no properties

	if (property_palette.item_type==item_neutral_pose) {
		list_palette_set_level(&property_palette,0);
		return;
	}

		// switch to properties
		// if double-click

	if (double_click) {
		property_palette_reset();
		list_palette_set_level(&property_palette,1);
	}
}

