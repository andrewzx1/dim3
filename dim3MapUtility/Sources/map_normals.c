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

#define normal_min_size_auto_out			1000

#define calc_normal_dir_neg_x				0
#define calc_normal_dir_pos_x				1
#define calc_normal_dir_neg_y				2
#define calc_normal_dir_pos_y				3
#define calc_normal_dir_neg_z				4
#define calc_normal_dir_pos_z				5

/* =======================================================

      Normal In/Out
      
======================================================= */

int map_recalc_normals_get_auto_mode(map_mesh_type *mesh)
{
	double			dx,dy,dz;
	
		// find square footage
		
	dx=(double)(mesh->box.max.x-mesh->box.min.x)/1000.0;
	dy=(double)(mesh->box.max.y-mesh->box.min.y)/1000.0;
	dz=(double)(mesh->box.max.z-mesh->box.min.z)/1000.0;
	
		// compare square footage
		
	if ((dx*dy*dz)<normal_min_size_auto_out) return(mesh_normal_mode_out);

	return(mesh_normal_mode_in);
}

bool map_recalc_normals_compare_sign(float f1,float f2)
{
	if ((f1<0.0f) && (f2<0.0f)) return(TRUE);
	if ((f1>=0.0f) && (f2>=0.0f)) return(TRUE);
	return(FALSE);
}

bool map_recalc_normals_poly_is_outside(map_mesh_type *mesh,int poly_idx)
{
	int					n,dir;
	d3pnt				*pt;
	map_mesh_poly_type	*poly,*chk_poly;
	
	poly=&mesh->polys[poly_idx];
	
		// if any point is actually on a corner, it auto
		// counts as outside
		
	for (n=0;n!=poly->ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];
		if ((pt->x==mesh->box.min.x) && (pt->y==mesh->box.min.y) && (pt->z==mesh->box.min.z)) return(TRUE);
		if ((pt->x==mesh->box.min.x) && (pt->y==mesh->box.min.y) && (pt->z==mesh->box.max.z)) return(TRUE);
		if ((pt->x==mesh->box.min.x) && (pt->y==mesh->box.max.y) && (pt->z==mesh->box.min.z)) return(TRUE);
		if ((pt->x==mesh->box.min.x) && (pt->y==mesh->box.max.y) && (pt->z==mesh->box.max.z)) return(TRUE);
		if ((pt->x==mesh->box.max.x) && (pt->y==mesh->box.min.y) && (pt->z==mesh->box.min.z)) return(TRUE);
		if ((pt->x==mesh->box.max.x) && (pt->y==mesh->box.min.y) && (pt->z==mesh->box.max.z)) return(TRUE);
		if ((pt->x==mesh->box.max.x) && (pt->y==mesh->box.max.y) && (pt->z==mesh->box.min.z)) return(TRUE);
		if ((pt->x==mesh->box.max.x) && (pt->y==mesh->box.max.y) && (pt->z==mesh->box.max.z)) return(TRUE);
	}
	
		// sort everything from the middle point out,
		// if there is anything further out, then it's outside
		// and the original is inside
		
		// get direction
		
	if (poly->box.wall_like) {
		if ((poly->box.min.x<mesh->box.mid.x) && (poly->box.max.x<mesh->box.mid.x)) {
			dir=calc_normal_dir_neg_x;
		}
		else {
			if ((poly->box.min.x>mesh->box.mid.x) && (poly->box.max.x>mesh->box.mid.x)) {
				dir=calc_normal_dir_pos_x;
			}
			else {
				if ((poly->box.min.z<mesh->box.mid.z) && (poly->box.max.z<mesh->box.mid.z)) {
					dir=calc_normal_dir_neg_z;
				}
				else {
					dir=calc_normal_dir_pos_z;
				}
			}
		}
	}
	else {
		if (poly->box.min.y<mesh->box.mid.y) {
			dir=calc_normal_dir_neg_y;
		}
		else {
			dir=calc_normal_dir_pos_y;
		}
	}
		
		// check against other polys
		
	for (n=0;n!=mesh->npoly;n++) {
		if (n==poly_idx) continue;
		
			// only check polygons of same type
			
		chk_poly=&mesh->polys[n];
		if (chk_poly->box.wall_like!=poly->box.wall_like) continue;
		
			// which direction?
			
		switch (dir) {
		
			case calc_normal_dir_neg_x:
				if (chk_poly->box.max.y<=poly->box.min.y) break;
				if (chk_poly->box.min.y>=poly->box.max.y) break;
				if (chk_poly->box.max.z<=poly->box.min.z) break;
				if (chk_poly->box.min.z>=poly->box.max.z) break;
				if (chk_poly->box.mid.x<poly->box.mid.x) return(FALSE);
				break;
				
			case calc_normal_dir_pos_x:
				if (chk_poly->box.max.y<=poly->box.min.y) break;
				if (chk_poly->box.min.y>=poly->box.max.y) break;
				if (chk_poly->box.max.z<=poly->box.min.z) break;
				if (chk_poly->box.min.z>=poly->box.max.z) break;
				if (chk_poly->box.mid.x>poly->box.mid.x) return(FALSE);
				break;

			case calc_normal_dir_neg_y:
				if (chk_poly->box.max.x<=poly->box.min.x) break;
				if (chk_poly->box.min.x>=poly->box.max.x) break;
				if (chk_poly->box.max.z<=poly->box.min.z) break;
				if (chk_poly->box.min.z>=poly->box.max.z) break;
				if (chk_poly->box.mid.y<poly->box.mid.y) return(FALSE);
				break;

			case calc_normal_dir_pos_y:
				if (chk_poly->box.max.x<=poly->box.min.x) break;
				if (chk_poly->box.min.x>=poly->box.max.x) break;
				if (chk_poly->box.max.z<=poly->box.min.z) break;
				if (chk_poly->box.min.z>=poly->box.max.z) break;
				if (chk_poly->box.mid.y>poly->box.mid.y) return(FALSE);
				break;
				
			case calc_normal_dir_neg_z:
				if (chk_poly->box.max.x<=poly->box.min.x) break;
				if (chk_poly->box.min.x>=poly->box.max.x) break;
				if (chk_poly->box.max.y<=poly->box.min.y) break;
				if (chk_poly->box.min.y>=poly->box.max.y) break;
				if (chk_poly->box.mid.z<poly->box.mid.z) return(FALSE);
				break;
				
			case calc_normal_dir_pos_z:
				if (chk_poly->box.max.x<=poly->box.min.x) break;
				if (chk_poly->box.min.x>=poly->box.max.x) break;
				if (chk_poly->box.max.y<=poly->box.min.y) break;
				if (chk_poly->box.min.y>=poly->box.max.y) break;
				if (chk_poly->box.mid.z>poly->box.mid.z) return(FALSE);
				break;
			
		}
		
	}
	
	return(TRUE);
}

