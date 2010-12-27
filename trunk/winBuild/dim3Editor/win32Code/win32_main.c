#include "dim3Editor.h"

#include "glue.h"
#include "interface.h"
#include "view.h"

#include "resource.h"

HINSTANCE					hinst;
ATOM						wnd_rg_class;
HFONT						fnt;
HWND						wnd;
HDC							wnd_gl_dc;
HGLRC						wnd_gl_ctx;

bool						quit;

map_type					map;
file_path_setup_type		file_path_setup;
editor_setup_type			setup;
editor_state_type			state;

extern d3rect				tool_palette_box,txt_palette_box;
extern list_palette_type	item_palette;

extern bool setup_xml_read(void);
extern void edit_view_draw(d3pnt *pt,d3ang *ang,d3rect *box,int wnd_high,bool focus);
void editor_button_down(int x,int y);
extern void glue_start(void);
extern void glue_end(void);
extern void editor_menu_commands(int id);
extern void editor_menu_create(void);
extern bool dialog_file_open_run(char *dialog_name,char *search_path,char *extension,char *required_file_name,char *file_name);
extern int os_win32_menu_lookup(int id);

/* =======================================================

      Supergumba -- temporary patches
      
======================================================= */

void launch_map_script_editor(void)
{
}
void launch_engine(void)
{
}
void dialog_about_run(void)
{
}
void dialog_preference_run(void)
{
}
bool dialog_map_settings_run(void)
{
	return(FALSE);
}
bool dialog_group_settings_run(group_type *group)
{
	return(FALSE);
}
bool dialog_movement_settings_run(int movement_idx)
{
	return(FALSE);
}
bool dialog_movement_move_settings_run(movement_move_type *move)
{
	return(FALSE);
}
bool dialog_cinema_settings_run(int cinema_idx)
{
	return(FALSE);
}
bool dialog_optimize_run(int *poly_threshold)
{
	return(FALSE);
}
bool dialog_resize_run(float *fct_x,float *fct_y,float *fct_z)
{
	return(FALSE);
}
bool dialog_reposition_run(d3pnt *min,d3pnt *max)
{
	return(FALSE);
}
bool dialog_free_rotate_run(float *rot_x,float *rot_y,float *rot_z)
{
	return(FALSE);
}
bool dialog_create_grid_mesh_run(int *xdiv,int *ydiv,int *zdiv)
{
	return(FALSE);
}
void dialog_texture_setting_run(int txt_idx)
{
}
bool dialog_height_import_run(int *div_cnt,int *size,int *high)
{
	return(FALSE);
}
bool dialog_map_auto_generate_setting_run(bool first)
{
	ag_build_setup_type		build_setup;

	build_setup.style_idx=0;
	build_setup.seed=5;
	build_setup.mirror=FALSE;
	build_setup.room_count=20;
	build_setup.room_sz=300;
	build_setup.room_high=60;
	build_setup.story_count=1;
	build_setup.merge_count=2;

	ag_generate_map(&build_setup);

	return(TRUE);
}




/* =======================================================

      UI Window Procedure
      
======================================================= */

LRESULT CALLBACK editor_wnd_proc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int					cmd;
	d3pnt				pnt;
	PAINTSTRUCT			ps;

	switch (msg) {

		case WM_PAINT:
			BeginPaint(wnd,&ps);
			main_wind_draw();
			EndPaint(wnd,&ps);
			break;

		case WM_SIZE:
			tool_palette_setup();
			texture_palette_setup();
			item_palette_setup();
			property_palette_setup();
			// supergumba -- deal with these, need to check if map is loaded
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
			pnt.x=LOWORD(lParam);
			pnt.y=HIWORD(lParam);

			main_wind_scroll_wheel(&pnt,(GET_WHEEL_DELTA_WPARAM(wParam)/60));
			break;

		case WM_KEYDOWN:
			main_wind_key_down((char)wParam);
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
			if (state.map_opened) {
				if (!menu_save_changes_dialog()) return(0);
				file_close_map();
			}
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
}

void win32_main_wind_open(void)
{
	int						format;
	WNDCLASSEX				wcx;
	PIXELFORMATDESCRIPTOR	pf;
	HINSTANCE				hInst;
	HMENU					menu;

		// create window

	hInst=GetModuleHandle(NULL);
 
    wcx.cbSize=sizeof(wcx);
    wcx.style=CS_DBLCLKS;
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

    wnd=CreateWindow("dim3EditorWindowClass","dim3 Editor",WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_MAXIMIZE,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,hInst,NULL);

		// menu

	menu=LoadMenu(hinst,MAKEINTRESOURCE(IDR_MAIN_MENU));
	SetMenu(wnd,menu);

	undo_initialize();
	menu_fix_enable();

		// create font for window

	fnt=CreateFont(-10,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,(DEFAULT_PITCH|FF_DONTCARE),"Monaco");

		// show window

	ShowWindow(wnd,SW_MAXIMIZE);

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

		// delete window

	DeleteObject(fnt);
	DestroyWindow(wnd);
	UnregisterClass("dim3EditorWindowClass",GetModuleHandle(NULL));
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
	hinst=hInstance;

	state.map_opened=FALSE;
	
		// glue start

	glue_start();
	
	if (!file_paths_setup(&file_path_setup)) {
		glue_end();
		MessageBox(NULL,"No data folder","Error",MB_OK);
		return(0);
	}
	
		// settings

	setup_xml_read();

		// open window

	win32_main_wind_open();

		// open map

	file_open_map();

	editor_pump();

		// close window

	win32_main_wind_close();

		// close glue

	glue_end();

	return(0);
}

