/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Model Rag Dolls

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

extern server_type				server;
extern map_type					map;

/* =======================================================

      Model Fades
      
======================================================= */

void model_rag_doll_clear(model_draw *draw)
{
	draw->rag_doll.on=FALSE;
}

void model_rag_doll_start(model_draw *draw,d3pnt *force_pnt,int force,int force_msec)
{
	model_draw_rag_doll		*rag_doll;

	rag_doll=&draw->rag_doll;

	rag_doll->tick=force_msec;
	rag_doll->start_tick=game_time_get();

	rag_doll->force=force;
	memmove(&rag_doll->force_pnt,force_pnt,sizeof(d3pnt));

	rag_doll->on=TRUE;
}

void model_rag_doll_run(model_draw *draw)
{
	int						tick;
	model_draw_rag_doll		*rag_doll;

	rag_doll=&draw->rag_doll;
	if (!rag_doll->on) return;

	tick=game_time_get();

		// time up?

	tick-=rag_doll->start_tick;
	if (tick>rag_doll->tick) {
		rag_doll->on=FALSE;
		return;
	}

		// run rag dolls

//	fct=((float)tick)/(float)fade->tick;
//	draw->alpha=fade->start_alpha+((fade->end_alpha-fade->start_alpha)*fct);
}

