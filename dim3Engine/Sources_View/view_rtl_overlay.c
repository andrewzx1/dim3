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

// supergumba -- for now we have a max limit for text so we don't chew up too many positions
#define rtl_temp_text_max_len				32

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
	int						n,k;
	rtl2DPoint				p_pnt,s_pnt;
	iface_bitmap_type		*bitmap;
	iface_text_type			*text;
	iface_crosshair_type	*crosshair;
	texture_font_size_type	*font_size;

	if (setup.no_hud) return;

		// hud bitmaps

	bitmap=iface.bitmap_list.bitmaps;
	
	for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
		p_pnt.x=bitmap->pnt.x;
		p_pnt.y=bitmap->pnt.y;
		s_pnt.x=bitmap->size.x;
		s_pnt.y=bitmap->size.y;

		bitmap->rtl_overlay_id=rtlSceneOverlayAdd(view_rtl_draw_scene_id,RL_OVERLAY_TYPE_QUAD_MATERIAL,0);
		rtlSceneOverlaySetMaterial(view_rtl_draw_scene_id,bitmap->rtl_overlay_id,bitmap->rtl_material_id);
		rtlSceneOverlaySetQuadPosition(view_rtl_draw_scene_id,bitmap->rtl_overlay_id,&p_pnt,&s_pnt);
		rtlSceneOverlaySetHidden(view_rtl_draw_scene_id,bitmap->rtl_overlay_id,(!bitmap->show));

		bitmap++;

		progress_update();
	}

		// hud texts
		// start with no quads or position as
		// this is all set by whatever text we
		// have to draw

	text=iface.text_list.texts;
	
	for (n=0;n!=iface.text_list.ntext;n++) {
		font_size=view_dim3rtl_material_text_get_font(font_hud_index,text->size);

		for (k=0;k!=rtl_temp_text_max_len;k++) {
			text->rtl_overlay_id[k]=rtlSceneOverlayAdd(view_rtl_draw_scene_id,RL_OVERLAY_TYPE_QUAD_MATERIAL,0);
			rtlSceneOverlaySetMaterial(view_rtl_draw_scene_id,text->rtl_overlay_id[k],font_size->rtl_material_id);
			rtlSceneOverlaySetHidden(view_rtl_draw_scene_id,text->rtl_overlay_id[k],TRUE);
		}

		text++;

		progress_update();
	}

		// crosshair

	crosshair=&iface.crosshair_list.crosshairs[0];

	view_rtl_overlay_crosshair_id=rtlSceneOverlayAdd(view_rtl_draw_scene_id,RL_OVERLAY_TYPE_QUAD_MATERIAL,0);
	rtlSceneOverlaySetMaterial(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,crosshair->rtl_material_id);
	rtlSceneOverlaySetHidden(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,TRUE);

	progress_update();
}

void view_dim3rtl_overlay_stop(void)
{
	int						n,k;
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
		for (k=0;k!=rtl_temp_text_max_len;k++) {
			rtlSceneOverlayDelete(view_rtl_draw_scene_id,text->rtl_overlay_id[k]);
		}
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

	rtlSceneOverlaySetTint(view_rtl_draw_scene_id,bitmap->rtl_overlay_id,&col);
}

/* =======================================================

      dim3RTL Overlay Text Update
      
======================================================= */

void view_dim3rtl_overlay_set_to_char(int scene_id,int overlay_id,texture_font_size_type *font_size,int ch)
{
	int				xoff,yoff;
	rtlUV			uv,uv_stamp;

		// check for non-defined characters

	if ((ch<'!') || (ch>'z')) {
		uv.x=0.0f;
		uv.y=0.0f;

		uv_stamp.x=0.0f;
		uv_stamp.y=0.0f;
	}

		// create UV for character

	else {
		ch-='!';

		yoff=ch/font_size->char_per_line;
		xoff=ch-(yoff*font_size->char_per_line);

		uv.x=((float)xoff)*font_size->gl_xoff;
		uv.y=((float)yoff)*font_size->gl_yoff;

		uv_stamp.x=font_size->gl_xadd;
		uv_stamp.y=font_size->gl_yadd;
	}

	rtlSceneOverlaySetQuadUV(scene_id,overlay_id,&uv,&uv_stamp);
}

