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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "view.h"
#include "dialog.h"

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;
extern file_path_setup_type		file_path_setup;

extern int						tool_palette_pixel_sz,txt_palette_pixel_sz;
extern bool						list_palette_open;

int								prop_last_sel_type;
list_palette_type				property_palette;

/* =======================================================

      Property Palette Setup
      
======================================================= */

void property_palette_initialize(void)
{
	list_palette_list_initialize(&property_palette,"Item Properties");

	property_palette.item_type=0;
	property_palette.item_idx=-1;

	prop_last_sel_type=-1;
}

void property_palette_shutdown(void)
{
	list_palette_list_shutdown(&property_palette);
}

void property_palette_setup(void)
{
	int				y;
	d3rect			wbox;
	
	os_get_window_box(&wbox);

	if (list_palette_open) {
		property_palette.pixel_sz=list_palette_tree_sz;
	}
	else {
		property_palette.pixel_sz=list_palette_border_sz;
	}

	y=wbox.ty+(((wbox.by-wbox.ty)/3)*2);

	property_palette.box.lx=wbox.rx-property_palette.pixel_sz;
	property_palette.box.rx=wbox.rx;
	property_palette.box.ty=y-1;
	property_palette.box.by=wbox.by-txt_palette_pixel_sz;

	property_palette.scroll_size=((property_palette.box.by-property_palette.box.ty)-((list_scroll_button_high*2)+list_title_high))>>2;
}

/* =======================================================

      Property Palette Fill
      
======================================================= */

void property_palette_fill(void)
{
	int					sel_type,main_idx,sub_idx;

		// delete the properties

	list_palette_delete_all_items(&property_palette);

		// fill in the properties for
		// the currently selected item

	main_idx=-1;
	
	if (select_count()!=0)  select_get(0,&sel_type,&main_idx,&sub_idx);
	
	if (main_idx==-1) {
		list_palette_set_title(&property_palette,"No Selection");
		return;
	}

	switch (sel_type) {

		case mesh_piece:
			list_palette_set_title(&property_palette,"Mesh Properties");
			if (state.drag_mode!=drag_mode_polygon) sub_idx=-1;
			property_palette_fill_mesh(main_idx,sub_idx);
			break;

		case liquid_piece:
			list_palette_set_title(&property_palette,"Liquid Properties");
			property_palette_fill_liquid(main_idx);
			break;

		case spot_piece:
			list_palette_set_title(&property_palette,"Spot Properties");
			property_palette_fill_spot(main_idx);
			break;

		case light_piece:
			list_palette_set_title(&property_palette,"Light Properties");
			property_palette_fill_light(main_idx);
			break;

		case sound_piece:
			list_palette_set_title(&property_palette,"Sound Properties");
			property_palette_fill_sound(main_idx);
			break;

		case particle_piece:
			list_palette_set_title(&property_palette,"Particle Properties");
			property_palette_fill_particle(main_idx);
			break;

		case scenery_piece:
			list_palette_set_title(&property_palette,"Scenery Properties");
			property_palette_fill_scenery(main_idx);
			break;

		case node_piece:
			list_palette_set_title(&property_palette,"Node Properties");
			property_palette_fill_node(main_idx);
			break;

	}
}

/* =======================================================

      Property Palette Draw
      
======================================================= */

void property_palette_draw(void)
{
	property_palette_fill();
	list_palette_draw(&property_palette);
}

/* =======================================================

      Property Palette Reset For Selection Change
      
======================================================= */

