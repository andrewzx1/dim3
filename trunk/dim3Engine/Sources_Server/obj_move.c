/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Movement

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
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"
#include "objects.h"
#include "models.h"
#include "physics.h"

extern map_type				map;
extern server_type			server;

/* =======================================================

      Object Slice Setup
      
======================================================= */

void object_slice_setup(obj_type *obj)
{
		// item counts
		
	obj->count++;
	if (obj->item_count!=0) obj->item_count--;
	
		// last positions
		
	obj->last_pnt.x=obj->pnt.x;
	obj->last_pnt.y=obj->pnt.y;
	obj->last_pnt.z=obj->pnt.z;

	obj->last_ang.x=obj->ang.x;
	obj->last_ang.y=obj->ang.y;
	obj->last_ang.z=obj->ang.z;
}

/* =======================================================

      Get Motion Movement
      
======================================================= */

inline void get_motion_movement(float ang,obj_movement *move,d3pnt *motion)
{
    angle_get_movement(ang,(int)move->speed,&motion->x,&motion->z);
    if (move->reverse) {
        motion->x=-motion->x;
        motion->z=-motion->z;
    }
}

void object_motion_setup(obj_type *obj,d3pnt *motion)
{
	int				xadd,zadd;
	float			ang;
	d3pnt			side_motion;
	
		// if thrust input type, then use thrust
		
	if (obj->input.mode==im_thrust) {
		motion->x=(int)(obj->thrust.vct.x+obj->force.vct.x);
		motion->y=(int)(obj->thrust.vct.y+obj->force.vct.y);
		motion->z=(int)(obj->thrust.vct.z+obj->force.vct.z);
		return;
	}
	
		// forward motion
		
	ang=obj->motion.ang.y;
	get_motion_movement(ang,&obj->forward_move,motion);
	
		// side motion
		
	if (obj->side_step) {
		get_motion_movement(angle_add(ang,270),&obj->side_move,&side_motion);
		motion->x+=side_motion.x;
		motion->z+=side_motion.z;
	}
	
		// vertical motion
		
	motion->y=0;

		// flying or swiming
	
	if ((obj->fly) || (obj->liquid.mode==lm_under) || (obj->liquid.mode==lm_float)) {
	
			// in auto-walk?
			
		if (obj->auto_walk.mode!=aw_none) {
			if (obj->forward_move.moving) {
				angle_get_movement(obj->vert_move.seek_ang,(int)obj->vert_move.max_walk_speed,&xadd,&zadd);
				motion->y=xadd;
			}
		}
		
			// regular seeking
			
		else {
			if (obj->vert_move.moving) {
				motion->y=(int)obj->vert_move.speed;
				if (obj->vert_move.reverse) motion->y=-motion->y;
			}
		}
	}

		// add in outside forces

	motion->x+=(int)obj->force.vct.x;
	motion->y+=(int)obj->force.vct.y;
	motion->z+=(int)obj->force.vct.z;
}

/* =======================================================

      Object Speed Control
      
======================================================= */

void object_movement(obj_type *obj,obj_movement *move)
{
    float		speed,max_speed,accelerate,decelerate;
    
    speed=move->speed;
	
		// single speed mode?
		
	if (obj->single_speed) {
		max_speed=move->max_walk_speed;
		accelerate=move->accelerate;
		decelerate=move->decelerate;
	}
	
		// variable speeds
		
	else {
    
		if (obj->duck.mode!=dm_stand) {
			max_speed=move->max_crawl_speed;
			accelerate=move->accelerate;
			decelerate=move->decelerate;
		}
		else {
			if ((obj->air_mode!=am_ground) || (obj->fly) || (obj->liquid.mode==lm_under) || (obj->liquid.mode==lm_float)) {
				max_speed=move->max_air_speed;
				accelerate=move->air_accelerate;
				decelerate=move->air_decelerate;
			}
			else {
				if (obj->forward_move.running) {
					max_speed=move->max_run_speed;
				}
				else {
					max_speed=move->max_walk_speed;
				}
				accelerate=move->accelerate;
				decelerate=move->decelerate;
			}
		}
	}
    
		// Accelerating
	
	if (move->moving) {
		
        if (speed==max_speed) return;
        
        if (speed<max_speed) {							// if max speed has changed to be less, auto-decelerate
            speed+=(accelerate*map.settings.resistance);
            if (speed>max_speed) speed=max_speed;
		
            move->speed=speed;
            return;
        }
	}
	
		// Decelerating

	speed-=(decelerate*map.settings.resistance);
	if (speed<0) speed=0;
    
	move->speed=speed;
}

