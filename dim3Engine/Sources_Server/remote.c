/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Remote Player Objects

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

#include "network.h"
#include "scripts.h"
#include "objects.h"
#include "remotes.h"
#include "weapons.h"
#include "projectiles.h"
#include "models.h"
#include "physics.h"
#include "sounds.h"
#include "interfaces.h"
#include "consoles.h"
#include "timing.h"

extern map_type				map;
extern view_type			view;
extern server_type			server;
extern hud_type				hud;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern bool					game_loop_quit;

extern void game_reset(void);
extern void chat_add_message(char *name,char *str,d3col *col);
extern bool map_rebuild_changes(char *err_str);
extern void mesh_triggers(obj_type *obj,int old_mesh_idx,int mesh_idx);
extern void group_moves_synch_with_host(network_reply_group_synch *synch);

/* =======================================================

      Add and Remove Remote Object
      
======================================================= */

bool remote_add(network_request_object_add *add,bool send_event)
{
	int					idx;
	char				err_str[256];
	obj_type			*obj,*player_obj;
	
		// create new object
		
	idx=object_create(add->name,object_type_remote,bt_game);
    if (idx==-1) return(FALSE);

	obj=server.obj_list.objs[idx];
	
		// setup remote
		
	obj->team_idx=(signed short)ntohs(add->team_idx);
	obj->tint_color_idx=(signed short)ntohs(add->tint_color_idx);
	obj->character_idx=(signed short)ntohs(add->character_idx);
	
	obj->pnt.x=ntohl(add->pnt_x);
	obj->pnt.y=ntohl(add->pnt_y);
	obj->pnt.z=ntohl(add->pnt_z);

	obj->remote.uid=(signed short)ntohs(add->player_uid);
	obj->remote.last_update=game_time_get();
	obj->remote.talking=FALSE;
	
	obj->remote.predict.move.x=0;
	obj->remote.predict.move.y=0;
	obj->remote.predict.move.z=0;
	
	obj->remote.predict.turn_y=0.0f;
	
	obj->mesh.cur_mesh_idx=-1;
	
		// start remote scripts

	if (ntohs(add->bot)!=0) {
		object_start_script(obj,"Bot",NULL,err_str);
	}
	else {
		object_start_script(obj,"Player",NULL,err_str);
	}

		// load models
		
	if (!model_draw_load(&obj->draw,"Remote",obj->name,err_str)) {
		console_add_error(err_str);
	}
	
		// initial score
		
	obj->score.score=(signed short)ntohs(add->score);
	
		// start remotes hidden
		
	obj->hidden=TRUE;

		// send event to player

	if (send_event) {
		player_obj=server.obj_list.objs[server.player_obj_index];
		scripts_post_event_console(&player_obj->attach,sd_event_remote,sd_event_remote_join,obj->idx);
	}
	
	return(TRUE);
}

void remote_team(network_request_team *team,bool send_event)
{
	int					remote_obj_uid;
	obj_type			*obj,*player_obj;
	
		// change team
		
	remote_obj_uid=(signed short)ntohs(team->remote_obj_uid);

	obj=object_find_remote_uid(remote_obj_uid);
	if (obj==NULL) return;

	obj->team_idx=(signed short)ntohs(team->team_idx);

		// send event to player

	if (send_event) {
		player_obj=server.obj_list.objs[server.player_obj_index];
		scripts_post_event_console(&player_obj->attach,sd_event_remote,sd_event_remote_team,obj->idx);
	}
}

void remote_remove(int remote_uid,bool send_event)
{
	obj_type			*obj,*player_obj;
	
		// find remote index
		
	obj=object_find_remote_uid(remote_uid);
	if (obj==NULL) return;

		// send event to player
		// do it before dispose so player can
		// read the object if it wants to

	if (send_event) {
		player_obj=server.obj_list.objs[server.player_obj_index];
		scripts_post_event_console(&player_obj->attach,sd_event_remote,sd_event_remote_leave,obj->idx);
	}
	
		// remove the obj
		
	object_dispose_single(obj->idx);
}

