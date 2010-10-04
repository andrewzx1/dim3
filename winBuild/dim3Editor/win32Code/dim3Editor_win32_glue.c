#include "dim3Editor.h"

#include "common_view.h"

extern HWND				wnd;

HCURSOR					cur_arrow,cur_wait,cur_hand,cur_drag,cur_resize;

/* =======================================================

      Start/Stop Glue
      
======================================================= */

void glue_start(void)
{
		// load cursors

	cur_arrow=LoadCursor(NULL,IDC_ARROW);
	cur_wait=LoadCursor(NULL,IDC_WAIT);
	cur_hand=LoadCursor(NULL,IDC_HAND);
	cur_drag=LoadCursor(NULL,IDC_HAND);
	cur_resize=LoadCursor(NULL,IDC_SIZEALL);
}

void glue_end(void)
{
		// destroy cursors

	DestroyCursor(cur_arrow);
	DestroyCursor(cur_wait);
	DestroyCursor(cur_hand);
	DestroyCursor(cur_drag);
	DestroyCursor(cur_resize);
}

/* =======================================================

      Win32 Glue Routines
      
======================================================= */
		
void os_get_icon_file_path(char *path)
{
	strcpy(path,"dim3 Editor Icons");
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

void os_enable_menu_item_undo(bool enable)
{
	/* supergumba -- reactivate when we have menus

	HMENU			menu;

	menu=GetSubMenu([MAIN MENU HERE],menu_idx);

	if (enable) {
		EnableMenuItem(menu,0,MF_BYPOSITION|MF_ENABLED);
	}
	else {
		EnableMenuItem(menu,0,MF_BYPOSITION|MF_GRAYED);
	}
	*/
}

bool os_key_space_down(void)
{
	return(GetAsyncKeyState(VK_SPACE)!=0x0);
}

bool os_key_option_down(void)
{
	return(FALSE);
	return((GetAsyncKeyState(VK_LWIN)!=0x0)||(GetAsyncKeyState(VK_RWIN)!=0x0));
}

bool os_key_control_down(void)
{
	return(GetAsyncKeyState(VK_CONTROL)!=0x0);
}

bool os_key_command_down(void)
{
	return(GetAsyncKeyState(VK_MENU)!=0x0);
}

bool os_key_shift_down(void)
{
	return(GetAsyncKeyState(VK_SHIFT)!=0x0);
}

bool os_button_down(void)
{
	if (!GetSystemMetrics(SM_SWAPBUTTON)) return(GetAsyncKeyState(VK_LBUTTON)!=0);
	return(GetAsyncKeyState(VK_RBUTTON)!=0);
}

bool os_track_mouse_location(d3pnt *pt,d3rect *offset_box)
{
	POINT					ui_pt;

		// get cursor pos in window

	GetCursorPos(&ui_pt);
	ScreenToClient(wnd,&ui_pt);

		// add in offsets
	
	if (offset_box==NULL) {
		pt->x=ui_pt.x;
		pt->y=ui_pt.y;
	}
	else {
		pt->x=ui_pt.x-offset_box->lx;
		pt->y=ui_pt.y-offset_box->ty;
	}
	
	return(!os_button_down());
}

/* =======================================================

      Dialogs
      
======================================================= */

void dialog_alert(char *a,char *b)
{
	// supergumba -- temporary
}

// supergumba -- misc for now

void launch_map_script_editor(void) {}
void launch_engine(void) {}
void piece_create_spot(void) {}
void piece_create_light(void) {}
void piece_create_sound(void) {}
void piece_create_particle(void) {}
void piece_create_scenery(void) {}
void piece_create_node(void) {}
void piece_add_obj_mesh(void) {}
void piece_add_obj_mesh_uv(void) {}
void piece_add_height_map_mesh(void) {}
void piece_add_grid_mesh(void) {}
void piece_add_polygon_mesh(void) {}
void piece_create_liquid(void) {}
void piece_combine_mesh(void) {}
void piece_split_mesh(void) {}
void piece_tesselate(void) {}