void object_simple_movement(obj_type *obj,obj_movement *move)
{
    float		speed,max_speed,accelerate,decelerate;
    
    speed=move->speed;
	
	max_speed=move->max_walk_speed;
	accelerate=move->accelerate;
	decelerate=move->decelerate;

		// Accelerating
	
	if (move->moving) {
		
        if (speed==max_speed) return;
        
        if (speed<max_speed) {							// if max speed has changed to be less, auto-decelerate
            speed+=(accelerate*map.settings.resistance);
            if (speed>max_speed) speed=max_speed;
		
            move->speed=speed;
            return;
        }
	}
	
		// Decelerating

	speed-=(decelerate*map.settings.resistance);
	if (speed<0) speed=0;
    
	move->speed=speed;
}

/* =======================================================

      Increase Gravity
      
======================================================= */

void object_gravity(obj_type *obj)
{
    float		weight,gravity,gravity_max_power,
				liq_speed_alter;
    
		// reset gravity if flying, climbing ladder, or on ground

	if ((obj->fly) || (obj->climb.on) || (obj->air_mode==am_ground)) {
		obj->force.gravity=gravity_start_power;
		return;
	}

		// under liquids changes gravity

	weight=(float)obj->size.weight;
	gravity_max_power=map.settings.gravity_max_power;

	if ((obj->liquid.mode==lm_under) || (obj->liquid.mode==lm_float)) {
		liq_speed_alter=object_liquid_alter_speed(obj);

		weight*=liq_speed_alter;
		gravity_max_power*=liq_speed_alter;
	}

		// run gravity
    
    gravity=obj->force.gravity;
	
		// increase the gravity
		
	if (gravity<gravity_max_power) {
		gravity+=(weight*(map.settings.gravity/gravity_factor));
		if (gravity>gravity_max_power) gravity=map.settings.gravity_max_power;
        
        obj->force.gravity=gravity;
	}
}

void object_fix_force(obj_type *obj)
{
	float			decelerate,gravity_max_speed;
	d3vct			*vct;
	
		// get the deceleration
		
	decelerate=map.settings.resistance;
	
		// reduce the x/z by resistance
		
	vct=&obj->force.vct;
	
	if (vct->x!=0) {
		if (vct->x>0) {
			vct->x-=decelerate;
			if (vct->x<0) vct->x=0;
		}
		else {
			vct->x+=decelerate;
			if (vct->x>0) vct->x=0;
		}
	}
	
	if (vct->z!=0) {
		if (vct->z>0) {
			vct->z-=decelerate;
			if (vct->z<0) vct->z=0;
		}
		else {
			vct->z+=decelerate;
			if (vct->z>0) vct->z=0;
		}
	}

		// gravity works differently under
		// a liquid.  We reduce gravity to give
		// bouancy and no gravity if floating

	if (obj->liquid.mode==lm_float) return;

	gravity_max_speed=map.settings.gravity_max_speed;
	if (obj->liquid.mode==lm_under) gravity_max_speed*=(object_liquid_alter_speed(obj)*0.5f);
	
		// reduce the y by gravity
		
	if (obj->force.gravity>0) {
		vct->y+=obj->force.gravity;
		if (vct->y>gravity_max_speed) vct->y=gravity_max_speed;
	}
}

/* =======================================================

      Object Alterations
      
======================================================= */

void object_alter_speed(obj_type *obj,float alt_speed)
{
    obj->forward_move.speed*=alt_speed;
    obj->side_move.speed*=alt_speed;
}

