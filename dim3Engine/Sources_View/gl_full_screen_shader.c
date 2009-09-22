/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Full Screen Shader Routines

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "video.h"

GLuint						fs_shader_fbo_id,fs_shader_fbo_depth_stencil_id,fs_shader_txt_id;
bool						fs_shader_on,fs_shader_init,fs_shader_active;

extern map_type				map;
extern setup_type			setup;
extern view_type			view;
extern render_info_type		render_info;

// supergumba
// GL_TEXTURE_RECTANGLE_ARB

/* =======================================================

      Initialize Full Screen Shader
      
======================================================= */

void gl_fs_shader_initialize(void)
{
		// check if fbo and shaders are available
		// or already started
		
	if (!fs_shader_on) return;
	if (fs_shader_init) return;
	
		// create depth buffer and stencil object

	glGenRenderbuffersEXT(1,&fs_shader_fbo_depth_stencil_id);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT,fs_shader_fbo_depth_stencil_id);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,GL_DEPTH_STENCIL_EXT,setup.screen.x_sz,setup.screen.y_sz);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT,fs_shader_fbo_depth_stencil_id);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_STENCIL_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT,fs_shader_fbo_depth_stencil_id);

		// create the frame buffer object and attach depth/stencil

	glGenFramebuffersEXT(1,&fs_shader_fbo_id);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fs_shader_fbo_id);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT,fs_shader_fbo_depth_stencil_id);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_STENCIL_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT,fs_shader_fbo_depth_stencil_id);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);

		// create the texture

	glGenTextures(1,&fs_shader_txt_id);
	glBindTexture(GL_TEXTURE_2D,fs_shader_txt_id);
	
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,0);
	
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D,0);

		// shader started

	fs_shader_init=TRUE;
}

void gl_fs_shader_shutdown(void)
{
	if (!fs_shader_on) return;
	if (!fs_shader_init) return;

		// destroy frame buffer and depth/stencil

	glDeleteTextures(1,&fs_shader_txt_id);
	glDeleteFramebuffersEXT(1,&fs_shader_fbo_id);
	glDeleteRenderbuffersEXT(1,&fs_shader_fbo_depth_stencil_id);

		// shader stopped

	fs_shader_init=FALSE;
}

/* =======================================================

      Per Map Back Shader Start/Stop
      
======================================================= */

void gl_fs_shader_map_start(void)
{
		// check if fbo and shaders are available
		
	fs_shader_on=gl_check_frame_buffer_ok()&&gl_check_shader_ok();

		// no fs shader started

	fs_shader_init=FALSE;
}

void gl_fs_shader_map_end(void)
{
	gl_fs_shader_shutdown();
}

/* =======================================================

      Start and Stop FS Shader
      
======================================================= */

bool gl_fs_shader_start(char *shader_name,char *err_str)
{
	if (!fs_shader_on) return(TRUE);

		// initialize if needed

	gl_fs_shader_initialize();
		
		// now running under shaders

	fs_shader_active=TRUE;

	return(TRUE);
}

void gl_fs_shader_end(void)
{
	fs_shader_active=FALSE;
}

/* =======================================================

      Run FS Shader Render
      
======================================================= */

void gl_fs_shader_render_begin(void)
{
	if (!fs_shader_on) return;
	if (!fs_shader_active) return;

		// setup fbo

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fs_shader_fbo_id);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D,fs_shader_txt_id,0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT,fs_shader_fbo_depth_stencil_id);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_STENCIL_ATTACHMENT_EXT,GL_RENDERBUFFER_EXT,fs_shader_fbo_depth_stencil_id);
	
	if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)!=GL_FRAMEBUFFER_COMPLETE_EXT) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
		fs_shader_active=FALSE;
	}
}

void gl_fs_shader_render_finish(void)
{
	float			*vertex_ptr,*uv_ptr;

	if (!fs_shader_on) return;
	if (!fs_shader_active) return;

		// make sure rendering is over

	glFlush();

		// turn off the fbo

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);

		// create the vertexes and uv

	vertex_ptr=view_bind_map_next_vertex_object(4*(2+2));
	if (vertex_ptr==NULL) return;

	uv_ptr=vertex_ptr+(4*2);

	*vertex_ptr++=0.0f;
	*vertex_ptr++=0.0f;

	*uv_ptr++=0.0f;
	*uv_ptr++=0.0f;

	*vertex_ptr++=(float)setup.screen.x_sz;
	*vertex_ptr++=0.0f;

	*uv_ptr++=1.0f;
	*uv_ptr++=0.0f;

	*vertex_ptr++=(float)setup.screen.x_sz;
	*vertex_ptr++=(float)setup.screen.y_sz;

	*uv_ptr++=1.0f;
	*uv_ptr++=1.0f;

	*vertex_ptr++=0.0f;
	*vertex_ptr++=(float)setup.screen.y_sz;

	*uv_ptr++=0.0f;
	*uv_ptr++=1.0f;

  	view_unmap_current_vertex_object();

		// setup fbo texture draw

	gl_2D_view_screen();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	glColor4f(1.0f,0.0f,1.0f,1.0f);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

	glBindTexture(GL_TEXTURE_2D,fs_shader_txt_id);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

		// draw the quad

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,(void*)0);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)((4*2)*sizeof(float)));

	glDrawArrays(GL_QUADS,0,4);

 	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

		// finish fbo draw

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	
	glDisable(GL_TEXTURE_2D);

		// unbind the vbo

	view_unbind_current_vertex_object();
}

