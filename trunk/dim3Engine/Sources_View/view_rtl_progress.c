/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View dim3RTL Progress

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
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "ray_interface.h"

extern map_type					map;
extern server_type				server;
extern camera_type				camera;
extern view_type				view;
extern iface_type				iface;
extern setup_type				setup;
extern network_setup_type		net_setup;
extern file_path_setup_type		file_path_setup;

extern int						view_rtl_gui_scene_id;
extern GLuint					view_rtl_gui_gl_id;

int								progress_rtl_background_material_id,
								progress_rtl_bitmap_material_id;

extern int view_dim3rtl_create_material_from_path(char *path,int alpha_type);
extern void gui_dim3rtl_add_overlay_box_material(int x,int y,int wid,int high,float alpha,int material_id);
extern void view_dim3rtl_transfer_to_opengl(int scene_id,int x,int y,int wid,int high,GLuint gl_id,int buff_wid,int buff_high);

/* =======================================================

      Setup Progress
      
======================================================= */

void progress_dim3rtl_initialize(char *map_name)
{
	char			path[1024];
	rtl2DPoint		scale;

		// clear any overlays
		
	rtlSceneOverlayDeleteAll(view_rtl_gui_scene_id);
	
		// set the scale
		
	scale.x=iface.scale_x;
	scale.y=iface.scale_y;

	rtlSceneOverlaySetScale(view_rtl_gui_scene_id,&scale);
	
		// check for map progress background,
		// otherwise use default
		
	progress_rtl_background_material_id=-1;

	if (map_name!=NULL) {
		file_paths_data(&file_path_setup,path,"Bitmaps/Backgrounds_Map",map_name,"png");
		progress_rtl_background_material_id=view_dim3rtl_create_material_from_path(path,RL_MATERIAL_ALPHA_PASS_THROUGH);
	}

	if (progress_rtl_background_material_id==-1) {
		view_file_paths_bitmap_check_wide(path,"Bitmaps/Backgrounds","load");
		progress_rtl_background_material_id=view_dim3rtl_create_material_from_path(path,RL_MATERIAL_ALPHA_PASS_THROUGH);
	}

		// progress bitmap
		
	progress_rtl_bitmap_material_id=-1;

	if (iface.progress.bitmap_name[0]!=0x0) {
		file_paths_data(&file_path_setup,path,"Bitmaps/Interface",iface.progress.bitmap_name,"png");
		progress_rtl_bitmap_material_id=view_dim3rtl_create_material_from_path(path,RL_MATERIAL_ALPHA_PASS_THROUGH);
	}
}

void progress_dim3rtl_shutdown(void)
{
	if (progress_rtl_background_material_id!=-1) rtlMaterialDelete(progress_rtl_background_material_id);
	if (progress_rtl_bitmap_material_id!=-1) rtlMaterialDelete(progress_rtl_bitmap_material_id);
}

/* =======================================================

      Draw Progress
      
======================================================= */

void progress_dim3rtl_draw(int cur_tick)
{
	int				overlay_id;
	float			gx,gy,g_size;
	rtl2DPoint		p_pnt,s_pnt;
	rtlUV			p_uv,s_uv;
	rtlColor		col;

		// clear buffer

	col.r=iface.color.background.r;
	col.g=iface.color.background.g;
	col.b=iface.color.background.b;
	col.a=1.0f;
	
	rtlSceneClearBuffer(view_rtl_gui_scene_id,&col);

		// every frame we redo the entire
		// overlay system.  This is NOT optimal, but
		// it's a GUI and it's the easiest way to
		// retrofit in with the OpenGL stuff

	rtlSceneOverlayDeleteAll(view_rtl_gui_scene_id);

		// add the background

	if (progress_rtl_background_material_id!=-1) gui_dim3rtl_add_overlay_box_material(0,0,iface.scale_x,iface.scale_y,1.0f,progress_rtl_background_material_id);

		// draw the progress bitmap
		
	if (progress_rtl_bitmap_material_id!=-1) {

		overlay_id=rtlSceneOverlayAdd(view_rtl_gui_scene_id,RL_OVERLAY_TYPE_QUAD_MATERIAL,0);
		rtlSceneOverlaySetMaterial(view_rtl_gui_scene_id,overlay_id,progress_rtl_bitmap_material_id);

		p_pnt.x=iface.progress.x;
		p_pnt.y=iface.progress.y;
		s_pnt.x=iface.progress.wid;
		s_pnt.y=iface.progress.high;

		rtlSceneOverlaySetQuadPosition(view_rtl_gui_scene_id,overlay_id,&p_pnt,&s_pnt);

		effect_image_animate_get_uv(cur_tick,0,&iface.progress.animate,&gx,&gy,&g_size);

		p_uv.x=gx;
		p_uv.y=gy;
		s_uv.x=s_uv.y=g_size;

		rtlSceneOverlaySetQuadUV(view_rtl_gui_scene_id,overlay_id,&p_uv,&s_uv);
	}

		// draw the overlay, there
		// is no 3D to render here

	rtlSceneOverlayDraw(view_rtl_gui_scene_id);
	view_dim3rtl_transfer_to_opengl(view_rtl_gui_scene_id,0,0,setup.screen_wid,setup.screen_high,view_rtl_gui_gl_id,setup.screen_wid,setup.screen_high);
}


