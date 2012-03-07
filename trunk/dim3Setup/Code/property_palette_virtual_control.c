/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette Virtual Control

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
	#include "dim3setup.h"
#endif

#define kVirutalControlStick			100
#define kVirutalControlButton			200

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Virtual Control
      
======================================================= */

void property_palette_fill_virtual_control(void)
{
	int							n;
	char						str[256];
	
	list_palette_set_title(&property_palette,"Virtual Controls",NULL,NULL,NULL,NULL,NULL);
	
		// sticks

	list_palette_add_header(&property_palette,0,"Sticks");

	for (n=0;n!=max_virtual_stick;n++) {
		sprintf(str,"Stick %d",n);
		list_palette_add_string_selectable(&property_palette,(kVirutalControlStick+n),str,NULL,(state.cur_virtual_control_stick_idx==n),FALSE);
	}

		// buttons

	list_palette_add_header(&property_palette,0,"Buttons");

	for (n=0;n!=max_virtual_button;n++) {
		sprintf(str,"Button %d",n);
		list_palette_add_string_selectable(&property_palette,(kVirutalControlButton+n),str,NULL,(state.cur_virtual_control_button_idx==n),FALSE);
	}
}

/* =======================================================

      Property Palette Click Virtual Control
      
======================================================= */

void property_palette_click_virtual_control(int id,bool double_click)
{
		// select sticks

	if ((id>=kVirutalControlStick) && (id<(kVirutalControlStick+max_virtual_stick))) {
		state.cur_virtual_control_stick_idx=id-kVirutalControlStick;
		state.cur_virtual_control_button_idx=-1;
		if (double_click) list_palette_set_level(&property_palette,2);
		return;
	}

		// select buttons

	if ((id>=kVirutalControlButton) && (id<(kVirutalControlButton+max_virtual_button))) {
		state.cur_virtual_control_button_idx=id-kVirutalControlButton;
		state.cur_virtual_control_stick_idx=-1;
		if (double_click) list_palette_set_level(&property_palette,2);
		return;
	}
}

