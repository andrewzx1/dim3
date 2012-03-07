/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: win32 Glue Routines

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

#ifdef D3_EDITOR
	#include "dim3editor.h"
#endif
#ifdef D3_ANIMATOR
	#include "dim3Animator.h"
#endif
#ifdef D3_SETUP
	#include "dim3Setup.h"
#endif

#include "glue.h"

extern bool				quit;

extern HINSTANCE		hinst;
extern HWND				wnd;
extern HDC				wnd_gl_dc;

HCURSOR					cur_arrow,cur_wait,cur_hand,cur_drag,cur_resize,
						cur_add,cur_subtract;
COLORREF				custom_colors[16];

/* =======================================================

      Start/Stop Glue
      
======================================================= */

void os_glue_start(void)
{
	int				n;

		// initial com for shell

	CoInitializeEx(NULL,(COINIT_APARTMENTTHREADED|COINIT_DISABLE_OLE1DDE));

		// load cursors

	cur_arrow=LoadCursor(NULL,IDC_ARROW);
	cur_wait=LoadCursor(NULL,IDC_WAIT);
	cur_hand=LoadCursor(NULL,IDC_HAND);
	cur_drag=LoadCursor(NULL,IDC_HAND);
	cur_resize=LoadCursor(NULL,IDC_SIZEALL);
	cur_add=LoadCursor(NULL,IDC_CROSS);
	cur_subtract=LoadCursor(NULL,IDC_NO);

		// custom colors

	for (n=0;n!=16;n++) {
		custom_colors[n]=RGB(0,0,0);
	}
}

void os_glue_end(void)
{
		// destroy cursors

	DestroyCursor(cur_arrow);
	DestroyCursor(cur_wait);
	DestroyCursor(cur_hand);
	DestroyCursor(cur_drag);
	DestroyCursor(cur_resize);
	DestroyCursor(cur_add);
	DestroyCursor(cur_subtract);

		// unload com

	CoUninitialize();
}

/* =======================================================

      Files
      
======================================================= */
		
void os_get_support_file_path(char *path,char *app_name)
{
	sprintf(path,"dim3 App Support\\%s",app_name);
}

void os_create_directory(char *path)
{
	CreateDirectory(path,NULL);
}

/* =======================================================

      Windows
      
======================================================= */

void os_application_quit(void)
{
	quit=TRUE;
}

void os_get_window_box(d3rect *box)
{
	RECT			wbox;

	GetClientRect(wnd,&wbox);

	box->lx=wbox.left;
	box->rx=wbox.right;
	box->ty=wbox.top;
	box->by=wbox.bottom;
}

void os_select_window(void)
{
    ShowWindow(wnd,SW_SHOW);
}

void os_set_title_window(char *title)
{
	SetWindowText(wnd,title);
}

void os_swap_gl_buffer(void)
{
	SwapBuffers(wnd_gl_dc);
}

/* =======================================================

      Cursors
      
======================================================= */

void os_set_arrow_cursor(void)
{
	SetCursor(cur_arrow);
}

void os_set_wait_cursor(void)
{
	SetCursor(cur_wait);
}

void os_set_hand_cursor(void)
{
	SetCursor(cur_hand);
}

void os_set_drag_cursor(void)
{
	SetCursor(cur_drag);
}

void os_set_resize_cursor(void)
{
	SetCursor(cur_resize);
}

void os_set_add_cursor(void)
{
	SetCursor(cur_add);
}

void os_set_subtract_cursor(void)
{
	SetCursor(cur_subtract);
}

/* =======================================================

      Menus
      
======================================================= */

