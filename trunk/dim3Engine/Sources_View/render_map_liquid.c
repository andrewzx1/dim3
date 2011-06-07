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

extern bool boundbox_inview(int x,int z,int ex,int ez,int ty,int by);

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

void liquid_render_liquid_create_vertex(map_liquid_type *liq,float uv_shift,bool is_overaly)
{
	int				n,k,vbo_cnt;
	float			fy,gx,gy,gx2,gy2,gx_add,gy_add,
					f_tick,f_stamp_size;
	bool			shader_on;
	float			*vertex_ptr,*vl,*uv,*uv2,*ct,*cn,*cl;
	
		// setup vbo
		// shaders don't need light lists
		
	shader_on=view_shader_on();

	if (shader_on) {
		vbo_cnt=4*(3+2+2+3+3);
	}
	else {
		vbo_cnt=4*(3+2+2+3);
	}

	vertex_ptr=view_bind_map_liquid_vertex_object(vbo_cnt);
	if (vertex_ptr==NULL) return;

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
		gl_lights_calc_color_light_cache(liq->light_cache.count,liq->light_cache.indexes,FALSE,(double)liq->rgt,fy,(double)liq->top,(cl+3));
		gl_lights_calc_color_light_cache(liq->light_cache.count,liq->light_cache.indexes,FALSE,(double)liq->lft,fy,(double)liq->bot,(cl+6));
		gl_lights_calc_color_light_cache(liq->light_cache.count,liq->light_cache.indexes,FALSE,(double)liq->rgt,fy,(double)liq->bot,(cl+9));
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

	view_unmap_liquid_vertex_object();
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
	
		// start vertex array

	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,0);

		// shader UVs

	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)((4*(3+2))*sizeof(float)));
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)((4*3)*sizeof(float)));

		// shader lights and tangents

	gl_lights_build_liquid_light_list(liq,&light_list);
	
	tangent_offset=(4*(3+2+2))*sizeof(float);
	normal_offset=(4*(3+2+2+3))*sizeof(float);

	gl_shader_draw_start();
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
	
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	
	gl_shader_draw_end();

		// end arrays

	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void liquid_render_liquid_fixed(map_liquid_type *liq,int txt_idx,int lmap_txt_idx,bool back_rendering)
{
	int						frame;
	float					alpha;
	GLuint					gl_id,lmap_gl_id;
	texture_type			*texture;

		// setup texture

	texture=&map.textures[txt_idx];
	frame=texture->animate.current_frame&max_texture_frame_mask;

	if (texture->additive) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}
	
		// start vertex array

	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,0);

		// FF UVs

	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)((4*3)*sizeof(float)));
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)((4*(3+2))*sizeof(float)));

		// need color pointers for fixed function

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,0,(void*)((4*(3+2+2))*sizeof(float)));

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
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	gl_texture_transparent_light_map_end();

		// disable color array

	glDisableClientState(GL_COLOR_ARRAY);

		// end arrays

	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void liquid_render_liquid(map_liquid_type *liq)
{
	float				uv_shift;
	bool				shader_on,is_transparent;

	shader_on=view_shader_on();

		// uv factor

	uv_shift=liquid_tide_get_uv_factor(liq);

		// draw the reflection liquid

	liquid_render_liquid_create_vertex(liq,uv_shift,FALSE);

	if (shader_on) {
		liquid_render_liquid_shader(liq,liq->txt_idx,liq->lmap_txt_idx,TRUE);
	}
	else {
		liquid_render_liquid_fixed(liq,liq->txt_idx,liq->lmap_txt_idx,TRUE);
	}

	view_unbind_liquid_vertex_object();
	
		// count the liquid
		
	view.count.liquid++;

		// draw the overlay?

	if (!liq->overlay.on) return;

	is_transparent=liquid_is_transparent(liq);

	if (!is_transparent) {
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
	}

		// draw the overlay

	liquid_render_liquid_create_vertex(liq,(uv_shift*0.5f),TRUE);

	if (shader_on) {
		liquid_render_liquid_shader(liq,liq->overlay.txt_idx,liq->lmap_txt_idx,FALSE);
	}
	else {
		liquid_render_liquid_fixed(liq,liq->overlay.txt_idx,liq->lmap_txt_idx,FALSE);
	}

	view_unbind_liquid_vertex_object();

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
	
		// setup drawing

	glDisable(GL_BLEND);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
						
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	
		// draw opaque liquids
		
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type==view_render_type_liquid) {
			liq=&map.liquid.liquids[view.render->draw_list.items[n].idx];
			if (!liquid_is_transparent(liq)) {
				liquid_render_liquid(liq);
			}
		}
	}
}

void render_map_liquid_transparent(void)
{
	int					n;
	map_liquid_type		*liq;

		// setup view

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();
	
		// setup drawing

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
						
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	
		// draw transparent liquids
		
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type==view_render_type_liquid) {
			liq=&map.liquid.liquids[view.render->draw_list.items[n].idx];
			if (liquid_is_transparent(liq)) {
				liquid_render_liquid(liq);
			}
		}
	}
}

