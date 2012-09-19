/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View OpenRL Patches

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
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

#ifdef OpenRL

	#define view_rl_buffer_wid		320
	#define view_rl_buffer_high		200

	int								view_rl_scene_id;
	GLuint							view_rl_gl_id;
#endif

/* =======================================================

      Blank Patches
      
======================================================= */

#ifndef OpenRL

bool view_openrl_initialize(char *err_str) { return(TRUE); }
void view_openrl_shutdown(void) {}

#else

/* =======================================================

      View OpenRL Init/Shutdown
      
======================================================= */

bool view_openrl_initialize(char *err_str)
{
	unsigned char		*data;

		// initialize OpenRL

	if (rlInitialize()!=RL_ERROR_OK) {
		strcpy(err_str,"Unable to initialize OpenRL");
		return(FALSE);
	}

		// make the scene

	view_rl_scene_id=rlSceneAdd(view_rl_buffer_wid,view_rl_buffer_high,RL_SCENE_TARGET_MEMORY,RL_SCENE_FORMAT_32_RGBA);
	if (view_rl_scene_id<0) {
		strcpy(err_str,"Unable to create OpenRL scene");
		rlShutdown();
		return(FALSE);
	}

		// we need a texture to transfer
		// the scene to opengl raster

	glGenTextures(1,&view_rl_gl_id);

	glBindTexture(GL_TEXTURE_RECTANGLE_ARB,view_rl_gl_id);

	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_RECTANGLE_ARB,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	sz=(view_rl_buffer_wid*4)*buffer_height;
	data=malloc(sz);
	if (data==NULL) {
		strcpy(err_str,"Out of memory");
		rlShutdown();
		return(FALSE);
	}
	bzero(data,sz);

	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB,0,GL_RGBA,view_rl_buffer_wid,view_rl_buffer_high,0,GL_RGBA,GL_UNSIGNED_BYTE,data);

	free(data);

	glBindTexture(GL_TEXTURE_RECTANGLE_ARB,0);
}

void view_openrl_shutdown(void)
{
	glDeleteTextures(1,&view_rl_gl_id);

	rlSceneDelete(view_rl_scene_id);
	rlShutdown();
}



#endif