/* =======================================================

      Remote Timeouts
      
======================================================= */

bool remote_timed_out(obj_type *obj)
{
	return((game_time_get()-obj->remote.last_update)>client_communication_timeout_msec);
}

/* =======================================================

      Predict Remote Objects
      
======================================================= */

void remote_update_current_mesh(obj_type *obj)
{
	int				old_mesh_idx;
	
		// reset current mesh
		
	old_mesh_idx=obj->mesh.cur_mesh_idx;
	obj->mesh.cur_mesh_idx=map_mesh_find(&map,&obj->pnt);
	
		// handle any triggers
		
	if (old_mesh_idx!=obj->mesh.cur_mesh_idx) mesh_triggers(obj,old_mesh_idx,obj->mesh.cur_mesh_idx);
}

void remote_predict_move(obj_type *obj)
{
		// stop predicting after slow timeout
		
	if (remote_timed_out(obj)) return;
	
		// predict rotations
	
    obj->ang.y=angle_add(obj->ang.y,obj->remote.predict.turn_y);
	
		// predict movements
		
	object_move_remote(obj);
	remote_update_current_mesh(obj);
}

/* =======================================================

      Remote Game Resets and Host Exits
      
======================================================= */

void remote_game_reset(network_request_game_reset *reset)
{
	char				err_str[256];
	obj_type			*player_obj;

		// switch map

	strcpy(map.info.name,reset->map_name);
	map.info.player_start_name[0]=0x0;

	if (!map_rebuild_changes(err_str)) {
		game_end();
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}

		// respawn the player

	player_obj=server.obj_list.objs[server.player_obj_index];
	
	player_obj->next_spawn_sub_event=sd_event_spawn_game_reset;
	
	if (!object_spawn(player_obj,err_str)) {
		game_end();
		error_setup(err_str,"Network Game Canceled");
		server.next_state=gs_error;
		return;
	}
}

void remote_host_exit(void)
{
		// quit current game
		
	map_end();
	game_end();

		// set error page

	error_setup("The network server has been shutdown or stopped responding","Network Game Canceled");
	server.next_state=gs_error;
}

/* =======================================================

      Remote Spawn and Death
      
======================================================= */

void remote_spawn(network_request_remote_spawn *spawn)
{
	int								remote_obj_uid,sub_event;
	obj_type						*obj;
	
	remote_obj_uid=(signed short)ntohs(spawn->remote_obj_uid);
	
	obj=object_find_remote_uid(remote_obj_uid);
	if (obj==NULL) return;

		// update position

	obj->pnt.x=ntohl(spawn->pnt_x);
	obj->pnt.y=ntohl(spawn->pnt_y);
	obj->pnt.z=ntohl(spawn->pnt_z);
				
	obj->ang.x=ntohf(spawn->fp_ang_x);
	obj->ang.y=ntohf(spawn->fp_ang_y);
	obj->ang.z=ntohf(spawn->fp_ang_z);
		
		// call the spawn

	sub_event=(signed short)ntohs(spawn->sub_event);
		
	scripts_post_event_console(&obj->attach,sd_event_spawn,sub_event,0);
}

