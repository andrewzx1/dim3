#ifdef OSX

	#include <carbon/carbon.h>

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <sys/stat.h>
	#include <sys/param.h>
	#include <sys/sysctl.h>
	#include <unistd.h>
	#include <dirent.h>

	#include <pthread.h>

	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
	#include <OpenGL/glu.h>

	#include <zlib.h>
	
	#define GL_TEXTURE_RECTANGLE	GL_TEXTURE_RECTANGLE_ARB

	#define os_mutex				pthread_mutex_t
	#define os_thread				pthread_t

#endif

#ifdef LINUX

	#include <unistd.h>

	#define os_mutex				pthread_mutex_t
	#define os_thread				pthread_t

#endif

#ifdef WIN32

	#define _CRT_SECURE_NO_WARNINGS

	#include <windows.h>

	#include <time.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <math.h>
	#include <time.h>
	#include <process.h>

	#include "glew.h"
	#include "wglew.h"

	#define inline					__inline

	typedef unsigned char			bool;

	#define bzero(a,b)				memset(a,0x0,(b))
	#define bcmp(a,b,c)				memcmp(a,b,(c))
	#define usleep(x)               Sleep((DWORD)((x)/1000))
	
	#define random					rand
	#define srandom					srand

	#define os_mutex				HANDLE	
	#define os_thread				uintptr_t

#endif

