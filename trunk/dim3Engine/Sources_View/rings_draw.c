/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Ring Drawing Routines

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

extern map_type				map;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;

/* =======================================================

      Draw Ring Position
      
======================================================= */

void ring_draw_position(effect_type *effect,int count,d3pnt *pnt)
{
	float				m_tick;
	iface_ring_type		*ring;
	ring_effect_data	*eff_ring;
	
	eff_ring=&effect->data.ring;
	ring=&iface.ring_list.rings[eff_ring->ring_idx];

	m_tick=((float)count)/10.0f;
	
	pnt->x=effect->pnt.x+(int)(ring->vct.x*m_tick);
	pnt->y=effect->pnt.y+(int)(ring->vct.y*m_tick);
	pnt->z=effect->pnt.z+(int)(ring->vct.z*m_tick);
}

/* =======================================================

      Draw Rings
      
======================================================= */

void ring_draw(effect_type *effect,int count)
{
	int						n,nvertex,life_tick;
	float					mx,my,mz,fx,fy,fz,
							vx[4],vy[4],vz[4],tx[4],ty[4],
							outer_sz,inner_sz,rd,rd2,
							color_dif,alpha,gx,gy,g_size,
							f_count,f_tick;
	float					*pf,*vertex_ptr;
	d3pnt					pnt;
	d3col					col;
	iface_ring_type			*ring;
	ring_effect_data		*eff_ring;
	matrix_type				mat_x,mat_y,mat_z;
	
	eff_ring=&effect->data.ring;
	ring=&iface.ring_list.rings[eff_ring->ring_idx];
	
		// get size
		
	life_tick=effect->life_tick;

	f_tick=(float)life_tick;
	f_count=(float)count;
	
	outer_sz=(float)(ring->end_outer_size-ring->start_outer_size);
	outer_sz=((outer_sz*f_count)/f_tick)+(float)ring->start_outer_size;

	inner_sz=(float)(ring->end_inner_size-ring->start_inner_size);
	inner_sz=((inner_sz*f_count)/f_tick)+(float)ring->start_inner_size;

		// color and alpha

	color_dif=ring->end_color.r-ring->start_color.r;
    col.r=ring->start_color.r+((color_dif*f_count)/f_tick);
	color_dif=ring->end_color.g-ring->start_color.g;
    col.g=ring->start_color.g+((color_dif*f_count)/f_tick);
	color_dif=ring->end_color.b-ring->start_color.b;
    col.b=ring->start_color.b+((color_dif*f_count)/f_tick);

	col.r*=eff_ring->tint.r;
	col.g*=eff_ring->tint.g;
	col.b*=eff_ring->tint.b;

	alpha=ring->end_alpha-ring->start_alpha;
	alpha=((alpha*f_count)/f_tick)+ring->start_alpha;
	
		// setup images
		
	effect_image_animate_get_uv(count,&ring->animate,&gx,&gy,&g_size);
	
		// position and ring rotation

	ring_draw_position(effect,count,&pnt);

	mx=(float)pnt.x;
	my=(float)pnt.y;
	mz=(float)pnt.z;

	fx=f_count*ring->rot.x;
	fx+=(fx*ring->rot_accel.x);
	fx=angle_add(eff_ring->ang.x,fx);

	fy=f_count*ring->rot.y;
	fy+=(fy*ring->rot_accel.y);
	fy=angle_add(eff_ring->ang.y,fy);

	fz=f_count*ring->rot.z;
	fz+=(fz*ring->rot_accel.z);
	fz=angle_add(eff_ring->ang.z,fz);

	matrix_rotate_x(&mat_x,-fx);
	matrix_rotate_z(&mat_z,fz);
	matrix_rotate_y(&mat_y,fy);

		// construct VBO
		// effect vbos are dynamic, so it'll auto construct
		// the first time called

	nvertex=36*6;

	view_create_effect_vertex_object(effect,((nvertex*(3+2))*sizeof(float)));

	view_bind_effect_vertex_object(effect);
	vertex_ptr=(float*)view_map_effect_vertex_object();
	if (vertex_ptr==NULL) {
		view_unbind_effect_vertex_object();
		return;
	}

		// set ring arrays

	pf=vertex_ptr;

	for (n=0;n!=360;n+=10) {
		rd=((float)n)*ANG_to_RAD;
		rd2=((float)(n+10))*ANG_to_RAD;

			// vertex 0

		fx=cosf(rd)*outer_sz;
		fy=-(sinf(rd)*outer_sz);
		fz=0.0f;

		matrix_vertex_multiply(&mat_x,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_z,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_y,&fx,&fy,&fz);

		vx[0]=mx+fx;
		vy[0]=my+fy;
		vz[0]=mz+fz;

		tx[0]=gx+(g_size*((fx+outer_sz)/(outer_sz*2.0f)));
		ty[0]=gy+(g_size*((fy+outer_sz)/(outer_sz*2.0f)));

			// vertex 1

		fx=cosf(rd2)*outer_sz;
		fy=-(sinf(rd2)*outer_sz);
		fz=0.0f;
		
		matrix_vertex_multiply(&mat_x,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_z,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_y,&fx,&fy,&fz);

		vx[1]=mx+fx;
		vy[1]=my+fy;
		vz[1]=mz+fz;

		tx[1]=gx+(g_size*((fx+outer_sz)/(outer_sz*2.0f)));
		ty[1]=gy+(g_size*((fy+outer_sz)/(outer_sz*2.0f)));

			// vertex 2

		fx=cosf(rd2)*inner_sz;
		fy=-(sinf(rd2)*inner_sz);
		fz=0.0f;

		matrix_vertex_multiply(&mat_x,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_z,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_y,&fx,&fy,&fz);

		vx[2]=mx+fx;
		vy[2]=my+fy;
		vz[2]=mz+fz;

		tx[2]=gx+(g_size*((fx+outer_sz)/(outer_sz*2.0f)));
		ty[2]=gy+(g_size*((fy+outer_sz)/(outer_sz*2.0f)));
		
			// vertex 3

		fx=cosf(rd)*inner_sz;
		fy=-(sinf(rd)*inner_sz);
		fz=0.0f;

		matrix_vertex_multiply(&mat_x,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_z,&fx,&fy,&fz);
		matrix_vertex_multiply(&mat_y,&fx,&fy,&fz);

		vx[3]=mx+fx;
		vy[3]=my+fy;
		vz[3]=mz+fz;

		tx[3]=gx+(g_size*((fx+outer_sz)/(outer_sz*2.0f)));
		ty[3]=gy+(g_size*((fy+outer_sz)/(outer_sz*2.0f)));
		
			// 0-1-3 trig

		*pf++=vx[0];
		*pf++=vy[0];
		*pf++=vz[0];

		*pf++=tx[0];
		*pf++=ty[0];

		*pf++=vx[1];
		*pf++=vy[1];
		*pf++=vz[1];

		*pf++=tx[1];
		*pf++=ty[1];

		*pf++=vx[3];
		*pf++=vy[3];
		*pf++=vz[3];

		*pf++=tx[3];
		*pf++=ty[3];

			// 1-2-3 trig

		*pf++=vx[1];
		*pf++=vy[1];
		*pf++=vz[1];

		*pf++=tx[1];
		*pf++=ty[1];

		*pf++=vx[2];
		*pf++=vy[2];
		*pf++=vz[2];

		*pf++=tx[2];
		*pf++=ty[2];

		*pf++=vx[3];
		*pf++=vy[3];
		*pf++=vz[3];

		*pf++=tx[3];
		*pf++=ty[3];
	}

		// unmap vertex object

	view_unmap_effect_vertex_object();

		// draw ring
		
	gl_texture_simple_start();
	gl_texture_simple_set(view_images_get_gl_id(ring->image_idx),FALSE,col.r,col.g,col.b,alpha);
	
	glEnable(GL_BLEND);
	
	if (ring->blend_add) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	glVertexPointer(3,GL_FLOAT,((3+2)*sizeof(float)),(GLvoid*)0);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,((3+2)*sizeof(float)),(GLvoid*)(3*sizeof(float)));

	glDrawArrays(GL_TRIANGLES,0,nvertex);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDepthMask(GL_TRUE);
	
	gl_texture_simple_end();

		// unbind vertex object
		
	view_unbind_effect_vertex_object();
}

