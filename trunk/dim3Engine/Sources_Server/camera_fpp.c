/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: FPP Camera

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "objects.h"
#include "cameras.h"

extern server_type			server;
extern camera_type			camera;
extern setup_type			setup;

/* =======================================================

      FPP Camera Position
      
======================================================= */

void camera_fpp_get_position(d3pnt *pnt,d3ang *ang)
{
	obj_type		*obj;

	obj=server.obj_list.objs[camera.obj_idx];
	
    pnt->x=obj->pnt.x;
    pnt->y=(obj->pnt.y+obj->duck.y_move+obj->liquid.bob_y_move)+obj->size.eye_offset;
    pnt->z=obj->pnt.z;

	ang->x=camera.setup.ang.x-obj->view_ang.x;
	ang->y=angle_add(obj->ang.y,camera.setup.ang.y);
    ang->z=camera.setup.ang.z+obj->view_ang.z;
}

