/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Remote Player Status Icons and Names

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

extern int							remote_slow_image_idx,remote_talk_image_idx;

/* =======================================================

      Draw Remote Status Icons
      
======================================================= */

void remote_draw_icon(obj_type *obj,unsigned long gl_id)
{
	int				x,y,z,x_sz,y_sz,z_sz;
	float			vertexes[12],uvs[8];
	d3col			col;
	
		// get the position and rotation
		
	x=obj->pnt.x;
	y=(obj->pnt.y-obj->size.y)-(crosshair_max_ray_trace_distance>>1);
	z=obj->pnt.z;
	
	x_sz=crosshair_max_ray_trace_distance;
	y_sz=crosshair_max_ray_trace_distance<<1;
	z_sz=0;
	rotate_2D_point_center(&x_sz,&z_sz,view.render->camera.ang.y);

		// setup vertexes

    vertexes[0]=(float)(x-x_sz);
	vertexes[1]=(float)(y-y_sz);
	vertexes[2]=-(float)(z-z_sz);

    vertexes[3]=(float)(x-x_sz);
	vertexes[4]=(float)y;
	vertexes[5]=-(float)(z-z_sz);

    vertexes[6]=(float)(x+x_sz);
	vertexes[7]=(float)(y-y_sz);
 	vertexes[8]=-(float)(z+z_sz);

    vertexes[9]=(float)(x+x_sz);
	vertexes[10]=(float)y;
	vertexes[11]=-(float)(z+z_sz);

		// setup UVs

    uvs[0]=0.0f;
	uvs[1]=0.0f;
    uvs[2]=0.0f;
	uvs[3]=1.0f;
    uvs[4]=1.0f;
	uvs[5]=0.0f;
	uvs[6]=1.0f;
	uvs[7]=1.0f;

		// setup colors
		
	col.r=col.b=col.g=1.0f;

		// draw the bitmap
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);

	gl_shader_draw_execute_simple_bitmap_ptr(gl_id,3,vertexes,uvs,&col,1.0f);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}

void remote_draw_status(obj_type *obj)
{
		// slow status
		
	if (remote_timed_out(obj)) {
		remote_draw_icon(obj,view_images_get_gl_id(remote_slow_image_idx));
		return;
	}
	
		// talking status
		
	if (obj->remote.talking) {
		remote_draw_icon(obj,view_images_get_gl_id(remote_talk_image_idx));
		return;
	}
}

/* =======================================================

      Remote Names
      
======================================================= */

void remote_draw_names_setup(void)
{
	/* supergumba -- move all this
	int						n,dist;
	bool					hit;
	d3pnt					pnt,spt,ept;
	obj_type				*obj;
	model_type				*mdl;
	ray_trace_contact_type	contact;
	
	if (net_setup.mode==net_mode_none) return;
	if (!setup.network.show_names) return;
	
		// clear all name draws
	
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_object) continue;
		server.obj_list.objs[view.render->draw_list.items[n].idx]->draw.remote_name.on=FALSE;
	}

		// remove names behind z or off-screen
		// ignore console as it won't matter for projection
		
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_object) continue;
		obj=server.obj_list.objs[view.render->draw_list.items[n].idx];
		
		if ((obj->type!=object_type_remote_player) && (obj->type!=object_type_bot_multiplayer)) continue;
		if (obj->hidden) continue;
		
			// get name point
		
		if ((obj->draw.model_idx==-1) || (!obj->draw.on)) continue;
		
		mdl=server.model_list.models[obj->draw.model_idx];

		pnt.x=obj->pnt.x;
		pnt.y=obj->pnt.y;
		pnt.z=obj->pnt.z;
		model_get_name_position(mdl,&obj->draw.setup,&pnt);
		
			// ignore if past fade distance
			
		dist=distance_get(pnt.x,pnt.y,pnt.z,view.render->camera.pnt.x,view.render->camera.pnt.y,view.render->camera.pnt.z);
		if (dist>=label_name_max_distance) continue;

			// ignore if obscured by ray trace

		contact.obj.on=TRUE;
		contact.proj.on=FALSE;
		contact.origin=poly_ray_trace_origin_object;

		spt.x=obj->draw.remote_name.pnt.x;
		spt.y=obj->draw.remote_name.pnt.y;
		spt.z=obj->draw.remote_name.pnt.z;
		
		ept.x=view.render->camera.pnt.x;
		ept.y=view.render->camera.pnt.y;
		ept.z=view.render->camera.pnt.z;

		contact.obj.ignore_idx=obj->idx;
		
		hit=ray_trace_map_by_point(&pnt,&ept,&contact);
		
		if (map.camera.mode==cv_fpp) {
			if (!hit) continue;
			if (contact.obj.idx!=server.player_obj_idx) continue;
		}
		else {
			if (hit) continue;
		}
		
			// translate to 2D

		if (!gl_project_in_view_z(&pnt)) continue;
		gl_project_point(&pnt);

		obj->draw.remote_name.pnt.x=pnt.x;
		obj->draw.remote_name.pnt.y=pnt.y;
		obj->draw.remote_name.pnt.z=pnt.z;
		
			// calculate the fade
			
		if (dist<label_name_min_distance) {
			obj->draw.remote_name.fade=1.0f;
		}
		else {
			obj->draw.remote_name.fade=1.0f-((float)(dist-remote_name_min_distance)/(float)(label_name_max_distance-label_name_min_distance));
		}
		
		obj->draw.remote_name.on=TRUE;
	}
	*/
}

void remote_draw_names_render(void)
{
	/*
	int					n,x,y;
	bool				use_text;
	d3col				col;
	obj_type			*obj;
	
	if (net_setup.mode==net_mode_none) return;
	if (!setup.network.show_names) return;
	
	glDisable(GL_DEPTH_TEST);
	
		// draw all names

	use_text=FALSE;

	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_object) continue;

		obj=server.obj_list.objs[view.render->draw_list.items[n].idx];	
		if (!obj->draw.remote_name.on) continue;
				
		if (!use_text) {
			use_text=TRUE;
			gl_text_start(font_hud_index,iface.font.text_size_mini,FALSE);
		}

		x=obj->draw.remote_name.pnt.x;
		y=view.screen.y_sz-obj->draw.remote_name.pnt.y;

		object_get_tint(obj,&col);
		gl_text_draw(x,y,obj->name,tx_center,FALSE,&col,obj->draw.remote_name.fade);
	}

	if (use_text) gl_text_end();
	*/
}
