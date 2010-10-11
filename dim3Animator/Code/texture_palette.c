/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Texture Palette

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3animator.h"
#endif

#include "window.h"
#include "dialog.h"
#include "model.h"

extern int					cur_mesh;
extern d3rect				model_box;

extern model_type			model;

int							txt_palette_max_page_count,txt_palette_per_page_count,
							txt_palette_cur_page,txt_palette_pixel_sz,
							txt_palette_page_list_count,txt_palette_page_list_width;
d3rect						txt_palette_box;

/* =======================================================

      Texture Palette Setup
      
======================================================= */

void texture_palette_setup(void)
{
	int				x_sz;
	d3rect			wbox;
	
	os_get_window_box(&wbox);
	
	x_sz=(wbox.rx-wbox.lx)-total_list_width;
	if (x_sz<model_view_min_size) x_sz=model_view_min_size;
	
	wbox.rx=wbox.lx+x_sz;
	
	txt_palette_max_page_count=max_model_texture;
	txt_palette_per_page_count=16;

	txt_palette_page_list_count=txt_palette_max_page_count/txt_palette_per_page_count;
	txt_palette_page_list_width=(txt_palette_page_list_count>>1)*16;
	
	txt_palette_pixel_sz=((wbox.rx-16)-(wbox.lx+txt_palette_page_list_width))/txt_palette_per_page_count;
	
	txt_palette_box.lx=wbox.lx;
	txt_palette_box.rx=wbox.rx;
	txt_palette_box.ty=wbox.by-txt_palette_pixel_sz;
	txt_palette_box.by=(txt_palette_box.ty+txt_palette_pixel_sz)+1;
}

/* =======================================================

      Texture Palette Drawing
      
======================================================= */

