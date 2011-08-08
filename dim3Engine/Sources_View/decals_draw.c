/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Decals

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

extern map_type			map;
extern view_type		view;
extern server_type		server;
extern iface_type		iface;
extern setup_type		setup;

extern bool view_mesh_in_draw_list(int mesh_idx);

/* =======================================================

      Decals
      
======================================================= */

void decal_render_stencil(map_mesh_type *mesh,map_mesh_poly_type *poly,int stencil_idx)
{
	int			n;
	d3pnt		*pt;
	float		*vp,*vertex_ptr;

		// already stenciled for decal?

	if (poly->draw.decal_stencil_idx!=0) return;

		// setup vertex ptr

	vertex_ptr=view_bind_map_next_vertex_object(poly->ptsz*3);
	if (vertex_ptr==NULL) return;
	
	vp=vertex_ptr;
	
	for (n=0;n!=poly->ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];
		*vp++=(float)pt->x;
		*vp++=(float)pt->y;
		*vp++=(float)pt->z;
	}
	
	view_unmap_current_vertex_object();
	
		// stencil

	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);

	glStencilFunc(GL_ALWAYS,stencil_idx,0xFF);

	glDrawArrays(GL_TRIANGLE_FAN,0,poly->ptsz);

	view_unbind_current_vertex_object();

		// remember stencil

	poly->draw.decal_stencil_idx=stencil_idx;
}

void decal_render_mark(int stencil_idx,decal_type *decal)
{
	int					k,tick,fade_out_start_tick;
	float				alpha,g_size,gx,gy,cf[3];
	float				*vp,*uv,*vertex_ptr,*uv_ptr;
	iface_mark_type		*mark;
	
		// get the alpha
	
	mark=&iface.mark_list.marks[decal->mark_idx];
		
	alpha=decal->alpha;
	tick=game_time_get()-decal->start_tick;
	
	if (tick<mark->fade_in_msec) {
		alpha=(alpha*(float)tick)/(float)mark->fade_in_msec;
	}
	else {
		fade_out_start_tick=mark->fade_in_msec+mark->life_msec;
		if (tick>=fade_out_start_tick) {
			k=tick-fade_out_start_tick;
			alpha=(alpha*(float)(mark->fade_out_msec-k))/(float)mark->fade_out_msec;
		}
	}
	
		// blending
		
	if (mark->blend_add) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}
	
		// get the decal image
		
	effect_image_animate_get_uv(tick,&mark->animate,&gx,&gy,&g_size);

		// get lighting
		
	if (mark->hilite) {
		cf[0]=decal->tint.r;
		cf[1]=decal->tint.g;
		cf[2]=decal->tint.b;
	}
	else {
		gl_lights_calc_color((double)decal->x[0],(double)decal->y[0],(double)decal->z[0],cf);
		cf[0]*=decal->tint.r;
		cf[1]*=decal->tint.g;
		cf[2]*=decal->tint.b;
	}

		// setup vertex ptr

	vertex_ptr=view_bind_map_next_vertex_object(4*(3+2));
	if (vertex_ptr==NULL) return;

	uv_ptr=vertex_ptr+(4*3);
	
	vp=vertex_ptr;
	uv=uv_ptr;

    *vp++=(float)decal->x[0];
	*vp++=(float)decal->y[0];
	*vp++=(float)decal->z[0];

    *uv++=gx;
	*uv++=gy;

    *vp++=(float)decal->x[3];
	*vp++=(float)decal->y[3];
	*vp++=(float)decal->z[3];

    *uv++=gx;
	*uv++=gy+g_size;

    *vp++=(float)decal->x[1];
	*vp++=(float)decal->y[1];
	*vp++=(float)decal->z[1];

    *uv++=gx+g_size;
	*uv++=gy;

    *vp++=(float)decal->x[2];
	*vp++=(float)decal->y[2];
	*vp++=(float)decal->z[2];

    *uv++=gx+g_size;
	*uv++=gy+g_size;
	
	view_unmap_current_vertex_object();
	
         // draw the polygon

	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)((4*3)*sizeof(float)));
			
	glStencilFunc(GL_EQUAL,stencil_idx,0xFF);
	gl_texture_decal_set(view_images_get_gl_id(mark->image_idx),cf[0],cf[1],cf[2],alpha);

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

	view_unbind_current_vertex_object();
}

void decal_render(void)
{
	int					n,stencil_idx;
	bool				has_decal;
	decal_type			*decal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// clear all rendering stencil marks
		// and detect if in view

	has_decal=FALSE;
	
	for (n=0;n!=max_decal_list;n++) {
		decal=server.decal_list.decals[n];
		if (!decal->on) continue;
	
		decal->in_view=view_mesh_in_draw_list(decal->mesh_idx);
		
		if (decal->in_view) {
			poly=&map.mesh.meshes[decal->mesh_idx].polys[decal->poly_idx];
			poly->draw.decal_stencil_idx=0;
			has_decal=TRUE;
		}
	}
	
	if (!has_decal) return;

		// decals use stenciling

	glEnable(GL_STENCIL_TEST);

		// stencil decal segments

	glDisable(GL_BLEND);
			
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

	glColor4f(0.0f,0.0f,0.0f,1.0f);

	stencil_idx=stencil_poly_start;

	for (n=0;n!=max_decal_list;n++) {
		decal=server.decal_list.decals[n];
		if (!decal->on) continue;

		if (decal->in_view) {
			mesh=&map.mesh.meshes[decal->mesh_idx];
			poly=&mesh->polys[decal->poly_idx];
			decal_render_stencil(mesh,poly,stencil_idx);
			stencil_idx++;
		}
	}

	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

		// draw decals to stencils

	gl_texture_decal_start();
				
	glEnable(GL_BLEND);
	
	glDisable(GL_DEPTH_TEST);
			
	glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);

	for (n=0;n!=max_decal_list;n++) {
		decal=server.decal_list.decals[n];
		if (!decal->on) continue;
	
		if (decal->in_view) {
			poly=&map.mesh.meshes[decal->mesh_idx].polys[decal->poly_idx];
			decal_render_mark(poly->draw.decal_stencil_idx,decal);
		}
	}

	gl_texture_decal_end();
	
	glDisable(GL_STENCIL_TEST);
}