void object_alter_gravity(obj_type *obj,float alt_gravity)
{
    obj->force.vct.y*=alt_gravity;
    obj->force.gravity=gravity_start_power;
}

/* =======================================================

      Object Bumping and Bouncing
      
======================================================= */

void object_fix_bump_smooth(obj_type *obj)
{
	int			k;
	
	if (obj->bump.smooth_offset==0) return;
	
		// slowly remove bump offset
		
	k=(int)(((float)obj->bump.smooth_offset)*obj->bump.smooth_factor);
	if (k<10) k=10;
	
	obj->bump.smooth_offset-=k;
	if (obj->bump.smooth_offset<0) obj->bump.smooth_offset=0;
}

void object_move_xz_bounce(obj_type *obj)
{
	if (obj->contact.hit_poly.mesh_idx==-1) {
		obj->bounce.mesh_idx=-1;
		return;
	}

		// only bounce if bounce factor != 0 and not same mesh
		
	if (obj->bounce.factor==0.0f) return;
	if (obj->bounce.mesh_idx==obj->contact.hit_poly.mesh_idx) return;
	
		// bounce
		
	obj->force.vct.x-=(obj->motion.vct.x*obj->bounce.factor);
	obj->force.vct.z-=(obj->motion.vct.z*obj->bounce.factor);
}

/* =======================================================

      Object Slope Speed Alterations
      
======================================================= */

void object_motion_slope_alter_movement_single(int *mv,float slope_y,float slope_mv)
{
	bool			same_dir;
	
		// are we going in same direction as slope?
		
	same_dir=FALSE;
	
	if ((*mv)!=0) {
		same_dir=(((*mv)<0) && (slope_mv>=0.0f)) || (((*mv)>=0) && (slope_mv<=0.0f));
	}
	
		// if slope is greater than max gravity,
		// then if going against slope, stop all movement, otherwise always go down

	if (slope_y>=gravity_slope_max_y) {
		if (same_dir) {
			*mv=-(int)slope_mv;
		}
		else {
			*mv=0;
		}
		return;
	}

		// if going in same direction, then cut gravity effect

	if (same_dir) slope_mv*=gravity_slope_down_cut;
	
		// subtract slope push from movement

	*mv-=(int)slope_mv;
}

void object_motion_slope_alter_movement(obj_type *obj,d3pnt *motion)
{
	int					x,y,z,sy;
	poly_pointer_type	poly;
	map_mesh_poly_type	*mesh_poly;

		// if not on ground or ignoring slope gravity, then no speed reduction
		
	if (!obj->slope_gravity) return;
	if (obj->air_mode!=am_ground) return;
	if (obj->contact.stand_obj_idx!=-1) return;
	
		// get floor going to
		
	x=obj->pnt.x+motion->x;
	y=obj->pnt.y;
	z=obj->pnt.z+motion->z;

	sy=pin_downward_movement_point(x,y,z,obj->size.y,&poly);
	if (poly.mesh_idx==-1) return;

		// ignore flat polygons

	mesh_poly=&map.mesh.meshes[poly.mesh_idx].polys[poly.poly_idx];
	if (mesh_poly->box.flat) return;

		// if less then min slope, no gravity effects

	if (mesh_poly->slope.y<gravity_slope_min_y) return;

		// apply gravity

	object_motion_slope_alter_movement_single(&motion->x,mesh_poly->slope.y,mesh_poly->slope.move_x);
	object_motion_slope_alter_movement_single(&motion->z,mesh_poly->slope.y,mesh_poly->slope.move_z);
}

/* =======================================================

      Motion Locks and Script Setup
      
======================================================= */

void object_motion_lock(obj_type *obj,d3pnt *motion)
{
	if (obj->lock.x) motion->x=0;
	if (obj->lock.y) motion->y=0;
	if (obj->lock.z) motion->z=0;
}

void object_motion_set_script_property(obj_type *obj,d3pnt *motion)
{
	obj->motion.vct.x=(float)motion->x;
	obj->motion.vct.z=(float)motion->y;
	obj->motion.vct.y=(float)motion->z;
}

/* =======================================================

      Y Movement
      
======================================================= */

