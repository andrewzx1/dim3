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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "objects.h"

extern map_type				map;
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
	int						n,x,y,z,radius,pixel_sz,dist,d;
	float					alpha;
	bool					hit;
	d3pnt					spt,ept;
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

	contact.origin=poly_ray_trace_origin_halo;

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
			radius=object_get_radius(obj);
			ray_push_to_end(&spt,&ept,radius);
		}

		hit=ray_trace_map_by_point(&spt,&ept,&contact);

			// check hit and ignore hitting the projecting
			// player

		if (hit) {
			if ((contact.poly.mesh_idx!=-1) || (map.camera.mode!=cv_fpp)) {
				halo_draw->in_view=FALSE;
				continue;
			}
			if (contact.obj.idx!=server.player_obj_idx) {
				halo_draw->in_view=FALSE;
				continue;
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
			
		if (((halo_draw->proj_pnt.x+halo_draw->pixel_sz)<0) || ((halo_draw->proj_pnt.y+halo_draw->pixel_sz)<0) || ((halo_draw->proj_pnt.x-halo_draw->pixel_sz)>=view.screen.x_sz) || ((halo_draw->proj_pnt.y-halo_draw->pixel_sz)>=view.screen.y_sz)) {
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
	float					vertexes[8],uvs[8];
	d3col					col;
	halo_draw_type			*halo_draw;
	
		// any halos to draw?
		
	if (view.render->halo_draw.in_view_count==0) return;
		
		// halos are post-render overlay effects
		
	gl_2D_view_screen();

		// setup texture drawing

	gl_texture_simple_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	
		// get color

	col.r=col.g=col.b=1.0f;

		// draw halos

	gl_shader_draw_simple_bitmap_start();

	for (n=0;n!=view.render->halo_draw.count;n++) {
		halo_draw=&view.render->halo_draw.halos[n];
		if (!halo_draw->in_view) continue;
	
			// halo size
			
		x=halo_draw->proj_pnt.x;
		y=view.screen.y_sz-halo_draw->proj_pnt.y;
		psz=halo_draw->pixel_sz>>1;

			// setup vertex

		vertexes[0]=(float)(x-psz);
		vertexes[1]=(float)(y-psz);
		uvs[0]=0.0f;
		uvs[1]=0.0f;

		vertexes[2]=(float)(x-psz);
		vertexes[3]=(float)(y+psz);
		uvs[2]=0.0f;
		uvs[3]=1.0f;

		vertexes[4]=(float)(x+psz);
		vertexes[5]=(float)(y-psz);
		uvs[4]=1.0f;
		uvs[5]=0.0f;

		vertexes[6]=(float)(x+psz);
		vertexes[7]=(float)(y+psz);
		uvs[6]=1.0f;
		uvs[7]=1.0f;

			// draw halo
	
		gl_shader_draw_execute_simple_bitmap_ptr(view_images_get_gl_id(iface.halo_list.halos[halo_draw->idx].image_idx),2,vertexes,uvs,&col,halo_draw->alpha);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	}
		
	gl_shader_draw_simple_bitmap_start();

		// reset repeat

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
}

