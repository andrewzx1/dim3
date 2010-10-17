/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Texture Palette

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
	#include "dim3animator.h"
#endif

#include "model.h"
#include "interface.h"
#include "dialog.h"

extern int					cur_mesh;
extern d3rect				model_box;

extern model_type			model;

int							txt_palette_max_page_count,txt_palette_per_page_count,
							txt_palette_cur_page,txt_palette_pixel_sz,
							txt_palette_page_list_count,txt_palette_page_list_width;
d3rect						txt_palette_box;

/* =======================================================

      Texture Palette Setup
      
======================================================= */

void texture_palette_setup(void)
{
	int				x_sz;
	d3rect			wbox;
	
	os_get_window_box(&wbox);
	
	x_sz=(wbox.rx-wbox.lx)-total_list_width;
	if (x_sz<model_view_min_size) x_sz=model_view_min_size;
	
	wbox.rx=wbox.lx+x_sz;
	
	txt_palette_max_page_count=max_model_texture;
	txt_palette_per_page_count=16;

	txt_palette_page_list_count=txt_palette_max_page_count/txt_palette_per_page_count;
	txt_palette_page_list_width=(txt_palette_page_list_count>>1)*16;
	
	txt_palette_pixel_sz=((wbox.rx-16)-(wbox.lx+txt_palette_page_list_width))/txt_palette_per_page_count;
	
	txt_palette_box.lx=wbox.lx;
	txt_palette_box.rx=wbox.rx;
	txt_palette_box.ty=wbox.by-txt_palette_pixel_sz;
	txt_palette_box.by=(txt_palette_box.ty+txt_palette_pixel_sz)+1;
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
	vertex_set_sel_mask_material(cur_mesh,txt_idx);
	hilite_vertex_rows();
}