void remote_death(network_request_remote_death *death)
{
	int					remote_obj_uid,remote_killer_obj_uid;
	bool				telefrag;
	obj_type			*obj,*kill_obj,*telefrag_obj,*player_obj;
	
	remote_obj_uid=(signed short)ntohs(death->remote_obj_uid);

	obj=object_find_remote_uid(remote_obj_uid);
	if (obj==NULL) return;
	
		// set health to obj
		// this stops dead remotes from picking things up
		
	obj->status.health=0;

		// normal death
		
	telefrag=(ntohs(death->telefrag)!=0);

	if (!telefrag) {
		
			// get killing remote uid
			
		remote_killer_obj_uid=(signed short)ntohs(death->remote_killer_obj_uid);
		
			// find kill object
			
		obj->damage_obj_uid=-1;
		
		if (remote_killer_obj_uid!=-1) {
			kill_obj=object_find_remote_uid(remote_killer_obj_uid);
			if (kill_obj!=NULL) obj->damage_obj_uid=kill_obj->idx;
		}

			// send death/suicide remote event

		player_obj=server.obj_list.objs[server.player_obj_index];
		if ((obj->damage_obj_uid==-1) || (obj->damage_obj_uid==obj->idx)) {
			scripts_post_event_console(&player_obj->attach,sd_event_remote,sd_event_remote_suicide,obj->idx);
		}
		else {
			scripts_post_event_console(&player_obj->attach,sd_event_remote,sd_event_remote_death,obj->idx);
		}
	}

		// telefrag death

	else {
		remote_killer_obj_uid=(signed short)ntohs(death->remote_killer_obj_uid);

		telefrag_obj=object_find_remote_uid(remote_killer_obj_uid);
		object_telefrag(telefrag_obj,obj);
			
			// send telefrag remote event
				
		player_obj=server.obj_list.objs[server.player_obj_index];
		scripts_post_event_console(&player_obj->attach,sd_event_remote,sd_event_remote_telefrag,telefrag_obj->idx);
	}
		
		// change the score
		
	object_score_death(obj);
	
		// send alert to all objects with watches on

	object_watch_death_alert(obj);
}

/* =======================================================

      Remote Updates
      
======================================================= */

void remote_update(network_request_remote_update *update)
{
	int								n,remote_obj_uid,flags,old_score,
									animation_mode,animate_idx,animate_next_idx;
	d3pnt							org_pnt;
	obj_type						*obj;
	model_draw						*draw;
	model_draw_dynamic_bone			*dyn_bone;
	model_draw_animation			*animation;
	network_request_dynamic_bone	*net_dyn_bone;
	network_request_animation		*net_animation;
	
	remote_obj_uid=(signed short)ntohs(update->remote_obj_uid);
	
	obj=object_find_remote_uid(remote_obj_uid);
	if (obj==NULL) return;

	draw=&obj->draw;
	
		// update position

	memmove(&org_pnt,&obj->pnt,sizeof(d3pnt));
	
	obj->pnt.x=ntohl(update->pnt_x);
	obj->pnt.y=ntohl(update->pnt_y);
	obj->pnt.z=ntohl(update->pnt_z);
				
	obj->ang.x=ntohf(update->fp_ang_x);
	obj->ang.y=ntohf(update->fp_ang_y);
	obj->ang.z=ntohf(update->fp_ang_z);
	
	draw->offset.x=(signed short)ntohs(update->offset_x);
	draw->offset.y=(signed short)ntohs(update->offset_y);
	draw->offset.z=(signed short)ntohs(update->offset_z);

		// update predicition values
		
	obj->remote.predict.move.x=ntohl(update->fp_predict_move_x);
	obj->remote.predict.move.y=ntohl(update->fp_predict_move_y);
	obj->remote.predict.move.z=ntohl(update->fp_predict_move_z);

	obj->turn.fix_ang_add.x=obj->turn.fix_ang_add.z=0.0f;
	obj->remote.predict.turn_y=ntohf(update->fp_predict_turn_y);
	
		// update animations
		// only change animations if mode, animation, or next
		// animation are changing
		
	animation=draw->animations;
	net_animation=update->animation;

	for (n=0;n!=max_model_blend_animation;n++) {
	
		animation_mode=(signed short)ntohs(net_animation->model_mode);
		animate_idx=(signed short)ntohs(net_animation->model_animate_idx);
		animate_next_idx=(signed short)ntohs(net_animation->model_animate_next_idx);
		
		if ((animation_mode!=animation->mode) || (animation->animate_idx!=animate_idx) || (animation->animate_next_idx!=animate_next_idx)) {
			animation->tick=ntohl(net_animation->model_tick)+game_time_get();
			animation->mode=animation_mode;
			animation->animate_idx=animate_idx;
			animation->animate_next_idx=animate_next_idx;
			animation->pose_move_idx=(signed short)ntohs(net_animation->model_pose_move_idx);
			animation->smooth_animate_idx=(signed short)ntohs(net_animation->model_smooth_animate_idx);
			animation->smooth_pose_move_idx=(signed short)ntohs(net_animation->model_smooth_pose_move_idx);
		}
		
		animation++;
		net_animation++;
	}			
		
	draw->mesh_mask=ntohl(update->model_mesh_mask);
	memmove(&draw->cur_texture_frame,update->model_cur_texture_frame,max_model_texture);

		// dynamic bones

	dyn_bone=draw->dynamic_bones;
	net_dyn_bone=update->dynamic_bones;

	for (n=0;n!=max_model_dynamic_bone;n++) {

		dyn_bone->bone_idx=(signed short)ntohs(net_dyn_bone->bone_idx);

		if (dyn_bone->bone_idx!=-1) {
			dyn_bone->mov.x=ntohf(net_dyn_bone->fp_mov_x);
			dyn_bone->mov.y=ntohf(net_dyn_bone->fp_mov_y);
			dyn_bone->mov.z=ntohf(net_dyn_bone->fp_mov_z);
			dyn_bone->rot.x=ntohf(net_dyn_bone->fp_rot_x);
			dyn_bone->rot.y=ntohf(net_dyn_bone->fp_rot_y);
			dyn_bone->rot.z=ntohf(net_dyn_bone->fp_rot_z);
			dyn_bone->resize=ntohf(net_dyn_bone->fp_resize);
		}

		dyn_bone++;
		net_dyn_bone++;
	}
	
		// update flags
	
	flags=ntohl(update->flags);
		
	obj->hidden=((flags&net_update_flag_hidden)!=0);
	obj->contact.object_on=!((flags&net_update_flag_no_contact_object)!=0);
	obj->contact.projectile_on=!((flags&net_update_flag_no_contact_projectile)!=0);
	obj->contact.force_on=!((flags&net_update_flag_no_contact_force)!=0);
	obj->remote.talking=((flags&net_update_flag_talking)!=0);
	
		// update status
		
	old_score=obj->score.score;
	
	obj->score.score=(signed short)ntohs(update->score);
	obj->status.health=(signed short)ntohs(update->health);
	
		// last update tick
		
	obj->remote.last_update=game_time_get();
	
		// update current mesh and handle
		// triggers
		
	remote_update_current_mesh(obj);
}

