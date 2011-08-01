/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Segment Liquid Tide and Breakup Routines

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

extern map_type				map;
extern server_type			server;
extern view_type			view;
extern setup_type			setup;

extern bitmap_type		lmap_black_bitmap;

/* =======================================================

      Liquid Tides
      
======================================================= */

float liquid_tide_get_high(map_liquid_type *liq)
{
	float			f_time,sn;

		// get rate between 0..1

	f_time=(float)(game_time_get()%liq->tide.rate);
	f_time=f_time/((float)liq->tide.rate);
	
		// waves are sin waves

	sn=(float)sin((TRIG_PI*2.0f)*f_time);
	return(((float)liq->tide.high)*sn);
}

float liquid_tide_get_uv_factor(map_liquid_type *liq)
{
	float			f_time,sn,shift;

		// get rate between 0..1

	f_time=(float)(game_time_get()%liq->tide.rate);
	f_time=f_time/((float)liq->tide.rate);
	
		// waves are sin waves

	sn=(float)sin((TRIG_PI*2.0f)*f_time);
	
	shift=liq->tide.uv_shift*0.5f;
	return(shift+(shift*sn));
}

/* =======================================================

      Check if Liquid is Transparent
      
======================================================= */

bool liquid_is_transparent(map_liquid_type *liq)
{
	texture_type				*texture;

	texture=&map.textures[liq->txt_idx];
	return(texture->frames[texture->animate.current_frame&max_texture_frame_mask].bitmap.alpha_mode==alpha_mode_transparent);
}

/* =======================================================

      Setup Liquid Vertexes
      
======================================================= */

bool liquid_render_liquid_create_vertex(map_liquid_type *liq,float uv_shift,bool is_overaly)
{
	int				n,k;
	unsigned short	*index_ptr;
	float			fy,gx,gy,gx2,gy2,gx_add,gy_add,
					f_tick,f_stamp_size;
	float			*vertex_ptr,*vl,*uv,*uv2,*ct,*cn,*cl;
	bool			shader_on;
	
		// setup vbo
		// shaders don't need light lists
		
	shader_on=view_shader_on();

	view_bind_mesh_liquid_vertex_object(&liq->vbo);

	vertex_ptr=view_map_mesh_liquid_vertex_object(&liq->vbo);
	if (vertex_ptr==NULL) return(FALSE);

		// liquid texture uvs
		
	if (is_overaly) {
		f_tick=((float)game_time_get())*0.001f;

		gx_add=f_tick*liq->x_shift;
		k=(int)gx_add;
		gx_add=gx_add-(float)k;
		
		gy_add=f_tick*liq->y_shift;
		k=(int)gy_add;
		gy_add=gy_add-(float)k;

		f_stamp_size=1.0f/((float)liq->overlay.stamp_size);
		
		gx=((float)liq->lft)*f_stamp_size;
		gx2=((float)liq->rgt)*f_stamp_size;
		
		k=(int)gx;
		gx-=((float)k);
		gx2-=((float)k);
		
		gy=((float)liq->top)*f_stamp_size;
		gy2=((float)liq->bot)*f_stamp_size;
		
		k=(int)gx;
		gy-=((float)k);
		gy2-=((float)k);

		gx+=gx_add;
		gy+=gy_add;
		
		gx2+=gx_add;
		gy2+=gy_add;
	}
	else {
		gx=liq->main_uv.x_offset;
		gy=liq->main_uv.y_offset;

		gx2=gx+liq->main_uv.x_size;
		gy2=gy+liq->main_uv.y_size;
	}
	
		// uv factors

	gx+=uv_shift;
	gy+=uv_shift;
	gx2-=uv_shift;
	gy2-=uv_shift;

		// liquid vertexes

	vl=vertex_ptr;
	uv=vertex_ptr+(4*3);
	uv2=vertex_ptr+(4*(3+2));

	fy=((float)liq->y)-liquid_tide_get_high(liq);

		// left-top

	*vl++=(float)liq->lft;
	*vl++=fy;
	*vl++=(float)liq->top;

	*uv++=gx;
	*uv++=gy;

	*uv2++=liq->lmap_uv.x_offset;
	*uv2++=liq->lmap_uv.y_offset;

		// right-top

	*vl++=(float)liq->rgt;
	*vl++=fy;
	*vl++=(float)liq->top;

	*uv++=gx2;
	*uv++=gy;

	*uv2++=liq->lmap_uv.x_offset+liq->lmap_uv.x_size;
	*uv2++=liq->lmap_uv.y_offset;

		// left-bottom

	*vl++=(float)liq->lft;
	*vl++=fy;
	*vl++=(float)liq->bot;

	*uv++=gx;
	*uv++=gy2;

	*uv2++=liq->lmap_uv.x_offset;
	*uv2++=liq->lmap_uv.y_offset+liq->lmap_uv.y_size;

		// right-bottom

	*vl++=(float)liq->rgt;
	*vl++=fy;
	*vl++=(float)liq->bot;

	*uv++=gx2;
	*uv++=gy2;

	*uv2++=liq->lmap_uv.x_offset+liq->lmap_uv.x_size;
	*uv2++=liq->lmap_uv.y_offset+liq->lmap_uv.y_size;

		// colors

	if (!shader_on) {
		cl=vertex_ptr+(4*(3+2+2));

		gl_lights_calc_color_light_cache(liq->light_cache.count,liq->light_cache.indexes,FALSE,(double)liq->lft,fy,(double)liq->top,cl);
		gl_lights_calc_color_light_cache(liq->light_cache.count,liq->light_cache.indexes,FALSE,(double)liq->rgt,fy,(double)liq->top,(cl+4));
		gl_lights_calc_color_light_cache(liq->light_cache.count,liq->light_cache.indexes,FALSE,(double)liq->lft,fy,(double)liq->bot,(cl+8));
		gl_lights_calc_color_light_cache(liq->light_cache.count,liq->light_cache.indexes,FALSE,(double)liq->rgt,fy,(double)liq->bot,(cl+12));
		
		*(cl+3)=1.0f;
		*(cl+7)=1.0f;
		*(cl+11)=1.0f;
		*(cl+15)=1.0f;
	}

		// normals and tangents

	if (shader_on) {

		ct=vertex_ptr+(4*(3+2+2));
		cn=vertex_ptr+(4*(3+2+2+3));

		for (n=0;n!=4;n++) {
			*ct++=1.0f;
			*ct++=0.0f;
			*ct++=0.0f;
			*cn++=0.0f;
			*cn++=-1.0f;
			*cn++=0.0f;
		}
	}

	view_unmap_mesh_liquid_vertex_object();
	view_unbind_mesh_liquid_vertex_object();

		// the indexes

	view_bind_mesh_liquid_index_object(&liq->vbo);
	
	index_ptr=view_map_mesh_liquid_index_object(&liq->vbo);
	if (index_ptr==NULL) return(FALSE);

	*index_ptr++=0;
	*index_ptr++=1;
	*index_ptr++=2;
	*index_ptr++=3;

	view_unmap_mesh_liquid_index_object();
	view_unbind_mesh_liquid_index_object();

	return(TRUE);
}

