/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: HUD Routines

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
extern server_type			server;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern render_info_type		render_info;

/* =======================================================

      Run HUD Item Fades
      
======================================================= */

bool hud_item_fade_run(iface_item_fade_type *fade,float *alpha)
{
	int			fade_tick;
	float		achg;

		// if fade not on then do nothing
	
	if (!fade->on) return(FALSE);
	
		// fade still on?
		
	fade_tick=game_time_get()-fade->start_tick;
	
	if (fade_tick>=(fade->fade_in_tick+fade->life_tick+fade->fade_out_tick)) {
		fade->on=FALSE;
		return(TRUE);
	}
	
	if (fade_tick>(fade->fade_in_tick+fade->life_tick)) {
		achg=1.0f-((float)(fade_tick-(fade->fade_in_tick+fade->life_tick))/(float)fade->fade_out_tick);
	}
	else {
		if (fade_tick>fade->fade_in_tick) {
			achg=1.0f;
		}
		else {
			achg=(float)fade_tick/(float)fade->fade_in_tick;
		}
	}
	
	*alpha=(*alpha)*achg;

	return(FALSE);
}

/* =======================================================

      Draw HUD Bitmaps
      
======================================================= */

void hud_bitmaps_draw(void)
{
	int							n,tick,r,wrap_count,
								px[4],py[4],sx,sy,rx,ry,
								wid,high,repeat_count;
	float						gx,gy,gx2,gy2,g_size,alpha,cur_alpha;
	float						*vp,*uv,*vertex_ptr,*uv_ptr;
	GLuint						cur_gl_id;
	d3col						tint,cur_tint,team_tint;
	iface_bitmap_type			*bitmap;
	obj_type					*obj;
	bitmap_type					*bitmap_data;
	
	cur_gl_id=-1;
	cur_alpha=1.0f;
	cur_tint.r=cur_tint.g=cur_tint.b=1.0f;
	
	obj=server.obj_list.objs[server.player_obj_idx];
	object_get_tint(obj,&team_tint);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);

	gl_texture_simple_start();

		// start vertex ptrs

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// timing for flashes and fades

	tick=game_time_get();
	
		// draw bitmaps
		
	for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
		bitmap=&iface.bitmap_list.bitmaps[n];
		if (!bitmap->show) continue;

			// flashing

		if (bitmap->flash) {
			if (((tick>>9)&0x1)==0x0) continue;		// half-second flash
		}
		
			// fading?
			
		alpha=bitmap->alpha;
		if (hud_item_fade_run(&bitmap->fade,&alpha)) {
			bitmap->show=FALSE;			// a fade has turned off bitmap
			continue;
		}
		
			// tint
			
		if (!bitmap->team_tint) {
			tint.r=tint.g=tint.b=1.0f;
		}
		else {
			memmove(&tint,&team_tint,sizeof(d3col));
		}

            // get bitmap and position
			
		bitmap_data=view_images_get_bitmap(bitmap->image_idx);
            
		sx=bitmap->x;
		sy=bitmap->y;
		
		wid=bitmap->x_size;
		if (wid==-1) wid=bitmap_data->wid;

		high=bitmap->y_size;
		if (high==-1) high=bitmap_data->high;
		
			// setup images
			
		effect_image_animate_get_uv((tick-bitmap->show_tick),&bitmap->animate,&gx,&gy,&g_size);

            // flips
           
		if (!bitmap->flip_horz) {
			gx2=gx+g_size;
		}
		else {
			gx2=gx;
			gx+=g_size;
		}

		if (!bitmap->flip_vert) {
			gy2=gy+g_size;
		}
		else {
			gy2=gy;
			gy+=g_size;
		}

			// need to change texture?

		if ((cur_gl_id!=bitmap_data->gl_id) || (cur_alpha!=alpha) || (cur_tint.r!=tint.r) || (cur_tint.g!=tint.g) || (cur_tint.b!=tint.b)) {
			cur_gl_id=bitmap_data->gl_id;
			cur_alpha=alpha;
			memmove(&cur_tint,&tint,sizeof(d3col));
			gl_texture_simple_set(cur_gl_id,TRUE,tint.r,tint.g,tint.b,cur_alpha);
		}
		
			// find the repeat count
		
		if (bitmap->repeat.on) {
			repeat_count=bitmap->repeat.count;
			if (repeat_count<1) continue;
		}
		else {
			repeat_count=1;
		}

			// scale and rotate bitmap

		rx=sx+wid;
		ry=sy+high;

		px[0]=px[3]=sx;
		px[1]=px[2]=rx;

		py[0]=py[1]=sy;
		py[2]=py[3]=ry;

		if (bitmap->rot!=0.0f) rotate_2D_polygon(4,px,py,((sx+rx)>>1),((sy+ry)>>1),bitmap->rot);

		rx=bitmap->repeat.x_add;
		ry=bitmap->repeat.y_add;
		
		wrap_count=0;

			// setup the vertex pointer

		vertex_ptr=view_bind_map_next_vertex_object((repeat_count*4)*(2+2));
		if (vertex_ptr==NULL) continue;

		uv_ptr=vertex_ptr+((repeat_count*4)*2);
		
		vp=vertex_ptr;
		uv=uv_ptr;

		for (r=0;r!=repeat_count;r++) {

				// quad

			*vp++=(float)px[0];
			*vp++=(float)py[0];
			*uv++=gx;
			*uv++=gy;

			*vp++=(float)px[3];
			*vp++=(float)py[3];
			*uv++=gx;
			*uv++=gy2;

			*vp++=(float)px[1];
			*vp++=(float)py[1];
			*uv++=gx2;
			*uv++=gy;

			*vp++=(float)px[2];
			*vp++=(float)py[2];
			*uv++=gx2;
			*uv++=gy2;
			
				// column wrapping repeats
				
			if (bitmap->repeat.col!=0) {
				px[0]+=rx;
				px[1]+=rx;
				px[2]+=rx;
				px[3]+=rx;

				wrap_count++;
				if (wrap_count==bitmap->repeat.col) {
					wrap_count=0;
					
					px[0]=px[3]=sx;
					px[1]=px[2]=sx+wid;

					py[0]+=ry;
					py[1]+=ry;
					py[2]+=ry;
					py[3]+=ry;
				}
			}
			
			else {
			
					// row wrapping repeats
					
				if (bitmap->repeat.row!=0) {
					py[0]+=ry;
					py[1]+=ry;
					py[2]+=ry;
					py[3]+=ry;

					wrap_count++;
					if (wrap_count==bitmap->repeat.row) {
						wrap_count=0;
						
						py[0]=py[1]=sy;
						py[2]=py[3]=sy+high;
						
						px[0]+=rx;
						px[1]+=rx;
						px[2]+=rx;
						px[3]+=rx;
					}
				}
				
					// regular repeats
					
				else {
					px[0]+=rx;
					px[1]+=rx;
					px[2]+=rx;
					px[3]+=rx;
					py[0]+=ry;
					py[1]+=ry;
					py[2]+=ry;
					py[3]+=ry;
				}
			}
		}
			
		view_unmap_current_vertex_object();

			// draw the quads

		glVertexPointer(2,GL_FLOAT,0,(GLvoid*)0);
		glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)(((repeat_count*4)*2)*sizeof(float)));
			
		for (r=0;r!=repeat_count;r++) {
			glDrawArrays(GL_TRIANGLE_STRIP,(r*4),4);
		}

		view_unbind_current_vertex_object();
	}
	
	gl_texture_simple_end();

		// disable vertex drawing

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