/* =======================================================

      Remote Vehicles
      
======================================================= */

void remote_vehicle(network_request_remote_update *update)
{

	// supergumba -- finish for vehicle support
/*	
		// check for vehicles
		
	map_spawn_idx=(signed short)ntohs(update->vehicle_map_spawn_idx);
	if (map_spawn_idx!=-1) {
		
			// hide original object
			
		obj->hidden=TRUE;
		obj->contact.object_on=FALSE;
		obj->contact.projectile_on=FALSE;
		obj->contact.force_on=FALSE;
		
			// switch to vehicle
			
		obj=object_find_spawn_idx(map_spawn_idx);
		if (obj==NULL) return;
	}
	*/
}

/* =======================================================

      Remote Chats and Sounds
      
======================================================= */

void remote_chat(network_request_remote_chat *chat)
{
	int					remote_obj_uid;
	d3col				col;
	obj_type			*obj;
	
	remote_obj_uid=(signed short)ntohs(chat->remote_obj_uid);

	obj=object_find_remote_uid(remote_obj_uid);
	if (obj==NULL) return;
	
		// update chat

	object_get_tint(obj,&col);
	chat_add_message(obj->name,chat->str,&col);
}

void remote_sound(network_request_remote_sound *sound)
{
	int					buffer_idx;
	float				pitch;
	d3pnt				pnt;
	
	pnt.x=ntohl(sound->pnt_x);
	pnt.y=ntohl(sound->pnt_y);
	pnt.z=ntohl(sound->pnt_z);
	
	pitch=ntohf(sound->fp_pitch);
	
	buffer_idx=al_find_buffer(sound->name);
	if (buffer_idx!=-1) al_play_source(buffer_idx,&pnt,pitch,FALSE,FALSE,FALSE,FALSE);
}

/* =======================================================

      Remote Projectiles, Hit Scans, and Melees
      
======================================================= */

