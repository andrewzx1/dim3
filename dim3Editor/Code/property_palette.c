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

	property_palette.scroll_size=((property_palette.box.by-property_palette.box.ty)-((list_scroll_button_high*2)+list_title_high))>>1;
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

	if (select_count()==0) return;

	select_get(0,&sel_type,&main_idx,&sub_idx);
	if (main_idx==-1) return;

	switch (sel_type) {

		case mesh_piece:
			if (state.drag_mode!=drag_mode_polygon) {
				property_palette_fill_mesh(&map.mesh.meshes[main_idx]);
			}
			else {
				property_palette_fill_polygon(&map.mesh.meshes[main_idx].polys[sub_idx],NULL);
			}
			break;

		case liquid_piece:
			if (state.drag_mode!=drag_mode_polygon) {
				property_palette_fill_liquid(&map.liquid.liquids[main_idx]);
			}
			else {
				property_palette_fill_polygon(NULL,&map.liquid.liquids[main_idx]);
			}
			break;

		case spot_piece:
			property_palette_fill_spot(&map.spots[main_idx]);
			break;

		case light_piece:
			property_palette_fill_light(&map.lights[main_idx]);
			break;

		case sound_piece:
			property_palette_fill_sound(&map.sounds[main_idx]);
			break;

		case particle_piece:
			property_palette_fill_particle(&map.particles[main_idx]);
			break;

		case scenery_piece:
			property_palette_fill_scenery(&map.sceneries[main_idx]);
			break;

		case node_piece:
			property_palette_fill_node(&map.nodes[main_idx]);
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
			if (state.drag_mode!=drag_mode_polygon) {
				property_palette_click_mesh(&map.mesh.meshes[main_idx],property_palette.item_id);
			}
			else {
				property_palette_click_polygon(&map.mesh.meshes[main_idx].polys[sub_idx],NULL,property_palette.item_id);
			}
			break;

		case liquid_piece:
			if (state.drag_mode!=drag_mode_polygon) {
				property_palette_click_liquid(&map.liquid.liquids[main_idx],property_palette.item_id);
			}
			else {
				property_palette_click_polygon(NULL,&map.liquid.liquids[main_idx],property_palette.item_id);
			}
			break;

		case spot_piece:
			property_palette_click_spot(&map.spots[main_idx],property_palette.item_id);
			break;

		case light_piece:
			property_palette_click_light(&map.lights[main_idx],property_palette.item_id);
			break;

		case sound_piece:
			property_palette_click_sound(&map.sounds[main_idx],property_palette.item_id);
			break;

		case particle_piece:
			property_palette_click_particle(&map.particles[main_idx],property_palette.item_id);
			break;

		case scenery_piece:
			property_palette_click_scenery(&map.sceneries[main_idx],property_palette.item_id);
			break;

		case node_piece:
			property_palette_click_node(&map.nodes[main_idx],property_palette.item_id);
			break;

	}
}
