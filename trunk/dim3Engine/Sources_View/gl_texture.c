/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Texture Routines

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

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern render_info_type		render_info;

bitmap_type					null_bitmap,lmap_black_bitmap,lmap_white_bitmap;

float						gl_texture_current_alpha,gl_texture_current_glow_color;
GLuint						gl_texture_current_binds[4];

/* =======================================================

      Texture Utility Initialization
      
======================================================= */

void gl_texture_initialize(void)
{
	int				n;
	d3col			col;
	
		// a null bitmap for certain rendering
		// operations

	col.r=col.b=col.g=0.0f;
	bitmap_color(&null_bitmap,&col);

		// a all black bitmap for polygons
		// missing a lightmap

	col.r=col.b=col.g=0.0f;
	bitmap_color(&lmap_black_bitmap,&col);

		// an all white bitmap for debug mode

	col.r=col.b=col.g=1.0f;
	bitmap_color(&lmap_white_bitmap,&col);

		// we keep track of all bound textures
		// to reduce the number of bindings

	for (n=0;n!=4;n++) {
		gl_texture_current_binds[n]=-1;
	}
}

void gl_texture_shutdown(void)
{
	bitmap_close(&null_bitmap);
	bitmap_close(&lmap_black_bitmap);
	bitmap_close(&lmap_white_bitmap);
}

/* =======================================================

      Texture Binding
      
======================================================= */

void gl_texture_frame_start(void)
{
	int			n;

	for (n=0;n!=4;n++) {
		gl_texture_current_binds[n]=-1;
	}
}

void gl_texture_bind(int unit,bool rectangle,GLuint txt_id)
{
	if (gl_texture_current_binds[unit]==txt_id) return;

	gl_texture_current_binds[unit]=txt_id;

	glActiveTexture(GL_TEXTURE0+unit);
	if (!rectangle) {
		glBindTexture(GL_TEXTURE_2D,txt_id);
	}
	else {
		glBindTexture(GL_TEXTURE_RECTANGLE,txt_id);
	}
}

void gl_texture_clear(int unit,bool rectangle)
{
	gl_texture_bind(unit,rectangle,0);
	gl_texture_current_binds[unit]=-1;
}

/* =======================================================

      Simple Texture Drawing
      
======================================================= */

void gl_texture_simple_start(void)
{
		// preset texture unit 0
		// it simply modultes the texture with the preset color/alpha
		
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_ALPHA,GL_SRC_ALPHA);
}

void gl_texture_simple_end(void)
{
		// reset any possible clamping
		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	
	glDisable(GL_TEXTURE_2D);
}

void gl_texture_simple_set(GLuint txt_id,bool clamp,float r,float g,float b,float alpha)
{
	GLfloat			col4[4];
	
		// set the texture
		
	gl_texture_bind(0,FALSE,txt_id);
	
		// set the clamping
		
	if (clamp) {
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	}
	
		// put the constants in environment color
		
	col4[0]=r;
	col4[1]=g;
	col4[2]=b;
	col4[3]=alpha;
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,col4);
}

