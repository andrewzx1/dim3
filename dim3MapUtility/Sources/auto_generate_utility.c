/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Auto-Generation (and Import) Utility Routines

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
	#include "dim3maputility.h"
#endif

extern int								ag_box_count;
extern auto_generate_box_type			ag_boxes[max_ag_box];

int										map_ag_mesh_idx;

/* =======================================================

      Import Map Clear
      
======================================================= */

void map_auto_generate_clear(map_type *map)
{
	map->nspot=0;
	map->nnode=0;
	map->nscenery=0;
	map->nlight=0;
	map->nsound=0;
	map->nparticle=0;
	map->nmovement=0;
	map->ngroup=0;
	map->narea=0;
	map->mesh.nmesh=0;
	map->liquid.nliquid=0;
}

/* =======================================================

      Random Number Generator
      
======================================================= */

void map_auto_generate_random_seed(int seed)
{
	srandom(seed);
}

int map_auto_generate_random_int(int max)
{
	float			f;

	f=(float)random()/(float)RAND_MAX;
	return((int)(f*(float)max));
}

/* =======================================================

      Portal Collisions and Connections
      
======================================================= */

bool map_auto_generate_portal_collision(int x,int z,int ex,int ez,int skip_idx,bool corridor_only)
{
	int						n;
	auto_generate_box_type	*portal;

	for (n=0;n!=ag_box_count;n++) {
		if (skip_idx==n) continue;

		portal=&ag_boxes[n];

		if (corridor_only) {
			if (portal->corridor_flag==ag_corridor_flag_portal) continue;
		}

		if (ez<=portal->min.z) continue;
		if (ex<=portal->min.x) continue;
		if (x>=portal->max.x) continue;
		if (z>=portal->max.z) continue;

		return(TRUE);
	}

	return(FALSE);
}

int map_auto_generate_portal_horz_edge_block(int skip_portal_idx,int z,int ez,int x)
{
	int						n;
	auto_generate_box_type	*portal;

	for (n=0;n!=ag_box_count;n++) {
		if (skip_portal_idx==n) continue;

		portal=&ag_boxes[n];

		if ((portal->min.x!=x) && (portal->max.x!=x)) continue;
		if ((z>=portal->min.z) && (z<portal->max.z)) return(n);
		if ((ez>portal->min.z) && (ez<=portal->max.z)) return(n);
	}

	return(-1);
}

int map_auto_generate_portal_vert_edge_block(int skip_portal_idx,int x,int ex,int z)
{
	int						n;
	auto_generate_box_type	*portal;

	for (n=0;n!=ag_box_count;n++) {
		if (skip_portal_idx==n) continue;

		portal=&ag_boxes[n];

		if ((portal->min.z!=z) && (portal->max.z!=z)) continue;
		if ((x>=portal->min.x) && (x<portal->max.x)) return(n);
		if ((ex>portal->min.x) && (ex<=portal->max.x)) return(n);
	}

	return(-1);
}

bool map_auto_generate_portal_horz_edge_touch(int skip_portal_idx,int z,int ez,int x)
{
	int						n;
	auto_generate_box_type	*portal;

	for (n=0;n!=ag_box_count;n++) {
		if (skip_portal_idx==n) continue;

		portal=&ag_boxes[n];

		if ((portal->min.x!=x) && (portal->max.x!=x)) continue;
		if ((z>=portal->min.z) && (z<portal->max.z)) return(TRUE);
		if ((ez>portal->min.z) && (ez<=portal->max.z)) return(TRUE);
		if ((portal->min.z>=z) && (portal->min.z<=ez)) return(TRUE);
		if ((portal->max.z>=z) && (portal->max.z<=ez)) return(TRUE);
	}

	return(FALSE);
}

