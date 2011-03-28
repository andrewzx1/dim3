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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
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

extern int os_win32_menu_lookup(int id);

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
			cmd=os_win32_menu_lookup(LOWORD(wParam));
			if (cmd==-1) break;

			menu_event_run(cmd);
			break;

		case WM_CLOSE:
			os_application_quit();
			break;

		default:
			return(DefWindowProc(hWnd,msg,wParam,lParam));

	}

	return(0);
}

/* =======================================================

      Start/Stop Window
      
======================================================= */

void main_wind_open(void)
{
	// win32 applications always leave the window open so "new" can be used
	// from the menu
}

void main_wind_close(void)
{
		// win32 keeps window open so menu can be
		// used so we need to reset here

	os_set_title_window("dim3 Setup");
	main_wind_draw();
}

void win32_main_wind_open(void)
{
	int						x,y,wid,high,format;
	RECT					wbox;
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

	wid=(list_palette_tree_sz*3)+8;
	high=(wbox.bottom-wbox.top)-150;

	x=((wbox.right-wbox.left)-wid)/2;
	y=50;

    wnd=CreateWindow("dim3SetupWindowClass","dim3 Setup",WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_MAXIMIZE,x,y,wid,high,NULL,NULL,hInst,NULL);

		// menu

	wnd_menu=LoadMenu(hinst,MAKEINTRESOURCE(IDR_MAIN_MENU));
	SetMenu(wnd,wnd_menu);

	wnd_accel=LoadAccelerators(hinst,MAKEINTRESOURCE(IDR_ACCELERATOR));

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
	pf.cDepthBits=16;
	
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

	DestroyAcceleratorTable(wnd_accel);
	DestroyMenu(wnd_menu);

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

      Supergumba -- testing
      
======================================================= */

void test_debug(char *str)
{
	FILE				*file;
	
	file=fopen("c:\\test_output.txt","a");
	if (file==NULL) return;
	
	fwrite(str,1,strlen(str),file);
	fwrite("\r\n",1,2,file);

	fclose(file);
}

/* =======================================================

      Setup Main
      
======================================================= */

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	hinst=hInstance;
	
		// initialize

	os_glue_start();
	
	if (!file_paths_setup(&file_path_setup)) {
		os_dialog_alert("Error","No data folder");
		os_glue_end();
		return(0);
	}

	if (!iface_initialize(&iface,&file_path_setup)) {
		os_dialog_alert("Error","Out of memory");
		os_glue_end();
		return(0);
	}

	iface_read(&iface);

		// open window

	win32_main_wind_open();

		// run setup
	
	setup_pump();

		// close window

	win32_main_wind_close();

		// save and close

	iface_write(&iface);
	iface_shutdown(&iface);

	os_glue_end();

	return(0);
}

