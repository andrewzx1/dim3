/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Sky Boxes

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
extern setup_type		setup;
extern view_type		view;

/* =======================================================

      Draw Background
      
======================================================= */

void draw_background_single(map_background_layer_type *layer)
{
	float				gx,gy,gy_high;
	texture_type		*texture;
	bitmap_type			*bitmap;

	if ((layer->fill<0) || (layer->fill>=max_map_texture)) return;
	
		// get y scaling
		
#ifndef D3_OS_IPHONE
	gy_high=(((float)setup.screen.y_sz)/((float)setup.screen.x_sz))*layer->y_fact;
#else
	gy_high=(((float)setup.screen.x_sz)/((float)setup.screen.y_sz))*layer->y_fact;
#endif

		// get scrolling
		
	gx=((float)(view.render->camera.pnt.x+view.render->camera.pnt.z))*(layer->x_scroll_fact*0.001f);
	gy=((float)view.render->camera.pnt.y)*(layer->y_scroll_fact*0.001f);

		// draw background
		
	texture=&map.textures[layer->fill];
	bitmap=&texture->frames[texture->animate.current_frame].bitmap;
	
	view_draw_next_vertex_object_2D_texture_quad(bitmap->gl_id,NULL,1.0f,0,setup.screen.x_sz,0,setup.screen.y_sz,gx,(gx+layer->x_fact),gy,(gy+gy_high));
}