bool map_auto_generate_portal_vert_edge_touch(int skip_portal_idx,int x,int ex,int z)
{
	int						n;
	auto_generate_box_type	*portal;

	for (n=0;n!=ag_box_count;n++) {
		if (skip_portal_idx==n) continue;

		portal=&ag_boxes[n];

		if ((portal->min.z!=z) && (portal->max.z!=z)) continue;
		if ((x>=portal->min.x) && (x<portal->max.x)) return(TRUE);
		if ((ex>portal->min.x) && (ex<=portal->max.x)) return(TRUE);
		if ((portal->min.x>=x) && (portal->min.x<=ex)) return(TRUE);
		if ((portal->max.x>=x) && (portal->max.x<=ex)) return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Portal Edge Touching
      
======================================================= */

bool map_auto_generate_portal_touching_left(int portal_idx)
{
	int							n;
	auto_generate_box_type		*portal,*chk_portal;

	portal=&ag_boxes[portal_idx];
	
	for (n=0;n!=ag_box_count;n++) {
		if (portal_idx==n) continue;
		
		chk_portal=&ag_boxes[n];
		if (chk_portal->corridor_flag!=ag_corridor_flag_portal) continue;
		
		if (((chk_portal->min.z>=portal->min.z) && (chk_portal->min.z<portal->max.z)) || ((chk_portal->max.z>portal->min.z) && (chk_portal->max.z<=portal->max.z))) {
			if (portal->min.x==chk_portal->max.x) return(TRUE);
		}
	}

	return(FALSE);
}

bool map_auto_generate_portal_touching_right(int portal_idx)
{
	int							n;
	auto_generate_box_type		*portal,*chk_portal;

	portal=&ag_boxes[portal_idx];
	
	for (n=0;n!=ag_box_count;n++) {
		if (portal_idx==n) continue;

		chk_portal=&ag_boxes[n];
		if (chk_portal->corridor_flag!=ag_corridor_flag_portal) continue;
		
		if (((chk_portal->min.z>=portal->min.z) && (chk_portal->min.z<portal->max.z)) || ((chk_portal->max.z>portal->min.z) && (chk_portal->max.z<=portal->max.z))) {
			if (portal->max.x==chk_portal->min.x) return(TRUE);
		}
	}

	return(FALSE);
}

bool map_auto_generate_portal_touching_top(int portal_idx)
{
	int							n;
	auto_generate_box_type		*portal,*chk_portal;

	portal=&ag_boxes[portal_idx];
	
	for (n=0;n!=ag_box_count;n++) {
		if (portal_idx==n) continue;

		chk_portal=&ag_boxes[n];
		if (chk_portal->corridor_flag!=ag_corridor_flag_portal) continue;
		
		if (((chk_portal->min.x>=portal->min.x) && (chk_portal->min.x<portal->max.x)) || ((chk_portal->max.x>portal->min.x) && (chk_portal->max.x<=portal->max.x))) {
			if (portal->min.z==chk_portal->max.z) return(TRUE);
		}
	}

	return(FALSE);
}

bool map_auto_generate_portal_touching_bottom(int portal_idx)
{
	int							n;
	auto_generate_box_type		*portal,*chk_portal;

	portal=&ag_boxes[portal_idx];
	
	for (n=0;n!=ag_box_count;n++) {
		if (portal_idx==n) continue;

		chk_portal=&ag_boxes[n];
		if (chk_portal->corridor_flag!=ag_corridor_flag_portal) continue;
		
		if (((chk_portal->min.x>=portal->min.x) && (chk_portal->min.x<portal->max.x)) || ((chk_portal->max.x>portal->min.x) && (chk_portal->max.x<=portal->max.x))) {
			if (portal->max.z==chk_portal->min.z) return(TRUE);
		}
	}

	return(FALSE);
}

bool map_auto_generate_portal_touching_any(int portal_idx)
{
	if (map_auto_generate_portal_touching_left(portal_idx)) return(TRUE);
	if (map_auto_generate_portal_touching_right(portal_idx)) return(TRUE);
	if (map_auto_generate_portal_touching_top(portal_idx)) return(TRUE);
	if (map_auto_generate_portal_touching_bottom(portal_idx)) return(TRUE);

	return(FALSE);
}

/* =======================================================

      Map Edge Touching
      
======================================================= */

bool map_auto_generate_portal_on_map_left_edge(int portal_idx)
{
	int							n;
	auto_generate_box_type		*portal,*chk_portal;

	portal=&ag_boxes[portal_idx];
	
	for (n=0;n!=ag_box_count;n++) {
		if (portal_idx==n) continue;
		
		chk_portal=&ag_boxes[n];
		
		if (((portal->min.z>=chk_portal->min.z) && (portal->min.z<chk_portal->max.z)) || ((portal->max.z>chk_portal->min.z) && (portal->max.z<=chk_portal->max.z))) {
			if (chk_portal->max.x<portal->min.x) return(FALSE);
		}
	}

	return(TRUE);
}

bool map_auto_generate_portal_on_map_right_edge(int portal_idx)
{
	int							n;
	auto_generate_box_type		*portal,*chk_portal;

	portal=&ag_boxes[portal_idx];
	
	for (n=0;n!=ag_box_count;n++) {
		if (portal_idx==n) continue;

		chk_portal=&ag_boxes[n];
		
		if (((portal->min.z>=chk_portal->min.z) && (portal->min.z<chk_portal->max.z)) || ((portal->max.z>chk_portal->min.z) && (portal->max.z<=chk_portal->max.z))) {
			if (chk_portal->min.x>portal->max.x) return(FALSE);
		}
	}

	return(TRUE);
}

bool map_auto_generate_portal_on_map_top_edge(int portal_idx)
{
	int							n;
	auto_generate_box_type		*portal,*chk_portal;

	portal=&ag_boxes[portal_idx];
	
	for (n=0;n!=ag_box_count;n++) {
		if (portal_idx==n) continue;

		chk_portal=&ag_boxes[n];
		
		if (((portal->min.x>=chk_portal->min.x) && (portal->min.x<chk_portal->max.x)) || ((portal->max.x>chk_portal->min.x) && (portal->max.x<=chk_portal->max.x))) {
			if (chk_portal->max.z<portal->min.z) return(FALSE);
		}
	}

	return(TRUE);
}

bool map_auto_generate_portal_on_map_bottom_edge(int portal_idx)
{
	int							n;
	auto_generate_box_type		*portal,*chk_portal;

	portal=&ag_boxes[portal_idx];
	
	for (n=0;n!=ag_box_count;n++) {
		if (portal_idx==n) continue;

		chk_portal=&ag_boxes[n];
		
		if (((portal->min.x>=chk_portal->min.x) && (portal->min.x<chk_portal->max.x)) || ((portal->max.x>chk_portal->min.x) && (portal->max.x<=chk_portal->max.x))) {
			if (chk_portal->min.z>portal->max.z) return(FALSE);
		}
	}

	return(TRUE);
}

/* =======================================================

      Polygon Utilities
      
======================================================= */

float map_auto_generate_poly_reduce_coord(float f)
{
	int			i;

	i=(int)f;
	return(f-(float)i);
}

void map_auto_generate_poly_from_square_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy)
{
	x[0]=x[3]=lx;
	x[1]=x[2]=rx;
	z[0]=z[3]=lz;
	z[1]=z[2]=rz;

	y[0]=y[1]=ty;
	y[2]=y[3]=by;
	
	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;
}

void map_auto_generate_poly_from_top_trig_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy)
{
	x[0]=x[2]=lx;
	x[1]=rx;
	z[0]=z[2]=lz;
	z[1]=rz;

	y[0]=ty;
	y[1]=y[2]=by;

	gx[0]=gx[1]=gx[2]=0.0f;
	gy[0]=gy[1]=gy[2]=0.0f;
}

