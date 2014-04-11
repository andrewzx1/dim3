/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Weapon Projectiles

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "network.h"
#include "scripts.h"
#include "objects.h"

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern view_type			view;
extern setup_type			setup;
extern network_setup_type	net_setup;

/* =======================================================

      Setup Weapon Fire Script Objects
      
======================================================= */

void weapon_setup_fire(weapon_type *weap,int method)
{
	obj_type		*obj;
	
		// setup fire method
		
	weap->fire.method=method;

		// not canceled

	weap->fire.cancel=FALSE;
	
		// setup weapon fire in object
		
	obj=server.obj_list.objs[weap->obj_idx];

	strcpy(obj->weapon_fire.name,weap->name);
	obj->weapon_fire.method=method;
}

/* =======================================================

      Add Projectile
      
======================================================= */

bool weapon_add_projectile(obj_type *obj,weapon_type *weap,proj_setup_type *proj_setup,d3pnt *pt,d3ang *ang)
{
	d3pnt					spt,ept;
	proj_type				*proj;
	ray_trace_contact_type	contact;
	
		// create new projectile
		
	proj=projectile_create(obj,weap,proj_setup,FALSE);
	if (proj==NULL) return(FALSE);

	projectile_spawn_position(proj,pt,ang,obj);
	
		// call spawn
		
	scripts_post_event_console(proj->script_idx,proj->idx,sd_event_spawn,sd_event_spawn_init,0);
	
		// if this object is the player or multiplayer\map bot,
		// then spawn projectile in remotes
		
	if (net_setup.mode!=net_mode_none) {
		if (object_networkable(obj)) net_client_send_projectile_add(obj,weap->name,proj_setup->name,pt,ang);
	}
	
		// setup model drawing, we need
		// this in case there are effects on
		// a non-running projectile
		
	model_draw_setup_projectile(proj);

		// add in object motion

	proj->force.vct.x=obj->motion.vct.x*proj_setup->inherit_motion_factor;
	proj->force.vct.y=obj->motion.vct.y*proj_setup->inherit_motion_factor;
	proj->force.vct.z=obj->motion.vct.z*proj_setup->inherit_motion_factor;

		// detect weapons being fired into walls
		// if so, destroy projectile

	spt.x=obj->pnt.x;
	spt.y=obj->pnt.y-(obj->size.y>>1);
	spt.z=obj->pnt.z;

	ept.x=proj->pnt.x;
	ept.y=proj->pnt.y;
	ept.z=proj->pnt.z;
	
	contact.obj.on=TRUE;
	contact.proj.on=FALSE;
	contact.obj.ignore_idx=obj->idx;

	contact.origin=poly_ray_trace_origin_projectile;
	
	if (ray_trace_map_by_point(&spt,&ept,&contact)) {

		proj->pnt.x=contact.hpt.x;
		proj->pnt.y=contact.hpt.y;
		proj->pnt.z=contact.hpt.z;
		
		proj->contact.hit_poly.mesh_idx=contact.poly.mesh_idx;
		proj->contact.hit_poly.poly_idx=contact.poly.poly_idx;
		proj->contact.obj_idx=contact.obj.idx;
		proj->contact.proj_idx=-1;

		projectile_hit(proj);
	}

	return(TRUE);
}

/* =======================================================

      Get Projectile Fire Position and Angle
      
======================================================= */

