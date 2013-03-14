/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View dim3RTL Overlays

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
#include "ray_interface.h"

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

extern int						view_rtl_draw_scene_id;

extern texture_font_type		view_rtl_fonts[2];

int								view_rtl_overlay_crosshair_id;

extern texture_font_size_type* view_dim3rtl_material_text_get_font(int text_font,int text_size);
extern int gl_text_get_monospace_width(texture_font_size_type *font,int text_size);
extern bool hud_item_fade_run(iface_item_fade_type *fade,float *alpha);

/* =======================================================

      View dim3RTL Init/Shutdown
      
======================================================= */

void view_dim3rtl_overlay_start(void)
{
	int						n;
	rtl2DPoint				p_pnt,s_pnt;
	iface_bitmap_type		*bitmap;
	iface_text_type			*text;
	iface_crosshair_type	*crosshair;
	texture_font_size_type	*font_size;

	if (setup.no_hud) return;

		// hud bitmaps

	bitmap=iface.bitmap_list.bitmaps;
	
	for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
		p_pnt.x=(bitmap->pnt.x*setup.screen_rtl_wid)/iface.scale_x;
		p_pnt.y=(bitmap->pnt.y*setup.screen_rtl_high)/iface.scale_y;
		s_pnt.x=(bitmap->size.x*setup.screen_rtl_wid)/iface.scale_x;
		s_pnt.y=(bitmap->size.y*setup.screen_rtl_high)/iface.scale_y;

		bitmap->rtl_overlay_id=rtlSceneOverlayAdd(view_rtl_draw_scene_id,bitmap->rtl_material_id,0);
		rtlSceneOverlaySetPosition(view_rtl_draw_scene_id,bitmap->rtl_overlay_id,&p_pnt);
		rtlSceneOverlaySetSize(view_rtl_draw_scene_id,bitmap->rtl_overlay_id,&s_pnt);
		rtlSceneOverlaySetHidden(view_rtl_draw_scene_id,bitmap->rtl_overlay_id,(!bitmap->show));

		p_pnt.x=0;
		p_pnt.y=0;

		rtlSceneOverlaySetQuadCount(view_rtl_draw_scene_id,bitmap->rtl_overlay_id,1);
		rtlSceneOverlaySetQuadPosition(view_rtl_draw_scene_id,bitmap->rtl_overlay_id,0,&p_pnt);
		rtlSceneOverlaySetQuadSize(view_rtl_draw_scene_id,bitmap->rtl_overlay_id,0,&s_pnt);
		bitmap++;

		progress_update();
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
		font_size=view_dim3rtl_material_text_get_font(font_hud_index,text->size);

		text->rtl_overlay_id=rtlSceneOverlayAdd(view_rtl_draw_scene_id,font_size->rtl_material_id,0);
		rtlSceneOverlaySetPosition(view_rtl_draw_scene_id,text->rtl_overlay_id,&p_pnt);
		rtlSceneOverlaySetSize(view_rtl_draw_scene_id,text->rtl_overlay_id,&s_pnt);
		rtlSceneOverlaySetHidden(view_rtl_draw_scene_id,text->rtl_overlay_id,TRUE);
		text++;

		progress_update();
	}

		// crosshair

	crosshair=&iface.crosshair_list.crosshairs[0];

	p_pnt.x=p_pnt.y=0;
	s_pnt.x=s_pnt.y=10;

	view_rtl_overlay_crosshair_id=rtlSceneOverlayAdd(view_rtl_draw_scene_id,crosshair->rtl_material_id,0);
	rtlSceneOverlaySetPosition(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,&p_pnt);
	rtlSceneOverlaySetSize(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,&s_pnt);

	rtlSceneOverlaySetQuadCount(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,1);
	rtlSceneOverlaySetQuadPosition(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,0,&p_pnt);
	rtlSceneOverlaySetQuadSize(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,0,&s_pnt);

	rtlSceneOverlaySetHidden(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,TRUE);

	progress_update();
}

void view_dim3rtl_overlay_stop(void)
{
	int						n;
	iface_bitmap_type		*bitmap;
	iface_text_type			*text;

	if (setup.no_hud) return;

		// remove bitmaps

	bitmap=iface.bitmap_list.bitmaps;
	
	for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
		rtlSceneOverlayDelete(view_rtl_draw_scene_id,bitmap->rtl_overlay_id);
		bitmap++;

		progress_update();
	}

		// remove texts
	
	text=iface.text_list.texts;
	
	for (n=0;n!=iface.text_list.ntext;n++) {
		rtlSceneOverlayDelete(view_rtl_draw_scene_id,text->rtl_overlay_id);
		text++;

		progress_update();
	}

		// remove crosshair

	rtlSceneOverlayDelete(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id);
	progress_update();
}

