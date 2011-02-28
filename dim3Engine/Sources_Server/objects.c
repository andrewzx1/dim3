/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Creation/Dispose

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

#include "network.h"
#include "scripts.h"
#include "objects.h"
#include "weapons.h"
#include "projectiles.h"
#include "models.h"
#include "interfaces.h"
#include "consoles.h"

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern hud_type				hud;
extern js_type				js;

int							game_obj_rule_idx;

/* =======================================================

      Initialize Object List
      
======================================================= */

void object_initialize_list(void)
{
	int				n;

		// clear all objects

	for (n=0;n!=max_obj_list;n++) {
		server.obj_list.objs[n]=NULL;
	}

		// no game rule attachment

	game_obj_rule_idx=-1;
}

void object_free_list(void)
{
	int				n;

	for (n=0;n!=max_obj_list;n++) {
		if (server.obj_list.objs[n]!=NULL) free(server.obj_list.objs[n]);
	}
}

int object_count_list(void)
{
	int				n,count;

	count=0;

	for (n=0;n!=max_obj_list;n++) {
		if (server.obj_list.objs[n]!=NULL) count++;
	}

	return(count);
}

int object_count_weapons(obj_type *obj)
{
	int				n,count;

	count=0;

	for (n=0;n!=max_weap_list;n++) {
		if (obj->weap_list.weaps[n]!=NULL) count++;
	}

	return(count);
}

/* =======================================================

      Clear Object Structures
      
======================================================= */

void object_clear_size(obj_size *size)
{
	size->x=size->z=10*map_enlarge;
	size->y=20*map_enlarge;

	size->eye_offset=18*map_enlarge;

	size->radius=10*map_enlarge;
	size->weight=200;

	size->node_touch_dist=3*map_enlarge;
}

void object_clear_position(d3pnt *pnt)
{
	pnt->x=pnt->y=pnt->z=0;
}

void object_clear_angle(d3ang *ang)
{
	ang->x=ang->y=ang->z=0;
}

void object_clear_motion(obj_motion *motion)
{
	motion->last_y_change=0;
	motion->rigid_body_offset_y=0;
	motion->vct.x=motion->vct.z=motion->vct.y=0;
	motion->ang.y=motion->ang.x=motion->ang.z=0;
}

void object_clear_movement(obj_movement *move)
{
    move->speed=0;
    move->max_crawl_speed=move->max_walk_speed=move->max_run_speed=move->max_air_speed=0;
    move->accelerate=move->decelerate=move->air_accelerate=move->air_decelerate=0;
	move->slop=3.0f;
	move->seek_ang=0.0f;
	move->running=FALSE;
    move->moving=FALSE;
    move->reverse=FALSE;
	move->seeking=FALSE;
}

void object_clear_force(obj_force *force)
{
	force->vct.x=force->vct.z=force->vct.y=0.0f;
    force->gravity=gravity_start_power;
}

void object_clear_contact(obj_contact *contact)
{
	contact->hit_poly.mesh_idx=-1;
	contact->stand_poly.mesh_idx=-1;
	contact->head_poly.mesh_idx=-1;

	contact->liquid_idx=-1;
	
	contact->obj_idx=-1;
	contact->proj_idx=-1;
	contact->stand_obj_idx=-1;
	
	contact->melee=FALSE;
}

void object_clear_touch(obj_touch *touch)
{
	touch->obj_idx=-1;
	touch->last_obj_idx=-1;
	
	touch->stand=FALSE;
	
	touch->last_touch_tick=0;
	touch->last_stand=FALSE;
	
	touch->pnt.x=touch->pnt.y=touch->pnt.z=0;
	touch->ang.x=touch->ang.y=touch->ang.z=0.0f;
}

void object_clear_hit(obj_hit *hit)
{
	hit->obj_idx=-1;
	hit->weap_idx=-1;
	hit->proj_setup_idx=-1;
	
	hit->pnt.x=hit->pnt.y=hit->pnt.z=0;
	hit->ang.x=hit->ang.y=hit->ang.z=0.0f;
	
	hit->damage=0;
}