void map_auto_generate_poly_from_bot_trig_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy)
{
	x[0]=x[2]=lx;
	x[1]=rx;
	z[0]=z[2]=lz;
	z[1]=rz;

	y[0]=by;
	y[1]=y[2]=ty;

	gx[0]=gx[1]=gx[2]=0.0f;
	gy[0]=gy[1]=gy[2]=0.0f;
}

void map_auto_generate_poly_from_square_floor(int lx,int lz,int rx,int rz,int fy,int *x,int *y,int *z,float *gx,float *gy)
{
	x[0]=x[3]=lx;
	x[1]=x[2]=rx;
	z[0]=z[1]=lz;
	z[2]=z[3]=rz;
		
	y[0]=y[1]=y[2]=y[3]=fy;

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;
}

void map_auto_generate_poly_from_square_floor_slant(int lx,int lz,int rx,int rz,int fy,int yadd,int lower_mode,bool reverse_slant,int *x,int *y,int *z,float *gx,float *gy)
{
	map_auto_generate_poly_from_square_floor(lx,lz,rx,rz,fy,x,y,z,gx,gy);
	
	if (reverse_slant) {
	
		switch (lower_mode) {
			case ag_ceiling_lower_neg_x:
				y[0]+=yadd;
				y[3]+=yadd;
				break;
			case ag_ceiling_lower_pos_x:
				y[1]+=yadd;
				y[2]+=yadd;
				break;
			case ag_ceiling_lower_neg_z:
				y[0]+=yadd;
				y[1]+=yadd;
				break;
			case ag_ceiling_lower_pos_z:
				y[2]+=yadd;
				y[3]+=yadd;
				break;
		}
	}
	
	else {
	
		switch (lower_mode) {
			case ag_ceiling_lower_neg_x:
				y[0]-=yadd;
				y[3]-=yadd;
				break;
			case ag_ceiling_lower_pos_x:
				y[1]-=yadd;
				y[2]-=yadd;
				break;
			case ag_ceiling_lower_neg_z:
				y[0]-=yadd;
				y[1]-=yadd;
				break;
			case ag_ceiling_lower_pos_z:
				y[2]-=yadd;
				y[3]-=yadd;
				break;
		}
	
	}
}