/* =======================================================

      Liquid Rendering
      
======================================================= */

void liquid_render_liquid_shader(map_liquid_type *liq,int txt_idx,int lmap_txt_idx,bool back_rendering)
{
	int						frame,tangent_offset,normal_offset;
	float					alpha;
	GLuint					gl_id;
	texture_type			*texture;
	view_light_list_type	light_list;

		// setup texture

	texture=&map.textures[txt_idx];
	frame=texture->animate.current_frame&max_texture_frame_mask;

	if (texture->additive) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}
	
		// the liquid VBO
	
	view_bind_mesh_liquid_vertex_object(&liq->vbo);
	view_bind_mesh_liquid_index_object(&liq->vbo);

	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);

	glClientActiveTexture(GL_TEXTURE1);
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)((4*(3+2))*sizeof(float)));
	
	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)((4*3)*sizeof(float)));

		// shader lights and tangents

	gl_lights_build_liquid_light_list(liq,&light_list);
	
	tangent_offset=(4*(3+2+2))*sizeof(float);
	normal_offset=(4*(3+2+2+3))*sizeof(float);

	gl_shader_draw_start();
	gl_shader_draw_reset_normal_tangent_attrib();
	gl_shader_draw_execute(core_shader_group_liquid,texture,txt_idx,frame,lmap_txt_idx,1.0f,&light_list,tangent_offset,normal_offset);
			
		// fix texture if any back rendering
		
	gl_id=texture->frames[frame].bitmap.gl_id;
	alpha=1.0f;

	if (back_rendering) {
		if (gl_back_render_get_texture(liq->camera,&gl_id,&alpha)) {
			gl_shader_texture_override(gl_id,alpha);
		}
	}
	
		// draw liquid
	
	#ifndef D3_OPENGL_ES
		glDrawRangeElements(GL_TRIANGLE_STRIP,0,4,4,GL_UNSIGNED_SHORT,(GLvoid*)0);
	#else
		glDrawElements(GL_TRIANGLE_STRIP,4,GL_UNSIGNED_SHORT,(GLvoid*)0);
	#endif

	gl_shader_draw_end();
	
	view_unbind_mesh_liquid_vertex_object();
	view_unbind_mesh_liquid_index_object();
}