void object_clear_pickup(obj_pickup *pickup)
{
	pickup->obj_idx=-1;
	pickup->item_idx=-1;
}

void object_clear_remote(obj_remote *remote)
{
	remote->net_uid=-1;
}

void object_clear_watch(obj_watch *watch)
{
	watch->on=FALSE;
	watch->obj_idx=-1;
	watch->dist=map_enlarge*50;
	watch->watch_restrict.on=FALSE;
	watch->watch_restrict.ray_trace=FALSE;
	watch->watch_restrict.ang=0.0f;
	memset(watch->obj_flags,0x0,max_obj_list);
}

void object_clear_draw(model_draw *draw)
{
	int				k;
	
		// model draw
		
	draw->on=FALSE;
	draw->model_idx=-1;
	draw->name[0]=0x0;
	draw->bounce=FALSE;
	draw->face_forward=FALSE;
	draw->offset.x=draw->offset.z=draw->offset.y=0;
	draw->rot.x=draw->rot.z=draw->rot.y=0.0f;
	draw->spin.x=draw->spin.z=draw->spin.y=0.0f;
	draw->spin_tick=0;
	draw->alpha=1;
	draw->resize=1;
	
		// start with non-no_rot
		// drawings
		
	draw->no_rot.on=FALSE;

		// dynamic bones

	for (k=0;k!=max_model_dynamic_bone;k++) {
		draw->dynamic_bones[k].bone_idx=-1;
	}
	
		// lights
		
	draw->script_light_idx=0;

	for (k=0;k!=max_model_light;k++) {
		draw->lights[k].on=FALSE;
		draw->lights[k].type=lt_normal;
		draw->lights[k].direction=ld_all;
		draw->lights[k].intensity=0;
		draw->lights[k].exponent=1.0f;
		draw->lights[k].col.r=draw->lights[k].col.g=draw->lights[k].col.b=1;
	}
	
		// halos
		
	draw->script_halo_idx=0;

	for (k=0;k!=max_model_halo;k++) {
		draw->halos[k].on=FALSE;
		draw->halos[k].min_dist=20*map_enlarge;
		draw->halos[k].max_dist=350*map_enlarge;
		draw->halos[k].min_size=500;
		draw->halos[k].max_size=200;
		draw->halos[k].min_alpha=0.8f;
		draw->halos[k].max_alpha=0.05f;
		draw->halos[k].no_clip_object=FALSE;
		draw->halos[k].no_clip_self=TRUE;
	}

		// shadow
		
	draw->shadow.on=FALSE;

		// default normal

	draw->normal[0]=0.5f;
	draw->normal[1]=0.5f;
	draw->normal[2]=1.0f;
	
		// animate setup
		
	for (k=0;k!=max_model_blend_animation;k++) {
		draw->animations[k].mode=am_stopped;
		draw->setup.poses[k].idx_1=draw->setup.poses[k].idx_2=-1;
	}

		// mesh and textures

	draw->mesh_mask=0x1;				// default to first mesh
	
	for (k=0;k!=max_model_texture;k++) {
		draw->cur_texture_frame[k]=0;
	}

		// mesh fades

	for (k=0;k!=max_model_mesh;k++) {
		draw->meshes[k].fade.on=FALSE;
	}
}

/* =======================================================

      Object Size and Radius
      
======================================================= */

void object_set_radius(obj_type *obj)
{
	int			radius;
	
	radius=obj->size.x;
	if (obj->size.z>radius) radius=obj->size.z;
	
	obj->size.radius=radius>>1;
}

/* =======================================================

      Object Positions
      
======================================================= */

void object_set_current_mesh(obj_type *obj)
{
	obj->mesh.cur_mesh_idx=map_mesh_find(&map,&obj->pnt);
	obj->contact.stand_poly.mesh_idx=-1;
}

