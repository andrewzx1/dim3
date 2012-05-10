/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Collide Setup

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
#include "scripts.h"
#include "objects.h"

extern map_type			map;
extern view_type		view;
extern server_type		server;

int						collide_obj_count,collide_proj_count,
						collide_obj_list[max_obj_list],
						collide_proj_list[max_proj_list];

/* =======================================================

      Build List of Collidable Objects and Projectiles
      
======================================================= */

void collide_setup(void)
{
	int				n;
	obj_type		*obj;
	proj_type		*proj;

		// build object list

	collide_obj_count=0;

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (!obj->hidden) collide_obj_list[collide_obj_count++]=n;
	}

		// build projectile list

	collide_proj_count=0;
	
	for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (proj->on) collide_proj_list[collide_proj_count++]=n;
	}
}

