/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Collision Projectile Routines

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

#include "models.h"
#include "physics.h"

extern map_type			map;
extern server_type		server;

/* =======================================================

      Colliding Projectile to Object
      
======================================================= */

void collide_projectile_polygon(proj_type *proj,int *px,int *pz)
{
	int			x,z,sz;
	float		rang;

	x=proj->pnt.x;
	sz=proj->size.x>>1;

	px[0]=px[3]=x-sz;
	px[1]=px[2]=x+sz;
	
	z=proj->pnt.z;
	sz=proj->size.z>>1;

	pz[0]=pz[1]=z-sz;
	pz[2]=pz[3]=z+sz;

	rang=angle_add(proj->ang.y,proj->draw.rot.y);
	rotate_2D_polygon(4,px,pz,x,z,rang);
}

bool collide_projectile_to_object(proj_type *proj,obj_type *obj)
{
	int		x,z,ls1,ms1,ls2,ms2,d,
			radius,px[4],pz[4];
	
		// check largest bounding circle first

	radius=obj->size.x;
	if (obj->size.z>radius) radius=obj->size.z;

	x=obj->pnt.x-proj->pnt.x;
	z=obj->pnt.z-proj->pnt.z;
	
	d=(int)sqrtf((float)((x*x)+(z*z)));
	if (d>(radius+proj->size.radius)) return(FALSE);
	
		// check y
		
	ms1=proj->pnt.y;
	ls1=ms1-proj->size.y;
	ms2=obj->pnt.y;
	ls2=ms2-obj->size.y;
	
	if (ms1<=ls2) return(FALSE);
	if (ls1>=ms2) return(FALSE);

		// finally do polygonal collisions
		// use on vertex inside another polygon method

	collide_projectile_polygon(proj,px,pz);
	polygon_2D_collision_setup(4,px,pz);

	collide_object_polygon(obj,0,0,px,pz);
	return(polygon_2D_collision_polygon(4,px,pz));
}

bool collide_projectile_to_object_hit_box(proj_type *proj,obj_type *obj,model_hit_box_type *hit_box)
{
	int					ls1,ms1,ls2,ms2,
						px[4],pz[4];
	
		// check y
		
	ms1=proj->pnt.y;
	ls1=ms1-proj->size.y;
	ms2=obj->pnt.y+obj->draw.offset.y+hit_box->box.offset.y;
	ls2=ms2-hit_box->box.size.y;
	
	if (ms1<=ls2) return(FALSE);
	if (ls1>=ms2) return(FALSE);
	
		// 2D x/z polygon to polygon collision

	collide_projectile_polygon(proj,px,pz);
	polygon_2D_collision_setup(4,px,pz);
	
	collide_object_hit_box_polygon(obj,hit_box,px,pz);
	return(polygon_2D_collision_polygon(4,px,pz));
}

bool collide_set_object_hit_box_for_projectile_hit(proj_type *proj,obj_type *obj)
{
	int					n,nhit_box,
						dist,d,x,y,z;
	model_draw			*draw;
	model_type			*model;
	model_hit_box_type	*hit_box;
	
		// tag no hit box hits
		
	obj->hit_box.proj_hit_box_idx=-1;
	
		// find hit box middle closest to damage box middle
	
	dist=-1;
	
		// check model
		
	draw=&obj->draw;
	if ((draw->model_idx==-1) || (!draw->on)) return(FALSE);
	
	model=server.model_list.models[draw->model_idx];
	
		// check hit boxes
		
	hit_box=model->hit_boxes;
	nhit_box=model->nhit_box;
	
	for (n=0;n!=nhit_box;n++) {
	
			// in this hit box?
			
		if (!collide_projectile_to_object_hit_box(proj,obj,hit_box)) {
			hit_box++;
			continue;
		}
		
			// get the distance
	
		x=obj->pnt.x+draw->offset.x+hit_box->box.offset.x;
		y=obj->pnt.y+draw->offset.y+hit_box->box.offset.y;
		z=obj->pnt.z+draw->offset.z+hit_box->box.offset.z;
	
		d=distance_get(proj->pnt.x,proj->pnt.y,proj->pnt.z,x,y,z);
				
		if ((d<dist) || (dist==-1)) {
			dist=d;
			obj->hit_box.proj_hit_box_idx=n;
		}
		
		hit_box++;
	}

	return(obj->hit_box.proj_hit_box_idx!=-1);
}