/* =======================================================

      HUD Text Specials
      
======================================================= */

void hud_texts_fps(char *data)
{
	if (view.fps.total==0.0f) {
		strcpy(data,"--.--");
	}
	else {
		if (view.fps.total>render_info.monitor_refresh_rate) {
			sprintf(data,"%d.00",render_info.monitor_refresh_rate);
		}
		else {
			sprintf(data,"%.2f",view.fps.total);
		}
	}
}

void hud_texts_score(char *data)
{
	obj_type		*obj;
	
	obj=server.obj_list.objs[server.player_obj_idx];
	sprintf(data,"%d",obj->score.score);
}

void hud_texts_place(char *data)
{
	obj_type		*obj;
	
	obj=server.obj_list.objs[server.player_obj_idx];
	sprintf(data,"%d",obj->score.place);
}

void hud_texts_spread(char *data)
{
	int				n,spread,find_place;
	bool			hit;
	obj_type		*obj,*chk_obj;
	
	obj=server.obj_list.objs[server.player_obj_idx];
	
		// if in first place, compare with second place
		// else compare with first place
		
	if (obj->score.place==1) {
		find_place=2;
	}
	else {
		find_place=1;
	}
	
	hit=FALSE;

	for (n=0;n!=max_obj_list;n++) {
		chk_obj=server.obj_list.objs[n];
		if (chk_obj==NULL) continue;

		if ((chk_obj->type!=object_type_player) && (chk_obj->type!=object_type_remote) && (chk_obj->type!=object_type_bot_multiplayer)) continue;

		if (chk_obj->score.place==find_place) {
			hit=TRUE;
			break;
		}
	}
	
		// create the spread
		
	spread=0;
	if (hit) spread=obj->score.score-chk_obj->score.score;
	
	sprintf(data,"%d",spread);
}

