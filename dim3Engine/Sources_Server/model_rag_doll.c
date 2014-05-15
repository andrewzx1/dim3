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
#include "objects.h"

extern server_type				server;
extern map_type					map;

/* =======================================================

      Model Rag Doll Bones
      
======================================================= */

void model_rag_doll_clear_bones(model_draw *draw)
{
	int							n,nbone;
	model_type					*model;
	model_draw_rag_bone_type	*rag_bone;

	if (draw->model_idx==-1) return;
	model=server.model_list.models[draw->model_idx];

	nbone=model->nbone;
	rag_bone=draw->setup.rag_bones;
	
	for (n=0;n!=nbone;n++) {
		rag_bone->rot_add.x=0.0f;
		rag_bone->rot_add.y=0.0f;
		rag_bone->rot_add.z=0.0f;
		rag_bone++;
	}
}

void model_rag_doll_push_bones(int tick,model_draw *draw)
{
	int							n,nbone;
	float						fct;
	model_type					*model;
	model_draw_rag_doll			*rag_doll;
	model_draw_rag_bone_type	*rag_bone;

	rag_doll=&draw->rag_doll;

		// get push factor

	fct=1.0f-(((float)tick)/((float)rag_doll->tick));

		// get the model

	if (draw->model_idx==-1) return;
	model=server.model_list.models[draw->model_idx];

		// push the bones

	nbone=model->nbone;
	rag_bone=draw->setup.rag_bones;

	for (n=0;n!=nbone;n++) {

		switch (n%3) {
			case 0:
				rag_bone->rot_add.x+=0.5f;
				break;
			case 1:
				rag_bone->rot_add.y+=0.5f;
				break;
			case 2:
				rag_bone->rot_add.z+=0.5f;
				break;
		}

		rag_bone++;
	}
}

/* =======================================================

      Model Rag Doll Start/Stop
      
======================================================= */

void model_rag_doll_clear(model_draw *draw)
{
		// only reset bones if we were
		// in a rag doll

	if (draw->rag_doll.on) model_rag_doll_clear_bones(draw);

	draw->rag_doll.on=FALSE;
}

void model_rag_doll_start(model_draw *draw,d3pnt *force_pnt,int force,int force_msec)
{
	model_draw_rag_doll		*rag_doll;

		// stop all current animation

	model_stop_animation(draw);

		// start the rag doll

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

	model_rag_doll_push_bones(tick,draw);
}