void texture_palette_draw(void)
{
	int					n,k,x,ty,by,idx;
	bool				has_texture;
	d3rect				wbox;
	texture_type		*texture;
	
		// viewport setup
		
	os_get_window_box(&wbox);

	glEnable(GL_SCISSOR_TEST);
	glScissor(txt_palette_box.lx,(wbox.by-txt_palette_box.by),(txt_palette_box.rx-txt_palette_box.lx),(txt_palette_box.by-txt_palette_box.ty));

	glViewport(txt_palette_box.lx,(wbox.by-txt_palette_box.by),(txt_palette_box.rx-txt_palette_box.lx),(txt_palette_box.by-txt_palette_box.ty));
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)txt_palette_box.lx,(GLdouble)txt_palette_box.rx,(GLdouble)txt_palette_box.by,(GLdouble)txt_palette_box.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		// draw setup

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_BLEND);
	
		// texture page switch
	
	x=0;
	ty=txt_palette_box.ty;
	by=ty+(txt_palette_pixel_sz>>1);

	for (n=0;n!=txt_palette_page_list_count;n++) {
	
			// time to move down?
			
		if (n==(txt_palette_page_list_count>>1)) {
			x=0;
			ty=by;
			by=txt_palette_box.ty+txt_palette_pixel_sz;
		}
		
			// draw page
			
		if (txt_palette_cur_page==n) {
			glColor4f(0.3f,0.3f,1.0f,1.0f);
		}
		else {
			glColor4f(0.75f,0.75f,0.75f,1.0f);
		}

		glBegin(GL_QUADS);
		glVertex2i(x,ty);
		glVertex2i((x+16),ty);
		glVertex2i((x+16),by);
		glVertex2i(x,by);
		glEnd();

			// outline

		glColor4f(0.0f,0.0f,0.0f,1.0f);
		
		glBegin(GL_LINE_LOOP);
		glVertex2i(x,ty);
		glVertex2i((x+16),ty);
		glVertex2i((x+16),by);
		glVertex2i(x,by);
		glEnd();
		
		
			// mark if there are textures
			
		has_texture=FALSE;
		idx=n*txt_palette_per_page_count;
		
		for (k=idx;k!=(idx+txt_palette_per_page_count);k++) {
			if (model.textures[k].frames[0].bitmap.gl_id!=-1) {
				has_texture=TRUE;
				break;
			}
		}
		
		if (has_texture) {
			glColor4f(0.0f,0.0f,0.0f,1.0f);
			
			glBegin(GL_QUADS);
			glVertex2i((x+11),(by-5));
			glVertex2i((x+14),(by-5));
			glVertex2i((x+14),(by-2));
			glVertex2i((x+11),(by-2));
			glEnd();
		}
		
		x+=16;
	}

		// textures
		
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
	
	x=txt_palette_page_list_width;
	ty=txt_palette_box.ty+1;
	by=(ty+txt_palette_pixel_sz)-1;
		
	for (n=0;n!=txt_palette_per_page_count;n++) {
		texture=&model.textures[n+(txt_palette_cur_page*txt_palette_per_page_count)];
		
			// the textures
			
		if (texture->frames[0].bitmap.gl_id!=-1) {
			glBindTexture(GL_TEXTURE_2D,texture->frames[0].bitmap.gl_id);

			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex2i(x,ty);
			glTexCoord2f(1,0);
			glVertex2i((x+txt_palette_pixel_sz),ty);
			glTexCoord2f(1,1);
			glVertex2i((x+txt_palette_pixel_sz),by);
			glTexCoord2f(0,1);
			glVertex2i(x,by);
			glEnd();
		}
		
		x+=txt_palette_pixel_sz;
	}
	
	glDisable(GL_TEXTURE_2D);
	
	glDisable(GL_ALPHA_TEST);
	
		// right fill
		
	glColor4f(0.75f,0.75f,0.75f,1.0f);
		
	glBegin(GL_QUADS);
	glVertex2i(x,ty);
	glVertex2i(txt_palette_box.rx,ty);
	glVertex2i(txt_palette_box.rx,by);
	glVertex2i(x,by);
	glEnd();
	
		// lines
		
	x=txt_palette_page_list_width;
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	for (n=0;n!=txt_palette_per_page_count;n++) {
		idx=n+(txt_palette_cur_page*txt_palette_per_page_count);
		
		glBegin(GL_LINE_LOOP);
		glVertex2i(x,ty);
		glVertex2i((x+txt_palette_pixel_sz),ty);
		glVertex2i((x+txt_palette_pixel_sz),by);
		glVertex2i(x,by);
		glEnd();
		
		x+=txt_palette_pixel_sz;
	}
	
	glBegin(GL_LINES);
	glVertex2i(txt_palette_box.lx,ty);
	glVertex2i(txt_palette_box.rx,ty);
	glEnd();
}

/* =======================================================

      Texture Palette Clicking
      
======================================================= */

void texture_palette_click(d3pnt *pnt,bool dblclick)
{
	int				nsel,page;
	
		// move within palette
		
	pnt->y-=txt_palette_box.ty;
	
		// texture page change
		
	if (pnt->x<txt_palette_page_list_width) {
		page=((pnt->y/(txt_palette_pixel_sz>>1))*(txt_palette_page_list_count>>1))+(pnt->x/16);
		if (txt_palette_cur_page!=page) {
			txt_palette_cur_page=page;
			main_wind_draw();
		}
		return;
	}
	
		// find clicked texture
	
	nsel=(pnt->x-txt_palette_page_list_width)/txt_palette_pixel_sz;
	nsel+=(txt_palette_cur_page*txt_palette_per_page_count);
		
		// double clicks
		
	if (dblclick) {
		dialog_texture_setting_run(nsel);
		main_wind_draw();
		return;
	}
    
		// single clicks
		
	if (model.textures[nsel].frames[0].bitmap.gl_id==-1) return;
	
	vertex_set_sel_mask_material(cur_mesh,nsel);
	hilite_vertex_rows();
	
    main_wind_draw();
}







