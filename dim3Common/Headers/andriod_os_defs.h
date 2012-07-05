/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Specific OS Definitions

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2005 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// includes
//

#include <endian.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <pthread.h>

#include <OpenGLES/es1/gl.h>
#include <OpenGLES/es1/glext.h>

#include <zlib.h>

//
// os flags
//

#define D3_OS_ANDRIOD					1
#define D3_OPENGL_ES					1
#define D3_ROTATE_VIEW					1
#define D3_SIMPLE_RENDER				1

//
// OpenGL ES re-defines
//

#define GL_SOURCE0_RGB					GL_SRC0_RGB
#define GL_SOURCE1_RGB					GL_SRC1_RGB
#define GL_SOURCE0_ALPHA				GL_SRC0_ALPHA
#define GL_SOURCE1_ALPHA				GL_SRC1_ALPHA

#define glMapBuffer						glMapBufferOES
#define glUnmapBuffer					glUnmapBufferOES

#define GL_WRITE_ONLY					GL_WRITE_ONLY_OES

#define glFramebufferRenderbufferEXT	glFramebufferRenderbufferOES
#define glGenFramebuffersEXT			glGenFramebuffersOES
#define glBindFramebufferEXT			glBindFramebufferOES
#define glDeleteFramebuffersEXT			glDeleteFramebuffersOES
#define glFramebufferTexture2DEXT		glFramebufferTexture2DOES
#define glCheckFramebufferStatusEXT		glCheckFramebufferStatusOES
#define glGenRenderbuffersEXT			glGenRenderbuffersOES
#define glRenderbufferStorageEXT		glRenderbufferStorageOES
#define glBindRenderbufferEXT			glBindRenderbufferOES
#define glDeleteRenderbuffersEXT		glDeleteRenderbuffersOES

#define GL_RENDERBUFFER_EXT				GL_RENDERBUFFER_OES
#define GL_FRAMEBUFFER_EXT				GL_FRAMEBUFFER_OES
#define GL_DEPTH_ATTACHMENT_EXT			GL_DEPTH_ATTACHMENT_OES
#define GL_DEPTH_STENCIL_EXT			GL_DEPTH_STENCIL_OES
#define GL_STENCIL_ATTACHMENT_EXT		GL_STENCIL_ATTACHMENT_OES
#define GL_FRAMEBUFFER_BINDING_EXT		GL_FRAMEBUFFER_BINDING_OES
#define GL_COLOR_ATTACHMENT0_EXT		GL_COLOR_ATTACHMENT0_OES
#define GL_FRAMEBUFFER_COMPLETE_EXT		GL_FRAMEBUFFER_COMPLETE_OES

#define glOrtho							glOrthof
#define glFrustum						glFrustumf

//
// os specific keys
//

#define D3_KEY_TAB				0x9
#define D3_KEY_ESC				0x1B
#define D3_KEY_BACKSPACE		0x8
#define D3_KEY_DELETE			0x7F

//
// os specific types
//

typedef unsigned char			byte;
typedef unsigned char*			ptr;
typedef unsigned char			bool8;
typedef int						bool;
typedef int						d3socket;

#define TRUE					1
#define FALSE					0
#define D3_NULL_SOCKET			-1

#define closesocket(x)			close(x)

#define D3_APP_EXTENSION		"a"

