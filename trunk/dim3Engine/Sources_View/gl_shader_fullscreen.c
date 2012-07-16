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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

extern map_type				map;
extern setup_type			setup;
extern view_type			view;
extern render_info_type		render_info;

GLuint						fs_shader_fbo_id,fs_shader_fbo_depth_stencil_id,fs_shader_txt_id;
int							fs_shader_idx,fs_shader_life_msec,fs_shader_start_tick;
bool						fs_shader_on,fs_shader_init,fs_shader_active;

extern shader_type			*gl_shader_current,
							user_shaders[max_iface_user_shader];

/* =======================================================

      OpenGL ES Stubs
      
======================================================= */

#ifdef D3_OPENGL_ES

void gl_fs_shader_map_start(void) {}
void gl_fs_shader_map_end(void) {}
bool gl_fs_shader_start(char *shader_name,int life_msec,char *err_str) { return(FALSE); }
void gl_fs_shader_end(void) {}
void gl_fs_shader_render_begin(void) {}
void gl_fs_shader_render_finish(void) {}

#else

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

	glGenRenderbuffers(1,&fs_shader_fbo_depth_stencil_id);
	glBindRenderbuffer(GL_RENDERBUFFER,fs_shader_fbo_depth_stencil_id);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,view.screen.x_sz,view.screen.y_sz);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,fs_shader_fbo_depth_stencil_id);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,GL_RENDERBUFFER,fs_shader_fbo_depth_stencil_id);

		// create the frame buffer object and attach depth/stencil

	glGenFramebuffers(1,&fs_shader_fbo_id);
	glBindFramebuffer(GL_FRAMEBUFFER,fs_shader_fbo_id);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,fs_shader_fbo_depth_stencil_id);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,GL_RENDERBUFFER,fs_shader_fbo_depth_stencil_id);
	
		// some additional setup
		
	gl_setup_context();

		// turn off framebuffer
		
	glBindFramebuffer(GL_FRAMEBUFFER,0);

		// create the texture

	glGenTextures(1,&fs_shader_txt_id);
	
	gl_texture_clear(0,FALSE);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB,fs_shader_txt_id);
	
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB,0,GL_RGBA,view.screen.x_sz,view.screen.y_sz,0,GL_RGBA,GL_UNSIGNED_BYTE,0);
	
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB,0);

		// shader started

	fs_shader_init=TRUE;
}

void gl_fs_shader_shutdown(void)
{
	if (!fs_shader_on) return;
	if (!fs_shader_init) return;

		// destroy frame buffer and depth/stencil

	glDeleteTextures(1,&fs_shader_txt_id);
	glDeleteFramebuffers(1,&fs_shader_fbo_id);
	glDeleteRenderbuffers(1,&fs_shader_fbo_depth_stencil_id);

		// shader stopped

	fs_shader_init=FALSE;
}

/* =======================================================

      Start and Stop FS Shader
      
======================================================= */

bool gl_fs_shader_start(char *shader_name,int life_msec,char *err_str)
{
		// start with no error
		
	err_str[0]=0x0;
	
		// false return but no error (couldn't use shaders)
		
	if (!fs_shader_on) return(FALSE);

		// set the shader

	fs_shader_idx=gl_user_shader_find(shader_name);
	if (fs_shader_idx==-1) {
		sprintf(err_str,"No shader with name: %s\n",shader_name);
		return(FALSE);
	}

		// initialize if needed

	gl_fs_shader_initialize();
		
		// now running under shaders

	fs_shader_start_tick=game_time_get();
	fs_shader_life_msec=life_msec;

	fs_shader_active=TRUE;

	return(TRUE);
}

void gl_fs_shader_end(void)
{
	fs_shader_active=FALSE;
}

/* =======================================================

      Per Map Back Shader Start/Stop
      
======================================================= */

void gl_fs_shader_map_start(void)
{
		// check if fbo and shaders are available
		
	fs_shader_on=gl_check_frame_buffer_ok()&&gl_check_texture_rectangle_ok();

		// no fs shader started

	fs_shader_init=FALSE;
}

void gl_fs_shader_map_end(void)
{
	if (!fs_shader_on) return;

	gl_fs_shader_end();
	gl_fs_shader_shutdown();
}

/* =======================================================

      Run FS Shader Render
      
======================================================= */

void gl_fs_shader_render_begin(void)
{
	if ((!fs_shader_on) || (!fs_shader_active)) return;

		// check for time outs

	if (fs_shader_life_msec!=-1) {
		if ((fs_shader_start_tick+fs_shader_life_msec)<game_time_get()) {
			gl_fs_shader_end();
			return;
		}
	}

		// setup fbo

	glBindFramebuffer(GL_FRAMEBUFFER,fs_shader_fbo_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_RECTANGLE_ARB,fs_shader_txt_id,0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,fs_shader_fbo_depth_stencil_id);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_STENCIL_ATTACHMENT,GL_RENDERBUFFER,fs_shader_fbo_depth_stencil_id);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE) {
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		fs_shader_active=FALSE;
	}
	
	glViewport(0,0,view.screen.x_sz,view.screen.y_sz);

		// clear buffer

	gl_frame_clear(TRUE);
	gl_shader_frame_start();
}

void gl_fs_shader_render_finish(void)
{
	float					vertexes[8],uvs[8];
	shader_type				*shader;
	
	if ((!fs_shader_on) || (!fs_shader_active)) return;

		// make sure rendering is over

	glFlush();

		// turn off the fbo

	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glViewport(0,0,view.screen.x_sz,view.screen.y_sz);

		// create the vertexes and uv

	vertexes[0]=0.0f;
	vertexes[1]=0.0f;
	vertexes[2]=0.0f;
	vertexes[3]=(float)view.screen.y_sz;
	vertexes[4]=(float)view.screen.x_sz;
	vertexes[5]=0.0f;
	vertexes[6]=(float)view.screen.x_sz;
	vertexes[7]=(float)view.screen.y_sz;

	uvs[0]=0.0f;
	uvs[1]=(float)view.screen.y_sz;
	uvs[2]=0.0f;
	uvs[3]=0.0f;
	uvs[4]=(float)view.screen.x_sz;
	uvs[5]=(float)view.screen.y_sz;
	uvs[6]=(float)view.screen.x_sz;
	uvs[7]=0.0f;

		// setup fbo texture draw

	gl_2D_view_screen();

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	
	gl_texture_bind(0,TRUE,fs_shader_txt_id);

		// start the shader

	shader=&user_shaders[fs_shader_idx];
	
	if (gl_shader_current!=shader) {
		gl_shader_current=shader;
		glUseProgram(shader->program_obj);
	}

	shader->start_tick=fs_shader_start_tick;			// make sure frequency matches start of shader
	gl_shader_set_scene_variables(shader);

		// required attributes

	glVertexAttribPointer(shader->var_locs.dim3Vertex,2,GL_FLOAT,GL_FALSE,0,(void*)vertexes);
	glVertexAttribPointer(shader->var_locs.dim3VertexUV,2,GL_FLOAT,GL_FALSE,0,(void*)uvs);

		// draw the quad

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

		// finish fbo draw
	
	glDisable(GL_TEXTURE_RECTANGLE_ARB);
}

#endif
