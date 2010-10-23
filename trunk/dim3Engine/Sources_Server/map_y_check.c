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

#include "scripts.h"
#include "objects.h"
#include "physics.h"

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

int pin_downward_movement_complex(obj_type *obj,int ydist,poly_pointer_type *stand_poly)
{
	int						n,cy,x,z,x_sz,z_sz,
							grid_x,grid_z,lx,tz,idx,ty,by;
	ray_trace_contact_type	base_contact;
	
		// get contact grid
		
	grid_x=obj->size.x/map_enlarge;
	if (grid_x>16) grid_x=16;
	if (grid_x<5) grid_x=5;
	
	grid_z=obj->size.z/map_enlarge;
	if (grid_z>16) grid_z=16;
	if (grid_z<5) grid_z=5;
	
		// setup contact
		
	base_contact.obj.on=FALSE;
	base_contact.proj.on=FALSE;

	base_contact.origin=poly_ray_trace_origin_unknown;

		// create ray arrays
	
	idx=0;
	
	lx=obj->pnt.x-(obj->size.x>>1);
	tz=obj->pnt.z-(obj->size.z>>1);
	
	ty=obj->pnt.y-ydist;
	by=obj->pnt.y+ydist;
	
	x_sz=obj->size.x/grid_x;
	z_sz=obj->size.z/grid_z;
	
	for (z=0;z!=grid_z;z++) {
		for (x=0;x!=grid_x;x++) {
			pin_movement_spt[idx].x=pin_movement_ept[idx].x=lx+(x_sz*x);
			pin_movement_spt[idx].z=pin_movement_ept[idx].z=tz+(z_sz*z);
			pin_movement_spt[idx].y=ty;
			pin_movement_ept[idx].y=by;
			idx++;
		}
	}
		
		// run the rays
		
	ray_trace_map_by_point_array(idx,pin_movement_spt,pin_movement_ept,pin_movement_hpt,pin_movement_hits,&base_contact,pin_movement_contacts);
	
		// find the highest point
		
	stand_poly->mesh_idx=-1;
	cy=-1;
	
	for (n=0;n!=idx;n++) {
		
			// check poly collisions
			
		if (pin_movement_hits[n]) {
		
			if (stand_poly->mesh_idx==-1) {
				stand_poly->mesh_idx=pin_movement_contacts[n].poly.mesh_idx;
				stand_poly->poly_idx=pin_movement_contacts[n].poly.poly_idx;
				cy=pin_movement_hpt[n].y;
			}
			else {
				if (pin_movement_hpt[n].y<cy) {
					stand_poly->mesh_idx=pin_movement_contacts[n].poly.mesh_idx;
					stand_poly->poly_idx=pin_movement_contacts[n].poly.poly_idx;
					cy=pin_movement_hpt[n].y;
				}
			}
			
		}
	}
	
	if (cy==-1) return(obj->pnt.y+ydist);
	
	return(cy);
}

int pin_downward_movement_obj(obj_type *obj,int my)
{
	int				y;

	y=pin_downward_movement_complex(obj,my,&obj->contact.stand_poly);
	if (obj->contact.stand_poly.mesh_idx==-1) return(my);
	
	return(y-obj->pnt.y);
}

/* =======================================================

      Pin Upward Movements
      
======================================================= */

