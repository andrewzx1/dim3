#include "dim3Editor.h"

#include "common_view.h"
#include "interface.h"

#define EDITOR_WIN_X			10
#define EDITOR_WIN_Y			40
#define EDITOR_WIN_WIDTH		1100
#define EDITOR_WIN_HEIGHT		700
#define EDITOR_WIN_EXTRA_HEIGHT	20

#define EDITOR_MOUSE_FUDGE		5

#define EDITOR_VIEW_LEFT		0
#define EDITOR_VIEW_FORWARD		1
#define EDITOR_VIEW_TOP			2
#define EDITOR_VIEW_RIGHT		3

ATOM					wnd_rg_class;
HFONT					fnt;
HWND					wnd;
HDC						wnd_gl_dc;
HGLRC					wnd_gl_ctx;

int						mouse_last_x,mouse_last_y,mouse_last_view;
bool					mouse_down,mouse_forward,quit;
d3pnt					view_pnt;

map_type				map;
file_path_setup_type	file_path_setup;
setup_type				setup;
editor_state_type		state;

d3rect					main_wind_box;

extern bool setup_xml_read(void);
extern void edit_view_draw(d3pnt *pt,d3ang *ang,d3rect *box,int wnd_high,bool focus);
void editor_button_down(int x,int y,bool forward);
void editor_button_up(int x,int y);
void editor_mouse_move(int x,int y);
void editor_draw(void);	// supergumba -- testing
extern void glue_start(void);
extern void glue_end(void);
extern void walk_view_draw(void);
extern bool walk_view_initialize(void);
extern void walk_view_shutdown(void);

/* =======================================================

      Supergumba -- temporary patches
      
======================================================= */

void menu_fix_enable(void)
{
}

void palette_reset(void)
{
}

bool main_wind_click_check_box(d3pnt *pt,d3rect *box)
{
	return(FALSE);
}

void piece_key(editor_3D_view_setup *view_setup,int view_move_dir,char ch)
{
}

void dialog_texture_setting_run(int txt)
{
}

bool node_link_click(int node_idx)
{
	return(FALSE);
}

/* =======================================================

      UI Window Procedure
      
======================================================= */

LRESULT CALLBACK editor_wnd_proc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
//	PAINTSTRUCT		ps;
//	HDC				dc;

	switch (msg) {

		case WM_PAINT:
//			editor_draw();
			break;

		case WM_LBUTTONDOWN:
			SetCapture(wnd);
			editor_button_down(LOWORD(lParam),HIWORD(lParam),((wParam&MK_CONTROL)!=0));
			break;

		case WM_LBUTTONUP:
			ReleaseCapture();
			editor_button_up(LOWORD(lParam),HIWORD(lParam));
			break;

		case WM_MOUSEMOVE:
		//	editor_mouse_move(LOWORD(lParam),HIWORD(lParam));
			break;

		case WM_CLOSE:
			quit=TRUE;
			break;

		default:
			return(DefWindowProc(hWnd,msg,wParam,lParam));

	}

	return(0);
}

/* =======================================================

      Start/Stop Window
      
======================================================= */

