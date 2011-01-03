/****************************** File *********************************

Module: dim3 Editor
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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "dialog.h"

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;
extern file_path_setup_type		file_path_setup;

extern int						tool_palette_pixel_sz;
extern d3rect					tool_palette_box;

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
	tool_palette_box.ty=wbox.ty+1;
	tool_palette_box.by=tool_palette_box.ty+(tool_palette_pixel_sz+1);
}

/* =======================================================

      Tool Palette State
      
======================================================= */

bool tool_get_highlight_state(int tool_idx)
{
	editor_view_type		*view;

	switch (tool_idx) {

			// vertex mode

		case 0:
			return(state.vertex_mode==vertex_mode_none);
		case 1:
			return(state.vertex_mode==vertex_mode_lock);
		case 2:
			return(state.vertex_mode==vertex_mode_snap);

			// free look and select

		case 3:
			return(state.free_look);
		case 4:
			return(state.select_add);

			// drag mode

		case 5:
			return(state.drag_mode==drag_mode_mesh);
		case 6:
			return(state.drag_mode==drag_mode_polygon);
		case 7:
			return(state.drag_mode==drag_mode_vertex);
			
			// grids
			
		case 11:
		case 12:
		case 13:
			return(state.grid_mode==(tool_idx-11));

			// auto texture

		case 14:
			return(state.auto_texture);

			// handle mode

		case 15:
			return(state.handle_mode==handle_mode_rotate);
		case 16:
			return(state.handle_mode==handle_mode_move);

			// node mode

		case 17:
			return(state.node_mode==node_mode_select);
		case 18:
			return(state.node_mode==node_mode_link);
		case 19:
			return(state.node_mode==node_mode_remove_link);
			
			// normals and culling

		case 20:
			view=view_get_current_view();
			return(state.show_normals);
		case 21:
			view=view_get_current_view();
			return(view->cull);
	}

	return(FALSE);
}

/* =======================================================

      Tool Palette Button Clicks
      
======================================================= */

void tool_click(int tool_idx)
{
	editor_view_type		*view;

		// if in texture view, any tool click
		// leaves it

	if (state.view_texture_idx!=-1) view_texture_switch(-1);

		// run the click

	switch (tool_idx) {
	
			// vertex mode
			
		case 0:
			state.vertex_mode=vertex_mode_none;
			break;
			
		case 1:
			state.vertex_mode=vertex_mode_lock;
			break;
			
		case 2:
			state.vertex_mode=vertex_mode_snap;
			break;
			
			// free look and selection toggle
			
		case 3:
			state.free_look=!state.free_look;
			break;

		case 4:
			state.select_add=!state.select_add;
			break;
			
			// drag mode buttons
			
		case 5:
			state.drag_mode=drag_mode_mesh;
			break;
			
		case 6:
			state.drag_mode=drag_mode_polygon;
			break;
			
		case 7:
			state.drag_mode=drag_mode_vertex;
			break;
			
			// mesh polygons
			
		case 8:
			piece_combine_mesh();
			break;
			
		case 9:
			piece_split_mesh();
			break;

		case 10:
			piece_tesselate();
			break;
			
			
			// grids
			
		case 11:
		case 12:
		case 13:
			state.grid_mode=tool_idx-11;
			break;
			
			// auto-texture
			
		case 14:
			state.auto_texture=!state.auto_texture;
			break;
			
			// handle modes
			
		case 15:
			state.handle_mode=handle_mode_rotate;
			break;
			
		case 16:
			state.handle_mode=handle_mode_move;
			break;
			
			// node editing
			
		case 17:
			state.show_node=TRUE;
			menu_update_view();
			state.node_mode=node_mode_select;
			break;
			
		case 18:
			state.show_node=TRUE;
			menu_update_view();
			state.node_mode=node_mode_link;
			break;
			
		case 19:
			state.show_node=TRUE;
			menu_update_view();
			state.node_mode=node_mode_remove_link;
			break;
			
			// normals
			
		case 20:
			state.show_normals=!state.show_normals;
			break;
			
		case 21:
			view=view_get_current_view();
			view_cull(!view->cull);
			break;
			
			// script and run buttons
			
		case 22:
			launch_map_script_editor();
			break;
			
		case 23:
			launch_engine();
			break;
			
			// pieces create
			
		case 25:
			piece_create_spot();
			break;
			
		case 26:
			piece_create_light();
			break;
			
		case 27:
			piece_create_sound();
			break;
			
		case 28:
			piece_create_particle();
			break;
			
		case 29:
			piece_create_scenery();
			break;
			
		case 30:
			piece_create_node();
			break;
			
		case 31:
			piece_add_obj_mesh();
			break;
			
		case 32:
			piece_add_obj_mesh_uv();
			break;
			
		case 33:
			piece_add_height_map_mesh();
			break;
			
		case 34:
			piece_add_grid_mesh();
			break;
			
		case 35:
			piece_add_polygon_mesh();
			break;
			
		case 36:
			piece_create_liquid();
			break;
			
	}
	
	menu_fix_enable();
	texture_palette_reset();
	
	main_wind_draw();
}

/* =======================================================

      Values and States
      
======================================================= */

void tool_default(void)
{
	state.free_look=setup.free_look;
	state.select_add=FALSE;
	
    state.vertex_mode=vertex_mode_none;
	state.drag_mode=drag_mode_mesh;
	state.grid_mode=grid_mode_small;
	state.node_mode=node_mode_select;
	state.handle_mode=handle_mode_rotate;
	
	state.auto_texture=setup.auto_texture;
	
	state.show_liquid=TRUE;
	state.show_object=TRUE;
	state.show_lightsoundparticle=TRUE;
	state.show_node=FALSE;
	
	state.show_normals=FALSE;
}

void tool_switch_vertex_mode(void)
{
	state.vertex_mode++;
	if (state.vertex_mode>vertex_mode_snap) state.vertex_mode=vertex_mode_none;
	
	main_wind_draw();
}

void tool_switch_drag_mode(void)
{
	state.drag_mode++;
	if (state.drag_mode>drag_mode_vertex) state.drag_mode=drag_mode_mesh;

	main_wind_draw();
}

void tool_switch_grid_mode(void)
{
	state.grid_mode++;
	if (state.grid_mode>grid_mode_large) state.grid_mode=grid_mode_none;
	
	main_wind_draw();
}

void tool_switch_node_mode(void)
{
	state.node_mode++;
	if (state.node_mode>node_mode_remove_link) state.node_mode=node_mode_select;
	
	main_wind_draw();
}



