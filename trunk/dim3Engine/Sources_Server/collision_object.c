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

// supergumba -- go over all these to find what to delete

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
	
	model=server.model_list.models[draw->model_idx];
	
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
	int				n,ignore_obj_idx;
	obj_type	 	*check_obj;
	
	ignore_obj_idx=obj->idx;
	
	for (n=0;n!=max_obj_list;n++) {
		check_obj=server.obj_list.objs[n];
		if (check_obj==NULL) continue;
	
		if ((check_obj->hidden) || (!check_obj->contact.object_on) || (check_obj->pickup.on) || (check_obj->idx==ignore_obj_idx)) continue;

			// object hit?
			
		if (!collide_object_to_object(obj,x,z,check_obj,TRUE,FALSE)) continue;
		
			// which hit box?
		
		if (!check_obj->hit_box.on) return(check_obj->idx);
		if (collide_set_object_hit_box_for_object_hit(obj,x,z,check_obj)) return(check_obj->idx);
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
	int			n,uid,y,ty,ydist,ignore_obj_idx;
	obj_type	*stand_obj;
	
	y=obj->pnt.y;
	ydist=floor_slop;
	uid=-1;
	
	ignore_obj_idx=obj->idx;
	
	for (n=0;n!=max_obj_list;n++) {
		stand_obj=server.obj_list.objs[n];
		if (stand_obj==NULL) continue;
	
		if ((stand_obj->idx==ignore_obj_idx) || (stand_obj->hidden) || (!stand_obj->contact.object_on) || (stand_obj->pickup.on)) continue;
		
		ty=abs(y-(stand_obj->pnt.y-stand_obj->size.y));
		if (ty>ydist) continue;

		if (collide_object_to_object(obj,0,0,stand_obj,FALSE,FALSE)) {
			uid=stand_obj->idx;
			ydist=ty;
		}
	}
	
	return(uid);	
}

