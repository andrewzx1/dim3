/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Chooser

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"
#include "interfaces.h"

extern server_type			server;
extern js_type				js;
extern hud_type				hud;
extern setup_type			setup;

int							chooser_idx;
bool						chooser_start_trigger;

extern int chooser_find(char *name);

/* =======================================================

      Chooser Operations
      
======================================================= */

void chooser_open(void)
{
	int					n;
	char				path[1024],path2[1024],fname[256];
	chooser_type		*chooser;
	chooser_item_type	*item;
	
	gui_initialize("Bitmaps/Backgrounds","chooser");
	
		// load up the elements
		
	chooser=&hud.choosers[chooser_idx];
	
	item=chooser->items;
	for (n=0;n<chooser->nitem;n++) {
	
		file_paths_data(&setup.file_path_setup,path,"Chooser",item->file,"png");
		
		if (item->clickable) {
			sprintf(fname,"%s_selected",item->file);
			file_paths_data(&setup.file_path_setup,path2,"Chooser",fname,"png");
			element_button_add(path,path2,item->item_id,item->x,item->y,-1,-1,element_pos_left,element_pos_top);
		}
		else {
			element_bitmap_add(path,0,item->x,item->y,-1,-1,FALSE);
		}
		
		item++;
	}
	
	server.state=gs_chooser;
}

void chooser_close(void)
{
	gui_shutdown();
	
	if (server.state==gs_chooser) server.state=gs_running;			// only reset to running if picked chooser item didn't reset to something else
}

/* =======================================================

      Chooser Triggers
      
======================================================= */

void chooser_trigger_clear(void)
{
	chooser_start_trigger=FALSE;
}

void chooser_trigger_check(void)
{
	if (chooser_start_trigger) chooser_open();
}	

void chooser_trigger_set(char *name)
{
	chooser_idx=chooser_find(name);
	if (chooser_idx==-1) return;
	
	chooser_start_trigger=TRUE;
}

/* =======================================================

      Menu Input
      
======================================================= */

void chooser_click(void)
{
	int					id;
		
	id=gui_click();
	if (id==-1) return;
	
	hud_click();
	
	chooser_close();
	scripts_post_event_console(&js.game_attach,sd_event_chooser,0,id);
}

/* =======================================================

      Run Chooser
      
======================================================= */

void chooser_run(void)
{
	gui_draw(1.0f,TRUE);
	chooser_click();
}