void object_move_y_up(obj_type *obj,int ymove)
{
	int				fy,up_move;
	
	obj->air_mode=am_up;
	
		// have any floors come up from below

	fy=pin_downward_movement_obj(obj,floor_slop);
	if (fy<0) {
		obj->pnt.y+=fy;
		ymove+=fy;
		if (ymove>=0) return;
	}

		// go upwards

	up_move=pin_upward_movement_obj(obj,ymove);
	obj->pnt.y+=up_move;
	
	if (up_move>ymove) {
		obj->force.vct.y=0;
		obj->force.gravity=gravity_start_power;
		return;
	}

		// check for above map

	if (obj->pnt.y<=0) {
		obj->pnt.y=0;
		obj->force.vct.y=0;
		obj->force.gravity=gravity_start_power;
		return;
	}
}

void object_move_y_fall(obj_type *obj)
{
	int				y,fy,ymove,idx;
	obj_type		*hit_obj;

		// check standing on polygons

	y=obj->pnt.y;
		
	fy=pin_downward_movement_obj(obj,floor_slop);
	fy+=y;

		// deal with polygon contacts
		
	if (obj->contact.stand_poly.mesh_idx!=-1) {
	
			// clear any stand touch
			
		if (obj->touch.stand) {
			obj->touch.obj_idx=-1;
		}
		
			// below or on the floor?

		if (y>=fy) {
			obj->air_mode=am_ground;
			obj->pnt.y=fy;
			return;
		}
		
			// above the polygon

		ymove=(int)obj->force.vct.y;
		if ((y>(fy-floor_slop)) || ((y+ymove)>fy)) {		// next floor or slop will go past floor
			if (ymove>=0) {
				obj->air_mode=am_ground;
				obj->pnt.y=fy;
			}
			return;
		}
	}
	
		// check standing on objects

	idx=collide_object_for_object_stand(obj);
	if (idx!=-1) {
		hit_obj=server.obj_list.objs[idx];
		
		obj->contact.obj_idx=idx;
		obj->pnt.y=hit_obj->pnt.y-hit_obj->size.y;
		obj->air_mode=am_ground;
		
			// stand on contact touch
			
		obj->contact.stand_obj_idx=hit_obj->idx;
		return;
	}
	
		// find air mode
		
	if (obj->force.vct.y<0) {
		obj->air_mode=am_up;
	}
	else {
		obj->air_mode=am_falling;
	}
}

void object_move_y_down(obj_type *obj,int ymove)
{
	int			fy;
	
		// fix objects gravity and view angles
		// if on ground
		
	if (obj->air_mode==am_ground) {
 		if (obj->force.vct.y>0) obj->force.vct.y=0;
		obj->force.gravity=gravity_start_power;
		return;
	}
	
		// moving down
		
	fy=pin_downward_movement_obj(obj,ymove);
	obj->pnt.y+=fy;
	
	if (obj->air_mode==am_falling) obj->fall.dist+=fy;
}

void object_move_y(obj_type *obj,int ymove)
{
	if (ymove==0) {
		object_move_y_fall(obj);
		return;
	}

	if (ymove<0) {
		object_move_y_up(obj,ymove);
	}
	else {
		object_move_y_down(obj,ymove);
	}
}

void object_move_y_fly(obj_type *obj,int ymove)
{
		// moving up
		
	if (ymove<0) {
		obj->pnt.y+=pin_upward_movement_obj(obj,ymove);
		if (obj->contact.head_poly.mesh_idx!=-1) return;

			// check for above map
			
		if (obj->pnt.y<=0) {
			obj->pnt.y=0;
			return;
		}

		return;
	}
	
		// moving down
		
	if (ymove<=0) return;
	
	obj->pnt.y+=pin_downward_movement_obj(obj,ymove);
}

/* =======================================================

      X/Z Movement
      
======================================================= */

