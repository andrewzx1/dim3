/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Virtual Control Draw Routines

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

      Draw HUD Bitmaps
      
======================================================= */

/*
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
*/

/* =======================================================

      Initialize and Shutdown Virtual Controls
      
======================================================= */

/* =======================================================

      Draw Virtual Sticks and Buttons
      
======================================================= */

void virtual_stick_draw(iface_virtual_stick_type *stick)
{

}

void virtual_button_draw(iface_virtual_button_type *button)
{

}

/* =======================================================

      Draw Virtual Controls
      
======================================================= */

void virtual_control_draw(void)
{
	int				n;
	
		// set up view
		
	gl_2D_view_interface();
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glDisable(GL_DEPTH_TEST);
	
		// draw the virtual controls
		
		// supergumba -- hard set them!
		
	iface.virtual_control.sticks[0].on=TRUE;
	iface.virtual_control.sticks[0].x=10;
	iface.virtual_control.sticks[0].y=550;
	iface.virtual_control.sticks[0].x_size=200;
	iface.virtual_control.sticks[0].y_size=200;
		
	iface.virtual_control.sticks[1].on=TRUE;
	iface.virtual_control.sticks[1].x=790;
	iface.virtual_control.sticks[1].y=550;
	iface.virtual_control.sticks[1].x_size=200;
	iface.virtual_control.sticks[1].y_size=200;
	
	for (n=0;n!=max_virtual_stick;n++) {
		if (iface.virtual_control.sticks[n].on) virtual_stick_draw(&iface.virtual_control.sticks[n]);
	}
	
	for (n=0;n!=max_virtual_button;n++) {
		if (iface.virtual_control.buttons[n].on) virtual_button_draw(&iface.virtual_control.buttons[n]);
	}
	
		// reset any color changes
		
	glColor4f(0.0f,0.0f,0.0f,1.0f);
}

