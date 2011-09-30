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

#include <carbon/carbon.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <pthread.h>

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>

#include <zlib.h>

//
// os flags
//

#define D3_OS_MAC				1
#undef D3_OPENGL_ES
#undef D3_ROTATE_VIEW

//
// some opengl defines
//

#define GL_TEXTURE_RECTANGLE	GL_TEXTURE_RECTANGLE_ARB

//
// os specific keys
//

#define D3_KEY_TAB				0x9
#define D3_KEY_ESC				0x1B
#define D3_KEY_BACKSPACE		0x8
#define D3_KEY_DELETE			0x7F
#define D3_KEY_LEFT				0x1C
#define D3_KEY_RIGHT			0x1D
#define D3_KEY_UP				0x1E
#define D3_KEY_DOWN				0x1F

//
// os specific types
//

typedef unsigned char			byte;
typedef unsigned char*			ptr;
typedef unsigned char			bool8;
typedef int						d3socket;

#define D3_NULL_SOCKET			-1
#define closesocket(x)			close(x)

#define D3_APP_EXTENSION		"app"