void draw_background(void)
{
		// is there a background?
	
	if (!map.background.on) return;

		// setup view

	gl_2D_view_screen();
	
		// draw the layers
		
	glDisable(GL_DEPTH_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
		
	draw_background_single(&map.background.back);
	draw_background_single(&map.background.middle);
	draw_background_single(&map.background.front);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

/* =======================================================

      Draw Dome Panoramic Sky
      
======================================================= */

void draw_sky_dome_panoramic_setup(void)
{
    int					i,n,radius;
	float				gx1,gx2,tgy,bgy,f_ty,f_by;
	float				*vertex_ptr,*uv_ptr;
	double				top_reduce,bot_reduce,d_radius,
						rxz,rxz2,r_add;

		// dome setup

	int					y_fct[5]={2,3,5,6,9};
	float				gy_fct[6]={1.00f,0.80f,0.60f,0.40f,0.20f,0.01f};
	double				reduce_fct[5]={6.8f,6.4f,6.0f,5.5f,5.0f};
	
		// dome setup
	
	radius=map.sky.radius;
	d_radius=(double)radius;

	f_ty=(float)map.sky.dome_y;
	top_reduce=d_radius;

	r_add=ANG_to_RAD*(360/20);
	
		// construct VBO

	view_create_sky_vertex_object(((120*6)+(3*2))*sizeof(float));

	view_bind_sky_vertex_object();

	vertex_ptr=(float*)view_map_sky_vertex_object();
	if (vertex_ptr==NULL) {
		view_unbind_sky_vertex_object();
		return;
	}

	uv_ptr=vertex_ptr+((120*6)*3);

		// create the dome vertexes

	for (i=0;i!=5;i++) {				// the y
		
		f_by=f_ty;
		f_ty-=(float)(radius/y_fct[i]);

		rxz=0.0;

		bot_reduce=top_reduce;
		top_reduce-=(d_radius/reduce_fct[i]);
			
		bgy=gy_fct[i];
		tgy=gy_fct[i+1];

		for (n=0;n!=20;n++) {			// the x and z
		
			if (n!=19) {
				rxz2=rxz+r_add;
			}
			else {
				rxz2=0.0;
			}
			
			gx1=((float)n)/20.0f;
			gx2=((float)(n+1))/20.0f;
		
				// trig 1

			*vertex_ptr++=(float)(-sin(rxz)*bot_reduce);
			*vertex_ptr++=f_by;
			*vertex_ptr++=(float)(cos(rxz)*bot_reduce);

			*uv_ptr++=gx1;
			*uv_ptr++=bgy;

			*vertex_ptr++=(float)(-sin(rxz2)*bot_reduce);
			*vertex_ptr++=f_by;
			*vertex_ptr++=(float)(cos(rxz2)*bot_reduce);

			*uv_ptr++=gx2;
			*uv_ptr++=bgy;

			*vertex_ptr++=(float)(-sin(rxz)*top_reduce);
			*vertex_ptr++=f_ty;
			*vertex_ptr++=(float)(cos(rxz)*top_reduce);

			*uv_ptr++=gx1;
			*uv_ptr++=tgy;

				// trig 2

			*vertex_ptr++=(float)(-sin(rxz2)*bot_reduce);
			*vertex_ptr++=f_by;
			*vertex_ptr++=(float)(cos(rxz2)*bot_reduce);

			*uv_ptr++=gx2;
			*uv_ptr++=bgy;
			
			*vertex_ptr++=(float)(-sin(rxz2)*top_reduce);
			*vertex_ptr++=f_ty;
			*vertex_ptr++=(float)(cos(rxz2)*top_reduce);

			*uv_ptr++=gx2;
			*uv_ptr++=tgy;

			*vertex_ptr++=(float)(-sin(rxz)*top_reduce);
			*vertex_ptr++=f_ty;
			*vertex_ptr++=(float)(cos(rxz)*top_reduce);

			*uv_ptr++=gx1;
			*uv_ptr++=tgy;



			rxz+=r_add;
		}
	}

		// create the cap vertexes
		// cap gets color from pixel at 0,0

	rxz=0.0f;

	for (n=0;n!=20;n++) {			// the x and z
	
		if (n!=19) {
			rxz2=rxz+r_add;
		}
		else {
			rxz2=0.0;
		}
	
			// trig 1

		*vertex_ptr++=(float)(-sin(rxz)*top_reduce);
		*vertex_ptr++=f_by;
		*vertex_ptr++=(float)(cos(rxz)*top_reduce);
	
		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=(float)(-sin(rxz2)*top_reduce);
		*vertex_ptr++=f_by;
		*vertex_ptr++=(float)(cos(rxz2)*top_reduce);
		
		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=0.0f;
		*vertex_ptr++=f_ty;
		*vertex_ptr++=0.0f;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

			// trig 2

		*vertex_ptr++=(float)(-sin(rxz2)*top_reduce);
		*vertex_ptr++=f_by;
		*vertex_ptr++=(float)(cos(rxz2)*top_reduce);
		
		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=0.0f;
		*vertex_ptr++=f_ty;
		*vertex_ptr++=0.0f;
		
		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=0.0f;
		*vertex_ptr++=f_ty;
		*vertex_ptr++=0.0f;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		rxz+=r_add;
	}

		// unmap and unbind vbo

  	view_unmap_sky_vertex_object();
	view_unbind_sky_vertex_object();
}

void draw_sky_dome_panoramic(void)
{
    int					k,tick,txt_id;
	float				txt_x_shift,txt_y_shift;
	texture_type		*texture;
	
		// texture sizes
		
	tick=game_time_get();
		
	txt_x_shift=((float)tick*0.0005f)*map.sky.txt_x_shift;
	k=(int)txt_x_shift;
	txt_x_shift=txt_x_shift-(float)k;
	
	txt_y_shift=((float)tick*0.0005f)*map.sky.txt_y_shift;
	k=(int)txt_y_shift;
	txt_y_shift=txt_y_shift-(float)k;
					
		// setup view

	gl_3D_view();
	gl_3D_rotate(NULL,&view.render->camera.ang);
	gl_setup_project();
	
		// construct vbo

	view_bind_sky_vertex_object();

	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)(((120*6)*3)*sizeof(float)));

		// draw textured dome
		
	gl_texture_simple_start();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	
	glMatrixMode(GL_TEXTURE);
	glTranslatef(txt_x_shift,txt_y_shift,0.0f);

	texture=&map.textures[map.sky.fill];
	txt_id=texture->frames[texture->animate.current_frame].bitmap.gl_id;
	
	gl_texture_simple_set(txt_id,FALSE,1,1,1,1);

	glDrawArrays(GL_TRIANGLES,0,(120*6));
		
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
		
	gl_texture_simple_end();

		// unbind the vbo

	view_unbind_sky_vertex_object();
}

/* =======================================================

      Draw Dome Hemisphere Sky
      
======================================================= */

