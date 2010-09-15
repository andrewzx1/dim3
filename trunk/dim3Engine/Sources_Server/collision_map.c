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


/*

supergumba -- deal with these later

bool circle_line_intersect(d3pnt *p1,d3pnt *p2,d3pnt *circle_pt,int radius,d3pnt *hit_pt)
{
	double		v_x,v_y,v_z,line_a,line_b,line_c,
				bb4ac,sqr_bb4ac,in_1,in_2,
				dx,dy,dz,hdist_1,hdist_2;
	d3pnt		hpt_1,hpt_2;

		// line vector
		
	v_x=(double)(p2->x-p1->x);
	v_y=(double)(p2->y-p1->y);
	v_z=(double)(p2->z-p1->z);

		// calc intersection
		
	line_a=(v_x*v_x)+(v_y*v_y)+(v_z*v_z);
	line_b=2.0*((v_x*(double)(p1->x-circle_pt->x))+(v_y*(double)(p1->y-circle_pt->y))+(v_z*(double)(p1->z-circle_pt->z)));
	line_c=(double)((circle_pt->x*circle_pt->x)+(circle_pt->y*circle_pt->y)+(circle_pt->z*circle_pt->z));
	line_c+=(double)((p1->x*p1->x)+(p1->y*p1->y)+(p1->z*p1->z));
	line_c-=2.0*(double)((circle_pt->x*p1->x)+(circle_pt->y*p1->y)+(circle_pt->z*p1->z));
	line_c-=(double)(radius*radius);
	
	bb4ac=(line_b*line_b)-((4*line_a)*line_c);

		// no intersections
		
	if (bb4ac<0.0f) return(FALSE);
	
		// on intersection
		
	if (bb4ac==0.0) {
		in_1=(-line_b)/(2.0*line_a);
		hit_pt->x=p1->x+(int)(v_x*in_1);
		hit_pt->y=p1->y+(int)(v_y*in_1);
		hit_pt->z=p1->z+(int)(v_z*in_1);
		return(TRUE);
	}
	
		// two intersections
		// pick the one closest

	sqr_bb4ac=sqrt(bb4ac);
	in_1=((-line_b)+sqr_bb4ac)/(2.0f*line_a);
	in_2=((-line_b)-sqr_bb4ac)/(2.0f*line_a);
	
		// get the 2 hit points
	
	hpt_1.x=p1->x+(int)(v_x*in_1);
	hpt_1.y=p1->y+(int)(v_y*in_1);
	hpt_1.z=p1->z+(int)(v_z*in_1);
	
	hpt_2.x=p1->x+(int)(v_x*in_2);
	hpt_2.y=p1->y+(int)(v_y*in_2);
	hpt_2.z=p1->z+(int)(v_z*in_2);
	
		// determine closest point

	dx=(double)(hpt_1.x-circle_pt->x);
	dy=(double)(hpt_1.y-circle_pt->y);
	dz=(double)(hpt_1.z-circle_pt->z);
		
	hdist_1=(dx*dx)+(dy*dy)+(dz*dz);
	
	dx=(double)(hpt_2.x-circle_pt->x);
	dy=(double)(hpt_2.y-circle_pt->y);
	dz=(double)(hpt_2.z-circle_pt->z);
	
	hdist_2=(dx*dx)+(dy*dy)+(dz*dz);

	if (hdist_1<hdist_2) {
		hit_pt->x=hpt_1.x;
		hit_pt->y=hpt_1.y;
		hit_pt->z=hpt_1.z;
	}
	else {
		hit_pt->x=hpt_2.x;
		hit_pt->y=hpt_2.y;
		hit_pt->z=hpt_2.z;
	}

	return(TRUE);
}


*/




int circle_line_intersect(d3pnt *p1,d3pnt *p2,d3pnt *circle_pnt,int radius,d3pnt *hit_pnt)
{
	double			d,dx,dz;
	float			fr,fx,fz;
	d3pnt			cp2;
	
		// get the normalized line vector
		
	dx=(double)(p2->x-p1->x);
	dz=(double)(p2->z-p1->z);

	d=sqrt((dx*dx)+(dz*dz));
	if (d!=0.0) {
		d=1.0/d;
		fx=(float)(dx*d);
		fz=(float)(dz*d);
	}
	
		// reverse to get perpendicular
		// line from circle center
		// we need to test both directions
		
	fr=(float)radius;

	cp2.x=circle_pnt->x-(int)(fr*fz);
	cp2.z=circle_pnt->z+(int)(fr*fx);
		
	if (!line_2D_get_intersect(p1->x,p1->z,p2->x,p2->z,circle_pnt->x,circle_pnt->z,cp2.x,cp2.z,&hit_pnt->x,&hit_pnt->z)) {
		cp2.x=circle_pnt->x+(int)(fr*fz);
		cp2.z=circle_pnt->z-(int)(fr*fx);

		if (!line_2D_get_intersect(p1->x,p1->z,p2->x,p2->z,circle_pnt->x,circle_pnt->z,cp2.x,cp2.z,&hit_pnt->x,&hit_pnt->z)) return(-1);
	}

		// return distance to hit point

	dx=(hit_pnt->x-circle_pnt->x);
	dz=(hit_pnt->z-circle_pnt->z);
	
	return((int)sqrt((dx*dx)+(dz*dz)));
}

