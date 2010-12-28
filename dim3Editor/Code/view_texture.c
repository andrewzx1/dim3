/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Texture Edit View

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

#define view_texture_item_high		140

extern int						tool_palette_pixel_sz,txt_palette_pixel_sz;
extern list_palette_type		item_palette,property_palette;

extern map_type					map;
extern editor_setup_type		setup;
extern editor_state_type		state;

/* =======================================================

      Switching View
      
======================================================= */

void view_texture_switch(int texture_idx)
{
		// clicking again leaves

	if ((state.view_texture_idx!=-1) && (state.view_texture_idx==texture_idx)) {
		state.view_texture_idx=-1;
	}
	else {
		state.view_texture_idx=texture_idx;
	}

	main_wind_draw();
}

/* =======================================================

      View Texture Edit Draw
      
======================================================= */

void view_texture_get_box(d3rect *box)
{
	os_get_window_box(box);
	
	box->ty+=tool_palette_pixel_sz;
	box->by-=txt_palette_pixel_sz;
	box->rx-=item_palette.pixel_sz;
}

void view_texture_draw_bitmap(d3rect *box,char *name,unsigned long gl_id)
{
		// the bitmap

	if (gl_id!=-1) {

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,gl_id);

		glColor4f(1.0f,1.0f,1.0f,1.0f);

  		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f);
		glVertex2i(box->lx,box->ty);
		glTexCoord2f(1.0f,0.0f);
		glVertex2i(box->rx,box->ty);
		glTexCoord2f(1.0f,1.0f);
		glVertex2i(box->rx,box->by);
		glTexCoord2f(0.0f,1.0f);
		glVertex2i(box->lx,box->by);
		glEnd();

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

		glDisable(GL_TEXTURE_2D);
	}

		// the box

	glColor4f(0.0f,0.0f,0.0f,1.0f);

	glBegin(GL_LINE_LOOP);
	glVertex2i(box->lx,box->ty);
	glVertex2i(box->rx,box->ty);
	glVertex2i(box->rx,box->by);
	glVertex2i(box->lx,box->by);
	glEnd();

		// the name
		
	text_draw_center(((box->lx+box->rx)>>1),(box->by+15),12.0f,NULL,name);
}

