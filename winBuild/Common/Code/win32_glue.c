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
extern HMENU			wnd_menu;
extern HACCEL			wnd_accel;

HCURSOR					cur_arrow,cur_wait,cur_hand,cur_drag,cur_resize,
						cur_add,cur_subtract;
COLORREF				custom_colors[16];
bool					os_dialog_quit;
HWND					os_dialog_wind;
HFONT					os_dialog_font;
os_dialog_callback_ptr	os_dialog_callback;

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

void os_menu_create(os_menu_item_type *os_menus)
{
	int						len,accel_count;
	char					last_menu_name[32],item_name[64];
	os_menu_item_type		*menu;
	HMENU					sub_menu;
	ACCEL					accels[64];

		// create the menu

	wnd_menu=CreateMenu();

	menu=os_menus;

	last_menu_name[0]=0x0;
	sub_menu=NULL;

	while (TRUE) {
		if (menu->menu_name[0]==0x0) break;

			// switching to a new menu

		if (strcmp(menu->menu_name,last_menu_name)!=0) {
			if (sub_menu!=NULL) AppendMenu(wnd_menu,(MF_STRING|MF_POPUP),(UINT)sub_menu,last_menu_name);
			strcpy(last_menu_name,menu->menu_name);
			sub_menu=CreatePopupMenu();
		}

			// add item

		if (menu->item_name[0]!=0x0) {

				// add in ctrl/alt/shift

			strcpy(item_name,menu->item_name);

			switch (menu->key_type) {

				case os_menu_key_cmd:
					strcat(item_name,"\tCtrl+");
					len=strlen(item_name);
					item_name[len]=menu->key;
					item_name[len+1]=0x0;
					break;

				case os_menu_key_cmd_opt:
					strcat(item_name,"\tCtrl+Alt+");
					len=strlen(item_name);
					item_name[len]=menu->key;
					item_name[len+1]=0x0;
					break;

				case os_menu_key_cmd_shift:
					strcat(item_name,"\tCtrl+Shift+");
					len=strlen(item_name);
					item_name[len]=menu->key;
					item_name[len+1]=0x0;
					break;

			}

			AppendMenu(sub_menu,MF_STRING,menu->id,item_name);
		}
		else {
			AppendMenu(sub_menu,MF_SEPARATOR,0,"");
		}

			// next menu item

		menu++;
	}

	if (sub_menu!=NULL) AppendMenu(wnd_menu,(MF_STRING|MF_POPUP),(UINT)sub_menu,last_menu_name);

		// set the menu

	SetMenu(wnd,wnd_menu);

		// create the accelerators

	accel_count=0;
	menu=os_menus;

	while (TRUE) {
		if (menu->menu_name[0]==0x0) break;

		switch (menu->key_type) {

			case os_menu_key_cmd:
				accels[accel_count].fVirt=FVIRTKEY|FCONTROL;
				accels[accel_count].key=menu->key;
				accels[accel_count].cmd=menu->id;
				accel_count++;
				break;

			case os_menu_key_cmd_opt:
				accels[accel_count].fVirt=FVIRTKEY|FCONTROL|FALT;
				accels[accel_count].key=menu->key;
				accels[accel_count].cmd=menu->id;
				accel_count++;
				break;

			case os_menu_key_cmd_shift:
				accels[accel_count].fVirt=FVIRTKEY|FCONTROL|FSHIFT;
				accels[accel_count].key=menu->key;
				accels[accel_count].cmd=menu->id;
				accel_count++;
				break;

		}

		menu++;
	}

	wnd_accel=CreateAcceleratorTable(accels,accel_count);
}

void os_menu_dispose(void)
{
	DestroyAcceleratorTable(wnd_accel);
	DestroyMenu(wnd_menu);
}

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

/* =======================================================

      Dialogs
      
======================================================= */

LRESULT CALLBACK os_dialog_proc(HWND diag,UINT msg,WPARAM wparam,LPARAM lparam)
{
	LPNMHDR			hdr;

	switch (msg) {

		case WM_INITDIALOG:
			(*os_dialog_callback)(os_dialog_msg_type_init,0);
			return(FALSE);		// we set the focus

		case WM_COMMAND:
			if (HIWORD(wparam)==BN_CLICKED) {
				(*os_dialog_callback)(os_dialog_msg_type_button,(int)LOWORD(wparam));
				return(TRUE);
			}
			break;

		case WM_NOTIFY:
			hdr=(LPNMHDR)lparam;
			if (hdr->code==TVN_SELCHANGED) {
				(*os_dialog_callback)(os_dialog_msg_type_sel_change,hdr->idFrom);
				return(TRUE);
			}
			if (hdr->code==NM_DBLCLK) {
				(*os_dialog_callback)(os_dialog_msg_type_double_click,hdr->idFrom);
				return(TRUE);
			}
			break;

	}

	return(DefDlgProc(diag,msg,wparam,lparam));
}