void os_menu_enable_item(int menu_idx,int item_idx,bool enable)
{
	HMENU			menu;

		// enabling entire menu

	if (item_idx==0) {
		if (enable) {
			EnableMenuItem(GetMenu(wnd),(menu_idx-129),MF_BYPOSITION|MF_ENABLED);
		}
		else {
			EnableMenuItem(GetMenu(wnd),(menu_idx-129),MF_BYPOSITION|MF_GRAYED);
		}
		DrawMenuBar(wnd);
		return;
	}

		// enabling single item

	menu=GetSubMenu(GetMenu(wnd),(menu_idx-129));

	if (enable) {
		EnableMenuItem(menu,(item_idx-1),MF_BYPOSITION|MF_ENABLED);
	}
	else {
		EnableMenuItem(menu,(item_idx-1),MF_BYPOSITION|MF_GRAYED);
	}
}

void os_menu_check_item(int menu_idx,int item_idx,bool check)
{
	HMENU			menu;

	menu=GetSubMenu(GetMenu(wnd),(menu_idx-129));

	if (check) {
		CheckMenuItem(menu,(item_idx-1),MF_BYPOSITION|MF_CHECKED);
	}
	else {
		CheckMenuItem(menu,(item_idx-1),MF_BYPOSITION|MF_UNCHECKED);
	}
}

/* =======================================================

      Keyboard
      
======================================================= */

bool os_key_space_down(void)
{
	return((GetAsyncKeyState(VK_SPACE)&0x8000)!=0x0);
}

bool os_key_option_down(void)
{
	return(((GetAsyncKeyState(VK_LWIN)&0x8000)!=0x0)||((GetAsyncKeyState(VK_RWIN)&0x8000)!=0x0));
}

bool os_key_control_down(void)
{
	return((GetAsyncKeyState(VK_CONTROL)&0x8000)!=0x0);
}

bool os_key_command_down(void)
{
	return((GetAsyncKeyState(VK_MENU)&0x8000)!=0x0);
}

bool os_key_shift_down(void)
{
	return(((GetAsyncKeyState(VK_SHIFT)&0x8000)!=0x0)||((GetAsyncKeyState(VK_CAPITAL)&0x8000)!=0x0));
}

/* =======================================================

      Mouse
      
======================================================= */

void os_get_cursor(d3pnt *pnt)
{
	POINT			ui_pt;

	GetCursorPos(&ui_pt);
	ScreenToClient(wnd,&ui_pt);
	
	pnt->x=ui_pt.x;
	pnt->y=ui_pt.y;
}

bool os_button_down(void)
{
	if (!GetSystemMetrics(SM_SWAPBUTTON)) return(GetAsyncKeyState(VK_LBUTTON)!=0);
	return(GetAsyncKeyState(VK_RBUTTON)!=0);
}

bool os_track_mouse_location(d3pnt *pnt,d3rect *offset_box)
{
	POINT			ui_pt;

		// get cursor pos in window

	GetCursorPos(&ui_pt);
	ScreenToClient(wnd,&ui_pt);

		// add in offsets
	
	if (offset_box==NULL) {
		pnt->x=ui_pt.x;
		pnt->y=ui_pt.y;
	}
	else {
		pnt->x=ui_pt.x-offset_box->lx;
		pnt->y=ui_pt.y-offset_box->ty;
	}
	
	return(!os_button_down());
}

/* =======================================================

      Dialog Alerts and Messages
      
======================================================= */

void os_dialog_alert(char *title,char *msg)
{
	MessageBox(NULL,msg,title,MB_ICONINFORMATION|MB_OK);
}

int os_dialog_confirm(char *title,char *msg,bool include_cancel)
{
	int			rtn;
	
	if (include_cancel) {
		rtn=MessageBox(NULL,msg,title,MB_ICONINFORMATION|MB_YESNOCANCEL);
	}
	else {
		rtn=MessageBox(NULL,msg,title,MB_ICONINFORMATION|MB_YESNO);
	}

	if (rtn==IDYES) return(0);
	if (rtn==IDNO) return(2);

	return(1);
}

/* =======================================================

      Open Files
      
======================================================= */