bool weapon_get_projectile_position_angle_weapon_model(obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,d3ang *fire_ang,char *err_str)
{
	int						pose_idx,bone_idx;
	model_type				*mdl;
	model_draw				*draw;
	model_draw_setup		*setup;

		// get model

	if (weap->draw.model_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has no model");
		return(FALSE);
	}

	mdl=server.model_list.models[weap->draw.model_idx];

		// get current pose

	pose_idx=model_find_pose(mdl,weap->proj.fire_pose_name);
	if (pose_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has missing or no fire pose");
		return(FALSE);
	}
	
		// get 'fire' bone offset and calc

	bone_idx=model_find_bone(mdl,weap->proj.fire_bone_name);
	if (bone_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has missing or no fire bone");
		return(FALSE);
	}

	model_draw_setup_weapon(obj,weap,TRUE,weap->dual.in_dual);

	if (weap->dual.in_dual) {
		draw=&weap->draw_dual;
	}
	else {
		draw=&weap->draw;
	}

	setup=&draw->setup;

	setup->move.x=setup->move.y=setup->move.z=0;
	setup->sway.x=setup->sway.y=setup->sway.z=0.0f;

	model_calc_draw_bone_position(mdl,setup,pose_idx,bone_idx,fire_pnt);

	if (draw->flip_x) fire_pnt->x=-fire_pnt->x;
	
	fire_pnt->x+=draw->pnt.x;
	fire_pnt->y+=(draw->pnt.y+obj->duck.y_move);
	fire_pnt->z+=draw->pnt.z;
	
		// fire angle

	fire_ang->x=setup->ang.x;
	fire_ang->y=setup->ang.y;
	fire_ang->z=setup->ang.z;
	
	if (draw->flip_x) fire_ang->y=angle_add(fire_ang->y,180.0f);

	return(TRUE);
}

bool weapon_get_projectile_position_angle_weapon_barrel(obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,d3ang *fire_ang,char *err_str)
{
	int						pose_idx,bone_idx,dist;
	d3pnt					barrel_pnt;
	model_type				*mdl;
	model_draw				*draw;
	model_draw_setup		*setup;

		// get weapon model

	if (weap->draw.model_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has no model");
		return(FALSE);
	}

	mdl=server.model_list.models[weap->draw.model_idx];

		// get current pose

	pose_idx=model_find_pose(mdl,weap->proj.fire_pose_name);
	if (pose_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has missing or no fire pose");
		return(FALSE);
	}
	
		// get 'fire' bone offset and calc

	bone_idx=model_find_bone(mdl,weap->proj.fire_bone_name);
	if (bone_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has missing or no fire bone");
		return(FALSE);
	}

	model_draw_setup_weapon(obj,weap,TRUE,weap->dual.in_dual);

	if (weap->dual.in_dual) {
		draw=&weap->draw_dual;
	}
	else {
		draw=&weap->draw;
	}

	setup=&draw->setup;

	setup->move.x=setup->move.y=setup->move.z=0;
	setup->sway.x=setup->sway.y=setup->sway.z=0.0f;

	model_calc_draw_bone_position(mdl,setup,pose_idx,bone_idx,fire_pnt);

	if (draw->flip_x) fire_pnt->x=-fire_pnt->x;

	fire_pnt->x+=draw->pnt.x;
	fire_pnt->y+=(draw->pnt.y+obj->duck.y_move);
	fire_pnt->z+=draw->pnt.z;

		// get 'barrel' bone offset (draw bones already calced above)

	bone_idx=model_find_bone(mdl,weap->proj.barrel_bone_name);
	if (bone_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Weapon has missing or no barrel bone");
		return(FALSE);
	}

	model_get_draw_bone_position(setup,bone_idx,&barrel_pnt);

	if (draw->flip_x) barrel_pnt.x=-barrel_pnt.x;

	barrel_pnt.x+=draw->pnt.x;
	barrel_pnt.y+=(draw->pnt.y+obj->duck.y_move);
	barrel_pnt.z+=draw->pnt.z;

		// angles between them

	dist=distance_2D_get(0,0,(fire_pnt->x-barrel_pnt.x),(fire_pnt->z-barrel_pnt.z));
	fire_ang->x=-(180.0f-angle_find(0,0,(fire_pnt->y-barrel_pnt.y),dist));
	fire_ang->y=angle_find(barrel_pnt.x,barrel_pnt.z,fire_pnt->x,fire_pnt->z);
	fire_ang->z=0.0f;

	return(TRUE);
}

