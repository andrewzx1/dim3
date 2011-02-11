/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette

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
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

extern model_type				model;
extern model_draw_setup			draw_setup;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern int						tool_palette_pixel_sz,txt_palette_pixel_sz;
extern bool						list_palette_open,alt_property_open;

list_palette_type				property_palette;

/* =======================================================

      Property Palette Setup
      
======================================================= */

void property_palette_initialize(void)
{
	list_palette_list_initialize(&property_palette,"No Properties");

	property_palette.item_type=0;
	property_palette.item_idx=-1;
}

void property_palette_shutdown(void)
{
	list_palette_list_shutdown(&property_palette);
}

void property_palette_setup(void)
{
	int				x,y;
	d3rect			wbox;
	
	os_get_window_box(&wbox);

	if (list_palette_open) {
		property_palette.pixel_sz=list_palette_tree_sz;
	}
	else {
		property_palette.pixel_sz=list_palette_border_sz;
	}

	x=wbox.rx-property_palette.pixel_sz;
	if ((list_palette_open) && (alt_property_open)) x-=property_palette.pixel_sz;
	
	y=wbox.ty+((wbox.by-wbox.ty)>>1);

	property_palette.box.lx=x;
	property_palette.box.rx=x+property_palette.pixel_sz;
	property_palette.box.ty=y-1;
	property_palette.box.by=wbox.by-txt_palette_pixel_sz;
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
		list_palette_set_title(&property_palette,"Texture Properties");
		property_palette_fill_texture(state.texture_edit_idx);
		return;
	}

		// if preference window is up,
		// put in preference properties

	if (state.in_preference) {
		list_palette_set_title(&property_palette,"Animator Preferences");
		property_palette_fill_animator_preference();
		return;
	}

		// selection properties

	switch (state.cur_item) {

		case item_model:
			list_palette_set_title(&property_palette,"Model Properties");
			property_palette_fill_model();
			break;

		case item_mesh:
			list_palette_set_title(&property_palette,"Mesh Properties");
			property_palette_fill_mesh(state.cur_mesh_idx);
			break;

		case item_animate:
			list_palette_set_title(&property_palette,"Animation Properties");
			property_palette_fill_animation(state.cur_animate_idx);
			break;

		case item_pose:
			list_palette_set_title(&property_palette,"Pose Properties");
			property_palette_fill_pose(state.cur_pose_idx);
			break;
			
		case item_neutral_pose:
			list_palette_set_title(&property_palette,"No Properties");
			break;

		case item_bone:
			list_palette_set_title(&property_palette,"Bone Properties");
			property_palette_fill_bone(state.cur_bone_idx);
			break;

		case item_hit_box:
			list_palette_set_title(&property_palette,"Hit Box Properties");
			property_palette_fill_hit_box(state.cur_hit_box_idx);
			break;

	}
}

/* =======================================================

      Property Palette Draw
      
======================================================= */

void property_palette_draw(void)
{
	property_palette_fill();
	list_palette_draw(&property_palette,TRUE);
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
	list_palette_scroll_wheel(&property_palette,pnt,move);
}

/* =======================================================

      Property Palette Click
      
======================================================= */

void property_palette_click(d3pnt *pnt,bool double_click)
{
	bool				old_open;

		// check if open changes
	
	old_open=list_palette_open;

		// click

	if (!list_palette_click(&property_palette,pnt,double_click)) {
		if (old_open!=list_palette_open) item_palette_state_rebuild();
		return;
	}

		// click editing

	if (property_palette.item_id==-1) return;

		// if texture window is up, texture properties

	if (state.texture_edit_idx!=-1) {
		property_palette_click_texture(state.texture_edit_idx,property_palette.item_id);
		return;
	}

		// if preference window is up, preference properties

	if (state.in_preference) {
		property_palette_click_animator_preference(property_palette.item_id);
		return;
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
			property_palette_click_bone(state.cur_bone_idx,property_palette.item_id);
			break;

		case item_hit_box:
			property_palette_click_hit_box(state.cur_hit_box_idx,property_palette.item_id);
			break;

	}

		// need to reset in case
		// the alt window has open/closed

	item_palette_state_rebuild();
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
	int				idx,sound_count,head_tag,tag;
	char			path[1024],sound_names[256][name_str_len];
	
		// load in the sounds

	idx=-1;
	sound_count=0;
		
	file_paths_data(&file_path_setup,path,"Settings","Sounds","xml");
	if (xml_open_file(path)) {
	
		head_tag=xml_findrootchild("Sounds");
		if (head_tag!=-1) {
	
			tag=xml_findfirstchild("Sound",head_tag);
		
			while (tag!=-1) {
				xml_get_attribute_text(tag,"name",sound_names[sound_count],name_str_len);
				if (strcmp(sound_names[sound_count],name)==0) idx=sound_count;
				sound_count++;
				tag=xml_findnextchild(tag);
			}
		}
		
		xml_close_file();
	}
	
		// run the dialog

	dialog_property_list_run((char*)sound_names,sound_count,name_str_len,0,include_none,&idx);
	
	name[0]=0x0;
	if (idx!=-1) strcpy(name,sound_names[idx]);
}

