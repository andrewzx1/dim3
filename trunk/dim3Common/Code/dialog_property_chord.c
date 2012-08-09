/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Dialog Property Chord

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

#ifdef D3_PCH
#ifdef D3_EDITOR
	#include "dim3editor.h"
#endif
#ifdef D3_ANIMATOR
	#include "dim3Animator.h"
#endif
#ifdef D3_SETUP
	#include "dim3Setup.h"
#endif
#endif

#include "glue.h"
#include "interface.h"
#include "ui_common.h"

int								dialog_property_chord_value_type;
void							*dialog_property_chord_value;

// controls

#define diag_prop_chord_str_x	5000
#define diag_prop_chord_str_y	5001
#define diag_prop_chord_str_z	5002
#define diag_prop_chord_ok		5003

os_dialog_ctrl_type		diag_property_chord_ctrls[]={
							{os_dialog_ctrl_type_text_edit,diag_prop_chord_str_x,"",5,10,145,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_chord_str_y,"",155,10,145,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_chord_str_z,"",305,10,145,20},
							{os_dialog_ctrl_type_default_button,diag_prop_chord_ok,"OK",370,40,80,25},
							{-1,-1,"",0,0,0,0}
						};

/* =======================================================

      Run Property Chord
      
======================================================= */

void dialog_property_chord_proc(int msg_type,int id)
{
	char		str_x[256],str_y[256],str_z[256];
	bool		three_value;

	switch (msg_type) {

		case os_dialog_msg_type_init:
			three_value=property_chord_get_values(dialog_property_chord_value_type,dialog_property_chord_value,str_x,str_y,str_z);
			os_dialog_set_text(diag_prop_chord_str_x,str_x);
			os_dialog_set_text(diag_prop_chord_str_y,str_y);
			os_dialog_set_text(diag_prop_chord_str_z,str_z);
			os_dialog_enable(diag_prop_chord_str_z,three_value);
			os_dialog_set_focus(diag_prop_chord_str_x,TRUE);
			break;

		case os_dialog_msg_type_button:
			if (id==diag_prop_chord_ok) {
				os_dialog_get_text(diag_prop_chord_str_x,str_x,256);
				os_dialog_get_text(diag_prop_chord_str_y,str_y,256);
				os_dialog_get_text(diag_prop_chord_str_z,str_z,256);
				property_chord_set_values(dialog_property_chord_value_type,dialog_property_chord_value,str_x,str_y,str_z);

				os_dialog_close();
			}
			break;
	}
}

void dialog_property_chord_run(int value_type,void *value)
{
	char			title[256];

		// setup dialog

	dialog_property_chord_value_type=value_type;
	dialog_property_chord_value=value;

	property_chord_get_title(value_type,title);

		// run dialog

	os_dialog_run(title,455,70,diag_property_chord_ctrls,dialog_property_chord_proc);
}