void draw_sky_dome_hemisphere_setup(void)
{
    int					i,n,radius,sz;
	float				f_ty,f_by,f_ty2,f_by2,gx1,gx2,tgy,bgy;
	float				*vertex_ptr,*uv_ptr;
	double				top_reduce,bot_reduce,d_radius,
						rxz,rxz2,r_add;

		// dome setup

	int					y_fct[6]={2,3,4,6,8,11};
	float				gy_fct[7]={1.00f,0.70f,0.40f,0.20f,0.10f,0.01f,0.01f};
	double				reduce_fct[7]={6.6f,6.4f,5.9f,5.2f,4.3f,3.2f};

		// dome setup
	
	radius=map.sky.radius;
	d_radius=(double)radius;

	f_ty=f_ty2=(float)map.sky.dome_y;
	top_reduce=d_radius;

	r_add=ANG_to_RAD*(360/20);

		// construct VBO
		
	sz=(5*20)*6;
	if (map.sky.dome_mirror) sz*=2;

	view_create_sky_vertex_object((sz+(3*2))*sizeof(float));

	view_bind_sky_vertex_object();

	vertex_ptr=(float*)view_map_sky_vertex_object();
	if (vertex_ptr==NULL) {
		view_unbind_sky_vertex_object();
		return;
	}

	uv_ptr=vertex_ptr+(sz*3);

		// create the dome vertexes

	for (i=0;i!=5;i++) {				// the y
		
		f_by=f_ty;
		f_ty-=((float)(radius/y_fct[i]));

		f_by2=f_ty2;
		f_ty2+=((float)(radius/y_fct[i]));

		rxz=0.0;

		bot_reduce=top_reduce;
		top_reduce-=(d_radius/reduce_fct[i]);

		bgy=gy_fct[i];
		tgy=gy_fct[i+1];
			
		for (n=0;n!=20;n++) {			// the x and z
		
			if (n!=19) {
				rxz2=rxz+r_add;
			}
			else {
				rxz2=0.0;
			}

			gx1=((float)n)/20.0f;
			gx2=((float)(n+1))/20.0f;

				// regular dome

			if (i!=4) {

					// trig 1

				*vertex_ptr++=(float)(-sin(rxz)*bot_reduce);
				*vertex_ptr++=f_by;
				*vertex_ptr++=(float)(cos(rxz)*bot_reduce);

				*uv_ptr++=gx1;
				*uv_ptr++=bgy;

				*vertex_ptr++=(float)(-sin(rxz2)*bot_reduce);
				*vertex_ptr++=f_by;
				*vertex_ptr++=(float)(cos(rxz2)*bot_reduce);

				*uv_ptr++=gx2;
				*uv_ptr++=bgy;

				*vertex_ptr++=(float)(-sin(rxz)*top_reduce);
				*vertex_ptr++=f_ty;
				*vertex_ptr++=(float)(cos(rxz)*top_reduce);

				*uv_ptr++=gx1;
				*uv_ptr++=tgy;

					// trig 2

				*vertex_ptr++=(float)(-sin(rxz2)*bot_reduce);
				*vertex_ptr++=f_by;
				*vertex_ptr++=(float)(cos(rxz2)*bot_reduce);

				*uv_ptr++=gx2;
				*uv_ptr++=bgy;

				*vertex_ptr++=(float)(-sin(rxz2)*top_reduce);
				*vertex_ptr++=f_ty;
				*vertex_ptr++=(float)(cos(rxz2)*top_reduce);

				*uv_ptr++=gx2;
				*uv_ptr++=tgy;

				*vertex_ptr++=(float)(-sin(rxz)*top_reduce);
				*vertex_ptr++=f_ty;
				*vertex_ptr++=(float)(cos(rxz)*top_reduce);

				*uv_ptr++=gx1;
				*uv_ptr++=tgy;
			}
			else {

					// trig 1

				*vertex_ptr++=(float)(-sin(rxz)*bot_reduce);
				*vertex_ptr++=f_by;
				*vertex_ptr++=(float)(cos(rxz)*bot_reduce);

				*uv_ptr++=gx1;
				*uv_ptr++=bgy;

				*vertex_ptr++=(float)(-sin(rxz2)*bot_reduce);
				*vertex_ptr++=f_by;
				*vertex_ptr++=(float)(cos(rxz2)*bot_reduce);

				*uv_ptr++=gx2;
				*uv_ptr++=bgy;

				*vertex_ptr++=0.0f;
				*vertex_ptr++=f_ty;
				*vertex_ptr++=0.0f;

				*uv_ptr++=0.5f;
				*uv_ptr++=0.01f;

					// trig 2

				*vertex_ptr++=(float)(-sin(rxz2)*bot_reduce);
				*vertex_ptr++=f_by;
				*vertex_ptr++=(float)(cos(rxz2)*bot_reduce);

				*uv_ptr++=gx2;
				*uv_ptr++=bgy;

				*vertex_ptr++=0.0f;
				*vertex_ptr++=f_ty;
				*vertex_ptr++=0.0f;

				*uv_ptr++=0.5f;
				*uv_ptr++=0.01f;
				
				*vertex_ptr++=0.0f;
				*vertex_ptr++=f_ty;
				*vertex_ptr++=0.0f;

				*uv_ptr++=0.5f;
				*uv_ptr++=0.01f;
			}

				// flipped dome

			if (map.sky.dome_mirror) {

				if (i!=4) {

						// trig 1

					*vertex_ptr++=(float)(-sin(rxz)*bot_reduce);
					*vertex_ptr++=f_by2;
					*vertex_ptr++=(float)(cos(rxz)*bot_reduce);
				
					*uv_ptr++=gx1;
					*uv_ptr++=bgy;

					*vertex_ptr++=(float)(-sin(rxz2)*bot_reduce);
					*vertex_ptr++=f_by2;
					*vertex_ptr++=(float)(cos(rxz2)*bot_reduce);
					
					*uv_ptr++=gx2;
					*uv_ptr++=bgy;

					*vertex_ptr++=(float)(-sin(rxz)*top_reduce);
					*vertex_ptr++=f_ty2;
					*vertex_ptr++=(float)(cos(rxz)*top_reduce);
					
					*uv_ptr++=gx1;
					*uv_ptr++=tgy;

						// trig 2

					*vertex_ptr++=(float)(-sin(rxz2)*bot_reduce);
					*vertex_ptr++=f_by2;
					*vertex_ptr++=(float)(cos(rxz2)*bot_reduce);
					
					*uv_ptr++=gx2;
					*uv_ptr++=bgy;

					*vertex_ptr++=(float)(-sin(rxz2)*top_reduce);
					*vertex_ptr++=f_ty2;
					*vertex_ptr++=(float)(cos(rxz2)*top_reduce);
					
					*uv_ptr++=gx2;
					*uv_ptr++=tgy;

					*vertex_ptr++=(float)(-sin(rxz)*top_reduce);
					*vertex_ptr++=f_ty2;
					*vertex_ptr++=(float)(cos(rxz)*top_reduce);
					
					*uv_ptr++=gx1;
					*uv_ptr++=tgy;
				}
				else {

						// trig 1

					*vertex_ptr++=(float)(-sin(rxz)*bot_reduce);
					*vertex_ptr++=f_by2;
					*vertex_ptr++=(float)(cos(rxz)*bot_reduce);
				
					*uv_ptr++=gx1;
					*uv_ptr++=bgy;

					*vertex_ptr++=(float)(-sin(rxz2)*bot_reduce);
					*vertex_ptr++=f_by2;
					*vertex_ptr++=(float)(cos(rxz2)*bot_reduce);
					
					*uv_ptr++=gx2;
					*uv_ptr++=bgy;

					*vertex_ptr++=0.0f;
					*vertex_ptr++=f_ty2;
					*vertex_ptr++=0.0f;

					*uv_ptr++=0.5f;
					*uv_ptr++=0.01f;

						// trig 2

					*vertex_ptr++=(float)(-sin(rxz2)*bot_reduce);
					*vertex_ptr++=f_by2;
					*vertex_ptr++=(float)(cos(rxz2)*bot_reduce);
					
					*uv_ptr++=gx2;
					*uv_ptr++=bgy;

					*vertex_ptr++=0.0f;
					*vertex_ptr++=f_ty2;
					*vertex_ptr++=0.0f;

					*uv_ptr++=0.5f;
					*uv_ptr++=0.01f;
					
					*vertex_ptr++=0.0f;
					*vertex_ptr++=f_ty2;
					*vertex_ptr++=0.0f;

					*uv_ptr++=0.5f;
					*uv_ptr++=0.01f;
				}
			}
			
			rxz+=r_add;
		}
	}

		// unmap and unbind vbo

  	view_unmap_sky_vertex_object();
	view_unbind_sky_vertex_object();
}

