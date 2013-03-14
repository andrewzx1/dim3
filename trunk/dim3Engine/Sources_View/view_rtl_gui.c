/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View dim3RTL GUI

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

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

extern int						view_rtl_gui_scene_id;
extern GLuint					view_rtl_gui_gl_id;

int								gui_rtl_cursor_overlay_id,
								gui_rtl_cursor_material_id;

extern int view_dim3rtl_create_material_from_path(char *path,int alpha_type);
extern void view_dim3rtl_material_text_start(void);
extern void view_dim3rtl_material_text_stop(void);
extern void view_dim3rtl_transfer_to_opengl(int scene_id,int x,int y,int wid,int high,GLuint gl_id,int buff_wid,int buff_high);

/* =======================================================

      Start/Stop RTL GUI
      
======================================================= */

void gui_dim3rtl_initialize(void)
{
	int				sz;
	char			path[1024];
	rtl2DPoint		p_pnt,s_pnt;
	
		// load the cursor

	file_paths_data(&file_path_setup,path,"Bitmaps/UI_Elements","cursor","png");
	gui_rtl_cursor_material_id=view_dim3rtl_create_material_from_path(path,RL_MATERIAL_ALPHA_PASS_THROUGH);

	sz=(int)(((float)iface.scale_x)*cursor_size_factor);
	
	p_pnt.x=0;
	p_pnt.y=0;
	s_pnt.x=sz;
	s_pnt.y=sz;

	gui_rtl_cursor_overlay_id=rtlSceneOverlayAdd(view_rtl_gui_scene_id,gui_rtl_cursor_material_id,0);
	rtlSceneOverlaySetPosition(view_rtl_gui_scene_id,gui_rtl_cursor_overlay_id,&p_pnt);
	rtlSceneOverlaySetSize(view_rtl_gui_scene_id,gui_rtl_cursor_overlay_id,&s_pnt);
	rtlSceneOverlaySetQuadCount(view_rtl_gui_scene_id,gui_rtl_cursor_overlay_id,1);
	rtlSceneOverlaySetQuadPosition(view_rtl_gui_scene_id,gui_rtl_cursor_overlay_id,0,&p_pnt);
	rtlSceneOverlaySetQuadSize(view_rtl_gui_scene_id,gui_rtl_cursor_overlay_id,0,&s_pnt);
}

void gui_dim3rtl_shutdown(void)
{
	rtlSceneOverlayDelete(view_rtl_gui_scene_id,gui_rtl_cursor_overlay_id);
	rtlMaterialDelete(gui_rtl_cursor_material_id);
	// supergumba -- this causes problems with texture indexes, maybe not move lists to preserve lookup indexes?
}




void gui_dim3rtl_draw(float background_alpha,bool cursor)
{
	int					x,y,sz;
	rtl2DPoint			p_pnt;
	rtlColor			col;

	col.r=col.g=col.b=0.0f;
	col.a=1.0f;
	rtlSceneClearBuffer(view_rtl_gui_scene_id,&col);

		// cursor

	input_gui_get_position(&x,&y);

	sz=(int)(((float)iface.scale_x)*cursor_size_factor);
	
	p_pnt.x=x;
	p_pnt.y=y;
	rtlSceneOverlaySetPosition(view_rtl_gui_scene_id,gui_rtl_cursor_overlay_id,&p_pnt);
	
	gl_frame_clear(FALSE);
	
	rtlSceneRender(view_rtl_gui_scene_id);
	rtlSceneRenderFinish(view_rtl_gui_scene_id);
	view_dim3rtl_transfer_to_opengl(view_rtl_gui_scene_id,0,0,setup.screen_wid,setup.screen_high,view_rtl_gui_gl_id,setup.screen_wid,setup.screen_high);

}