void object_set_position(obj_type *obj,int x,int y,int z,float ang_y,float ymove)
{
	d3pnt				*pnt;
	d3ang				*ang;
	obj_motion			*motion;
	
	pnt=&obj->pnt;
	
	pnt->x=x;
	pnt->y=y;
	pnt->z=z;
	
	memmove(&obj->last_pnt,pnt,sizeof(d3pnt));
	memmove(&obj->draw.pnt,pnt,sizeof(d3pnt));
	
	ang=&obj->ang;
	ang->x=0;
	ang->y=ang_y;
	ang->z=0;
	
	memmove(&obj->last_ang,ang,sizeof(d3ang));
	
	ang=&obj->view_ang;
	ang->x=0;
	ang->y=0;
	ang->z=0;
	
	motion=&obj->motion;
	
	motion->last_y_change=0;
	motion->ang.y=ang_y;
	motion->vct.y=ymove;
	
	obj->turn.ang_to.y=ang_y;
	
	obj->force.gravity=gravity_start_power;

	object_set_current_mesh(obj);
}

void object_stop(obj_type *obj)
{
    obj->forward_move.running=FALSE;
	
	object_clear_angle(&obj->turn.ang_add);
	object_clear_angle(&obj->turn.ang_to);
	object_clear_angle(&obj->turn.fix_ang_add);
    obj->look.ang_add=0;

	obj->face.obj_idx=-1;
	object_clear_angle(&obj->face.ang);
	
	obj->forward_move.moving=FALSE;
	obj->side_move.moving=FALSE;
	obj->vert_move.moving=FALSE;
	
	obj->forward_move.speed=0;
	obj->side_move.speed=0;
	obj->vert_move.speed=0;

	obj->motion.last_y_change=0;
	
	obj->force.vct.x=obj->force.vct.z=obj->force.vct.y=0;
	
	obj->auto_walk.mode=aw_none;
	obj->auto_walk.node_seek_idx=obj->auto_walk.node_dest_idx=-1;
	
	obj->last_move_animation_event=-1;
	obj->last_turn_animation_event=-1;
	
	obj->thrust.vct.x=obj->thrust.vct.y=obj->thrust.vct.z=0.0f;
}   

/* =======================================================

      Object Team Colors
      
======================================================= */

void object_team_get_name(int team_idx,char *str)
{
	if (team_idx==net_team_red) {
		strcpy(str,"Red");
	}
	else {
		strcpy(str,"Blue");
	}
}

void object_team_get_tint(int team_idx,d3col *tint)
{
	if (team_idx==net_team_red) {
		tint->r=1.0f;
		tint->g=0.0f;
		tint->b=0.0f;
	}
	else {
		tint->r=0.0f;
		tint->g=0.0f;
		tint->b=1.0f;
	}
}

void object_get_tint(obj_type *obj,d3col *tint)
{
	d3col		*col;

		// if in multiplayer and using teams,
		// then get team tint

	if (net_setup.mode!=net_mode_none) {
		if (hud.net_game.games[net_setup.game_idx].use_teams) {
			object_team_get_tint(obj->team_idx,tint);
			return;
		}
	}

		// otherwise use set interface color

	col=&hud.color.tints[obj->tint_color_idx];

	tint->r=col->r;
	tint->g=col->g;
	tint->b=col->b;
}

/* =======================================================

      Check if Object Sends Network Messages
      
======================================================= */

bool object_networkable(obj_type *obj)
{
		// players and bots always send messages

	if ((obj->idx==server.player_obj_idx) || (obj->type==object_type_bot_multiplayer)) return(TRUE);
	
		// map type bots only send messages if
		// this process is the host

	if (obj->type==object_type_bot_map) {
		return((net_setup.mode==net_mode_host) || (net_setup.mode==net_mode_host_dedicated));
	}
	
	return(FALSE);
}

/* =======================================================

      Create a New Object
      
======================================================= */

