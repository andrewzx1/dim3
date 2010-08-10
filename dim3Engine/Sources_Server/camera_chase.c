/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Chase Camera

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
	#include "dim3engine.h"
#endif

#include "objects.h"
#include "weapons.h"
#include "cameras.h"
#include "physics.h"

extern server_type		server;
extern map_type			map;
extern camera_type		camera;
extern setup_type		setup;

extern float object_player_look_constrain(obj_type *obj,weapon_type *weap,float ang_x);

/* =======================================================

      Setup Chase Camera Structure
      
======================================================= */

void camera_chase_connect(void)
{
	obj_type		*obj;

	obj=server.obj_list.objs[camera.obj_idx];
	
	memmove(&camera.setup.pnt,&obj->pnt,sizeof(d3pnt));
	
		// current chasing angle
	
	camera.cur_chase_ang.x=camera.setup.ang.x;
    camera.cur_chase_ang.y=angle_add(obj->ang.y,camera.setup.ang.y);
	camera.cur_chase_ang.z=camera.setup.ang.z;
}

/* =======================================================

      Run Chase Camera
      
======================================================= */

float camera_chase_run_slop(float ang,float goal,float slop,float chase_track_speed)
{
	float		dif;
	bool		cwise;
	
	if ((ang==goal) || (slop==0)) return(goal);
	
		// force angle to within min/max slop
		
	dif=angle_dif(ang,goal,&cwise);
	if (dif>slop) {
		ang=angle_add(goal,(cwise?(-slop):(slop)));
	}
	
		// now turn towards goal
		
	return(angle_turn_toward(ang,goal,chase_track_speed));	
}

void camera_chase_run(void)
{
	float			goal_y;
	obj_type		*obj;

	obj=server.obj_list.objs[camera.obj_idx];
	
	goal_y=angle_add(obj->ang.y,camera.setup.ang.y);
	camera.cur_chase_ang.y=camera_chase_run_slop(camera.cur_chase_ang.y,goal_y,camera.setup.chase.slop.y,camera.setup.chase.track_speed);
}

/* =======================================================

      Find Chase Division
      
======================================================= */

int camera_chase_get_division(int x,int z,int y)
{
	int			k;
	
	k=x;
	if (z>k) k=z;
	if (y>k) k=y;
	
	return(k/map_enlarge);
}

/* =======================================================

      Get Chase Camera Position
      
======================================================= */

void camera_chase_get_position(d3pnt *pnt,d3ang *ang)
{
	int						n,xadd,yadd,zadd,radius,div,
							cx,cz,cy;
	float					fang,fx,fy,fz;
	d3pnt					spt,ept,hpt;
	ray_trace_contact_type	contact;
	matrix_type				mat;
	obj_type				*obj;
	weapon_type				*weap;

	obj=server.obj_list.objs[camera.obj_idx];
	
		// get maximum camera offset
		
	fx=0;
	fy=0;
	fz=(float)camera.setup.chase.distance;
	
	fang=angle_add(camera.cur_chase_ang.x,-obj->view_ang.x);
	
	matrix_rotate_zyx(&mat,fang,camera.cur_chase_ang.y,camera.cur_chase_ang.z);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
	cx=(int)fx+obj->pnt.x;
	cy=(int)fy+obj->pnt.y;
	cz=(int)fz+obj->pnt.z;
    
        // get camera movement
        
	div=camera_chase_get_division(abs(cx-obj->pnt.x),abs(cz-obj->pnt.z),abs(cy-obj->pnt.y));
	if (div==0) div=1;
	
	xadd=(cx-obj->pnt.x)/div;
	yadd=(cy-obj->pnt.y)/div;
	zadd=(cz-obj->pnt.z)/div;
	
		// get camera starting position
	
	radius=obj->size.radius;
	
	fx=0;
	fy=0;
	fz=(float)(radius>>2);			// always move camera back to protect against hitting walls in front of player
	
	fang=angle_add(camera.cur_chase_ang.x,(obj->view_ang.x*2));
	
	matrix_rotate_zyx(&mat,fang,camera.cur_chase_ang.y,camera.cur_chase_ang.z);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	
	cx=(int)fx+obj->pnt.x;
	cz=(int)fz+obj->pnt.z;
	cy=(int)fy+(obj->pnt.y-obj->size.y);

		// setup contacts

	contact.obj.on=TRUE;
	contact.obj.ignore_idx=obj->idx;

	contact.proj.on=FALSE;

	contact.origin=poly_ray_trace_origin_unknown;
		
		// move camera
		
	for (n=0;n!=div;n++) {
		
			// xz movement
			
		spt.x=cx;
		spt.y=cy;
		spt.z=cz;

		ept.x=cx+xadd;
		ept.y=cy;
		ept.z=cz+zadd;

		contact.hit_mode=poly_ray_trace_hit_mode_wall_only;
		if (!ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) {
			cx+=xadd;
			cz+=zadd;
		}
		
			// y movement

		spt.x=cx;
		spt.y=cy;
		spt.z=cz;

		ept.x=cx;
		ept.y=cy+yadd;
		ept.z=cz;

		contact.hit_mode=poly_ray_trace_hit_mode_floor_only;
		if (!ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) cy+=yadd;
	}
	
        // new camera position

	camera.setup.pnt.x=cx;
	camera.setup.pnt.y=cy;
	camera.setup.pnt.z=cz;
	
		// return values
		
    memmove(pnt,&camera.setup.pnt,sizeof(d3pnt));
	
		// looking angles
	
	ang->x=(camera.cur_chase_ang.x+camera.setup.ang.x)-obj->view_ang.x;

	weap=weapon_find_current(obj);
	if (weap!=NULL) ang->x=object_player_look_constrain(obj,weap,ang->x);

	ang->y=angle_add(camera.cur_chase_ang.y,camera.setup.ang.y);
	ang->z=camera.setup.ang.z;
}

/* =======================================================

      Get Chase Static Camera Position
      
======================================================= */

void camera_chase_static_get_position(d3pnt *pnt,d3ang *ang)
{
	float			fx,fy,fz,fang;
	obj_type		*obj;
	matrix_type		mat;

	obj=server.obj_list.objs[camera.obj_idx];
	
        // new camera position

	fx=0;
	fy=0;
	fz=(float)camera.setup.chase.distance;
	
	fang=camera.setup.ang.x;
	if (fang>180.0f) fang=fang-360.0f;
	
	matrix_rotate_zyx(&mat,fang,camera.setup.ang.y,camera.setup.ang.z);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);

	pnt->x=camera.setup.pnt.x=obj->pnt.x+((int)fx);
	pnt->y=camera.setup.pnt.y=obj->pnt.y+((int)fy);
	pnt->z=camera.setup.pnt.z=obj->pnt.z+((int)fz);
	
		// looking angles
		
	ang->x=-fang;
	ang->y=camera.setup.ang.y;
	ang->z=camera.setup.ang.z;
}

