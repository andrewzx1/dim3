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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "walk_view.h"
#include "interface.h"
#include "dialog.h"
#include "common_view.h"

extern map_type					map;
extern editor_state_type		state;
extern file_path_setup_type		file_path_setup;

int								tool_pixel_sz,tool_palette_push_idx;
d3rect							tool_palette_box;

bitmap_type						tool_bitmaps[tool_count],
								tool_grid_bitmaps[3],
								piece_bitmaps[piece_count];

char							tool_bitmaps_file_name[tool_count][64]=
									{
										"Tool Move Points",
										"Tool Move Points Together",
										"Tool Snap Points",
										"Tool Free Look",
										"Tool Toggle Mode",
										"Tool Edit Mesh",
										"Tool Edit Polygons",
										"Tool Edit Vertexes",
										"Tool Combine Meshes",
										"Tool Split Mesh",
										"Tool Tesselate Mesh",
										"Tool No Grid",
										"Tool Auto-Texture Mesh",
										"Tool Rotate Mode",
										"Tool Move Mode",
										"Tool Node Select",
										"Tool Node Link",
										"Tool Node Remove Link",
										"Tool Normals",
										"Tool Cull",
										"Tool Edit Map Script",
										"Tool Run Map"
									};

char							tool_grid_bitmaps_file_name[3][64]=
									{
										"Tool Small Grid",
										"Tool Medium Grid",
										"Tool Large Grid"
									};

char							piece_bitmaps_file_name[piece_count][64]=
									{
										"Piece Spot",
										"Piece Light",
										"Piece Sound",
										"Piece Particle",
										"Piece Scenery",
										"Piece Node",
										"Piece Mesh",
										"Piece Mesh UV",
										"Piece Height Map",
										"Piece Grid",
										"Piece Polygon",
										"Piece Liquid",
									};

/* =======================================================

      Tool Palette Initialize and Shutdown
      
======================================================= */

void tool_palette_initialize(void)
{
	int				n;
	char			sub_path[1024],path[1024];

		// tool icons

	os_get_icon_file_path(sub_path);
	strcat(sub_path,"/Tools");
		
	for (n=0;n!=tool_count;n++) {
		file_paths_app(&file_path_setup,path,sub_path,tool_bitmaps_file_name[n],"png");
		bitmap_open(&tool_bitmaps[n],path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);
	}

	for (n=0;n!=3;n++) {
		file_paths_app(&file_path_setup,path,sub_path,tool_grid_bitmaps_file_name[n],"png");
		bitmap_open(&tool_grid_bitmaps[n],path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);
	}

		// piece icons

	os_get_icon_file_path(sub_path);
	strcat(sub_path,"/Pieces");
		
	for (n=0;n!=piece_count;n++) {
		file_paths_app(&file_path_setup,path,sub_path,piece_bitmaps_file_name[n],"png");
		bitmap_open(&piece_bitmaps[n],path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);
	}
}

void tool_palette_shutdown(void)
{
	int				n;

		// tool icons

	for (n=0;n!=tool_count;n++) {
		bitmap_close(&tool_bitmaps[n]);
	}

	for (n=0;n!=3;n++) {
		bitmap_close(&tool_grid_bitmaps[n]);
	}

		// piece icons

	for (n=0;n!=piece_count;n++) {
		bitmap_close(&piece_bitmaps[n]);
	}
}

/* =======================================================

      Tool Palette Setup
      
======================================================= */

void tool_palette_setup(void)
{
	d3rect			wbox;
	
	os_get_window_box(&wbox);

	tool_pixel_sz=(wbox.rx-wbox.lx)/(tool_count+piece_count);
	if (tool_pixel_sz>tool_button_size) tool_pixel_sz=tool_button_size;
	
	tool_palette_box.lx=wbox.lx;
	tool_palette_box.rx=wbox.rx;
	tool_palette_box.ty=0;
	tool_palette_box.by=tool_pixel_sz+1;

	tool_palette_push_idx=-1;
}

/* =======================================================

      Tool Palette State
      
======================================================= */

