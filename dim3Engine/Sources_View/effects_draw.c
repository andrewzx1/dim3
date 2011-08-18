/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Effect Drawing

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

float					effect_lightning_line_zag[16]={1.0f,0.5f,0.8f,-0.2f,0.3f,-1.0f,-0.7f,0.1f,-0.4f,0.6f,-0.4f,0.5f,-0.6f,0.2f,-0.8f,-0.4f};

extern map_type			map;
extern server_type		server;
extern view_type		view;

/* =======================================================

      Draw Lightning
      
======================================================= */

void effect_draw_lightning(effect_type *effect)
{
	int						n,k,nline,wid,x,z,y,
                            sx,sz,sy,ex,ez,ey,xadd,zadd,yadd;
	float					varient,fx,fy,fz,f_xadd,f_yadd,f_zadd;
	float					*vertex_ptr;
	lightning_effect_data	*lightning;
	
	lightning=&effect->data.lightning;
	
        // get line start and end
        
	sx=lightning->start_pnt.x;
	sy=lightning->start_pnt.y;
	sz=lightning->start_pnt.z;

	ex=lightning->end_pnt.x;
	ey=lightning->end_pnt.y;
	ez=lightning->end_pnt.z;
	
        // find number of lines

    x=abs(ex-sx);
    z=abs(ez-sz);
    y=abs(ey-sy);
	nline=distance_get(x,z,y,0,0,0)/200;
    
    if (nline<effect_lightning_min_lines) nline=effect_lightning_min_lines;
	if (nline>=effect_lightning_max_lines) nline=(effect_lightning_max_lines-1);
	
        // get line steps
        
	xadd=(ex-sx)/nline;
	zadd=(ez-sz)/nline;
	yadd=(ey-sy)/nline;
	
        // line varients
        
	k=((game_time_get()>>5)+lightning->line_offset)&0xF;
	varient=lightning->varient;

		// construct VBO
		// effect vbos are dynamic, so it'll auto construct
		// the first time called

	view_create_effect_vertex_object(effect,(((nline*3)*2)*sizeof(float)));

	view_bind_effect_vertex_object(effect);
	vertex_ptr=(float*)view_map_effect_vertex_object();
	if (vertex_ptr==NULL) return;

		// setup vertexes

	fx=(float)sx;
	fy=(float)sy;
	fz=(float)sz;

	f_xadd=(float)xadd;
	f_yadd=(float)yadd;
	f_zadd=(float)zadd;

	for (n=0;n!=nline;n++) {
		
		*vertex_ptr++=fx;
		*vertex_ptr++=fy;
		*vertex_ptr++=fz;
	
		if (n==(nline-1)) {
			*vertex_ptr++=(float)ex;
			*vertex_ptr++=(float)ey;
			*vertex_ptr++=(float)ez;
		}
		else {
			fx=(fx+f_xadd)+(int)(effect_lightning_line_zag[k]*varient);
			fy=(fy+f_yadd)+(int)(effect_lightning_line_zag[(k+4)&0xF]*varient);
			fz=(fz+f_zadd)+(int)(effect_lightning_line_zag[(k+8)&0xF]*varient);

			*vertex_ptr++=fx;
			*vertex_ptr++=fy;
			*vertex_ptr++=fz;
		}

		k=(k+1)&0xF;
	}

		// unmap vertex object

	view_unmap_effect_vertex_object();
	    
        // draw lines
		
	wid=lightning->wid;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	glDisable(GL_ALPHA_TEST);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
		
	glDisable(GL_LINE_SMOOTH);
	
	glLineWidth((float)(wid*6));
	glColor4f(lightning->col.r,lightning->col.g,lightning->col.b,0.1f);
	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);
	glDrawArrays(GL_LINES,0,(nline*2));

	glLineWidth((float)(wid*3));
	glColor4f(lightning->col.r,lightning->col.g,lightning->col.b,0.2f);
	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);
	glDrawArrays(GL_LINES,0,(nline*2));

	glLineWidth((float)wid);
	glColor4f(lightning->col.r,lightning->col.g,lightning->col.b,0.4f);
	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);
	glDrawArrays(GL_LINES,0,(nline*2));

	glLineWidth(1.0f);
	
	glEnable(GL_LINE_SMOOTH);

		// unbind vertex object
		
	view_unbind_effect_vertex_object();
}

