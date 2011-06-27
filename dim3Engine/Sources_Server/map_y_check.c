/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Y Collision Checking

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 /Users/bbarnes/dim3Code/dim3Engine/Sources_Server/map_y_check.c:46: warning: 'n' may be used uninitialized in this function

*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "scripts.h"
#include "objects.h"

#define map_collide_pin_radius_len			100

extern map_type			map;
extern server_type		server;

bool					pin_movement_hits[256];
d3pnt					pin_movement_spt[256],pin_movement_ept[256],pin_movement_hpt[256];
ray_trace_contact_type	pin_movement_contacts[256];

/* =======================================================

      Find Next Nearest Stand Poly Point
      
======================================================= */

int find_poly_nearest_stand(int x,int y,int z,int ydist,bool ignore_higher)
{
	d3pnt					spt,ept,hpt;
	ray_trace_contact_type	contact;

	spt.x=x;
	spt.y=y;
	if (!ignore_higher) spt.y-=ydist;
	spt.z=z;

	ept.x=x;
	ept.y=y+ydist;
	ept.z=z;

	contact.obj.on=FALSE;
	contact.proj.on=FALSE;

	contact.origin=poly_ray_trace_origin_unknown;

	if (ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) return(hpt.y);

	return(-1);
}

/* =======================================================

      Built Pin X/Z rays
      
======================================================= */

int pin_build_ray_set_obj(obj_type *obj,int ty,int by)
{
	int				n,k,x,z,radius,radius_sub,
					circle_count,ray_count;
	double			rad,rad_add,d_radius;

		// get radius

	radius=obj->size.x;
	if (obj->size.z>radius) radius=obj->size.z;

	radius=radius>>1;

		// get center

	x=obj->pnt.x;
	z=obj->pnt.z;

		// how many concentric circles
		// always leave one more spot
		// for center ray (only do at most 15 circles
		// of 16 points each)

	circle_count=radius/map_collide_pin_radius_len;
	if (circle_count<4) circle_count=4;
	if (circle_count>15) circle_count=15;

	radius_sub=radius/circle_count;

		// loop in radians

	rad_add=(D_TRIG_PI*2.0)/16.0;

		// create circles

	ray_count=0;

	for (n=0;n!=circle_count;n++) {

		rad=0.0;
		d_radius=(double)radius;

		for (k=0;k!=16;k++) {
			pin_movement_spt[ray_count].x=pin_movement_ept[ray_count].x=x+(int)(d_radius*sin(rad));
			pin_movement_spt[ray_count].z=pin_movement_ept[ray_count].z=z-(int)(d_radius*cos(rad));
			pin_movement_spt[ray_count].y=ty;
			pin_movement_ept[ray_count].y=by;
			ray_count++;

			rad+=rad_add;
		}

		radius-=radius_sub;
	}

		// always add one for center
	
	pin_movement_spt[ray_count].x=pin_movement_ept[ray_count].x=x;
	pin_movement_spt[ray_count].z=pin_movement_ept[ray_count].z=z;
	pin_movement_spt[ray_count].y=ty;
	pin_movement_ept[ray_count].y=by;
	ray_count++;

	return(ray_count);
}

/* =======================================================

      Pin Downward Movements
      
======================================================= */

int pin_downward_movement_point(int x,int y,int z,int ydist,poly_pointer_type *stand_poly)
{
	d3pnt					spt,ept,hpt;
	ray_trace_contact_type	contact;

	spt.x=x;
	spt.y=y-ydist;
	spt.z=z;

	ept.x=x;
	ept.y=y+ydist;
	ept.z=z;
	
	contact.obj.on=FALSE;
	contact.proj.on=FALSE;

	contact.origin=poly_ray_trace_origin_unknown;

	if (ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) {
		stand_poly->mesh_idx=contact.poly.mesh_idx;
		stand_poly->poly_idx=contact.poly.poly_idx;
		return(hpt.y);
	}

	stand_poly->mesh_idx=-1;

	return(y+ydist);
}

