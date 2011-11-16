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
#include "ui_common.h"

extern file_path_setup_type		file_path_setup;

int								tool_palette_push_idx;

bitmap_type						tool_bitmaps[tool_count];

char							tool_bitmaps_file_name[tool_count][64]=tool_file_names;
char							tool_bitmaps_separator[tool_count]=tool_separators;
char							tool_bitmaps_tip[tool_count][64]=tool_tip_names;

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
		bitmap_open(&tool_bitmaps[n],path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE,FALSE);
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
	int				pixel_sz;
	float			col;
	float			vertexes[8],colors[16],uvs[8]={0.0f,0.0f,1.0f,0.0f,1.0f,1.0f,0.0f,1.0f};

	pixel_sz=tool_palette_pixel_size();

		// background

	if (is_pushed) {
		col=0.60f;
	}
	else {
		if (!is_highlight) {
			col=0.95f;
		}
		else {
			col=0.85f;
		}
	}

	vertexes[0]=vertexes[6]=(float)x;
	vertexes[2]=vertexes[4]=(float)(x+pixel_sz);
	vertexes[1]=vertexes[3]=(float)y;
	vertexes[5]=vertexes[7]=(float)(y+pixel_sz);

	colors[0]=colors[1]=colors[2]=colors[4]=colors[5]=colors[6]=col;
	colors[8]=colors[9]=colors[10]=colors[12]=colors[13]=colors[14]=col-0.2f;
	colors[3]=colors[7]=colors[11]=colors[15]=1.0f;
			
	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4,GL_FLOAT,0,colors);

	glDrawArrays(GL_QUADS,0,4);

	glDisableClientState(GL_COLOR_ARRAY);

		// bitmap

	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D,gl_id);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,uvs);

	glDrawArrays(GL_QUADS,0,4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_TEXTURE_2D);

		// outline

	vertexes[0]=vertexes[6]=(float)x;
	vertexes[2]=vertexes[4]=(float)(x+pixel_sz);
	vertexes[1]=vertexes[3]=(float)y;
	vertexes[5]=vertexes[7]=(float)(y+(pixel_sz+1));
			
	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(0.4f,0.4f,0.4f,1.0f);
	glDrawArrays(GL_LINE_LOOP,0,4);
}

void tool_palette_draw(void)
{
	int				n,x,pixel_sz;
	float			vertexes[8];
	d3rect			wbox,tbox;
	
	tool_palette_box(&tbox);

		// viewport setup
		
	os_get_window_box(&wbox);

	glEnable(GL_SCISSOR_TEST);
	glScissor(tbox.lx,(wbox.by-tbox.by),(tbox.rx-tbox.lx),(tbox.by-tbox.ty));

	glViewport(tbox.lx,(wbox.by-tbox.by),(tbox.rx-tbox.lx),(tbox.by-tbox.ty));
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)tbox.lx,(GLdouble)tbox.rx,(GLdouble)tbox.by,(GLdouble)tbox.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		// draw setup

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

		// background

	vertexes[0]=vertexes[6]=(float)tbox.lx;
	vertexes[2]=vertexes[4]=(float)tbox.rx;
	vertexes[1]=vertexes[3]=(float)tbox.ty;
	vertexes[5]=vertexes[7]=(float)tbox.by;
			
	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(0.9f,0.9f,0.9f,1.0f);
	glDrawArrays(GL_QUADS,0,4);

		// tools

	x=tbox.lx;
	pixel_sz=tool_palette_pixel_size();

	for (n=0;n!=tool_count;n++) {
	
			// splitter?
			
		if (tool_bitmaps_file_name[n][0]==0x0) {
			x=tbox.rx-(((tool_count-1)-n)*pixel_sz);
			continue;
		}
		
			// separator
			
		if (tool_bitmaps_separator[n]=='1') x+=tool_palette_seperator_size;
		
			// draw tool
			
		tool_palette_draw_icon(x,tbox.ty,tool_bitmaps[n].gl_id,tool_get_highlight_state(n),(tool_palette_push_idx==n));
		x+=pixel_sz;
	}

	glDisable(GL_ALPHA_TEST);
	
		// border

	vertexes[0]=vertexes[4]=(float)tbox.lx;
	vertexes[2]=vertexes[6]=(float)tbox.rx;
#ifdef D3_OS_WINDOWS
	vertexes[1]=vertexes[3]=(float)(tbox.ty+1);		// win32 has white line under menu
#else
	vertexes[1]=vertexes[3]=(float)tbox.ty;
#endif
	vertexes[5]=vertexes[7]=(float)tbox.by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_LINES,0,4);
}

/* =======================================================

      Tool Palette Clicking
      
======================================================= */

bool tool_palette_click_mouse_down(int push_idx,int lx,int ty)
{
	int					pixel_sz;
	d3pnt				pt;
	
	tool_palette_push_idx=push_idx;
	pixel_sz=tool_palette_pixel_size();
	
	main_wind_draw();

	while (!os_track_mouse_location(&pt,NULL)) {
		if ((pt.x<lx) || (pt.x>=(lx+pixel_sz)) || (pt.y<ty) || (pt.y>=(ty+pixel_sz))) {
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

int tool_palette_click_find_index(d3pnt *pnt,int *px)
{
	int				n,x,pixel_sz;
	d3rect			tbox;
	
	tool_palette_box(&tbox);

	x=tbox.lx;
	pixel_sz=tool_palette_pixel_size();

	for (n=0;n!=tool_count;n++) {
	
			// splitter?
			
		if (tool_bitmaps_file_name[n][0]==0x0) {
			x=tbox.rx-(((tool_count-1)-n)*pixel_sz);
			continue;
		}
		
			// separator
			
		if (tool_bitmaps_separator[n]=='1') x+=tool_palette_seperator_size;
		
			// check click
			
		if ((pnt->x>=x) && (pnt->x<(x+pixel_sz))) {
			*px=x;
			return(n);
		}
		
		x+=pixel_sz;
	}

	return(-1);
}

void tool_palette_click(d3pnt *pnt)
{
	int				idx,x;
	d3rect			tbox;

	idx=tool_palette_click_find_index(pnt,&x);
	if (idx==-1) return;
	
	tool_palette_box(&tbox);
	
	if (tool_palette_click_mouse_down(idx,x,tbox.ty)) tool_click(idx);
}

/* =======================================================

      Tool Palette Mouse Over
      
======================================================= */

void tool_palette_mouse_move(d3pnt *pnt)
{
	int				idx,x,mx;
	d3pnt			tpnt;
	d3rect			tbox;
	
	tool_palette_box(&tbox);

	idx=-1;

	if ((pnt->y>=tbox.ty) && (pnt->y<=tbox.by)) {
		idx=tool_palette_click_find_index(pnt,&x);
	}

		// setup tool tip
		
	if (idx==-1) {
		tool_tip_clear();
	}
	else {
		tpnt.x=pnt->x;
		tpnt.y=tbox.by+15;
		
		mx=(tbox.lx+tbox.rx)>>1;
		tool_tip_setup(&tpnt,tool_bitmaps_tip[idx],(x>mx));
	}
}