unsigned long tool_palette_draw_get_tool_gl_id(int tool_idx)
{
		// only grid mode is switchable

	if (tool_idx!=11) return(tool_bitmaps[tool_idx].gl_id);

		// grid mode

	if (state.grid_mode==0) return(tool_bitmaps[tool_idx].gl_id);
	return(tool_grid_bitmaps[state.grid_mode-1].gl_id);
}

bool tool_palette_draw_is_tool_hilight(int tool_idx)
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

			// auto texture

		case 12:
			return(state.auto_texture);

			// handle mode

		case 13:
			return(state.handle_mode==handle_mode_rotate);
		case 14:
			return(state.handle_mode==handle_mode_move);

			// node mode

		case 15:
			return(state.node_mode==node_mode_select);
		case 16:
			return(state.node_mode==node_mode_link);
		case 17:
			return(state.node_mode==node_mode_remove_link);
			
			// normals and culling

		case 18:
			view=walk_view_get_current_view();
			return(state.show_normals);
		case 19:
			view=walk_view_get_current_view();
			return(view->cull);
	}

	return(FALSE);
}

/* =======================================================

      Tool Palette Drawing
      
======================================================= */

void tool_palette_draw_icon(int x,int y,unsigned long gl_id,bool is_highlight,bool is_pushed)
{
	float			top_col,bot_col;

		// background

	if (is_pushed) {
		top_col=0.60f;
		bot_col=0.85f;
	}
	else {
		if (!is_highlight) {
			top_col=0.95f;
			bot_col=0.85f;
		}
		else {
			top_col=0.85f;
			bot_col=0.60f;
		}
	}

	glBegin(GL_QUADS);
	glColor4f(top_col,top_col,top_col,1.0f);
	glVertex2i(x,y);
	glVertex2i((x+tool_pixel_sz),y);
	glColor4f(bot_col,bot_col,bot_col,1.0f);
	glVertex2i((x+tool_pixel_sz),(y+tool_pixel_sz));
	glVertex2i(x,(y+tool_pixel_sz));
	glEnd();

		// bitmap

	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,gl_id);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f);
	glVertex2i(x,y);
	glTexCoord2f(1.0f,0.0f);
	glVertex2i((x+tool_pixel_sz),y);
	glTexCoord2f(1.0f,1.0f);
	glVertex2i((x+tool_pixel_sz),(y+tool_pixel_sz));
	glTexCoord2f(0.0f,1.0f);
	glVertex2i(x,(y+tool_pixel_sz));
	glEnd();

	glDisable(GL_TEXTURE_2D);

		// outline

	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	glBegin(GL_LINE_LOOP);
	glVertex2i(x,y);
	glVertex2i((x+tool_pixel_sz),y);
	glVertex2i((x+tool_pixel_sz),(y+tool_pixel_sz));
	glVertex2i(x,(y+tool_pixel_sz));
	glEnd();
}

void tool_palette_draw(void)
{
	int				n,x;

		// viewport setup
		
	walk_view_set_viewport_box(&tool_palette_box,TRUE,FALSE);

		// draw setup

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		// background

	glColor4f(0.75f,0.75f,0.75f,1.0f);
		
	glBegin(GL_QUADS);
	glVertex2i(tool_palette_box.lx,tool_palette_box.ty);
	glVertex2i(tool_palette_box.rx,tool_palette_box.ty);
	glVertex2i(tool_palette_box.rx,tool_palette_box.by);
	glVertex2i(tool_palette_box.lx,tool_palette_box.by);
	glEnd();

		// tools

	x=tool_palette_box.lx;

	for (n=0;n!=tool_count;n++) {
		tool_palette_draw_icon(x,tool_palette_box.ty,tool_palette_draw_get_tool_gl_id(n),tool_palette_draw_is_tool_hilight(n),(tool_palette_push_idx==n));
		x+=tool_pixel_sz;
	}

		// piece

	x=tool_palette_box.rx-(tool_pixel_sz*piece_count);

	for (n=0;n!=piece_count;n++) {
		tool_palette_draw_icon(x,tool_palette_box.ty,piece_bitmaps[n].gl_id,FALSE,(tool_palette_push_idx==(n+tool_count)));
		x+=tool_pixel_sz;
	}

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
}

/* =======================================================

      Tool Palette Button Clicks
      
======================================================= */

