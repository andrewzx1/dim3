/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: HUD Radar Routines

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "objects.h"
#include "remotes.h"
#include "video.h"
#include "timing.h"

extern server_type			server;
extern view_type			view;
extern iface_type			iface;
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
	float					*vp,*uv,*vertex_ptr,*uv_ptr;
	d3col					tint;
	obj_type				*obj,*player_obj;
	iface_radar_icon_type	*icon;
	
		// radar on?
		
	if (!iface.radar.on) return;
	
		// get player object (center of radar)
		
	player_obj=server.obj_list.objs[server.player_obj_idx];

		// set up view
		
	gl_2D_view_interface();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glDisable(GL_DEPTH_TEST);

		// get radar size

	radar_sz=iface.radar.display_radius;

	lx=iface.radar.x-radar_sz;
	ty=iface.radar.y-radar_sz;
	rx=lx+(radar_sz<<1);
	by=ty+(radar_sz<<1);

		// draw radar background

	if (!iface.radar.team_tint) {
		tint.r=tint.g=tint.b=1.0f;
	}
	else {
		if (net_setup.mode==net_mode_none) {
			memmove(&tint,&iface.color.default_tint,sizeof(d3col));
		}
		else {
			object_get_tint(player_obj,&tint);
		}
	}

		// setup vertex ptr

	vertex_ptr=view_bind_map_next_vertex_object(4*(2+2));
	if (vertex_ptr==NULL) return;

	uv_ptr=vertex_ptr+(4*2);
	
	vp=vertex_ptr;
	uv=uv_ptr;

    *vp++=(float)lx;
	*vp++=(float)ty;
    *uv++=0.0f;
	*uv++=0.0f;

    *vp++=(float)lx;
	*vp++=(float)by;
    *uv++=0.0f;
    *uv++=1.0f;

    *vp++=(float)rx;
	*vp++=(float)ty;
    *uv++=1.0f;
	*uv++=0.0f;

    *vp++=(float)rx;
	*vp++=(float)by;
	*uv++=1.0f;
	*uv++=1.0f;
	
	view_unmap_current_vertex_object();

		// enable vertex drawing

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// draw background

	glVertexPointer(2,GL_FLOAT,0,0);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)((4*2)*sizeof(float)));
			
	gl_texture_simple_start();
	gl_texture_simple_set(view_images_get_gl_id(iface.radar.background_image_idx),TRUE,tint.r,tint.g,tint.b,1.0f);

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

	view_unbind_current_vertex_object();

		// ticks for fades

	tick=game_time_get();

		// draw radar objects
		
	cur_gl_id=-1;
	cur_alpha=1.0f;
	
	max_dist=iface.radar.view_radius;
	fade_dist=max_dist>>1;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (obj->idx==server.player_obj_idx) continue;
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
				if (iface.radar.no_motion_fade==0) continue;

				fade_count=tick-obj->radar.fade_start_tick;
				if (fade_count>iface.radar.no_motion_fade) continue;

				fade_mult=1.0f-(((float)fade_count)/((float)iface.radar.no_motion_fade));
			}
		}

			// get the position

		x=obj->pnt.x-player_obj->pnt.x;
		y=-(obj->pnt.z-player_obj->pnt.z);
			
		if (iface.radar.rot) rotate_2D_point_center(&x,&y,player_obj->ang.y);

			// if outside max, stick to edge

		if (dist>max_dist) {
			x=iface.radar.x+((x*radar_sz)/dist);
			y=iface.radar.y-((y*radar_sz)/dist);
		}
		else {
			x=iface.radar.x+((x*radar_sz)/max_dist);
			y=iface.radar.y-((y*radar_sz)/max_dist);
		}

			// get alpha

		alpha=1.0f;
		if ((!obj->radar.always_visible) && (dist>fade_dist)) {
			alpha=1.0f-(float)(dist-fade_dist)/(float)fade_dist;
		}

		alpha*=fade_mult;
			
			// draw object
		
		icon=&iface.radar.icons[obj->radar.icon_idx];
		
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

			// setup vertex ptr

		vertex_ptr=view_bind_map_next_vertex_object(4*(2+2));
		if (vertex_ptr==NULL) continue;

		uv_ptr=vertex_ptr+(4*2);
		
		vp=vertex_ptr;
		uv=uv_ptr;

		*vp++=(float)px[0];
		*vp++=(float)py[0];
		*uv++=0.0f;
		*uv++=0.0f;

		*vp++=(float)px[3];
		*vp++=(float)py[3];
		*uv++=0.0f;
		*uv++=1.0f;

		*vp++=(float)px[1];
		*vp++=(float)py[1];
		*uv++=1.0f;
		*uv++=0.0f;

		*vp++=(float)px[2];
		*vp++=(float)py[2];
		*uv++=1.0f;
		*uv++=1.0f;
		
		view_unmap_current_vertex_object();

			// draw icon

		glVertexPointer(2,GL_FLOAT,0,0);
		glTexCoordPointer(2,GL_FLOAT,0,(void*)((4*2)*sizeof(float)));

		glDrawArrays(GL_TRIANGLE_STRIP,0,4);

		view_unbind_current_vertex_object();
	}

	gl_texture_simple_end();

		// disable vertex drawing

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

