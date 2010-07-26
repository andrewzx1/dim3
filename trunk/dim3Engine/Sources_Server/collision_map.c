/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Collisions

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

#include "scripts.h"
#include "objects.h"
#include "weapons.h"
#include "projectiles.h"
#include "models.h"
#include "physics.h"

extern map_type			map;
extern view_type		view;
extern server_type		server;

/* =======================================================

      Determine if Contact Poly is Wall-Like
      
======================================================= */

bool collide_contact_is_wall_hit(poly_pointer_type *hit_poly)
{
	map_mesh_poly_type		*mesh_poly;

	if (hit_poly->mesh_idx==-1) return(FALSE);

	mesh_poly=&map.mesh.meshes[hit_poly->mesh_idx].polys[hit_poly->poly_idx];
	return(mesh_poly->box.wall_like);
}

/* =======================================================

      Box-Map Collisions
      
======================================================= */

int collide_point_distance(d3pnt *pt_1,d3pnt *pt_2)
{
	double		dx,dz,dy;
	
	dx=(double)(pt_1->x-pt_2->x);
	dy=(double)(pt_1->y-pt_2->y);
	dz=(double)(pt_1->z-pt_2->z);
	
	return((int)sqrt((dx*dx)+(dy*dy)+(dz*dz)));
}

bool collide_object_box_to_map(obj_type *obj,d3pnt *pt,d3pnt *box_sz,int *xadd,int *yadd,int *zadd)
{
	int						n,k,y,idx,d,dist,
							hx[collide_obj_ray_spindle_count],hz[collide_obj_ray_spindle_count],vert_y[5];
	float					move_ang,ang,fx,fy,fz;
	double					rad;
	bool					hits[collide_obj_ray_count];
	d3pnt					mov,spt[collide_obj_ray_count],ept[collide_obj_ray_count],hpt[collide_obj_ray_count];
	ray_trace_contact_type	base_contact,contacts[collide_obj_ray_count];
	matrix_type				mat;

		// get movements out of pointers
		// and movement angle

	mov.x=*xadd;
	mov.y=*yadd;
	mov.z=*zadd;

	move_ang=angle_find(0,0,mov.x,mov.z);

		// vertical race trace positions

	vert_y[0]=obj->pnt.y-obj->size.y;
	vert_y[2]=obj->pnt.y-(obj->size.y>>1);
	vert_y[4]=obj->pnt.y-1;

	vert_y[1]=(vert_y[0]+vert_y[2])>>1;
	vert_y[3]=(vert_y[2]+vert_y[4])>>1;
	
		// create the ray trace points
		
	ang=360.0f-collide_obj_ray_half_sweep;
	
	matrix_rotate_y(&mat,angle_add(move_ang,180.0f));
		
	for (n=0;n!=collide_obj_ray_spindle_count;n++) {
	
			// the eclipse points
			
		rad=(double)(ang*ANG_to_RAD);
		
		fx=(float)((obj->size.x>>1))*(float)sin(rad);
		fz=(float)((obj->size.z>>1))*(float)cos(rad);
		
		ang+=collide_obj_ray_spindle_size;
		if (ang>=360.0f) ang=ang-360.0f;
		
			// rotate the eclipse
			
		fy=0.0f;
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
		
		hx[n]=(int)fx+mov.x;
		hz[n]=(int)fz+mov.z;
	}

		// create the rays that come from
		// center of object

	idx=0;

	for (k=0;k!=5;k++) {

		y=vert_y[k];

		for (n=0;n!=collide_obj_ray_spindle_count;n++) {

			spt[idx].x=obj->pnt.x;
			spt[idx].y=y;
			spt[idx].z=obj->pnt.z;
			ept[idx].x=obj->pnt.x+hx[n];
			ept[idx].y=y+mov.y;
			ept[idx].z=obj->pnt.z+hz[n];

			idx++;
		}
	}

		// collision debugging

	if (view.debug.on) {
		memmove(obj->debug.collide_spt,spt,(sizeof(d3pnt)*collide_obj_ray_count));
		memmove(obj->debug.collide_ept,ept,(sizeof(d3pnt)*collide_obj_ray_count));
	}

		// set the collisions and run the
		// ray tracing

	base_contact.obj.on=TRUE;
	base_contact.obj.ignore_uid=obj->idx;

	base_contact.proj.on=FALSE;
	base_contact.proj.ignore_uid=-1;

	base_contact.hit_mode=poly_ray_trace_hit_mode_wall_only;
	base_contact.origin=poly_ray_trace_origin_object;

		// run ray trace

	ray_trace_map_by_point_array(collide_obj_ray_count,spt,ept,hpt,hits,&base_contact,contacts);

		// find the one that moves the leasts
		// as the most suitable hit point

	idx=-1;
	dist=0;

	for (n=0;n!=collide_obj_ray_count;n++) {
			
		if (hits[n]) {
				
			d=collide_point_distance(&spt[n],&hpt[n]);
			if ((d<=dist) || (idx==-1)) {
				dist=d;
				idx=n;
			}
		}
	}

		// no hits?

	if (idx==-1) return(FALSE);
	
		// setup the hits
		
	*xadd=hpt[idx].x-spt[idx].x;
	*zadd=hpt[idx].z-spt[idx].z;

	if (contacts[idx].poly.mesh_idx!=-1) {
		memmove(&obj->contact.hit_poly,&contacts[idx].poly,sizeof(poly_pointer_type));
	}

	if (contacts[idx].obj.uid!=-1) {
		obj->contact.obj_idx=contacts[idx].obj.uid;
		obj->contact.hit_face=contacts[idx].obj.hit_face;
	}

	if (contacts[idx].proj.uid!=-1) {
		obj->contact.proj_idx=contacts[idx].proj.uid;
		obj->contact.hit_face=contacts[idx].proj.hit_face;
	}

	return(TRUE);
}