int object_create(char *name,int type,int bind)
{
	int				n,idx;
	obj_type		*obj;

		// find a unused object

	idx=-1;

	for (n=0;n!=max_obj_list;n++) {
		if (server.obj_list.objs[n]==NULL) {
			idx=n;
			break;
		}
	}

	if (idx==-1) return(-1);

		// create memory for new object

	obj=(obj_type*)malloc(sizeof(obj_type));
	if (obj==NULL) return(-1);

		// put in list

	server.obj_list.objs[idx]=obj;

		// initial setup
		
	strcpy(obj->name,name);

	obj->idx=idx;
	obj->type=type;
	obj->bind=bind;
	
	obj->next_spawn_sub_event=sd_event_spawn_init;

		// initialize object
	
	obj->hidden=FALSE;
	obj->suspend=FALSE;
	obj->scenery.on=FALSE;
	obj->fly=FALSE;
	obj->single_speed=FALSE;
	obj->side_step=TRUE;
	obj->quick_reverse=TRUE;
	obj->no_slide=FALSE;
	obj->crawl=TRUE;
	obj->open_doors=FALSE;
	obj->hide_all_weapons=FALSE;
	obj->find_on=TRUE;
	obj->contact.object_on=TRUE;
	obj->contact.projectile_on=TRUE;
	obj->contact.force_on=TRUE;
	obj->contact.pushable=FALSE;
	obj->hit_box.on=FALSE;
	
	obj->pickup.on=FALSE;
	obj->pickup.ignore=FALSE;
	
	obj->damage.on=FALSE;
	obj->damage.crushable=FALSE;
	obj->damage.invincible=FALSE;
	obj->damage.in_damage=FALSE;
	
	obj->tint_color_idx=0;
	obj->character_idx=0;
	
	obj->input.mode=im_fpp;
	obj->input.freeze=FALSE;
	obj->input.respawn_freeze=FALSE;
	
	obj->in_collide_event=FALSE;
	obj->death_trigger=FALSE;
	
	obj->lock.x=obj->lock.y=obj->lock.z=FALSE;
	
	obj->damage_obj_idx=-1;
	
	obj->team_idx=net_team_none;
	obj->spawn_spot_name[0]=0x0;
	
	obj->click.on=FALSE;
	obj->click.distance=map_enlarge*10;
	obj->click.crosshair_up_idx=-1;
	obj->click.crosshair_down_idx=-1;
	
	object_clear_size(&obj->size);
	object_clear_position(&obj->pnt);
	object_clear_angle(&obj->ang);
	object_clear_motion(&obj->motion);
	object_clear_force(&obj->force);
	object_clear_movement(&obj->forward_move);
    object_clear_movement(&obj->side_move);
	object_clear_movement(&obj->vert_move);
	object_stop(obj);

	obj->sight.side_angle=20.0f;
	obj->sight.look_angle=0.0f;
	obj->sight.side_division=3;
	obj->sight.look_division=1;
	obj->sight.distance=map_enlarge*150;
	
    obj->bump.on=TRUE;
    obj->bump.high=450;
	obj->bump.smooth_factor=0.1f;
	obj->bump.smooth_offset=0;

	obj->slope_gravity=TRUE;
	
	obj->bounce.mesh_idx=-1;
	obj->bounce.factor=0.0f;
	
	obj->turn.ignore_mouse=FALSE;
	obj->turn.only_when_moving=FALSE;
	obj->turn.restrict_player_turning=FALSE;

	obj->face.obj_idx=-1;
	object_clear_angle(&obj->face.ang);
	
	obj->thrust.speed=0.5f;
	obj->thrust.max_speed=60.0f;
	obj->thrust.vct.x=obj->thrust.vct.y=obj->thrust.vct.z=0.0f;
	obj->thrust.drag=FALSE;
	
	obj->look.ang_add=0;
	obj->look.up_angle=80;
	obj->look.down_angle=35;
	obj->look.effect_weapons=TRUE;
	
	obj->duck.on=TRUE;
    obj->duck.mode=dm_stand;
    obj->duck.y_move=0;
    obj->duck.y_add=16;
    obj->duck.y_change=300;
	
	obj->jump.on=TRUE;
    obj->jump.y_add=32;
	
	obj->climb.on=FALSE;
	obj->climb.poly_ptr.mesh_idx=-1;
	
	obj->kickback.size=100;

	obj->vehicle.on=FALSE;
	obj->vehicle.use_vehicles=TRUE;
	obj->vehicle.in_enter=obj->vehicle.in_exit=FALSE;
	obj->vehicle.attach_obj_idx=-1;

	obj->radar.on=FALSE;
	obj->radar.icon[0]=0x0;
	obj->radar.icon_idx=-1;
	obj->radar.motion_only=FALSE;
	obj->radar.always_visible=FALSE;
	obj->radar.fade_start_tick=0;
	
	obj->status.health=obj->status.start_health=obj->status.max_health=100;
	obj->status.health_recover_tick=obj->status.health_recover_count=0;
	obj->status.health_recover_amount=1;
	obj->status.health_factor=1.0f;
	obj->status.mesh_harm_count=0;
	
	object_clear_draw(&obj->draw);
	obj->ambient.on=FALSE;
    
    obj->held_weapon.mode=wm_held;
    obj->held_weapon.swap_tick=0;
	obj->held_weapon.current_idx=obj->held_weapon.next_idx=-1;
	obj->held_weapon.bounce_y=0;
	
	obj->crosshair_draw.on=FALSE;
	obj->crosshair_draw.alt_tick=0;

	obj->melee.strike_bone_tag=model_null_tag;
	obj->melee.strike_pose_name[0]=0x0;
	obj->melee.object_strike_bone_tag=model_null_tag;
	obj->melee.object_strike_pose_name[0]=0x0;
	obj->melee.radius=0;
	obj->melee.distance=0;
	obj->melee.damage=0;
	obj->melee.force=0;
	
	obj->hit_box.obj_hit_box_idx=-1;
	obj->hit_box.proj_hit_box_idx=-1;
	
	obj->count=0;
	obj->item_count=0;

	obj->turn.ang_add.y=0;
	obj->turn.fix_ang_add.y=0;
	obj->turn.ang_to.y=0;
	obj->turn.walk_speed=obj->turn.run_speed=obj->turn.crawl_speed=obj->turn.air_speed=1;
	obj->turn.walk_motion_speed=obj->turn.run_motion_speed=obj->turn.crawl_motion_speed=obj->turn.air_motion_speed=1;
	obj->turn.key_speed=1.8f;
	obj->turn.top_down_ang_offset=0.0f;

	obj->air_mode=am_ground;

    obj->liquid.mode=lm_out;
	obj->liquid.bob_y_move=0;

	obj->fall.dist=0;
	obj->fall.damage_minimum_height=map_enlarge*20;
	obj->fall.damage_factor=0.0f;
    obj->fall.change=FALSE;
	
	obj->last_move_animation_event=-1;
	obj->last_turn_animation_event=-1;

	obj->auto_walk.mode=aw_none;

	obj->debug.str[0]=0x0;
	
	object_clear_contact(&obj->contact);
	object_clear_touch(&obj->touch);
	object_clear_hit(&obj->hit);
	object_clear_pickup(&obj->pickup);
	object_clear_watch(&obj->watch);

	object_clear_remote(&obj->remote);

	object_grow_clear(obj);
	
	obj->score.kill=obj->score.death=obj->score.suicide=obj->score.goal=obj->score.score=0;
	obj->score.place=1;
	
		// clear weapons

	for (n=0;n!=max_weap_list;n++) {
		obj->weap_list.weaps[n]=NULL;
	}
	
		// connections for animated effects
		
	obj->draw.connect.obj_idx=idx;
	obj->draw.connect.weap_idx=-1;
	obj->draw.connect.proj_idx=-1;
	obj->draw.connect.net_sound=FALSE;
	obj->draw.connect.motion_vct.x=0.0f;
	obj->draw.connect.motion_vct.y=0.0f;
	obj->draw.connect.motion_vct.z=0.0f;
	
	return(idx);
}

