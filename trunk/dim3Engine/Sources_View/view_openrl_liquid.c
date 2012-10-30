/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View OpenRL Map Liquid

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

#ifdef D3_OPENRL

#include "ray_interface.h"

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

extern int					view_rl_scene_id;

/* =======================================================

      OpenRL Liquid Mesh Setup
      
======================================================= */

void view_openrl_map_liquid_mesh_start(void)
{
	int					n;
	map_liquid_type		*liq;
			
		// build the meshes

	for (n=0;n!=map.liquid.nliquid;n++) {
		liq=&map.liquid.liquids[n];
	}
}

void view_openrl_map_liquid_mesh_stop(void)
{
}

/* =======================================================

      OpenRL Map Liquid Mesh Update
      
======================================================= */

void view_openrl_map_liquid_mesh_update(void)
{
}

#endif