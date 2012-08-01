/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Labels

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

extern map_type				map;
extern view_type			view;
extern camera_type			camera;
extern server_type			server;
extern iface_type			iface;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern int					remote_slow_image_idx,remote_talk_image_idx;

/* =======================================================

      Label Setup
      
======================================================= */

void label_draw_setup_single(obj_type *obj,int bone_idx,obj_label_draw *label_draw)
{
	int						dist;
	bool					hit;
	d3pnt					ept;
	ray_trace_contact_type	contact;

		// get bone position

	label_draw->pnt.x=obj->pnt.x;
	label_draw->pnt.y=obj->pnt.y;
	label_draw->pnt.z=obj->pnt.z;

	if (!model_get_generic_bone_position(&obj->draw.setup,bone_idx,&label_draw->pnt)) return;

		// skip if behind z

	if (!gl_project_in_view_z(&label_draw->pnt)) return;

		// ignore if past fade distance

	dist=distance_get(label_draw->pnt.x,label_draw->pnt.y,label_draw->pnt.z,view.render->camera.pnt.x,view.render->camera.pnt.y,view.render->camera.pnt.z);
	if (dist>=iface.label.max_dist) return;

		// ignore if obscured by ray trace

	contact.obj.on=TRUE;
	contact.proj.on=FALSE;
	contact.origin=poly_ray_trace_origin_object;
	
	ept.x=view.render->camera.pnt.x;
	ept.y=view.render->camera.pnt.y;
	ept.z=view.render->camera.pnt.z;

	contact.obj.ignore_idx=obj->idx;
	
	hit=ray_trace_map_by_point(&label_draw->pnt,&ept,&contact);
	
	if (map.camera.mode==cv_fpp) {
		if (!hit) return;
		if (contact.obj.idx!=server.player_obj_idx) return;
	}
	else {
		if (hit) return;
	}

		// calculate fade and size
			
	if (dist<iface.label.min_dist) {
		label_draw->alpha=1.0f;
		label_draw->size_factor=1.0f;
	}
	else {
		label_draw->alpha=1.0f-((float)(dist-iface.label.min_dist)/(float)(iface.label.max_dist-iface.label.min_dist));
		label_draw->size_factor=label_draw->alpha;
		if (label_draw->size_factor<0.1f) label_draw->size_factor=0.1f;
	}

		// project point in 2D

	gl_project_point(&label_draw->pnt);

	label_draw->on=TRUE;
}

void label_draw_setup(void)
{
	int						n;
	obj_type				*obj;
	model_type				*mdl;

		// clear all labels
	
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_object) continue;

		obj=server.obj_list.objs[view.render->draw_list.items[n].idx];
		obj->label.text.draw.on=FALSE;
		obj->label.bitmap.draw.on=FALSE;
		obj->label.health.draw.on=FALSE;
		obj->label.remote_name.draw.on=FALSE;
	}

		// remove labels behind z or off-screen
		// ignore console as it won't matter for projection
		
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_object) continue;

			// skip objects hidden or with no models

		obj=server.obj_list.objs[view.render->draw_list.items[n].idx];
		if (obj->hidden) continue;
		if ((obj->draw.model_idx==-1) || (!obj->draw.on)) continue;
		
		mdl=server.model_list.models[obj->draw.model_idx];
	
			// setup labels

		if (obj->label.text.str[0]!=0x0) label_draw_setup_single(obj,mdl->bone_connect.label_text_bone_idx,&obj->label.text.draw);
		if (obj->label.bitmap.image_idx!=-1) label_draw_setup_single(obj,mdl->bone_connect.label_bitmap_bone_idx,&obj->label.bitmap.draw);
		if (obj->label.health.on) label_draw_setup_single(obj,mdl->bone_connect.label_health_bone_idx,&obj->label.health.draw);
		if ((net_setup.mode!=net_mode_none) && (setup.network.show_names)) label_draw_setup_single(obj,mdl->bone_connect.name_bone_idx,&obj->label.remote_name.draw);
	}
}

/* =======================================================

      Remote Icons
      
======================================================= */

