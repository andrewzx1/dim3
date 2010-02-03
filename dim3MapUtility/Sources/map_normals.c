/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Normal Routines

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

/* =======================================================

      Calculate Normals
      
======================================================= */

void map_recalc_normals_mesh(map_mesh_type *mesh,bool only_tangent_binormal)
{
	int					n;
	float				u10,u20,v10,v20,f_denom;
	d3vct				p10,p20,vlft,vrgt,v_num;
	d3pnt				*pt,*pt_1,*pt_2;
	map_mesh_poly_type	*poly;

		// expect polys to be flat (not twisted)
		// this would cause all sorts of rendering
		// errors anyway, so we can just consider
		// it a triangle to get the plane

	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {

			// get the side vectors (p1-p0) and (p2-p0)

		pt=&mesh->vertexes[poly->v[0]];
		pt_1=&mesh->vertexes[poly->v[1]];
		pt_2=&mesh->vertexes[poly->v[2]];

		vector_create(&p10,pt_1->x,pt_1->y,pt_1->z,pt->x,pt->y,pt->z);
		vector_create(&p20,pt_2->x,pt_2->y,pt_2->z,pt->x,pt->y,pt->z);

			// get the UV scalars (u1-u0), (u2-u0), (v1-v0), (v2-v0)

		u10=poly->main_uv.x[1]-poly->main_uv.x[0];
		u20=poly->main_uv.x[2]-poly->main_uv.x[0];
		v10=poly->main_uv.y[1]-poly->main_uv.y[0];
		v20=poly->main_uv.y[2]-poly->main_uv.y[0];

			// calculate the tangent
			// (v20xp10)-(v10xp20) / (u10*v20)-(v10*u20)

		vector_scalar_multiply(&vlft,&p10,v20);
		vector_scalar_multiply(&vrgt,&p20,v10);
		vector_subtract(&v_num,&vlft,&vrgt);

		f_denom=(u10*v20)-(v10*u20);

		vector_scalar_multiply(&poly->tangent_space.tangent,&v_num,(1.0f/f_denom));
		vector_normalize(&poly->tangent_space.tangent);

			// calculate the binormal
			// (u20xp10)-(u10xp20) / (v10*u20)-(u10*v20)

		vector_scalar_multiply(&vlft,&p10,u20);
		vector_scalar_multiply(&vrgt,&p20,u10);
		vector_subtract(&v_num,&vlft,&vrgt);

		f_denom=(v10*u20)-(u10*v20);

		vector_scalar_multiply(&poly->tangent_space.binormal,&v_num,(1.0f/f_denom));
		vector_normalize(&poly->tangent_space.binormal);

			// calculate the normal
			// T cross B (cross routine automatically normalizes)

		if (!only_tangent_binormal) {
			vector_cross_product(&poly->tangent_space.normal,&poly->tangent_space.tangent,&poly->tangent_space.binormal);
		}
		
		poly++;
	}
}

void map_recalc_normals(map_type *map,bool only_tangent_binormal)
{
	int					n;
	map_mesh_type		*mesh;

	mesh=map->mesh.meshes;

	for (n=0;n!=map->mesh.nmesh;n++) {
		map_recalc_normals_mesh(mesh,only_tangent_binormal);
		mesh++;
	}
}