/* =======================================================

      dim3RTL Overlay Bitmap Update
      
======================================================= */

void view_dim3rtl_overlay_bitmap_to_overlay(iface_bitmap_type *bitmap)
{
	float				alpha;
	d3col				team_tint;
	obj_type			*obj;
	rtlColor			col;

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

	rtlSceneOverlaySetQuadColor(view_rtl_draw_scene_id,bitmap->rtl_overlay_id,0,&col);
}

/* =======================================================

      dim3RTL Overlay Text Update
      
======================================================= */

void view_dim3rtl_overlay_set_to_char(texture_font_size_type *font_size,int overlay_id,int index,int ch)
{
	int				xoff,yoff;
	rtlUV			uv,uv_size;

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

	rtlSceneOverlaySetQuadUV(view_rtl_draw_scene_id,overlay_id,index,&uv);
	rtlSceneOverlaySetQuadUVStamp(view_rtl_draw_scene_id,overlay_id,index,&uv_size);
}

bool view_dim3rtl_overlay_text_to_overlay(iface_text_type *text)
{
	int						n,x,y,txt_len,txt_wid,ch,
							lft,wid,high;
	float					alpha;
	char					*c;
	texture_font_size_type	*font_size;
	rtl2DPoint				p_pnt,s_pnt;
	rtlColor				col;

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

	rtlSceneOverlaySetQuadCount(view_rtl_draw_scene_id,text->rtl_overlay_id,txt_len);
	
		// get font
		
	font_size=view_dim3rtl_material_text_get_font(font_hud_index,text->size);

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

	p_pnt.x=(x*setup.screen_rtl_wid)/iface.scale_x;
	p_pnt.y=((y-high)*setup.screen_rtl_high)/iface.scale_y;
	rtlSceneOverlaySetPosition(view_rtl_draw_scene_id,text->rtl_overlay_id,&p_pnt);

	s_pnt.x=(txt_wid*setup.screen_rtl_wid)/iface.scale_x;
	s_pnt.y=(high*setup.screen_rtl_high)/iface.scale_y;
	rtlSceneOverlaySetSize(view_rtl_draw_scene_id,text->rtl_overlay_id,&s_pnt);

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

		// main quad size

	s_pnt.x=(wid*setup.screen_rtl_wid)/iface.scale_x;
	s_pnt.y=(high*setup.screen_rtl_high)/iface.scale_y;

		// create the quads
		// quads are offsets into overlays

	lft=0;
	
	c=text->data;

	for (n=0;n!=txt_len;n++) {
	
		ch=(int)*c++;

			// the overlay quad

		p_pnt.x=(lft*setup.screen_rtl_wid)/iface.scale_x;
		p_pnt.y=0;

		rtlSceneOverlaySetQuadPosition(view_rtl_draw_scene_id,text->rtl_overlay_id,n,&p_pnt);
		rtlSceneOverlaySetQuadSize(view_rtl_draw_scene_id,text->rtl_overlay_id,n,&s_pnt);
		view_dim3rtl_overlay_set_to_char(font_size,text->rtl_overlay_id,n,ch);
		rtlSceneOverlaySetQuadColor(view_rtl_draw_scene_id,text->rtl_overlay_id,n,&col);

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

      dim3RTL Overlay CrossHair Update
      
======================================================= */

int view_dim3rtl_overlay_crosshair_setup_click(obj_type *obj,rtl2DPoint *p_pnt,rtl2DPoint *s_pnt,rtlColor *col)
{
	int				sz,idx;

	sz=setup.screen_rtl_wid/20;

	p_pnt->x=(setup.screen_rtl_wid>>1)-sz;
	p_pnt->y=(setup.screen_rtl_high>>1)-sz;
	s_pnt->x=s_pnt->y=sz*2;

	col->r=col->g=col->b=col->a=1.0f;
	
	if (obj->crosshair_draw.alt_tick>game_time_get()) {
		idx=obj->click.crosshair_down_idx;
	}
	else {
		idx=obj->click.crosshair_up_idx;
	}

	if (idx==-1) return(-1);

	return(iface.crosshair_list.crosshairs[idx].rtl_material_id);
}

int view_dim3rtl_overlay_crosshair_setup_weapon(obj_type *obj,weapon_type *weap,rtl2DPoint *p_pnt,rtl2DPoint *s_pnt,rtlColor *col)
{
	int				x,y,sz,dist,obj_idx,
					item_count,weap_mode,move_tick,swap_tick;
	
		// weapon displays a crosshair?

	if (obj->hide_all_weapons) return(-1);
	if (!weap->crosshair.on) return(-1);
	if (weap->crosshair.fire_idx==-1) return(-1);
	if ((weap->zoom.on) && (weap->zoom.mode!=zoom_mode_off)) return(-1);
	
		// get crosshair location

	if (!crosshair_get_location(obj,weap,&x,&y,&obj_idx,&dist)) return(-1);
	obj->crosshair_draw.aim_obj_idx=obj_idx;

		// we can't handle some of this as it's
		// tied to the OpenGL portion, so lets
		// just pretend everything is centered
		
	sz=(weap->crosshair.min_size*setup.screen_rtl_wid)/iface.scale_x;
	
	p_pnt->x=(setup.screen_rtl_wid>>1)-sz;
	p_pnt->y=(setup.screen_rtl_high>>1)-sz;

	s_pnt->x=s_pnt->y=sz*2;
	
		// change color if picking up an item or empty

	item_count=obj->item_count;
	if (item_count!=0) {
		col->r=weap->crosshair.pickup_col.r;
		col->g=weap->crosshair.pickup_col.g;
		col->b=weap->crosshair.pickup_col.b;
	}
	else {
		if (weap->ammo.count!=0) {
			col->r=weap->crosshair.col.r;
			col->g=weap->crosshair.col.g;
			col->b=weap->crosshair.col.b;
		}
		else {
			col->r=weap->crosshair.empty_col.r;
			col->g=weap->crosshair.empty_col.g;
			col->b=weap->crosshair.empty_col.b;
		}
	}
	
	col->a=1.0f;
	
		// crosshair alpha if weapon changing
	
	weap_mode=obj->held_weapon.mode;
	swap_tick=obj->held_weapon.swap_tick;
	
	move_tick=game_time_get();
	
    if (weap_mode==wm_lower) {
        move_tick-=swap_tick;
        col->a=(float)(weap->hand.raise_tick-move_tick)/(float)weap->hand.raise_tick;
	}
    if (weap_mode==wm_raise) {
        move_tick-=swap_tick;
        col->a=1.0f-((float)(weap->hand.raise_tick-move_tick)/(float)weap->hand.raise_tick);
	}
	
		// regular weapon crosshair
		
	return(iface.crosshair_list.crosshairs[weap->crosshair.fire_idx].rtl_material_id);
}

void view_dim3rtl_overlay_crosshair_setup(void)
{
	int				material_id;
	obj_type		*obj;
	weapon_type		*weap;
	rtl2DPoint		p_pnt,s_pnt;
	rtlColor		col;

	obj=server.obj_list.objs[server.player_obj_idx];
	weap=weapon_find_current(obj);

	if (weap==NULL) return;

		// get cursor

	if (obj->click.current_click_obj_idx!=-1) {
		material_id=view_dim3rtl_overlay_crosshair_setup_click(obj,&p_pnt,&s_pnt,&col);
	}
	else {
		material_id=view_dim3rtl_overlay_crosshair_setup_weapon(obj,weap,&p_pnt,&s_pnt,&col);
	}

	if (material_id==-1) {
		rtlSceneOverlaySetHidden(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,TRUE);
		return;
	}

		// set cursor overlay

	rtlSceneOverlaySetMaterial(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,material_id);
	rtlSceneOverlaySetHidden(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,FALSE);

	rtlSceneOverlaySetPosition(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,&p_pnt);
	rtlSceneOverlaySetSize(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,&s_pnt);

	rtlSceneOverlaySetQuadSize(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,0,&s_pnt);
	rtlSceneOverlaySetQuadColor(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,0,&col);
}

/* =======================================================

      dim3RTL Overlay Update
      
======================================================= */

void view_dim3rtl_overlay_update(void)
{
	int						n;
	iface_bitmap_type		*bitmap;
	iface_text_type			*text;

	if (setup.no_hud) return;

		// update hud bitmaps

	bitmap=iface.bitmap_list.bitmaps;
	
	for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
		view_dim3rtl_overlay_bitmap_to_overlay(bitmap);
		rtlSceneOverlaySetHidden(view_rtl_draw_scene_id,bitmap->rtl_overlay_id,(!bitmap->show));
		bitmap++;
	}

		// hud text
		// if text string is break, always auto-hide

	text=iface.text_list.texts;
	
	for (n=0;n!=iface.text_list.ntext;n++) {
		if (!view_dim3rtl_overlay_text_to_overlay(text)) {
			rtlSceneOverlaySetHidden(view_rtl_draw_scene_id,text->rtl_overlay_id,TRUE);
		}
		else {
			rtlSceneOverlaySetHidden(view_rtl_draw_scene_id,text->rtl_overlay_id,(!text->show));
		}
		text++;
	}

		// crosshair

	view_dim3rtl_overlay_crosshair_setup();
}