bool weapon_get_projectile_position_angle_object_model(obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,d3ang *fire_ang,char *err_str)
{
	int						pose_idx,bone_idx;
	model_type				*mdl;
	model_draw_setup		*setup;
	
		// get model

	if (obj->draw.model_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Object has no model");
		return(FALSE);
	}

	mdl=server.model_list.models[obj->draw.model_idx];

		// get current pose

	pose_idx=model_find_pose(mdl,weap->proj.object_fire_pose_name);
	if (pose_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Object has missing or no fire pose");
		return(FALSE);
	}
	
		// get bone offset and calc

	bone_idx=model_find_bone(mdl,weap->proj.object_fire_bone_name);
	if (bone_idx==-1) {
		if (err_str!=NULL) strcpy(err_str,"Object has missing or no fire bone");
		return(FALSE);
	}

	model_draw_setup_object(obj);

	setup=&obj->draw.setup;

	setup->move.x=setup->move.y=setup->move.z=0;
	setup->sway.x=setup->sway.y=setup->sway.z=0.0f;

	model_calc_draw_bone_position(mdl,setup,pose_idx,bone_idx,fire_pnt);

		// move fire point in front of obj

	fire_pnt->x+=obj->draw.pnt.x;
	fire_pnt->y+=(obj->draw.pnt.y+obj->duck.y_move);
	fire_pnt->z+=obj->draw.pnt.z;
	
		// angle from object
		
	fire_ang->x=angle_add(setup->ang.x,obj->view_ang.x);
	fire_ang->y=setup->ang.y;
	fire_ang->z=setup->ang.z;

	return(TRUE);
}

void weapon_get_projectile_position_center(obj_type *obj,weapon_type *weap,d3pnt *fire_pnt,d3ang *fire_ang)
{
		// model point

	fire_pnt->x=obj->pnt.x;
	fire_pnt->y=(obj->pnt.y+obj->duck.y_move)+obj->size.eye_offset;
	fire_pnt->z=obj->pnt.z;

		// get fire the angle

	fire_ang->x=-obj->view_ang.x;
	fire_ang->y=obj->ang.y;
	fire_ang->z=0.0f;

	weapon_recoil_add(obj,weap,fire_ang);
}

void weapon_get_projectile_position_angle_add_offset(d3ang *ang,d3ang *off_ang)
{
	if (off_ang==NULL) return;
	
	ang->x+=off_ang->x;
	ang->y+=off_ang->y;
	ang->z+=off_ang->z;
}

/* =======================================================

      Obj.Weapon.Fire Method
      
======================================================= */

bool weapon_script_fire(obj_type *obj,weapon_type *weap,int method)
{
		// fail under liquid?

	if ((weap->fail_in_liquid) && (obj->liquid.mode==lm_under)) return(FALSE);

		// send event

	weapon_setup_fire(weap,method);
	scripts_post_event_console(weap->script_idx,-1,sd_event_weapon_fire,sd_event_weapon_fire_single,0);

	if (weap->fire.cancel) return(FALSE);

	if (!weap->dual.in_dual) {
		weap->fire.last_fire_tick=game_time_get();
	}
	else {
		weap->fire.last_fire_dual_tick=game_time_get();
	}
	
	return(TRUE);
}

/* =======================================================

      Weapon.Projectile.Spawn Methods
      
======================================================= */

bool weapon_script_projectile_spawn(obj_type *obj,weapon_type *weap,char *proj_name,d3pnt *pt,d3ang *ang,int count,float slop,char *err_str)
{
	int					n;
	float				r_slop;
	d3ang				proj_ang;
	proj_setup_type		*proj_setup;

		// fail under liquid?

	if ((weap->fail_in_liquid) && (obj->liquid.mode==lm_under)) return(TRUE);

		// get projectile setup

	proj_setup=find_proj_setups(weap,proj_name);
	if (proj_setup==NULL) {
		sprintf(err_str,"Named projectile does not exist in weapon: %s",proj_name);
		return(FALSE);
	}

		// create projectiles

	for (n=0;n<count;n++) {

		memmove(&proj_ang,ang,sizeof(d3ang));

			// add in slop

		if (slop!=0.0f) {
			r_slop=slop*2.0f;
			proj_ang.x+=(slop-random_float(r_slop));
			proj_ang.y+=(slop-random_float(r_slop));
			proj_ang.z+=(slop-random_float(r_slop));
		}

			// zoom sways

		if (weap->zoom.mode==zoom_mode_on) {
			proj_ang.x+=obj->zoom_draw.sway_ang.x;
			proj_ang.y+=obj->zoom_draw.sway_ang.y;
		}

			// hit-scan fires
			
		if (proj_setup->hitscan.on) {
			projectile_hitscan(obj,weap,proj_setup,pt,&proj_ang);
		}
		
			// projectile fires
			
		else {

			if (!weapon_add_projectile(obj,weap,proj_setup,pt,&proj_ang)) {
				strcpy(err_str,"No more spots to spawn projectiles");
				return(FALSE);
			}

		}
	}

	return(TRUE);
}