void remote_projectile_add(network_request_remote_fire *proj_add)
{
	int					remote_obj_uid;
	d3pnt				pt;
	d3ang				ang;
	obj_type			*obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	
	remote_obj_uid=(signed short)ntohs(proj_add->remote_obj_uid);

	obj=object_find_remote_uid(remote_obj_uid);
	if (obj==NULL) return;
	
	weap=weapon_find_name(obj,proj_add->weap_name);
	if (weap==NULL) return;
	
	proj_setup=find_proj_setups(weap,proj_add->proj_setup_name);
	if (proj_setup==NULL) return;

	pt.x=ntohl(proj_add->pt_x);
	pt.y=ntohl(proj_add->pt_y);
	pt.z=ntohl(proj_add->pt_z);
	
	ang.x=ntohf(proj_add->fp_ang_x);
	ang.y=ntohf(proj_add->fp_ang_y);
	ang.z=ntohf(proj_add->fp_ang_z);

	weapon_add_projectile(obj,weap,proj_setup,&pt,&ang);
}

void remote_hit_scan_add(network_request_remote_fire *hitscan_add)
{
	int					remote_obj_uid;
	d3pnt				pt;
	d3ang				ang;
	obj_type			*obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	
	remote_obj_uid=(signed short)ntohs(hitscan_add->remote_obj_uid);

	obj=object_find_remote_uid(remote_obj_uid);
	if (obj==NULL) return;
	
	weap=weapon_find_name(obj,hitscan_add->weap_name);
	if (weap==NULL) return;
	
	proj_setup=find_proj_setups(weap,hitscan_add->proj_setup_name);
	if (proj_setup==NULL) return;
				
	pt.x=ntohl(hitscan_add->pt_x);
	pt.y=ntohl(hitscan_add->pt_y);
	pt.z=ntohl(hitscan_add->pt_z);
	
	ang.x=ntohf(hitscan_add->fp_ang_x);
	ang.y=ntohf(hitscan_add->fp_ang_y);
	ang.z=ntohf(hitscan_add->fp_ang_z);
	
	projectile_hitscan(obj,weap,proj_setup,&pt,&ang);
}

void remote_melee_add(network_request_remote_fire *rem_melee)
{
	int					remote_obj_uid;
	d3pnt				pt;
	d3ang				ang;
	obj_type			*obj;
	weapon_type			*weap;
	melee_type			melee;
	
	remote_obj_uid=(signed short)ntohs(rem_melee->remote_obj_uid);

	obj=object_find_remote_uid(remote_obj_uid);
	if (obj==NULL) return;
	
	weap=NULL;
	if (rem_melee->weap_name[0]!=0x0) weap=weapon_find_name(obj,rem_melee->weap_name);
	
	pt.x=ntohl(rem_melee->pt_x);
	pt.y=ntohl(rem_melee->pt_y);
	pt.z=ntohl(rem_melee->pt_z);
	
	ang.x=ntohf(rem_melee->fp_ang_x);
	ang.y=ntohf(rem_melee->fp_ang_y);
	ang.z=ntohf(rem_melee->fp_ang_z);
	
	melee.radius=(int)ntohs(rem_melee->radius);
	melee.distance=(int)ntohs(rem_melee->distance);
	melee.damage=(int)ntohs(rem_melee->damage);
	melee.force=(int)ntohs(rem_melee->force);
	
	melee_add(obj,weap,&pt,&ang,&melee,obj->idx);
}

void remote_fire(network_request_remote_fire *fire)
{
	int				fire_type;

	fire_type=(int)ntohs(fire->fire_type);

	switch (fire_type) {

		case net_remote_fire_type_projectile:
			remote_projectile_add(fire);
			break;

		case net_remote_fire_type_hit_scan:
			remote_hit_scan_add(fire);
			break;

		case net_remote_fire_type_melee:
			remote_melee_add(fire);
			break;
	}
}

/* =======================================================

      Remote Pickup
      
======================================================= */

