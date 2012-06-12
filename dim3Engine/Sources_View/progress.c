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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

extern app_type				app;
extern map_type				map;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;

int							progress_current,progress_max;
bitmap_type					progress_background_bitmap,progress_overlay_bitmap;

/* =======================================================

      Setup Progress
      
======================================================= */

void progress_initialize(char *map_name,int max)
{
	char			path[1024];
	bool			bitmap_ok;

		// ignore if dedicated host

	if (app.dedicated_host) return;
	
		// check for map progress background,
		// otherwise use default
		
	bitmap_ok=FALSE;
		
	if (map_name!=NULL) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Backgrounds_Map",map_name,"png");
		bitmap_ok=bitmap_open(&progress_background_bitmap,path,mipmap_mode_none,FALSE,gl_check_texture_rectangle_ok(),FALSE,FALSE);
	}
	
	if (!bitmap_ok) {
		view_file_paths_bitmap_check_wide(path,"Bitmaps/Backgrounds","load");
		bitmap_open(&progress_background_bitmap,path,mipmap_mode_none,FALSE,gl_check_texture_rectangle_ok(),FALSE,FALSE);
	}

		// overlay bitmap

	if (iface.progress.overlay) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","progress_overlay","png");
		bitmap_open(&progress_overlay_bitmap,path,mipmap_mode_none,FALSE,FALSE,FALSE,FALSE);
	}
	
		// current progress
		
	progress_current=-1;
	progress_max=max;
}

void progress_shutdown(void)
{
		// ignore if dedicated host

	if (app.dedicated_host) return;

		// close bitmaps

	bitmap_close(&progress_background_bitmap);
	if (iface.progress.overlay) bitmap_close(&progress_overlay_bitmap);
}

/* =======================================================

      Draw Progress
      
======================================================= */

void progress_next(void)
{
	int				lft,rgt,top,bot,mid,rgt2;
	d3col			col;

		// ignore if dedicated host

	if (app.dedicated_host) return;

		// next progress

	progress_current++;
	
		// start the frame
	
	gl_frame_clear(FALSE);
	
		// setup draw
		
	gl_2D_view_interface();
	
		// draw background

	gl_texture_clear(0);			// progress can be called while baseutility loads bitmaps, need to reset the current bitmap so it doesn't get lost

	if (gl_check_texture_rectangle_ok()) {
		view_primitive_2D_texture_quad_rectangle(progress_background_bitmap.gl_id,1.0f,0,iface.scale_x,0,iface.scale_y,progress_background_bitmap.wid,progress_background_bitmap.high);
	}
	else {
		view_primitive_2D_texture_quad(progress_background_bitmap.gl_id,NULL,1.0f,0,iface.scale_x,0,iface.scale_y,0.0f,1.0f,0.0f,1.0f,TRUE);
	}
	
		// draw the progress background
		
	lft=iface.progress.lx;
	rgt=iface.progress.rx;
	top=iface.progress.ty;
	bot=iface.progress.by;

	mid=(top+bot)>>1;
	
	col.r=iface.progress.background_color.r*element_gradient_factor_foreground;
	col.g=iface.progress.background_color.g*element_gradient_factor_foreground;
	col.b=iface.progress.background_color.b*element_gradient_factor_foreground;

	view_primitive_2D_color_poly(lft,top,&col,rgt,top,&col,rgt,mid,&iface.progress.background_color,lft,mid,&iface.progress.background_color,1.0f);
	view_primitive_2D_color_poly(lft,mid,&iface.progress.background_color,rgt,mid,&iface.progress.background_color,rgt,bot,&col,lft,bot,&col,1.0f);
	
		// draw the progress foreground
	
	if (progress_current!=-1) {
		if (progress_current>=progress_max) {
			rgt2=rgt;
		}
		else {
			rgt2=lft+(((rgt-lft)*progress_current)/progress_max);
		}
		
		col.r=iface.progress.hilite_color.r*element_gradient_factor_foreground;
		col.g=iface.progress.hilite_color.g*element_gradient_factor_foreground;
		col.b=iface.progress.hilite_color.b*element_gradient_factor_foreground;

		view_primitive_2D_color_poly(lft,top,&iface.progress.hilite_color,rgt2,top,&iface.progress.hilite_color,rgt2,mid,&col,lft,mid,&col,1.0f);
		view_primitive_2D_color_poly(lft,mid,&col,rgt2,mid,&col,rgt2,bot,&iface.progress.hilite_color,lft,bot,&iface.progress.hilite_color,1.0f);
	}

		// progress outline

	if (iface.progress.outline) view_primitive_2D_line_quad(&iface.progress.outline_color,1.0f,lft,rgt,top,bot);

		// progress overlay

	if (iface.progress.overlay) view_primitive_2D_texture_quad(progress_overlay_bitmap.gl_id,NULL,1.0f,lft,rgt,top,bot,0.0f,1.0f,0.0f,1.0f,TRUE);

	gl_frame_swap();
}

