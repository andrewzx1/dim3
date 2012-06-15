/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: win32 Dialog Utilities

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

extern void win32_dialog_set_text(HWND diag,int id,char *value);
extern void win32_dialog_get_text(HWND diag,int id,char *value,int value_len);
extern void win32_dialog_set_int(HWND diag,int id,int value);
extern int win32_dialog_get_int(HWND diag,int id);
extern void win32_dialog_set_float(HWND diag,int id,float value);
extern float win32_dialog_get_float(HWND diag,int id);
extern void win32_dialog_set_boolean(HWND diag,int id,boolean value);
extern boolean win32_dialog_get_boolean(HWND diag,int id);
extern void win32_dialog_combo_clear(HWND diag,int id);
extern void win32_dialog_combo_add(HWND diag,int id,char *str);
extern void win32_dialog_combo_set_value(HWND diag,int id,int value);
extern int win32_dialog_combo_get_value(HWND diag,int id);
extern void win32_dialog_list_add(HWND diag,int id,char *str);
extern void win32_dialog_list_set_value(HWND diag,int id,int value);
extern int win32_dialog_list_get_value(HWND diag,int id);
extern void win32_dialog_enable(HWND diag,int id,bool enable);
extern void win32_dialog_set_focus(HWND diag,int id);
extern int win32_dialog_select_all(HWND diag,int id);