/* =======================================================

      Draw HUD Text
      
======================================================= */

void hud_texts_draw_return(int x,int y,int ysz,char *data,int just,d3col *col,float alpha)
{
	char			*c,*c2;
	
	c=data;
	
	while (*c!=0x0) {
		c2=strstr(c,"{r}");
		if (c2!=NULL) *c2=0x0;
		
		gl_text_draw(x,y,c,just,FALSE,col,alpha);
		
		if (c2==NULL) return;
		
		*c2='{';
		
		c=c2+3;
		y+=ysz;
	}
}

void hud_texts_draw(void)
{
	int				n,high,cur_size;
	float			alpha;
	iface_text_type	*text;

	cur_size=-1;

	for (n=0;n!=iface.text_list.ntext;n++) {

		text=&iface.text_list.texts[n];
		if (!text->show) continue;

			// time for a new text size?

		if (text->size!=cur_size) {
			cur_size=text->size;

			gl_text_start(font_hud_index,cur_size);
			high=gl_text_get_char_height(cur_size);
		}

			// fading?
	
		alpha=text->alpha;
		if (hud_item_fade_run(&text->fade,&alpha)) {
			text->show=FALSE;			// a fade has turned off bitmap
			continue;
		}

			// any special replacements

		switch (text->special) {
		
			case text_special_fps:
				hud_texts_fps(text->data);
				break;
				
			case text_special_score:
				hud_texts_score(text->data);
				break;
				
			case text_special_place:
				hud_texts_place(text->data);
				break;
				
			case text_special_spread:
				hud_texts_spread(text->data);
				break;

		}

			// draw text
			
		if (text->has_return) {
			hud_texts_draw_return(text->x,text->y,high,text->data,text->just,&text->color,alpha);
		}
		else {
			gl_text_draw(text->x,text->y,text->data,text->just,FALSE,&text->color,alpha);
		}
	}
	
	gl_text_end();		// OK to call text_end without a text_start
}

/* =======================================================

      Draw HUD Bars
      
======================================================= */

void hud_bars_draw(void)
{
	int					n,lx,rx,ty,by,wid,high;
	d3col				fill_end_color;
	iface_bar_type		*bar;
	
		// draw bars
		
	for (n=0;n!=iface.bar_list.nbar;n++) {

		bar=&iface.bar_list.bars[n];
		if (!bar->show) continue;
		
			// find size
			
		wid=bar->x_size;
		high=bar->y_size;
		
		if (!bar->vert) {
			wid=(int)(((float)wid)*bar->value);
		}
		else {
			high=(int)(((float)high)*bar->value);
		}
		
			// find the color
			
		memmove(&fill_end_color,&bar->fill_end_color,sizeof(d3col));
		fill_end_color.r+=((bar->fill_end_color.r-bar->fill_start_color.r)*bar->value);
		fill_end_color.g+=((bar->fill_end_color.g-bar->fill_start_color.g)*bar->value);
		fill_end_color.b+=((bar->fill_end_color.b-bar->fill_start_color.b)*bar->value);
		
			// draw bar
			
		lx=bar->x;
		rx=lx+wid;
		by=bar->y+bar->y_size;
		ty=by-high;
			
		if (!bar->vert) {
			view_draw_next_vertex_object_2D_color_poly(lx,by,&bar->fill_start_color,lx,ty,&bar->fill_start_color,rx,ty,&fill_end_color,rx,by,&fill_end_color,bar->fill_alpha);
		}
		else {
			view_draw_next_vertex_object_2D_color_poly(lx,ty,&fill_end_color,rx,ty,&fill_end_color,rx,by,&bar->fill_start_color,lx,by,&bar->fill_start_color,bar->fill_alpha);
		}
		
			// draw outline
			
		if (bar->outline) {
		
			lx=bar->x;
			rx=lx+bar->x_size;
			ty=bar->y;
			by=ty+bar->y_size;
			
			glLineWidth((float)setup.screen.x_sz/(float)iface.scale_x);
			view_draw_next_vertex_object_2D_line_quad(&bar->outline_color,bar->outline_alpha,lx,rx,ty,by);
			glLineWidth(1.0f);
		}
	}
}

/* =======================================================

      HUD Metrics
      
======================================================= */