void view_texture_draw(void)
{
	int					n,ty,by,wid,high;
	char				str[256];
	char				type_str[3][32]={"Opaque","Cut-Out","Transparent"};
	d3rect				wbox,box,tbox;
	texture_type		*texture;

	texture=&map.textures[state.view_texture_idx];

		// setup drawing

	os_get_window_box(&wbox);
	view_texture_get_box(&box);

	wid=box.rx-box.lx;
	high=box.by-box.ty;

	glEnable(GL_SCISSOR_TEST);
	glScissor(box.lx,(wbox.by-box.by),(box.rx-box.lx),(box.by-box.ty));

	glViewport(box.lx,(wbox.by-box.by),(box.rx-box.lx),(box.by-box.ty));
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)0,(GLdouble)wid,(GLdouble)high,(GLdouble)0,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		// draw setup

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

		// the textures

	for (n=0;n!=max_texture_frame;n++) {
		ty=view_texture_item_high*n;
		by=ty+view_texture_item_high;

			// the separator

		glColor4f(0.0f,0.0f,0.0f,1.0f);

		glBegin(GL_LINES);
		glVertex2i(0,ty);
		glVertex2i(wid,ty);
		glEnd();

			// the frame

		sprintf(str,"Frame %d",n);
		text_draw(5,(ty+20),15.0f,NULL,str);

			// the textures

		tbox.lx=25;
		tbox.rx=tbox.lx+100;
		tbox.ty=ty+22;
		tbox.by=tbox.ty+100;
		view_texture_draw_bitmap(&tbox,"Bitmap",texture->frames[n].bitmap.gl_id);

		tbox.lx+=110;
		tbox.rx+=110;
		view_texture_draw_bitmap(&tbox,"Bump",texture->frames[n].bumpmap.gl_id);

		tbox.lx+=110;
		tbox.rx+=110;
		view_texture_draw_bitmap(&tbox,"Specular",texture->frames[n].specularmap.gl_id);

		tbox.lx+=110;
		tbox.rx+=110;
		view_texture_draw_bitmap(&tbox,"Glow",texture->frames[n].glowmap.gl_id);

			// the info

		str[0]=0x0;
	
		if (texture->frames[n].name[0]!=0x0) {
			sprintf(str,"Name: %s",texture->frames[n].name);
			text_draw(470,(ty+35),15.0f,NULL,str);

			sprintf(str,"Size: %dx%d",texture->frames[n].bitmap.wid,texture->frames[n].bitmap.high);
			text_draw(470,(ty+55),15.0f,NULL,str);

			sprintf(str,"Alpha: %s",type_str[texture->frames[n].bitmap.alpha_mode]);
			text_draw(470,(ty+75),15.0f,NULL,str);

			sprintf(str,"Wait: %d",texture->animate.wait[n]);
			text_draw(470,(ty+95),15.0f,NULL,str);
		}
		else {
			text_draw(470,(ty+35),15.0f,NULL,"Name: (empty)");
		}
	}

		// the return click

	tbox.lx=wid-120;
	tbox.rx=tbox.lx+120;
	tbox.ty=0;
	tbox.by=40;

	glBegin(GL_QUADS);
	glColor4f(0.9f,0.9f,0.9f,1.0f);
	glVertex2i(tbox.lx,tbox.ty);
	glVertex2i(tbox.rx,tbox.ty);
	glColor4f(0.7f,0.7f,0.7f,1.0f);
	glVertex2i(tbox.rx,tbox.by);
	glVertex2i(tbox.lx,tbox.by);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor4f(0.2f,0.2f,1.0f,1.0f);
	glVertex2i((tbox.rx-20),(tbox.ty+5));
	glColor4f(0.0f,0.0f,0.8f,1.0f);
	glVertex2i((tbox.rx-5),(tbox.by-5));
	glVertex2i((tbox.rx-35),(tbox.by-5));
	glEnd();

	glColor4f(0.0f,0.0f,0.0f,1.0f);

	glBegin(GL_LINE_LOOP);
	glVertex2i((tbox.rx-20),(tbox.ty+5));
	glVertex2i((tbox.rx-5),(tbox.by-5));
	glVertex2i((tbox.rx-35),(tbox.by-5));
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex2i(tbox.lx,tbox.ty);
	glVertex2i(tbox.rx,tbox.ty);
	glVertex2i(tbox.rx,tbox.by);
	glVertex2i(tbox.lx,tbox.by);
	glEnd();

	text_draw((tbox.lx+5),(tbox.by-5),25.0f,NULL,"Return");
}

/* =======================================================

      View Texture Edit Click
      
======================================================= */

/*
bool texture_setting_bitmap_open(char *bitmap_name)
{
    char				err_str[256],path[1024];
	
		// get bitmap
				
	if (!dialog_file_open_run("Open a Bitmap","Bitmaps/Textures","png",NULL,bitmap_name)) return(FALSE);
	
		// check bitmap
		
	file_paths_data(&file_path_setup,path,"Bitmaps/Textures",bitmap_name,"png");
	if (!bitmap_check(path,err_str)) {
		os_dialog_alert("Texture Error",err_str);
		return(FALSE);
	}
	
	return(TRUE);
}
*/

bool view_texture_click(d3pnt *pnt,bool double_click)
{
	int				wid,high;
	d3rect			box,tbox;

	view_texture_get_box(&box);

	wid=box.rx-box.lx;
	high=box.by-box.ty;

		// move within view
		
	pnt->x-=box.lx;
	pnt->y-=box.ty;

		// clicking in return?

	tbox.lx=wid-120;
	tbox.rx=tbox.lx+120;
	tbox.ty=0;
	tbox.by=40;

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<=tbox.by)) {
		view_texture_switch(-1);
		return(TRUE);
	}

	return(TRUE);
}

