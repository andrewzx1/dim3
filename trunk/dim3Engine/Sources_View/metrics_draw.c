/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: HUD Metric Routines

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
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

      Metrics
      
======================================================= */

void metrics_draw_single(int y,char *title,char *data)
{
	int				x;

	x=(int)(((float)iface.scale_x)*0.08f);

	gl_text_draw(x,y,title,tx_right,FALSE,&iface.color.system.metric,1.0f);
	gl_text_draw((x+5),y,data,tx_left,FALSE,&iface.color.system.metric,1.0f);
}

void metrics_draw(void)
{
	int					y,wid,high,count;
	char				str[256],str2[256];
	d3col				col;
	obj_type			*obj;
	
	if ((setup.no_hud) || (!setup.metrics_on)) return;

		// set up view
		
	gl_2D_view_interface();
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glDisable(GL_DEPTH_TEST);

		// text height
		
	high=gl_text_get_char_height(metrics_txt_sz)-1;
	y=(high+2)+8;
	
		// background
		
	wid=(int)(((float)iface.scale_x)*0.18f);
	
	count=9;
	if (net_setup.mode==net_mode_client) count++;
	if (map.optimize.ray_trace_obscure) count++;
	
	col.r=col.g=col.b=0.0f;
	view_primitive_2D_color_quad(&col,0.5f,2,wid,2,((count*high)+(high/2)));

		// fps
		
	gl_text_start(font_hud_index,metrics_txt_sz);
		
	hud_texts_fps(str);

	metrics_draw_single(y,"FPS:",str);
	y+=high;

		// latency

	if (net_setup.mode==net_mode_client) {
		if (net_setup.client.latency>=100) {
			strcat(str,"---");
		}
		else {
			sprintf(str,"%d",net_setup.client.latency);
		}

		metrics_draw_single(y,"Latency:",str);
		y+=high;
	}

		// meshes and polys

	sprintf(str,"%d.%d",view.count.mesh,view.count.mesh_poly);

	metrics_draw_single(y,"Meshes:",str);
	y+=high;

		// liquids

	sprintf(str,"%d.%d",view.count.liquid,view.count.liquid_poly);

	metrics_draw_single(y,"Liquids:",str);
	y+=high;

		// models

	sprintf(str,"%d.%d",view.count.model,view.count.model_poly);

	metrics_draw_single(y,"Models:",str);
	y+=high;

		// shadows

	sprintf(str,"%d.%d",view.count.shadow,view.count.shadow_poly);

	metrics_draw_single(y,"Shadows:",str);
	y+=high;

		// effects

	sprintf(str,"%d",view.count.effect);

	metrics_draw_single(y,"Effects:",str);
	y+=high;

		// lights

	sprintf(str,"%d",view.render->light.count);

	metrics_draw_single(y,"Lights:",str);
	y+=high;

		// obscure

	if (map.optimize.ray_trace_obscure) {
		sprintf(str,"%d%%",view.count.obscure_percent);

		metrics_draw_single(y,"Obscure:",str);
		y+=high;
	}

		// object projectile hits

	obj=server.obj_list.objs[server.player_obj_idx];

	str[0]=0x0;

	if (obj->contact.obj_idx==-1) {
		strcat(str,"- ");
	}
	else {
		sprintf(str2,"%d ",obj->contact.obj_idx);
		strcat(str,str2);
	}

	if (obj->contact.proj_idx==-1) {
		strcat(str,"- ");
	}
	else {
		sprintf(str2,"%d ",obj->contact.proj_idx);
		strcat(str,str2);
	}

	metrics_draw_single(y,"Hit:",str);
	y+=high;

		// map contacts

	str[0]=0x0;

	if (obj->contact.hit_poly.mesh_idx==-1) {
		strcat(str,"- ");
	}
	else {
		sprintf(str2,"%d.%d ",obj->contact.hit_poly.mesh_idx,obj->contact.hit_poly.poly_idx);
		strcat(str,str2);
	}

	if (obj->contact.stand_poly.mesh_idx==-1) {
		strcat(str,"- ");
	}
	else {
		sprintf(str2,"%d.%d ",obj->contact.stand_poly.mesh_idx,obj->contact.stand_poly.poly_idx);
		strcat(str,str2);
	}

	if (obj->contact.liquid_idx==-1) {
		strcat(str,"- ");
	}
	else {
		sprintf(str2,"%d ",obj->contact.liquid_idx);
		strcat(str,str2);
	}

	metrics_draw_single(y,"Contact:",str);
	
	gl_text_end();
	
		// reset any color changes
		
	glColor4f(0.0f,0.0f,0.0f,1.0f);

}

