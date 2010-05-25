/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Weapon Selecting

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
#include "models.h"
#include "timing.h"

extern server_type				server;
extern camera_type				camera;

/* =======================================================

      Reset Ammo for Weapons
      
======================================================= */

void weapon_reset_ammo(weapon_type *weap)
{
	weap->ammo.count=weap->ammo.count_dual=weap->ammo.init_count;
	weap->ammo.clip_count=weap->ammo.init_clip_count;
	
	weap->alt_ammo.count=weap->alt_ammo.count_dual=weap->alt_ammo.init_count;
	weap->alt_ammo.clip_count=weap->alt_ammo.init_clip_count;
}	

void weapon_reset_ammo_object(obj_type *obj)
{
    int					n;
	weapon_type			*weap;
    
    for (n=0;n!=max_weap_list;n++) {
		weap=obj->weap_list.weaps[n];
		if (weap==NULL) continue;

		weapon_reset_ammo(weap);
    }
}
	
/* =======================================================

      Set Weapon
      
======================================================= */

void weapon_clear_animation(weapon_type *weap)
{
	int						n;
	model_draw_animation	*animation;
	
	animation=weap->draw.animations;

	for (n=0;n!=max_model_blend_animation;n++) {
		animation->mode=am_stopped;
		animation->smooth_animate_idx=-1;
		animation->smooth_pose_move_idx=0;
		animation++;
	}
}

void weapon_set(obj_type *obj,weapon_type *weap)
{
	obj->held_weapon.current_index=weap->idx;
	obj->held_weapon.next_index=weap->idx;
    obj->held_weapon.mode=wm_held;
    
	weapon_clear_animation(weap);

	scripts_post_event_console(&weap->attach,sd_event_animation_weapon,sd_event_animation_weapon_held,0);
	
	scripts_post_event_console(&weap->attach,sd_event_weapon_select,0,0);
	scripts_post_event_console(&obj->attach,sd_event_weapon_select,0,0);
}

/* =======================================================

      Clear Weapon States
      
======================================================= */

void weapon_clear_state(obj_type *obj)
{
	weapon_type			*weap;
	
		// if there was a changing weapon
		// complete the change
		
	if (obj->held_weapon.next_index!=-1) obj->held_weapon.current_index=obj->held_weapon.next_index;

	obj->held_weapon.mode=wm_held;

		// turn off any weapon changing

	obj->held_weapon.next_index=-1;
    obj->held_weapon.mode=wm_held;
    obj->held_weapon.swap_tick=0;
	obj->held_weapon.bounce_y=0;
	
		// get held weapon
		
	weap=weapon_find_current(obj);
	if (weap==NULL) return;

		// turn off any zooming

	weap->zoom.mode=zoom_mode_off;

		// and any animation
		// and get idle animation from script and
		// make sure weapon gets set

	weapon_clear_animation(weap);

	scripts_post_event_console(&weap->attach,sd_event_animation_weapon,sd_event_animation_weapon_held,0);

	scripts_post_event_console(&weap->attach,sd_event_weapon_select,0,0);
	scripts_post_event_console(&obj->attach,sd_event_weapon_select,0,0);
}

/* =======================================================

      Change Weapon
      
======================================================= */

void weapon_lower(obj_type *obj)
{
	weapon_type			*weap;
	
	obj->held_weapon.mode=wm_lower;
	obj->held_weapon.swap_tick=game_time_get();
    
	weap=weapon_find_current(obj);
	if (weap==NULL) return;
	
	scripts_post_event_console(&weap->attach,sd_event_animation_weapon,sd_event_animation_weapon_lower,0);
}

void weapon_raise(obj_type *obj)
{
	weapon_type			*weap;
	
    obj->held_weapon.mode=wm_raise;
 	obj->held_weapon.swap_tick=game_time_get();
   
	weap=weapon_find_current(obj);
	if (weap==NULL) return;

	scripts_post_event_console(&weap->attach,sd_event_animation_weapon,sd_event_animation_weapon_raise,0);
}