int pin_downward_movement_obj(obj_type *obj,int my)
{
	int						n,cy,ray_count,ty,by;
	ray_trace_contact_type	base_contact;
	
		// setup contact
		
	base_contact.obj.on=TRUE;
	base_contact.obj.ignore_idx=obj->idx;
	base_contact.proj.on=FALSE;

	base_contact.origin=poly_ray_trace_origin_unknown;

		// create ray arrays

	ty=obj->pnt.y-my;
	by=obj->pnt.y+my;
	ray_count=pin_build_ray_set_obj(obj,ty,by);

		// run the rays
		
	ray_trace_map_by_point_array(ray_count,pin_movement_spt,pin_movement_ept,pin_movement_hpt,pin_movement_hits,&base_contact,pin_movement_contacts);
	
		// find the highest point
		
	obj->contact.stand_poly.mesh_idx=-1;
	cy=-1;
	
	for (n=0;n!=ray_count;n++) {
		
			// check poly collisions
			
		if (pin_movement_hits[n]) {
		
			if ((cy==-1) || (pin_movement_hpt[n].y<cy)) {
				obj->contact.stand_poly.mesh_idx=pin_movement_contacts[n].poly.mesh_idx;
				obj->contact.stand_poly.poly_idx=pin_movement_contacts[n].poly.poly_idx;
				cy=pin_movement_hpt[n].y;
			}
			
		}
	}
	
	if (cy==-1) return(my);
	
	return(cy-obj->pnt.y);
}

/* =======================================================

      Pin Upward Movements
      
======================================================= */

int pin_upward_movement_point(int x,int y,int z,int ydist,poly_pointer_type *head_poly)
{
	d3pnt					spt,ept,hpt;
	ray_trace_contact_type	contact;

	spt.x=x;
	spt.y=y;
	spt.z=z;

	ept.x=x;
	ept.y=y-ydist;
	ept.z=z;

	contact.obj.on=FALSE;
	contact.proj.on=FALSE;

	contact.origin=poly_ray_trace_origin_unknown;

	if (ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) {
		head_poly->mesh_idx=contact.poly.mesh_idx;
		head_poly->poly_idx=contact.poly.poly_idx;
		return(hpt.y);
	}

	head_poly->mesh_idx=-1;

	return(y-ydist);
}

int pin_upward_movement_obj(obj_type *obj,int my)
{
	int						n,cy,y_sz,ray_count,ty,by;
	ray_trace_contact_type	base_contact;

		// setup contact
		
	base_contact.obj.on=FALSE;
	base_contact.proj.on=FALSE;

	base_contact.origin=poly_ray_trace_origin_unknown;

		// create ray arrays

	y_sz=obj->size.y;
	if (obj->duck.mode!=dm_stand) y_sz-=obj->duck.y_move;
	by=(obj->pnt.y-y_sz)+my;
	ty=(obj->pnt.y-y_sz)-my;

	ray_count=pin_build_ray_set_obj(obj,ty,by);

		// run the rays
		
	ray_trace_map_by_point_array(ray_count,pin_movement_spt,pin_movement_ept,pin_movement_hpt,pin_movement_hits,&base_contact,pin_movement_contacts);
	
		// find the lowest point
		
	obj->contact.head_poly.mesh_idx=-1;
	cy=-1;
	
	for (n=0;n!=ray_count;n++) {
		
			// check poly collisions
			
		if (pin_movement_hits[n]) {
		
			if ((cy==-1) || (pin_movement_hpt[n].y>cy)) {
				obj->contact.head_poly.mesh_idx=pin_movement_contacts[n].poly.mesh_idx;
				obj->contact.head_poly.poly_idx=pin_movement_contacts[n].poly.poly_idx;
				cy=pin_movement_hpt[n].y;
			}			
		}
	}
	
	if (cy==-1) return(my);

	return(cy-(obj->pnt.y-y_sz));
}

/* =======================================================

      Check if Object is being Crushed from Above
      
======================================================= */

bool map_stand_crush_object(obj_type *obj)
{
	int					y,fudge;
	
		// pick a crushing fudge, reduce size and
		// see if you can move up that far

	fudge=obj->size.y>>3;
	
	obj->size.y-=fudge;
	y=pin_upward_movement_obj(obj,fudge);
	obj->size.y+=fudge;
	
	if (obj->contact.head_poly.mesh_idx==-1) return(FALSE);
	
		// can only be crushed by moveable meshes
		
	if (!map.mesh.meshes[obj->contact.head_poly.mesh_idx].flag.moveable) return(FALSE);
	
		// if we can, go into auto duck before crushing
		// if already in duck, crush
		
	if (obj->duck.mode==dm_duck) return(TRUE);
	
	object_start_duck(obj);
	
	return(FALSE);
}

/* =======================================================

      Check if Standing Up is Possible
      
======================================================= */

bool map_stand_check_object(obj_type *obj)
{
	int					y,sy;

		// total stand up height

	sy=obj->duck.y_change;

		// possible to move up?

	y=pin_upward_movement_obj(obj,sy);
	return(obj->contact.head_poly.mesh_idx==-1);
}

