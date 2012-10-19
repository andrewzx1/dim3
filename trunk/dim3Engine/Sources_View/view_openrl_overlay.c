/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View OpenRL Overlays

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

#ifdef D3_OPENRL

#include "ray_interface.h"

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

extern int						view_rl_scene_id,
								view_rl_font_material_id,
								view_rl_last_msec,view_rl_msec_display,
								view_rl_msec,view_rl_msec_count;

extern texture_font_size_type	view_rl_font;

int								view_rl_overlay_crosshair_id,
								view_rl_overlay[4];

/* =======================================================

      View OpenRL Init/Shutdown
      
======================================================= */

void view_openrl_overlay_start(int wid,int high)
{
	int						n;
	rlColor					col;
	rl2DPoint				p_pnt,s_pnt;
	iface_bitmap_type		*bitmap;
	iface_crosshair_type	*crosshair;

		// crosshair
		// supergumba, this is all hardcoded for now

	crosshair=&iface.crosshair_list.crosshairs[0];

	p_pnt.x=(wid>>1)-5;
	p_pnt.y=(high>>1)-5;
	s_pnt.x=s_pnt.y=10;

	col.r=1.0f;
	col.g=1.0f;
	col.b=0.0f;
	col.a=1.0f;

	view_rl_overlay_crosshair_id=rlSceneOverlayAdd(view_rl_scene_id,crosshair->openrl_material_id,0);
	rlSceneOverlaySetPosition(view_rl_scene_id,view_rl_overlay_crosshair_id,&p_pnt);
	rlSceneOverlaySetSize(view_rl_scene_id,view_rl_overlay_crosshair_id,&s_pnt);
	rlSceneOverlayColor(view_rl_scene_id,view_rl_overlay_crosshair_id,&col);

		// hud bitmaps

	bitmap=iface.bitmap_list.bitmaps;
	
	for (n=0;n!=iface.bitmap_list.nbitmap;n++) {

		p_pnt.x=(bitmap->pnt.x*wid)/iface.scale_x;
		p_pnt.y=(bitmap->pnt.y*high)/iface.scale_y;
		s_pnt.x=(bitmap->size.x*wid)/iface.scale_x;
		s_pnt.y=(bitmap->size.y*high)/iface.scale_y;

		bitmap->openrl_mesh_id=rlSceneOverlayAdd(view_rl_scene_id,bitmap->openrl_material_id,0);
		rlSceneOverlaySetPosition(view_rl_scene_id,bitmap->openrl_mesh_id,&p_pnt);
		rlSceneOverlaySetSize(view_rl_scene_id,bitmap->openrl_mesh_id,&s_pnt);
		rlSceneOverlaySetHidden(view_rl_scene_id,bitmap->openrl_mesh_id,(!bitmap->show));

		bitmap++;
	}

		// overlays for fps

	p_pnt.x=5;
	p_pnt.y=high-25;
	s_pnt.x=15;
	s_pnt.y=20;

	col.r=1.0f;
	col.g=1.0f;
	col.b=0.0f;
	col.a=1.0f;

	for (n=0;n!=4;n++) {
		view_rl_overlay[n]=rlSceneOverlayAdd(view_rl_scene_id,view_rl_font_material_id,0);
		rlSceneOverlaySetPosition(view_rl_scene_id,view_rl_overlay[n],&p_pnt);
		rlSceneOverlaySetSize(view_rl_scene_id,view_rl_overlay[n],&s_pnt);
		rlSceneOverlayColor(view_rl_scene_id,view_rl_overlay[n],&col);
		p_pnt.x+=s_pnt.x;
	}
}

void view_openrl_overlay_end(void)
{
	int				n;

	for (n=0;n!=4;n++) {
		rlSceneOverlayDelete(view_rl_scene_id,view_rl_overlay[n]);
	}

	rlSceneOverlayDelete(view_rl_scene_id,view_rl_overlay_crosshair_id);
}

/* =======================================================

      OpenRL Overlay Update
      
======================================================= */

void view_openrl_overlay_update(void)
{
	int					n,xoff,yoff;
	rlUV				uv,uv_size;
	char				ch,str[32];
	iface_bitmap_type	*bitmap;

		// update hud bitmaps

	bitmap=iface.bitmap_list.bitmaps;
	
	for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
		rlSceneOverlaySetHidden(view_rl_scene_id,bitmap->openrl_mesh_id,(!bitmap->show));
		bitmap++;
	}

		// update timing

	view_rl_msec_count++;
	view_rl_msec+=(game_time_get_raw()-view_rl_last_msec);

	if (view_rl_msec_count==10) {
		view_rl_msec_display=view_rl_msec/view_rl_msec_count;
		view_rl_msec=0;
		view_rl_msec_count=0;
	}

		// update overlay

	sprintf(str,"%4d",view_rl_msec_display);

	for (n=0;n!=4;n++) {
		ch=str[n]-'!';

		yoff=ch/view_rl_font.char_per_line;
		xoff=ch-(yoff*view_rl_font.char_per_line);

		uv.x=((float)xoff)*view_rl_font.gl_xoff;
		uv.y=((float)yoff)*view_rl_font.gl_yoff;
		rlSceneOverlaySetUV(view_rl_scene_id,view_rl_overlay[n],&uv);

		uv_size.x=view_rl_font.gl_xadd;
		uv_size.y=view_rl_font.gl_yadd;
		rlSceneOverlaySetUVStamp(view_rl_scene_id,view_rl_overlay[n],&uv_size);
	}
}

#endif