void view_dim3rtl_overlay_text_to_overlay(iface_text_type *text)
{
	int						n,x,y,txt_len,txt_wid,ch,
							wid,high;
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

		// get text size

	txt_len=strlen(text->data);
	if (txt_len>255) txt_len=255;

		// get alpha

	alpha=text->alpha;
	if (hud_item_fade_run(&text->fade,&alpha)) {
		text->show=FALSE;			// a fade has turned off text
	}

		// hidden text

	if ((!text->show) || (txt_len==0)) {
		for (n=0;n!=rtl_temp_text_max_len;n++) {
			rtlSceneOverlaySetHidden(view_rtl_draw_scene_id,text->rtl_overlay_id[n],TRUE);
		}
		return;
	}

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

		// get the color

	col.r=text->color.r;
	col.g=text->color.g;
	col.b=text->color.b;
	col.a=alpha;

		// create the quads
		// quads are offsets into overlays

	p_pnt.x=x;
	p_pnt.y=y-high;

	s_pnt.x=wid;
	s_pnt.y=high;
	
	c=text->data;

	for (n=0;n!=rtl_temp_text_max_len;n++) {

		if (n>=txt_len) {
			rtlSceneOverlaySetHidden(view_rtl_draw_scene_id,text->rtl_overlay_id[n],TRUE);
			continue;
		}
	
		ch=(int)*c++;

			// the overlay quad

		rtlSceneOverlaySetQuadPosition(view_rtl_draw_scene_id,text->rtl_overlay_id[n],&p_pnt,&s_pnt);
		view_dim3rtl_overlay_set_to_char(view_rtl_draw_scene_id,text->rtl_overlay_id[n],font_size,ch);
		rtlSceneOverlaySetTint(view_rtl_draw_scene_id,text->rtl_overlay_id[n],&col);
		rtlSceneOverlaySetHidden(view_rtl_draw_scene_id,text->rtl_overlay_id[n],FALSE);

			// next character

		if (text->monospaced) {
			p_pnt.x+=wid;
		}
		else {
			if ((ch<'!') || (ch>'z')) {
				p_pnt.x+=(int)(((float)wid)*0.33f);
			}
			else {
				p_pnt.x+=(int)(((float)wid)*font_size->char_size[ch-'!']);
			}
		}
	}
}

/* =======================================================

      dim3RTL Overlay CrossHair Update
      
======================================================= */

int view_dim3rtl_overlay_crosshair_setup_click(obj_type *obj,rtl2DPoint *p_pnt,rtl2DPoint *s_pnt,rtlColor *col)
{
	int				sz,idx;

	sz=iface.scale_x/20;

	p_pnt->x=(iface.scale_x>>1)-sz;
	p_pnt->y=(iface.scale_y>>1)-sz;
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
		
	sz=weap->crosshair.min_size;
	
	p_pnt->x=(iface.scale_x>>1)-sz;
	p_pnt->y=(iface.scale_y>>1)-sz;

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
	rtlSceneOverlaySetQuadPosition(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,&p_pnt,&s_pnt);
	rtlSceneOverlaySetTint(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,&col);

	rtlSceneOverlaySetHidden(view_rtl_draw_scene_id,view_rtl_overlay_crosshair_id,FALSE);
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
		// the overlay to text handles hiding if not
		// shown, or faded out, or a blank string

	text=iface.text_list.texts;
	
	for (n=0;n!=iface.text_list.ntext;n++) {
		view_dim3rtl_overlay_text_to_overlay(text);
		text++;
	}

		// crosshair

	view_dim3rtl_overlay_crosshair_setup();
}
