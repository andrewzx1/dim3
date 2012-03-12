/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: File Palette

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kPropertyAddSpot			0
#define kPropertyAddLight			1
#define kPropertyAddSound			2
#define kPropertyAddParticle		3
#define kPropertyAddScenery			4
#define kPropertyAddNode			5
#define kPropertyAddGroup			6
#define kPropertyAddMovement		7
#define kPropertyAddCinema			8

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;
extern file_path_setup_type		file_path_setup;

list_palette_type				file_palette;

/* =======================================================

      Item Palette Setup
      
======================================================= */

void file_palette_initialize(void)
{
	list_palette_list_initialize(&file_palette,"Maps",TRUE,FALSE,FALSE);

	file_palette.item_pane.click.id=0;
	file_palette.item_pane.click.idx=-1;
	file_palette.item_pane.click.item=NULL;

	file_palette.open=FALSE;
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
	int			n;

		// maps

	list_palette_add_header_button(&file_palette,kPropertyAddSpot,"Maps",list_button_plus);
	/*
	list_palette_sort_mark_start(&file_palette);

	for (n=0;n!=map.nspot;n++) {
		if (map.spots[n].name[0]!=0x0) {
			list_palette_add_item(&file_palette,spot_piece,n,map.spots[n].name,((file_palette.item_type==spot_piece)&&(n==item_palette.item_idx)),FALSE);
		}
		else {
			list_palette_add_item(&file_palette,spot_piece,n,"[none]",((file_palette.item_type==spot_piece)&&(n==item_palette.item_idx)),FALSE);
		}
	}

	list_palette_sort(&file_palette);
*/
}

/* =======================================================

      File Palette Draw
      
======================================================= */

void file_palette_draw(void)
{
	list_palette_delete_all_items(&file_palette);

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

	if (!double_click) return(FALSE);

		// open new map


	main_wind_draw();

	return(TRUE);
}