void object_move_xz(obj_type *obj,d3pnt *motion)
{
	int				hit_obj_idx;

		// move object

	if (!collide_object_to_map(obj,motion)) {
		object_motion_lock(obj,motion);
		obj->pnt.x+=motion->x;
		obj->pnt.z+=motion->z;
		return;
	}

		// if we hit something, see if it's a
		// pushable object, otherwise we just
		// add in the motion for sliding

	if (!object_push_with_object(obj,motion)) {
		object_motion_lock(obj,motion);
		obj->pnt.x+=motion->x;
		obj->pnt.z+=motion->z;
		return;
	}

		// save hit contact and move
		// original object after moving
		// pushed object

	hit_obj_idx=obj->contact.obj_idx;
	
	collide_object_to_map(obj,motion);

	object_motion_lock(obj,motion);
	obj->pnt.x+=motion->x;
	obj->pnt.z+=motion->z;

	obj->contact.obj_idx=hit_obj_idx;
}

/* =======================================================

      Climbing Movements
      
======================================================= */

void object_move_climb(obj_type *obj)
{
	int			ymove;
	
		// climb up and down
		
	ymove=(int)obj->vert_move.speed;
	if (obj->vert_move.reverse) ymove=-ymove;
	
	object_move_y_fly(obj,(int)ymove);
}

/* =======================================================

      Flying Movements
      
======================================================= */

void object_move_fly(obj_type *obj)
{
	d3pnt			motion;

		// get object motion
		
	object_motion_setup(obj,&motion);
	object_motion_lock(obj,&motion);
	object_motion_set_script_property(obj,&motion);
 
		// clear all contacts

	object_clear_contact(&obj->contact);
	
	if ((motion.x!=0) || (motion.y!=0) || (motion.z!=0)) {

			// move object

		object_move_xz(obj,&motion);
	
			// move objects standing on this object
	
		if ((motion.x!=0) || (motion.x!=0)) object_move_with_standing_object(obj,&motion);
	}

		// bounces
		
	object_move_xz_bounce(obj);
	
        // move Y

	object_move_y_fly(obj,motion.y);
}

/* =======================================================

      Swimming Movements
      
======================================================= */

void object_move_swim(obj_type *obj)
{
    float			liq_speed_alter;
	d3pnt			motion;

		// get object motion
		
	object_motion_setup(obj,&motion);
	object_motion_lock(obj,&motion);
	object_motion_set_script_property(obj,&motion);

		// clear all contacts

	object_clear_contact(&obj->contact);

		// speed cuts

	liq_speed_alter=object_liquid_alter_speed(obj);
	
	motion.x=(int)(((float)motion.x)*liq_speed_alter);
	motion.y=(int)(((float)motion.y)*liq_speed_alter);
	motion.z=(int)(((float)motion.z)*liq_speed_alter);

		// falling in water
		// if on surface, then auto-float

	if (motion.y>=0) object_move_y_fall(obj);

	if ((obj->air_mode==am_ground) || (motion.y<0)) {
		if (obj->force.vct.y>0) obj->force.vct.y=0;
		obj->force.gravity=gravity_start_power;
	}

        // moving x/z

	if ((motion.x!=0) || (motion.y!=0) || (motion.z!=0)) {

			// move object

		object_move_xz(obj,&motion);

			// move objects standing on this object

		if ((motion.x!=0) || (motion.z!=0)) object_move_with_standing_object(obj,&motion);
	}

		// bounces
		
	object_move_xz_bounce(obj);
	
        // move Y

	object_move_y_fly(obj,motion.y);
}

/* =======================================================

      Normal Movements
      
======================================================= */