void tool_palette_click_tool(int tool_idx)
{
	editor_view_type		*view;
	
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
			palette_reset();
			break;
			
		case 6:
			state.drag_mode=drag_mode_polygon;
			palette_reset();
			break;
			
		case 7:
			state.drag_mode=drag_mode_vertex;
			palette_reset();
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
			
			
			// grid and auto-texture
			
		case 11:
			state.grid_mode++;
			if (state.grid_mode==4) state.grid_mode=0;
			break;
			
		case 12:
			state.auto_texture=!state.auto_texture;
			break;
			
			// handle modes
			
		case 13:
			state.handle_mode=handle_mode_rotate;
			break;
			
		case 14:
			state.handle_mode=handle_mode_move;
			break;
			
			// node editing
			
		case 15:
			state.show_node=TRUE;
			menu_update_view();
			state.node_mode=node_mode_select;
			break;
			
		case 16:
			state.show_node=TRUE;
			menu_update_view();
			state.node_mode=node_mode_link;
			break;
			
		case 17:
			state.show_node=TRUE;
			menu_update_view();
			state.node_mode=node_mode_remove_link;
			break;
			
			// normals
			
		case 18:
			state.show_normals=!state.show_normals;
			break;
			
		case 19:
			view=walk_view_get_current_view();
			walk_view_cull(!view->cull);
			break;
			
			// script and run buttons
			
		case 20:
			launch_map_script_editor();
			break;
			
		case 21:
			launch_engine();
			break;
			
	}
	
	menu_fix_enable();
	main_wind_draw();
}

void tool_palette_click_piece(int piece_idx)
{
	switch (piece_idx) {

		case 0:
			piece_create_spot();
			break;
			
		case 1:
			piece_create_light();
			break;
			
		case 2:
			piece_create_sound();
			break;
			
		case 3:
			piece_create_particle();
			break;
			
		case 4:
			piece_create_scenery();
			break;
			
		case 5:
			piece_create_node();
			break;
			
		case 6:
			piece_add_obj_mesh();
			break;
			
		case 7:
			piece_add_obj_mesh_uv();
			break;
			
		case 8:
			piece_add_height_map_mesh();
			break;
			
		case 9:
			piece_add_grid_mesh();
			break;
			
		case 10:
			piece_add_polygon_mesh();
			break;
			
		case 11:
			piece_create_liquid();
			break;
			
	}
	
	palette_reset();
}

/* =======================================================

      Tool Palette Clicking
      
======================================================= */

bool tool_palette_click_mouse_down(int push_idx,int lx,int ty)
{
	d3pnt				pt;

	while (!os_track_mouse_location(&pt,NULL)) {
		if ((pt.x<lx) || (pt.x>=(lx+tool_pixel_sz)) || (pt.y<ty) || (pt.y>=(ty+tool_pixel_sz))) {
			tool_palette_push_idx=-1;
		}
		else {
			tool_palette_push_idx=push_idx;
		}

		main_wind_draw();
		usleep(10000);
	}

	if (tool_palette_push_idx==-1) return(FALSE);

	tool_palette_push_idx=-1;
	main_wind_draw();

	return(TRUE);
}

void tool_palette_click(d3pnt *pnt)
{
	int				lx,rx,idx;

		// tools

	lx=tool_palette_box.lx;
	rx=lx+(tool_count*tool_pixel_sz);

	if ((pnt->x>=lx) && (pnt->x<rx)) {
		idx=(pnt->x-lx)/tool_pixel_sz;
		if (!tool_palette_click_mouse_down(idx,(lx+(idx*tool_pixel_sz)),tool_palette_box.ty)) return;

		tool_palette_click_tool(idx);
		main_wind_draw();
		return;
	}

		// pieces

	lx=tool_palette_box.rx-(piece_count*tool_pixel_sz);
	rx=tool_palette_box.rx;

	if ((pnt->x>=lx) && (pnt->x<rx)) {
		idx=(pnt->x-lx)/tool_pixel_sz;
		if (!tool_palette_click_mouse_down((tool_count+idx),(lx+(idx*tool_pixel_sz)),tool_palette_box.ty)) return;

		tool_palette_click_piece(idx);
		main_wind_draw();
		return;
	}
}