void property_palette_reset(void)
{
	int				sel_type,main_idx,sub_idx;

	if (select_count()==0) {
		prop_last_sel_type=-1;
		property_palette.scroll_page=0;
		return;
	}

	select_get(0,&sel_type,&main_idx,&sub_idx);
	if (main_idx==-1) {
		prop_last_sel_type=-1;
		property_palette.scroll_page=0;
		return;
	}

	if (prop_last_sel_type!=sel_type) {
		prop_last_sel_type=sel_type;
		property_palette.scroll_page=0;
	}
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
	int					sel_type,main_idx,sub_idx;
	bool				old_open;

		// check if open changes
	
	old_open=list_palette_open;

		// click

	if (!list_palette_click(&property_palette,pnt,double_click)) {
		if (old_open!=list_palette_open) {
			item_palette_setup();
			property_palette_setup();
			main_wind_draw();
		}
		return;
	}

		// click editing

	if (property_palette.item_id==-1) return;

		// get the selection

	if (select_count()==0) return;
	
	select_get(0,&sel_type,&main_idx,&sub_idx);
	if (main_idx==-1) return;

	switch (sel_type) {

		case mesh_piece:
			if (state.drag_mode!=drag_mode_polygon) sub_idx=-1;
			property_palette_click_mesh(main_idx,sub_idx,property_palette.item_id);
			break;

		case liquid_piece:
			property_palette_click_liquid(main_idx,property_palette.item_id);
			break;

		case spot_piece:
			property_palette_click_spot(main_idx,property_palette.item_id);
			break;

		case light_piece:
			property_palette_click_light(main_idx,property_palette.item_id);
			break;

		case sound_piece:
			property_palette_click_sound(main_idx,property_palette.item_id);
			break;

		case particle_piece:
			property_palette_click_particle(main_idx,property_palette.item_id);
			break;

		case scenery_piece:
			property_palette_click_scenery(main_idx,property_palette.item_id);
			break;

		case node_piece:
			property_palette_click_node(main_idx,property_palette.item_id);
			break;

	}
}

/* =======================================================

      Property Palette Editing Utilities
      
======================================================= */

void property_palette_pick_list(char *list,int *idx)
{
	int			count;
	char		*c;

		// get count

	c=list;
	count=0;

	while (TRUE) {
		if (*c==0x0) break;
		c+=name_str_len;
		count++;
	}

		// run dialog

	dialog_property_list_run(list,count,name_str_len,0,FALSE,idx);
}

void property_palette_pick_group(int *group_idx)
{
	dialog_property_list_run((char*)map.group.groups,map.group.ngroup,sizeof(group_type),(int)offsetof(group_type,name),TRUE,group_idx);
}

void property_palette_pick_spot(char *name)
{
	int				n,idx;
	
	idx=-1;
	
	for (n=0;n!=map.nnode;n++) {
		if (strcmp(map.spots[n].name,name)==0) {
			idx=n;
			break;
		}
	}

	dialog_property_list_run((char*)map.spots,map.nspot,sizeof(spot_type),(int)offsetof(spot_type,name),TRUE,&idx);
	
	name[0]=0x0;
	if (idx!=-1) strcpy(name,map.spots[idx].name);
}

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

void property_palette_pick_node(char *name)
{
	int				n,count,mem_sz,list_pos,idx;
	char			*list_ptr,*list;
	
		// only pick from nodes with names
		
	count=0;
	
	for (n=0;n!=map.nnode;n++) {
		if (map.nodes[n].name[0]!=0x0) count++;
	}
	
	if (count==0) {
		mem_sz=name_str_len;
	}
	else {
		mem_sz=count*name_str_len;
	}
	
	list_ptr=(char*)malloc(mem_sz);
	if (list_ptr==NULL) return;
	
	idx=-1;
	list_pos=0;
	
	for (n=0;n!=map.nnode;n++) {
		if (map.nodes[n].name[0]==0x0) continue;
		
		list=list_ptr+(list_pos*name_str_len);
		strcpy(list,map.nodes[n].name);
			
		if (strcmp(map.nodes[n].name,name)==0) idx=list_pos;
		list_pos++;
	}
	
	dialog_property_list_run(list_ptr,count,name_str_len,0,TRUE,&idx);
	
	name[0]=0x0;
	if (idx!=-1) strcpy(name,(list_ptr+(idx*name_str_len)));
	
	free(list_ptr);
}

