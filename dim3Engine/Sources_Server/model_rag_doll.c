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

      Model Rag Doll Clear Bones
      
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

/* =======================================================

      Model Rag Doll Constrain Bones
      
======================================================= */

void model_rag_doll_constrain_bone(model_draw_rag_bone_type *rag_bone)
{
	if (rag_bone->rot_add.x<-90.0f) rag_bone->rot_add.x=-90.0f;		// supergumba -- temporary
	if (rag_bone->rot_add.x>90.0f) rag_bone->rot_add.x=90.0f;

	if (rag_bone->rot_add.y<-90.0f) rag_bone->rot_add.y=-90.0f;
	if (rag_bone->rot_add.y>90.0f) rag_bone->rot_add.y=90.0f;

	if (rag_bone->rot_add.z<-90.0f) rag_bone->rot_add.z=-90.0f;
	if (rag_bone->rot_add.z>90.0f) rag_bone->rot_add.z=90.0f;
}

/* =======================================================

      Model Rag Doll Random Bones
      
======================================================= */

void model_rag_doll_random_bones(int tick,model_draw *draw)
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

		model_rag_doll_constrain_bone(rag_bone);

		rag_bone++;
	}
}

/* =======================================================

      Model Rag Doll Force Bones
      
======================================================= */

float model_rag_doll_2D_bone_direction(int pivot_x,int pivot_y,int bone_x,int bone_y,int force_x,int force_y)
{
	float		a1,a2,f;
	float		vx,vy,v2x,v2y;
	bool		cwise;

		// get the angle between the bone
		// and the 0,-1 straight up vector

	vx=(float)(bone_x-pivot_x);
	vy=(float)(bone_y-pivot_y);

	f=sqrtf((vx*vx)+(vy*vy));
	if (f!=0.0f) {
		f=1.0f/f;
		vx*=f;
		vy*=f;
	}

	a1=acosf((vx*0.0f)+(vy*-1.0f))*RAD_to_ANG;

		// transpose the force vector
		// to the pivot bone, and then
		// get the angle between it and the
		// bone

	v2x=(float)((bone_x-force_x)-pivot_x);
	v2y=(float)((bone_y-force_y)-pivot_y);

	f=sqrtf((v2x*v2x)+(v2y*v2y));
	if (f!=0.0f) {
		f=1.0f/f;
		v2x*=f;
		v2y*=f;
	}

	a2=acosf((vx*v2x)+(vy*v2y))*RAD_to_ANG;

		// get the sign for the direction

	angle_dif(a1,a2,&cwise);
	return(cwise?1.0f:-1.0f);
}

void model_rag_doll_push_bones(int tick,model_draw *draw)
{
	int							n,nbone;
	float						fct;
	d3pnt						bone_pnt[max_model_bone];
	d3ang						rot;
	model_type					*model;
	model_draw_bone_type		*draw_bone;
	model_draw_rag_doll			*rag_doll;
	model_draw_rag_bone_type	*rag_bone;

	rag_doll=&draw->rag_doll;

		// get push factor

	fct=1.0f-(((float)tick)/((float)rag_doll->tick));

		// get the model

	if (draw->model_idx==-1) return;
	model=server.model_list.models[draw->model_idx];

		// precalc all the bone positions

	nbone=model->nbone;

	for (n=0;n!=nbone;n++) {
		model_get_draw_bone_position(&draw->setup,n,&bone_pnt[n]);
		bone_pnt[n].x+=draw->pnt.x;
		bone_pnt[n].y+=draw->pnt.y;
		bone_pnt[n].z+=draw->pnt.z;
	}

		// push the bones

	for (n=0;n!=nbone;n++) {

			// can't rotate if no parents

		draw_bone=&draw->setup.bones[n];
		if (draw_bone->parent_idx==-1) continue;
		
			// for force hit

		rot.x=0.5f*model_rag_doll_2D_bone_direction(bone_pnt[draw_bone->parent_idx].y,bone_pnt[draw_bone->parent_idx].z,bone_pnt[n].y,bone_pnt[n].z,rag_doll->force_pnt.y,rag_doll->force_pnt.z);
		rot.y=0.5f*model_rag_doll_2D_bone_direction(bone_pnt[draw_bone->parent_idx].x,bone_pnt[draw_bone->parent_idx].z,bone_pnt[n].x,bone_pnt[n].z,rag_doll->force_pnt.x,rag_doll->force_pnt.z);
		rot.z=0.5f*model_rag_doll_2D_bone_direction(bone_pnt[draw_bone->parent_idx].x,bone_pnt[draw_bone->parent_idx].y,bone_pnt[n].x,bone_pnt[n].y,rag_doll->force_pnt.x,rag_doll->force_pnt.y);
		
			// for gravity

		rot.x=0.3f*model_rag_doll_2D_bone_direction(bone_pnt[draw_bone->parent_idx].y,bone_pnt[draw_bone->parent_idx].z,bone_pnt[n].y,bone_pnt[n].z,draw->pnt.y,draw->pnt.z);
		rot.y=0.3f*model_rag_doll_2D_bone_direction(bone_pnt[draw_bone->parent_idx].x,bone_pnt[draw_bone->parent_idx].z,bone_pnt[n].x,bone_pnt[n].z,draw->pnt.x,draw->pnt.z);
		rot.z=0.3f*model_rag_doll_2D_bone_direction(bone_pnt[draw_bone->parent_idx].x,bone_pnt[draw_bone->parent_idx].y,bone_pnt[n].x,bone_pnt[n].y,draw->pnt.x,draw->pnt.y);

			// get parent rag bone
			// for setting rotation

		rag_bone=&draw->setup.rag_bones[draw_bone->parent_idx];

		rag_bone->rot_add.x+=rot.x;
		rag_bone->rot_add.y+=rot.y;
		rag_bone->rot_add.z+=rot.z;

		model_rag_doll_constrain_bone(rag_bone);
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

void model_rag_doll_start(model_draw *draw,d3pnt *force_pnt,int force,int force_msec,bool rag_random)
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

	rag_doll->rag_random=rag_random;

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

	if (rag_doll->rag_random) {
		model_rag_doll_random_bones(tick,draw);
	}
	else {
		model_rag_doll_push_bones(tick,draw);
	}
}