/* =======================================================

      Mesh/Poly Adding
      
======================================================= */

bool map_auto_generate_mesh_start(map_type *map,int group_idx,bool moveable)
{
	map_mesh_type			*mesh;

		// create a new mesh

	map_ag_mesh_idx=map_mesh_add(map);
	if (map_ag_mesh_idx==-1) return(FALSE);

	mesh=&map->mesh.meshes[map_ag_mesh_idx];
	mesh->group_idx=group_idx;
	mesh->flag.moveable=moveable;

		// consider everything created through
		// this routine to have out-facing normals

	mesh->normal_mode=mesh_normal_mode_out;

	return(TRUE);
}

void map_auto_generate_mesh_set_portal_mesh(int box_idx)
{
	map_ag_mesh_idx=ag_boxes[box_idx].mesh_idx;
}

void map_auto_generate_mesh_set_lock(map_type *map)
{
	map->mesh.meshes[map_ag_mesh_idx].flag.lock_uv=TRUE;
}

void map_auto_generate_mesh_set_rot_offset(map_type *map,int x,int y,int z)
{
	map_mesh_type			*mesh;

	mesh=&map->mesh.meshes[map_ag_mesh_idx];

	mesh->rot_off.x=x;
	mesh->rot_off.y=y;
	mesh->rot_off.z=z;

	mesh->flag.rot_independent=FALSE;
}

bool map_auto_generate_mesh_add_poly(map_type *map,int box_idx,int txt_idx,int ptsz,int *x,int *y,int *z,float *gx,float *gy)
{
	int						n,px[8],pz[8];
	auto_generate_box_type	*box;

		// move within x/z bounds
		
	box=&ag_boxes[box_idx];

	for (n=0;n!=ptsz;n++) {
		px[n]=x[n]+box->min.x;
		pz[n]=z[n]+box->min.z;
	}

		// add the mesh
		
	return(map_mesh_add_poly(map,map_ag_mesh_idx,ptsz,px,y,pz,gx,gy,txt_idx)!=-1);
}

void map_auto_generate_mesh_get_last_poly_index(map_type *map,int *mesh_idx,int *poly_idx)
{
	map_mesh_type	*mesh;

	*mesh_idx=map_ag_mesh_idx;

	mesh=&map->mesh.meshes[map_ag_mesh_idx];
	*poly_idx=mesh->npoly-1;
}

void map_auto_generate_mesh_get_poly_points(map_type *map,int mesh_idx,int poly_idx,int *ptsz,int *px,int *py,int *pz)
{
	int					n;
	d3pnt				*pt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];

	for (n=0;n!=poly->ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];
		px[0]=pt->x;
		py[0]=pt->y;
		pz[0]=pt->z;
	}

	*ptsz=poly->ptsz;
}

