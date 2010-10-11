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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3animator.h"
#endif

extern model_type				model;
extern display_type				display;
extern file_path_setup_type		file_path_setup;

int								tool_pixel_sz,tool_palette_push_idx;
d3rect							tool_palette_box;

bitmap_type						tool_bitmaps[tool_count];

char							tool_bitmaps_file_name[tool_count][64]=
									{
										"Tool Textured",
										"Tool Mesh",
										"Tool Bones",
										"Tool Hit Boxes",
										"Tool Edit Polygons",
										"Tool Edit Vertexes",
										"Tool Boxes",
										"Tool Normals",
										"Tool Show First Mesh",
										"Tool Rotate Mode",
										"Tool Move Mode",
										"Tool Play"
									};

char							tool_bitmaps_separator[tool_count]={"000010100101"};

/* =======================================================

      Tool Palette Initialize and Shutdown
      
======================================================= */

void tool_palette_initialize(void)
{
	int				n;
	char			sub_path[1024],path[1024];

	os_get_icon_file_path(sub_path);
	strcat(sub_path,"/Tools");
		
	for (n=0;n!=tool_count;n++) {
		if (tool_bitmaps_file_name[n][0]==0x0) continue;
		
		file_paths_app(&file_path_setup,path,sub_path,tool_bitmaps_file_name[n],"png");
		bitmap_open(&tool_bitmaps[n],path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);
	}
}

void tool_palette_shutdown(void)
{
	int				n;

	for (n=0;n!=tool_count;n++) {
		if (tool_bitmaps_file_name[n][0]!=0x0) bitmap_close(&tool_bitmaps[n]);
	}
}

/* =======================================================

      Tool Palette Setup
      
======================================================= */

void tool_palette_setup(void)
{
	d3rect			wbox;
	
	os_get_window_box(&wbox);

	tool_pixel_sz=(wbox.rx-wbox.lx)/tool_count;
	if (tool_pixel_sz>tool_button_size) tool_pixel_sz=tool_button_size;
	
	tool_palette_box.lx=wbox.lx;
	tool_palette_box.rx=wbox.rx;
	tool_palette_box.ty=wbox.ty;
	tool_palette_box.by=wbox.ty+(tool_pixel_sz+1);

	tool_palette_push_idx=-1;
}

/* =======================================================

      Tool Palette State
      
======================================================= */

bool tool_get_highlight_state(int tool_idx)
{
	switch (tool_idx) {

		case 0:
			return(display.texture);

		case 1:
			return(display.mesh);

		case 2:
			return(display.bone);

		case 3:
			return(display.hit_box);

		case 4:
			return(display.select_mode==select_mode_polygon);

		case 5:
			return(display.select_mode==select_mode_vertex);

		case 6:
			return(display.view_box);

		case 7:
			return(display.normal);

		case 8:
			return(display.first_mesh);

		case 9:
			return(display.drag_bone_mode==drag_bone_mode_rotate);

		case 10:
			return(display.drag_bone_mode==drag_bone_mode_stretch);

		case 11:
			return(display.playing);
			
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
	glVertex2i((x+tool_pixel_sz),(y+(tool_pixel_sz+1)));
	glVertex2i(x,(y+(tool_pixel_sz+1)));
	glEnd();
}

void tool_palette_draw(void)
{
	int				n,x;
	d3rect			wbox;

		// viewport setup
		
	os_get_window_box(&wbox);

	glEnable(GL_SCISSOR_TEST);
	glScissor(tool_palette_box.lx,(wbox.by-tool_palette_box.by),(tool_palette_box.rx-tool_palette_box.lx),(tool_palette_box.by-tool_palette_box.ty));

	glViewport(tool_palette_box.lx,(wbox.by-tool_palette_box.by),(tool_palette_box.rx-tool_palette_box.lx),(tool_palette_box.by-tool_palette_box.ty));
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)tool_palette_box.lx,(GLdouble)tool_palette_box.rx,(GLdouble)tool_palette_box.by,(GLdouble)tool_palette_box.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

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
	
			// splitter?
			
		if (tool_bitmaps_file_name[n][0]==0x0) {
			x=tool_palette_box.rx-(((tool_count-1)-n)*tool_pixel_sz);
			continue;
		}
		
			// separator
			
		if (tool_bitmaps_separator[n]=='1') x+=2;
		
			// draw tool
			
		tool_palette_draw_icon(x,tool_palette_box.ty,tool_bitmaps[n].gl_id,tool_get_highlight_state(n),(tool_palette_push_idx==n));
		x+=tool_pixel_sz;
	}
	
		// border
		
	glColor4f(0.0f,0.0f,0.0f,1.0f);

	glBegin(GL_LINES);
	glVertex2i(tool_palette_box.lx,tool_palette_box.by);
	glVertex2i(tool_palette_box.rx,tool_palette_box.by);
	glEnd();

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
}

