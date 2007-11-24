/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Lighting Routines

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

extern map_type			map;
extern server_type		server;
extern setup_type		setup;

int						nlight;
bool					hilite_on=FALSE;
light_spot_type			lspot_cache[max_light_spot];

double					light_flicker_value[64]={
													1.00,1.00,1.00,1.00,1.00,1.00,1.00,1.00,
													0.92,0.85,0.70,0.85,0.92,1.00,0.92,0.85,
													0.92,1.00,1.00,1.00,1.00,0.92,0.85,0.70,
													0.50,0.70,0.85,0.70,0.85,0.92,1.00,1.00,
													1.00,1.00,1.00,1.00,0.92,0.85,0.70,0.85,
													0.70,0.85,0.92,1.00,1.00,0.92,0.85,0.70,
													0.50,0.70,0.85,0.92,1.00,1.00,0.92,1.00,
													0.92,0.85,0.70,0.85,0.92,1.00,1.00,1.00
												},
						light_fail_value[64]=	{
													0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
													0.00,0.15,0.45,0.15,0.00,0.00,0.00,0.15,
													0.00,0.00,0.00,0.00,0.00,0.00,0.15,0.45,
													0.85,0.45,0.15,0.45,0.15,0.00,0.00,0.00,
													0.00,0.00,0.00,0.00,0.00,0.15,0.45,0.15,
													0.45,0.15,0.00,0.00,0.00,0.00,0.15,0.45,
													0.85,0.45,0.15,0.00,0.00,0.00,0.00,0.00,
													0.00,0.15,0.45,0.15,0.00,0.00,0.00,0.00
												};

extern int game_time_get(void);
extern void light_trace_set_base_color(d3col *col);

/* =======================================================

      Clear Lights
      
======================================================= */

void light_get_base_color(d3col *col)
{
	if (!hilite_on) {							// F2 hilite debugging
		col->r=map.ambient.light_color.r+setup.gamma;
		col->g=map.ambient.light_color.g+setup.gamma;
		col->b=map.ambient.light_color.b+setup.gamma;
	}
	else {
		col->r=col->g=col->b=1;
	}
}

void light_clear(void)
{
	int					i;
	d3col				light_base;
	portal_type			*portal;
	
		// no lights in map
		
	nlight=0;
	
		// clear portal light lists
		
	portal=map.portals;
	
	for ((i=0);(i!=map.nportal);i++) {
		map_portal_clear_lights(portal);
		portal++;
	}

		// base light
		
	light_get_base_color(&light_base);
	map_set_light_base(&light_base);
	light_trace_set_base_color(&light_base);
}

/* =======================================================

      Get Light Intensity For Type
      
======================================================= */

double light_get_intensity(int light_type,int intensity)
{
	int				tick;
	double			d_tick,d_intensity;
	
	tick=game_time_get();
	d_intensity=(double)intensity;
	
	switch (light_type) {
	
		case lt_normal:
			return(d_intensity);
	
		case lt_blink:
			if ((tick&0x100)==0) return(d_intensity);
			return(0);
			
		case lt_glow:
			d_tick=(double)(tick&0x7FF);
			if (d_tick>1024) d_tick=2048-d_tick;
			return(d_intensity*(0.75+(d_tick/4096)));
			
		case lt_pulse:
			d_tick=(double)(tick&0x3FF);
			if (d_tick>512) d_tick=1024-d_tick;
			return(d_intensity*(0.5+(d_tick/1024)));
			
		case lt_flicker:
			d_intensity*=light_flicker_value[(tick>>7)&0x3F];
			break;
			
		case lt_failing:
			d_intensity*=light_fail_value[(tick>>7)&0x3F];
			break;
	}
	
	return(d_intensity);
}

/* =======================================================

      Add a Light to the Light List
      
======================================================= */

void light_add(d3pos *pos,int light_type,int intensity,bool confine_to_portal,d3col *col)
{
	int						i;
	portal_type				*portal;
	light_spot_type			*lspot;
	
	if (nlight==max_light_spot) return;

	lspot=&lspot_cache[nlight];
	
		// create intensity for light type
		
	lspot->intensity=light_get_intensity(light_type,intensity);
	if (lspot->intensity<=0) return;
	
		// create light position and color
		
	lspot->pnt.x=pos->x;
	lspot->pnt.z=pos->z;
	lspot->pnt.y=pos->y;
	memmove(&lspot->col,col,sizeof(d3col));
	
		// confined light
		
	if (confine_to_portal) {
		portal=&map.portals[pos->rn];
		if (portal->in_path) map_portal_add_light(&map,portal,lspot);
	}
	
		// light against all hit portals
		
	else {
		portal=map.portals;
		
		for (i=0;i!=map.nportal;i++) {
			if (portal->in_path) map_portal_add_light(&map,portal,lspot);
			portal++;
		}
	}
	
		// count the lights
		
	nlight++;
}

/* =======================================================

      Create GLSL Light Arrays
      
======================================================= */

int light_create_glsl_array(float *light_pos,float *light_col)
{
	int						n;
	float					*pos,*col;
	light_spot_type			*lspot;


		// clear all lights

	memset(light_pos,0x0,((sizeof(float)*3)*max_light_spot));
	memset(light_col,0x0,((sizeof(float)*4)*max_light_spot));

		// set the lights

	lspot=lspot_cache;
	pos=light_pos;
	col=light_col;

	for (n=0;n!=nlight;n++) {
		*pos=(float)lspot->pnt.x;
		pos++;
		*pos=(float)lspot->pnt.y;
		pos++;
		*pos=(float)lspot->pnt.z;
		pos++;

		*col=lspot->col.r;
		col++;
		*col=lspot->col.g;
		col++;
		*col=lspot->col.b;
		col++;
		*col=(float)lspot->intensity;
		col++;
	}

	return(nlight);
}