/* =======================================================

      Weapon.Projectile.Spawn Methods (Object Models)
      
======================================================= */

bool weapon_script_projectile_spawn_object_model(obj_type *obj,weapon_type *weap,char *proj_name,int count,float slop,d3ang *off_ang,char *err_str)
{
	d3pnt			pt;
	d3ang			ang;

		// get fire position and angle

	if (!weapon_get_projectile_position_angle_object_model(obj,weap,&pt,&ang,err_str)) return(FALSE);

	weapon_get_projectile_position_angle_add_offset(&ang,off_ang);

		// spawn projectile
		
	return(weapon_script_projectile_spawn(obj,weap,proj_name,&pt,&ang,count,slop,err_str));
}

/* =======================================================

      Weapon.Projectile.Spawn Methods (Center)
      
======================================================= */

bool weapon_script_projectile_spawn_center(obj_type *obj,weapon_type *weap,char *proj_name,int count,float slop,d3ang *off_ang,char *err_str)
{
	d3pnt			pt;
	d3ang			ang;

	weapon_get_projectile_position_center(obj,weap,&pt,&ang);
	return(weapon_script_projectile_spawn(obj,weap,proj_name,&pt,&ang,count,slop,err_str));
}

/* =======================================================

      Weapon.Projectile.Spawn Methods (Weapon Models)
      
======================================================= */

bool weapon_script_projectile_spawn_weapon_model(obj_type *obj,weapon_type *weap,char *proj_name,int count,float slop,d3ang *off_ang,char *err_str)
{
	d3pnt			pt;
	d3ang			ang;

		// if not in fpp, auto-switch to center spawn

	if (map.camera.camera_mode!=cv_fpp) {
		return(weapon_script_projectile_spawn_center(obj,weap,proj_name,count,slop,off_ang,err_str));
	}

		// get fire position and angle

	if (!weapon_get_projectile_position_angle_weapon_model(obj,weap,&pt,&ang,err_str)) return(FALSE);

	weapon_get_projectile_position_angle_add_offset(&ang,off_ang);

		// spawn projectile
		
	return(weapon_script_projectile_spawn(obj,weap,proj_name,&pt,&ang,count,slop,err_str));
}

bool weapon_script_projectile_spawn_weapon_barrel(obj_type *obj,weapon_type *weap,char *proj_name,int count,float slop,d3ang *off_ang,char *err_str)
{
	d3pnt			pt;
	d3ang			ang;

		// if not in fpp, auto-switch to center spawn

	if (map.camera.camera_mode!=cv_fpp) {
		return(weapon_script_projectile_spawn_center(obj,weap,proj_name,count,slop,off_ang,err_str));
	}
	
		// get fire position and angle

	if (!weapon_get_projectile_position_angle_weapon_barrel(obj,weap,&pt,&ang,err_str)) return(FALSE);
	
	weapon_get_projectile_position_angle_add_offset(&ang,off_ang);

		// spawn projectile

	return(weapon_script_projectile_spawn(obj,weap,proj_name,&pt,&ang,count,slop,err_str));
}

/* =======================================================

      Player Fire Weapons
      
======================================================= */