void property_palette_pick_particle(char *name)
{
	int				idx,particle_count,head_tag,data_head_tag,tag;
	char			path[1024],particle_names[256][name_str_len];
	
		// load in the particles

	idx=-1;
	particle_count=0;
	
	file_paths_data(&file_path_setup,path,"Settings","Particles","xml");
	if (xml_open_file(path)) {
	
		data_head_tag=xml_findrootchild("Particle_Data");
		
		if (data_head_tag==-1) {
			head_tag=xml_findrootchild("Particles");
		}
		else {
			head_tag=xml_findfirstchild("Particles",data_head_tag);
		}
	
		if (head_tag!=-1) {

			tag=xml_findfirstchild("Particle",head_tag);
			
			while (tag!=-1) {
				xml_get_attribute_text(tag,"name",particle_names[particle_count],name_str_len);
				if (strcmp(particle_names[particle_count],name)==0) idx=particle_count;
				particle_count++;
				tag=xml_findnextchild(tag);
			}
		}
		
		if (data_head_tag==-1) {
			head_tag=xml_findrootchild("Particle_Groups");
		}
		else {
			head_tag=xml_findfirstchild("Particle_Groups",data_head_tag);
		}

		if (head_tag!=-1) {

			tag=xml_findfirstchild("Particle_Group",head_tag);
			
			while (tag!=-1) {
				xml_get_attribute_text(tag,"name",particle_names[particle_count],name_str_len);
				if (strcmp(particle_names[particle_count],name)==0) idx=particle_count;
				particle_count++;
				tag=xml_findnextchild(tag);
			}
		}
	
		xml_close_file();
	}
	
		// run the dialog

	dialog_property_list_run((char*)particle_names,particle_count,name_str_len,0,FALSE,&idx);
	
	name[0]=0x0;
	if (idx!=-1) strcpy(name,particle_names[idx]);
}

void property_palette_pick_ring(char *name)
{
	int				idx,ring_count,head_tag,tag;
	char			path[1024],ring_names[256][name_str_len];
	
		// load in the rings

	idx=-1;
	ring_count=0;
		
	file_paths_data(&file_path_setup,path,"Settings","Rings","xml");
	if (xml_open_file(path)) {
	
		head_tag=xml_findrootchild("Rings");
		if (head_tag!=-1) {
	
			tag=xml_findfirstchild("Ring",head_tag);
		
			while (tag!=-1) {
				xml_get_attribute_text(tag,"name",ring_names[ring_count],name_str_len);
				if (strcmp(ring_names[ring_count],name)==0) idx=ring_count;
				ring_count++;
				tag=xml_findnextchild(tag);
			}
		}
		
		xml_close_file();
	}
	
		// run the dialog

	dialog_property_list_run((char*)ring_names,ring_count,name_str_len,0,FALSE,&idx);
	
	name[0]=0x0;
	if (idx!=-1) strcpy(name,ring_names[idx]);
}

void property_palette_pick_mesh(int *mesh_idx)
{
	dialog_property_list_run((char*)model.meshes,model.nmesh,sizeof(model_mesh_type),(int)offsetof(model_mesh_type,name),TRUE,mesh_idx);
}

void property_palette_pick_bone(int *bone_idx)
{
	dialog_property_list_run((char*)model.bones,model.nbone,sizeof(model_bone_type),(int)offsetof(model_bone_type,name),TRUE,bone_idx);
}

void property_palette_pick_bone_tag(unsigned long *bone_tag)
{
	int				idx;

	idx=model_find_bone(&model,*bone_tag);
	property_palette_pick_bone(&idx);
	if (idx==-1) {
		*bone_tag=model_null_tag;
		return;
	}

	*bone_tag=model.bones[idx].tag;
}

void property_palette_pick_pose(int *pose_idx)
{
	dialog_property_list_run((char*)model.poses,model.npose,sizeof(model_pose_type),(int)offsetof(model_pose_type,name),TRUE,pose_idx);
}

void property_palette_pick_shader(char *name)
{
	int					idx,shader_count,head_tag,tag;
	char				path[1024],shader_names[256][file_str_len];

	idx=-1;
	shader_count=0;
	
	file_paths_data(&file_path_setup,path,"Settings","Shaders","xml");
	if (xml_open_file(path)) {
	
		head_tag=xml_findrootchild("Shaders");
		if (head_tag!=-1) {

			tag=xml_findfirstchild("Shader",head_tag);
			
			while (tag!=-1) {
				xml_get_attribute_text(tag,"name",shader_names[shader_count],file_str_len);
				if ((idx==-1) && (strcasecmp(shader_names[shader_count],name)==0)) idx=shader_count;
				shader_count++;
				tag=xml_findnextchild(tag);
			}
		}
	
		xml_close_file();
	}

	dialog_property_list_run((char*)shader_names,shader_count,file_str_len,0,TRUE,&idx);

	name[0]=0x0;
	if (idx!=-1) strcpy(name,shader_names[idx]);
}

