/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Shader Execution

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
extern view_type			view;
extern setup_type			setup;
extern file_path_setup_type	file_path_setup;

extern int					nuser_shader;
extern shader_type			*gl_shader_current,
							user_shaders[max_iface_user_shader],
							core_shaders[max_shader_light+1][max_core_shader],
							color_shader,bitmap_shader,bitmap_rect_shader;

/* =======================================================

      Execute Simple Color Shaders
      
======================================================= */

void gl_shader_draw_simple_color_start(void)
{
		// remember current shader

	gl_shader_current=NULL;
}

void gl_shader_draw_simple_color_end(void)
{
		// deactivate any current shader
		
	if (gl_shader_current!=NULL) glUseProgramObjectARB(0);
}

void gl_shader_draw_execute_simple_color(int vertex_size,float *vertexes,unsigned char *colors)
{
		// change over the shader
		
	if ((&color_shader)!=gl_shader_current) {
		gl_shader_current=&color_shader;
		glUseProgramObjectARB(color_shader.program_obj);
	}
	
		// bind the required attributes
		
	glVertexAttribPointerARB(color_shader.var_locs.dim3Vertex,vertex_size,GL_FLOAT,GL_FALSE,0,(void*)vertexes);
	glVertexAttribPointerARB(color_shader.var_locs.dim3VertexColor,4,GL_UNSIGNED_BYTE,GL_TRUE,0,(void*)colors);
}

/* =======================================================

      Execute Simple Bitmap Shaders
      
======================================================= */

void gl_shader_draw_simple_bitmap_start(void)
{
		// remember current shader

	gl_shader_current=NULL;

		// enable texturing
		
	glEnable(GL_TEXTURE_2D);
}

void gl_shader_draw_simple_bitmap_end(void)
{
		// deactivate any current shader
		
	if (gl_shader_current!=NULL) glUseProgramObjectARB(0);
	
		// turn off any used textures

	glDisable(GL_TEXTURE_2D);
}

void gl_shader_draw_execute_simple_bitmap(unsigned long gl_id,int vertex_size,float *vertexes,float *uvs,unsigned char *colors)
{
		// change over the shader
		
	if ((&bitmap_shader)!=gl_shader_current) {
		gl_shader_current=&bitmap_shader;
		glUseProgramObjectARB(bitmap_shader.program_obj);
	}

		// bind the bitmap

	gl_texture_bind(0,FALSE,gl_id);
	
		// bind the required attributes
		
	glVertexAttribPointerARB(bitmap_shader.var_locs.dim3Vertex,vertex_size,GL_FLOAT,GL_FALSE,0,(void*)vertexes);
	glVertexAttribPointerARB(bitmap_shader.var_locs.dim3VertexUV,2,GL_FLOAT,GL_FALSE,0,(void*)uvs);
	glVertexAttribPointerARB(bitmap_shader.var_locs.dim3VertexColor,4,GL_UNSIGNED_BYTE,GL_TRUE,0,(void*)colors);
}

/* =======================================================

      Execute Simple Bitmap Rectangle Shaders
      
======================================================= */

void gl_shader_draw_simple_bitmap_rect_start(void)
{
#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)
	gl_shader_draw_simple_bitmap_start(gl_id);
#else
		// remember current shader

	gl_shader_current=NULL;

		// enable texturing
		
	glEnable(GL_TEXTURE_RECTANGLE);
#endif
}

void gl_shader_draw_simple_bitmap_rect_end(void)
{
#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)
	gl_shader_draw_simple_bitmap_end(gl_id);
#else
		// deactivate any current shader
		
	if (gl_shader_current!=NULL) glUseProgramObjectARB(0);
	
		// turn off any used textures

	glDisable(GL_TEXTURE_RECTANGLE);
#endif
}

void gl_shader_draw_execute_simple_bitmap_rect(unsigned long gl_id,int vertex_size,float *vertexes,float *uvs,unsigned char *colors)
{
#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)
	gl_shader_draw_execute_simple_bitmap(gl_id,vertex_size,vertexes,uvs,colors);
#else

		// change over the shader
		
	if ((&bitmap_rect_shader)!=gl_shader_current) {
		gl_shader_current=&bitmap_rect_shader;
		glUseProgramObjectARB(bitmap_rect_shader.program_obj);
	}

		// bind the bitmap

	gl_texture_bind(0,TRUE,gl_id);
	
		// bind the required attributes
		
	glVertexAttribPointerARB(bitmap_rect_shader.var_locs.dim3Vertex,vertex_size,GL_FLOAT,GL_FALSE,0,(void*)vertexes);
	glVertexAttribPointerARB(bitmap_rect_shader.var_locs.dim3VertexUV,2,GL_FLOAT,GL_FALSE,0,(void*)uvs);
	glVertexAttribPointerARB(bitmap_rect_shader.var_locs.dim3VertexColor,4,GL_UNSIGNED_BYTE,GL_TRUE,0,(void*)colors);
#endif
}

/* =======================================================

      Execute Map Shaders
      
======================================================= */

