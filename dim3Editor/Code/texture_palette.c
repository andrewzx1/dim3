/****************************** File *********************************

Module: dim3 Editor
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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "dialog.h"

extern map_type				map;
extern editor_state_type	state;
extern editor_setup_type	setup;

extern int					txt_palette_max_page_count,txt_palette_per_page_count,
							txt_palette_cur_page,txt_palette_pixel_sz,
							txt_palette_page_list_count,txt_palette_page_list_width;
extern d3rect				txt_palette_box;

/* =======================================================

      Texture Palette Setup
      
======================================================= */

void texture_palette_setup(void)
{
	d3rect			wbox;
	
	os_get_window_box(&wbox);
	
	txt_palette_max_page_count=max_map_texture;
	txt_palette_per_page_count=setup.big_texture?16:32;

	txt_palette_page_list_count=txt_palette_max_page_count/txt_palette_per_page_count;
	txt_palette_page_list_width=(txt_palette_page_list_count>>1)*16;
	
	txt_palette_pixel_sz=((wbox.rx-16)-(wbox.lx+txt_palette_page_list_width))/txt_palette_per_page_count;
	
	txt_palette_box.lx=wbox.lx;
	txt_palette_box.rx=wbox.rx;
	txt_palette_box.ty=wbox.by-txt_palette_pixel_sz;
	txt_palette_box.by=(txt_palette_box.ty+txt_palette_pixel_sz)+1;
}

/* =======================================================

      Get and Put Selection Texture
      
======================================================= */

int texture_palette_get_selected_texture(void)
{
	int					type,main_idx,poly_idx;
	
	if (select_count()!=1) return(-1);
	
	select_get(0,&type,&main_idx,&poly_idx);
	
	if (type==liquid_piece) {
		if (view_get_uv_layer()==uv_layer_normal) return(map.liquid.liquids[main_idx].txt_idx);
		return(map.liquid.liquids[main_idx].lmap_txt_idx);
	}
	
	if (type==mesh_piece) {
		if (view_get_uv_layer()==uv_layer_normal) return(map.mesh.meshes[main_idx].polys[poly_idx].txt_idx);
		return(map.mesh.meshes[main_idx].polys[poly_idx].lmap_txt_idx);
	}
	
	return(-1);
}

void texture_palette_put_selected_texture(int txt_idx)
{
	int					n,k,sel_count,type,main_idx,poly_idx;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
	
		select_get(n,&type,&main_idx,&poly_idx);
		
			// liquids
			
		if (type==liquid_piece) {
			if (view_get_uv_layer()==uv_layer_normal) {
				map.liquid.liquids[main_idx].txt_idx=txt_idx;
			}
			else {
				map.liquid.liquids[main_idx].lmap_txt_idx=txt_idx;
			}
			continue;
		}
		
			// meshes
			
		if (type!=mesh_piece) continue;
		
		mesh=&map.mesh.meshes[main_idx];
		
			// only set polygon
			
		if (state.drag_mode==drag_mode_polygon) {
			if (view_get_uv_layer()==uv_layer_normal) {
				mesh->polys[poly_idx].txt_idx=txt_idx;
			}
			else {
				mesh->polys[poly_idx].lmap_txt_idx=txt_idx;
			}
		}
		
			// set all mesh
			
		else {
			
			poly=mesh->polys;
			
			for (k=0;k!=mesh->npoly;k++) {
				if (view_get_uv_layer()==uv_layer_normal) {
					poly->txt_idx=txt_idx;
				}
				else {
					poly->lmap_txt_idx=txt_idx;
				}
				poly++;
			}
		
		}		
	}
}

/* =======================================================

      Reset Texture Palette
      
======================================================= */

void texture_palette_reset(void)
{
	int			sel;
	
		// get to proper texture page
		
	sel=texture_palette_get_selected_texture();
	if (sel!=-1) txt_palette_cur_page=sel/txt_palette_per_page_count;
}

