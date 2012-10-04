#ifdef __APPLE__

	#include <stdio.h>
	#include <stdlib.h>
	#include <sys/stat.h>
	#include <sys/param.h>
	#include <sys/sysctl.h>
	#include <unistd.h>

	#include <pthread.h>

#endif

#ifdef __linux__

	#include <unistd.h>

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

	#define inline					__inline

	typedef unsigned char			bool;

#endif