/* =======================================================

      Object-Map Collisions
      
======================================================= */

bool collide_object_to_map(obj_type *obj,int *xadd,int *yadd,int *zadd)
{
	int					n,nhit_box;
	bool				hit_box_hit;
	d3pnt				pt,box_sz;
	model_draw			*draw;
	model_type			*model;
	model_hit_box_type	*hit_box;

		// are we using hit box hits?

	hit_box_hit=FALSE;

	if (obj->hit_box.on) {
		draw=&obj->draw;
		if ((draw->model_idx!=-1) && (!draw->on)) {
			model=server.model_list.models[draw->model_idx];
			hit_box_hit=TRUE;
		}
	}

		// regular collisions

	if (!hit_box_hit) {
		box_sz.x=obj->size.x;
		box_sz.z=obj->size.z;

		box_sz.y=obj->size.y;
		if (obj->duck.mode!=dm_stand) box_sz.y-=obj->duck.y_move;

		return(collide_object_box_to_map(obj,&obj->pnt,&box_sz,xadd,yadd,zadd));
	}

		// hit box collisions

	else {

		hit_box=model->hit_boxes;
		nhit_box=model->nhit_box;
		
		for (n=0;n!=nhit_box;n++) {
			pt.x=obj->pnt.x+hit_box->box.offset.x;
			pt.y=obj->pnt.y+hit_box->box.offset.y;
			pt.z=obj->pnt.z+hit_box->box.offset.z;

			box_sz.x=hit_box->box.size.x;
			box_sz.z=hit_box->box.size.z;

			box_sz.y=hit_box->box.size.y;
			if (obj->duck.mode!=dm_stand) box_sz.y-=obj->duck.y_move;
			
			if (collide_object_box_to_map(obj,&pt,&box_sz,xadd,yadd,zadd)) {
				obj->hit_box.obj_hit_box_idx=n;
				return(TRUE);
			}

			hit_box++;
		}
	}

	return(FALSE);
}