void circle_get_point_on_radius_through_hit_point(d3pnt *circle_pnt,int radius,d3pnt *hit_pnt,d3pnt *radius_pt)
{
	double			d,dx,dz;
	float			fr,fx,fz;

		// get the normalized line vector
		
	dx=(double)(hit_pnt->x-circle_pnt->x);
	dz=(double)(hit_pnt->z-circle_pnt->z);

	d=sqrt((dx*dx)+(dz*dz));
	if (d!=0.0) {
		d=1.0/d;
		fx=(float)(dx*d);
		fz=(float)(dz*d);
	}

		// get point on radius
		// that line hits

	fr=(float)radius;

	radius_pt->x=circle_pnt->x+(int)(fr*fx);
	radius_pt->z=circle_pnt->z+(int)(fr*fz);
}







inline int collide_circle_check_object(d3pnt *circle_pnt,int radius,d3pnt *min,d3pnt *max,int *p_cur_dist,obj_type *obj,d3pnt *cur_hit_pnt)
{
	return(FALSE);
/*
	int			x,z,radius;
	
		// object a hit candidate?

	if ((obj->hidden) || (obj->pickup.on) || (obj->idx==contact->obj.ignore_idx)) return(FALSE);
	if (((contact->origin==poly_ray_trace_origin_object) && (!obj->contact.object_on)) || ((contact->origin==poly_ray_trace_origin_projectile) && (!obj->contact.projectile_on))) return(FALSE);
	
		// rough y vector box check
		
	if (max->y<(obj->pnt.y-obj->size.y)) return(FALSE);
	if (min->y>obj->pnt.y) return(FALSE);
	
		// rough x/z vector box checks
		
	x=obj->pnt.x;
	z=obj->pnt.z;

	radius=obj->size.x;
	if (obj->size.z>radius) radius=obj->size.z;
	
	radius=radius>>1;
	
	if (max->x<(x-radius)) return(FALSE);
	if (min->x>(x+radius)) return(FALSE);
	if (max->z<(z-radius)) return(FALSE);
	if (min->z>(z+radius)) return(FALSE);
	
	return(TRUE);
*/
}



inline bool collide_circle_check_mesh(d3pnt *circle_pnt,int radius,d3pnt *min,d3pnt *max,int *p_cur_dist,int mesh_idx,int *idx,d3pnt *cur_hit_pnt)
{
	int					n,y,dist,poly_count,cur_dist;
	short				*poly_idx;
	double				dx,dz,hit_dist,tot_dist;
	bool				hit;
	d3pnt				p1,p2,hit_pnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	
	mesh=&map.mesh.meshes[mesh_idx];
	
		// check mesh bounds

	if ((mesh->box.min.x>=max->x) || (mesh->box.max.x<=min->x)) return(FALSE);
	if ((mesh->box.min.y>=max->y) || (mesh->box.max.y<=min->y)) return(FALSE);
	if ((mesh->box.min.z>=max->z) || (mesh->box.max.z<=min->z)) return(FALSE);

		// check wall polys
					
	poly_count=mesh->poly_list.wall_count;
	poly_idx=mesh->poly_list.wall_idxs;
	
	hit=FALSE;
	cur_dist=*p_cur_dist;
	
	for (n=0;n!=poly_count;n++) {

		poly=&mesh->polys[poly_idx[n]];
			
			// check poly bounds

		if ((poly->box.min.x>=max->x) || (poly->box.max.x<=min->x)) continue;
		if ((poly->box.min.y>=max->y) || (poly->box.max.y<=min->y)) continue;
		if ((poly->box.min.z>=max->z) || (poly->box.max.z<=min->z)) continue;

			// collide with line

		p1.x=poly->line.lx;
		p1.z=poly->line.lz;

		p2.x=poly->line.rx;
		p2.z=poly->line.rz;

		dist=circle_line_intersect(&p1,&p2,circle_pnt,radius,&hit_pnt);
		if (dist==-1) continue;
		if (dist>radius) continue;
				
			// deal with the slants
			// against y collisions
			
		if (poly->line.l_ty!=poly->line.r_ty) {
			dx=(double)(hit_pnt.x-poly->line.lx);
			dz=(double)(hit_pnt.z-poly->line.lz);
			hit_dist=sqrt((dx*dx)+(dz*dz));
			
			dx=(double)(poly->line.rx-poly->line.lx);
			dz=(double)(poly->line.rz-poly->line.lz);
			tot_dist=sqrt((dx*dx)+(dz*dz));
			
			if ((tot_dist!=0.0) && (hit_dist!=0.0)) {
				y=poly->line.l_ty+(int)((double)(poly->line.r_ty-poly->line.l_ty)*(hit_dist/tot_dist));
				if (y>=circle_pnt->y) continue;
			}
		}

			// is this the next nearest hit?

		if ((dist<cur_dist) || (cur_dist==-1)) {
			*idx=poly_idx[n];

			cur_dist=dist;
			cur_hit_pnt->x=hit_pnt.x;
			cur_hit_pnt->z=hit_pnt.z;
			
			hit=TRUE;
		}
	}
	
		// return new cur dist
		
	*p_cur_dist=cur_dist;
	
	return(hit);
}