bool os_load_file(char *title,char *path,char *ext)
{
	char			filter[256],file_title[256],ext2[8],
					initial_dir[1024],*c;
	OPENFILENAME	ofn;

	GetModuleFileName(NULL,initial_dir,1024);

	if (initial_dir[0]=='\"') {				// parse out the quotes
		strcpy(initial_dir,(char*)&initial_dir[1]);
		c=strchr(initial_dir,'\"');
		if (c!=NULL) *c=0x0;
	}

	c=strrchr(initial_dir,'\\');
	if (c!=0x0) *(c+1)=0x0;

		// setup call

	ext2[0]=toupper(ext[0]);
	ext2[1]=toupper(ext[1]);
	ext2[2]=toupper(ext[2]);
	ext2[3]=0x0;

	sprintf(filter,"%s Files\0*.%s\0\0",ext2,ext);

	file_title[0]=0x0;
	sprintf(path,"*.%s",ext);

		// get file

	ofn.lStructSize=sizeof(OPENFILENAME);
	ofn.hwndOwner=wnd;
	ofn.hInstance=hinst;
	ofn.lpstrFilter=filter;
	ofn.lpstrCustomFilter=NULL;
	ofn.nMaxCustFilter=0;
	ofn.nFilterIndex=1;
	ofn.lpstrFile=path;
	ofn.nMaxFile=1024;
	ofn.lpstrFileTitle=file_title;
	ofn.nMaxFileTitle=256;
	ofn.lpstrInitialDir=initial_dir;
	ofn.lpstrTitle=title;
	ofn.Flags=OFN_HIDEREADONLY|OFN_LONGNAMES|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST;
	ofn.nFileOffset=0;
	ofn.nFileExtension=0;
	ofn.lpstrDefExt=ext;
	ofn.lCustData=(LPARAM)NULL;
	ofn.lpfnHook=NULL;
	ofn.lpTemplateName=NULL;

	if (!GetOpenFileName(&ofn)) return(FALSE);

		// check for proper extension

	c=strrchr(path,'.');
	if (c!=NULL) {
		if (strcasecmp((c+1),ext)!=0) {
			MessageBox(wnd,"Please pick a file of the correct type.","Wrong File Type",MB_OK);
			return(FALSE);
		}
	}

	return(TRUE);
}

/* =======================================================

      Color Picker
      
======================================================= */

void os_pick_color(d3col *col)
{
	CHOOSECOLOR			cc;

	bzero(&cc,sizeof(CHOOSECOLOR));
	cc.lStructSize=sizeof(CHOOSECOLOR);
	cc.hwndOwner=wnd;
	cc.lpCustColors=(LPDWORD)custom_colors;
	cc.rgbResult=RGB((int)(col->r*255.0f),(int)(col->g*255.0f),(int)(col->b*255.0f));
	cc.Flags=CC_ANYCOLOR|CC_FULLOPEN|CC_SOLIDCOLOR|CC_RGBINIT;

	if (ChooseColor(&cc)) {
		col->r=((float)GetRValue(cc.rgbResult))/255.0f;
		col->g=((float)GetGValue(cc.rgbResult))/255.0f;
		col->b=((float)GetBValue(cc.rgbResult))/255.0f;
	}
}

/* =======================================================

      Launch Process
      
======================================================= */

bool os_launch_process(char *path,bool text_editor)
{
	int			n,len,err;
	char		*c,path2[1024];

		// we need to flip any / to \

	len=strlen(path);
	c=path;

	for (n=0;n!=len;n++) {
		if (*c=='/') *c='\\';
		c++;
	}

		// text edit or run link

	if (text_editor) {
		sprintf(path2,"\"%s\"",path);
		err=(int)ShellExecute(wnd,"open","wordpad.exe",path2,NULL,SW_SHOW);
	}
	else {
		err=(int)ShellExecute(wnd,"open",path,NULL,NULL,SW_SHOW);
	}

	return(err>32);
}