/* =======================================================

      Scripts for Objects
      
======================================================= */

bool object_start_script(obj_type *obj,char *err_str)
{
	scripts_clear_attach(&obj->attach,thing_type_object);
	
	obj->attach.obj_idx=obj->idx;

		// was it a non-script scenery
		// this is usually something set when re-loading
		// state from a saved file

	if (obj->scenery.on) {
		obj->attach.script_idx=-1;
		return(TRUE);
	}

		// if player or remote, use player script

	if ((obj->type==object_type_player) || (obj->type==object_type_remote)) return(scripts_add(&obj->attach,"Objects","Player",err_str));

		// otherwise use script setup by spot

	return(scripts_add(&obj->attach,"Objects",obj->spot_script,err_str));
}

/* =======================================================

      Reset Object to Original Spawn Spot
      
======================================================= */

void object_reset_prepare(obj_type *obj)
{
	memmove(&obj->org_pnt,&obj->pnt,sizeof(d3pnt));
	memmove(&obj->org_ang,&obj->ang,sizeof(d3ang));
}

void object_reset(obj_type *obj)
{
	object_stop(obj);
	
	memmove(&obj->pnt,&obj->org_pnt,sizeof(d3pnt));	
	memmove(&obj->ang,&obj->org_ang,sizeof(d3ang));
	
	memmove(&obj->last_pnt,&obj->pnt,sizeof(d3pnt));
	memmove(&obj->last_ang,&obj->ang,sizeof(d3ang));
	
	memmove(&obj->draw.pnt,&obj->pnt,sizeof(d3pnt));

	obj->motion.ang.x=obj->motion.ang.z=0.0f;
	obj->motion.ang.y=obj->turn.ang_to.y=obj->ang.y;
	obj->motion.last_y_change=0;
	obj->motion.vct.x=obj->motion.vct.y=obj->motion.vct.z=0.0f;
	
	obj->force.gravity=gravity_start_power;
	
	object_set_current_mesh(obj);
}

