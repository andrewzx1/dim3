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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "interface.h"

extern model_type				model;
extern file_path_setup_type		file_path_setup;
extern app_state_type			state;

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

			// left side

		case 0:
			return(state.model.texture);

		case 1:
			return(state.model.mesh);

		case 2:
			return(state.model.bone);

		case 3:
			return(state.model.hit_box);

		case 4:
			return(state.model.select_mode==select_mode_mesh);

		case 5:
			return(state.model.select_mode==select_mode_polygon);

		case 6:
			return(state.model.select_mode==select_mode_vertex);

		case 7:
			return(state.model.view_box);

		case 8:
			return(state.model.normal);

		case 9:
			return(state.model.bone_names);

		case 10:
			return(state.model.sel_vertex_with_bone);

		case 11:
			return(state.model.drag_bone_mode==drag_bone_mode_rotate);

		case 12:
			return(state.model.drag_bone_mode==drag_bone_mode_stretch);

			// right side

		case 14:
			return(state.model.play_mode==play_mode_stop);

		case 15:
			return(state.model.play_mode==play_mode_normal);

		case 16:
			return(state.model.play_mode==play_mode_blend);

		case 17:
			return(state.model.play_mode==play_mode_slow);
			
		case 18:
			return(state.model.play_mode==play_mode_prev);
			
		case 19:
			return(state.model.play_mode==play_mode_next);
			
	}

	return(FALSE);
}

bool tool_get_disabled_state(int tool_idx)
{
	return(!state.model.model_open);
}

/* =======================================================

      Tool Palette Button Clicks
      
======================================================= */

void tool_click(int tool_idx)
{
	switch (tool_idx) {
	
			// left side

		case 0:
			state.model.texture=!state.model.texture;
			break;
			
		case 1:
			state.model.mesh=!state.model.mesh;
			break;
			
		case 2:
			state.model.bone=!state.model.bone;
			break;
			
		case 3:
			state.model.hit_box=!state.model.hit_box;
			break;
			
		case 4:
			vertex_mask_clear_sel(state.model.cur_mesh_idx);
			state.model.select_mode=select_mode_mesh;
			break;

		case 5:
			vertex_mask_clear_sel(state.model.cur_mesh_idx);
			state.model.select_mode=select_mode_polygon;
			break;
			
		case 6:
			vertex_mask_clear_sel(state.model.cur_mesh_idx);
			state.model.select_mode=select_mode_vertex;
			break;
			
		case 7:
			state.model.view_box=!state.model.view_box;
			break;
			
		case 8:
			state.model.normal=!state.model.normal;
			break;
			
		case 9:
			state.model.bone_names=!state.model.bone_names;
			break;
			
		case 10:
			state.model.sel_vertex_with_bone=!state.model.sel_vertex_with_bone;
			break;
		
		case 11:
			state.model.drag_bone_mode=drag_bone_mode_rotate;
			break;
			
		case 12:
			state.model.drag_bone_mode=drag_bone_mode_stretch;
			break;
			
			// right side

		case 14:
			main_wind_play(play_mode_stop);
			break;

		case 15:
			main_wind_play(play_mode_normal);
			break;

		case 16:
			if (dialog_play_blend_animation_run()) main_wind_play(play_mode_blend);
			break;

		case 17:
			main_wind_play(play_mode_slow);
			break;

		case 18:
			main_wind_play(play_mode_prev);
			break;

		case 19:
			main_wind_play(play_mode_next);
			break;
	}
	
	main_wind_draw();
}

