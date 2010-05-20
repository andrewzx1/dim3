/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Collision Object Routines

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

#include "objects.h"
#include "models.h"
#include "physics.h"

extern map_type			map;
extern server_type		server;

/* =======================================================

      Colliding Object to Object
      
======================================================= */

void collide_object_polygon(obj_type *obj,int x_add,int z_add,int *px,int *pz)
{
	int			x,z,sz;
	float		rang;

		// position

	x=obj->pnt.x+x_add;
	z=obj->pnt.z+z_add;

		// sizes

	sz=obj->size.x>>1;

	px[0]=px[3]=x-sz;
	px[1]=px[2]=x+sz;
	
	sz=obj->size.z>>1;

	pz[0]=pz[1]=z-sz;
	pz[2]=pz[3]=z+sz;

		// rotate

	rang=angle_add(obj->ang.y,obj->draw.rot.y);
	rotate_2D_polygon(4,px,pz,x,z,rang);
}

void collide_object_hit_box_polygon(obj_type *obj,model_hit_box_type *hit_box,int *px,int *pz)
{
	int				x,z,x2,z2,wid;
	float			rang;

	x=obj->pnt.x+obj->draw.offset.x;
	z=obj->pnt.z+obj->draw.offset.z;

	x2=x+hit_box->box.offset.x;
	wid=hit_box->box.size.x>>1;
	px[0]=px[3]=x2-wid;
	px[1]=px[2]=x2+wid;
	
	z2=z+hit_box->box.offset.z;
	wid=hit_box->box.size.z>>1;
	pz[0]=pz[1]=z2-wid;
	pz[2]=pz[3]=z2+wid;
	
	rang=angle_add(obj->ang.y,obj->draw.rot.y);
	rotate_2D_polygon(4,px,pz,x,z,rang);
}

bool collide_object_to_object(obj_type *obj1,int x_add,int z_add,obj_type *obj2,bool include_y,bool include_stand)
{
	int		x,z,ls1,ms1,ls2,ms2,d,radius_1,radius_2;
	int		px_1[4],pz_1[4],px_2[4],pz_2[4];

		// check largest bounding circle first

	radius_1=obj1->size.x;
	if (obj1->size.z>radius_1) radius_1=obj1->size.z;

	radius_2=obj2->size.x;
	if (obj2->size.z>radius_2) radius_2=obj2->size.z;

	x=(obj1->pnt.x+x_add)-obj2->pnt.x;
	z=(obj1->pnt.z+z_add)-obj2->pnt.z;
	
	d=(int)sqrtf((float)((x*x)+(z*z)));
	if (d>(radius_1+radius_2)) return(FALSE);

		// check y
		
	if (include_y) {
		ms1=obj1->pnt.y;
		ls1=ms1-obj1->size.y;
		ms2=obj2->pnt.y;
		ls2=ms2-obj2->size.y;
		
		if (!include_stand) {
			if (ms1<=ls2) return(FALSE);
			if (ls1>=ms2) return(FALSE);
		}
		else {
			if (ms1<ls2) return(FALSE);
			if (ls1>=ms2) return(FALSE);
		}
	}

		// finally do polygonal collisions
		// use on vertex inside another polygon method

	collide_object_polygon(obj1,x_add,z_add,px_1,pz_1);
	polygon_2D_collision_setup(4,px_1,pz_1);

	collide_object_polygon(obj2,0,0,px_2,pz_2);
	return(polygon_2D_collision_polygon(4,px_2,pz_2));
}