void map_auto_generate_mesh_poly_punch_hole(map_type *map,int mesh_idx,int poly_idx,int x,int z,int txt_idx)
{
	int				n,start_idx;
	d3pnt			extrude_pnt;
	map_mesh_type	*mesh;

		// remember last poly

	mesh=&map->mesh.meshes[mesh_idx];
	start_idx=mesh->npoly+3;

		// punch the hole

	extrude_pnt.x=x;
	extrude_pnt.y=0;
	extrude_pnt.z=z;
	
	map_mesh_poly_punch_hole(map,mesh_idx,poly_idx,&extrude_pnt);

		// fix the textures in new hole

	for (n=start_idx;n<mesh->npoly;n++) {
		mesh->polys[n].txt_idx=txt_idx;
	}
}

void map_auto_generate_mesh_effect_uv_last_poly(map_type *map,float mult_x,float mult_y,bool rot)
{
	int						n;
	float					f;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	mesh=&map->mesh.meshes[map_ag_mesh_idx];
	poly=&mesh->polys[mesh->npoly-1];

	for (n=0;n!=poly->ptsz;n++) {
		poly->main_uv.x[n]*=mult_x;
		poly->main_uv.y[n]*=mult_y;

		if (rot) {
			f=poly->main_uv.x[n];
			poly->main_uv.x[n]=poly->main_uv.y[n];
			poly->main_uv.y[n]=f;
		}
	}
}

/* =======================================================

      Create Simple Lights
      
======================================================= */

void map_auto_generate_add_simple_lights(map_type *map)
{
	int							n,x,z,y,intensity;
	double						dx,dz;
	auto_generate_box_type		*portal;
	map_light_type				*lit;

	portal=ag_boxes;
	
	for (n=0;n!=ag_box_count;n++) {

		if (map->nlight>=max_map_light) break;
	
			// find light position

		x=(portal->min.x+portal->max.x)>>1;
		y=(portal->min.y+portal->max.y)>>1;
		z=(portal->min.z+portal->max.z)>>1;
		
			// get intensity
			
		dx=(portal->max.x-portal->min.x);
		dz=(portal->max.z-portal->min.z);
		intensity=(int)sqrt((dx*dx)+(dz*dz));
		
		intensity/=2;								// radius, so use half
		if (intensity<100) intensity=100;
		
			// create the light
	
		lit=&map->lights[map->nlight];
		map->nlight++;
		
		lit->pnt.x=x;
		lit->pnt.y=y;
		lit->pnt.z=z;
		lit->type=lt_normal;
		lit->light_map=TRUE;
		lit->direction=ld_all;
		lit->col.r=lit->col.g=lit->col.b=1.0f;
		lit->intensity=intensity;
		lit->exponent=1.0f;
		lit->name[0]=0x0;
		lit->on=TRUE;

		portal++;
	}
}

/* =======================================================

      Add Player Start
      
======================================================= */

void map_auto_generate_add_player_spot(map_type *map)
{
	int							x,y,z;
	spot_type					*spot;
	auto_generate_box_type		*portal;
	
	if (ag_box_count==0) return;
		
		// spot place
	
	portal=&ag_boxes[0];

	x=(portal->min.x+portal->max.x)>>1;
	y=portal->max.y;
	z=(portal->min.z+portal->max.z)>>1;
	
		// add spot
		
	spot=&map->spots[0];
	map->nspot=1;
	
	spot->pnt.x=x;
	spot->pnt.y=y;
	spot->pnt.z=z;
	
	spot->ang.x=0.0f;
	spot->ang.y=135.0f;
	spot->ang.z=0.0f;
	
	strcpy(spot->name,"Start");
	strcpy(spot->type,"Player");
	strcpy(spot->script,"Player");
	spot->display_model[0]=0x0;
	spot->params[0]=0x0;
}

/* =======================================================

      Rebuild All UVs
      
======================================================= */

void map_auto_generate_reset_UVs(map_type *map)
{
    int				n;
	map_mesh_type	*mesh;
    
 	mesh=map->mesh.meshes;
	
	for (n=0;n!=map->mesh.nmesh;n++) {
		if (!mesh->flag.lock_uv) map_mesh_reset_uv(map,n);
		mesh++;
	}
}