bool collide_object_to_map_bump(obj_type *obj,int xadd,int yadd,int zadd,int *bump_y_move)
{
	int							n,k,ty,px[4],pz[4];
	d3pnt						min,max;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	obj_type	 				*check_obj;

		// get polygon and object bounds

	collide_object_polygon(obj,xadd,zadd,px,pz);

	min.x=max.x=px[0];
	min.z=max.z=pz[0];

	for (n=1;n!=4;n++) {
		if (px[n]<min.x) min.x=px[n];
		if (px[n]>max.x) max.x=px[n];
		if (pz[n]<min.z) min.z=pz[n];
		if (pz[n]>max.z) max.z=pz[n];
	}

		// min y is top of bump over

	max.y=obj->pnt.y+yadd;
	min.y=max.y-obj->bump.high;

		// check mesh polygons

		// setup collision checks

	polygon_2D_collision_setup(4,px,pz);

		// find polys to check

	for (n=0;n!=map.mesh.nmesh;n++) {

		mesh=&map.mesh.meshes[n];
		
			// rough bounds check

		if (max.x<mesh->box.min.x) continue;
		if (min.x>mesh->box.max.x) continue;
		if (max.z<mesh->box.min.z) continue;
		if (min.z>mesh->box.max.z) continue;
		if (max.y<mesh->box.min.y) continue;
		if (min.y>mesh->box.max.y) continue;
			
			// skip off and pass through meshes

		if (!mesh->flag.on) continue;
		if (mesh->flag.pass_through) continue;
		
			// check wall polys
			
		for (k=0;k!=mesh->poly_list.wall_count;k++) {

			poly=&mesh->polys[mesh->poly_list.wall_idxs[k]];
			if (!poly->draw.bump_ok) continue;
			
				// rough bounds check

			if (max.x<poly->box.min.x) continue;
			if (min.x>poly->box.max.x) continue;
			if (max.z<poly->box.min.z) continue;
			if (min.z>poly->box.max.z) continue;
			if (max.y<poly->box.min.y) continue;
			if (min.y>poly->box.max.y) continue;

				// is it a bump up candidate?

			if ((poly->box.min.y<min.y) || (poly->box.min.y>=max.y)) continue;
			if (!polygon_2D_collision_line(poly->line.lx,poly->line.lz,poly->line.rx,poly->line.rz)) continue;

			*bump_y_move=poly->box.min.y-obj->pnt.y;		// don't use Y added version, it bumps too much on slopes

			return(TRUE);
		}
				
	}

		// check objects

	for (n=0;n!=max_obj_list;n++) {

		check_obj=server.obj_list.objs[n];
		if (check_obj==NULL) continue;
	
			// any collision?

		if ((check_obj->hidden) || (!check_obj->contact.object_on) || (check_obj->pickup.on) || (check_obj->idx==obj->idx)) continue;
		if (!collide_object_to_object(obj,xadd,zadd,check_obj,TRUE,FALSE)) continue;
		
			// is it a bump up candidate?
		
		ty=obj->pnt.y-obj->size.y;
		if ((ty<min.y) || (obj->pnt.y>=max.y)) continue;
		
		*bump_y_move=ty-obj->pnt.y;		// don't use Y added version, it bumps too much on slopes
		return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Project-Map Collisions
      
======================================================= */

bool collide_projectile_to_map(proj_type *proj,int xadd,int yadd,int zadd)
{
	d3pnt					spt,ept,hpt;
	ray_trace_contact_type	contact;
	obj_type				*obj;
	weapon_type				*weap;
	proj_setup_type			*proj_setup;

		// setup ray trace

	spt.x=proj->pnt.x;
	spt.y=proj->pnt.y;
	spt.z=proj->pnt.z;

	ept.x=spt.x+xadd;
	ept.y=spt.y+yadd;
	ept.z=spt.z+zadd;

	contact.obj.on=TRUE;
	if (proj->parent_grace>0) {
		contact.obj.ignore_uid=proj->obj_idx;
	}
	else {
		contact.obj.ignore_uid=-1;
	}
	
	obj=server.obj_list.objs[proj->obj_idx];
	weap=obj->weap_list.weaps[proj->weap_idx];
	proj_setup=weap->proj_setup_list.proj_setups[proj->proj_setup_idx];

	contact.proj.on=proj_setup->collision;
	contact.proj.ignore_uid=proj->idx;

	contact.hit_mode=poly_ray_trace_hit_mode_all;
	contact.origin=poly_ray_trace_origin_projectile;

		// run trace

	if (!ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) {
		proj->pnt.x=ept.x;
		proj->pnt.y=ept.y;
		proj->pnt.z=ept.z;
		return(FALSE);
	}

		// move to hit point

	proj->pnt.x=hpt.x;
	proj->pnt.y=hpt.y;
	proj->pnt.z=hpt.z;

		// setup hits

	proj->contact.obj_idx=contact.obj.uid;
	proj->contact.proj_idx=contact.proj.uid;
	memmove(&proj->contact.hit_poly,&contact.poly,sizeof(poly_pointer_type));

	return(TRUE);
}

/* =======================================================

      Polygon APIs
      
======================================================= */

int collide_polygon_find_faced_by_object(obj_type *obj)
{
	int						sz,xadd,zadd;
	d3pnt					spt,ept,hpt;
	ray_trace_contact_type	contact;

		// setup ray trace

	spt.x=obj->pnt.x;
	spt.y=obj->pnt.y-(obj->size.y>>1);
	spt.z=obj->pnt.z;

	sz=obj->size.radius*10;
	angle_get_movement(obj->ang.y,sz,&xadd,&zadd);

	ept.x=spt.x+xadd;
	ept.y=spt.y;
	ept.z=spt.z+zadd;

	contact.obj.on=FALSE;
	contact.proj.on=FALSE;

	contact.hit_mode=poly_ray_trace_hit_mode_wall_only;
	contact.origin=poly_ray_trace_origin_object;

		// run trace

	if (!ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) return(-1);

		// return polygon contact

	if (contact.poly.mesh_idx==-1) return(-1);

	return((contact.poly.mesh_idx<<16)|contact.poly.poly_idx);
}

int collide_polygon_distance_to_object(int poly_uid,obj_type *obj)
{
	int					mesh_idx,poly_idx,xadd,zadd,x,z;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh_idx=poly_uid>>16;
	if ((mesh_idx<0) || (mesh_idx>=map.mesh.nmesh)) return(-1);
	mesh=&map.mesh.meshes[mesh_idx];

	poly_idx=(poly_uid&0xFFFF);
	if ((poly_idx<0) || (poly_idx>=mesh->npoly)) return(-1);
	poly=&mesh->polys[poly_idx];

	angle_get_movement(obj->ang.y,map_max_size,&xadd,&zadd);
	if (!line_2D_get_intersect(obj->pnt.x,obj->pnt.z,(obj->pnt.x+xadd),(obj->pnt.z+zadd),poly->line.lx,poly->line.lz,poly->line.rx,poly->line.rz,&x,&z)) return(-1);

	return(distance_2D_get(obj->pnt.x,obj->pnt.z,x,z));
}

void collide_polygon_hit_point_to_object(int poly_uid,obj_type *obj,d3pnt *pt)
{
	int					mesh_idx,poly_idx,xadd,zadd,x,z;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	pt->x=obj->pnt.x;
	pt->y=obj->pnt.y;
	pt->z=obj->pnt.z;

	mesh_idx=poly_uid>>16;
	if ((mesh_idx<0) || (mesh_idx>=map.mesh.nmesh)) return;
	mesh=&map.mesh.meshes[mesh_idx];

	poly_idx=(poly_uid&0xFFFF);
	if ((poly_idx<0) || (poly_idx>=mesh->npoly)) return;
	poly=&mesh->polys[poly_idx];

	angle_get_movement(obj->ang.y,map_max_size,&xadd,&zadd);
	if (!line_2D_get_intersect(obj->pnt.x,obj->pnt.z,(obj->pnt.x+xadd),(obj->pnt.z+zadd),poly->line.lx,poly->line.lz,poly->line.rx,poly->line.rz,&x,&z)) return;

	pt->x=x;
	pt->z=z;
}

void collide_polygon_get_normal(int poly_uid,d3vct *normal)
{
	int					mesh_idx,poly_idx;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	normal->x=normal->y=normal->z=0.0f;

	mesh_idx=poly_uid>>16;
	if ((mesh_idx<0) || (mesh_idx>=map.mesh.nmesh)) return;
	mesh=&map.mesh.meshes[mesh_idx];

	poly_idx=(poly_uid&0xFFFF);
	if ((poly_idx<0) || (poly_idx>=mesh->npoly)) return;
	poly=&mesh->polys[poly_idx];

	normal->x=poly->tangent_space.normal.x;
	normal->y=poly->tangent_space.normal.y;
	normal->z=poly->tangent_space.normal.z;
}

float collide_polygon_dot_product_to_object(int poly_uid,obj_type *obj)
{
	int					mesh_idx,poly_idx;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh_idx=poly_uid>>16;
	if ((mesh_idx<0) || (mesh_idx>=map.mesh.nmesh)) return(0.0f);
	mesh=&map.mesh.meshes[mesh_idx];

	poly_idx=(poly_uid&0xFFFF);
	if ((poly_idx<0) || (poly_idx>=mesh->npoly)) return(0.0f);
	poly=&mesh->polys[poly_idx];

	return(((poly->tangent_space.normal.x*(float)(poly->box.mid.x-obj->pnt.x))+(poly->tangent_space.normal.y*(float)(poly->box.mid.y-obj->pnt.y))+(poly->tangent_space.normal.z*(float)(poly->box.mid.z-obj->pnt.z))));
}