void draw_sky_dome_hemisphere(void)
{
    int					k,tick,txt_id,dome_cnt;
	float				txt_x_shift,txt_y_shift;
	texture_type		*texture;

		// texture sizes
		
	tick=game_time_get();
		
	txt_x_shift=((float)tick*0.0005f)*map.sky.txt_x_shift;
	k=(int)txt_x_shift;
	txt_x_shift=txt_x_shift-(float)k;
	
	txt_y_shift=((float)tick*0.0005f)*map.sky.txt_y_shift;
	k=(int)txt_y_shift;
	txt_y_shift=txt_y_shift-(float)k;

		// setup view

	gl_3D_view();
	gl_3D_rotate(NULL,&view.render->camera.ang);
	gl_setup_project();
	
		// setup texture
		
	gl_texture_simple_start();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	
	glMatrixMode(GL_TEXTURE);
	glTranslatef(txt_x_shift,txt_y_shift,0.0f);

	texture=&map.textures[map.sky.fill];
	txt_id=texture->frames[texture->animate.current_frame].bitmap.gl_id;
	
	gl_texture_simple_set(txt_id,FALSE,1,1,1,1);

		// bind the vbo

	view_bind_sky_vertex_object();

		// quad counts

	dome_cnt=(5*20)*6;
	if (map.sky.dome_mirror) dome_cnt*=2;

		// draw the dome
	
	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)((dome_cnt*3)*sizeof(float)));

	glDrawArrays(GL_TRIANGLES,0,dome_cnt);

		// end textures
		
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
		
	gl_texture_simple_end();

		// unbind the vbo

	view_unbind_sky_vertex_object();
}

