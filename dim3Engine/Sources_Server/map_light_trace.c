/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Light Ray Tracing

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

#include "objects.h"

extern map_type				map;
extern server_type			server;

extern float ray_trace_mesh_polygon(d3pnt *spt,d3vct *vct,d3pnt *hpt,map_mesh_type *mesh,map_mesh_poly_type *mesh_poly);

/* =======================================================

      Light Trace Map
      
======================================================= */

bool light_trace_map(d3pnt *spt,d3pnt *ept)
{
	int					n,k;
	float				t;
	d3pnt				pt;
	d3vct				vct;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	
		// get vector
		
	vct.x=(float)(ept->x-spt->x);
	vct.y=(float)(ept->y-spt->y);
	vct.z=(float)(ept->z-spt->z);
	
		// move start point ahead a bit
		
	spt->x+=(int)(vct.x*0.01);
	spt->y+=(int)(vct.y*0.01);
	spt->z+=(int)(vct.z*0.01);
	
		// check against map

	pt.x=pt.y=pt.z=0;

		// check the meshes

	for (n=0;n!=map.mesh.nmesh;n++) {

		mesh=&map.mesh.meshes[n];

			// rough bounds check

		if ((spt->y<mesh->box.min.y) && (ept->y<mesh->box.min.y)) continue;
		if ((spt->y>mesh->box.max.y) && (ept->y>mesh->box.max.y)) continue;
		if ((spt->x<mesh->box.min.x) && (ept->x<mesh->box.min.x)) continue;
		if ((spt->x>mesh->box.max.x) && (ept->x>mesh->box.max.x)) continue;
		if ((spt->z<mesh->box.min.z) && (ept->z<mesh->box.min.z)) continue;
		if ((spt->z>mesh->box.max.z) && (ept->z>mesh->box.max.z)) continue;

			// check polys

		mesh_poly=mesh->polys;
	
		for (k=0;k!=mesh->npoly;k++) {

				// rough bounds check

			if ((spt->y<mesh_poly->box.min.y) && (ept->y<mesh_poly->box.min.y)) continue;
			if ((spt->y>mesh_poly->box.max.y) && (ept->y>mesh_poly->box.max.y)) continue;
			if ((spt->x<mesh_poly->box.min.x) && (ept->x<mesh_poly->box.min.x)) continue;
			if ((spt->x>mesh_poly->box.max.x) && (ept->x>mesh_poly->box.max.x)) continue;
			if ((spt->z<mesh_poly->box.min.z) && (ept->z<mesh_poly->box.min.z)) continue;
			if ((spt->z>mesh_poly->box.max.z) && (ept->z>mesh_poly->box.max.z)) continue;

				// ray trace
				
			t=ray_trace_mesh_polygon(spt,&vct,&pt,mesh,mesh_poly);
			if (t!=-1.0f) return(TRUE);
		
			mesh_poly++;
		}
	}

	return(FALSE);
}


