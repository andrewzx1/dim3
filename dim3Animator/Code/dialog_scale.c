/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Dialog Pick Scale

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3Animator.h"
#endif

#include "glue.h"
#include "interface.h"
#include "ui_common.h"

d3fpnt					*dialog_pick_scale_scale;

// controls

#define diag_prop_pick_scale_type	5000
#define diag_prop_pick_scale_x		5001
#define diag_prop_pick_scale_y		5002
#define diag_prop_pick_scale_z		5003
#define diag_prop_pick_scale_cancel	5004
#define diag_prop_pick_scale_ok		5005

os_dialog_ctrl_type		diag_property_pick_scale_ctrls[]={
							{os_dialog_ctrl_type_text_right,0,"Scale Type:",5,13,65,20},
							{os_dialog_ctrl_type_combo,diag_prop_pick_scale_type,"",75,10,240,20},
							{os_dialog_ctrl_type_text_right,0,"X:",35,38,35,20},
							{os_dialog_ctrl_type_text_right,0,"Y:",35,63,35,20},
							{os_dialog_ctrl_type_text_right,0,"Z:",35,88,35,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_pick_scale_x,"",75,35,100,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_pick_scale_y,"",75,60,100,20},
							{os_dialog_ctrl_type_text_edit,diag_prop_pick_scale_z,"",75,85,100,20},
							{os_dialog_ctrl_type_button,diag_prop_pick_scale_cancel,"Cancel",180,110,80,25},
							{os_dialog_ctrl_type_default_button,diag_prop_pick_scale_ok,"OK",270,110,80,25},
							{-1,-1,"",0,0,0,0}
						};

/* =======================================================

      Run Pick Scale
      
======================================================= */

void dialog_property_pick_scale_proc(int msg_type,int id)
{
	switch (msg_type) {

		case os_dialog_msg_type_init:
			os_dialog_combo_add(diag_prop_pick_scale_type,"Percentage (Ex: 200% = 2x, 50% = 1/2)");
			os_dialog_combo_add(diag_prop_pick_scale_type,"To Specific Model Units");
			os_dialog_combo_set_value(diag_prop_pick_scale_type,0);

			os_dialog_set_float(diag_prop_pick_scale_x,100.0f);
			os_dialog_set_float(diag_prop_pick_scale_y,100.0f);
			os_dialog_set_float(diag_prop_pick_scale_z,100.0f);
			os_dialog_set_focus(diag_prop_pick_scale_x,TRUE);
			break;

		case os_dialog_msg_type_command:

			if (id==diag_prop_pick_scale_type) {

				switch (os_dialog_combo_get_value(diag_prop_pick_scale_type)) {
					case 0:
						os_dialog_set_float(diag_prop_pick_scale_x,100.0f);
						os_dialog_set_float(diag_prop_pick_scale_y,100.0f);
						os_dialog_set_float(diag_prop_pick_scale_z,100.0f);
						break;
					case 1:
						os_dialog_set_float(diag_prop_pick_scale_x,dialog_pick_scale_scale->x);
						os_dialog_set_float(diag_prop_pick_scale_y,dialog_pick_scale_scale->y);
						os_dialog_set_float(diag_prop_pick_scale_z,dialog_pick_scale_scale->z);
						break;
				}

				return;
			}

			if (id==diag_prop_pick_scale_cancel) {
				os_dialog_close(FALSE);
				return;
			}

			if (id==diag_prop_pick_scale_ok) {

				switch (os_dialog_combo_get_value(diag_prop_pick_scale_type)) {
					case 0:
						dialog_pick_scale_scale->x=os_dialog_get_float(diag_prop_pick_scale_x)/100.0f;
						dialog_pick_scale_scale->y=os_dialog_get_float(diag_prop_pick_scale_y)/100.0f;
						dialog_pick_scale_scale->z=os_dialog_get_float(diag_prop_pick_scale_z)/100.0f;
						break;
					case 1:
						dialog_pick_scale_scale->x=os_dialog_get_float(diag_prop_pick_scale_x)/dialog_pick_scale_scale->x;
						dialog_pick_scale_scale->y=os_dialog_get_float(diag_prop_pick_scale_y)/dialog_pick_scale_scale->y;
						dialog_pick_scale_scale->z=os_dialog_get_float(diag_prop_pick_scale_z)/dialog_pick_scale_scale->z;
						break;
				}

				os_dialog_close(TRUE);
				return;
			}

			break;
	}
}

bool dialog_scale_run(model_type *mdl,d3fpnt *scale)
{
	dialog_pick_scale_scale=scale;
	return(os_dialog_run("Pick Scale",355,140,diag_property_pick_scale_ctrls,dialog_property_pick_scale_proc));
}
