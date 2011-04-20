/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Halo Drawing

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "objects.h"

extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;

/* =======================================================

      Add Halos
      
======================================================= */

void halo_draw_clear(void)
{
	view.render->halo_draw.count=0;
}

void halo_draw_add(d3pnt *pnt,int obj_idx,int halo_idx)
{
	halo_draw_type		*halo_draw;
	
	if (halo_idx==-1) return;
	if (view.render->halo_draw.count>=max_light_spot) return;
	
	halo_draw=&view.render->halo_draw.halos[view.render->halo_draw.count];
	view.render->halo_draw.count++;
	
	halo_draw->idx=halo_idx;

	halo_draw->obj_idx=obj_idx;
	
	halo_draw->pnt.x=pnt->x;
	halo_draw->pnt.y=pnt->y;
	halo_draw->pnt.z=pnt->z;
}

/* =======================================================

      Setup Halos
      
======================================================= */

void halo_draw_setup(void)
{
	int						n,x,y,z,pixel_sz,dist,d;
	float					alpha;
	bool					hit;
	d3pnt					spt,ept,hpt;
	obj_type				*obj;
	halo_draw_type			*halo_draw;
	iface_halo_type			*halo;
	ray_trace_contact_type	contact;
	
	view.render->halo_draw.in_view_count=0;
	
		// clear all halo spots
	
	halo_draw=view.render->halo_draw.halos;
	
	for (n=0;n!=view.render->halo_draw.count;n++) {
		halo_draw->in_view=TRUE;
		halo_draw++;
	}

		// remove halos behind z or off-screen
		// ignore console as it won't matter for projection
		
	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();

	for (n=0;n!=view.render->halo_draw.count;n++) {
		halo_draw=&view.render->halo_draw.halos[n];
		
			// translate and rotate point
			
		x=halo_draw->pnt.x;
		y=halo_draw->pnt.y;
		z=halo_draw->pnt.z;
		
			// is it behind the z?

		if (!gl_project_in_view_z(x,y,z)) {
			halo_draw->in_view=FALSE;
			continue;
		}
				
			// project halos
			
		gl_project_point(&x,&y,&z);

		halo_draw->proj_pnt.x=x;
		halo_draw->proj_pnt.y=y;
		halo_draw->proj_pnt.z=z;
	}

		// ray trace halos to camera's eye level
		// to check for visibility

	ept.x=view.render->camera.pnt.x;
	ept.y=view.render->camera.pnt.y;
	ept.z=view.render->camera.pnt.z;

	contact.proj.on=FALSE;

	contact.origin=poly_ray_trace_origin_object;

	for (n=0;n!=view.render->halo_draw.count;n++) {
		halo_draw=&view.render->halo_draw.halos[n];
		if (!halo_draw->in_view) continue;

		spt.x=halo_draw->pnt.x;
		spt.y=halo_draw->pnt.y;
		spt.z=halo_draw->pnt.z;

		halo=&iface.halo_list.halos[halo_draw->idx];

			// is ray greater than max distance?

		dist=distance_get(spt.x,spt.y,spt.z,ept.x,ept.y,ept.z);
		if (dist>halo->max_dist) {
			halo_draw->in_view=FALSE;
			continue;
		}

			// ray trace for visibily

		contact.obj.on=!halo->no_clip_object;

		if ((halo->no_clip_self) && (halo_draw->obj_idx!=-1)) {
			contact.obj.ignore_idx=halo_draw->obj_idx;
		}
		else {
			contact.obj.ignore_idx=-1;
		}
		
			// push slighty towards camera to clear
			// any ray trace errors from being too
			// close to projecting object

		if (halo_draw->obj_idx!=-1) {
			obj=server.obj_list.objs[halo_draw->obj_idx];
			ray_push_to_end(&spt,&ept,obj->size.radius);
		}

		hit=ray_trace_map_by_point(&spt,&ept,&hpt,&contact);

			// check hit, if within min distance, then
			// ignore this as a hit (it's probably something
			// touching the halo) or ignore hitting the projecting
			// player

		if (hit) {
			if (distance_get(ept.x,ept.y,ept.z,hpt.x,hpt.y,hpt.z)>halo->min_dist) {
				if (camera.setup.mode==cv_fpp) {
					if (contact.obj.idx!=server.player_obj_idx) {
						halo_draw->in_view=FALSE;
						continue;
					}
				}
				else {
					halo_draw->in_view=FALSE;
					continue;
				}
			}
		}

			// get size

		if (dist>=halo->max_dist) {
			halo_draw->pixel_sz=halo->max_size;
			halo_draw->alpha=halo->max_alpha;
		}
		else {
			if (dist<=halo->min_dist) {
				halo_draw->pixel_sz=halo->min_size;
				halo_draw->alpha=halo->min_alpha;
			}
			else {
				dist-=halo->min_dist;
				
				d=halo->max_dist-halo->min_dist;
				pixel_sz=halo->max_size-halo->min_size;
				alpha=halo->max_alpha-halo->min_alpha;
				
				halo_draw->pixel_sz=((pixel_sz*dist)/d)+halo->min_size;
				halo_draw->alpha=((alpha*(float)dist)/(float)d)+halo->min_alpha;
			}
		}
		
			// obscure halos outside of view port
			
		if (((halo_draw->proj_pnt.x+halo_draw->pixel_sz)<0) || ((halo_draw->proj_pnt.y+halo_draw->pixel_sz)<0) || ((halo_draw->proj_pnt.x-halo_draw->pixel_sz)>=setup.screen.x_sz) || ((halo_draw->proj_pnt.y-halo_draw->pixel_sz)>=setup.screen.y_sz)) {
			halo_draw->in_view=FALSE;
			continue;
		}
		
			// add to view count

		view.render->halo_draw.in_view_count++;
	}
}

