/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Texture Palette

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

int							txt_palette_max_page_count,txt_palette_per_page_count,
							txt_palette_cur_page,txt_palette_pixel_sz,
							txt_palette_page_list_count,txt_palette_page_list_width;
d3rect						txt_palette_box;

extern int texture_palette_get_selected_texture(void);
extern void texture_palette_put_selected_texture(int txt_idx);
extern void dialog_texture_setting_run(int txt);

/* =======================================================

      Texture Palette Drawing
      
======================================================= */

void texture_palette_draw(texture_type *txt_list)
{
	int					n,k,x,ty,by,sel,idx;
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
			if (txt_list[k].frames[0].bitmap.gl_id!=-1) {
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
		texture=&txt_list[n+(txt_palette_cur_page*txt_palette_per_page_count)];
		
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
	
		// selection
		
	sel=texture_palette_get_selected_texture();
	if (sel==-1) return;
	if ((sel<(txt_palette_cur_page*txt_palette_per_page_count)) || (sel>=((txt_palette_cur_page+1)*txt_palette_per_page_count))) return;
	
	x=((sel-(txt_palette_cur_page*txt_palette_per_page_count))*txt_palette_pixel_sz)+txt_palette_page_list_width;
	
	glColor4f(1.0f,0.0f,0.0f,1.0f);
	
	glBegin(GL_LINE_LOOP);
	glVertex2i(x,ty);
	glVertex2i((x+txt_palette_pixel_sz),ty);
	glVertex2i((x+txt_palette_pixel_sz),by);
	glVertex2i(x,by);
	glEnd();
}

/* =======================================================

      Texture Palette Clicking
      
======================================================= */

void texture_palette_click(texture_type *txt_list,d3pnt *pnt,bool dblclick)
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
		
	if (txt_list[nsel].frames[0].bitmap.gl_id==-1) return;
	
	texture_palette_put_selected_texture(nsel);
	main_wind_draw();
}

