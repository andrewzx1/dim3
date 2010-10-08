/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: OS X Glue Code

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

extern WindowRef				mainwind;

/* =======================================================

      OS Specific Utility Routines
      
======================================================= */

void os_get_icon_file_path(char *path)
{
	strcpy(path,"Contents/Resources");
}

void os_get_window_box(d3rect *box)
{
	Rect			wbox;
	
	GetWindowPortBounds(mainwind,&wbox);

	box->lx=wbox.left;
	box->rx=wbox.right;
	box->ty=wbox.top;
	box->by=wbox.bottom;
}

void os_application_quit(void)
{
	QuitApplicationEventLoop();
}

void os_select_window(void)
{
   SelectWindow(mainwind);
}

void os_set_title_window(char *title)
{
	unsigned char	p_str[256];
	
	CopyCStringToPascal(title,p_str);
	SetWTitle(mainwind,p_str);
}

void os_set_arrow_cursor(void)
{
	SetThemeCursor(kThemeArrowCursor);
}

void os_set_wait_cursor(void)
{
	SetThemeCursor(kThemeWatchCursor);
}

void os_set_hand_cursor(void)
{
   SetThemeCursor(kThemeOpenHandCursor);
}

void os_set_drag_cursor(void)
{
   SetThemeCursor(kThemeClosedHandCursor);
}

void os_set_resize_cursor(void)
{
   SetThemeCursor(kThemeResizeUpDownCursor);
}

void os_menu_enable_item(int menu_idx,int item_idx,bool enable)
{
	if (enable) {
		EnableMenuItem(GetMenuRef(menu_idx),item_idx);
	}
	else {
		DisableMenuItem(GetMenuRef(menu_idx),item_idx);
	}
}

void os_menu_check_item(int menu_idx,int item_idx,bool check)
{
	CheckMenuItem(GetMenuRef(menu_idx),item_idx,check);
}

void os_menu_redraw(void)
{
	DrawMenuBar();
}

bool os_button_down(void)
{
	return(Button());
}

bool os_key_space_down(void)
{
	KeyMap			map;
	unsigned char	*c;
	
	GetKeys(map);
	c=(unsigned char*)map;
	
	return((c[6]&0x02)!=0);
}

bool os_key_option_down(void)
{
	return((GetCurrentKeyModifiers()&optionKey)!=0);
}

bool os_key_control_down(void)
{
	return((GetCurrentKeyModifiers()&controlKey)!=0);
}

bool os_key_command_down(void)
{
	return((GetCurrentKeyModifiers()&cmdKey)!=0);
}

bool os_key_shift_down(void)
{
	return((GetCurrentKeyModifiers()&shiftKey)!=0);
}

bool os_track_mouse_location(d3pnt *pt,d3rect *offset_box)
{
	Point					uipt;
	MouseTrackingResult		track;

	TrackMouseLocation(NULL,&uipt,&track);
	
	if (offset_box==NULL) {
		pt->x=uipt.h;
		pt->y=uipt.v;
	}
	else {
		pt->x=uipt.h-offset_box->lx;
		pt->y=uipt.v-offset_box->ty;
	}
	
	return(track==kMouseTrackingMouseReleased);
}