void os_dialog_run(char *title,int wid,int high,os_dialog_ctrl_type *ctrls,void *callback)
{
	int					x,y,m_wid,m_high,font_high;
	RECT				box;
	os_dialog_ctrl_type	*ctrl;
	MSG					msg;
	HWND				ctrl_wnd;
	WNDCLASSEX			wcx;

		// setup callback

	os_dialog_callback=callback;

		// get x,y of dialog

	m_wid=GetSystemMetrics(SM_CXSCREEN);
	m_high=GetSystemMetrics(SM_CYSCREEN);

	x=(m_wid/2)-(wid/2);
	y=(m_high/2)-(high/2);

		// enlarge size for framing
		// elements

	box.left=x;
	box.top=y;
	box.right=box.left+wid;
	box.bottom=box.top+high;

	AdjustWindowRectEx(&box,WS_CAPTION,FALSE,WS_EX_DLGMODALFRAME);

	x=box.left;
	y=box.top;
	wid=box.right-x;
	high=box.bottom-y;

		// create window

    wcx.cbSize=sizeof(wcx);
    wcx.style=CS_DBLCLKS;
    wcx.lpfnWndProc=os_dialog_proc;
    wcx.cbClsExtra=0;
    wcx.cbWndExtra=DLGWINDOWEXTRA;
    wcx.hInstance=hinst;
    wcx.hIcon=NULL;
    wcx.hCursor=LoadCursor(NULL,IDC_ARROW);
    wcx.hbrBackground=(HBRUSH)CreateSolidBrush(RGB(240,240,240));
    wcx.lpszMenuName=NULL;
    wcx.lpszClassName="dim3DialogClass";
    wcx.hIconSm=NULL;

    RegisterClassEx(&wcx);

    os_dialog_wind=CreateWindowEx(WS_EX_DLGMODALFRAME|WS_EX_NOPARENTNOTIFY,"dim3DialogClass",title,WS_POPUP|WS_CAPTION,x,y,wid,high,wnd,NULL,hinst,NULL);

		// disable main window

	EnableWindow(wnd,FALSE);

		// window font

	font_high=-MulDiv(8,GetDeviceCaps(GetDC(os_dialog_wind),LOGPIXELSY),72);
	os_dialog_font=CreateFont(font_high,0,0,0,400,0,0,0,0x1,0,0,0,0,"MS Shell Dlg");

		// add in the controls

	ctrl=ctrls;

	while (TRUE) {
		if (ctrl->type==-1) break;

		switch (ctrl->type) {

			case os_dialog_ctrl_type_button:
				ctrl_wnd=CreateWindowEx(0,"BUTTON",ctrl->str,BS_PUSHBUTTON|BS_TEXT|WS_CHILD|WS_VISIBLE,ctrl->x,ctrl->y,ctrl->wid,ctrl->high,os_dialog_wind,(HMENU)ctrl->id,hinst,NULL);
				break;

			case os_dialog_ctrl_type_default_button:
				ctrl_wnd=CreateWindowEx(0,"BUTTON",ctrl->str,BS_DEFPUSHBUTTON|BS_TEXT|WS_CHILD|WS_VISIBLE,ctrl->x,ctrl->y,ctrl->wid,ctrl->high,os_dialog_wind,(HMENU)ctrl->id,hinst,NULL);
				break;

			case os_dialog_ctrl_type_text_left:
				ctrl_wnd=CreateWindowEx(0,"STATIC",ctrl->str,WS_CHILD|WS_VISIBLE,ctrl->x,ctrl->y,ctrl->wid,ctrl->high,os_dialog_wind,(HMENU)ctrl->id,hinst,NULL);
				break;

			case os_dialog_ctrl_type_text_right:
				ctrl_wnd=CreateWindowEx(WS_EX_RIGHT,"STATIC",ctrl->str,WS_CHILD|WS_VISIBLE,ctrl->x,ctrl->y,ctrl->wid,ctrl->high,os_dialog_wind,(HMENU)ctrl->id,hinst,NULL);
				break;

			case os_dialog_ctrl_type_text_edit:
				ctrl_wnd=CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT",ctrl->str,ES_LEFT|ES_AUTOHSCROLL|WS_CHILD|WS_VISIBLE,ctrl->x,ctrl->y,ctrl->wid,ctrl->high,os_dialog_wind,(HMENU)ctrl->id,hinst,NULL);
				break;

			case os_dialog_ctrl_type_files:
				ctrl_wnd=CreateWindowEx(WS_EX_CLIENTEDGE,WC_TREEVIEW,ctrl->str,TVS_HASBUTTONS|TVS_HASLINES|WS_CHILD|WS_VISIBLE,ctrl->x,ctrl->y,ctrl->wid,ctrl->high,os_dialog_wind,(HMENU)ctrl->id,hinst,NULL);
				break;
	
		}

			// set font

		SendMessage(ctrl_wnd,WM_SETFONT,(WPARAM)os_dialog_font,0);

		ctrl++;
	}

		// init dialog message

	SendMessage(os_dialog_wind,WM_INITDIALOG,0,0);

		// show window

	ShowWindow(os_dialog_wind,SW_SHOWNORMAL);

		// run dialog

	os_dialog_quit=FALSE;

	while (!os_dialog_quit) {
		if (GetMessage(&msg,NULL,0,0)>0) {
			if (!IsDialogMessage(os_dialog_wind,&msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

		// return window states and close
		// dialog

	EnableWindow(wnd,TRUE);

	DestroyWindow(os_dialog_wind);
	UnregisterClass("dim3DialogClass",GetModuleHandle(NULL));

	DeleteObject(os_dialog_font);
}

void os_dialog_close(void)
{
	os_dialog_quit=TRUE;
}

void os_dialog_set_text(int id,char *value)
{
	SendDlgItemMessage(os_dialog_wind,id,WM_SETTEXT,0,(LPARAM)value);
}

void os_dialog_get_text(int id,char *value,int value_len)
{
	char			str[256];

	SendDlgItemMessage(os_dialog_wind,id,WM_GETTEXT,256,(LPARAM)str);

	strncpy(value,str,value_len);
	value[value_len-1]=0x0;
}

extern void os_dialog_set_float(int id,float f)
{
	char			str[256];

	sprintf(str,"%.2f",f);
	os_dialog_set_text(id,str);
}

float os_dialog_get_float(int id)
{
	char			str[256];

	os_dialog_get_text(id,str,256);
	return((float)atof(str));
}

HTREEITEM os_dialog_add_tree_recursive(int id,file_path_directory_type *fpd,HTREEITEM parent_item,int parent_idx)
{
	int					n;
	TV_INSERTSTRUCT		tv_item;
	HTREEITEM			item,first_item;

	first_item=NULL;

	for (n=0;n!=fpd->nfile;n++) {
		if (fpd->files[n].parent_idx==parent_idx) {
			tv_item.hParent=parent_item;
			tv_item.hInsertAfter=TVI_LAST;
			tv_item.item.mask=TVIF_TEXT|TVIF_PARAM;
			tv_item.item.pszText=fpd->files[n].file_name;
			tv_item.item.lParam=(fpd->files[n].is_dir?0xFFFF0000:0x0)|(n&0xFFFF);
			item=(HTREEITEM)SendMessage(GetDlgItem(os_dialog_wind,id),TVM_INSERTITEM,0,(LPARAM)&tv_item);

			if (first_item==NULL) first_item=item;
			os_dialog_add_tree_recursive(id,fpd,item,n);
		}
	}

	return(first_item);
}

void os_dialog_tree_add(int id,file_path_directory_type *fpd)
{
	HTREEITEM			first_item;

	first_item=os_dialog_add_tree_recursive(id,fpd,NULL,-1);
	if (first_item!=NULL) SendMessage(GetDlgItem(os_dialog_wind,id),TVM_EXPAND,TVE_EXPAND,(LPARAM)first_item);
}

int os_dialog_tree_get_value(int id)
{
	HTREEITEM			item;
	TV_ITEM				info;

	item=(HTREEITEM)SendMessage(GetDlgItem(os_dialog_wind,id),TVM_GETNEXTITEM,TVGN_CARET,(LPARAM)0);
	if (item==NULL) return(-1);

	info.mask=TVIF_PARAM;
	info.hItem=item;

	SendMessage(GetDlgItem(os_dialog_wind,id),TVM_GETITEM,0,(LPARAM)&info);

	return(info.lParam);
}

void os_dialog_set_focus(int id,bool select_all)
{
	SetFocus(GetDlgItem(os_dialog_wind,id));
	if (select_all) SendMessage(GetDlgItem(os_dialog_wind,id),EM_SETSEL,0,-1);
}

void os_dialog_enable(int id,bool enable)
{
	EnableWindow(GetDlgItem(os_dialog_wind,id),enable);
}

