/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: File Palette

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
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kPropertyModelAdd			0
#define kPropertyModel				100

extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

list_palette_type				file_palette;
bool							file_palette_reset_models;

/* =======================================================

      File Palette Setup
      
======================================================= */

void file_palette_initialize(void)
{
	list_palette_list_initialize(&file_palette,"Models",TRUE,FALSE,FALSE);

	file_palette.item_pane.click.id=0;
	file_palette.item_pane.click.idx=-1;
	file_palette.item_pane.click.item=NULL;

	file_palette.open=TRUE;

		// set models to get loaded

	file_palette_reset_models=TRUE;
}

void file_palette_shutdown(void)
{
	list_palette_list_shutdown(&file_palette);
}

/* =======================================================

      File Palette Fill
      
======================================================= */

void file_palette_fill(void)
{
	int								n;
	bool							sel;
	file_path_directory_type		*fpd;

		// already filled?

	if (!file_palette_reset_models) return;

	list_palette_delete_all_items(&file_palette);
	file_palette_reset_models=FALSE;

		// load the files

	fpd=file_paths_read_directory_data_dir(&file_path_setup,"Models","Mesh.xml;Model.xml");

		// fill the list

	list_palette_add_header_button(&file_palette,kPropertyModelAdd,"Models",list_button_plus);

	list_palette_sort_mark_start(&file_palette);

	for (n=0;n!=fpd->nfile;n++) {
		if (fpd->files[n].is_dir) continue;

		sel=FALSE;
		if (state.model_open) sel=strcmp(state.model_file_name,fpd->files[n].file_name);

		list_palette_add_item(&file_palette,kPropertyModel,n,fpd->files[n].file_name,sel,FALSE);
	}

	list_palette_sort(&file_palette);

	file_paths_close_directory(fpd);
}

/* =======================================================

      File Palette Draw
      
======================================================= */

void file_palette_draw(void)
{
	file_palette_fill();
	list_palette_draw(&file_palette);
}

/* =======================================================

      File Palette Scroll Wheel
      
======================================================= */

void file_palette_scroll_wheel(d3pnt *pnt,int move)
{
	list_palette_scroll_wheel(&file_palette,pnt,move);
}

/* =======================================================

      File Palette Click
      
======================================================= */

void file_palette_click(d3pnt *pnt,bool double_click)
{
	bool				old_open;
	
		// check if open changes
	
	old_open=list_palette_is_open(&file_palette);

		// click

	if (!list_palette_click(&file_palette,pnt,double_click)) {
		if (old_open!=list_palette_is_open(&file_palette)) main_wind_draw();
		return;
	}

		// ignore everything but double-clicks

	if (!double_click) return;

		// open new model


	main_wind_draw();

	return;
}