/*

#include "window.h"
#include "dialog.h"
#include "model.h"

extern int					cur_mesh,cur_pose,
							gl_view_texture_palette_size;
extern bool					fileopen;
extern model_type			model;

extern AGLContext			ctx;
extern WindowRef			wind;


void texture_palette_draw(void)
{
	int						i,x,y;
	GLint					rect[4];
	Rect					wbox;
	texture_type			*texture;
	
 	if (!fileopen) return;
	
	return;

		// texture palette viewport
		
	GetWindowPortBounds(wind,&wbox);
	
	glViewport(0,0,(wbox.right-wbox.left),gl_view_texture_palette_size);
	
//	glScissor(0,0,(wbox.right-wbox.left),gl_view_texture_palette_size);
//	glEnable(GL_SCISSOR_TEST);





	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0,(GLdouble)(wbox.right-wbox.left),(GLdouble)gl_view_texture_palette_size,0.0,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glClearColor(0.8f,0.8f,0.8f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
		// draw textures
		
	glDisable(GL_SMOOTH);
	glDisable(GL_DITHER);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
		
	glActiveTexture(GL_TEXTURE0);

	x=0;
	y=0;

	texture=model.textures;

	for (i=0;i!=max_model_texture;i++) {
		
			// the textures
			
		if (texture->frames[0].bitmap.gl_id!=-1) {
			glColor4f(1.0f,1.0f,1.0f,1.0f);
			
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,texture->frames[0].bitmap.gl_id);

			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex2i(x,y);
			glTexCoord2f(1,0);
			glVertex2i((x+gl_view_texture_palette_size),y);
			glTexCoord2f(1,1);
			glVertex2i((x+gl_view_texture_palette_size),(y+gl_view_texture_palette_size));
			glTexCoord2f(0,1);
			glVertex2i(x,(y+gl_view_texture_palette_size));
			glEnd();
			
			glDisable(GL_TEXTURE_2D);
		}
		else {
			glColor4f(1.0f,1.0f,1.0f,1.0f);
			
			glBegin(GL_QUADS);
			glVertex2i(x,y);
			glVertex2i((x+gl_view_texture_palette_size),y);
			glVertex2i((x+gl_view_texture_palette_size),(y+gl_view_texture_palette_size));
			glVertex2i(x,(y+gl_view_texture_palette_size));
			glEnd();
		}

			// the frame
			
        glColor4f(0.0f,0.0f,0.0f,1.0f);
		
		glBegin(GL_LINE_LOOP);
		glVertex2i(x,(y+1));
		glVertex2i((x+gl_view_texture_palette_size),(y+1));
		glVertex2i((x+gl_view_texture_palette_size),((y+1)+gl_view_texture_palette_size));
		glVertex2i(x,((y+1)+gl_view_texture_palette_size));
		glEnd();
		
			// next position
			
		x+=gl_view_texture_palette_size;
	
		texture++;
	}
	
		// top and bottom borders
		
	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	glBegin(GL_LINES);
	glVertex2i(0,1);
	glVertex2i((wbox.right-wbox.left),1);
	glVertex2i(0,(gl_view_texture_palette_size+1));
	glVertex2i((wbox.right-wbox.left),(gl_view_texture_palette_size+1));
	glEnd();
}


void texture_palette_click(Point pt,bool dblclick)
{
	int					nsel;
	Rect				wbox;
	return;
	
    if (!fileopen) return;
 
		// drawing area
		
	GetWindowPortBounds(wind,&wbox);
	
//	wbox.top+=tool_height+gl_view_y_sz;
	wbox.bottom=wbox.top+gl_view_texture_palette_size;

		// find clicked texture
	
	nsel=(pt.h-wbox.left)/gl_view_texture_palette_size;
	if ((nsel<0) || (nsel>(max_model_texture-1))) return;
		
		// handle click
	
	if (dblclick) {
		dialog_texture_setting_run(nsel);
	}
	else {
		vertex_set_sel_mask_material(cur_mesh,nsel);
		hilite_vertex_rows();
	}
	
    main_wind_draw();
}
*/
