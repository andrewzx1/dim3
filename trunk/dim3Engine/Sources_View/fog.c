/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Fog

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

extern map_type				map;
extern server_type			server;
extern view_type			view;
extern setup_type			setup;

bool						fog_vbo_created;

/* =======================================================

      Fog Initialize and Release
      
======================================================= */

void fog_draw_init(void)
{
	fog_vbo_created=FALSE;
}

void fog_draw_release(void)
{
	if (fog_vbo_created) view_dispose_fog_vertex_object();
}

/* =======================================================

      Textured Fog
      
======================================================= */

void fog_draw_textured(void)
{
	int					n,k,tick,count,outer_radius,inner_radius,
						radius_add,radius,frame,mem_sz;
	unsigned long		gl_id;
	float				r_ang,r_ang_2,r_add,fx,fz,fx_1,fx_2,fz_1,fz_2,f_ty,f_by,
						f_radius,gx,gx_add,gx_shift;
	float				*vertex_ptr,*uv_ptr;
	texture_type		*texture;
	
		// textured fog on?
		
	if ((!map.fog.on) || (map.fog.use_solid_color)) return;
	
		// if this is first time, setup VBO
		
	if (!fog_vbo_created) {
		fog_vbo_created=TRUE;
		
		mem_sz=(((16*6)*map.fog.count)*(3+2))*sizeof(float);
		view_create_fog_vertex_object(mem_sz);
	}

		// setup viewpoint
		
	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();

		// drawing layers
	
	count=map.fog.count;
	outer_radius=map.fog.outer_radius;
	inner_radius=map.fog.inner_radius;

	radius_add=(inner_radius-outer_radius)/count;

		// construct VBO
		
	view_bind_fog_vertex_object();

	vertex_ptr=(float*)view_map_fog_vertex_object();
	if (vertex_ptr==NULL) {
		view_unbind_fog_vertex_object();
		return;
	}

	uv_ptr=vertex_ptr+(((16*6)*count)*3);

		// get drawing setup

	tick=game_time_get();

	r_add=ANG_to_RAD*(360.0f/16.0f);

	radius=outer_radius;
	
	fx=(float)view.render->camera.pnt.x;
	fz=(float)view.render->camera.pnt.z;

	f_ty=(float)(view.render->camera.pnt.y-map.fog.high);
	f_by=(float)(view.render->camera.pnt.y+map.fog.drop);

	gx_add=map.fog.txt_fact.x/16.0f;
	gx_shift=1.0f/((float)count);

		// create the fog triangle vertexes

	for (n=0;n!=count;n++) {
	
		gx=gx_shift*((float)n);

		r_ang=0.0f;
		f_radius=(float)radius;

		for (k=0;k!=16;k++) {

			r_ang_2=r_ang+r_add;

			fx_1=(-sinf(r_ang)*f_radius)+fx;
			fx_2=(-sinf(r_ang_2)*f_radius)+fx;

			fz_1=-(cosf(r_ang)*f_radius)+fz;
			fz_2=-(cosf(r_ang_2)*f_radius)+fz;

				// triangle 1
				
			*vertex_ptr++=fx_1;
			*vertex_ptr++=f_ty;
			*vertex_ptr++=fz_1;

			*uv_ptr++=gx;
			*uv_ptr++=0.0f;
			
			*vertex_ptr++=fx_2;
			*vertex_ptr++=f_ty;
			*vertex_ptr++=fz_2;

			*uv_ptr++=gx+gx_add;
			*uv_ptr++=0.0f;
			
			*vertex_ptr++=fx_1;
			*vertex_ptr++=f_by;
			*vertex_ptr++=fz_1;

			*uv_ptr++=gx;
			*uv_ptr++=map.fog.txt_fact.y;
			
				// triangle 2
			
			*vertex_ptr++=fx_2;
			*vertex_ptr++=f_ty;
			*vertex_ptr++=fz_2;

			*uv_ptr++=gx+gx_add;
			*uv_ptr++=0.0f;

			*vertex_ptr++=fx_2;
			*vertex_ptr++=f_by;
			*vertex_ptr++=fz_2;

			*uv_ptr++=gx+gx_add;
			*uv_ptr++=map.fog.txt_fact.y;
			
			*vertex_ptr++=fx_1;
			*vertex_ptr++=f_by;
			*vertex_ptr++=fz_1;

			*uv_ptr++=gx;
			*uv_ptr++=map.fog.txt_fact.y;

			r_ang=r_ang_2;
			gx+=gx_add;
		}

		radius+=radius_add;
	}

  	view_unmap_fog_vertex_object();

		// setup drawing
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	texture=&map.textures[map.fog.texture_idx];
	frame=texture->animate.current_frame&max_texture_frame_mask;
	gl_id=texture->frames[frame].bitmap.gl_id;
	
	gl_texture_simple_start();
	gl_texture_simple_set(gl_id,FALSE,1.0f,1.0f,1.0f,map.fog.alpha);

		// draw the fog
	
	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)((((16*6)*count)*3)*sizeof(float)));

	glDrawArrays(GL_TRIANGLES,0,((16*6)*count));

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		// end texture

	gl_texture_simple_end();

		// unbind the vbo

	view_unbind_fog_vertex_object();
}

/* =======================================================

      Solid Fog
      
======================================================= */

bool fog_solid_on(void)
{
	return((map.fog.on) && (map.fog.use_solid_color));
}

void fog_solid_start(void)
{
	GLfloat		col[4];

	glEnable(GL_FOG);

	col[0]=map.fog.col.r;
	col[1]=map.fog.col.g;
	col[2]=map.fog.col.b;
	col[3]=1.0f;

#ifndef D3_OPENGL_ES
	glFogfv(GL_FOG_COLOR,col);
	glFogi(GL_FOG_MODE,GL_LINEAR);
	glFogi(GL_FOG_START,map.fog.inner_radius);
	glFogi(GL_FOG_END,map.fog.outer_radius);
	glFogi(GL_FOG_COORD_SRC,GL_FRAGMENT_DEPTH);
#else
	glFogfv(GL_FOG_COLOR,col);
	glFogx(GL_FOG_MODE,GL_LINEAR);
	glFogf(GL_FOG_START,(GLfloat)map.fog.inner_radius);
	glFogf(GL_FOG_END,(GLfloat)map.fog.outer_radius);
#endif
}

void fog_solid_end(void)
{
	glDisable(GL_FOG);
}