void object_move_normal(obj_type *obj)
{
	int					bump_y_move,start_y,fall_damage;
	bool				old_falling;
	d3pnt				motion,old_pnt;

		// get object motion
		
	object_motion_setup(obj,&motion);
	object_motion_slope_alter_movement(obj,&motion);
	object_motion_lock(obj,&motion);
	object_motion_set_script_property(obj,&motion);

		// save old settings

	old_falling=(obj->air_mode==am_falling);

		// special check for non-moving objects that can retain their
		// position and not run physics.  They must be standing on
		// non-moveable meshes and not be a player

	if ((motion.x==0) && (motion.y==0) && (motion.z==0) && (obj->type!=object_type_player)) {
	
			// are we standing on a non-moving mesh?
			// if so, mark no hit collisions and return

		if (obj->contact.stand_poly.mesh_idx!=-1) {

			if (!map.mesh.meshes[obj->contact.stand_poly.mesh_idx].flag.moveable) {
				obj->contact.hit_poly.mesh_idx=-1;
				return;
			}

		}
	}

		// clear all contacts

	object_clear_contact(&obj->contact);

		// move the object in y space at the projected
		// x/z position
		//
		// we want to make sure that we move over land
		// features before they get blocked by wall-like
		// features in the map
		//
		// also, remember the y change and use that when
		// running the x/z ray trace collisions to continue
		// to avoid land features that might block foward
		// movement

	memmove(&old_pnt,&obj->pnt,sizeof(d3pnt));

	start_y=obj->pnt.y;

	obj->pnt.x=obj->pnt.x+motion.x;
	obj->pnt.z=obj->pnt.z+motion.z;

	if (motion.y<0) {
		object_move_y_up(obj,motion.y);
	}
	else {
		object_move_y_fall(obj);
		object_move_y_down(obj,motion.y);
	}

	obj->pnt.x=old_pnt.x;
	obj->pnt.z=old_pnt.z;

		// add in the last y change. we use this
		// information in the next move to help
		// the object move over radically bumping
		// ground
		//
		// we only use the y change in x/z movement
		// if we are going up
		
	motion.y=(obj->pnt.y-start_y)+obj->motion.last_y_change;
	if (motion.y>0) motion.y=0;

	obj->motion.last_y_change=obj->pnt.y-start_y;

		// check if we will be hitting a bump up
		// in the course of this move, if so, bump up
		// now and if no movement, then bump back down

	bump_y_move=0;
	
	if ((obj->bump.on) && (obj->air_mode==am_ground)) {

		if (collide_object_to_map_bump(obj,&motion,&bump_y_move)) {

			bump_y_move=pin_upward_movement_obj(obj,bump_y_move);
			if (obj->contact.head_poly.mesh_idx==-1) {
				obj->pnt.y+=bump_y_move;
				obj->bump.smooth_offset-=bump_y_move;
			}
			else {
				bump_y_move=0;
			}
		}
	}

		// if on ground, stop all downward motion
		// and forces
	
	if (obj->air_mode==am_ground) {
		if (obj->force.vct.y>0) obj->force.vct.y=0;
		obj->force.gravity=gravity_start_power;
	}

		// now we move the object in x/z space
		//
		// we also add in the y movement into the
		// collision detection to help eliminate
		// land features that could hold up the
		// object
		
	if ((motion.x!=0) || (motion.z!=0)) {
	
			// attempt to move
			
		object_move_xz(obj,&motion);

			// determine if we moved or not

		motion.x=obj->pnt.x-old_pnt.x;
		motion.z=obj->pnt.z-old_pnt.z;
			
		if ((motion.x!=0) || (motion.z!=0)) {

				// move objects standing on object

			object_move_with_standing_object(obj,&motion);

				// objects with automatic bouncing

			object_move_xz_bounce(obj);
		}

			// if no movement, get rid of any bump

		else {
			if (bump_y_move!=0) {
				bump_y_move=pin_downward_movement_obj(obj,bump_y_move);
				obj->pnt.y+=bump_y_move;
				obj->bump.smooth_offset-=bump_y_move;
			}
		}
	}
	
		// check for objects that have finished falling
		// here we do any damage and send any landing
		// events
	
    if (obj->air_mode!=am_falling) {
    
        if (old_falling) {
            if (obj->fall.dist>map_enlarge) {
			
					// get damage
					
				if (obj->fall.dist>=obj->fall.damage_minimum_height) {
					fall_damage=(int)(((float)(obj->fall.dist-obj->fall.damage_minimum_height))*obj->fall.damage_factor);
					if (fall_damage!=0) object_damage(obj,NULL,NULL,NULL,NULL,fall_damage);
				}
				
					// send the land events
					
                scripts_post_event_console(&obj->attach,sd_event_land,0,0);
                object_post_move_animation_event(obj,sd_event_animation_object_land);
            }
        }
        
        obj->fall.dist=0;
        obj->fall.change=FALSE;

		return;
	}
	
		// check for objects that have started falling
	
	if ((obj->fall.dist>map_enlarge) && (!obj->fall.change)) {
		scripts_post_event_console(&obj->attach,sd_event_fall,0,0);
		scripts_post_event_console(&obj->attach,sd_event_animation_object,sd_event_animation_object_fall,0);
		obj->fall.change=TRUE;
	}
}