void remote_pickup(network_request_remote_pickup *pickup)
{
	int					n,remote_obj_uid,idx;
	d3pnt				org_pnt;
	obj_type			*obj;
	weapon_type			*weap;
	
	remote_obj_uid=(signed short)ntohs(pickup->remote_obj_uid);

	obj=object_find_remote_uid(remote_obj_uid);
	if (obj==NULL) return;

		// make sure pickup is at proper point

	memmove(&org_pnt,&obj->pnt,sizeof(d3pnt));
	
	obj->pnt.x=ntohl(pickup->pt_x);
	obj->pnt.y=ntohl(pickup->pt_y);
	obj->pnt.z=ntohl(pickup->pt_z);

		// setup health, ammo as they
		// could effect pickups

	obj->status.health=(signed short)ntohs(pickup->health);

	idx=0;
		
	for (n=0;n!=max_weap_list;n++) {
		weap=obj->weap_list.weaps[n];
		if (weap==NULL) continue;

		weap->hidden=((signed short)ntohs(pickup->ammos[idx].hidden)!=0);
		weap->ammo.count=(signed short)ntohs(pickup->ammos[idx].ammo_count);
		weap->ammo.clip_count=(signed short)ntohs(pickup->ammos[idx].clip_count);
		weap->alt_ammo.count=(signed short)ntohs(pickup->ammos[idx].alt_ammo_count);
		weap->alt_ammo.clip_count=(signed short)ntohs(pickup->ammos[idx].alt_clip_count);

		idx++;
		if (idx==net_max_weapon_per_remote) break;
	}

		// run pickup

	item_pickup_check(obj);

		// restore point

	memmove(&obj->pnt,&org_pnt,sizeof(d3pnt));
}

/* =======================================================

      Remote Click
      
======================================================= */

void remote_click(network_request_remote_click *click)
{
	int					remote_obj_uid,clicked_obj_uid;
	d3pnt				org_pnt;
	d3ang				org_ang;
	obj_type			*obj,*clicked_obj;
	
	remote_obj_uid=(signed short)ntohs(click->remote_obj_uid);

	obj=object_find_remote_uid(remote_obj_uid);
	if (obj==NULL) return;

		// make sure click is at proper point

	memmove(&org_pnt,&obj->pnt,sizeof(d3pnt));
	memmove(&org_ang,&obj->ang,sizeof(d3ang));
	
	obj->pnt.x=ntohl(click->pt_x);
	obj->pnt.y=ntohl(click->pt_y);
	obj->pnt.z=ntohl(click->pt_z);
	
	obj->ang.x=ntohf(click->fp_ang_x);
	obj->ang.y=ntohf(click->fp_ang_y);
	obj->ang.z=ntohf(click->fp_ang_z);

		// run click

	clicked_obj_uid=object_find_uid_click_object(obj);
	if (clicked_obj_uid!=-1) {
		clicked_obj=server.obj_list.objs[clicked_obj_uid];
		object_click(clicked_obj,obj);
	}

		// restore point

	memmove(&obj->pnt,&org_pnt,sizeof(d3pnt));
	memmove(&obj->ang,&org_ang,sizeof(d3ang));
}

/* =======================================================

      Remote Networking Receives
      
======================================================= */