int collide_find_object_for_projectile_hit(proj_type *proj,int ignore_obj_uid)
{
	int			n;
	obj_type	*obj;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if ((obj->hidden) || (!obj->contact.projectile_on) || (obj->pickup.on) || (obj->idx==ignore_obj_uid)) continue;
		
			// projectile hit?
			
		if (!collide_projectile_to_object(proj,obj)) continue;
		
			// which hit box?
		
		if (!obj->hit_box.on) return(obj->idx);
		if (collide_set_object_hit_box_for_projectile_hit(proj,obj)) return(obj->idx);
	}
	
	return(-1);
}

int collide_find_object_for_projectile(proj_type *proj,int ignore_obj_uid)
{
	int			n;
	obj_type	*obj;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if ((obj->hidden) || (!obj->contact.projectile_on) || (obj->pickup.on) || (obj->idx==ignore_obj_uid)) continue;
		
			// projectile hit?
			
		if (collide_projectile_to_object(proj,obj)) return(obj->idx);
	}
	
	return(-1);
}

/* =======================================================

      Colliding Projectile to Projectile
	  Note: Projectiles are assumed to be small
	        so this just uses circles
      
======================================================= */

bool collide_projectile_to_projectile(proj_type *proj1,proj_type *proj2)
{
	int		x,z,ls1,ms1,ls2,ms2,d;
	
		// check x
		
	x=proj1->pnt.x;
	ls1=x-proj1->size.radius;
	ms1=x+proj1->size.radius;
	
	x=proj2->pnt.x;
	ls2=x-proj2->size.radius;
	ms2=x+proj2->size.radius;
		
	if (ms1<ls2) return(FALSE);
	if (ls1>ms2) return(FALSE);
	
		// check z
		
	z=proj1->pnt.z;
	ls1=z-proj1->size.radius;
	ms1=z+proj1->size.radius;
	
	z=proj2->pnt.z;
	ls2=z-proj2->size.radius;
	ms2=z+proj2->size.radius;
		
	if (ms1<ls2) return(FALSE);
	if (ls1>ms2) return(FALSE);
	
		// check y
		
	ms1=proj1->pnt.y;
	ls1=ms1-proj1->size.y;
	ms2=proj2->pnt.y;
	ls2=ms2-proj2->size.y;
	
	if (ms1<=ls2) return(FALSE);
	if (ls1>=ms2) return(FALSE);
	
		// check radius
		
	x=proj1->pnt.x-proj2->pnt.x;
	z=proj1->pnt.z-proj2->pnt.z;
	
	d=(int)sqrtf((float)((x*x)+(z*z)));
	return(d<=(proj1->size.radius+proj2->size.radius));
}

int collide_find_projectile_for_projectile(proj_type *proj)
{
	int				n,ignore_proj_uid;
	proj_type		*check_proj;
	
	ignore_proj_uid=proj->idx;
	
	for (n=0;n!=max_proj_list;n++) {
		check_proj=server.proj_list.projs[n];
		if (!check_proj->on) continue;

		if (check_proj->idx==ignore_proj_uid) continue;
		
			// projectile hit?
			
		if (collide_projectile_to_projectile(proj,check_proj))  return(check_proj->idx);
	}
	
	return(-1);
}

/* =======================================================

      Colliding Sphere to Projectile
      
======================================================= */

bool collide_sphere_to_projectile(int sx,int sy,int sz,int radius,proj_type *proj)
{
	int		x,z,ls1,ms1,ls2,ms2,d;
	
		// check x
		
	ls1=sx-radius;
	ms1=sx+radius;
	
	x=proj->pnt.x;
	ls2=x-proj->size.radius;
	ms2=x+proj->size.radius;
		
	if (ms1<ls2) return(FALSE);
	if (ls1>ms2) return(FALSE);
	
		// check z
		
	ls1=sz-radius;
	ms1=sz+radius;
	
	z=proj->pnt.z;
	ls2=z-proj->size.radius;
	ms2=z+proj->size.radius;
		
	if (ms1<ls2) return(FALSE);
	if (ls1>ms2) return(FALSE);
	
		// check y's first
		
	ls1=sy-radius;
	ms1=sy+radius;
	ms2=proj->pnt.y;
	ls2=ms2-proj->size.y;
	
	if (ms1<=ls2) return(FALSE);
	if (ls1>=ms2) return(FALSE);
	
		// check radius
		
	x=sx-proj->pnt.x;
	z=sz-proj->pnt.z;
	
	d=(int)sqrtf((float)((x*x)+(z*z)));
	return(d<=(radius+proj->size.radius));
}