/* =======================================================

      Attaching Crosshairs
      
======================================================= */

void object_attach_click_crosshair_up(obj_type *obj)
{
	obj->click.crosshair_up_idx=crosshair_find(obj->click.crosshair_up_name);
}

void object_attach_click_crosshair_down(obj_type *obj)
{
	obj->click.crosshair_down_idx=crosshair_find(obj->click.crosshair_down_name);
}

/* =======================================================

      Run Game Rules for Objects
      
======================================================= */

void object_run_game_rules(obj_type *obj)
{
	game_obj_rule_idx=obj->idx;
	scripts_post_event_console(&js.game_attach,sd_event_rule,sd_event_rule_join,0);
	game_obj_rule_idx=-1;
}

/* =======================================================

      Start Objects
      
======================================================= */

int object_start(spot_type *spot,char *name,int type,int bind,char *err_str)
{
	int					idx;
	obj_type			*obj;
	weapon_type			*weap;

		// create object
		
	idx=object_create(name,type,bind);
	if (idx==-1) {
		strcpy(err_str,"Out of memory");
		return(-1);
	}

	obj=server.obj_list.objs[idx];

		// player default setup
		
	if (obj->type==object_type_player) {
		obj->team_idx=net_team_none;
		obj->spawn_spot_name[0]=0x0;
		
		obj->hidden=FALSE;
		
		obj->tint_color_idx=setup.network.tint_color_idx;
		obj->character_idx=setup.network.character_idx;

		server.player_obj_idx=obj->idx;
	}

		// regular object setup

	else {

			// if there's an editor display model, then
			// default model to it
			
		if (spot->display_model[0]!=0x0) {
			obj->draw.on=TRUE;
			strcpy(obj->draw.name,spot->display_model);
		}

			// attach object to spot

		object_set_position(obj,spot->pnt.x,spot->pnt.y,spot->pnt.z,spot->ang.y,0);
		obj->turn.ang_to.y=spot->ang.y;
	}

		// parameters

	obj->spot_script[0]=0x0;
	obj->spot_params[0]=0x0;
	if (spot!=NULL) {
		strcpy(obj->spot_script,spot->script);
		strcpy(obj->spot_params,spot->params);
	}

		// if networked player or multiplayer bot, run rules
		// to pick the team
	
	if (net_setup.mode!=net_mode_none) {
		if ((obj->type==object_type_player) || (obj->type==object_type_bot_multiplayer)) object_run_game_rules(obj);
	}
		
		// start script

	if (!object_start_script(obj,err_str)) {
		console_add_error(err_str);
		server.obj_list.objs[idx]=NULL;
		free(obj);
		return(-1);
	}

		// load object model

	if (!model_draw_load(&obj->draw,"Object",obj->name,err_str)) {
		server.obj_list.objs[idx]=NULL;
		free(obj);
		return(-1);
	}

		// setup held weapon

	if (obj->held_weapon.current_idx!=-1) {
		weap=weapon_find_current(obj);
		weapon_set(obj,weap);
	}

	return(obj->idx);
}