bool editor_start(char *err_str)
{
	int						format,glew_error;
	RECT					wbox;
	WNDCLASSEX				wcx;
	PIXELFORMATDESCRIPTOR	pf;
	HINSTANCE				hInst;

		// glue start

	glue_start();

		// settings

	setup_xml_read();

		// create window

	hInst=GetModuleHandle(NULL);
 
    wcx.cbSize=sizeof(wcx);
    wcx.style=0;
    wcx.lpfnWndProc=editor_wnd_proc;
    wcx.cbClsExtra=0;
    wcx.cbWndExtra=0;
    wcx.hInstance=hInst;
    wcx.hIcon=NULL;
    wcx.hCursor=LoadCursor(NULL,IDC_ARROW);
    wcx.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
    wcx.lpszMenuName=NULL;
    wcx.lpszClassName="dim3EditorWindowClass";
    wcx.hIconSm=NULL; 

    RegisterClassEx(&wcx); 

	wbox.left=EDITOR_WIN_X;
	wbox.right=EDITOR_WIN_X+EDITOR_WIN_WIDTH;
	wbox.top=EDITOR_WIN_Y;
	wbox.bottom=EDITOR_WIN_Y+EDITOR_WIN_HEIGHT+EDITOR_WIN_EXTRA_HEIGHT;

	AdjustWindowRect(&wbox,WS_OVERLAPPEDWINDOW,FALSE);

    wnd=CreateWindow("dim3EditorWindowClass","dim3 Server",WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,wbox.left,wbox.top,(wbox.right-wbox.left),(wbox.bottom-wbox.top),NULL,NULL,hInst,NULL);

		// create font for window

	fnt=CreateFont(-10,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,(DEFAULT_PITCH|FF_DONTCARE),"Monaco");

		// show window

    ShowWindow(wnd,SW_SHOW);

		// start opengl

	wnd_gl_dc=GetDC(wnd);
	
	bzero(&pf,sizeof(PIXELFORMATDESCRIPTOR));
	pf.nSize=sizeof(PIXELFORMATDESCRIPTOR);
	pf.nVersion=1;
	pf.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_GENERIC_ACCELERATED|PFD_DOUBLEBUFFER;
	pf.iPixelType=PFD_TYPE_RGBA;
	pf.cColorBits=24;
	pf.cDepthBits=16;
	
	format=ChoosePixelFormat(wnd_gl_dc,&pf);
	SetPixelFormat(wnd_gl_dc,format,&pf);

	wnd_gl_ctx=wglCreateContext(wnd_gl_dc);
	wglMakeCurrent(wnd_gl_dc,wnd_gl_ctx);

		// initialize glew

	glew_error=glewInit();
	if (glew_error!=GL_NO_ERROR) {
		strcpy(err_str,glewGetErrorString(glew_error));
		return(FALSE);
	}

		// default opengl setup
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

	glEnable(GL_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
		
	glHint(GL_TEXTURE_COMPRESSION_HINT,GL_NICEST);
	glHint(GL_GENERATE_MIPMAP_HINT,GL_NICEST);

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);

		// initial clear

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	SwapBuffers(wnd_gl_dc);

		// mouse setup

	mouse_down=FALSE;

		// main wind box

	main_wind_box.lx=0;
	main_wind_box.rx=(wbox.right-wbox.left)-palette_wid;
	main_wind_box.ty=toolbar_high;
	main_wind_box.by=(wbox.bottom-wbox.top)-info_high;

		// supergumba -- fake starting point

	view_pnt.x=346000;
	view_pnt.y=252000;
	view_pnt.z=354000;

		// initialize walk view

	texture_palette_setup();
	walk_view_initialize();

	return(TRUE);
}

void editor_end(void)
{
	walk_view_shutdown();

		// close opengl

	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(wnd_gl_ctx);
	ReleaseDC(wnd,wnd_gl_dc);

		// delete window

	DeleteObject(fnt);
	DestroyWindow(wnd);
	UnregisterClass("dim3ServerWindowClass",GetModuleHandle(NULL));

		// close glue

	glue_end();
}

/* =======================================================

      Editor Draw Utility
      
======================================================= */

void editor_gl_swap(void)
{
	SwapBuffers(wnd_gl_dc);
}

/* =======================================================

      Editor Message Pump
      
======================================================= */