bool remote_network_get_updates(void)
{
	int						action,player_uid,count;
	unsigned char			msg[net_max_msg_size];
	
	count=0;
	
	while (count<client_message_per_loop_count) {
	
			// check for messages

		if (!net_client_check_message_queue(&action,&player_uid,msg)) return(TRUE);
		
			// if at score limit, only accept reset messages
			
		if (server.state==gs_score_limit) {
			if (action!=net_action_request_game_reset) continue;
		}
		
			// run message
		
		switch (action) {
		
			case net_action_request_game_reset:
				remote_game_reset((network_request_game_reset*)msg);
				break;
				
			case net_action_request_team:
				remote_team((network_request_team*)msg,TRUE);
				break;

			case net_action_request_remote_add:
				remote_add((network_request_object_add*)msg,TRUE);
				break;
				
			case net_action_request_remote_remove:
				remote_remove(player_uid,TRUE);
				break;
				
			case net_action_request_remote_spawn:
				remote_spawn((network_request_remote_spawn*)msg);
				break;
				
			case net_action_request_remote_death:
				remote_death((network_request_remote_death*)msg);
				break;
			
			case net_action_request_remote_update:
				remote_update((network_request_remote_update*)msg);
				break;
				
			case net_action_request_remote_chat:
				remote_chat((network_request_remote_chat*)msg);
				break;
				
			case net_action_request_remote_sound:
				remote_sound((network_request_remote_sound*)msg);
				break;

			case net_action_request_remote_fire:
				remote_fire((network_request_remote_fire*)msg);
				break;

			case net_action_request_remote_pickup:
				remote_pickup((network_request_remote_pickup*)msg);
				break;

			case net_action_request_remote_click:
				remote_click((network_request_remote_click*)msg);
				break;

			case net_action_reply_latency_ping:
				net_setup.client.latency=(game_time_get()-net_setup.client.latency_ping_tick)>>1;		// latency is half of round trip as client-client
				break;

			case net_action_request_host_exit:
				remote_host_exit();
				return(FALSE);				// break out of all the loops and exit client game

			case net_action_reply_group_synch:
				group_moves_synch_with_host((network_reply_group_synch*)msg);
				break;
				
			case net_action_request_game_score_limit:
				server.next_state=gs_score_limit;
				break;
				
		}
	}
	
	return(TRUE);
}

/* =======================================================

      Remote Networking Updates, Pings, and Synchs
      
======================================================= */

void remote_network_send_updates(void)
{
	int					n,tick;
	bool				coop;
	obj_type			*obj;

		// time for an update

	tick=game_time_get();
	if (tick<server.time.network_update_tick) return;

	server.time.network_update_tick=tick+client_communication_update_msec_rate;

		// update the player

	obj=server.obj_list.objs[server.player_obj_index];
	net_client_send_remote_update(obj,hud.chat.type_on);

		// update any co-op bots if hosting

	if ((net_setup.mode==net_mode_host) || (net_setup.mode==net_mode_host_dedicated)) {
	
		coop=hud.net_game.games[net_setup.game_idx].monsters;

		for (n=0;n!=max_obj_list;n++) {
			obj=server.obj_list.objs[n];
			if (obj==NULL) continue;

			if ((obj->type==object_type_bot_multiplayer) || ((obj->type==object_type_bot_map) && (coop))) {
				net_client_send_remote_update(obj,FALSE);
			}
		}
	}
}

void remote_network_send_group_synch(void)
{
	int				tick;

		// time for group synch?

	tick=game_time_get();
	if (tick<server.time.network_group_synch_tick) return;

	server.time.network_group_synch_tick=tick+client_communication_group_synch_msec_rate;

		// run group synch

	net_client_request_group_synch_ping(object_player_get_remote_uid());
}

void remote_network_send_latency_ping(void)
{
	int				tick;

		// time for latency ping?

	tick=game_time_get();
	if (tick<server.time.network_latency_ping_tick) return;
	
	server.time.network_latency_ping_tick=tick+client_communication_latency_ping_msec_rate;

		// latency ping

	net_setup.client.latency_ping_tick=tick;
	net_client_send_latency_ping(object_player_get_remote_uid());
}

/* =======================================================

      Map Bots (in Co-Op) to Remotes
      
======================================================= */

void remote_setup_coop_bots(void)
{
	int					n,uid;
	bool				coop;
	obj_type			*obj;

		// nothing to do if not a network game
		
	if (net_setup.mode==net_mode_none) return;
	
		// is it co-op mode?
		
	coop=hud.net_game.games[net_setup.game_idx].monsters;
	
		// create remote IDs
		
	uid=net_player_uid_map_bot_start;

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
	
		if (obj->type==object_type_bot_map) {
		
				// if not in coop, then no map bots

			if (!coop) {
				obj->hidden=TRUE;
			}

				// map bots need a unique remote ID, build
				// from a high number to not interfere with real
				// remotes or bots

			else {
				obj->remote.uid=uid;
				uid++;
			}

		}
	}
}
