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
								view_rl_last_msec,view_rl_msec_display,
								view_rl_msec,view_rl_msec_count;

extern texture_font_type		view_rl_fonts[2];

int								view_rl_overlay_crosshair_id,
								view_rl_overlay[5];

extern texture_font_size_type* gl_text_get_font(int text_font,int text_size);
extern int gl_text_get_monospace_width(texture_font_size_type *font,int text_size);

/* =======================================================

      View OpenRL Init/Shutdown
      
======================================================= */

void view_openrl_overlay_start(void)
{
	int						n;
	rlColor					col;
	rl2DPoint				p_pnt,s_pnt;
	iface_bitmap_type		*bitmap;
	iface_crosshair_type	*crosshair;

		// crosshair
		// supergumba, this is all hardcoded for now

	crosshair=&iface.crosshair_list.crosshairs[0];

	p_pnt.x=(setup.screen_openrl_wid>>1)-5;
	p_pnt.y=(setup.screen_openrl_high>>1)-5;
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

		p_pnt.x=(bitmap->pnt.x*setup.screen_openrl_wid)/iface.scale_x;
		p_pnt.y=(bitmap->pnt.y*setup.screen_openrl_high)/iface.scale_y;
		s_pnt.x=(bitmap->size.x*setup.screen_openrl_wid)/iface.scale_x;
		s_pnt.y=(bitmap->size.y*setup.screen_openrl_high)/iface.scale_y;

		bitmap->openrl_overlay_id=rlSceneOverlayAdd(view_rl_scene_id,bitmap->openrl_material_id,0);
		rlSceneOverlaySetPosition(view_rl_scene_id,bitmap->openrl_overlay_id,&p_pnt);
		rlSceneOverlaySetSize(view_rl_scene_id,bitmap->openrl_overlay_id,&s_pnt);
		rlSceneOverlaySetHidden(view_rl_scene_id,bitmap->openrl_overlay_id,(!bitmap->show));

		bitmap++;
	}

		// overlays for fps

	p_pnt.x=5;
	p_pnt.y=setup.screen_openrl_high-25;
	s_pnt.x=15;
	s_pnt.y=20;

	col.r=1.0f;
	col.g=1.0f;
	col.b=0.0f;
	col.a=1.0f;

	for (n=0;n!=5;n++) {
		view_rl_overlay[n]=rlSceneOverlayAdd(view_rl_scene_id,view_rl_fonts[font_hud_index].size_24.openrl_material_id,0);
		rlSceneOverlaySetPosition(view_rl_scene_id,view_rl_overlay[n],&p_pnt);
		rlSceneOverlaySetSize(view_rl_scene_id,view_rl_overlay[n],&s_pnt);
		rlSceneOverlayColor(view_rl_scene_id,view_rl_overlay[n],&col);
		p_pnt.x+=s_pnt.x;
	}

		// fps

	view_rl_msec_count=0;
	view_rl_msec=0;
	view_rl_msec_display=0;
	view_rl_last_msec=game_time_get_raw();
}

void view_openrl_overlay_stop(void)
{
	int				n;

	for (n=0;n!=5;n++) {
		rlSceneOverlayDelete(view_rl_scene_id,view_rl_overlay[n]);
	}

	rlSceneOverlayDelete(view_rl_scene_id,view_rl_overlay_crosshair_id);
}

/* =======================================================

      OpenRL Overlay Character
      
======================================================= */

void view_openrl_overlay_set_to_char(texture_font_size_type *font_size,int overlay_id,char ch)
{
	int				xoff,yoff;
	rlUV			uv,uv_size;

	ch-='!';

	yoff=ch/font_size->char_per_line;
	xoff=ch-(yoff*font_size->char_per_line);

	uv.x=((float)xoff)*font_size->gl_xoff;
	uv.y=((float)yoff)*font_size->gl_yoff;
	rlSceneOverlaySetUV(view_rl_scene_id,overlay_id,&uv);

	uv_size.x=font_size->gl_xadd;
	uv_size.y=font_size->gl_yadd;
	rlSceneOverlaySetUVStamp(view_rl_scene_id,overlay_id,&uv_size);
}

