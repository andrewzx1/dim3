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

extern int						view_rtl_scene_id,
								view_rtl_lx,view_rtl_rx,
								view_rtl_ty,view_rtl_by;
extern GLuint					view_rtl_gl_id;

int								gui_rtl_cursor_material_id;

extern int view_dim3rtl_create_material_from_path(char *path,int alpha_type);
extern void view_dim3rtl_material_text_start(void);
extern void view_dim3rtl_material_text_stop(void);
extern void view_dim3rtl_transfer_to_opengl(void);

/* =======================================================

      Start/Stop RTL GUI
      
======================================================= */

void gui_dim3rtl_clear(void)
{
	rtlPoint	pnt;
	rtlMatrix	rot;
	char		path[1024];

		// load the cursor

	file_paths_data(&file_path_setup,path,"Bitmaps/UI_Elements","cursor","png");
	gui_rtl_cursor_material_id=view_dim3rtl_create_material_from_path(path,RL_MATERIAL_ALPHA_PASS_THROUGH);

		// clear everything out of scene

	pnt.x=pnt.y=pnt.z=0.0f;
	rtlMatrixIdentity(&rot);

	rtlSceneEyePositionSet(view_rtl_scene_id,&pnt,60.0f,&rot,100);
	rtlSceneLightDeleteAll(view_rtl_scene_id);
	rtlSceneMeshDeleteAll(view_rtl_scene_id);
	rtlSceneOverlayDeleteAll(view_rtl_scene_id);
}

/* supergumba
void gui_dim3rtl_shutdown(void)
{
	rtlMaterialDelete(gui_rtl_cursor_material_id);
}
*/

void gui_dim3rtl_draw(float background_alpha,bool cursor)
{
	int					x,y,sz,cursor_id;
	rtl2DPoint			p_pnt,s_pnt;
	rtlColor			col;

	col.r=col.g=col.b=0.0f;
	rtlSceneClearBuffer(view_rtl_scene_id,&col);

		// clear old overlays

	rtlSceneOverlayDeleteAll(view_rtl_scene_id);

		// cursor

	input_gui_get_position(&x,&y);

	sz=(int)(((float)iface.scale_x)*cursor_size_factor);

	x=(x*(view_rtl_rx-view_rtl_lx))/iface.scale_x;
	y=(y*(view_rtl_by-view_rtl_ty))/iface.scale_y;
		
	x=100;
	y=100;

	p_pnt.x=x;
	p_pnt.y=y;
	s_pnt.x=sz;
	s_pnt.y=sz;

	cursor_id=rtlSceneOverlayAdd(view_rtl_scene_id,gui_rtl_cursor_material_id,0);
	rtlSceneOverlaySetPosition(view_rtl_scene_id,cursor_id,&p_pnt);
	rtlSceneOverlaySetSize(view_rtl_scene_id,cursor_id,&s_pnt);

	p_pnt.x=0;
	p_pnt.y=0;

	rtlSceneOverlaySetQuadCount(view_rtl_scene_id,cursor_id,1);
	rtlSceneOverlaySetQuadPosition(view_rtl_scene_id,cursor_id,0,&p_pnt);
	rtlSceneOverlaySetQuadSize(view_rtl_scene_id,cursor_id,0,&s_pnt);

	rtlSceneRender(view_rtl_scene_id);
	rtlSceneRenderFinish(view_rtl_scene_id);
	view_dim3rtl_transfer_to_opengl();
}