/* =======================================================

      Render Halos
      
======================================================= */

void halo_draw_render(void)
{
	int						n,x,y,psz;
	float					*vp,*uv,*vertex_ptr,*uv_ptr;
	halo_draw_type			*halo_draw;
	
		// any halos to draw?
		
	if (view.render->halo_draw.in_view_count==0) return;
		
		// halos are post-render overlay effects
		
	gl_2D_view_screen();

		// enable vertex drawing

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// setup texture drawing

	gl_texture_simple_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);
	
	for (n=0;n!=view.render->halo_draw.count;n++) {
		halo_draw=&view.render->halo_draw.halos[n];
		if (!halo_draw->in_view) continue;
	
			// halo size
			
		x=halo_draw->proj_pnt.x;
		y=setup.screen.y_sz-halo_draw->proj_pnt.y;
		psz=halo_draw->pixel_sz>>1;

			// setup vertex

		vertex_ptr=view_bind_map_next_vertex_object(4*(2+2));
		if (vertex_ptr==NULL) continue;

		uv_ptr=vertex_ptr+(4*2);
		
		vp=vertex_ptr;
		uv=uv_ptr;

		*vp++=(float)(x-psz);
		*vp++=(float)(y-psz);
		*uv++=0.0f;
		*uv++=0.0f;

		*vp++=(float)(x-psz);
		*vp++=(float)(y+psz);
		*uv++=0.0f;
		*uv++=1.0f;

		*vp++=(float)(x+psz);
		*vp++=(float)(y-psz);
		*uv++=1.0f;
		*uv++=0.0f;

		*vp++=(float)(x+psz);
		*vp++=(float)(y+psz);
		*uv++=1.0f;
		*uv++=1.0f;

		view_unmap_current_vertex_object();

			// draw halo

		glVertexPointer(2,GL_FLOAT,0,0);
		glTexCoordPointer(2,GL_FLOAT,0,(void*)((4*2)*sizeof(float)));

		gl_texture_simple_set(view_images_get_gl_id(iface.halo_list.halos[halo_draw->idx].image_idx),TRUE,1,1,1,halo_draw->alpha);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);

		view_unbind_current_vertex_object();
	}
		
	gl_texture_simple_end();

		// disable vertex drawing

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