void weapon_player_fire_down(obj_type *obj,weapon_type *weap,int method)
{
	int			tick,clicked_obj_idx;
	obj_type	*clicked_obj;
	
		// if no weapon, send message directly to object
		
	if (weap==NULL) {
		scripts_post_event_console(obj->script_idx,-1,sd_event_weapon_fire,sd_event_weapon_fire_down,0);
		return;
	}
	
		// disable repeats unless weapon fires
		
	weap->proj.repeat_ok=FALSE;
	
		// if crosshair in click mode, the click
		
	clicked_obj_idx=obj->click.current_click_obj_idx;
	if (clicked_obj_idx!=-1) {
		clicked_obj=server.obj_list.objs[clicked_obj_idx];
		object_click(clicked_obj,obj);
		crosshair_show_alt(obj);
		return;
	}

		// fail under liquid?

	if ((weap->fail_in_liquid) && (obj->liquid.mode==lm_under)) return;
	
		// handle weapon fire down
		
	tick=game_time_get();
		
	weap->proj.next_repeat_tick=tick+weap->proj.repeat_tick;
	
	weapon_setup_fire(weap,method);
	
        // send fire message to weapon first
		
	weap->fire.cancel=FALSE;
	scripts_post_event_console(weap->script_idx,-1,sd_event_weapon_fire,sd_event_weapon_fire_down,0);
	if (weap->fire.cancel) return;
	
		// set last fire
	
	if (!weap->dual.in_dual) {
		weap->fire.last_fire_tick=tick;
	}
	else {
		weap->fire.last_fire_dual_tick=tick;
	}

	weap->proj.repeat_ok=TRUE;
   
        // if weapon OKs message, send message to player
	
	scripts_post_event_console(obj->script_idx,-1,sd_event_weapon_fire,sd_event_weapon_fire_down,0);
}

void weapon_player_fire_repeat(obj_type *obj,weapon_type *weap)
{
	int				tick;
	
		// if no weapon then no repeat
		
	if (weap==NULL) return;
	
		// are repeats allowed?
		
	if (!weap->proj.repeat_ok) return;
	
		// no fire messages if crosshair is in click mode
		
	if (obj->click.current_click_obj_idx!=-1) return;

		// fail under liquid?

	if ((weap->fail_in_liquid) && (obj->liquid.mode==lm_under)) return;
	
		// time to repeat?
	
	tick=game_time_get();
	
	if (tick<weap->proj.next_repeat_tick) return;
	weap->proj.next_repeat_tick+=weap->proj.repeat_tick;
	
		// setup firing
		
	weapon_setup_fire(weap,weap->fire.method);
	
        // send fire message to weapon first
		
	weap->fire.cancel=FALSE;
	scripts_post_event_console(weap->script_idx,-1,sd_event_weapon_fire,sd_event_weapon_fire_repeat,0);
	if (weap->fire.cancel) return;
	
		// set last fire
		
	if (!weap->dual.in_dual) {
		weap->fire.last_fire_tick=tick;
	}
	else {
		weap->fire.last_fire_dual_tick=tick;
	}
    
        // if weapon OKs message, send message to player
        
	scripts_post_event_console(obj->script_idx,-1,sd_event_weapon_fire,sd_event_weapon_fire_repeat,0);
}

void weapon_player_fire_up(obj_type *obj,weapon_type *weap,int method)
{
		// if no weapon, send message directly to object
		
	if (weap==NULL) {
		scripts_post_event_console(obj->script_idx,-1,sd_event_weapon_fire,sd_event_weapon_fire_up,0);
		return;
	}
	
		// no fire messages if crosshair is in click mode
		
	if (obj->click.current_click_obj_idx!=-1) return;

		// fail under liquid?

	if ((weap->fail_in_liquid) && (obj->liquid.mode==lm_under)) return;
	
		// handle weapon fire up

	weapon_setup_fire(weap,method);
	
        // send fire message to weapon first
		
	weap->fire.cancel=FALSE;
	scripts_post_event_console(weap->script_idx,-1,sd_event_weapon_fire,sd_event_weapon_fire_up,0);
	if (weap->fire.cancel) return;
    
        // if weapon OKs message, send message to player
        
	scripts_post_event_console(obj->script_idx,-1,sd_event_weapon_fire,sd_event_weapon_fire_up,0);
}