void editor_pump(void)
{
	MSG			msg;

	quit=FALSE;

	while (!quit) {
		if (GetMessage(&msg,NULL,0,0)>0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

/* =======================================================

      Open/Close Map
      
======================================================= */

bool editor_setup(char *err_str)
{
	if (!file_paths_setup(&file_path_setup)) {
		strcpy(err_str,"No data folder");
		return(FALSE);
	}

	return(TRUE);
}

bool editor_open_map(char *err_str)
{
		// open map

	map_setup(&file_path_setup,anisotropic_mode_high,mipmap_mode_trilinear,texture_quality_mode_normal,FALSE,FALSE);

	if (!map_open(&map,"DM Map")) {
		strcpy(err_str,"Could not open map");
		return(FALSE);
	}

//	walk_view_models_start();

	return(TRUE);
}

void editor_close_map(void)
{
//	walk_view_models_close();

	map_close(&map);
}

/* =======================================================

      Testing
      
======================================================= */

void editor_clear_viewport(void)
{
	int				wid,high;
	RECT			wbox;

	GetClientRect(wnd,&wbox);

	wid=wbox.right-wbox.left;
	high=wbox.bottom-wbox.top;

	glScissor(0,0,wid,high);
	glViewport(0,0,wid,high);
}

void editor_draw(void)
{
	int				mx,my,wnd_high;
	RECT			wbox;

	GetClientRect(wnd,&wbox);

	mx=(wbox.left+wbox.right)/2;
	my=(wbox.top+wbox.bottom)/2;

	wnd_high=wbox.bottom;

		// clear draw buffer

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	editor_clear_viewport();

	glClearColor(0.75f,0.75f,0.75f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// draw editor

	walk_view_draw();

	texture_palette_draw();

		// swap buffers

	editor_gl_swap();
}

void main_wind_draw(void)
{
	editor_draw();
}

/* =======================================================

      Mouse Move
      
======================================================= */

void editor_button_down(int x,int y,bool forward)
{
	d3ang		ang;
	d3rect		box;
	d3pnt		pnt;

	mouse_down=TRUE;
	mouse_forward=forward;

	mouse_last_x=x;
	mouse_last_y=y;

		// find view clicked in

	pnt.x=x;
	pnt.y=y;

	walk_view_click(&pnt,FALSE);
}

void editor_button_up(int x,int y)
{
	mouse_down=FALSE;
}

void editor_mouse_move(int x,int y)
{
	int			cx,cy;

	if (!mouse_down) return;
	if ((mouse_last_x==x) && (mouse_last_y==y)) return;

		// move within editor fudge

	cx=x-mouse_last_x;
	if (abs(cx)<EDITOR_MOUSE_FUDGE) {
		cx=0;
	}
	else {
		mouse_last_x=x;
	}

	cx=cx/EDITOR_MOUSE_FUDGE;

	cy=y-mouse_last_y;
	if (abs(cy)<EDITOR_MOUSE_FUDGE) {
		cy=0;
	}
	else {
		mouse_last_y=y;
	}

	cy=cy/EDITOR_MOUSE_FUDGE;

		// move depending on view

	switch (mouse_last_view) {
		case EDITOR_VIEW_LEFT:
			view_pnt.z-=cx;
			if (!mouse_forward) {
				view_pnt.y-=cy;
			}
			else {
				view_pnt.x+=cy;
			}
			break;
		case EDITOR_VIEW_FORWARD:
			view_pnt.x-=cx;
			if (!mouse_forward) {
				view_pnt.y-=cy;
			}
			else {
				view_pnt.z-=cy;
			}
			break;
		case EDITOR_VIEW_TOP:
			view_pnt.x-=cx;
			if (!mouse_forward) {
				view_pnt.z-=cy;
			}
			else {
				view_pnt.y+=cy;
			}
			break;
		case EDITOR_VIEW_RIGHT:
			view_pnt.z+=cx;
			if (!mouse_forward) {
				view_pnt.y-=cy;
			}
			else {
				view_pnt.x-=cy;
			}
			break;
	}

	editor_draw();
}

/* =======================================================

      Editor Main
      
======================================================= */

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	char		err_str[256];

	if (!editor_setup(err_str)) {
		MessageBox(NULL,err_str,"Error",MB_OK);
		return(0);
	}

	if (!editor_start(err_str)) {
		editor_end();
		MessageBox(NULL,err_str,"Error",MB_OK);
		return(0);
	}

	if (!editor_open_map(err_str)) {
		editor_end();
		MessageBox(NULL,err_str,"Error",MB_OK);
		return(0);
	}




	editor_draw();




	editor_pump();
	editor_end();

	editor_close_map();

	return(0);
}

