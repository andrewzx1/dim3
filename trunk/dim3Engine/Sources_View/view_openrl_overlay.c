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
#include "objects.h"

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
								view_rl_overlay;

extern texture_font_size_type* view_openrl_material_text_get_font(int text_font,int text_size);
extern int gl_text_get_monospace_width(texture_font_size_type *font,int text_size);
extern bool hud_item_fade_run(iface_item_fade_type *fade,float *alpha);

/* =======================================================

      View OpenRL Init/Shutdown
      
======================================================= */

void view_openrl_overlay_start(void)
{
	int						n;
	rlColor					col;
	rl2DPoint				p_pnt,s_pnt;
	iface_bitmap_type		*bitmap;
	iface_text_type			*text;
	iface_crosshair_type	*crosshair;
	texture_font_size_type	*font_size;

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

	p_pnt.x=0;
	p_pnt.y=0;

	rlSceneOverlaySetQuadCount(view_rl_scene_id,view_rl_overlay_crosshair_id,1);
	rlSceneOverlaySetQuadPosition(view_rl_scene_id,view_rl_overlay_crosshair_id,0,&p_pnt);
	rlSceneOverlaySetQuadSize(view_rl_scene_id,view_rl_overlay_crosshair_id,0,&s_pnt);
	rlSceneOverlaySetQuadColor(view_rl_scene_id,view_rl_overlay_crosshair_id,0,&col);

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

		p_pnt.x=0;
		p_pnt.y=0;

		rlSceneOverlaySetQuadCount(view_rl_scene_id,bitmap->openrl_overlay_id,1);
		rlSceneOverlaySetQuadPosition(view_rl_scene_id,bitmap->openrl_overlay_id,0,&p_pnt);
		rlSceneOverlaySetQuadSize(view_rl_scene_id,bitmap->openrl_overlay_id,0,&s_pnt);

		bitmap++;
	}

		// hud texts
		// start with no quads or position as
		// this is all set by whatever text we
		// have to draw
	
	p_pnt.x=0;
	p_pnt.y=0;
	s_pnt.x=0;
	s_pnt.y=0;

	text=iface.text_list.texts;
	
	for (n=0;n!=iface.text_list.ntext;n++) {
		font_size=view_openrl_material_text_get_font(font_hud_index,text->size);

		text->openrl_overlay_id=rlSceneOverlayAdd(view_rl_scene_id,font_size->openrl_material_id,0);
		rlSceneOverlaySetPosition(view_rl_scene_id,text->openrl_overlay_id,&p_pnt);
		rlSceneOverlaySetSize(view_rl_scene_id,text->openrl_overlay_id,&s_pnt);
		rlSceneOverlaySetHidden(view_rl_scene_id,text->openrl_overlay_id,TRUE);
		text++;
	}

		// overlays for fps

	p_pnt.x=5;
	p_pnt.y=setup.screen_openrl_high-25;
	s_pnt.x=15*5;
	s_pnt.y=20;

	col.r=1.0f;
	col.g=1.0f;
	col.b=0.0f;
	col.a=1.0f;

	font_size=view_openrl_material_text_get_font(font_hud_index,20);

	view_rl_overlay=rlSceneOverlayAdd(view_rl_scene_id,font_size->openrl_material_id,0);
	rlSceneOverlaySetPosition(view_rl_scene_id,view_rl_overlay,&p_pnt);
	rlSceneOverlaySetSize(view_rl_scene_id,view_rl_overlay,&s_pnt);

	rlSceneOverlaySetQuadCount(view_rl_scene_id,view_rl_overlay,5);

	p_pnt.x=0;
	p_pnt.y=0;
	s_pnt.x=15;
	s_pnt.y=20;

	for (n=0;n!=5;n++) {
		rlSceneOverlaySetQuadPosition(view_rl_scene_id,view_rl_overlay,n,&p_pnt);
		rlSceneOverlaySetQuadSize(view_rl_scene_id,view_rl_overlay,n,&s_pnt);
		rlSceneOverlaySetQuadColor(view_rl_scene_id,view_rl_overlay,n,&col);
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
	rlSceneOverlayDelete(view_rl_scene_id,view_rl_overlay);
	rlSceneOverlayDelete(view_rl_scene_id,view_rl_overlay_crosshair_id);
}

/* =======================================================

      OpenRL Overlay Bitmap Update
      
======================================================= */

void view_openrl_overlay_bitmap_to_overlay(iface_bitmap_type *bitmap)
{
	float				alpha;
	d3col				team_tint;
	obj_type			*obj;
	rlColor				col;

		// get the color
		// and alpha

	alpha=bitmap->alpha;
	if (hud_item_fade_run(&bitmap->fade,&alpha)) {
		bitmap->show=FALSE;			// a fade has turned off bitmap
		return;
	}

	if (!bitmap->team_tint) {
		col.r=col.g=col.b=1.0f;
	}
	else {
		obj=server.obj_list.objs[server.player_obj_idx];
		object_get_tint(obj,&team_tint);

		col.r=team_tint.r;
		col.g=team_tint.g;
		col.b=team_tint.b;
	}
	col.a=alpha;
	
		// set quad color

	rlSceneOverlaySetQuadColor(view_rl_scene_id,bitmap->openrl_overlay_id,0,&col);
}

/* =======================================================

      OpenRL Overlay Text Update
      
======================================================= */

