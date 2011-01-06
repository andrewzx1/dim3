/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Item Palette

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
#include "dialog.h"

extern model_type				model;
extern model_draw_setup			draw_setup;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern int						tool_palette_pixel_sz,txt_palette_pixel_sz;
extern bool						list_palette_open;

list_palette_type				item_palette;

/* =======================================================

      Item Palette Setup
      
======================================================= */

void item_palette_initialize(void)
{
	list_palette_list_initialize(&item_palette,"Model Items");

	item_palette.item_type=0;
	item_palette.item_idx=-1;
}

void item_palette_shutdown(void)
{
	list_palette_list_shutdown(&item_palette);
}

void item_palette_setup(void)
{
	int				y;
	d3rect			wbox;
	
	os_get_window_box(&wbox);

	if (list_palette_open) {
		item_palette.pixel_sz=list_palette_tree_sz;
	}
	else {
		item_palette.pixel_sz=list_palette_border_sz;
	}

	y=wbox.ty+((wbox.by-wbox.ty)>>1);

	item_palette.box.lx=wbox.rx-item_palette.pixel_sz;
	item_palette.box.rx=wbox.rx;
	item_palette.box.ty=wbox.ty+(tool_palette_pixel_sz+1);
	item_palette.box.by=y;
}

/* =======================================================

      Item Palette Fill
      
======================================================= */

void item_palette_fill(void)
{
	int			n;

	list_palette_delete_all_items(&item_palette);

		// meshes

	list_palette_add_header(&item_palette,item_mesh,"Meshes");

	for (n=0;n!=model.nmesh;n++) {
		list_palette_add_item(&item_palette,item_mesh,n,model.meshes[n].name,(state.cur_mesh_idx==n),FALSE);
	}

		// animations

	list_palette_add_header(&item_palette,item_animation,"Animations");

	for (n=0;n!=model.nanimate;n++) {
		list_palette_add_item(&item_palette,item_animation,n,model.animates[n].name,(state.cur_animate_idx==n),FALSE);
	}

		// poses

	list_palette_add_header(&item_palette,item_pose,"Poses");

	for (n=0;n!=model.npose;n++) {
		list_palette_add_item(&item_palette,item_pose,n,model.poses[n].name,(state.cur_pose_idx==n),FALSE);
	}

		// bones

	list_palette_add_header(&item_palette,item_bone,"Bones");

	for (n=0;n!=model.nbone;n++) {
		list_palette_add_item(&item_palette,item_bone,n,model.bones[n].name,(state.cur_bone_idx==n),FALSE);
	}

		// hit boxes

	list_palette_add_header(&item_palette,item_hit_box,"Hit Boxes");

	for (n=0;n!=model.nhit_box;n++) {
		list_palette_add_item(&item_palette,item_hit_box,n,model.hit_boxes[n].name,FALSE,FALSE);
	}
}

/* =======================================================

      Item Palette Draw
      
======================================================= */

void item_palette_draw(void)
{
	item_palette_fill();
	list_palette_draw(&item_palette);
}

/* =======================================================

      Item Palette Reset For Selection Change
      
======================================================= */
/* supergumba
void item_palette_reset(void)
{
	int				sel_type,main_idx,sub_idx;

	if (select_count()==0) {
		item_palette.item_type=-1;
		item_palette.item_idx=-1;
		return;
	}
	
	select_get(0,&sel_type,&main_idx,&sub_idx);
	if ((sel_type==mesh_piece) || (sel_type==liquid_piece)) {
		item_palette.item_idx=-1;
		return;
	}

	item_palette.item_type=sel_type;
	item_palette.item_idx=main_idx;
}

void item_palette_select(int sel_type,int sel_idx)
{
	item_palette.item_type=sel_type;
	item_palette.item_idx=sel_idx;
	
	main_wind_draw();
}
*/
/* =======================================================

      Item Palette Delete
      
======================================================= */
/* supergumba
bool item_palette_delete(void)
{
		// anything to delete?

	if ((item_palette.item_type==-1) || (item_palette.item_idx==-1)) return(FALSE);

	switch (item_palette.item_type) {

		case group_piece:
			if (os_dialog_confirm("Delete Group","Is it okay to delete this group?",FALSE)!=0) return(FALSE);
			map_group_delete(&map,item_palette.item_idx);
			item_palette_reset();
			return(TRUE);

		case movement_piece:
			if (os_dialog_confirm("Delete Movement","Is it okay to delete this movement?",FALSE)!=0) return(FALSE);
			map_movement_delete(&map,item_palette.item_idx);
			item_palette_reset();
			return(TRUE);

		case cinema_piece:
			if (os_dialog_confirm("Delete Cinema","Is it okay to delete this cinema?",FALSE)!=0) return(FALSE);
			map_cinema_delete(&map,item_palette.item_idx);
			item_palette_reset();
			return(TRUE);

	}

	return(FALSE);
}
*/
/* =======================================================

      Item Palette Scroll Wheel
      
======================================================= */

void item_palette_scroll_wheel(d3pnt *pnt,int move)
{
	list_palette_scroll_wheel(&item_palette,pnt,move);
}

/* =======================================================

      Item Palette Click
      
======================================================= */

void item_palette_click(d3pnt *pnt,bool double_click)
{
	bool					old_open;

		// check if open changes
	
	old_open=list_palette_open;

		// click

	if (!list_palette_click(&item_palette,pnt,double_click)) {
		if (old_open!=list_palette_open) {
			item_palette_setup();
			property_palette_setup();
			model_wind_setup();
			main_wind_draw();
		}
		return;
	}

	if (item_palette.item_idx==-1) return;

	/*

		// handle click

	select_clear();

	switch (item_palette.item_type) {
		case group_piece:
			select_add_group(item_palette.item_idx);
			break;
		case movement_piece:
			select_add_movement(item_palette.item_idx);
			break;
		case cinema_piece:
			select_add_cinema(item_palette.item_idx);
			break;
		default:
			select_add(item_palette.item_type,item_palette.item_idx,-1);
			break;
	}

	if (double_click) view_goto_select();

		// turn on any hidden items

	switch (item_palette.item_type) {

		case spot_piece:
		case scenery_piece:
			state.show_object=TRUE;
			break;

		case light_piece:
		case sound_piece:
		case particle_piece:
			state.show_lightsoundparticle=TRUE;
			break;

		case node_piece:
			state.show_node=TRUE;
			break;
	}

	main_wind_draw();
	
		// any double click

	if ((!double_click) || (item_palette.item_idx==-1)) return;
	
		// if double click, edit

	switch (item_palette.item_type) {

		case cinema_piece:
			dialog_cinema_settings_run(item_palette.item_idx);
			break;

		case group_piece:
			dialog_group_settings_run(item_palette.item_idx);
			break;

		case movement_piece:
			dialog_movement_settings_run(item_palette.item_idx);
			break;

	}
	*/
}