void weapon_held(obj_type *obj)
{
 	weapon_type			*weap;
	
	obj->held_weapon.mode=wm_held;
    
	weap=weapon_find_current(obj);
	if (weap==NULL) return;

	scripts_post_event_console(&weap->attach,sd_event_animation_weapon,sd_event_animation_weapon_held,0);
}

void weapon_cur_weapon_recoil_copy(obj_type *obj,int copy_weap_index)
{
	weapon_type		*to_weap,*cur_weap;

	to_weap=obj->weap_list.weaps[copy_weap_index];
	if (to_weap==NULL) return;
				
	cur_weap=weapon_find_current(obj);
	if (cur_weap==NULL) {
		weapon_recoil_clear(to_weap);
		return;
	}

	weapon_recoil_copy(to_weap,cur_weap);
}

void weapon_goto(obj_type *obj,weapon_type *weap)
{
		// copy any recoils

	weapon_cur_weapon_recoil_copy(obj,weap->idx);

		// set next weapon
	
	obj->held_weapon.next_index=weap->idx;

		// clear any animation smoothing

	weapon_clear_animation(weap);

        // tell player about new weapon
		
	scripts_post_event_console(&weap->attach,sd_event_weapon_select,0,0);
	scripts_post_event_console(&obj->attach,sd_event_weapon_select,0,0);
        
        // start hand swap

    if (obj->held_weapon.mode!=wm_lower) weapon_lower(obj);
}

void weapon_switch(obj_type *obj,int dir)
{
    int				weap_mode,weap_idx;
    weapon_type		*weap;

    weap_mode=obj->held_weapon.mode;
    
        // already changing weapons?
        
    if (weap_mode!=wm_lower) {
		weap_idx=obj->held_weapon.current_index;
    }
    else {
		weap_idx=obj->held_weapon.next_index;
    }

	if (weap_idx==-1) return;
	
		// change to new weapon
		
    while (TRUE) {
        weap_idx+=dir;
		
			// wrapping at edges

        if (weap_idx<0) weap_idx=max_weap_list-1;
        if (weap_idx>=max_weap_list) weap_idx=0;
		
			// have we wrapped back around?
			
		if (weap_idx==obj->held_weapon.current_index) return;
			
			// can we select this weapon?
			
		weap=obj->weap_list.weaps[weap_idx];
		if (weap!=NULL) {
			if (!weap->hidden) break;
		}
    }
    
    weapon_goto(obj,weap);
}

void weapon_pick(obj_type *obj,int index)
{
    weapon_type			*weap;
	
		// goto specific weapon
		
	weap=obj->weap_list.weaps[index];
	if (weap==NULL) return;
	
		// can we switch?

    if (weap->idx==obj->held_weapon.current_index) return;
	if (weap->hidden) return;
	
		// set weapon
    
    weapon_goto(obj,weap);
}
    
/* =======================================================

      Weapon Targetting
      
======================================================= */