void view_openrl_overlay_set_to_char(texture_font_size_type *font_size,int overlay_id,int index,int ch)
{
	int				xoff,yoff;
	rlUV			uv,uv_size;

		// check for non-defined characters

	if ((ch<'!') || (ch>'z')) {
		uv.x=0.0f;
		uv.y=0.0f;

		uv_size.x=0.0f;
		uv_size.y=0.0f;
	}

		// create UV for character

	else {
		ch-='!';

		yoff=ch/font_size->char_per_line;
		xoff=ch-(yoff*font_size->char_per_line);

		uv.x=((float)xoff)*font_size->gl_xoff;
		uv.y=((float)yoff)*font_size->gl_yoff;

		uv_size.x=font_size->gl_xadd;
		uv_size.y=font_size->gl_yadd;
	}

	rlSceneOverlaySetQuadUV(view_rl_scene_id,overlay_id,index,&uv);
	rlSceneOverlaySetQuadUVStamp(view_rl_scene_id,overlay_id,index,&uv_size);
}

bool view_openrl_overlay_text_to_overlay(iface_text_type *text)
{
	int						n,x,y,txt_len,txt_wid,ch,
							lft,top,wid,high;
	float					alpha;
	char					*c;
	texture_font_size_type	*font_size;
	rl2DPoint				p_pnt,s_pnt;
	rlColor					col;

		// apply specials
		// supergumba -- limited subset here

	if (text->special==text_special_fps) {
		hud_texts_fps(text->data);
	}

		// get text length

	txt_len=strlen(text->data);
	if (txt_len==0) return(FALSE);
	if (txt_len>255) txt_len=255;

		// reset overlay quads

	rlSceneOverlaySetQuadCount(view_rl_scene_id,text->openrl_overlay_id,txt_len);
	
		// get font
		
	font_size=view_openrl_material_text_get_font(font_hud_index,text->size);

        // font justification

	x=text->pnt.x;
	y=text->pnt.y;

	txt_wid=gl_text_get_string_width(font_hud_index,text->size,text->monospaced,text->data);
        
	switch (text->just) {
		case tx_center:
			x-=(txt_wid>>1);
			break;
		case tx_right:
			x-=txt_wid;
			break;
	}
	
		// get width and height
		
	if (text->monospaced) {
		wid=gl_text_get_monospace_width(font_size,text->size);
	}
	else {
		wid=text->size;
	}

	high=(int)(((float)text->size)*text_height_factor);

		// reset size of overlay

	p_pnt.x=(x*setup.screen_openrl_wid)/iface.scale_x;
	p_pnt.y=((y-high)*setup.screen_openrl_high)/iface.scale_y;
	rlSceneOverlaySetPosition(view_rl_scene_id,text->openrl_overlay_id,&p_pnt);

	s_pnt.x=(txt_wid*setup.screen_openrl_wid)/iface.scale_x;
	s_pnt.y=(high*setup.screen_openrl_high)/iface.scale_y;
	rlSceneOverlaySetSize(view_rl_scene_id,text->openrl_overlay_id,&s_pnt);

		// main quad size

	s_pnt.x=wid;
	s_pnt.y=high;

		// get the color
		// and alpha

	alpha=text->alpha;
	if (hud_item_fade_run(&text->fade,&alpha)) {
		text->show=FALSE;			// a fade has turned off text
		return(FALSE);
	}

	col.r=text->color.r;
	col.g=text->color.g;
	col.b=text->color.b;
	col.a=alpha;

		// create the quads
		// quads are offsets into overlays

	lft=0;
	top=-high;
	
	c=text->data;

	for (n=0;n!=txt_len;n++) {
	
		ch=(int)*c++;

			// the overlay quad

		p_pnt.x=(lft*setup.screen_openrl_wid)/iface.scale_x;
		p_pnt.y=(top*setup.screen_openrl_high)/iface.scale_y;

		rlSceneOverlaySetQuadPosition(view_rl_scene_id,text->openrl_overlay_id,n,&p_pnt);
		rlSceneOverlaySetQuadSize(view_rl_scene_id,text->openrl_overlay_id,n,&s_pnt);
		view_openrl_overlay_set_to_char(font_size,text->openrl_overlay_id,n,ch);
		rlSceneOverlaySetQuadColor(view_rl_scene_id,text->openrl_overlay_id,n,&col);

			// next character

		if (text->monospaced) {
			lft+=wid;
		}
		else {
			if ((ch<'!') || (ch>'z')) {
				lft+=(int)(((float)wid)*0.33f);
			}
			else {
				lft+=(int)(((float)wid)*font_size->char_size[ch-'!']);
			}
		}
	}

	return(TRUE);
}

/* =======================================================

      OpenRL Overlay Update
      
======================================================= */

void view_openrl_overlay_update(void)
{
	int						n;
	float					f;
	char					str[32];
	iface_bitmap_type		*bitmap;
	iface_text_type			*text;
	texture_font_size_type	*font_size;

		// update hud bitmaps

	bitmap=iface.bitmap_list.bitmaps;
	
	for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
		view_openrl_overlay_bitmap_to_overlay(bitmap);
		rlSceneOverlaySetHidden(view_rl_scene_id,bitmap->openrl_overlay_id,(!bitmap->show));
		bitmap++;
	}

		// hud text
		// if text string is break, always auto-hide

	text=iface.text_list.texts;
	
	for (n=0;n!=iface.text_list.ntext;n++) {
		if (!view_openrl_overlay_text_to_overlay(text)) {
			rlSceneOverlaySetHidden(view_rl_scene_id,text->openrl_overlay_id,TRUE);
		}
		else {
			rlSceneOverlaySetHidden(view_rl_scene_id,text->openrl_overlay_id,(!text->show));
		}
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
		strcat(str,"--.--");
	}
	else {
		f=1000.0f/((float)view_rl_msec_display);
		if (f<0.0f) f=0.0f;
		sprintf(str,"%.2f  ",f);
	}

	font_size=view_openrl_material_text_get_font(font_hud_index,20);

	for (n=0;n!=5;n++) {
		view_openrl_overlay_set_to_char(font_size,view_rl_overlay,n,str[n]);
	}
}

#endif