/* =======================================================

      Tool Palette Button Clicks
      
======================================================= */

void tool_click(int tool_idx)
{
	switch (tool_idx) {
	
		case 0:
			display.texture=!display.texture;
			break;
			
		case 1:
			display.mesh=!display.mesh;
			break;
			
		case 2:
			display.bone=!display.bone;
			break;
			
		case 3:
			display.hit_box=!display.hit_box;
			break;
			
		case 4:
			display.select_mode=select_mode_polygon;
			break;
			
		case 5:
			display.select_mode=select_mode_vertex;
			break;
			
		case 6:
			display.view_box=!display.view_box;
			break;
			
		case 7:
			display.normal=!display.normal;
			break;
			
		case 8:
			display.first_mesh=!display.first_mesh;
			break;
			
		case 9:
			display.drag_bone_mode=drag_bone_mode_rotate;
			break;
			
		case 10:
			display.drag_bone_mode=drag_bone_mode_stretch;
			break;
			
		case 11:
			model_wind_play(!display.playing,FALSE);
			break;
	}
	
	main_wind_draw();
}

/* =======================================================

      Tool Palette Clicking
      
======================================================= */

bool tool_palette_click_mouse_down(int push_idx,int lx,int ty)
{
	d3pnt				pt;
	
	tool_palette_push_idx=push_idx;
	
	tool_palette_draw();
	os_swap_gl_buffer();

	while (!os_track_mouse_location(&pt,NULL)) {
		if ((pt.x<lx) || (pt.x>=(lx+tool_pixel_sz)) || (pt.y<ty) || (pt.y>=(ty+tool_pixel_sz))) {
			tool_palette_push_idx=-1;
		}
		else {
			tool_palette_push_idx=push_idx;
		}

		tool_palette_draw();
		os_swap_gl_buffer();
		
		usleep(10000);
	}

	if (tool_palette_push_idx==-1) return(FALSE);

	tool_palette_push_idx=-1;
	tool_palette_draw();
	os_swap_gl_buffer();

	return(TRUE);
}

void tool_palette_click(d3pnt *pnt)
{
	int				n,x;

	x=tool_palette_box.lx;

	for (n=0;n!=tool_count;n++) {
	
			// splitter?
			
		if (tool_bitmaps_file_name[n][0]==0x0) {
			x=tool_palette_box.rx-(((tool_count-1)-n)*tool_pixel_sz);
			continue;
		}
		
			// separator
			
		if (tool_bitmaps_separator[n]=='1') x+=2;
		
			// check click
			
		if ((pnt->x>=x) && (pnt->x<(x+tool_pixel_sz))) {
			if (tool_palette_click_mouse_down(n,x,tool_palette_box.ty)) tool_click(n);
			return;
		}
		
		x+=tool_pixel_sz;
	}
}

/* =======================================================

      Values and States
      
======================================================= */

void tool_default(void)
{
/*
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
	*/
}
