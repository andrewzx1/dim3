/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Progress Routines

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
	#include "dim3engine.h"
#endif

#include "video.h"

extern map_type				map;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;

float						progress_current;
bitmap_type					progress_bitmap;

/* =======================================================

      Setup Progress
      
======================================================= */

void progress_initialize(char *map_name)
{
	char			path[1024];
	bool			bitmap_ok;
	
		// check for map progress background,
		// otherwise use default
		
	bitmap_ok=FALSE;
		
	if (map_name!=NULL) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Backgrounds_Map",map_name,"png");
		bitmap_ok=bitmap_open(&progress_bitmap,path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,gl_check_texture_rectangle_ok(),FALSE,FALSE,FALSE);
	}
	
	if (!bitmap_ok) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Backgrounds","load","png");
		bitmap_open(&progress_bitmap,path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,gl_check_texture_rectangle_ok(),FALSE,FALSE,FALSE);
	}
	
		// current progress
		
	progress_current=-1;
}

void progress_shutdown(void)
{
	bitmap_close(&progress_bitmap);
}

/* =======================================================

      Draw Progress
      
======================================================= */

void progress_draw(float percentage)
{
	int				lft,rgt,top,bot,mid,rgt2;

		// any change?
		
	if (progress_current==percentage) return;
	
	progress_current=percentage;
	
		// start the frame
	
	gl_frame_clear(FALSE);
	
		// setup draw
		
	gl_2D_view_interface();
	
		// draw background

	gl_texture_clear(0);			// progress can be called while baseutility loads bitmaps, need to reset the current bitmap so it doesn't get lost

	if (gl_check_texture_rectangle_ok()) {
		view_draw_next_vertex_object_2D_texture_quad_rectangle(progress_bitmap.gl_id,1.0f,0,iface.scale_x,0,iface.scale_y,progress_bitmap.wid,progress_bitmap.high);
	}
	else {
		view_draw_next_vertex_object_2D_texture_quad(progress_bitmap.gl_id,NULL,1.0f,0,iface.scale_x,0,iface.scale_y,0.0f,1.0f,0.0f,1.0f);
	}
	
		// draw the progress background
		
	lft=iface.progress.lx;
	rgt=iface.progress.rx;
	top=iface.progress.ty;
	bot=iface.progress.by;

	mid=(top+bot)>>1;

	view_draw_next_vertex_object_2D_color_poly(lft,top,&iface.progress.base_color_end,rgt,top,&iface.progress.base_color_end,rgt,mid,&iface.progress.base_color_start,lft,mid,&iface.progress.base_color_start,1.0f);
	view_draw_next_vertex_object_2D_color_poly(lft,mid,&iface.progress.base_color_start,rgt,mid,&iface.progress.base_color_start,rgt,bot,&iface.progress.base_color_end,lft,bot,&iface.progress.base_color_end,1.0f);
	
		// draw the progress foreground
	
	rgt2=lft+(int)((float)(rgt-lft)*(percentage/100));
	
	view_draw_next_vertex_object_2D_color_poly(lft,top,&iface.progress.hilite_color_start,rgt2,top,&iface.progress.hilite_color_start,rgt2,mid,&iface.progress.hilite_color_end,lft,mid,&iface.progress.hilite_color_end,1.0f);
	view_draw_next_vertex_object_2D_color_poly(lft,mid,&iface.progress.hilite_color_end,rgt2,mid,&iface.progress.hilite_color_end,rgt2,bot,&iface.progress.hilite_color_start,lft,bot,&iface.progress.hilite_color_start,1.0f);
	
		// progress outline

	if (iface.progress.outline) view_draw_next_vertex_object_2D_line_quad(&iface.progress.outline_color,1.0f,lft,rgt,top,bot);

	gl_frame_swap();
}

