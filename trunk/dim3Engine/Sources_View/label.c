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
	if (dist>=label_name_max_distance) return;

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
			
	if (dist<label_name_min_distance) {
		label_draw->alpha=1.0f;
		label_draw->size_factor=1.0f;
	}
	else {
		label_draw->alpha=1.0f-((float)(dist-label_name_min_distance)/(float)(label_name_max_distance-label_name_min_distance));
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

		if (obj->label.health.on) label_draw_setup_single(obj,mdl->bone_connect.label_health_bone_idx,&obj->label.health.draw);
	}
}

/* =======================================================

      Label Rendering
      
======================================================= */

void label_draw_render(void)
{
	int					n,wid,high,lft,rgt,top,bot;
	d3col				col;
	obj_type			*obj;

	glDisable(GL_DEPTH_TEST);
	
		// draw all labels

	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_object) continue;

		obj=server.obj_list.objs[view.render->draw_list.items[n].idx];


			// health

		if (obj->label.health.draw.on) {

			col.r=1.0f;
			col.g=1.0f;
			col.b=0.0f;
			
			wid=(int)(((float)obj->label.health.wid)*obj->label.health.draw.size_factor);
			high=(int)(((float)obj->label.health.high)*obj->label.health.draw.size_factor);

			lft=obj->label.health.draw.pnt.x-(wid>>1);
			rgt=lft+wid;
			top=(view.screen.y_sz-obj->label.health.draw.pnt.y)-(high>>1);
			bot=top+high;

			view_primitive_2D_color_quad(&col,obj->label.health.draw.alpha,lft,rgt,top,bot);
		}
	}
}
