#include "dim3Editor.h"

#include "common_view.h"

	// remapping for menu items

int				win32_menu_remap[][2]={
					{kCommandFileNew,100},
					{kCommandFileOpen,101},
					{kCommandFileClose,102},
					{kCommandFileSave,103},
					{kCommandFileAutoGenerateMap,104},
					{kCommandFileQuit,105},

					{kCommandEditUndo,200},
					{kCommandEditDelete,201},
					{kCommandEditDuplicate,202},
					{kCommandEditSelectMore,203},

					{kCommandViewFront,300},
					{kCommandViewLeft,301},
					{kCommandViewRight,302},
					{kCommandViewBack,303},
					{kCommandViewTop,304},
					{kCommandViewBottom,305},
					{kCommandViewPerspective,306},
					{kCommandViewOrtho,307},
					{kCommandViewUVLayer1,308},
					{kCommandViewUVLayer2,309},
					{kCommandViewGotoSelect,310},
					{kCommandViewGotoMapCenter,311},
					{kCommandViewClip,312},
					{kCommandViewShowHideLiquids,313},
					{kCommandViewShowHideSpots,314},
					{kCommandViewShowHideLights,315},
					{kCommandViewShowHideNodes,316},
					{kCommandViewSplitHorizontal,317},
					{kCommandViewSplitVertical,318},
					{kCommandViewRemoveSplit,319},

					{kCommandMapSettings,400},
					{kCommandMapRaiseY,401},
					{kCommandMapLowerY,402},
					{kCommandMapCenter,403},
					{kCommandMapResetUV,404},
					{kCommandMapOptimize,405},
					{kCommandMapCreateNormals,406},
					{kCommandClearLightMaps,407},
					{kCommandBuildLightMaps,408},
					{kCommandRun,409},

					{kCommandMeshCombine,500},
					{kCommandMeshSplit,501},
					{kCommandMeshTesselate,502},
					{kCommandMeshResize,503},
					{kCommandMeshReposition,504},
					{kCommandMeshSkew,505},
					{kCommandMeshFlipX,506},
					{kCommandMeshFlipY,507},
					{kCommandMeshFlipZ,508},
					{kCommandMeshRotateX,509},
					{kCommandMeshRotateY,510},
					{kCommandMeshRotateZ,511},
					{kCommandMeshFreeRotate,512},
					{kCommandMeshRaiseY,513},
					{kCommandMeshLowerY,514},
					{kCommandMeshSelectAllPoly,515},
					{kCommandMeshSnapToGrid,516},
					{kCommandMeshSnapClosestVertex,517},
					{kCommandMeshResetUV,518},
					{kCommandMeshWholeUV,519},
					{kCommandMeshSingleUV,520},
					{kCommandMeshCreateNormals,521},
					{kCommandMeshInvertNormals,522},

					{kCommandPolygonHole,600},
					{kCommandPolygonSnapToGrid,601},
					{kCommandPolygonRotateUV,602},
					{kCommandPolygonFlipU,603},
					{kCommandPolygonFlipV,604},
					{kCommandPolygonInvertNormal,605},
					{kCommandPolygonResetUV,606},
					{kCommandPolygonWholeUV,607},
					{kCommandPolygonSingleUV,608},

					{kCommandVertexSnapToGrid,700},

					{kCommandGroups,800},
					{kCommandGroupMovements,801},
					{-1,-1},
				};

extern bool				quit;
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

void os_create_directory(char *path)
{
	CreateDirectory(path,NULL);
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

void os_application_quit(void)
{
	quit=TRUE;
}

void os_select_window(void)
{
    ShowWindow(wnd,SW_SHOWNORMAL);
}

void os_set_title_window(char *title)
{
	SetWindowText(wnd,title);
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

void os_menu_redraw(void)
{
	DrawMenuBar(wnd);
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

      Menu Lookups
      
======================================================= */
		
int os_win32_menu_lookup(int id)
{
	int			idx;

	idx=0;

	while (TRUE) {
		if (win32_menu_remap[idx][1]==-1) return(-1);
		if (win32_menu_remap[idx][1]==id) return(win32_menu_remap[idx][0]);
		idx++;
	}

	return(-1);
}