void weapon_target_next_object(obj_type *obj,weapon_type *weap)
{
	int				n,dist,min_dist,cur_dist,dif,cur_dif,
					min_uid,next_uid;
	obj_type		*chk_obj;

		// get current distance

	if (weap->target.obj_uid!=-1) {
		chk_obj=server.obj_list.objs[weap->target.obj_uid];
		cur_dist=distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,chk_obj->pnt.x,chk_obj->pnt.y,chk_obj->pnt.z);
	}
	else {
		cur_dist=weap->target.distance;
	}

		// find next further object
		// or the closest object if no further objects

	min_dist=-1;
	cur_dif=-1;

	next_uid=-1;
	min_uid=-1;

	for (n=0;n!=max_obj_list;n++) {
		chk_obj=server.obj_list.objs[n];
		if (chk_obj==NULL) continue;

		if (chk_obj->hidden) continue;

		if (weap->target.opponent_only) {
			if (chk_obj->status.health==0) continue;
			if ((chk_obj->type!=object_type_remote) && (chk_obj->type!=object_type_bot_multiplayer) && (chk_obj->type!=object_type_bot_map)) continue;
		}
		
			// outside max distance?

		dist=distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,chk_obj->pnt.x,chk_obj->pnt.y,chk_obj->pnt.z);
		if (dist>weap->target.distance) continue;

			// remember closest target

		if ((min_dist==-1) || (dist<min_dist)) {
			min_dist=dist;
			min_uid=chk_obj->index;
		}

			// can't re-select same target

		if (chk_obj->index==weap->target.obj_uid) continue;

			// next further target?

		dif=dist-cur_dist;
		if (dif<0) continue;

		if ((cur_dif==-1) || (dif<cur_dif)) {
			cur_dif=dif;
			next_uid=chk_obj->index;
		}
	}

		// get next uid

	if (next_uid==-1) next_uid=min_uid;
	weap->target.obj_uid=next_uid;
}

void weapon_target_previous_object(obj_type *obj,weapon_type *weap)
{
	int				n,dist,max_dist,cur_dist,dif,cur_dif,
					max_uid,prev_uid;
	obj_type		*chk_obj;

		// get current distance

	if (weap->target.obj_uid!=-1) {
		chk_obj=server.obj_list.objs[weap->target.obj_uid];
		cur_dist=distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,chk_obj->pnt.x,chk_obj->pnt.y,chk_obj->pnt.z);
	}
	else {
		cur_dist=weap->target.distance;
	}

		// find next closest object
		// or the furthest object if no further objects

	max_dist=0;
	cur_dif=-1;

	prev_uid=-1;
	max_uid=-1;

	for (n=0;n!=max_obj_list;n++) {
		chk_obj=server.obj_list.objs[n];
		if (chk_obj==NULL) continue;

		if (chk_obj->hidden) continue;

		if (weap->target.opponent_only) {
			if (chk_obj->status.health==0) continue;
			if ((chk_obj->type!=object_type_remote) && (chk_obj->type!=object_type_bot_multiplayer) && (chk_obj->type!=object_type_bot_map)) continue;
		}
		
			// outside max distance?

		dist=distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,chk_obj->pnt.x,chk_obj->pnt.y,chk_obj->pnt.z);
		if (dist>weap->target.distance) continue;

			// remember furthest target

		if (dist>max_dist) {
			max_dist=dist;
			max_uid=chk_obj->index;
		}

			// can't re-select same target

		if (chk_obj->index==weap->target.obj_uid) continue;

			// next closest target?

		dif=cur_dist-dist;
		if (dif<0) continue;

		if ((cur_dif==-1) || (dif<cur_dif)) {
			cur_dif=dif;
			prev_uid=chk_obj->index;
		}
	}

		// get next uid

	if (prev_uid==-1) prev_uid=max_uid;
	weap->target.obj_uid=prev_uid;
}

bool weapon_target_start(obj_type *obj,weapon_type *weap,bool opponent_only)
{
	weap->target.obj_uid=-1;
	weap->target.opponent_only=opponent_only;
	
	weapon_target_next_object(obj,weap);

		// if no available targets, return false

	if (weap->target.obj_uid==-1) {
		weap->target.on=FALSE;
		return(FALSE);
	}

		// targetting on

	weap->target.on=TRUE;

	return(TRUE);
}

bool weapon_target_end(obj_type *obj,weapon_type *weap)
{
	weap->target.on=FALSE;

	return(weap->target.obj_uid!=-1);
}

/* =======================================================

      Weapon Zooming
      
======================================================= */

