/****************************** File *********************************

Module: dim3 Editor
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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kPropertyMapAdd			0
#define kPropertyMap			100

extern app_state_type			state;
extern file_path_setup_type		file_path_setup;
extern list_palette_type		property_palette;

list_palette_type				file_palette;

void file_palette_fill(void);		// forward reference

/* =======================================================

      File Palette Setup
      
======================================================= */

void file_palette_initialize(void)
{
	list_palette_list_initialize(&file_palette,"Maps",TRUE,FALSE,FALSE);

	file_palette.item_pane.click.id=0;
	file_palette.item_pane.click.idx=-1;
	file_palette.item_pane.click.item=NULL;

	file_palette.open=TRUE;

		// preload map list

	file_palette_fill();
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

	list_palette_delete_all_items(&file_palette);

		// load the files

	fpd=file_paths_read_directory_data(&file_path_setup,"Maps","xml");

		// fill the list

	list_palette_add_header_button(&file_palette,kPropertyMapAdd,"Maps",list_button_plus);

	list_palette_sort_mark_start(&file_palette);

	for (n=0;n!=fpd->nfile;n++) {
		if (fpd->files[n].is_dir) continue;

		sel=FALSE;
		if (state.map.map_open) sel=(strcmp(state.map.map_file_name,fpd->files[n].file_name)==0);

		list_palette_add_item(&file_palette,kPropertyMap,n,fpd->files[n].file_name,sel,FALSE);
	}

	list_palette_sort(&file_palette);

	file_paths_close_directory(fpd);
}

/* =======================================================

      File Palette Draw
      
======================================================= */

void file_palette_draw(void)
{
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
	char				file_name[file_str_len];
	
		// check if open changes
	
	old_open=list_palette_is_open(&file_palette);

		// click

	if (!list_palette_click(&file_palette,pnt,double_click)) {
		if (old_open!=list_palette_is_open(&file_palette)) main_wind_draw();
		return;
	}

		// add

	if (file_palette.item_pane.click.id==kPropertyMapAdd) {
		file_new_map();
		file_palette_fill();
		return;
	}

		// open map

	if (file_palette.item_pane.click.id!=kPropertyMap) return;

	strncpy(file_name,file_palette.item_pane.click.item->name,file_str_len);
	file_name[file_str_len-1]=0x0;

	file_open_map(file_name);

	file_palette_fill();

	main_wind_draw();
}