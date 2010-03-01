/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Tinting

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "consoles.h"
#include "video.h"
#include "timing.h"

/* supergumba
typedef struct		{
						int						start_tick,
												fade_in_tick,life_tick,fade_out_tick;
						float					alpha;
						bool					on;
						d3col					col;
					} hud_tint_type;
*/
int							tint_start_tick;

extern map_type				map;
extern setup_type			setup;

/* =======================================================

      Liquid Tint Drawing
      
======================================================= */

void view_draw_liquid_tint(int liquid_idx)
{
	map_liquid_type		*liq;
	
		// under liquid
		
	if (liquid_idx==-1) return;
	
		// get the liquid
		
	liq=&map.liquid.liquids[liquid_idx];
	
		// draw tint

	gl_2D_view_screen();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	glColor4f(liq->col.r,liq->col.g,liq->col.b,liq->tint_alpha);
	view_draw_next_vertex_object_2D_tint_screen();
}

/* =======================================================

      View Tint Drawing
      
======================================================= */

void view_draw_tint_start(void)
{

}

void view_draw_effect_tint(obj_type *obj)
{
	int				tick;
	float			alpha;
	obj_fs_tint		*tint;
	
	tint=&obj->fs_effect.tint;
	
		// is flash on?
		
	if (!tint->on) return;
	
		// get color and alpha
		
	tick=game_time_get()-tint->start_tick;
	
	if (tick>(tint->fade_in_tick+tint->life_tick)) {
		alpha=1.0f-((float)(tick-(tint->fade_in_tick+tint->life_tick))/(float)tint->fade_out_tick);
	}
	else {
		if (tick>tint->fade_in_tick) {
			alpha=1.0f;
		}
		else {
			alpha=(float)tick/(float)tint->fade_in_tick;
		}
	}
	
		// draw tint
		
	gl_2D_view_screen();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	glColor4f(tint->col.r,tint->col.g,tint->col.b,(alpha*tint->alpha));
	view_draw_next_vertex_object_2D_tint_screen();
}

