/****************************** File *********************************

Module: dim3 Editor
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
extern iface_type				iface;

extern bool						list_palette_open;

list_palette_type				property_palette;

/* =======================================================

      Property Palette Setup
      
======================================================= */

void property_palette_initialize(void)
{
	list_palette_list_initialize(&property_palette,"No Properties",TRUE);

	property_palette.item_type=0;
	property_palette.item_idx=-1;
}

void property_palette_shutdown(void)
{
	list_palette_list_shutdown(&property_palette);
}

/* =======================================================

      Property Palette Fill
      
======================================================= */

void property_palette_fill(void)
{
		// delete the properties

	list_palette_delete_all_items(&property_palette);

		// if texture window is up,
		// put in texture properties

	if (state.texture_edit_idx!=-1) {
		property_palette_fill_texture(state.texture_edit_idx);
		return;
	}

		// if preference window is up,
		// put in preference properties

	if (state.in_preference) {
		property_palette_fill_animator_preference();
		return;
	}

		// selection properties

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

/* =======================================================

      Property Palette Draw
      
======================================================= */

void property_palette_draw(void)
{
	if (list_palette_get_level()!=1) return;
	
	property_palette_fill();
	list_palette_draw(&property_palette);
}

/* =======================================================

      Property Palette Reset Scroll
      
======================================================= */

void property_palette_reset(void)
{
	property_palette.scroll_page=0;
}

/* =======================================================

      Property Palette Scroll Wheel
      
======================================================= */

void property_palette_scroll_wheel(d3pnt *pnt,int move)
{
	if (list_palette_get_level()==1) list_palette_scroll_wheel(&property_palette,pnt,move);
}

/* =======================================================

      Property Palette Click
      
======================================================= */

bool property_palette_click(d3pnt *pnt,bool double_click)
{
	bool				old_open;
	
	if (list_palette_get_level()!=1) return(FALSE);

		// check if open changes
	
	old_open=list_palette_open;

		// click

	if (!list_palette_click(&property_palette,pnt,double_click)) {
		if (old_open!=list_palette_open) item_palette_state_rebuild();
		return(TRUE);
	}

		// click editing

	if (property_palette.item_id==-1) return(TRUE);

		// if texture window is up, texture properties

	if (state.texture_edit_idx!=-1) {
		property_palette_click_texture(state.texture_edit_idx,property_palette.item_id);
		return(TRUE);
	}

		// if preference window is up, preference properties

	if (state.in_preference) {
		property_palette_click_animator_preference(property_palette.item_id);
		return(TRUE);
	}

		// selection properties

	switch (state.cur_item) {

		case item_model:
			property_palette_click_model(property_palette.item_id);
			break;

		case item_mesh:
			property_palette_click_mesh(state.cur_mesh_idx,property_palette.item_id);
			break;

		case item_animate:
			property_palette_click_animation(state.cur_animate_idx,property_palette.item_id);
			break;

		case item_pose:
			property_palette_click_pose(state.cur_pose_idx,property_palette.item_id);
			break;

		case item_bone:
			property_palette_click_bone(state.cur_bone_idx,state.cur_pose_idx,property_palette.item_id);
			break;

		case item_hit_box:
			property_palette_click_hit_box(state.cur_hit_box_idx,property_palette.item_id);
			break;

	}

		// need to reset in case
		// the alt window has open/closed

	item_palette_state_rebuild();
	
	return(TRUE);
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

void property_palette_pick_sound(char *name,bool include_none)
{
	list_palette_start_picking_mode("Pick a Sound",(char*)iface.sound_list.sounds,iface.sound_list.nsound,sizeof(iface_sound_type),(int)offsetof(iface_sound_type,name),include_none,NULL,name);
}

void property_palette_pick_particle(char *name)
{
	list_palette_start_picking_mode("Pick a Particle",(char*)iface.particle_list.particles,iface.particle_list.nparticle,sizeof(iface_particle_type),(int)offsetof(iface_particle_type,name),FALSE,NULL,name);
}

void property_palette_pick_ring(char *name)
{
	list_palette_start_picking_mode("Pick a Ring",(char*)iface.ring_list.rings,iface.ring_list.nring,sizeof(iface_ring_type),(int)offsetof(iface_ring_type,name),FALSE,NULL,name);
}

void property_palette_pick_mesh(int *mesh_idx)
{
	list_palette_start_picking_mode("Pick a Mesh",(char*)model.meshes,model.nmesh,sizeof(model_mesh_type),(int)offsetof(model_mesh_type,name),TRUE,mesh_idx,NULL);
}

void property_palette_pick_bone(int *bone_idx)
{
	list_palette_start_picking_mode("Pick a Bone",(char*)model.bones,model.nbone,sizeof(model_bone_type),(int)offsetof(model_bone_type,name),TRUE,bone_idx,NULL);
}

void property_palette_pick_pose(int *pose_idx)
{
	list_palette_start_picking_mode("Pick a Pose",(char*)model.poses,model.npose,sizeof(model_pose_type),(int)offsetof(model_pose_type,name),TRUE,pose_idx,NULL);
}

void property_palette_pick_shader(char *name)
{
	list_palette_start_picking_mode("Pick a Shader",(char*)iface.shader_list.shaders,iface.shader_list.nshader,sizeof(iface_shader_type),(int)offsetof(iface_shader_type,name),TRUE,NULL,name);
}