/* =======================================================

      Climbing Checks
      
======================================================= */

void object_move_climb_check(obj_type *obj)
{
	bool						cwise;
	map_mesh_poly_type			*poly;
	
		// check if we are touching ground
		// if not, we can step off next time we touch

	if (obj->contact.stand_poly.mesh_idx!=-1) {
		if ((obj->climb.on) && (obj->climb.step_off_ready)) {
			obj->climb.on=FALSE;
			return;
		}
	}
	else {
		obj->climb.step_off_ready=TRUE;
	}
	
		// check if we've gone above climbable polygon
		// or turned away from it
		
	if (obj->climb.on) {
	
		poly=&map.mesh.meshes[obj->climb.poly_ptr.mesh_idx].polys[obj->climb.poly_ptr.poly_idx];
		
			// above polygon?
			
		if (obj->pnt.y<poly->box.min.y) {
			obj->climb.on=FALSE;
			return;
		}
		
			// turned away from it?
			
		if (angle_dif(obj->ang.y,obj->climb.y_ang,&cwise)>60.0f) {
			obj->climb.on=FALSE;
			return;
		}
	
	}
	
		// if we aren't climbing and contact
		// a climbable mesh, then start climbing
		
	if (!obj->climb.on) {
		
		if (obj->contact.hit_poly.mesh_idx!=-1) {
		
			poly=&map.mesh.meshes[obj->contact.hit_poly.mesh_idx].polys[obj->contact.hit_poly.poly_idx];

			if (poly->climbable) {
				obj->climb.on=TRUE;
				obj->climb.step_off_ready=FALSE;
				
				memmove(&obj->climb.poly_ptr,&obj->contact.hit_poly,sizeof(poly_pointer_type));
				
				obj->climb.y_ang=angle_find(obj->pnt.x,obj->pnt.z,poly->box.mid.x,poly->box.mid.z);
			}
		}
	}
}

/* =======================================================

      Object Movement Mainline
      
======================================================= */

void object_move(obj_type *obj)
{
	d3pnt					old_pnt;
	
		// remember original point for force
		// cancelation later
		
	old_pnt.x=obj->pnt.x;
	old_pnt.y=obj->pnt.y;
	old_pnt.z=obj->pnt.z;
	
		// call proper movement
		
	if (obj->climb.on) {
		object_move_climb(obj);
	}
	else {
		if (obj->fly) {
			object_move_fly(obj);
		}
		else {
			if ((obj->liquid.mode==lm_under) || (obj->liquid.mode==lm_float)) {
				object_move_swim(obj);
			}
			else {
				object_move_normal(obj);
			}
		}
	}
	
		// check for contacts

	if (obj->contact.hit_poly.mesh_idx!=-1) {
	
			// stop any forces
			
		if (old_pnt.x==obj->pnt.x) obj->force.vct.x=0.0f;
		if (old_pnt.y==obj->pnt.z) obj->force.vct.y=0.0f;
		if (old_pnt.z==obj->pnt.z) obj->force.vct.z=0.0f;

			// if no movement at all, stop speed

		if ((old_pnt.x==obj->pnt.x) && (old_pnt.y==obj->pnt.y) && (old_pnt.z==obj->pnt.z)) {
			obj->forward_move.speed=0.0f;
			obj->side_move.speed=0.0f;
			obj->vert_move.speed=0.0f;
		}
	
			// send contact event
			
		if (!obj->in_collide_event) {
			obj->in_collide_event=TRUE;
			scripts_post_event_console(&obj->attach,sd_event_collide,0,0);
		}
		
	}
	
		// mark as out of event so we only
		// send event once
		
	else {
		obj->in_collide_event=FALSE;
	}
	
		// check for climbing
		
	object_move_climb_check(obj);
}

