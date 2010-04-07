/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Check Rendering Features

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
	#include "dim3engine.h"
#endif

#include "video.h"

bool						gl_check_value_frame_buffer,
							gl_check_value_fsaa,
							gl_check_value_texture_compress,
							gl_check_value_texture_anisotropic_filter,
							gl_check_value_texture_generate_mipmaps,
							gl_check_value_texture_rectangle,
							gl_check_value_shader;

extern bool					arch_is_ppc;

extern setup_type			setup;
extern render_info_type		render_info;

/* =======================================================

      GL Checks Setup
      
======================================================= */

bool gl_check_initialize_shader(void)
{
#ifdef D3_OS_MAC
	GLint			fragGPU,vertGPU;
#endif

		// don't run shaders on PPC machines
		
	if (arch_is_ppc) return(FALSE);

		// check GL string
		
	if (strstr(render_info.ext_string,"GL_ARB_shader_objects")==NULL) return(FALSE);
	if (strstr(render_info.ext_string,"GL_ARB_fragment_shader")==NULL) return(FALSE);
	if (strstr(render_info.ext_string,"GL_ARB_vertex_shader")==NULL) return(FALSE);
	if (strstr(render_info.ext_string,"GL_ARB_shading_language_100")==NULL) return(FALSE);
	
		// check if GPU will actually handle it
		
#ifdef D3_OS_MAC
	CGLGetParameter(CGLGetCurrentContext(),kCGLCPGPUFragmentProcessing,&fragGPU);
	CGLGetParameter(CGLGetCurrentContext(),kCGLCPGPUVertexProcessing,&vertGPU);
	if (!(fragGPU && vertGPU)) return(FALSE);
#endif

	return(TRUE);
}

void gl_check_initialize(void)
{
	gl_check_value_frame_buffer=FALSE;
	
	if (strstr(render_info.ext_string,"GL_EXT_framebuffer_object")!=NULL) {
		gl_check_value_frame_buffer=(strstr(render_info.ext_string,"GL_EXT_packed_depth_stencil")!=NULL);
	}
	
	gl_check_value_texture_compress=(strstr(render_info.ext_string,"GL_ARB_texture_compression")!=NULL);
	gl_check_value_fsaa=(strstr(render_info.ext_string,"GL_ARB_multisample")!=NULL);
	gl_check_value_texture_anisotropic_filter=(strstr(render_info.ext_string,"GL_EXT_texture_filter_anisotropic")!=NULL);
	gl_check_value_texture_generate_mipmaps=(strstr(render_info.ext_string,"GL_SGIS_generate_mipmap")!=NULL);
	gl_check_value_texture_rectangle=(strstr(render_info.ext_string,"GL_ARB_texture_rectangle")!=NULL);

	gl_check_value_shader=gl_check_initialize_shader();
}

/* =======================================================

      GL Checks
      
======================================================= */

inline bool gl_check_frame_buffer_ok(void)
{
	return(gl_check_value_frame_buffer);
}

inline bool gl_check_fsaa_ok(void)
{
	return(gl_check_value_fsaa);
}

inline bool gl_check_texture_compress_ok(void)
{
	return(gl_check_value_texture_compress);
}

inline bool gl_check_texture_anisotropic_filter_ok(void)
{
	return(gl_check_value_texture_anisotropic_filter);
}

inline bool gl_check_texture_generate_mipmaps_ok(void)
{
	return(gl_check_value_texture_generate_mipmaps);
}

inline bool gl_check_texture_rectangle_ok(void)
{
	return(gl_check_value_texture_rectangle);
}

inline bool gl_check_shader_ok(void)
{
	return(gl_check_value_shader);
}