void hud_metrics_draw_single(int y,char *title,char *data)
{
	int				x;
	d3col			col;

	col.r=col.g=col.b=1.0f;
	
	x=(int)(((float)iface.scale_x)*0.08f);

	gl_text_draw(x,y,title,tx_right,FALSE,&col,1.0f);
	gl_text_draw((x+5),y,data,tx_left,FALSE,&col,1.0f);
}

void hud_metrics_draw(void)
{
	int					y,high,txt_sz;
	char				str[256],str2[256];
	obj_type			*obj;

		// text sizes

	txt_sz=18;
	high=gl_text_get_char_height(txt_sz)-1;

		// start text
		
	gl_text_start(font_hud_index,txt_sz);

	y=(high+2)+8;

		// fps
		
	hud_texts_fps(str);

	hud_metrics_draw_single(y,"FPS:",str);
	y+=high;

		// latency

	if (net_setup.mode!=net_mode_client) {
		strcpy(str,"---");
	}
	else {
		if (net_setup.client.latency>=100) {
			strcat(str,"---");
		}
		else {
			sprintf(str,"%d",net_setup.client.latency);
		}
	}

	hud_metrics_draw_single(y,"Latency:",str);
	y+=high;

		// meshes and polys and mesh shadows

	sprintf(str,"%d.%d",view.count.mesh,view.count.mesh_poly);

	hud_metrics_draw_single(y,"Meshes:",str);
	y+=high;

		// liquids

	sprintf(str,"%d.%d",view.count.liquid,view.count.liquid_poly);

	hud_metrics_draw_single(y,"Liquids:",str);
	y+=high;

		// models

	sprintf(str,"%d.%d",view.count.model,view.count.model_poly);

	hud_metrics_draw_single(y,"Models:",str);
	y+=high;

		// shadows

	sprintf(str,"%d.%d",view.count.shadow,view.count.shadow_poly);

	hud_metrics_draw_single(y,"Shadows:",str);
	y+=high;

		// effects

	sprintf(str,"%d",view.count.effect);

	hud_metrics_draw_single(y,"Effects:",str);
	y+=high;

		// lights

	sprintf(str,"%d",view.render->light.count);

	hud_metrics_draw_single(y,"Lights:",str);
	y+=high;

		// object projectile hits

	obj=server.obj_list.objs[server.player_obj_idx];

	str[0]=0x0;

	if (obj->contact.obj_idx==-1) {
		strcat(str,"* ");
	}
	else {
		sprintf(str2,"%d ",obj->contact.obj_idx);
		strcat(str,str2);
	}

	if (obj->contact.proj_idx==-1) {
		strcat(str,"* ");
	}
	else {
		sprintf(str2,"%d ",obj->contact.proj_idx);
		strcat(str,str2);
	}

	hud_metrics_draw_single(y,"Hit:",str);
	y+=high;

		// map contacts

	str[0]=0x0;

	if (obj->contact.hit_poly.mesh_idx==-1) {
		strcat(str,"* ");
	}
	else {
		sprintf(str2,"%d.%d ",obj->contact.hit_poly.mesh_idx,obj->contact.hit_poly.poly_idx);
		strcat(str,str2);
	}

	if (obj->contact.stand_poly.mesh_idx==-1) {
		strcat(str,"* ");
	}
	else {
		sprintf(str2,"%d.%d ",obj->contact.stand_poly.mesh_idx,obj->contact.stand_poly.poly_idx);
		strcat(str,str2);
	}

	if (obj->contact.liquid_idx==-1) {
		strcat(str,"* ");
	}
	else {
		sprintf(str2,"%d ",obj->contact.liquid_idx);
		strcat(str,str2);
	}

	hud_metrics_draw_single(y,"Contact:",str);

		// end text

	gl_text_end();
}

/* =======================================================

      HUD Sounds
      
======================================================= */

void hud_click(void)
{
	int			buffer_idx;

	if (iface.click_sound[0]==0x0) return;
	
	buffer_idx=al_find_buffer(iface.click_sound);
	if (buffer_idx!=-1) al_play_source(buffer_idx,NULL,1,FALSE,FALSE,TRUE,FALSE);
}

/* =======================================================

      Draw HUD
      
======================================================= */

void hud_draw(void)
{
	if (setup.no_hud) return;

		// set up view
		
	gl_2D_view_interface();
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glDisable(GL_DEPTH_TEST);
	
		// draw the bitmaps, bars, and text
		
	hud_bitmaps_draw();
	hud_bars_draw();
	hud_texts_draw();
	if (setup.metrics_on) hud_metrics_draw();
	
		// reset any color changes
		
	glColor4f(0.0f,0.0f,0.0f,1.0f);
}