void object_move_remote(obj_type *obj)
{
	d3pnt			motion;

	motion.x=obj->remote.predict.move.x;
	motion.y=obj->remote.predict.move.y;
	motion.z=obj->remote.predict.move.z;

		// if we hit anything, clear
		// the prediction until next
		// network message

	if (collide_object_to_map(obj,&motion)) {
		obj->remote.predict.move.x=0;
		obj->remote.predict.move.z=0;
	}

	object_motion_lock(obj,&motion);

	obj->pnt.x+=motion.x;
	obj->pnt.z+=motion.z;

	object_move_y(obj,motion.y);
}

/* =======================================================

      Run Thrust
      
======================================================= */

void object_thrust(obj_type *obj)
{
	float			xmove,ymove,zmove,ztemp,
					xmax,ymax,zmax,drag;
	
	if (obj->input.mode!=im_thrust) return;
	
		// contacts cancel thrust

	if ((obj->contact.hit_poly.mesh_idx!=-1) || (obj->contact.obj_idx!=-1)) {
		obj->thrust.vct.x=0;
		obj->thrust.vct.y=0;
		obj->thrust.vct.z=0;
	}
	
		// run thrust
		
	if (!obj->forward_move.moving) return;
	if (obj->thrust.max_speed==0.0f) return;
	
    angle_get_movement_float(obj->ang.y,obj->thrust.speed,&xmove,&zmove);
	angle_get_movement_float(obj->view_ang.x,obj->thrust.speed,&ymove,&ztemp);

	obj->thrust.vct.x+=xmove;
	obj->thrust.vct.y+=ymove;
	obj->thrust.vct.z+=zmove;
	
		// drag option

	if (obj->thrust.drag) {
		
		drag=1.0f-(obj->thrust.speed/obj->thrust.max_speed);

		angle_get_movement_float(obj->ang.y,obj->thrust.max_speed,&xmax,&zmax);
		xmax=(float)fabs(xmax);
		zmax=(float)fabs(zmax);
	
		angle_get_movement_float(obj->view_ang.x,obj->thrust.max_speed,&ymax,&ztemp);
		ymax=(float)fabs(ymax);
	
		if ((obj->thrust.vct.x>xmax) || (obj->thrust.vct.x<-xmax)) obj->thrust.vct.x*=drag;
		if ((obj->thrust.vct.y>ymax) || (obj->thrust.vct.y<-ymax)) obj->thrust.vct.y*=drag;
		if ((obj->thrust.vct.z>zmax) || (obj->thrust.vct.z<-zmax)) obj->thrust.vct.z*=drag;
		
		return;
	}
	
		// regular thrust locked by max
		
	if (obj->thrust.vct.x>obj->thrust.max_speed) obj->thrust.vct.x=obj->thrust.max_speed;
	if (obj->thrust.vct.x<-obj->thrust.max_speed) obj->thrust.vct.x=-obj->thrust.max_speed;
	
	if (obj->thrust.vct.y>obj->thrust.max_speed) obj->thrust.vct.y=obj->thrust.max_speed;
	if (obj->thrust.vct.y<-obj->thrust.max_speed) obj->thrust.vct.y=-obj->thrust.max_speed;
	
	if (obj->thrust.vct.z>obj->thrust.max_speed) obj->thrust.vct.z=obj->thrust.max_speed;
	if (obj->thrust.vct.z<-obj->thrust.max_speed) obj->thrust.vct.z=-obj->thrust.max_speed;
}
	
/* =======================================================

      Start/Stop Object Movement
      
======================================================= */

void object_move_start(obj_type *obj)
{
	obj->forward_move.moving=TRUE;
}

void object_move_stop(obj_type *obj)
{
	obj->forward_move.moving=FALSE;
	obj->side_move.moving=FALSE;
	obj->vert_move.moving=FALSE;
	
	object_auto_walk_stop(obj);
}

