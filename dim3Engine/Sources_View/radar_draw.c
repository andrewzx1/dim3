/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: HUD Radar Routines

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

#include "objects.h"
#include "remotes.h"
#include "consoles.h"
#include "interfaces.h"
#include "video.h"
#include "timing.h"

extern server_type			server;
extern view_type			view;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern render_info_type		render_info;

/* =======================================================

      Draw HUD Radar
      
======================================================= */

void radar_draw(void)
{
	int						n,tick,x,y,lx,rx,ty,by,px[4],py[4],
							dist,max_dist,fade_dist,fade_count,radar_sz;
	unsigned long			cur_gl_id,gl_id;
	float					alpha,cur_alpha,fade_mult;
	d3col					tint;
	obj_type				*obj,*player_obj;
	hud_radar_icon_type		*icon;
	
		// radar on?
		
	if (!hud.radar.on) return;
	
		// get player object (center of radar)
		
	player_obj=object_find_uid(server.player_obj_index);

		// set up view
		
	gl_2D_view_interface();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glDisable(GL_DEPTH_TEST);

		// get radar size

	radar_sz=hud.radar.display_radius;

	lx=hud.radar.x-radar_sz;
	ty=hud.radar.y-radar_sz;
	rx=lx+(radar_sz<<1);
	by=ty+(radar_sz<<1);

		// draw radar background

	if (!hud.radar.team_tint) {
		tint.r=tint.g=tint.b=1.0f;
	}
	else {
		if (net_setup.mode==net_mode_none) {
			memmove(&tint,&hud.color.default_tint,sizeof(d3col));
		}
		else {
			object_get_tint(player_obj,&tint);
		}
	}

	gl_texture_simple_start();
	gl_texture_simple_set(view_images_get_gl_id(hud.radar.background_image_idx),TRUE,tint.r,tint.g,tint.b,1.0f);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f);
	glVertex2i(lx,ty);
	glTexCoord2f(1.0f,0.0f);
	glVertex2i(rx,ty);
	glTexCoord2f(1.0f,1.0f);
	glVertex2i(rx,by);
	glTexCoord2f(0.0f,1.0f);
	glVertex2i(lx,by);
	glEnd();

		// ticks for fades

	tick=game_time_get();

		// draw radar objects
		
	cur_gl_id=-1;
	cur_alpha=1.0f;
	
	max_dist=hud.radar.view_radius;
	fade_dist=max_dist>>1;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (obj->index==server.player_obj_index) continue;
		if ((obj->hidden) || (!obj->radar.on) || (obj->radar.icon_idx==-1)) continue;
			
			// get distance
			
		dist=distance_2D_get(obj->pnt.x,obj->pnt.z,player_obj->pnt.x,player_obj->pnt.z);
		if ((!obj->radar.always_visible) && (dist>max_dist)) continue;

			// motion only?

		fade_mult=1.0f;
			
		if (obj->radar.motion_only) {

				// if moving, turn on the fade count
				// for when it stops

			if ((obj->forward_move.moving) || (obj->vert_move.moving)) {
				obj->radar.fade_start_tick=tick;
			}

				// if not moving, handle the fading

			else {
				if (hud.radar.no_motion_fade==0) continue;

				fade_count=tick-obj->radar.fade_start_tick;
				if (fade_count>hud.radar.no_motion_fade) continue;

				fade_mult=1.0f-(((float)fade_count)/((float)hud.radar.no_motion_fade));
			}
		}

			// get the position

		x=obj->pnt.x-player_obj->pnt.x;
		y=-(obj->pnt.z-player_obj->pnt.z);
			
		if (hud.radar.rot) rotate_2D_point_center(&x,&y,player_obj->ang.y);

			// if outside max, stick to edge

		if (dist>max_dist) {
			x=hud.radar.x+((x*radar_sz)/dist);
			y=hud.radar.y-((y*radar_sz)/dist);
		}
		else {
			x=hud.radar.x+((x*radar_sz)/max_dist);
			y=hud.radar.y-((y*radar_sz)/max_dist);
		}

			// get alpha

		alpha=1.0f;
		if ((!obj->radar.always_visible) && (dist>fade_dist)) {
			alpha=1.0f-(float)(dist-fade_dist)/(float)fade_dist;
		}

		alpha*=fade_mult;
			
			// draw object
		
		icon=&hud.radar.icons[obj->radar.icon_idx];
		
		gl_id=view_images_get_gl_id(icon->image_idx);
		if ((gl_id!=cur_gl_id) || (alpha!=cur_alpha)) {
			cur_gl_id=gl_id;
			cur_alpha=alpha;
			gl_texture_simple_set(gl_id,TRUE,1.0f,1.0f,1.0f,alpha);
		}

		px[0]=px[3]=x-icon->size;
		px[1]=px[2]=x+icon->size;
		py[0]=py[1]=y-icon->size;
		py[2]=py[3]=y+icon->size;

		if (icon->rot) rotate_2D_polygon(4,px,py,x,y,obj->ang.y);
	
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f);
		glVertex2i(px[0],py[0]);
		glTexCoord2f(1.0f,0.0f);
		glVertex2i(px[1],py[1]);
		glTexCoord2f(1.0f,1.0f);
		glVertex2i(px[2],py[2]);
		glTexCoord2f(0.0f,1.0f);
		glVertex2i(px[3],py[3]);
		glEnd();
	}

	gl_texture_simple_end();
}