/* =======================================================

      Dispose Objects
      
======================================================= */

void object_dispose_single(int idx)
{
	int					n;
	obj_type			*obj;

	obj=server.obj_list.objs[idx];

		// dispose weapons

	for (n=0;n!=max_weap_list;n++) {
		weapon_dispose(obj,n);
	}

		// clear scripts and models

	scripts_dispose(obj->attach.script_idx);
	model_draw_dispose(&obj->draw);

		// free and empty from list

	free(obj);
	server.obj_list.objs[idx]=NULL;
}

void object_dispose_2(int bind)
{
	int				n;
	obj_type		*obj;
	
		// delete bound objects

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
	
		if (obj->bind==bind) object_dispose_single(n);
	}
}

void object_dispose_all(void)
{
	int				n;
	obj_type		*obj;

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj!=NULL) object_dispose_single(n);
	}
}

/* =======================================================

      Script Object Spawn/Remove
      
======================================================= */

int object_script_spawn(char *name,char *type,char *script,char *params,d3pnt *pnt,d3ang *ang,bool hide,char *err_str)
{
	int					idx,spawn_type;
	char				obj_err_str[256];
	spot_type			spot;
	obj_type			*obj;
	
		// get type, only allowed to
		// spawn objects and bots
		
	spawn_type=object_type_object;
	if (strcasecmp(type,"bot")==0) spawn_type=object_type_bot_map;
	
		// create fake spot

	bzero(&spot,sizeof(spot_type));

	strcpy(spot.name,name);
	strcpy(spot.script,script);
	strcpy(spot.params,params);

	memmove(&spot.pnt,&pnt,sizeof(d3pnt));
	memmove(&spot.ang,&ang,sizeof(d3ang));

		// start object

	idx=object_start(&spot,name,spawn_type,bt_map,obj_err_str);
	if (idx==-1) {
		sprintf(err_str,"Object Spawn Failed: %s",obj_err_str);
		console_add_error(err_str);
		
		return(-1);
	}

		// hide object

	if (hide) {
		obj=server.obj_list.objs[idx];

		obj->hidden=TRUE;
		obj->contact.object_on=FALSE;
		obj->contact.projectile_on=FALSE;
		obj->contact.force_on=FALSE;
	}

		// return index

	return(idx);
}

bool object_script_remove(int idx,char *err_str)
{
		// can not dispose player object

	if (idx==server.player_obj_idx) {
		strcpy(err_str,"Can not dispose player object");
		return(FALSE);
	}
	
		// does this object exist?
	
	if (server.obj_list.objs[idx]==NULL) {
		sprintf(err_str,"No object exists with ID: %d",idx);
		return(FALSE);
	}

		// dispose object

	object_dispose_single(idx);

	return(TRUE);
}
