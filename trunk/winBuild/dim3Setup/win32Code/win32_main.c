/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: win32 Setup Main

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

#include "dim3Setup.h"

#include "glue.h"
#include "interface.h"
#include "ui_common.h"

#include "resource.h"

HINSTANCE						hinst;
ATOM							wnd_rg_class;
HWND							wnd;
HMENU							wnd_menu;
HACCEL							wnd_accel;
HDC								wnd_gl_dc;
HGLRC							wnd_gl_ctx;

bool							quit;

extern iface_type				iface;
extern file_path_setup_type		file_path_setup;

/* =======================================================

      Unused About Dialog
      
======================================================= */

void dialog_about_run(void)
{
}

/* =======================================================

      UI Window Procedure
      
======================================================= */

LRESULT CALLBACK setup_wnd_proc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int					cmd;
	d3pnt				pnt;
	POINT				wpt;
	PAINTSTRUCT			ps;

	switch (msg) {

		case WM_PAINT:
			BeginPaint(wnd,&ps);
			main_wind_draw();
			EndPaint(wnd,&ps);
			break;

		case WM_SIZE:
			main_wind_resize();
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			pnt.x=LOWORD(lParam);
			pnt.y=HIWORD(lParam);

			SetCapture(wnd);
			main_wind_click(&pnt,(msg==WM_LBUTTONDBLCLK));
			ReleaseCapture();
			break;

		case WM_MOUSEWHEEL:
			wpt.x=LOWORD(lParam);
			wpt.y=HIWORD(lParam);
			ScreenToClient(wnd,&wpt);
			pnt.x=wpt.x;
			pnt.y=wpt.y;
			main_wind_scroll_wheel(&pnt,(GET_WHEEL_DELTA_WPARAM(wParam)/60));
			break;

		case WM_MOUSEMOVE:
			pnt.x=LOWORD(lParam);
			pnt.y=HIWORD(lParam);
			main_wind_mouse_move(&pnt);
			return(DefWindowProc(hWnd,msg,wParam,lParam));

		case WM_KEYDOWN:
			main_wind_key((char)wParam);
			break;

		case WM_SETCURSOR:
			if (!main_wind_cursor()) return(DefWindowProc(hWnd,msg,wParam,lParam));
			break;

		case WM_COMMAND:
			menu_event_run(LOWORD(wParam));
			break;

		case WM_CLOSE:
			if (menu_quit_save()) os_application_quit();
			break;

		default:
			return(DefWindowProc(hWnd,msg,wParam,lParam));

	}

	return(0);
}

/* =======================================================

      Start/Stop Window
      
======================================================= */

void win32_main_wind_open(void)
{
	int						x,y,wid,high,wstyle,format;
	RECT					wbox,cbox;
	WNDCLASSEX				wcx;
	PIXELFORMATDESCRIPTOR	pf;
	HINSTANCE				hInst;

		// create window

	hInst=GetModuleHandle(NULL);
 
    wcx.cbSize=sizeof(wcx);
    wcx.style=CS_DBLCLKS;
    wcx.lpfnWndProc=setup_wnd_proc;
    wcx.cbClsExtra=0;
    wcx.cbWndExtra=0;
    wcx.hInstance=hInst;
    wcx.hIcon=NULL;
    wcx.hCursor=LoadCursor(NULL,IDC_ARROW);
    wcx.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
    wcx.lpszMenuName=NULL;
    wcx.lpszClassName="dim3SetupWindowClass";
    wcx.hIconSm=NULL;

    RegisterClassEx(&wcx);

		// setup window isn't maximized

	GetClientRect(GetDesktopWindow(),&wbox);

	cbox.left=0;
	cbox.top=0;
	cbox.right=list_palette_tree_sz*2;
	cbox.bottom=(wbox.bottom-wbox.top)-150;
	
	wstyle=WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_MAXIMIZE;

	AdjustWindowRect(&cbox,wstyle,TRUE);

	wid=cbox.right-cbox.left;
	high=cbox.bottom-cbox.top;

	x=((wbox.right-wbox.left)-wid)/2;
	y=50;

    wnd=CreateWindow("dim3SetupWindowClass","dim3 Setup",wstyle,x,y,wid,high,NULL,NULL,hInst,NULL);

		// menu

	menu_create();
	menu_update();

		// show window

	ShowWindow(wnd,SW_SHOWNORMAL);

		// start opengl

	wnd_gl_dc=GetDC(wnd);
	
	bzero(&pf,sizeof(PIXELFORMATDESCRIPTOR));
	pf.nSize=sizeof(PIXELFORMATDESCRIPTOR);
	pf.nVersion=1;
	pf.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_GENERIC_ACCELERATED|PFD_DOUBLEBUFFER;
	pf.iPixelType=PFD_TYPE_RGBA;
	pf.cColorBits=24;
	pf.cAlphaBits=8;
	pf.cDepthBits=24;
	
	format=ChoosePixelFormat(wnd_gl_dc,&pf);
	SetPixelFormat(wnd_gl_dc,format,&pf);

	wnd_gl_ctx=wglCreateContext(wnd_gl_dc);
	wglMakeCurrent(wnd_gl_dc,wnd_gl_ctx);

		// initialize glew

	glewInit();

		// default opengl setup

	main_wind_gl_setup();

		// initialize

	main_wind_initialize();

	main_wind_draw();
}

void win32_main_wind_close(void)
{
		// shutdown

	main_wind_shutdown();

		// close opengl

	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(wnd_gl_ctx);
	ReleaseDC(wnd,wnd_gl_dc);

		// delete menu

	menu_dispose();

		// delete window

	DestroyWindow(wnd);
	UnregisterClass("dim3SetupWindowClass",GetModuleHandle(NULL));
}

/* =======================================================

      Setup Message Pump
      
======================================================= */

void setup_pump(void)
{
	MSG			msg;

	quit=FALSE;

	while (!quit) {
		if (GetMessage(&msg,NULL,0,0)>0) {
			if (!TranslateAccelerator(wnd,wnd_accel,&msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
}

/* =======================================================

      Setup Main
      
======================================================= */

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	hinst=hInstance;
	
		// initialize

	if (!main_app_initialize()) return(0);

		// main loop

	win32_main_wind_open();
	setup_pump();
	win32_main_wind_close();

		// shutdown

	main_app_shutdown();

	return(0);
}

