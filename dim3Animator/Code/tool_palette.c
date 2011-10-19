/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Tool Palette

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "interface.h"

extern model_type				model;
extern file_path_setup_type		file_path_setup;
extern animator_state_type		state;

/* =======================================================

      Tool Palette Setup
      
======================================================= */

int tool_palette_pixel_size(void)
{
	int				pixel_sz;
	d3rect			wbox;
	
	os_get_window_box(&wbox);

	pixel_sz=(wbox.rx-wbox.lx)/tool_count;
	if (pixel_sz>tool_button_size) pixel_sz=tool_button_size;
	
	return(pixel_sz);
}

void tool_palette_box(d3rect *box)
{
	int				pixel_sz;
	d3rect			wbox;
	
	pixel_sz=tool_palette_pixel_size();
	
	os_get_window_box(&wbox);
	
	box->lx=wbox.lx;
	box->rx=wbox.rx;
	box->ty=wbox.ty;
	box->by=box->ty+(pixel_sz+1);
}


/* =======================================================

      Tool Palette State
      
======================================================= */

bool tool_get_highlight_state(int tool_idx)
{
	switch (tool_idx) {

		case 0:
			return(state.texture);

		case 1:
			return(state.mesh);

		case 2:
			return(state.bone);

		case 3:
			return(state.hit_box);

		case 4:
			return(state.select_mode==select_mode_mesh);

		case 5:
			return(state.select_mode==select_mode_polygon);

		case 6:
			return(state.select_mode==select_mode_vertex);

		case 7:
			return(state.view_box);

		case 8:
			return(state.normal);

		case 9:
			return(state.bone_names);

		case 10:
			return(state.sel_vertex_with_bone);

		case 11:
			return(state.drag_bone_mode==drag_bone_mode_rotate);

		case 12:
			return(state.drag_bone_mode==drag_bone_mode_stretch);

		case 13:
			return(state.playing);
			
	}

	return(FALSE);
}

/* =======================================================

      Tool Palette Button Clicks
      
======================================================= */

void tool_click(int tool_idx)
{
	switch (tool_idx) {
	
		case 0:
			state.texture=!state.texture;
			break;
			
		case 1:
			state.mesh=!state.mesh;
			break;
			
		case 2:
			state.bone=!state.bone;
			break;
			
		case 3:
			state.hit_box=!state.hit_box;
			break;
			
		case 4:
			vertex_mask_clear_sel(state.cur_mesh_idx);
			state.select_mode=select_mode_mesh;
			break;

		case 5:
			vertex_mask_clear_sel(state.cur_mesh_idx);
			state.select_mode=select_mode_polygon;
			break;
			
		case 6:
			vertex_mask_clear_sel(state.cur_mesh_idx);
			state.select_mode=select_mode_vertex;
			break;
			
		case 7:
			state.view_box=!state.view_box;
			break;
			
		case 8:
			state.normal=!state.normal;
			break;
			
		case 9:
			state.bone_names=!state.bone_names;
			break;
			
		case 10:
			state.sel_vertex_with_bone=!state.sel_vertex_with_bone;
			break;
		
		case 11:
			state.drag_bone_mode=drag_bone_mode_rotate;
			break;
			
		case 12:
			state.drag_bone_mode=drag_bone_mode_stretch;
			break;
			
		case 13:
			main_wind_play(!state.playing,FALSE);
			break;
	}
	
	main_wind_draw();
}

