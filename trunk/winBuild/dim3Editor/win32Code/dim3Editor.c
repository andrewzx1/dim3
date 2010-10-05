#include "dim3Editor.h"

#include "walk_view.h"
#include "common_view.h"
#include "interface.h"

#define EDITOR_WIN_X						10
#define EDITOR_WIN_Y						40
#define EDITOR_WIN_WIDTH					1100
#define EDITOR_WIN_HEIGHT					700
#define EDITOR_WIN_EXTRA_HEIGHT				20

HINSTANCE				hinst;
ATOM					wnd_rg_class;
HFONT					fnt;
HWND					wnd;
HDC						wnd_gl_dc;
HGLRC					wnd_gl_ctx;

bool					quit;

map_type				map;
file_path_setup_type	file_path_setup;
setup_type				setup;
editor_state_type		state;

d3rect					main_wind_box;

extern d3rect			tool_palette_box,txt_palette_box;

extern bool setup_xml_read(void);
extern void edit_view_draw(d3pnt *pt,d3ang *ang,d3rect *box,int wnd_high,bool focus);
void editor_button_down(int x,int y);
void editor_draw(void);	// supergumba -- testing
extern void glue_start(void);
extern void glue_end(void);
extern void walk_view_draw(void);
extern bool walk_view_initialize(void);
extern void walk_view_shutdown(void);
extern void editor_menu_commands(int id);
extern void editor_menu_create(void);
extern bool dialog_file_open_run(char *dialog_name,char *search_path,char *extension,char *required_file_name,char *file_name);

/* =======================================================

      Supergumba -- temporary patches
      
======================================================= */

void menu_fix_enable(void) {}
void palette_reset(void) {}
bool main_wind_click_check_box(d3pnt *pt,d3rect *box)
{
	return(FALSE);
}
void dialog_texture_setting_run(int txt) {}
bool node_link_click(int node_idx)
{
	return(FALSE);
}
void menu_update_view(void) {}
void file_save_map(void) {}
void launch_map_script_editor(void) {}
void launch_engine(void) {}
bool dialog_resize_run(float *x,float *y,float *z)
{
	return(FALSE);
}
bool dialog_reposition_run(d3pnt *min,d3pnt *max)
{
	return(FALSE);
}
bool dialog_skew_run(int *axis,int *dir,int *size)
{
	return(FALSE);
}
bool dialog_free_rotate_run(float *x,float *y,float *z)
{
	return(FALSE);
}
bool import_load_file(char *path,char *ext)
{
	return(FALSE);
}
void dialog_progress_next(void) {}
void dialog_progress_start(char *title,int count) {}
void dialog_progress_end(void) {}
bool dialog_create_grid_mesh_run(int *xdiv,int *ydiv,int *zdiv)
{
	return(FALSE);
}
bool dialog_mesh_scale_run(d3fpnt *min,d3fpnt *max,bool replace_ok,float old_scale,float *scale)
{
	return(FALSE);
}
bool dialog_height_import_run(int *div_cnt,int *size,int *high)
{
	return(FALSE);
}
bool dialog_optimize_run(int *poly_threshold)
{
	return(FALSE);
}

/* =======================================================

      UI Window Procedure
      
======================================================= */

LRESULT CALLBACK editor_wnd_proc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int					delta;
	d3pnt				pnt;
	editor_view_type	*view;
	PAINTSTRUCT			ps;

	switch (msg) {

		case WM_PAINT:
			BeginPaint(wnd,&ps);
			editor_draw();
			EndPaint(wnd,&ps);
			break;

		case WM_SIZE:
			tool_palette_setup();
			texture_palette_setup();
			// supergumba -- deal with these, need to check if map is loaded, reset main_wind_box
			break;

		case WM_MOUSEMOVE:
			pnt.x=LOWORD(lParam);
			pnt.y=HIWORD(lParam);
			walk_view_cursor(&pnt);
			break;

		case WM_LBUTTONDOWN:
			pnt.x=LOWORD(lParam);
			pnt.y=HIWORD(lParam);
			
			SetCapture(wnd);

			if (pnt.y<tool_palette_box.by) {
				tool_palette_click(&pnt);
			}
			else {
				if (pnt.y>=txt_palette_box.ty) {
					texture_palette_click(&pnt,FALSE);
				}
				else {
					walk_view_click(&pnt,FALSE);
				}
			}

			ReleaseCapture();
			break;

		case WM_MOUSEWHEEL:
			view=walk_view_get_current_view();
			delta=GET_WHEEL_DELTA_WPARAM(wParam)/60;
			walk_view_scroll_wheel_z_movement(view,delta);
			break;

		case WM_CHAR:
			walk_view_key((char)wParam);
			break;

		case WM_COMMAND:
			editor_menu_commands(LOWORD(wParam));
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

    wnd=CreateWindow("dim3EditorWindowClass","dim3 Editor",WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,wbox.left,wbox.top,(wbox.right-wbox.left),(wbox.bottom-wbox.top),NULL,NULL,hInst,NULL);

		// menu

	editor_menu_create();

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
	pf.cAlphaBits=8;
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

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
		
	glHint(GL_TEXTURE_COMPRESSION_HINT,GL_NICEST);
	glHint(GL_GENERATE_MIPMAP_HINT,GL_NICEST);

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		// initial clear

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	SwapBuffers(wnd_gl_dc);

		// main wind box

	main_wind_box.lx=wbox.left;
	main_wind_box.rx=(wbox.right-wbox.left);
	main_wind_box.ty=wbox.top;
	main_wind_box.by=wbox.bottom-wbox.top;

		// initialize walk view

	tool_palette_initialize();
	tool_palette_setup();

	texture_palette_setup();
	
	walk_view_initialize();

	return(TRUE);
}

void editor_end(void)
{
	walk_view_shutdown();
	tool_palette_shutdown();

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
	char			file_name[256];
	d3pnt			pnt;

	if (!dialog_file_open_run("Open a Map","Maps","xml",NULL,file_name)) {
		strcpy(err_str,"no map!");
		return(FALSE);
	}

		// open map

	map_setup(&file_path_setup,anisotropic_mode_high,mipmap_mode_trilinear,texture_quality_mode_normal,FALSE);

	if (!map_open(&map,file_name)) {
		strcpy(err_str,"Could not open map");
		return(FALSE);
	}

	walk_view_models_start();
	walk_view_models_reset();

		// supergumba -- some temporary state

	state.show_liquid=TRUE;
	state.show_object=TRUE;
	state.show_lightsoundparticle=TRUE;
	state.drag_mode=drag_mode_mesh;

		// supergumba -- fake starting point

	pnt.x=346000;
	pnt.y=252000;
	pnt.z=354000;

	walk_view_setup_default_views();
	walk_view_set_position(&pnt);

	return(TRUE);
}

void editor_close_map(void)
{
	walk_view_models_close();

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

	tool_palette_draw();
	texture_palette_draw();

		// swap buffers

	editor_gl_swap();
}

void main_wind_draw(void)
{
	editor_draw();
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

      Editor Main
      
======================================================= */

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	char		err_str[256];

	hinst=hInstance;

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

	state.free_look=TRUE;		// supergumba -- lock on free look
	state.grid_mode=grid_mode_large;	// supergumba -- setup grid

	editor_pump();

	editor_end();
	editor_close_map();

	return(0);
}

