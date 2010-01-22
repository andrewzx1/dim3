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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "consoles.h"
#include "video.h"
#include "lights.h"

extern map_type				map;
extern server_type			server;
extern view_type			view;
extern setup_type			setup;

extern bool					dim3_debug;

extern bool boundbox_inview(int x,int z,int ex,int ez,int ty,int by);

/* =======================================================

      Initialize Liquid GL Lists
      
======================================================= */

inline int liquid_render_liquid_get_tide_split(map_liquid_type *liq)
{
	int				tide_split;

	tide_split=liq->rgt-liq->lft;
	if ((liq->bot-liq->top)>tide_split) tide_split=liq->bot-liq->top;
	
	if (liq->tide.flat) return(tide_split);
	
	return(tide_split/liq->tide.division);
}

inline int liquid_render_liquid_get_max_vertex(map_liquid_type *liq)
{
	int				tide_split,x_sz,z_sz;
	
	tide_split=liquid_render_liquid_get_tide_split(liq);

	x_sz=((liq->rgt-liq->lft)/tide_split)+4;		// possible extra edges on side because of griding
	z_sz=((liq->bot-liq->top)/tide_split)+4;

	return(x_sz*z_sz);
}

void liquid_gl_list_init(void)
{
	int					n,v_sz,sz;
	map_liquid_type		*liq;

	sz=0;

	liq=map.liquid.liquids;

	for (n=0;n!=map.liquid.nliquid;n++) {
		v_sz=liquid_render_liquid_get_max_vertex(liq);
		if (v_sz>sz) sz=v_sz;
		liq++;
	}

	view_init_liquid_vertex_object(v_sz*(3+2+3+2));
	view_init_liquid_index_object(v_sz*4);
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

void liquid_render_liquid_create_vertex(int tick,map_liquid_type *liq,int v_sz,bool shader_on)
{
	int				x,y,z,k,x_add,z_add,x_sz,z_sz,
					v_cnt,tide_split,tide_split_half,
					tide_high,tide_rate;
	float			fy,fgx,fgy,x_txtoff,y_txtoff,
					x_2_txtoff,y_2_txtoff,
					f_break,f_time,f_tick,sn,
					f_tide_split_half,f_tide_high;
	bool			x_break,z_break;
	float			*vertex_ptr,*vl,*uv,*uv2,*cl;
	
	y=liq->y;
	fy=(float)y;

		// setup vbo

	vertex_ptr=view_bind_map_liquid_vertex_object();
	if (vertex_ptr==NULL) return;
	
	vl=vertex_ptr;
	uv=vertex_ptr+(v_sz*3);
	cl=vertex_ptr+(v_sz*(3+2));
	if (liq->lmap_txt_idx!=-1) uv2=vertex_ptr+(v_sz*(3+2+3));

		// setup tiding

	tide_split=liquid_render_liquid_get_tide_split(liq);
	
	tide_high=liq->tide.high;
	if (tide_high<1) tide_high=1;
	
	tide_rate=liq->tide.rate;
	if (tide_rate<1) tide_rate=1;

	tide_split_half=tide_split<<2;
	f_tide_split_half=(float)tide_split_half;
	
	f_tide_high=(float)tide_high;

	f_time=(float)(tick%tide_rate);		// get rate between 0..1
	f_time=f_time/(float)tide_rate;
	
		// liquid texture movement
		
	f_tick=(float)tick*0.001f;
	
	x_txtoff=f_tick*liq->x_shift;
	k=(int)x_txtoff;
	x_txtoff=liq->uv[0].x_offset+(x_txtoff-(float)k);
	
	y_txtoff=f_tick*liq->y_shift;
	k=(int)y_txtoff;
	y_txtoff=liq->uv[0].y_offset+(y_txtoff-(float)k);
	
	if (liq->lmap_txt_idx!=-1) {
		x_2_txtoff=liq->uv[1].x_offset;
		y_2_txtoff=liq->uv[1].y_offset;
	}

		// setup vertex calcing

	gl_lights_calc_vertex_setup_liquid(liq);

		// create vertexes from tide splits

	fgx=(float)(liq->rgt-liq->lft);
	fgy=(float)(liq->bot-liq->top);
		
	z=liq->top;
	z_add=tide_split-(z%tide_split);
	z_break=FALSE;

	v_cnt=0;

	z_sz=0;
	
	while (TRUE) {

		x=liq->lft;
		x_add=tide_split-(x%tide_split);
		x_break=FALSE;

		x_sz=0;
		
		while (TRUE) {

				// color

			if ((dim3_debug) || (shader_on)) {
				*cl=*(cl+1)=*(cl+2)=1.0f;
			}
			else {
				gl_lights_calc_vertex((double)x,(double)y,(double)z,TRUE,cl);
			}
			
			cl+=3;

				// setup tide Y

			if (liq->tide.direction==liquid_direction_vertical) {
				f_break=(float)(z%tide_split_half);
			}
			else {
				f_break=(float)(x%tide_split_half);
			}
				
			f_break=f_break/f_tide_split_half;
		   
			sn=(float)sin((TRIG_PI*2.0f)*(f_break+f_time));

				// vertex and uvs

			*vl++=(float)x;
			*vl++=fy-(f_tide_high*sn);
			*vl++=(float)z;

			*uv++=x_txtoff+((liq->uv[0].x_size*(float)(x-liq->lft))/fgx);
			*uv++=y_txtoff+((liq->uv[0].y_size*(float)(z-liq->top))/fgy);

			if (liq->lmap_txt_idx!=-1) {
				*uv2++=x_2_txtoff+((liq->uv[1].x_size*(float)(x-liq->lft))/fgx);
				*uv2++=y_2_txtoff+((liq->uv[1].y_size*(float)(z-liq->top))/fgy);
			}

			v_cnt++;
			
			x_sz++;

			if (x_break) break;
			
			x+=x_add;
			x_add=tide_split;
			
			if (x>=liq->rgt) {
				x=liq->rgt;
				x_break=TRUE;
			}
		}

		z_sz++;
		
		if (z_break) break;
		
		z+=z_add;
		z_add=tide_split;
		
		if (z>=liq->bot) {
			z=liq->bot;
			z_break=TRUE;
		}
	}

	view_unmap_liquid_vertex_object();

		// setup split sizes

	liq->draw.v_cnt=v_cnt;

	liq->draw.x_sz=x_sz;
	liq->draw.z_sz=z_sz;
}

int liquid_render_liquid_create_quads(map_liquid_type *liq)
{
	int				x,z,x_sz,z_sz,quad_cnt,
					tz,bz,tz_add,top_row,bot_row,
					lx,rx,lx_add,tide_split;
	unsigned int	*index_ptr;

		// drawing the quads only draws to the edges

	x_sz=liq->draw.x_sz-1;
	if (x_sz<=0) return(0);

	z_sz=liq->draw.z_sz-1;
	if (z_sz<=0) return(0);

	tide_split=liquid_render_liquid_get_tide_split(liq);

		// setup index vbo

	index_ptr=view_bind_map_liquid_index_object();
	if (index_ptr==NULL) return(0);

		// create the draw indexes

	quad_cnt=0;
		
	tz=liq->top;
	tz_add=tide_split-(tz%tide_split);
	top_row=0;
	
	for (z=0;z!=z_sz;z++) {
		bz=tz+tz_add;
		tz_add=tide_split;
		if (bz>=liq->bot) bz=liq->bot;
		
		lx=liq->lft;
		lx_add=tide_split-(lx%tide_split);
		bot_row=top_row+(x_sz+1);
		
		for (x=0;x!=x_sz;x++) {
			rx=lx+lx_add;
			lx_add=tide_split;
			if (rx>=liq->rgt) rx=liq->rgt;
			
			*index_ptr++=(unsigned int)(top_row+x);
			*index_ptr++=(unsigned int)(top_row+(x+1));
			*index_ptr++=(unsigned int)(bot_row+(x+1));
			*index_ptr++=(unsigned int)(bot_row+x);

			quad_cnt++;

			lx=rx;
		}
	
		tz=bz;
		top_row=bot_row;
	}

	view_unmap_liquid_index_object();

	return(quad_cnt);
}

/* =======================================================

      Liquid Rendering
      
======================================================= */

void liquid_render_liquid(int tick,map_liquid_type *liq)
{
	int						v_sz,quad_cnt,frame;
	bool					shader_on;
	texture_type			*texture,*lmap_texture;
	view_light_list_type	light_list;

		// setup texture

	texture=&map.textures[liq->txt_idx];
	frame=texture->animate.current_frame&max_texture_frame_mask;

	if (texture->additive) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}

		// determine if shader in use

	shader_on=(!dim3_debug) && (texture->shader_idx!=-1);

		// create vertexes

	v_sz=liquid_render_liquid_get_max_vertex(liq);

	liquid_render_liquid_create_vertex(tick,liq,v_sz,shader_on);

		// create quads

	quad_cnt=liquid_render_liquid_create_quads(liq);
	if (quad_cnt==0) {
		view_unbind_liquid_vertex_object();
		return;
	}
	
		// start vertex and UV array

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,0);
		
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)((v_sz*3)*sizeof(float)));
	
	if (liq->lmap_txt_idx!=-1) {
		glClientActiveTexture(GL_TEXTURE1);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,0,(void*)((v_sz*(3+2+3))*sizeof(float)));
		glClientActiveTexture(GL_TEXTURE0);
	}

		// shader drawing

	if (shader_on) {

		gl_lights_build_from_liquid(liq,&light_list);

		gl_shader_draw_start();
		gl_shader_draw_execute(texture,liq->txt_idx,frame,liq->lmap_txt_idx,1.0f,&light_list,NULL,NULL,FALSE);

		glDrawElements(GL_QUADS,(quad_cnt*4),GL_UNSIGNED_INT,(GLvoid*)0);
		
		gl_shader_draw_end();

	}

		// simple, non-shader drawing

	else {

			// need color pointers for simple drawing

		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3,GL_FLOAT,0,(void*)((v_sz*(3+2))*sizeof(float)));

			// simple light map drawing

		if (liq->lmap_txt_idx!=-1) {
			lmap_texture=&map.textures[liq->lmap_txt_idx];

			gl_texture_transparent_light_map_start();
			gl_texture_transparent_light_map_set(texture->frames[frame].bitmap.gl_id,lmap_texture->frames[0].bitmap.gl_id,1.0f);
			glDrawElements(GL_QUADS,(quad_cnt*4),GL_UNSIGNED_INT,(GLvoid*)0);
			gl_texture_transparent_light_map_end();
		}

			// simple drawing

		else {
			gl_texture_transparent_start();
			gl_texture_transparent_set(texture->frames[frame].bitmap.gl_id,1.0f);
			glDrawElements(GL_QUADS,(quad_cnt*4),GL_UNSIGNED_INT,(GLvoid*)0);
			gl_texture_transparent_end();
		}

			// disable color array

		glDisableClientState(GL_COLOR_ARRAY);
	}

		// end arrays

	if (liq->lmap_txt_idx!=-1) {
		glClientActiveTexture(GL_TEXTURE1);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glClientActiveTexture(GL_TEXTURE0);
	}

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

		// unmap VBOs

	view_unbind_liquid_index_object();
	view_unbind_liquid_vertex_object();
}

/* =======================================================

      Liquid Rendering
      
======================================================= */

void render_map_liquid_opaque(int tick)
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
	glDepthMask(GL_FALSE);
	
		// draw opaque liquids
		
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type==view_render_type_liquid) {
			liq=&map.liquid.liquids[view.render->draw_list.items[n].idx];
			if (!liquid_is_transparent(liq)) {
				liquid_render_liquid(tick,liq);
			}
		}
	}
}

void render_map_liquid_transparent(int tick)
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
				liquid_render_liquid(tick,liq);
			}
		}
	}
}

