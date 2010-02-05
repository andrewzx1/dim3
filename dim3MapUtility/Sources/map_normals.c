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

#define normal_min_size_auto_out			350000

/* =======================================================

      Normal In/Out
      
======================================================= */

int map_recalc_normals_get_auto_mode(map_mesh_type *mesh)
{
	if ((mesh->box.max.x-mesh->box.min.x)<normal_min_size_auto_out) return(mesh_normal_mode_out);
	if ((mesh->box.max.y-mesh->box.min.y)<normal_min_size_auto_out) return(mesh_normal_mode_out);
	if ((mesh->box.max.z-mesh->box.min.z)<normal_min_size_auto_out) return(mesh_normal_mode_out);

	return(mesh_normal_mode_in);
}

bool map_recalc_normals_compare_sign(float f1,float f2)
{
	if ((f1<0.0f) && (f2<0.0f)) return(TRUE);
	if ((f1>=0.0f) && (f2>=0.0f)) return(TRUE);
	return(FALSE);
}

/* =======================================================

      Calculate Normals
      
======================================================= */

void map_recalc_normals_mesh(map_mesh_type *mesh,bool only_tangent_binormal)
{
	int					n,mode;
	float				u10,u20,v10,v20,f_denom;
	bool				is_out,invert;
	d3vct				p10,p20,vlft,vrgt,v_num,
						dvct;
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
	
		// setup mesh boxes
		
	map_prepare_mesh_box(mesh);
	
		// check for inversions
		
	mode=mesh->normal_mode;
	
	if (mode==mesh_normal_mode_auto) mode=map_recalc_normals_get_auto_mode(mesh);
	
	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {
		map_prepare_mesh_poly(mesh,poly);
		
		is_out=FALSE;
		
		dvct.x=(float)fabs(poly->box.mid.x-mesh->box.mid.x);
		dvct.y=(float)fabs(poly->box.mid.y-mesh->box.mid.y);
		dvct.z=(float)fabs(poly->box.mid.z-mesh->box.mid.z);
		
		if ((dvct.y>dvct.x) && (dvct.y>dvct.z)) {
			is_out=map_recalc_normals_compare_sign((float)(poly->box.mid.y-mesh->box.mid.y),poly->tangent_space.normal.y);
		}
		else {
			if (dvct.x>dvct.z) {
				is_out=map_recalc_normals_compare_sign((float)(poly->box.mid.x-mesh->box.mid.x),poly->tangent_space.normal.x);
			}
			else {
				is_out=map_recalc_normals_compare_sign((float)(poly->box.mid.z-mesh->box.mid.z),poly->tangent_space.normal.z);
			}
		}
		
		if (mode==mesh_normal_mode_in) {
			invert=is_out;
		}
		else {
			invert=!is_out;
		}
		
		if (invert) {
			poly->tangent_space.normal.x=-poly->tangent_space.normal.x;
			poly->tangent_space.normal.y=-poly->tangent_space.normal.y;
			poly->tangent_space.normal.z=-poly->tangent_space.normal.z;
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