/* =======================================================

      Kickback Weapons
      
======================================================= */

void weapon_kickback(obj_type *obj,weapon_type *weap)
{
	d3ang			ang;

	ang.x=0;
	ang.y=angle_add(obj->ang.y,180.0f);
	ang.z=0;

	object_shove(obj,&ang,(float)weap->kickback.size);
}

/* =======================================================

      Clear and Run Recoils

======================================================= */

void weapon_recoil_clear(weapon_type *weap)
{
	weap->recoil.ang.x=0.0f;
	weap->recoil.ang.y=0.0f;
	weap->recoil.ang.z=0.0f;
}

void weapon_recoil_copy(weapon_type *to_weap,weapon_type *from_weap)
{
	to_weap->recoil.ang.x=from_weap->recoil.ang.x;
	to_weap->recoil.ang.y=from_weap->recoil.ang.y;
	to_weap->recoil.ang.z=from_weap->recoil.ang.z;
}

float weapon_recoil_run_single_angle(float ang,float pct)
{
	ang*=(1.0f-pct);

	if ((ang<0.0f) && (ang>=-pct)) return(0.0f);
	if ((ang>0.0f) && (ang<=pct)) return(0.0f);

	return(ang);
}

void weapon_recoil_run(obj_type *obj,weapon_type *weap)
{
	float			look_add,turn_add;

		// regular weapon recoils

	weap->recoil.ang.x=weapon_recoil_run_single_angle(weap->recoil.ang.x,weap->recoil.reset_ang.x);
	weap->recoil.ang.y=weapon_recoil_run_single_angle(weap->recoil.ang.y,weap->recoil.reset_ang.y);
	weap->recoil.ang.z=weapon_recoil_run_single_angle(weap->recoil.ang.z,weap->recoil.reset_ang.z);

		// looking always reduces x recoil

	look_add=obj->look.ang_add;
    if (look_add!=0) {
		weap->recoil.ang.x=weapon_recoil_run_single_angle(weap->recoil.ang.x,weap->recoil.look_reset);
	}

		// turning always reduces x recoil

	turn_add=obj->turn.ang_add.y;
    if (turn_add!=0) {
		weap->recoil.ang.y=weapon_recoil_run_single_angle(weap->recoil.ang.y,weap->recoil.turn_reset);
	}
}

void weapon_recoil_add(obj_type *obj,weapon_type *weap,d3ang *ang)
{
		// no recoil if not in fpp
		
	if (map.camera.camera_mode!=cv_fpp) return;
	
		// add in recoil
		
	ang->x=object_player_look_constrain(obj,weap,(ang->x+weap->recoil.ang.x));
	ang->y=angle_add(ang->y,weap->recoil.ang.y);
	ang->z+=weap->recoil.ang.z;
}

/* =======================================================

      Launch Recoils

======================================================= */

float weapon_recoil_single_angle(float min,float max)
{
	float			dif,sign;
	
	if (min==max) return(min);
	
	sign=1.0f;
	dif=max-min;
	
	if (dif<0.0f) {
		sign=-1.0f;
		dif=fabsf(dif);
	}

	return(min+(random_float(dif)*sign));
}

void weapon_recoil(obj_type *obj,weapon_type *weap)
{
	d3ang			ang;

	ang.x=weapon_recoil_single_angle(weap->recoil.min_ang.x,weap->recoil.max_ang.x);
	ang.y=weapon_recoil_single_angle(weap->recoil.min_ang.y,weap->recoil.max_ang.y);
	ang.z=weapon_recoil_single_angle(weap->recoil.min_ang.z,weap->recoil.max_ang.z);

	weap->recoil.ang.x=object_player_look_constrain(obj,weap,(weap->recoil.ang.x-ang.x));
	weap->recoil.ang.y+=ang.y;
	weap->recoil.ang.z+=ang.z;
}