/* =======================================================

      Draw Cube Sky
      
======================================================= */

void draw_sky_cube_setup(void)
{
    float				f_radius;
	float				*vertex_ptr,*uv_ptr;

		// construct VBO

	view_create_sky_vertex_object(((6*4)+(3*2))*sizeof(float));

	view_bind_sky_vertex_object();

	vertex_ptr=(float*)view_map_sky_vertex_object();
	if (vertex_ptr==NULL) {
		view_unbind_sky_vertex_object();
		return;
	}

	uv_ptr=vertex_ptr+((6*4)*3);

		// setup cube quads
		
	f_radius=(float)map.sky.radius;
		
		// top
		
	if (map.sky.fill!=-1) {
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=1.0f;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=1.0f;
		*uv_ptr++=1.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=1.0f;
		*uv_ptr++=0.0f;
	}
	
		// bottom
		
	if (map.sky.bottom_fill!=-1) {
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=1.0f;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=1.0f;
		*uv_ptr++=1.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=1.0f;
		*uv_ptr++=0.0f;
	}
	
		// north
		
	if (map.sky.north_fill!=-1) {
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=1.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=1.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=1.0f;
		*uv_ptr++=1.0f;
	}
	
		// east

	if (map.sky.east_fill!=-1) {
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=1.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=1.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=1.0f;
		*uv_ptr++=1.0f;
	}
	
		// south

	if (map.sky.south_fill!=-1) {
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=1.0f;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=1.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=1.0f;
		*uv_ptr++=1.0f;
	}
	
		// west

	if (map.sky.west_fill!=-1) {
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=1.0f;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=1.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=1.0f;
		*uv_ptr++=1.0f;
	}
 
		// unmap and unbind vbo

	view_unmap_sky_vertex_object();
	view_unbind_sky_vertex_object();
}