bool collide_object_to_object_hit_box(obj_type *obj1,int x_add,int z_add,obj_type *obj2,model_hit_box_type *hit_box)
{
	int		x,z,ls1,ms1,ls2,ms2,d,radius_1,radius_2;
	int		px[4],pz[4];

		// check largest bounding circle first

	radius_1=obj1->size.x;
	if (obj1->size.z>radius_1) radius_1=obj1->size.z;

	radius_2=hit_box->box.size.x;
	if (hit_box->box.size.z>radius_2) radius_2=hit_box->box.size.z;

	x=(obj1->pnt.x+x_add)-(obj2->pnt.x+obj2->draw.offset.x+hit_box->box.offset.x);
	z=(obj1->pnt.z+z_add)-(obj2->pnt.z+obj2->draw.offset.z+hit_box->box.offset.z);
	
	d=(int)sqrtf((float)((x*x)+(z*z)));
	if (d>(radius_1+radius_2)) return(FALSE);

		// check y
		
	ms1=obj1->pnt.y;
	ls1=ms1-obj1->size.y;
	ms2=obj2->pnt.y+obj2->draw.offset.y+hit_box->box.offset.y;
	ls2=ms2-hit_box->box.size.y;
		
	if (ms1<=ls2) return(FALSE);
	if (ls1>=ms2) return(FALSE);

		// finally do polygonal collisions
		// use on vertex inside another polygon method

	collide_object_polygon(obj1,x_add,z_add,px,pz);
	polygon_2D_collision_setup(4,px,pz);

	collide_object_hit_box_polygon(obj2,hit_box,px,pz);
	return(polygon_2D_collision_polygon(4,px,pz));
}

bool collide_set_object_hit_box_for_object_hit(obj_type *obj,int x,int z,obj_type *check_obj)
{
	int					n,nhit_box;
	model_draw			*draw;
	model_type			*model;
	model_hit_box_type	*hit_box;

		// no hit boxes hit

	obj->hit_box.obj_hit_box_idx=-1;
	
		// check model
		
	draw=&check_obj->draw;
	if ((draw->model_idx==-1) || (!draw->on)) return(FALSE);
	
	model=&server.models[draw->model_idx];
	
		// check hit boxes
		// unlike projectiles, we can just take the
		// first hit off the first hitbox
		
	hit_box=model->hit_boxes;
	nhit_box=model->nhit_box;
	
	for (n=0;n!=nhit_box;n++) {
		if (collide_object_to_object_hit_box(obj,x,z,check_obj,hit_box)) {
			obj->hit_box.obj_hit_box_idx=n;
			return(TRUE);
		}
		hit_box++;
	}

	return(FALSE);
}

int collide_find_object_for_object_move(obj_type *obj,int x,int z)
{
	int				n,ignore_obj_uid;
	obj_type	 	*check_obj;
	
	ignore_obj_uid=obj->uid;
	
	check_obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {
	
		if ((check_obj->hidden) || (!check_obj->contact.object_on) || (check_obj->pickup.on) || (check_obj->uid==ignore_obj_uid)) {
			check_obj++;
			continue;
		}

			// object hit?
			
		if (!collide_object_to_object(obj,x,z,check_obj,TRUE,FALSE)) {
			check_obj++;
			continue;
		}
		
			// which hit box?
		
		if (!check_obj->hit_box.on) return(check_obj->uid);
		if (collide_set_object_hit_box_for_object_hit(obj,x,z,check_obj)) return(check_obj->uid);
		
		check_obj++;
	}
	
	return(-1);
}

inline int collide_find_object_for_object(obj_type *obj)
{
	return(collide_find_object_for_object_move(obj,0,0));
}

/* =======================================================

      Standing Objects
      
======================================================= */

int collide_find_object_for_standing_object(obj_type *obj)
{
	int			n,uid,y,ty,ydist,ignore_obj_uid;
	obj_type	*stand_obj;
	
	y=obj->pnt.y;
	ydist=floor_slop;
	uid=-1;
	
	ignore_obj_uid=obj->uid;
	
	stand_obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {
	
		if ((stand_obj->uid==ignore_obj_uid) || (stand_obj->hidden) || (!stand_obj->contact.object_on) || (stand_obj->pickup.on)) {
			stand_obj++;
			continue;
		}
		
		ty=abs(y-(stand_obj->pnt.y-stand_obj->size.y));
		if (ty>ydist) {
			stand_obj++;
			continue;
		}

		if (collide_object_to_object(obj,0,0,stand_obj,FALSE,FALSE)) {
			uid=stand_obj->uid;
			ydist=ty;
		}
		
		stand_obj++;
	}
	
	return(uid);	
}

/* =======================================================

      Get Object to Object Collision Slide Line
      
======================================================= */

