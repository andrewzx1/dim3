/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Texture Palette

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "interface.h"
#include "ui_common.h"

extern int						txt_palette_cur_page;
extern bool						list_palette_open;

extern model_type				model;
extern animator_state_type		state;

/* =======================================================

      Texture Palette Settings
      
======================================================= */

int texture_palette_per_page_count(void)
{
	return(32);
}

int texture_palette_page_list_count(void)
{
	return(max_model_texture/texture_palette_per_page_count());
}

int texture_palette_page_list_width(void)
{
	return((texture_palette_page_list_count()>>1)*16);
}

int texture_palette_pixel_size(void)
{
	int				rx;
	d3rect			wbox;
	
	os_get_window_box(&wbox);
	
	rx=wbox.rx;
	
	if (list_palette_open) {
		rx-=list_palette_tree_sz;
	}
	else {
		rx-=list_palette_border_sz;
	}

	return((rx-(wbox.lx+texture_palette_page_list_width()))/texture_palette_per_page_count());
}

void texture_palette_box(d3rect *box)
{
	int				rx,pixel_sz;
	d3rect			wbox;
	
	pixel_sz=texture_palette_pixel_size();
	
	os_get_window_box(&wbox);
	
	rx=wbox.rx;
	
	if (list_palette_open) {
		rx-=list_palette_tree_sz;
	}
	else {
		rx-=list_palette_border_sz;
	}
	
	box->lx=wbox.lx;
	box->rx=rx;
	box->ty=wbox.by-pixel_sz;
	box->by=(box->ty+pixel_sz)+1;
}

/* =======================================================

      Get and Set Textures
      
======================================================= */

int texture_palette_get_selected_texture(void)
{
	return(-1);
}

void texture_palette_put_selected_texture(int txt_idx)
{
	vertex_set_sel_mask_material(state.cur_mesh_idx,txt_idx);
}

