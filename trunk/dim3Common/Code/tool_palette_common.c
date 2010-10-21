/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Tool Palette

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
	#ifdef D3_EDITOR
		#include "dim3editor.h"
	#else
		#include "dim3Animator.h"
	#endif
#endif

#include "glue.h"
#include "interface.h"

extern file_path_setup_type		file_path_setup;

int								tool_pixel_sz,tool_palette_push_idx;
d3rect							tool_palette_box;

bitmap_type						tool_bitmaps[tool_count];

char							tool_bitmaps_file_name[tool_count][64]=tool_file_names;
char							tool_bitmaps_separator[tool_count]=tool_separators;

extern bool tool_get_highlight_state(int tool_idx);
extern void tool_click(int tool_idx);

/* =======================================================

      Tool Palette Initialize and Shutdown
      
======================================================= */

void tool_palette_initialize(char *app_name)
{
	int				n;
	char			sub_path[1024],path[1024];

		// load icons
		
	os_get_support_file_path(sub_path,app_name);
	strcat(sub_path,"/Tools");
		
	for (n=0;n!=tool_count;n++) {
		if (tool_bitmaps_file_name[n][0]==0x0) continue;
		
		file_paths_app(&file_path_setup,path,sub_path,tool_bitmaps_file_name[n],"png");
		bitmap_open(&tool_bitmaps[n],path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);
	}

		// currently no pressed icon
		
	tool_palette_push_idx=-1;
}

void tool_palette_shutdown(void)
{
	int				n;

	for (n=0;n!=tool_count;n++) {
		if (tool_bitmaps_file_name[n][0]!=0x0) bitmap_close(&tool_bitmaps[n]);
	}
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

      Tool Palette Clicking
      
======================================================= */

bool tool_palette_click_mouse_down(int push_idx,int lx,int ty)
{
	d3pnt				pt;
	
	tool_palette_push_idx=push_idx;
	
	main_wind_draw();

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