void label_draw_render_remote_icon(obj_type *obj,int image_idx)
{
	int				wid,high,txt_size,
					lft,rgt,top,bot;
	d3col			col;
	bitmap_type		*bitmap;

	wid=(int)(((float)iface.label.bitmap.size)*obj->label.remote_name.draw.size_factor);
	high=(int)(((float)iface.label.bitmap.size)*obj->label.remote_name.draw.size_factor);

	txt_size=(int)(((float)iface.label.text.size)*obj->label.remote_name.draw.size_factor);

	lft=obj->label.remote_name.draw.pnt.x-(wid>>1);
	rgt=lft+wid;
	top=((view.screen.y_sz-obj->label.remote_name.draw.pnt.y)-high)-txt_size;
	bot=top+high;

	col.r=col.g=col.b=1.0f;

	bitmap=view_images_get_bitmap(image_idx);
	view_primitive_2D_texture_quad(bitmap->gl_id,&col,obj->label.remote_name.draw.alpha,lft,rgt,top,bot,0.0f,1.0f,0.0f,1.0f,TRUE);
}

/* =======================================================

      Label Rendering
      
======================================================= */

void label_draw_render(void)
{
	int					n,wid,high,txt_size,
						x,y,lft,rgt,top,bot;
	d3col				col;
	obj_type			*obj;
	bitmap_type			*bitmap;

	glDisable(GL_DEPTH_TEST);
	
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_object) continue;

		obj=server.obj_list.objs[view.render->draw_list.items[n].idx];

			// text

		if (obj->label.text.draw.on) {
			x=obj->label.text.draw.pnt.x;
			y=view.screen.y_sz-obj->label.text.draw.pnt.y;

			txt_size=(int)(((float)iface.label.text.size)*obj->label.remote_name.draw.size_factor);

			gl_text_start(font_hud_index,txt_size,FALSE);
			gl_text_draw(x,y,obj->label.text.str,tx_center,FALSE,&iface.label.text.col,obj->label.remote_name.draw.alpha);
			gl_text_end();
		}

			// bitmap

		if (obj->label.bitmap.draw.on) {

				// box size

			wid=(int)(((float)iface.label.bitmap.size)*obj->label.health.draw.size_factor);
			high=(int)(((float)iface.label.bitmap.size)*obj->label.health.draw.size_factor);

			lft=obj->label.bitmap.draw.pnt.x-(wid>>1);
			rgt=lft+wid;
			top=(view.screen.y_sz-obj->label.bitmap.draw.pnt.y)-(high>>1);
			bot=top+high;

				// draw image

			col.r=col.g=col.b=1.0f;

			bitmap=view_images_get_bitmap(obj->label.bitmap.image_idx);
			view_primitive_2D_texture_quad(bitmap->gl_id,&col,obj->label.bitmap.draw.alpha,lft,rgt,top,bot,0.0f,1.0f,0.0f,1.0f,TRUE);
		}

			// health

		if (obj->label.health.draw.on) {
			
				// box size

			wid=(int)(((float)iface.label.health.wid)*obj->label.health.draw.size_factor);
			high=(int)(((float)iface.label.health.high)*obj->label.health.draw.size_factor);

			lft=obj->label.health.draw.pnt.x-(wid>>1);
			rgt=lft+wid;
			top=(view.screen.y_sz-obj->label.health.draw.pnt.y)-(high>>1);
			bot=top+high;

				// health position

			x=lft+(((rgt-lft)*obj->status.health.value)/obj->status.health.max_value);
			if (x>rgt) x=rgt;
			if (x<lft) x=lft;

				// draw

			if (iface.label.health.background_on) view_primitive_2D_color_quad(&iface.label.health.background_col,obj->label.health.draw.alpha,lft,rgt,top,bot);
			if (iface.label.health.border_on) view_primitive_2D_line_quad(&iface.label.health.border_col,obj->label.health.draw.alpha,lft,rgt,top,bot);

			view_primitive_2D_color_quad(&iface.label.health.bar_col,obj->label.health.draw.alpha,lft,x,top,bot);
		}

			// remote names

		if (obj->label.remote_name.draw.on) {
			x=obj->label.remote_name.draw.pnt.x;
			y=view.screen.y_sz-obj->label.remote_name.draw.pnt.y;

				// the name

			object_get_tint(obj,&col);

			txt_size=(int)(((float)iface.label.text.size)*obj->label.remote_name.draw.size_factor);

			gl_text_start(font_hud_index,txt_size,FALSE);
			gl_text_draw(x,y,obj->name,tx_center,FALSE,&col,obj->label.remote_name.draw.alpha);
			gl_text_end();

				// slow and talking status
		
			if (remote_timed_out(obj)) {
				label_draw_render_remote_icon(obj,remote_slow_image_idx);
			}
			else {
				if ((obj->remote.talking)) {
					label_draw_render_remote_icon(obj,remote_talk_image_idx);
				}
			}
		}

	}
}