/* =======================================================

      Calculate Normals
      
======================================================= */

void map_recalc_normals_mesh(map_mesh_type *mesh,bool only_tangent_binormal)
{
	int					n,mode;
	float				u10,u20,v10,v20,f_denom;
	bool				is_out,invert;
	d3vct				p10,p20,vlft,vrgt,v_num;
	d3pnt				*pt,*pt_1,*pt_2;
	map_mesh_poly_type	*poly;
	
		// skip locked normals
		
	if (mesh->normal_mode==mesh_normal_mode_lock) return;

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
	
	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {
		map_prepare_mesh_poly(mesh,poly);
		poly++;
	}
	
		// check for inversions
		
	mode=mesh->normal_mode;
	
	if (mode==mesh_normal_mode_auto) mode=map_recalc_normals_get_auto_mode(mesh);
	
	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {
		
		is_out=FALSE;
		
		if ((fabs(poly->tangent_space.normal.y)>fabs(poly->tangent_space.normal.x)) && (fabs(poly->tangent_space.normal.y)>fabs(poly->tangent_space.normal.z))) {
			is_out=map_recalc_normals_compare_sign((float)(poly->box.mid.y-mesh->box.mid.y),poly->tangent_space.normal.y);
		}
		else {
			if (fabs(poly->tangent_space.normal.x)>fabs(poly->tangent_space.normal.z)) {
				is_out=map_recalc_normals_compare_sign((float)(poly->box.mid.x-mesh->box.mid.x),poly->tangent_space.normal.x);
			}
			else {
				is_out=map_recalc_normals_compare_sign((float)(poly->box.mid.z-mesh->box.mid.z),poly->tangent_space.normal.z);
			}
		}
		
		switch (mode) {
		
			case mesh_normal_mode_in:
				invert=is_out;
				break;
			case mesh_normal_mode_out:
				invert=!is_out;
				break;
			case mesh_normal_mode_in_out:
				if (map_recalc_normals_poly_is_outside(mesh,n)) {
					invert=!is_out;
				}
				else {
					invert=is_out;
				}
				break;
				
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

