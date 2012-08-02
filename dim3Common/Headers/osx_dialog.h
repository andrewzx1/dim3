/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Dialog Functions

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// dialog utility
//

extern void dialog_open(WindowRef *wind,char *name);
extern void dialog_set_title(WindowRef wind,char *title);
extern void dialog_set_text(WindowRef wind,unsigned long sig,int id,char *str);
extern void dialog_get_text(WindowRef wind,unsigned long sig,int id,char *str,int len);
extern void dialog_set_int(WindowRef wind,unsigned long sig,int id,int value);
extern int dialog_get_int(WindowRef wind,unsigned long sig,int id);
extern void dialog_set_float(WindowRef wind,unsigned long sig,int id,float value);
extern float dialog_get_float(WindowRef wind,unsigned long sig,int id);
extern void dialog_set_boolean(WindowRef wind,unsigned long sig,int id,bool b);
extern bool dialog_get_boolean(WindowRef wind,unsigned long sig,int id);
extern void dialog_clear_combo(WindowRef wind,unsigned long sig,int id);
extern void dialog_add_combo_item(WindowRef wind,unsigned long sig,int id,char *str,unsigned long cmd);
extern void dialog_set_combo(WindowRef wind,unsigned long sig,int id,int idx);
extern int dialog_get_combo(WindowRef wind,unsigned long sig,int id);
extern void dialog_set_radio_buttons(WindowRef wind,unsigned long sig,int value,int count);
extern int dialog_get_radio_buttons(WindowRef wind,unsigned long sig,int count);
extern void dialog_set_value(WindowRef wind,unsigned long sig,int id,int value);
extern int dialog_get_value(WindowRef wind,unsigned long sig,int id);
extern void dialog_set_focus(WindowRef wind,unsigned long sig,int id);
extern void dialog_enable(WindowRef wind,unsigned long sig,int id,bool enable);
extern void dialog_hide(WindowRef wind,unsigned long sig,int id,bool show);
extern void dialog_redraw(WindowRef wind,unsigned long sig,int id);
