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
#include "ui_common.h"

int							txt_palette_cur_page;

extern int texture_palette_get_selected_texture(void);
extern void texture_palette_put_selected_texture(int txt_idx);

/* =======================================================

      Texture Palette Drawing
      
======================================================= */

void texture_palette_draw(texture_type *txt_list)
{
	int					n,k,x,ty,by,sel,idx,
						page_list_count,per_page_count,pixel_sz;
	float				vertexes[8],uvs[8]={0.0f,0.0f,1.0f,0.0f,1.0f,1.0f,0.0f,1.0f};
	bool				has_texture;
	d3rect				wbox,tbox;
	texture_type		*texture;
	
		// viewport setup
		
	os_get_window_box(&wbox);
	texture_palette_box(&tbox);

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

	glEnableClientState(GL_VERTEX_ARRAY);
	
		// settings
		
	page_list_count=texture_palette_page_list_count();
	per_page_count=texture_palette_per_page_count();
	pixel_sz=texture_palette_pixel_size();
	
		// texture page switch

	if (texture_palette_page_list_width()!=0) {
	
		x=0;
		ty=tbox.ty;
		by=ty+(pixel_sz>>1);

		for (n=0;n!=page_list_count;n++) {
		
				// time to move down?
				
			if (n==(page_list_count>>1)) {
				x=0;
				ty=by;
				by=tbox.ty+pixel_sz;
			}
			
				// draw page

			vertexes[0]=vertexes[6]=(float)x;
			vertexes[2]=vertexes[4]=(float)(x+16);
			vertexes[1]=vertexes[3]=(float)ty;
			vertexes[5]=vertexes[7]=(float)by;
			
			glVertexPointer(2,GL_FLOAT,0,vertexes);
			
			if (txt_palette_cur_page==n) {
				glColor4f(0.3f,0.3f,1.0f,1.0f);
			}
			else {
				glColor4f(0.75f,0.75f,0.75f,1.0f);
			}

			glDrawArrays(GL_QUADS,0,4);

				// outline

			glColor4f(0.0f,0.0f,0.0f,1.0f);
			glDrawArrays(GL_LINE_LOOP,0,4);
			
				// mark if there are textures
				
			has_texture=FALSE;
			idx=n*per_page_count;
			
			for (k=idx;k!=(idx+per_page_count);k++) {
				if (txt_list[k].frames[0].bitmap.gl_id!=-1) {
					has_texture=TRUE;
					break;
				}
			}
			
			if (has_texture) {
				vertexes[0]=vertexes[6]=(float)(x+11);
				vertexes[2]=vertexes[4]=(float)(x+14);
				vertexes[1]=vertexes[3]=(float)(by-5);
				vertexes[5]=vertexes[7]=(float)(by-2);
			
				glVertexPointer(2,GL_FLOAT,0,vertexes);
				
				glColor4f(0.0f,0.0f,0.0f,1.0f);
				glDrawArrays(GL_QUADS,0,4);
			}
			
			x+=16;
		}
	}

		// textures
		
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);

	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	x=texture_palette_page_list_width();
	ty=tbox.ty+1;
	by=(ty+pixel_sz)-1;
		
	for (n=0;n!=per_page_count;n++) {
		texture=&txt_list[n+(txt_palette_cur_page*per_page_count)];
		
			// the textures
			
		if (texture->frames[0].bitmap.gl_id!=-1) {
			vertexes[0]=vertexes[6]=(float)x;
			vertexes[2]=vertexes[4]=(float)(x+pixel_sz);
			vertexes[1]=vertexes[3]=(float)ty;
			vertexes[5]=vertexes[7]=(float)by;

			glVertexPointer(2,GL_FLOAT,0,vertexes);
			glTexCoordPointer(2,GL_FLOAT,0,uvs);

			glBindTexture(GL_TEXTURE_2D,texture->frames[0].bitmap.gl_id);
			glDrawArrays(GL_QUADS,0,4);
		}
		
		x+=pixel_sz;
	}
	
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDisable(GL_ALPHA_TEST);
	
		// right fill
	
	vertexes[0]=vertexes[6]=(float)x;
	vertexes[2]=vertexes[4]=(float)tbox.rx;
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);
		
	glColor4f(0.75f,0.75f,0.75f,1.0f);
	glDrawArrays(GL_QUADS,0,4);
	
		// lines
		
	x=texture_palette_page_list_width();
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	for (n=0;n!=per_page_count;n++) {
		idx=n+(txt_palette_cur_page*per_page_count);
	
		vertexes[0]=vertexes[6]=(float)x;
		vertexes[2]=vertexes[4]=(float)(x+pixel_sz);
		vertexes[1]=vertexes[3]=(float)ty;
		vertexes[5]=vertexes[7]=(float)by;

		glVertexPointer(2,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINE_LOOP,0,4);
		
		x+=pixel_sz;
	}

	vertexes[0]=(float)tbox.lx;
	vertexes[2]=(float)tbox.rx;
	vertexes[1]=vertexes[3]=(float)ty;
	
	glVertexPointer(2,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_LINES,0,2);
	
		// selection
		
	sel=texture_palette_get_selected_texture();
	if (sel==-1) return;
	if ((sel<(txt_palette_cur_page*per_page_count)) || (sel>=((txt_palette_cur_page+1)*per_page_count))) return;
	
	x=((sel-(txt_palette_cur_page*per_page_count))*pixel_sz)+texture_palette_page_list_width();
	
	vertexes[0]=vertexes[6]=(float)x;
	vertexes[2]=vertexes[4]=(float)(x+pixel_sz);
	vertexes[1]=vertexes[3]=(float)ty;
	vertexes[5]=vertexes[7]=(float)by;

	glVertexPointer(2,GL_FLOAT,0,vertexes);

	glColor4f(1.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_LINE_LOOP,0,4);

	glDisableClientState(GL_VERTEX_ARRAY);
}

/* =======================================================

      Texture Palette Clicking
      
======================================================= */

void texture_palette_click(texture_type *txt_list,d3pnt *pnt,bool double_click)
{
	int				nsel,page,
					page_list_count,per_page_count,pixel_sz;
	d3rect			tbox;
					
		// settings
		
	page_list_count=texture_palette_page_list_count();
	per_page_count=texture_palette_per_page_count();
	pixel_sz=texture_palette_pixel_size();
	texture_palette_box(&tbox);
	
		// move within palette
		
	pnt->y-=tbox.ty;
	
		// texture page change
		
	if (pnt->x<texture_palette_page_list_width()) {
		page=((pnt->y/(pixel_sz>>1))*(page_list_count>>1))+(pnt->x/16);
		if (txt_palette_cur_page!=page) {
			txt_palette_cur_page=page;
			main_wind_draw();
		}
		return;
	}
	
		// find clicked texture
		
	nsel=(pnt->x-texture_palette_page_list_width())/pixel_sz;
	nsel+=(txt_palette_cur_page*per_page_count);
		
		// double clicks
		
	if (double_click) {
		texture_edit_switch(nsel);
		main_wind_draw();
		return;
	}
    
		// single clicks
		
	if (txt_list[nsel].frames[0].bitmap.gl_id==-1) return;
	
	texture_palette_put_selected_texture(nsel);
	main_wind_draw();
}

