/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Tool Palette

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

extern int						tool_palette_pixel_sz;
extern d3rect					tool_palette_box;

extern model_type				model;
extern file_path_setup_type		file_path_setup;
extern animator_state_type		state;

/* =======================================================

      Tool Palette Setup
      
======================================================= */

void tool_palette_setup(void)
{
	d3rect			wbox;
	
	os_get_window_box(&wbox);

	tool_palette_pixel_sz=(wbox.rx-wbox.lx)/tool_count;
	if (tool_palette_pixel_sz>tool_button_size) tool_palette_pixel_sz=tool_button_size;
	
	tool_palette_box.lx=wbox.lx;
	tool_palette_box.rx=wbox.rx;
	tool_palette_box.ty=wbox.ty;
	tool_palette_box.by=wbox.ty+(tool_palette_pixel_sz+1);
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
			return(state.select_mode==select_mode_polygon);

		case 5:
			return(state.select_mode==select_mode_vertex);

		case 6:
			return(state.view_box);

		case 7:
			return(state.normal);

		case 8:
			return(state.first_mesh);

		case 9:
			return(state.drag_bone_mode==drag_bone_mode_rotate);

		case 10:
			return(state.drag_bone_mode==drag_bone_mode_stretch);

		case 11:
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
			vertex_clear_sel_mask(state.cur_mesh_idx);
			state.select_mode=select_mode_polygon;
			break;
			
		case 5:
			vertex_clear_sel_mask(state.cur_mesh_idx);
			state.select_mode=select_mode_vertex;
			break;
			
		case 6:
			state.view_box=!state.view_box;
			break;
			
		case 7:
			state.normal=!state.normal;
			break;
			
		case 8:
			state.first_mesh=!state.first_mesh;
			break;
			
		case 9:
			state.drag_bone_mode=drag_bone_mode_rotate;
			break;
			
		case 10:
			state.drag_bone_mode=drag_bone_mode_stretch;
			break;
			
		case 11:
			main_wind_play(!state.playing,FALSE);
			break;
	}
	
	main_wind_draw();
}

