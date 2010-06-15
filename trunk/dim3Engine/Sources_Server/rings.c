/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Ring Routines

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

#include "objects.h"
#include "effects.h"
#include "cameras.h"

extern map_type				map;
extern server_type			server;
extern setup_type			setup;

/* =======================================================

      Initialize Rings
      
======================================================= */

void ring_initialize_list(void)
{
	int				n;

	for (n=0;n!=max_ring_list;n++) {
		server.ring_list.rings[n]=NULL;
	}
}

void ring_free_list(void)
{
	int				n;

	for (n=0;n!=max_ring_list;n++) {
		if (server.ring_list.rings[n]!=NULL) free(server.ring_list.rings[n]);
	}
}

ring_type* ring_add_list(void)
{
	int				n;

	for (n=0;n!=max_ring_list;n++) {
		if (server.ring_list.rings[n]==NULL) {
			server.ring_list.rings[n]=(ring_type*)malloc(sizeof(ring_type));
			return(server.ring_list.rings[n]);
		}
	}

	return(NULL);
}

/* =======================================================

      Find Ring by Name
      
======================================================= */

ring_type* ring_find(char *name)
{
	int			n;
	ring_type	*ring;
	
	for (n=0;n!=max_ring_list;n++) {
		ring=server.ring_list.rings[n];
		if (ring==NULL) continue;

		if (strcasecmp(ring->name,name)==0) return(ring);
	}
	
	return(NULL);
}

int ring_find_index(char *name)
{
	int			n;
	ring_type	*ring;
	
	for (n=0;n!=max_ring_list;n++) {
		ring=server.ring_list.rings[n];
		if (ring==NULL) continue;

		if (strcasecmp(ring->name,name)==0) return(n);
	}
	
	return(-1);
}

/* =======================================================

      Find Total Size of Ring Effect
      
======================================================= */

int ring_get_effect_size(ring_type *ring)
{
	int			x,y,z;

	x=ring->end_outer_size+(int)ring->vct.x;
	y=ring->end_outer_size+(int)ring->vct.y;
	z=ring->end_outer_size+(int)ring->vct.z;

	return(distance_get(0,0,0,x,y,z));
}

/* =======================================================

      Spawn Rings
      
======================================================= */

bool ring_spawn(int ring_idx,int obj_uid,d3pnt *pt,d3ang *ang)
{
	obj_type				*obj;
	effect_type				*effect;
	ring_effect_data		*eff_ring;
	ring_type				*ring;
	
	ring=server.ring_list.rings[ring_idx];
		
		// create ring

	effect=effect_spawn(ef_ring,pt,ring->life_msec);
	if (effect==NULL) return(FALSE);
	
	eff_ring=&effect->data.ring;
	eff_ring->ring_idx=ring_idx;
	
		// ring angle

	if (ang==NULL) {
		memmove(&eff_ring->ang,&ring->ang,sizeof(d3ang));
	}
	else {
		eff_ring->ang.x=ring->ang.x+ang->x;
		eff_ring->ang.y=angle_add(ring->ang.y,ang->y);
		eff_ring->ang.z=ring->ang.z+ang->z;
	}

		// setup size

	effect->size=ring_get_effect_size(ring);

		// any tinting

	eff_ring->tint.r=eff_ring->tint.g=eff_ring->tint.b=1.0f;

	if ((ring->team_tint) && (obj_uid!=-1)) {
		obj=server.obj_list.objs[obj_uid];
		if (obj!=NULL) object_get_tint(obj,&eff_ring->tint);
	}
	
	return(TRUE);
}

bool ring_line_spawn(int ring_idx,int obj_uid,d3pnt *start_pt,d3pnt *end_pt,int count)
{
	int				n,dx,dz,dy;
	d3pnt			pt;
	d3ang			ang;

		// need more than 1 ring to do effect
		
	if (count<=1) {
		ring_spawn(ring_idx,obj_uid,start_pt,NULL);
		return(TRUE);
	}
	
		// get angle between lines
		// right now only do y rotations as two
		// many rotations can cause math errors

	ang.x=0.0f;
	ang.y=angle_find(start_pt->x,start_pt->z,end_pt->x,end_pt->z);
	ang.z=0.0f;

		// create line of rings
		// we want first ring and last ring
		// to be at end of lines so divide over
		// count-1 instead of count

	dx=end_pt->x-start_pt->x;
	dy=end_pt->y-start_pt->y;
	dz=end_pt->z-start_pt->z;
	
	count--;

	for (n=0;n!=count;n++) {
		
		pt.x=start_pt->x+((dx*n)/count);
		pt.y=start_pt->y+((dy*n)/count);
		pt.z=start_pt->z+((dz*n)/count);

		if (!ring_spawn(ring_idx,obj_uid,&pt,&ang)) return(FALSE);
	}

	return(TRUE);
}
