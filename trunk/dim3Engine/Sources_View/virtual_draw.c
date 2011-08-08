/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Virtual Control Draw Routines

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

      Draw Virtual Sticks
      
======================================================= */

void virtual_stick_draw(iface_virtual_stick_type *stick)
{
	int					x,y,sx,sy,bx,by,lft,rgt,top,bot,
						radius;
	float				f_outer_radius;
	d3vct				vct;
	bitmap_type			*bitmap;

	if (!stick->use_bitmap) return;
	
		// outer stick

	rgt=stick->x+stick->x_size;
	bot=stick->y+stick->y_size;
	
	bitmap=view_images_get_bitmap(stick->outer_image_idx);
	view_draw_next_vertex_object_2D_texture_quad(bitmap->gl_id,&stick->color,1.0f,stick->x,rgt,stick->y,bot,0.0f,1.0f,0.0f,1.0f);

		// inner stick location
		
	sx=stick->x_size>>1;
	sy=stick->y_size>>1;
	
	x=(int)(((float)sx)*stick->touch_x);
	y=(int)(((float)sy)*stick->touch_y);
	
		// make sure it's within circle
		
	radius=(int)sqrt((double)(x*x)+(double)(y*y));
	if (radius>stick->outer_radius) {
	
		vct.x=(float)x;
		vct.y=(float)y;
		vct.z=0.0f;
		vector_normalize(&vct);
		
		f_outer_radius=(float)stick->outer_radius;
		x=(int)(vct.x*f_outer_radius);
		y=(int)(vct.y*f_outer_radius);
	
	}

	bx=stick->x_size>>2;
	by=stick->y_size>>2;

	lft=(stick->x+sx)+x;
	lft-=(bx>>1);
	rgt=lft+bx;
	
	top=(stick->y+sy)+y;
	top-=(by>>1);
	bot=top+by;
	
	bitmap=view_images_get_bitmap(stick->inner_image_idx);
	view_draw_next_vertex_object_2D_texture_quad(bitmap->gl_id,&stick->color,1.0f,lft,rgt,top,bot,0.0f,1.0f,0.0f,1.0f);
}

/* =======================================================

      Draw Virtual Buttons
      
======================================================= */

void virtual_button_draw(iface_virtual_button_type *button)
{
	int					rgt,bot;
	bitmap_type			*bitmap;

	if (!button->use_bitmap) return;
	
	rgt=button->x+button->x_size;
	bot=button->y+button->y_size;
	bitmap=view_images_get_bitmap(button->up_image_idx);
	
	view_draw_next_vertex_object_2D_texture_quad(bitmap->gl_id,&button->color,1.0f,button->x,rgt,button->y,bot,0.0f,1.0f,0.0f,1.0f);
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
	
	for (n=0;n!=max_virtual_stick;n++) {
		if (iface.virtual_control.sticks[n].on) virtual_stick_draw(&iface.virtual_control.sticks[n]);
	}
	
	for (n=0;n!=max_virtual_button;n++) {
		if (iface.virtual_control.buttons[n].on) virtual_button_draw(&iface.virtual_control.buttons[n]);
	}
	
		// reset any color changes
		
	glColor4f(0.0f,0.0f,0.0f,1.0f);
}

