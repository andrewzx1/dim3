/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Text Routines

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

extern iface_type			iface;
extern setup_type			setup;

int							font_index,font_size;
float						*gl_text_vertexes,*gl_text_uvs;
texture_font_type			fonts[2];

/* =======================================================

      Initialize/Shutdown Text Drawing
      
======================================================= */

void gl_text_initialize(void)
{
	int			n;

		// load the fonts
		
	for (n=0;n!=max_iface_font_variant;n++) {
		strcpy(fonts[font_interface_index].name[n],iface.font.interface_name[n]);
		strcpy(fonts[font_hud_index].name[n],iface.font.hud_name[n]);
	}

	bitmap_text_initialize(&fonts[font_interface_index]);
	bitmap_text_initialize(&fonts[font_hud_index]);
	
		// memory for vertexes
		
	gl_text_vertexes=(float*)malloc(((1024*4)*2)*sizeof(float));
	gl_text_uvs=(float*)malloc(((1024*4)*2)*sizeof(float));
}

void gl_text_shutdown(void)
{
	bitmap_text_shutdown(&fonts[font_interface_index]);
	bitmap_text_shutdown(&fonts[font_hud_index]);
	
	free(gl_text_vertexes);
	free(gl_text_uvs);
}

/* =======================================================

      Get Proper Font
      
======================================================= */

texture_font_size_type* gl_text_get_font(int text_font,int text_size)
{
	if (text_size<=24) return(&fonts[text_font].size_24);
	return(&fonts[text_font].size_48);
}

/* =======================================================

      Character/String Sizes
      
======================================================= */

int gl_text_get_char_height(int text_size)
{
	return((int)(((float)text_size)*text_height_factor));
}

int gl_text_get_string_width(int text_font,int text_size,char *str)
{
	int						i,ch,len;
	float					fx,f_wid;
	char					*c;
	texture_font_size_type	*font;
	
	font=gl_text_get_font(text_font,text_size);
	
	f_wid=(float)text_size;
	
	c=str;
	len=strlen(str);
	
	fx=0.0f;
	
	for (i=0;i<len;i++) {
		ch=(int)*c++;

		if ((ch>='!') && (ch<='z')) {
			ch-='!';
			fx+=(f_wid*font->char_size[ch]);
		}
		else {
			fx+=(f_wid/3.0f);
		}
	}
	
	return((int)fx);
}

/* =======================================================

      Start and End Text Drawing
      
======================================================= */

void gl_text_start(int text_font,int text_size)
{
	GLfloat					fct[4];
	texture_font_size_type	*font;
	
	font=gl_text_get_font(text_font,text_size);

		// remember font size
		
	font_index=text_font;
	font_size=text_size;
	
		// setup texture
		
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	
	gl_texture_bind(0,font->bitmap.gl_id);
	
		// texture combines
		
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);

	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);
	
		// alpha
		
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_ALPHA,GL_SRC_ALPHA);

	fct[0]=fct[1]=fct[2]=fct[3]=1.0f;	
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,fct);
	
		// no wrapping
		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	
		// transparency blending
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
}

void gl_text_end(void)
{
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	
		// restore wrapping
		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    
	glDisable(GL_TEXTURE_2D);
}

/* =======================================================

      Draw Line of Text
      
======================================================= */

void gl_text_draw_internal(int x,int y,char *txt,int just,bool vcenter,d3col *col,float alpha)
{
	int						n,txtlen,ch,xoff,yoff,cnt;
	float					f_lft,f_rgt,f_top,f_bot,f_wid,f_high,
							gx_lft,gx_rgt,gy_top,gy_bot;
	float					*vp,*uv;
	char					*c;
	GLfloat					fct[4];
	texture_font_size_type	*font;

		// get text length

	txtlen=strlen(txt);
	if (txtlen==0) return;
	if (txtlen>255) txtlen=255;
	
		// get font
		
	font=gl_text_get_font(font_index,font_size);

        // font justification
        
	switch (just) {
		case tx_center:
			x-=(gl_text_get_string_width(font_index,font_size,txt)>>1);
			break;
		case tx_right:
			x-=gl_text_get_string_width(font_index,font_size,txt);
			break;
	}
	
        // font color and alpha
        
	glColor4f(col->r,col->g,col->b,1.0f);

	fct[0]=fct[1]=fct[2]=1.0f;
	fct[3]=alpha;
	
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,fct);
    
		// get width and height
		
	f_wid=(float)font_size;
	f_high=f_wid*text_height_factor;

		// construct vertexes

	vp=gl_text_vertexes;
	uv=gl_text_uvs;

		// create the quads

	f_lft=(float)x;
	f_bot=(float)y;
	if (vcenter) f_bot+=((f_high/2)+(f_high/8));		// add in middle + descender
	f_top=f_bot-f_high;
	
	c=txt;
	cnt=0;
	
	for (n=0;n<txtlen;n++) {
	
		ch=(int)*c++;

		if ((ch<'!') || (ch>'z')) {
			f_lft+=(f_wid/3);
			continue;
		}

		ch-='!';

			// the vertexes

		f_rgt=f_lft+f_wid;

		*vp++=f_lft;
		*vp++=f_top;
		*vp++=f_lft;
		*vp++=f_bot;
		*vp++=f_rgt;
		*vp++=f_top;
		*vp++=f_rgt;
		*vp++=f_bot;

		f_lft+=(f_wid*font->char_size[ch]);

			// the UVs

		yoff=ch/font->char_per_line;
		xoff=ch-(yoff*font->char_per_line);

		gx_lft=((float)xoff)*font->gl_xoff;
		gx_rgt=gx_lft+font->gl_xadd;
		gy_top=((float)yoff)*font->gl_yoff;
		gy_bot=gy_top+font->gl_yadd;

		*uv++=gx_lft;
		*uv++=gy_top;
		*uv++=gx_lft;
		*uv++=gy_bot;
		*uv++=gx_rgt;
		*uv++=gy_top;
		*uv++=gx_rgt;
		*uv++=gy_bot;

			// remember number of characters

		cnt++;
	}

		// draw text

	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)gl_text_vertexes);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)gl_text_uvs);

	for (n=0;n!=cnt;n++) {
		glDrawArrays(GL_TRIANGLE_STRIP,(n*4),4);
	}

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void gl_text_draw(int x,int y,char *txt,int just,bool vcenter,d3col *col,float alpha)
{
	d3col			shadow_col={0.3f,0.3f,0.3f};
	
	gl_text_draw_internal((x+1),(y+1),txt,just,vcenter,&shadow_col,(alpha*0.5f));
	gl_text_draw_internal(x,y,txt,just,vcenter,col,alpha);
}

