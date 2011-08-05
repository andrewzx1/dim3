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

      Liquid Waves
      
======================================================= */

int liquid_wave_get_divisions(map_liquid_type *liq)
{
	int			len,count;

	if (!liq->wave.on) return(1);

	if (liq->wave.dir_north_south) {
		len=liq->bot-liq->top;
	}
	else {
		len=liq->rgt-liq->lft;
	}

	count=len/liq->wave.length;
	if ((len%liq->wave.length)!=0) count++;

	return(count);
}

void liquid_wave_get_high(map_liquid_type *liq,float *wave_y)
{
	float			f,fy,f_time,cs;

		// start with the tide height

	fy=((float)liq->y)-liquid_tide_get_high(liq);

		// if no waves, then flat Ys

	if (!liq->wave.on) {
		wave_y[0]=wave_y[1]=wave_y[2]=wave_y[3]=fy;
		return;
	}

		// get rate between 0..1

	f_time=(float)(game_time_get()%liq->wave.period_msec);
	f_time=f_time/((float)liq->wave.period_msec);
	
		// waves are sin waves

	cs=(float)cos((TRIG_PI*2.0f)*f_time);
	f=((float)liq->tide.high)*cs;
	
	wave_y[0]=fy-f;
	wave_y[1]=wave_y[3]=fy-(f*0.5f);
	wave_y[2]=fy-(f*0.25f);
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
	int				k,div,div_count,lft,rgt,top,bot,
					top_add,lft_add;
	float			fy,wave_y[4],f_tick,f_stamp_size,
					gx,gy,gx2,gy2,gx_add,gy_add,
					lmap_gx,lmap_gy,lmap_gx2,lmap_gy2,lmap_gx_add,lmap_gy_add;
	float			*vl,*uv,*uv2,*ct,*cn;
	unsigned char	*vertex_ptr,*cl;
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

	vl=(float*)vertex_ptr;
	uv=(float*)(vertex_ptr+((liq->vbo.vertex_count*3)*sizeof(float)));
	uv2=(float*)(vertex_ptr+((liq->vbo.vertex_count*(3+2))*sizeof(float)));

	if (!shader_on) {
		cl=vertex_ptr+((liq->vbo.vertex_count*(3+2+2))*sizeof(float));
	}
	else {
		ct=(float*)(vertex_ptr+((liq->vbo.vertex_count*(3+2+2))*sizeof(float)));
		cn=(float*)(vertex_ptr+((liq->vbo.vertex_count*(3+2+2+3))*sizeof(float)));
	}

		// build the tide and wave Y

	liquid_wave_get_high(liq,wave_y);

		// get div count and setup
		// the division calculations

	lft=liq->lft;
	rgt=liq->rgt;
	top=liq->top;
	bot=liq->bot;

	lmap_gx=liq->lmap_uv.x_offset;
	lmap_gx2=liq->lmap_uv.x_offset+liq->lmap_uv.x_size;

	lmap_gy=liq->lmap_uv.y_offset;
	lmap_gy2=liq->lmap_uv.y_offset+liq->lmap_uv.y_size;

	div_count=liquid_wave_get_divisions(liq);
	
	lft_add=rgt-lft;
	gx_add=gx2-gx;
	lmap_gx_add=lmap_gx2-lmap_gx;

	top_add=bot-top;
	gy_add=gy2-gy;
	lmap_gy_add=lmap_gy2-lmap_gy;

	if (liq->wave.on) {
		if (liq->wave.dir_north_south) {
			top_add=liq->wave.length;
			gy_add=(gy2-gy)/((float)div_count);
			lmap_gy_add=(lmap_gy2-lmap_gy)/((float)div_count);
		}
		else {
			lft_add=liq->wave.length;
			gx_add=(gx2-gx)/((float)div_count);
			lmap_gx_add=(lmap_gx2-lmap_gx)/((float)div_count);
		}
	}
	
		// draw the divisions

	for (div=0;div<=div_count;div++) {

		fy=wave_y[div&0x3];

		if (liq->wave.dir_north_south) {
			
				// left-top

			*vl++=(float)lft;
			*vl++=fy;
			*vl++=(float)top;

			*uv++=gx;
			*uv++=gy;

			*uv2++=lmap_gx;
			*uv2++=lmap_gy;

				// right-top

			*vl++=(float)rgt;
			*vl++=fy;
			*vl++=(float)top;

			*uv++=gx2;
			*uv++=gy;

			*uv2++=lmap_gx2;
			*uv2++=lmap_gy;
				
		}

		else {
			
				// left-top

			*vl++=(float)lft;
			*vl++=fy;
			*vl++=(float)top;

			*uv++=gx;
			*uv++=gy;

			*uv2++=lmap_gx;
			*uv2++=lmap_gy;

				// left-bottom

			*vl++=(float)lft;
			*vl++=fy;
			*vl++=(float)bot;

			*uv++=gx;
			*uv++=gy2;

			*uv2++=lmap_gx;
			*uv2++=lmap_gy2;
		}

			// no shaders use colors

		if (!shader_on) {
			gl_lights_calc_color_light_cache_byte(liq->light_cache.count,liq->light_cache.indexes,FALSE,(double)lft,fy,(double)top,cl);
			cl+=3;
			*cl++=0xFF;

			gl_lights_calc_color_light_cache_byte(liq->light_cache.count,liq->light_cache.indexes,FALSE,(double)lft,fy,(double)bot,cl);
			cl+=3;
			*cl++=0xFF;
		}

			// shaders use normals and tangents

		else {
			*ct++=1.0f;
			*ct++=0.0f;
			*ct++=0.0f;
			
			*ct++=1.0f;
			*ct++=0.0f;
			*ct++=0.0f;

			*cn++=0.0f;
			*cn++=-1.0f;
			*cn++=0.0f;
			
			*cn++=0.0f;
			*cn++=-1.0f;
			*cn++=0.0f;
		}

			// division changes

		if (liq->wave.dir_north_south) {
			top+=top_add;
			if (top>liq->bot) top=liq->bot;

			gy+=gy_add;
			if (gy>gy2) gy=gy2;

			lmap_gy+=lmap_gy_add;
			if (lmap_gy>lmap_gy2) lmap_gy=lmap_gy2;
		}
		else {
			lft+=lft_add;
			if (lft>liq->rgt) lft=liq->rgt;

			gx+=gx_add;
			if (gx>gx2) gx=gx2;

			lmap_gx+=lmap_gx_add;
			if (lmap_gx>lmap_gx2) lmap_gx=lmap_gx2;
		}
	}

	view_unmap_mesh_liquid_vertex_object();
	view_unbind_mesh_liquid_vertex_object();

		// indexes are pre-created on startup

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
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)((liq->vbo.vertex_count*(3+2))*sizeof(float)));
	
	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)((liq->vbo.vertex_count*3)*sizeof(float)));

		// shader lights and tangents

	gl_lights_build_liquid_light_list(liq,&light_list);
	
	tangent_offset=(liq->vbo.vertex_count*(3+2+2))*sizeof(float);
	normal_offset=(liq->vbo.vertex_count*(3+2+2+3))*sizeof(float);

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
		glDrawRangeElements(GL_TRIANGLE_STRIP,0,liq->vbo.index_count,liq->vbo.index_count,GL_UNSIGNED_SHORT,(GLvoid*)0);
	#else
		glDrawElements(GL_TRIANGLE_STRIP,liq->vbo.index_count,GL_UNSIGNED_SHORT,(GLvoid*)0);
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
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)((liq->vbo.vertex_count*3)*sizeof(float)));
	
	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)((liq->vbo.vertex_count*(3+2))*sizeof(float)));

	glColorPointer(4,GL_UNSIGNED_BYTE,0,(GLvoid*)((liq->vbo.vertex_count*(3+2+2))*sizeof(float)));

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
		glDrawRangeElements(GL_TRIANGLE_STRIP,0,liq->vbo.index_count,liq->vbo.index_count,GL_UNSIGNED_SHORT,(GLvoid*)0);
	#else
		glDrawElements(GL_TRIANGLE_STRIP,liq->vbo.index_count,GL_UNSIGNED_SHORT,(GLvoid*)0);
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
	
		// count the liquid polys
		
	view.count.liquid_poly+=((liq->vbo.vertex_count+2)>>2);

		// draw the overlay?

	if ((!liq->overlay.on) || (liq->overlay.txt_idx==-1)) return;

	is_transparent=liquid_is_transparent(liq);

	if (!is_transparent) {
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
	}

		// draw the overlay

	view.count.liquid_poly+=((liq->vbo.vertex_count+2)>>2);

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