int pin_upward_movement_point(int x,int y,int z,int ydist,poly_pointer_type *head_poly)
{
	d3pnt					spt,ept,hpt;
	ray_trace_contact_type	contact;

	spt.x=x;
	spt.y=y+ydist;
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

int pin_upward_movement_complex(obj_type *obj,int ydist,poly_pointer_type *head_poly)
{
	int						n,cy,x,z,x_sz,z_sz,sz,
							grid_x,grid_z,lx,tz,idx,ty,by;
	ray_trace_contact_type	base_contact;
	
		// get contact grid
		
	grid_x=obj->size.x/map_enlarge;
	if (grid_x>16) grid_x=16;
	if (grid_x<5) grid_x=5;
	
	grid_z=obj->size.z/map_enlarge;
	if (grid_z>16) grid_z=16;
	if (grid_z<5) grid_z=5;
	
		// setup contact
		
	base_contact.obj.on=FALSE;
	base_contact.proj.on=FALSE;

	base_contact.origin=poly_ray_trace_origin_unknown;

		// create ray arrays
	
	idx=0;
	
	lx=obj->pnt.x-(obj->size.x>>1);
	tz=obj->pnt.z-(obj->size.z>>1);
	
	x_sz=obj->size.x/grid_x;
	z_sz=obj->size.z/grid_z;
	
	sz=obj->size.y;
	if (obj->duck.mode!=dm_stand) sz-=obj->duck.y_move;
	by=(obj->pnt.y-sz)+ydist;
	ty=(obj->pnt.y-sz)-ydist;

	
	for (z=0;z!=grid_z;z++) {
		for (x=0;x!=grid_x;x++) {
			pin_movement_spt[idx].x=pin_movement_ept[idx].x=lx+(x_sz*x);
			pin_movement_spt[idx].z=pin_movement_ept[idx].z=tz+(z_sz*z);
			pin_movement_spt[idx].y=by;
			pin_movement_ept[idx].y=ty;
			idx++;
		}
	}
		
		// run the rays
		
	ray_trace_map_by_point_array(idx,pin_movement_spt,pin_movement_ept,pin_movement_hpt,pin_movement_hits,&base_contact,pin_movement_contacts);
	
		// find the lowest point
		
	head_poly->mesh_idx=-1;
	cy=-1;
	
	for (n=0;n!=idx;n++) {
		
			// check poly collisions
			
		if (pin_movement_hits[n]) {
		
			if (head_poly->mesh_idx==-1) {
				head_poly->mesh_idx=pin_movement_contacts[n].poly.mesh_idx;
				head_poly->poly_idx=pin_movement_contacts[n].poly.poly_idx;
				cy=pin_movement_hpt[n].y;
			}
			else {
				if (pin_movement_hpt[n].y>cy) {
					head_poly->mesh_idx=pin_movement_contacts[n].poly.mesh_idx;
					head_poly->poly_idx=pin_movement_contacts[n].poly.poly_idx;
					cy=pin_movement_hpt[n].y;
				}
			}
			
		}
	}
	
	if (cy==-1) return((obj->pnt.y-sz)-ydist);
	
	return(cy);
}

int pin_upward_movement_obj(obj_type *obj,int my)
{
	int				y,sz;

	y=pin_upward_movement_complex(obj,abs(my),&obj->contact.head_poly);
	if (obj->contact.head_poly.mesh_idx==-1) return(my);

		// need to get duck value for this factor

	sz=obj->size.y;
	if (obj->duck.mode!=dm_stand) sz-=obj->duck.y_move;

	return((obj->pnt.y-sz)-y);
}

/* =======================================================

      Check if Object is being Crushed from Above
      
======================================================= */

bool map_stand_crush_object(obj_type *obj)
{
	int					y,fudge;
	poly_pointer_type	poly;
	
		// pick a crushing fudge, reduce size and
		// see if you can move up that far

	fudge=obj->size.y>>3;
	
	obj->size.y-=fudge;
	y=pin_upward_movement_complex(obj,fudge,&poly);
	obj->size.y+=fudge;
	
	if (poly.mesh_idx==-1) return(FALSE);
	
		// can only be crushed by moveable meshes
		
	if (!map.mesh.meshes[poly.mesh_idx].flag.moveable) return(FALSE);
	
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
	poly_pointer_type	poly;

		// total stand up height

	sy=obj->duck.y_change;

		// possible to move up?

	y=pin_upward_movement_complex(obj,sy,&poly);
	return(poly.mesh_idx==-1);
}