void view_openrl_overlay_text_to_overlay(iface_text_type *text)
{
	int						n,x,y,txtlen,ch,overlay_id;
	float					f_lft,f_top,f_bot,f_wid,f_high;
	char					*c;
	texture_font_size_type	*font_size;
	rl2DPoint				p_pnt,s_pnt;

		// apply specials
		// supergumba -- limited subset here

	if (text->special==text_special_fps) {
		hud_texts_fps(text->data);
	}

		// get text length

	txtlen=strlen(text->data);
	if (txtlen==0) return;
	if (txtlen>255) txtlen=255;
	
		// get font
		
	font_size=gl_text_get_font(font_hud_index,text->size);

        // font justification

	x=text->pnt.x;
	y=text->pnt.y;
        
	switch (text->just) {
		case tx_center:
			x-=(gl_text_get_string_width(font_hud_index,text->size,text->monospaced,text->data)>>1);
			break;
		case tx_right:
			x-=gl_text_get_string_width(font_hud_index,text->size,text->monospaced,text->data);
			break;
	}
	
		// get width and height
		
	if (text->monospaced) {
		f_wid=(float)gl_text_get_monospace_width(font_size,text->size);
	}
	else {
		f_wid=(float)text->size;
	}

	f_high=((float)text->size)*text_height_factor;

		// create the quads

	f_lft=(float)x;
	f_bot=(float)y;
//	if (vcenter) f_bot+=((f_high/2)+(f_high/8));		// add in middle + descender
	f_top=f_bot-f_high;
	
	c=text->data;
	text->openrl_overlay_count=0;

	s_pnt.x=(f_wid*setup.screen_openrl_wid)/iface.scale_x;
	s_pnt.y=(f_high*setup.screen_openrl_high)/iface.scale_y;
	
	for (n=0;n<txtlen;n++) {
	
		ch=(int)*c++;

		if ((ch<'!') || (ch>'z')) {
			f_lft+=(f_wid/3);
			continue;
		}

			// the overlay quad

		p_pnt.x=(f_lft*setup.screen_openrl_wid)/iface.scale_x;
		p_pnt.y=(f_top*setup.screen_openrl_high)/iface.scale_y;

		overlay_id=rlSceneOverlayAdd(view_rl_scene_id,font_size->openrl_material_id,0);
		rlSceneOverlaySetPosition(view_rl_scene_id,overlay_id,&p_pnt);
		rlSceneOverlaySetSize(view_rl_scene_id,overlay_id,&s_pnt);
		view_openrl_overlay_set_to_char(font_size,overlay_id,ch);

		text->openrl_overlay_ids[text->openrl_overlay_count]=overlay_id;

		if (text->monospaced) {
			f_lft+=f_wid;
		}
		else {
			f_lft+=(f_wid*font_size->char_size[ch]);
		}

		text->openrl_overlay_count++;
	}
}

void view_openrl_overlay_text_to_overlay_clear(iface_text_type *text)
{
	int				n;

	for (n=0;n!=text->openrl_overlay_count;n++) {
		rlSceneOverlayDelete(view_rl_scene_id,text->openrl_overlay_ids[n]);
	}
}

/* =======================================================

      OpenRL Overlay Update
      
======================================================= */

void view_openrl_overlay_update(void)
{
	int					n;
	char				str[32];
	iface_bitmap_type	*bitmap;
	iface_text_type		*text;

		// update hud bitmaps

	bitmap=iface.bitmap_list.bitmaps;
	
	for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
		rlSceneOverlaySetHidden(view_rl_scene_id,bitmap->openrl_overlay_id,(!bitmap->show));
		bitmap++;
	}

		// hud text

	text=iface.text_list.texts;
	
	for (n=0;n!=iface.text_list.ntext;n++) {
		view_openrl_overlay_text_to_overlay(text);
		text++;
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

	if (view_rl_msec_display==0) {
		strcat(str,"-.--");
	}
	else {
		sprintf(str,"%.2f",(1000.0f/((float)view_rl_msec_display)));
	}

	for (n=0;n!=5;n++) {
		view_openrl_overlay_set_to_char(&view_rl_fonts[font_hud_index].size_24,view_rl_overlay[n],str[n]);
	}
}

void view_openrl_overlay_cleanup(void)
{
	int					n;
	iface_text_type		*text;

		// remove text overlays

	text=iface.text_list.texts;
	
	for (n=0;n!=iface.text_list.ntext;n++) {
		view_openrl_overlay_text_to_overlay_clear(text);
		text++;
	}
}

#endif