void draw_sky_cube(void)
{
    int					k,tick,txt_id,offset;
    float				txt_fact,txt_x_shift,txt_y_shift;
	texture_type		*texture;
					
		// texture sizes
		
	tick=game_time_get();
		
	txt_fact=map.sky.txt_fact;
	
	txt_x_shift=((float)tick*0.0005f)*map.sky.txt_x_shift;
	k=(int)txt_x_shift;
	txt_x_shift=txt_x_shift-(float)k;
	
	txt_y_shift=((float)tick*0.0005f)*map.sky.txt_y_shift;
	k=(int)txt_y_shift;
	txt_y_shift=txt_y_shift-(float)k;

		// setup view

	gl_3D_view();
	gl_3D_rotate(NULL,&view.render->camera.ang);
	gl_setup_project();

		// setup texture
		
	gl_texture_simple_start();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_TEXTURE);
	glTranslatef(txt_x_shift,txt_y_shift,0.0f);
	glScalef(txt_fact,txt_fact,1.0f);
	
		// bind VBO

	view_bind_sky_vertex_object();

		// draw cube sides

	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)(((6*4)*3)*sizeof(float)));

	offset=0;

	if (map.sky.fill!=-1) {
		texture=&map.textures[map.sky.fill];
		txt_id=texture->frames[texture->animate.current_frame].bitmap.gl_id;

		gl_texture_simple_set(txt_id,TRUE,1.0f,1.0f,1.0f,1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		
		offset+=4;
	}
	
		// bottom
		
	if (map.sky.bottom_fill!=-1) {
		texture=&map.textures[map.sky.bottom_fill];
		txt_id=texture->frames[texture->animate.current_frame].bitmap.gl_id;

		gl_texture_simple_set(txt_id,TRUE,1.0f,1.0f,1.0f,1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP,offset,4);
		
		offset+=4;
	}
	
		// north
		
	if (map.sky.north_fill!=-1) {
		texture=&map.textures[map.sky.north_fill];
		txt_id=texture->frames[texture->animate.current_frame].bitmap.gl_id;

		gl_texture_simple_set(txt_id,TRUE,1.0f,1.0f,1.0f,1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP,offset,4);
		
		offset+=4;
	}
	
		// east

	if (map.sky.east_fill!=-1) {
		texture=&map.textures[map.sky.east_fill];
		txt_id=texture->frames[texture->animate.current_frame].bitmap.gl_id;

		gl_texture_simple_set(txt_id,TRUE,1.0f,1.0f,1.0f,1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP,offset,4);
		
		offset+=4;
	}
	
		// south

	if (map.sky.south_fill!=-1) {
		texture=&map.textures[map.sky.south_fill];
		txt_id=texture->frames[texture->animate.current_frame].bitmap.gl_id;

		gl_texture_simple_set(txt_id,TRUE,1.0f,1.0f,1.0f,1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP,offset,4);
		
		offset+=4;
	}
	
		// west

	if (map.sky.west_fill!=-1) {
		texture=&map.textures[map.sky.west_fill];
		txt_id=texture->frames[texture->animate.current_frame].bitmap.gl_id;

		gl_texture_simple_set(txt_id,TRUE,1.0f,1.0f,1.0f,1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP,offset,4);
	}

		// end texture
		
	glLoadIdentity();
	gl_texture_simple_end();

		// unbind the vbo

	view_unbind_sky_vertex_object();
}

/* =======================================================

      Initialize Sky Drawing
      
======================================================= */

void draw_sky_init(void)
{
		// is there a sky?
	
	if (!map.sky.on) return;

		// init vertex list

	switch (map.sky.type) {
	
		case st_dome_panoramic:
			draw_sky_dome_panoramic_setup();
			break;

		case st_dome_hemisphere:
			draw_sky_dome_hemisphere_setup();
			break;
			
		case st_cube:
			draw_sky_cube_setup();
			break;
	
	}
}

void draw_sky_release(void)
{
	if (map.sky.on) view_dispose_sky_vertex_object();
}

/* =======================================================

      Draw Sky Mainline
      
======================================================= */

void draw_sky(void)
{
		// is there a sky?
	
	if (!map.sky.on) return;
	
		// draw specific sky type

	switch (map.sky.type) {
	
		case st_dome_panoramic:
			draw_sky_dome_panoramic();
			break;

		case st_dome_hemisphere:
			draw_sky_dome_hemisphere();
			break;
			
		case st_cube:
			draw_sky_cube();
			break;
	
	}
}


