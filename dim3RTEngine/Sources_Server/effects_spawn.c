/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Effect Spawing

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

extern map_type				map;
extern server_type			server;

/* =======================================================

      Flash Effects
      
======================================================= */

bool effect_spawn_flash(d3pnt *pt,d3col *col,int intensity,float exponent,int flash_msec,int fade_msec)
{
	int					effect_idx;
	effect_type			*effect;
	flash_effect_data	*flash;
	
		// spawn the flash
		
	effect_idx=effect_spawn(ef_flash,pt,(flash_msec+fade_msec));
	if (effect_idx==-1) return(FALSE);
	
	effect=server.effect_list.effects[effect_idx];

	flash=&effect->data.flash;
	memmove(&flash->col,col,sizeof(d3col));
	flash->intensity=intensity;
	flash->exponent=exponent;
	flash->mid_tick=flash_msec;
	
	effect->size=0;
	
	return(TRUE);
}

/* =======================================================

      Lightning Effects
      
======================================================= */

bool effect_spawn_lightning(d3pnt *start_pt,d3pnt *end_pt,int wid,float varient,d3col *col,int life_msec)
{
	int						effect_idx;
	effect_type				*effect;
	lightning_effect_data	*lightning;

	effect_idx=effect_spawn(ef_lightning,start_pt,life_msec);
	if (effect_idx==-1) return(FALSE);
	
	effect=server.effect_list.effects[effect_idx];
	
	lightning=&effect->data.lightning;
	
	memmove(&lightning->start_pnt,start_pt,sizeof(d3pnt));
	memmove(&lightning->end_pnt,end_pt,sizeof(d3pnt));
	
	lightning->wid=wid;
	lightning->line_offset=game_time_get()&0xF;
	lightning->varient=varient;
	
	memmove(&lightning->col,col,sizeof(d3col));
	
	effect->size=(abs(end_pt->x-start_pt->x)+abs(end_pt->z-start_pt->z))>>2;

	return(TRUE);
}

/* =======================================================

      Ray Effects
      
======================================================= */

bool effect_spawn_ray(d3pnt *start_pt,d3pnt *end_pt,int wid,d3col *col,int life_msec)
{
	int						effect_idx;
	effect_type				*effect;
	ray_effect_data			*ray;

	effect_idx=effect_spawn(ef_ray,start_pt,life_msec);
	if (effect_idx==-1) return(FALSE);
	
	effect=server.effect_list.effects[effect_idx];
	
	ray=&effect->data.ray;
	
	memmove(&ray->start_pnt,start_pt,sizeof(d3pnt));
	memmove(&ray->end_pnt,end_pt,sizeof(d3pnt));
	
	ray->wid=wid;
	
	memmove(&ray->col,col,sizeof(d3col));
	
	effect->size=(abs(end_pt->x-start_pt->x)+abs(end_pt->z-start_pt->z))>>2;

	return(TRUE);
}

/* =======================================================

      Shake Effects
      
======================================================= */

bool effect_spawn_shake(d3pnt *pt,int distance,int size,int life_msec)
{
	int					effect_idx;
	effect_type			*effect;

	effect_idx=effect_spawn(ef_shake,pt,life_msec);
	if (effect_idx==-1) return(FALSE);
	
	effect=server.effect_list.effects[effect_idx];
	
	effect->data.shake.distance=distance;
	effect->size=size;
	
	return(TRUE);
}