/* =======================================================

      Draw Ray
      
======================================================= */

void effect_draw_ray(effect_type *effect,int count)
{
	int						wid,sx,sz,sy,ex,ez,ey,life_tick;
	float					vertexes[3*2];
	ray_effect_data			*ray;
	
	ray=&effect->data.ray;
	
        // find line
        
	sx=ray->start_pnt.x;
	sy=ray->start_pnt.y;
	sz=ray->start_pnt.z;

	ex=ray->end_pnt.x;
	ey=ray->end_pnt.y;
	ez=ray->end_pnt.z;
	
		// build line for count
	
	life_tick=effect->life_tick>>1;

	if (count<life_tick) {
		ex=sx+(((ex-sx)*count)/life_tick);
		ey=sy+(((ey-sy)*count)/life_tick);
		ez=sz+(((ez-sz)*count)/life_tick);
	}
	else {
		count=effect->life_tick-count;
		sx=ex+(((sx-ex)*count)/life_tick);
		sy=ey+(((sy-ey)*count)/life_tick);
		sz=ez+(((sz-ez)*count)/life_tick);
	}

		// setup vertexes

	vertexes[0]=(float)sx;
	vertexes[1]=(float)sy;
	vertexes[2]=(float)sz;

	vertexes[3]=(float)ex;
	vertexes[4]=(float)ey;
	vertexes[5]=(float)ez;

        // draw lines
		
	wid=ray->wid;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	
	glEnable(GL_ALPHA_TEST);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
		
	glDisable(GL_LINE_SMOOTH);
	
	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)vertexes);
	
	while (wid>0) {
	
		glLineWidth((float)wid);
		glColor4f(ray->col.r,ray->col.g,ray->col.b,0.1f);
		glDrawArrays(GL_LINES,0,2);
		
		wid-=2;
	}
	
	glEnable(GL_LINE_SMOOTH);
}

/* =======================================================

      Get UV for animated Images
      
======================================================= */

void effect_image_animate_get_uv(int tick,iface_image_animation_type *animate,float *gx,float *gy,float *g_size)
{
	int				k,nimage,image_per_row;
	float			gsz;
	
	nimage=animate->image_count;
	
	*gx=*gy=0.0f;
	*g_size=1.0f;
	
	if (nimage>1) {
		if (animate->msec==0) {
			k=0;
		}
		else {
			k=tick/animate->msec;
		}

		if ((!animate->loop) && (k>=nimage)) {
			k=nimage-1;
		}
		else {
			if (!animate->loop_back) {
				k=k%nimage;
			}
			else {
				k=k%((nimage<<1)-2);
				if (k>=nimage) k=(nimage+2)-k;
			}
		}

		image_per_row=animate->image_per_row;
		gsz=1.0f/(float)image_per_row;
		
		*gx=gsz*(float)(k%image_per_row);
		*gy=gsz*(float)(k/image_per_row);
		
		*g_size=gsz;
	}
}

/* =======================================================

      Effects Draw Mainline
      
======================================================= */

void effect_draw(void)
{
	int				n,tick,count;
	effect_type		*effect;

		// setup view

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();
		
		// draw effects
		// draw backwards to sort back to front
		
	tick=game_time_get();

	for (n=(view.render->draw_list.count-1);n>=0;n--) {
		if (view.render->draw_list.items[n].type!=view_render_type_effect) continue;

		effect=server.effect_list.effects[view.render->draw_list.items[n].idx];
		
		count=tick-effect->start_tick;

		switch (effect->effecttype) {

			case ef_particle:
				particle_draw(effect,count);
				break;
				
			case ef_ring:
				ring_draw(effect,count);
				break;
				
			case ef_lightning:
				effect_draw_lightning(effect);
				break;
				
			case ef_ray:
				effect_draw_ray(effect,count);
				break;
				
		}
	}
}


