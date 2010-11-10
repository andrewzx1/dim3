#include "dim3Editor.h"

#include "glue.h"
#include "interface.h"
#include "view.h"

#include "resource.h"

#define EDITOR_WIN_X						10
#define EDITOR_WIN_Y						40
#define EDITOR_WIN_WIDTH					1200
#define EDITOR_WIN_HEIGHT					800
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
editor_setup_type		setup;
editor_state_type		state;

d3rect					main_wind_box;

extern d3rect			tool_palette_box,txt_palette_box,item_palette_box;

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

void palette_reset(void)
{
}
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
bool dialog_file_new_run(char *title,char *file_name)
{
	return(FALSE);
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
bool dialog_light_map_run(void)
{
	return(TRUE);
}
bool dialog_resize_run(float *fct_x,float *fct_y,float *fct_z)
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
	int					cmd,delta;
	d3pnt				pnt;
	editor_view_type	*view;
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
			// supergumba -- deal with these, need to check if map is loaded, reset main_wind_box
			break;

		case WM_MOUSEMOVE:
			pnt.x=LOWORD(lParam);
			pnt.y=HIWORD(lParam);
			view_cursor(&pnt);
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			pnt.x=LOWORD(lParam);
			pnt.y=HIWORD(lParam);
			
			SetCapture(wnd);

			if (pnt.y<tool_palette_box.by) {
				tool_palette_click(&pnt);
			}
			else {
				if (pnt.y>=txt_palette_box.ty) {
					texture_palette_click(map.textures,&pnt,(msg==WM_LBUTTONDBLCLK));
				}
				else {
					if (pnt.x>=item_palette_box.lx) {
						item_palette_click(&pnt,(msg==WM_LBUTTONDBLCLK));
					}
					else {
						view_click(&pnt,FALSE);
					}
				}
			}

			ReleaseCapture();
			break;

		case WM_MOUSEWHEEL:

				// scroll wheel in item palette

			if ((pnt.y>=tool_palette_box.by) && (pnt.y<txt_palette_box.ty) && (pnt.x>=item_palette_box.lx)) {
				item_palette_scroll_wheel(&pnt,GET_WHEEL_DELTA_WPARAM(wParam));
				break;
			}

				// scroll wheel in view

			view=view_get_current_view();
			delta=GET_WHEEL_DELTA_WPARAM(wParam)/60;
			view_scroll_wheel_z_movement(view,delta);
			break;

		case WM_CHAR:
			view_key((char)wParam);
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
	int						format;
	RECT					wbox;
	WNDCLASSEX				wcx;
	PIXELFORMATDESCRIPTOR	pf;
	HINSTANCE				hInst;
	HMENU					menu;

		// settings

	setup_xml_read();

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

	wbox.left=EDITOR_WIN_X;
	wbox.right=EDITOR_WIN_X+EDITOR_WIN_WIDTH;
	wbox.top=EDITOR_WIN_Y;
	wbox.bottom=EDITOR_WIN_Y+EDITOR_WIN_HEIGHT+EDITOR_WIN_EXTRA_HEIGHT;

	AdjustWindowRect(&wbox,WS_OVERLAPPEDWINDOW,FALSE);

    wnd=CreateWindow("dim3EditorWindowClass","dim3 Editor",WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,wbox.left,wbox.top,(wbox.right-wbox.left),(wbox.bottom-wbox.top),NULL,NULL,hInst,NULL);

		// menu

	menu=LoadMenu(hinst,MAKEINTRESOURCE(IDR_MAIN_MENU));
	SetMenu(wnd,menu);

	undo_initialize();
	menu_fix_enable();

		// create font for window

	fnt=CreateFont(-10,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,(DEFAULT_PITCH|FF_DONTCARE),"Monaco");

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

		// text

	text_initialize();

		// initialize walk view

	tool_palette_initialize("Editor");
	tool_palette_setup();

	texture_palette_setup();

	item_palette_initialize();
	item_palette_setup();
	
	view_initialize();
}

void main_wind_close(void)
{
	view_shutdown();
	item_palette_shutdown();
	tool_palette_shutdown();
	text_shutdown();

		// close opengl

	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(wnd_gl_ctx);
	ReleaseDC(wnd,wnd_gl_dc);

		// delete window

	DeleteObject(fnt);
	DestroyWindow(wnd);
	UnregisterClass("dim3ServerWindowClass",GetModuleHandle(NULL));
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

	view_models_start();
	view_models_reset();

	state.map_opened=TRUE;

	menu_fix_enable();

	view_setup_default_views();
	view_set_position(&pnt);

	return(TRUE);
}

void editor_close_map(void)
{
	view_models_close();

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

void main_wind_draw(void)
{
		// clear draw buffer

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	editor_clear_viewport();

	glClearColor(0.75f,0.75f,0.75f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// draw window

	if (state.map_opened) {
		view_draw();

		tool_palette_draw();
		texture_palette_draw(map.textures);
		item_palette_draw();
	}

		// swap buffers

	SwapBuffers(wnd_gl_dc);
}

void main_wind_draw_no_swap(void)
{
		// clear draw buffer

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	editor_clear_viewport();

	glClearColor(0.75f,0.75f,0.75f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// draw window

	if (state.map_opened) {
		view_draw();

		tool_palette_draw();
		texture_palette_draw(map.textures);
		item_palette_draw();
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

	if (!file_open_map()) {
		glue_end();
		return(0);
	}

	editor_pump();

		// close glue

	glue_end();

	return(0);
}

