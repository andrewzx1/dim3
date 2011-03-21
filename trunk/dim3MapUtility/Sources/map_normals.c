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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
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

      Mesh Centers for Normal
      
======================================================= */

void map_recalc_normals_find_mesh_center_for_poly_add_recursive_poly_vertex(map_mesh_type *mesh,int poly_idx,unsigned char *vertex_mask,unsigned char *poly_mask)
{
	int					n,k,t,vertex_idx;
	map_mesh_poly_type	*poly,*chk_poly;
	
		// mark all vertexes
		// touched by this poly
		
	poly=&mesh->polys[poly_idx];
	
	for (n=0;n!=poly->ptsz;n++) {
	
		vertex_idx=poly->v[n];
		if (vertex_mask[vertex_idx]==0x1) continue;
		
		vertex_mask[vertex_idx]=0x1;
		
			// find any other polys touching
			// vertexes to build proper vertexes
			
		chk_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			if (k!=poly_idx) {
				
				if (poly_mask[k]==0x1) {
					chk_poly++;
					continue;
				}
				
				for (t=0;t!=chk_poly->ptsz;t++) {
					if (chk_poly->v[t]==vertex_idx) {
						poly_mask[k]=0x1;
						map_recalc_normals_find_mesh_center_for_poly_add_recursive_poly_vertex(mesh,k,vertex_mask,poly_mask);
						break;
					}
				}
			}
			
			chk_poly++;
		}
	}
}

bool map_recalc_normals_find_mesh_center_for_poly(map_mesh_type *mesh,int poly_idx,d3pnt *center)
{
	int					n,cnt;
	unsigned char		*vertex_mask,*poly_mask;
	d3pnt				*pt;
	
		// mesh centers for normals are all the points that are
		// connected to this polygon by other polygons.  This
		// helps separate meshes into smaller units when they are composed
		// of distinct primitive like meshes
		
	center->x=center->y=center->z=0;
	
		// find all common vertexes
		// by recursively adding vertexes
		// for polys touching them
		
	vertex_mask=(unsigned char*)malloc(mesh->nvertex);
	if (vertex_mask==NULL) return(TRUE);
	
	bzero(vertex_mask,mesh->nvertex);
	
	poly_mask=(unsigned char*)malloc(mesh->npoly);
	if (poly_mask==NULL) {
		free(vertex_mask);
		return(TRUE);
	}
	
	bzero(poly_mask,mesh->npoly);
	
	map_recalc_normals_find_mesh_center_for_poly_add_recursive_poly_vertex(mesh,poly_idx,vertex_mask,poly_mask);
	
		// find center
		
	cnt=0;
	
	for (n=0;n!=mesh->nvertex;n++) {
		if (vertex_mask[n]!=0x0) {
			pt=&mesh->vertexes[n];
			center->x+=pt->x;
			center->y+=pt->y;
			center->z+=pt->z;
			cnt++;
		}
	}
	
	if (cnt!=0) {
		center->x/=cnt;
		center->y/=cnt;
		center->z/=cnt;
	}
	
		// special flag if all polys are connected
		
	return(cnt==mesh->nvertex);
}

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

bool map_recalc_normals_poly_is_outside_edge(map_mesh_type *mesh,int poly_idx)
{
	int					n,cnt;
	d3pnt				*pt;
	map_mesh_poly_type	*poly;
	
	poly=&mesh->polys[poly_idx];
	
		// if any point is on any 2 edges, it counts
		// as outside
		
	for (n=0;n!=poly->ptsz;n++) {
		cnt=0;
		
		pt=&mesh->vertexes[poly->v[n]];
		if ((pt->x==mesh->box.min.x) || (pt->x==mesh->box.max.x)) cnt++;
		if ((pt->y==mesh->box.min.y) || (pt->y==mesh->box.max.y)) cnt++;
		if ((pt->z==mesh->box.min.z) || (pt->z==mesh->box.max.z)) cnt++;
		
		if (cnt>=2) return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Calculate Normals
      
======================================================= */

void map_recalc_normals_mesh(map_mesh_type *mesh,bool only_tangent_binormal)
{
	int					n,mode;
	float				u10,u20,v10,v20,f_denom;
	bool				is_out,invert,all_poly_mesh;
	d3vct				p10,p20,vlft,vrgt,v_num,face_vct;
	d3pnt				*pt,*pt_1,*pt_2,center;
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
	
	all_poly_mesh=FALSE;
	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {
	
			// get center for polys connected to
			// this poly.  Happens in meshes where there
			// are distinct primitives
			
			// we have a flag that tells us if the first time
			// we checked this that all polys were connected, so
			// we can skip.  This is a good speed up on enormous meshes
			
		if (!all_poly_mesh) {
			all_poly_mesh=map_recalc_normals_find_mesh_center_for_poly(mesh,n,&center);
		}
		
			// determine if poly is facing 'out'
		
		vector_create(&face_vct,poly->box.mid.x,poly->box.mid.y,poly->box.mid.z,center.x,center.y,center.z);
		is_out=(vector_dot_product(&poly->tangent_space.normal,&face_vct)>0.0f);
		
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
			case mesh_normal_mode_edge:
				if (map_recalc_normals_poly_is_outside_edge(mesh,n)) {
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