void liquid_render_liquid_fixed(map_liquid_type *liq,int txt_idx,int lmap_txt_idx,bool back_rendering)
{
	int						frame;
	float					alpha;
	GLuint					gl_id,lmap_gl_id;
	texture_type			*texture;

		// only this route uses color arrays
		
	glEnableClientState(GL_COLOR_ARRAY);

		// setup texture

	texture=&map.textures[txt_idx];
	frame=texture->animate.current_frame&max_texture_frame_mask;

	if (texture->additive) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}

		// the liquid VBO
	
	view_bind_mesh_liquid_vertex_object(&liq->vbo);
	view_bind_mesh_liquid_index_object(&liq->vbo);

	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);

	glClientActiveTexture(GL_TEXTURE1);
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)((4*3)*sizeof(float)));
	
	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)((4*(3+2))*sizeof(float)));

	glColorPointer(4,GL_FLOAT,0,(GLvoid*)((4*(3+2+2))*sizeof(float)));

		// back rendering overrides
		
	gl_id=texture->frames[frame].bitmap.gl_id;
	alpha=1.0f;
		
	if (back_rendering) {
		if (!gl_back_render_get_texture(liq->camera,&gl_id,&alpha)) {
			gl_id=texture->frames[frame].bitmap.gl_id;
		}
	}

		// light map

	if (lmap_txt_idx==-1) {
		lmap_gl_id=lmap_black_bitmap.gl_id;
	}
	else {
		lmap_gl_id=map.textures[lmap_txt_idx].frames[0].bitmap.gl_id;
	}

	gl_texture_transparent_light_map_start();
	gl_texture_transparent_light_map_set(gl_id,lmap_gl_id,alpha);

		// draw liquid
	
	#ifndef D3_OPENGL_ES
		glDrawRangeElements(GL_TRIANGLE_STRIP,0,4,4,GL_UNSIGNED_SHORT,(GLvoid*)0);
	#else
		glDrawElements(GL_TRIANGLE_STRIP,4,GL_UNSIGNED_SHORT,(GLvoid*)0);
	#endif
	
		// only this route uses color arrays

	glDisableClientState(GL_COLOR_ARRAY);

	gl_texture_transparent_light_map_end();

	view_unbind_mesh_liquid_vertex_object();
	view_unbind_mesh_liquid_index_object();
}

void liquid_render_liquid(map_liquid_type *liq)
{
	float				uv_shift;
	bool				shader_on,is_transparent;

	shader_on=view_shader_on();

		// uv factor

	uv_shift=liquid_tide_get_uv_factor(liq);

		// draw the reflection liquid
		// or just the regular texture

	if (!liquid_render_liquid_create_vertex(liq,uv_shift,FALSE)) return;

	if (shader_on) {
		liquid_render_liquid_shader(liq,liq->txt_idx,liq->lmap_txt_idx,TRUE);
	}
	else {
		liquid_render_liquid_fixed(liq,liq->txt_idx,liq->lmap_txt_idx,TRUE);
	}
	
		// count the liquid
		
	view.count.liquid++;

		// draw the overlay?

	if ((!liq->overlay.on) || (liq->overlay.txt_idx==-1)) return;

	is_transparent=liquid_is_transparent(liq);

	if (!is_transparent) {
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
	}

		// draw the overlay

	if (!liquid_render_liquid_create_vertex(liq,(uv_shift*0.5f),TRUE)) return;

	if (shader_on) {
		liquid_render_liquid_shader(liq,liq->overlay.txt_idx,liq->lmap_txt_idx,FALSE);
	}
	else {
		liquid_render_liquid_fixed(liq,liq->overlay.txt_idx,liq->lmap_txt_idx,FALSE);
	}

	if (!is_transparent) {
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}
}

/* =======================================================

      Liquid Rendering
      
======================================================= */

void render_map_liquid_opaque(void)
{
	int					n;
	map_liquid_type		*liq;

		// setup view

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();
	
		// common setup

	glDisable(GL_BLEND);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
						
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glEnableClientState(GL_VERTEX_ARRAY);
	
	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
		// draw opaque liquids
		
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type==view_render_type_liquid) {
			liq=&map.liquid.liquids[view.render->draw_list.items[n].idx];
			if (!liquid_is_transparent(liq)) {
				liquid_render_liquid(liq);
			}
		}
	}

		// common finish
		
	glDisableClientState(GL_VERTEX_ARRAY);

	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void render_map_liquid_transparent(void)
{
	int					n;
	map_liquid_type		*liq;

		// setup view

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();
	
		// common setup

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
						
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// draw transparent liquids
		
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type==view_render_type_liquid) {
			liq=&map.liquid.liquids[view.render->draw_list.items[n].idx];
			if (liquid_is_transparent(liq)) {
				liquid_render_liquid(liq);
			}
		}
	}

		// common finish
		
	glDisableClientState(GL_VERTEX_ARRAY);

	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