void weapon_zoom_enter(obj_type *obj,weapon_type *weap)
{
	weap->zoom.mode=zoom_mode_in;
	obj->zoom_draw.on=FALSE;

		// fix looking problems when zooming

	object_player_look_fix_zoom(obj,weap);

		// save old camera mode and switch to fpp

	obj->zoom_draw.old_camera_mode=camera.mode;
	obj->zoom_draw.start_tick=game_time_get();
	obj->zoom_draw.sway_reset=TRUE;
	
	camera.mode=cv_fpp;

 	scripts_post_event_console(&weap->attach,sd_event_weapon_fire,sd_event_weapon_fire_zoom_enter,0);
}

void weapon_zoom_exit(obj_type *obj,weapon_type *weap)
{
	weap->zoom.mode=zoom_mode_out;
	obj->zoom_draw.start_tick=game_time_get();

	scripts_post_event_console(&weap->attach,sd_event_weapon_fire,sd_event_weapon_fire_zoom_exit,0);
}

void weapon_zoom_off(obj_type *obj,weapon_type *weap)
{
		// switch back to original camera mode

	camera.mode=obj->zoom_draw.old_camera_mode;

		// turn off zoom

	weap->zoom.mode=zoom_mode_off;
	obj->zoom_draw.on=FALSE;
}

void weapon_zoom_key(obj_type *obj,weapon_type *weap)
{
		// starting zoom

	if (weap->zoom.mode==zoom_mode_off) {
		weapon_zoom_enter(obj,weap);
		return;
	}

		// ending zoom

	if (weap->zoom.mode==zoom_mode_on) {
		weapon_zoom_exit(obj,weap);
		return;
	}
}

/* =======================================================

      Run Player Weapons
      
======================================================= */

void weapon_run_hand(obj_type *obj)
{
    int					weap_mode,swap_tick;
	weapon_type			*weap;
	
	weap=weapon_find_current(obj);
	if (weap==NULL) return;
    
        // model animations
       
	model_run_animation(&weap->draw);
	model_fade_run(&weap->draw);
	model_mesh_fade_run(&weap->draw);

	if ((weap->dual.on) && (weap->dual.active)) {
		model_run_animation(&weap->draw_dual);
		model_fade_run(&weap->draw_dual);
		model_mesh_fade_run(&weap->draw_dual);
	}

		// recoil changes

	weapon_recoil_run(obj,weap);
    
        // raising or lowering models
	
	weap_mode=obj->held_weapon.mode;
    swap_tick=game_time_get()-obj->held_weapon.swap_tick;
        
    if (weap_mode==wm_lower) {
        if (swap_tick>weap->hand.lower_tick) {
			weapon_cur_weapon_recoil_copy(obj,obj->held_weapon.next_index);
            obj->held_weapon.current_index=obj->held_weapon.next_index;
            weapon_raise(obj);
        }
        return;
    }
    
    if (weap_mode==wm_raise) {
        if (swap_tick>weap->hand.raise_tick) {
            weapon_held(obj);
        }
        return;
    }    
}

/* =======================================================

      Run Weapon Bouncing
      
======================================================= */

void weapon_hand_bounce(obj_type *obj)
{
	float		speed;
	weapon_type	*weap;
	
		// get values
		
	weap=weapon_find_current(obj);
	if (weap==NULL) return;
	
	speed=weap->hand.bounce_speed;
	
		// return weapon to neutral
		
	if ((!obj->forward_move.moving) && (!obj->side_move.moving)) {
	
		if (obj->held_weapon.bounce_y>0) {
			obj->held_weapon.bounce_y-=(int)(speed*2.0f);
			if (obj->held_weapon.bounce_y<0) obj->held_weapon.bounce_y=0;
		}
	
		return;
	}
	
		// bounce weapon
		
	if (!obj->forward_move.running) speed=speed/2.0f;;
	
	obj->held_weapon.bounce_y+=(int)speed;
	if (obj->held_weapon.bounce_y>=360) obj->held_weapon.bounce_y=0;
}

int weapon_get_bounce(obj_type *obj,weapon_type *weap)
{
	float		f;
	
	f=(float)sin(((float)obj->held_weapon.bounce_y)*ANG_to_RAD);
	return((int)(f*weap->hand.bounce_ang));
}

