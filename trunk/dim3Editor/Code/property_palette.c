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
#include "ui_common.h"
#include "interface.h"

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;
extern iface_type				iface;
extern file_path_setup_type		file_path_setup;

extern int						tool_palette_pixel_sz,txt_palette_pixel_sz;
extern bool						list_palette_open,alt_property_open;
extern list_palette_type		item_palette;

list_palette_type				property_palette;

/* =======================================================

      Property Palette Setup
      
======================================================= */

void property_palette_initialize(void)
{
	list_palette_list_initialize(&property_palette,"Item Properties");

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

	x=wbox.rx-item_palette.pixel_sz;
	if ((list_palette_open) && (alt_property_open)) x-=item_palette.pixel_sz;

	y=wbox.ty+((wbox.by-wbox.ty)>>1);

	property_palette.box.lx=x;
	property_palette.box.rx=x+item_palette.pixel_sz;
	property_palette.box.ty=y-1;
	property_palette.box.by=wbox.by-txt_palette_pixel_sz;
}

/* =======================================================

      Property Palette Fill
      
======================================================= */

void property_palette_fill(void)
{
	int					sel_type,main_idx,sub_idx;

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
		// put in preferences

	if (state.in_preference) {
		list_palette_set_title(&property_palette,"Editor Preferences");
		property_palette_fill_editor_preference();
		return;
	}

		// fill in the properties for
		// the currently selected item

		// special check for non-selection property lists

	switch (item_palette.item_type) {

		case cinema_piece:
			list_palette_set_title(&property_palette,"Cinema Properties");
			property_palette_fill_cinema(item_palette.item_idx);
			return;

		case group_piece:
			list_palette_set_title(&property_palette,"Group Properties");
			property_palette_fill_group(item_palette.item_idx);
			return;

		case movement_piece:
			list_palette_set_title(&property_palette,"Movement Properties");
			property_palette_fill_movement(item_palette.item_idx);
			return;

	}

		// check selection

	main_idx=-1;
	
	if (select_count()!=0)  select_get(0,&sel_type,&main_idx,&sub_idx);
	
		// no selection, map properties

	if (main_idx==-1) {
		list_palette_set_title(&property_palette,"Map Properties");
		property_palette_fill_map();
		return;
	}

		// selection properties

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
	list_palette_draw(&property_palette,TRUE);
}

/* =======================================================

      Property Palette Reset For Selection Change
      
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
	int					sel_type,main_idx,sub_idx;
	bool				old_open;

		// check if open changes
	
	old_open=list_palette_open;

		// click

	if (!list_palette_click(&property_palette,pnt,double_click)) {
		if (old_open!=list_palette_open) {
			item_palette_setup();
			property_palette_setup();
			alt_property_palette_setup();
			main_wind_draw();
		}
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
		property_palette_click_editor_preference(property_palette.item_id);
		return;
	}

		// special check for non-selection property lists
		// and check state changes

	switch (item_palette.item_type) {

		case cinema_piece:
			state.cur_cinema_idx=item_palette.item_idx;
			property_palette_click_cinema(item_palette.item_idx,property_palette.item_id);
			alt_property_fix_open_state();
			return;

		case group_piece:
			property_palette_click_group(item_palette.item_idx,property_palette.item_id);
			alt_property_fix_open_state();
			return;

		case movement_piece:
			state.cur_movement_idx=item_palette.item_idx;
			property_palette_click_movement(item_palette.item_idx,property_palette.item_id);
			alt_property_fix_open_state();
			return;

	}

		// need to do the setup again incase
		// the alt window has open/closed

	alt_property_fix_open_state();

		// get the selection

	main_idx=-1;
	
	if (select_count()!=0) select_get(0,&sel_type,&main_idx,&sub_idx);

		// no selection, map properties

	if (main_idx==-1) {
		property_palette_click_map(property_palette.item_id);
		return;
	}

		// selection properties

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

      Property Palette List Utilities
      
======================================================= */

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
	int				idx;

	dialog_property_list_run((char*)iface.sound_list.sounds,iface.sound_list.nsound,sizeof(iface_sound_type),(int)offsetof(iface_sound_type,name),include_none,&idx);

	name[0]=0x0;
	if (idx!=-1) strcpy(name,iface.sound_list.sounds[idx].name);
}

void property_palette_pick_particle(char *name)
{
	int				idx;

	dialog_property_list_run((char*)iface.particle_list.particles,iface.particle_list.nparticle,sizeof(iface_particle_type),(int)offsetof(iface_particle_type,name),FALSE,&idx);

	name[0]=0x0;
	if (idx!=-1) strcpy(name,iface.particle_list.particles[idx].name);
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

void property_palette_pick_movement(char *name)
{
	int				n,idx;
	
	idx=-1;
	
	for (n=0;n!=map.movement.nmovement;n++) {
		if (strcmp(map.movement.movements[n].name,name)==0) {
			idx=n;
			break;
		}
	}

	dialog_property_list_run((char*)map.movement.movements,map.movement.nmovement,sizeof(movement_type),(int)offsetof(movement_type,name),TRUE,&idx);
	
	name[0]=0x0;
	if (idx!=-1) strcpy(name,map.movement.movements[idx].name);
}

void property_palette_pick_hud_text(char *name)
{
	int				idx;

	dialog_property_list_run((char*)iface.text_list.texts,iface.text_list.ntext,sizeof(iface_text_type),(int)offsetof(iface_text_type,name),FALSE,&idx);

	name[0]=0x0;
	if (idx!=-1) strcpy(name,iface.text_list.texts[idx].name);
}

void property_palette_pick_hud_bitmap(char *name)
{
	int				idx;

	dialog_property_list_run((char*)iface.bitmap_list.bitmaps,iface.bitmap_list.nbitmap,sizeof(iface_bitmap_type),(int)offsetof(iface_bitmap_type,name),FALSE,&idx);

	name[0]=0x0;
	if (idx!=-1) strcpy(name,iface.bitmap_list.bitmaps[idx].name);
}

void property_palette_pick_texture(int *txt_idx)
{
	int				n;
	char			texture_names[max_map_texture][name_str_len];
	
	for (n=0;n!=max_map_texture;n++) {
		if (map.textures[n].frames[0].name[0]==0x0) {
			strcpy((char*)texture_names[n],"(none)");
		}
		else {
			strcpy((char*)texture_names[n],map.textures[n].frames[0].name);
		}
	}

	dialog_property_list_run((char*)texture_names,max_map_texture,name_str_len,0,TRUE,txt_idx);
}

void property_palette_pick_shader(char *name)
{
	int				idx;

	dialog_property_list_run((char*)iface.shader_list.shaders,iface.shader_list.nshader,sizeof(iface_shader_type),(int)offsetof(iface_shader_type,name),TRUE,&idx);

	name[0]=0x0;
	if (idx!=-1) strcpy(name,iface.shader_list.shaders[idx].name);
}

