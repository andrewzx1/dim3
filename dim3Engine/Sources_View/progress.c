/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Progress Routines

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

#include "video.h"

extern map_type				map;
extern setup_type			setup;
extern hud_type				hud;

float						progress_current;
char						progress_str[256];
bitmap_type					progress_bitmap;

/* =======================================================

      Setup Progress
      
======================================================= */

void progress_initialize(char *action)
{
	char			path[1024];
	
		// progress text
		
	strcpy(progress_str,action);
	
		// check for map progress background,
		// otherwise use default
		
	file_paths_data(&setup.file_path_setup,path,"Bitmaps/Backgrounds_Map",map.info.name,"png");
	if (!bitmap_open(&progress_bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,gl_check_texture_rectangle_ok(),FALSE,FALSE)) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Backgrounds","load","png");
		bitmap_open(&progress_bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,gl_check_texture_rectangle_ok(),FALSE,FALSE);
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
	int					lft,rgt,top,bot,mid,rgt2,x,y;

		// any change?
		
	if (progress_current==percentage) return;
	
	progress_current=percentage;
	
		// start the frame
	
	gl_frame_clear(FALSE);
	
		// setup draw
		
	gl_2D_view_interface();
	
		// draw background

	if (gl_check_texture_rectangle_ok()) {
		view_draw_next_vertex_object_2D_texture_quad_rectangle(progress_bitmap.gl_id,1.0f,0,hud.scale_x,0,hud.scale_y,progress_bitmap.wid,progress_bitmap.high);
	}
	else {
		view_draw_next_vertex_object_2D_texture_quad(progress_bitmap.gl_id,NULL,1.0f,0,hud.scale_x,0,hud.scale_y,0.0f,0.0f);
	}
	
		// draw the progress background
		
	lft=hud.progress.lx;
	rgt=hud.progress.rx;
	top=hud.progress.ty;
	bot=hud.progress.by;

	mid=(top+bot)>>1;

	view_draw_next_vertex_object_2D_color_poly(lft,top,&hud.progress.base_color_start,rgt,top,&hud.progress.base_color_start,rgt,mid,&hud.progress.base_color_end,lft,mid,&hud.progress.base_color_end,1.0f);
	view_draw_next_vertex_object_2D_color_poly(lft,mid,&hud.progress.base_color_end,rgt,mid,&hud.progress.base_color_end,rgt,bot,&hud.progress.base_color_start,lft,bot,&hud.progress.base_color_start,1.0f);
	
		// draw the progress foreground
	
	rgt2=lft+(int)((float)(rgt-lft)*(percentage/100));
	
	view_draw_next_vertex_object_2D_color_poly(lft,top,&hud.progress.hilite_color_start,rgt2,top,&hud.progress.hilite_color_start,rgt2,mid,&hud.progress.hilite_color_end,lft,mid,&hud.progress.hilite_color_end,1.0f);
	view_draw_next_vertex_object_2D_color_poly(lft,mid,&hud.progress.hilite_color_end,rgt2,mid,&hud.progress.hilite_color_end,rgt2,bot,&hud.progress.hilite_color_start,lft,bot,&hud.progress.hilite_color_start,1.0f);

		// progress text

	x=(hud.progress.lx+hud.progress.rx)>>1;
	y=(hud.progress.ty+hud.progress.by)>>1;
	
	gl_text_start(hud.progress.text_size);
	gl_text_draw(x,y,progress_str,tx_center,TRUE,&hud.progress.text_color,1.0f);
	gl_text_end();

	gl_frame_swap();
}

