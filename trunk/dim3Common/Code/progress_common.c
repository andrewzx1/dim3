/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Progress Indicator

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
	#ifdef D3_EDITOR
		#include "dim3editor.h"
	#else
		#include "dim3Animator.h"
	#endif
#endif

#include "glue.h"
#include "interface.h"

int					progress_count,progress_pos;
char				progress_title[256];

/* =======================================================

      Progress Drawing
      
======================================================= */

void progress_draw(void)
{
	int					lx,rx,mx,ty,by;
	d3rect				wbox;

		// draw the main window

	main_wind_draw_no_swap();

		// draw the progress

	os_get_window_box(&wbox);

	glEnable(GL_SCISSOR_TEST);
	glScissor(wbox.lx,0,(wbox.rx-wbox.lx),(wbox.by-wbox.ty));

	glViewport(wbox.lx,0,(wbox.rx-wbox.lx),(wbox.by-wbox.ty));
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)wbox.lx,(GLdouble)wbox.rx,(GLdouble)wbox.by,(GLdouble)wbox.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		// draw setup

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		// background

	glColor4f(0.0f,0.0f,0.0f,0.5f);
		
	glBegin(GL_QUADS);
	glVertex2i(wbox.lx,wbox.ty);
	glVertex2i(wbox.rx,wbox.ty);
	glVertex2i(wbox.rx,wbox.by);
	glVertex2i(wbox.lx,wbox.by);
	glEnd();

		// progress box

	lx=wbox.lx+50;
	rx=wbox.rx-50;
	ty=((wbox.ty+wbox.by)>>1)-30;
	by=ty+60;

	glColor4f(1.0f,1.0f,1.0f,1.0f);
		
	glBegin(GL_QUADS);
	glVertex2i(lx,ty);
	glVertex2i(rx,ty);
	glVertex2i(rx,by);
	glVertex2i(lx,by);
	glEnd();

	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	glBegin(GL_LINE_LOOP);
	glVertex2i(lx,ty);
	glVertex2i(rx,ty);
	glVertex2i(rx,by);
	glVertex2i(lx,by);
	glEnd();

		// progress bar

	lx+=10;
	rx-=10;
	by-=10;
	ty=by-15;

	mx=lx+((rx-lx)*progress_pos)/progress_count;
		
	glBegin(GL_QUADS);
	glColor4f(0.5f,0.5f,0.5f,1.0f);
	glVertex2i(lx,ty);
	glVertex2i(rx,ty);
	glColor4f(0.7f,0.7f,0.7f,1.0f);
	glVertex2i(rx,by);
	glVertex2i(lx,by);
	glEnd();

	glBegin(GL_QUADS);
	glColor4f(0.0f,1.0f,0.0f,1.0f);
	glVertex2i(lx,ty);
	glVertex2i(mx,ty);
	glColor4f(0.0f,0.7f,0.0f,1.0f);
	glVertex2i(mx,by);
	glVertex2i(lx,by);
	glEnd();

	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	glBegin(GL_LINE_LOOP);
	glVertex2i(lx,ty);
	glVertex2i(rx,ty);
	glVertex2i(rx,by);
	glVertex2i(lx,by);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex2i(lx,ty);
	glVertex2i(mx,ty);
	glVertex2i(mx,by);
	glVertex2i(lx,by);
	glEnd();

		// title

	text_draw(((lx+rx)>>1),(ty-5),20.0f,TRUE,progress_title);

	os_swap_gl_buffer();
}

/* =======================================================

      Progress Running
      
======================================================= */

void progress_start(char *title,int count)
{
	progress_count=count;
	progress_pos=0;

	strcpy(progress_title,title);

	progress_draw();
}

void progress_end(void)
{
	main_wind_draw();
}

void progress_next(void)
{
	progress_pos++;
	if (progress_pos>progress_count) progress_pos=progress_count;

	progress_draw();
}

void progress_next_title(char *title)
{
	strcpy(progress_title,title);
	progress_next();
}