bool collide_object_to_object_get_slide_line(obj_type *obj,int hit_face,int *p_lx,int *p_rx,int *p_lz,int *p_rz)
{
	int			x,z,lx,rx,tz,bz,sz,px[2],pz[2];
	float		rang;

	sz=obj->size.x>>1;
	x=obj->pnt.x;
	lx=x-sz;
	rx=x+sz;
	
	sz=obj->size.z>>1;
	z=obj->pnt.z;
	tz=z-sz;
	bz=z+sz;
		
	rang=angle_add(obj->ang.y,obj->draw.rot.y);

	switch (hit_face) {

		case ray_trace_face_hit_neg_z:
			px[0]=lx;
			px[1]=rx;
			pz[0]=pz[1]=tz;
			break;

		case ray_trace_face_hit_pos_z:
			px[0]=lx;
			px[1]=rx;
			pz[0]=pz[1]=bz;
			break;
	
		case ray_trace_face_hit_neg_x:
			px[0]=px[1]=lx;
			pz[0]=tz;
			pz[1]=bz;
			break;
	
		case ray_trace_face_hit_pos_x:
			px[0]=px[1]=rx;
			pz[0]=tz;
			pz[1]=bz;
			break;
	
		case ray_trace_face_hit_neg_y:
		case ray_trace_face_hit_pos_y:
			return(FALSE);

	}

	rotate_2D_polygon(2,px,pz,x,z,rang);

	*p_lx=px[0];
	*p_rx=px[1];
	*p_lz=pz[0];
	*p_rz=pz[1];

	return(TRUE);
}

bool collide_object_to_hit_box_get_slide_line(obj_type *obj,int hit_face,model_hit_box_type *hit_box,int *p_lx,int *p_rx,int *p_lz,int *p_rz)
{
	int			x,z,lx,rx,tz,bz,sz,px[2],pz[2];
	float		rang;

	sz=hit_box->box.size.x>>1;
	x=obj->pnt.x+hit_box->box.offset.x;
	lx=x-sz;
	rx=x+sz;
	
	sz=hit_box->box.size.z>>1;
	z=obj->pnt.z+hit_box->box.offset.z;
	tz=z-sz;
	bz=z+sz;
		
	rang=angle_add(obj->ang.y,obj->draw.rot.y);

	switch (hit_face) {

		case ray_trace_face_hit_neg_z:
			px[0]=lx;
			px[1]=rx;
			pz[0]=pz[1]=tz;
			break;

		case ray_trace_face_hit_pos_z:
			px[0]=lx;
			px[1]=rx;
			pz[0]=pz[1]=bz;
			break;
	
		case ray_trace_face_hit_neg_x:
			px[0]=px[1]=lx;
			pz[0]=tz;
			pz[1]=bz;
			break;
	
		case ray_trace_face_hit_pos_x:
			px[0]=px[1]=rx;
			pz[0]=tz;
			pz[1]=bz;
			break;
	
		case ray_trace_face_hit_neg_y:
		case ray_trace_face_hit_pos_y:
			return(FALSE);

	}

	rotate_2D_polygon(2,px,pz,x,z,rang);

	*p_lx=px[0];
	*p_rx=px[1];
	*p_lz=pz[0];
	*p_rz=pz[1];

	return(TRUE);
}

/* =======================================================

      Colliding Sphere to Object
      
======================================================= */

bool collide_set_object_hit_box_for_sphere_hit(int sx,int sy,int sz,int radius,obj_type *obj)
{
	int					n,nhit_box,
						dist,d,x,y,z;
	model_draw			*draw;
	model_type			*model;
	model_hit_box_type	*hit_box;
	
		// tag no hit box hits
		
	obj->hit_box.obj_hit_box_idx=-1;
	
		// find hit box middle closest to damage box middle
	
	dist=-1;
	
		// check model
		
	draw=&obj->draw;
	if ((draw->model_idx==-1) || (!draw->on)) return(FALSE);
	
	model=&server.models[draw->model_idx];
	
		// check hit boxes
		
	hit_box=model->hit_boxes;
	nhit_box=model->nhit_box;
	
	for (n=0;n!=nhit_box;n++) {
	
			// in this hit box?
			
		if (!collide_sphere_to_object_hit_box(sx,sy,sz,radius,obj,hit_box)) {
			hit_box++;
			continue;
		}
		
			// get the distance
	
		x=obj->pnt.x+draw->offset.x+hit_box->box.offset.x;
		y=obj->pnt.y+draw->offset.y+hit_box->box.offset.y;
		z=obj->pnt.z+draw->offset.z+hit_box->box.offset.z;
	
		d=distance_get(sx,sy,sz,x,y,z);
				
		if ((d<dist) || (dist==-1)) {
			dist=d;
			obj->hit_box.obj_hit_box_idx=n;
		}
		
		hit_box++;
	}

	return(obj->hit_box.obj_hit_box_idx!=-1);
}