// supergumba -- rework

// TODO
// 1.5 figure out sliding and true/false, always use TRUE for hit
// 2. y checks
// 2.5 add object, projectiles, etc
// 3. make it universal
// 4. move intersect to utility?

// supergumba -- can get rid of obj_contact->face? and other stuff?

bool collide_object_box_to_map(obj_type *obj,d3pnt *pt,d3pnt *box_sz,int *xadd,int *yadd,int *zadd)
{
	int						n,radius,cur_dist,idx;
	d3pnt					circle_pnt,
							cur_hit_pnt,radius_pnt,
							min,max;
	obj_type				*chk_obj;

		// get the circle radius

	radius=box_sz->x;
	if (box_sz->z>radius) radius=box_sz->z;

	radius=radius>>1;

		// get the moved circle

	circle_pnt.x=pt->x+(*xadd);
	circle_pnt.y=pt->y;
	circle_pnt.z=pt->z+(*zadd);

		// find min and max for object

	min.x=circle_pnt.x-radius;
	max.x=circle_pnt.x+radius;
	min.z=circle_pnt.z-radius;
	max.z=circle_pnt.z+radius;

	min.y=pt->y-box_sz->y;
	max.y=pt->y;

		// collide against map

	cur_dist=-1;
	
	obj->contact.obj_idx=-1;
	obj->contact.proj_idx=-1;
	obj->contact.hit_poly.mesh_idx=-1;
	
		// check objects

	for (n=0;n!=max_obj_list;n++) {
		chk_obj=server.obj_list.objs[n];
		if (chk_obj==NULL) continue;
/*
		if (collide_circle_check_object(&circle_pnt,radius,&min,&max,&cur_dist,chk_obj,&cur_hit_pnt)) {
			obj->contact.obj_idx=n;
		}
		*/
	}

		// check meshes

	for (n=0;n!=map.mesh.nmesh;n++) {
		if (collide_circle_check_mesh(&circle_pnt,radius,&min,&max,&cur_dist,n,&idx,&cur_hit_pnt)) {
			obj->contact.obj_idx=-1;
			obj->contact.proj_idx=-1;
			obj->contact.hit_poly.mesh_idx=n;
			obj->contact.hit_poly.poly_idx=idx;
		}
	}

		// any hits?

	if (cur_dist!=-1) {
		circle_get_point_on_radius_through_hit_point(&circle_pnt,radius,&cur_hit_pnt,&radius_pnt);
	
		*xadd-=radius_pnt.x-cur_hit_pnt.x;
		*zadd-=radius_pnt.z-cur_hit_pnt.z;

		return((*xadd==0)&&(*zadd==0));		// supergumba -- clear up all this return stuff
	}

	return(FALSE);
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
		contact.obj.ignore_idx=proj->obj_idx;
	}
	else {
		contact.obj.ignore_idx=-1;
	}
	
	obj=server.obj_list.objs[proj->obj_idx];
	weap=obj->weap_list.weaps[proj->weap_idx];
	proj_setup=weap->proj_setup_list.proj_setups[proj->proj_setup_idx];

	contact.proj.on=proj_setup->collision;
	contact.proj.ignore_idx=proj->idx;

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

	proj->contact.obj_idx=contact.obj.idx;
	proj->contact.proj_idx=contact.proj.idx;
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

