/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Errors

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

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "scripts.h"
#include "sounds.h"
#include "inputs.h"
#include "video.h"

#define error_close_id				0

extern server_type			server;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern js_type				js;

/* =======================================================

      Error Operations
      
======================================================= */

void error_open(void)
{
	int					x,y,wid,high,control_y_add;
	char				tab_list[][name_str_len]={"Error"};

		// setup gui
		
	gui_initialize(NULL,NULL);
	
		// the dialog
		
	element_tab_add((char*)tab_list,0,-1,1);

		// the text
		
	control_y_add=element_get_control_high();

	x=iface.scale_x/2;
	y=(iface.scale_y/2)-control_y_add;
	element_text_add("[Error]",-1,x,y,iface.font.text_size_large,tx_center,FALSE,TRUE);

	y+=control_y_add;
	element_text_add(view.error.str,-1,x,y,iface.font.text_size_small,tx_center,FALSE,FALSE);
	
	y+=control_y_add;
	element_text_add(view.error.str_2,-1,x,y,iface.font.text_size_small,tx_center,FALSE,FALSE);
	
		// buttons
		
	wid=(int)(((float)iface.scale_x)*0.1f);
	high=(int)(((float)iface.scale_x)*0.04f);
	
	element_get_button_bottom_right(&x,&y,wid,high);
	
	element_button_text_add("Close",error_close_id,x,y,wid,high,element_pos_right,element_pos_bottom);
}

void error_close(void)
{
	gui_shutdown();
}

void error_setup(char *err_str,char *err_str_2)
{
	strcpy(view.error.str,err_str);
	strcpy(view.error.str_2,err_str_2);
}

/* =======================================================

      Run Error Page
      
======================================================= */

void error_run(void)
{
	int				id;

	gui_draw(1.0f,TRUE);
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
		// always exit to intro
		
	if (id==error_close_id) server.next_state=gs_intro;
}