bool collide_sphere_to_object(int sx,int sy,int sz,int radius,obj_type *obj)
{
	int		x,z,ls1,ms1,ls2,ms2,d;
	
		// check x
		
	ls1=sx-radius;
	ms1=sx+radius;
	
	x=obj->pnt.x;
	ls2=x-obj->size.radius;
	ms2=x+obj->size.radius;
		
	if (ms1<ls2) return(FALSE);
	if (ls1>ms2) return(FALSE);
	
		// check z
		
	ls1=sz-radius;
	ms1=sz+radius;
	
	z=obj->pnt.z;
	ls2=z-obj->size.radius;
	ms2=z+obj->size.radius;
		
	if (ms1<ls2) return(FALSE);
	if (ls1>ms2) return(FALSE);
	
		// check y
		
	ls1=sy-radius;
	ms1=sy+radius;
	ms2=obj->pnt.y;
	ls2=ms2-obj->size.y;
	
	if (ms1<=ls2) return(FALSE);
	if (ls1>=ms2) return(FALSE);
	
		// check radius
		
	x=sx-obj->pnt.x;
	z=sz-obj->pnt.z;
	
	d=(int)sqrtf((float)((x*x)+(z*z)));
	return(d<=(radius+obj->size.radius));
}

bool collide_sphere_to_object_hit_box(int sx,int sy,int sz,int radius,obj_type *obj,model_hit_box_type *hit_box)
{
	int				x,z,wid,ls1,ms1,ls2,ms2,
					px[4],pz[4];
	float			rang;
	
		// check y
		
	ls1=sy-radius;
	ms1=sy+radius;
	ms2=obj->pnt.y+obj->draw.offset.y+hit_box->box.offset.y;
	ls2=ms2-hit_box->box.size.y;
	
	if (ms1<=ls2) return(FALSE);
	if (ls1>=ms2) return(FALSE);
	
		// 2D x/z polygon to polygon collision

	px[0]=px[3]=sx-radius;
	px[1]=px[2]=sx+radius;
	
	pz[0]=pz[1]=sz-radius;
	pz[2]=pz[3]=sz+radius;
	
	polygon_2D_collision_setup(4,px,pz);
	
	x=obj->pnt.x+obj->draw.offset.x+hit_box->box.offset.x;
	wid=hit_box->box.size.x>>1;
	px[0]=px[3]=x-wid;
	px[1]=px[2]=x+wid;
	
	z=obj->pnt.z+obj->draw.offset.z+hit_box->box.offset.z;
	wid=hit_box->box.size.z>>1;
	pz[0]=pz[1]=z-wid;
	pz[2]=pz[3]=z+wid;
	
	x=obj->pnt.x+obj->draw.offset.x;
	z=obj->pnt.z+obj->draw.offset.z;
	rang=angle_add(obj->ang.y,obj->draw.rot.y);
	
	rotate_2D_polygon(4,px,pz,x,z,rang);
	return(polygon_2D_collision_polygon(4,px,pz));
}

/* =======================================================

      Push Objects
      
======================================================= */

void collide_push_objects(int sx,int sy,int sz,int radius,int force)
{
	int			n,yhit;
	d3ang		ang;
	obj_type	*obj;
	
		// check objects
		
	obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {

		if ((obj->hidden) || (obj->suspend) || (!obj->contact.object_on)) {
			obj++;
			continue;
		}
		
			// check for collision
			
		if (!collide_sphere_to_object(sx,sy,sz,radius,obj)) {
			obj++;
			continue;
		}
		
			// add push
				
		yhit=obj->pnt.y-(obj->size.y/2);
		
		ang.x=angle_find(sy,sz,yhit,obj->pnt.z);
		ang.y=angle_find(sx,sz,obj->pnt.x,obj->pnt.z);
		ang.z=0;
		
		object_push(obj,&ang,force,FALSE);
		
		obj++;
	}
}
