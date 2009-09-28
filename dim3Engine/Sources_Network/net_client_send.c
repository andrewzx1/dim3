/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Client Send Message to Host

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
#include "interfaces.h"
#include "network.h"

extern map_type					map;
extern server_type				server;
extern js_type					js;
extern setup_type				setup;

extern d3socket					client_socket;

extern network_setup_type		net_setup;

/* =======================================================

      Host Messages
      
======================================================= */

void net_client_send_ready(int remote_uid)
{
	if (net_setup.host.hosting) {
		net_host_player_ready(remote_uid,TRUE);
	}
	else {
		net_send_message(client_socket,net_action_request_ready,remote_uid,NULL,0);
	}
}

void net_client_send_set_team(int remote_uid,int team_idx)
{
	network_request_team	team;
	
	team.team_idx=htons((short)team_idx);

	if (net_setup.host.hosting) {
		net_host_client_handle_set_team(remote_uid,&team);
	}
	else {
		net_send_message(client_socket,net_action_request_team,remote_uid,(unsigned char*)&team,sizeof(network_request_team));
	}
}

void net_client_send_leave_host(int remote_uid)
{
	if (net_setup.host.hosting) {
		net_host_client_handle_leave(remote_uid);
	}
	else {
		net_send_message(client_socket,net_action_request_leave,remote_uid,NULL,0);
	}
}

void net_client_send_latency_ping(int remote_uid)
{
	if (!net_setup.host.hosting) {
		net_send_message(client_socket,net_action_request_latency_ping,remote_uid,NULL,0);
	}
}	

/* =======================================================

      Host Synching
      
======================================================= */

void net_client_request_group_synch_ping(int remote_uid)
{
	if (!net_setup.host.hosting) {
		net_send_message(client_socket,net_action_request_group_synch,remote_uid,NULL,0);
	}
}	

/* =======================================================

      Object Update Messages
      
======================================================= */

void net_client_send_remote_update(int tick,int remote_uid,obj_type *obj,bool chat_on)
{
	int								n,flags;
	model_draw						*draw;
	model_draw_animation			*animation;
	model_draw_dynamic_bone			*dyn_bone;
	network_request_animation		*net_animation;
	network_request_dynamic_bone	*net_dyn_bone;
	network_request_remote_update	update;
	
		// no updates if pipe full
		
	if (!net_setup.host.hosting) {
		if (!net_send_ready(client_socket)) return;
	}

	draw=&obj->draw;

		// create flags
		
	flags=0;
	
	if (obj->hidden) flags|=net_update_flag_hidden;
	if (!obj->contact.object_on) flags|=net_update_flag_no_contact_object;
	if (!obj->contact.projectile_on) flags|=net_update_flag_no_contact_object;
	if (!obj->contact.force_on) flags|=net_update_flag_no_contact_object;
	if (chat_on) flags|=net_update_flag_talking;
	
	update.flags=htonl(flags);
	
		// status

	update.score=htons((short)obj->score.score);
	update.health=htons((short)obj->status.health);
	
		// position
		
	update.pnt_x=htonl(obj->pnt.x);
	update.pnt_y=htonl(obj->pnt.y);
	update.pnt_z=htonl(obj->pnt.z);
	
	update.fp_ang_x=htonf(obj->ang.x);
	update.fp_ang_y=htonf(obj->ang.y);
	update.fp_ang_z=htonf(obj->ang.z);

	update.offset_x=htons((short)draw->offset.x);
	update.offset_y=htons((short)draw->offset.y);
	update.offset_z=htons((short)draw->offset.z);

	update.fp_predict_move_x=htonl(obj->pnt.x-obj->last_pnt.x);
	update.fp_predict_move_y=htonl(obj->pnt.y-obj->last_pnt.y);
	update.fp_predict_move_z=htonl(obj->pnt.z-obj->last_pnt.z);
	
	update.fp_predict_turn_y=htonf(obj->last_ang.y-obj->ang.y);
	
		// vehicles
		
	if (obj->vehicle.attach_obj_uid==-1) {
		update.vehicle_map_spawn_idx=htons(-1);
	}
	else {
		update.vehicle_map_spawn_idx=htons((short)obj->spawn_idx);
	}
	
		// model animations

	animation=draw->animations;
	net_animation=update.animation;
	
	for (n=0;n!=max_model_blend_animation;n++) {
		net_animation->model_tick=htonl(animation->tick-tick);
		net_animation->model_mode=htons((short)animation->mode);
		net_animation->model_animate_idx=htons((short)animation->animate_idx);
		net_animation->model_animate_next_idx=htons((short)animation->animate_next_idx);
		net_animation->model_pose_move_idx=htons((short)animation->pose_move_idx);
		net_animation->model_smooth_animate_idx=htons((short)animation->smooth_animate_idx);
		net_animation->model_smooth_pose_move_idx=htons((short)animation->smooth_pose_move_idx);

		animation++;
		net_animation++;
	}
	
	update.model_mesh_mask=htonl(draw->mesh_mask);
	memmove(update.model_cur_texture_frame,draw->cur_texture_frame,max_model_texture);

		// dynamic bones

	dyn_bone=draw->dynamic_bones;
	net_dyn_bone=update.dynamic_bones;

	for (n=0;n!=max_model_dynamic_bone;n++) {

		net_dyn_bone->bone_idx=htons((short)dyn_bone->bone_idx);

		if (dyn_bone->bone_idx!=-1) {
			net_dyn_bone->fp_mov_x=htonf(dyn_bone->mov.x);
			net_dyn_bone->fp_mov_y=htonf(dyn_bone->mov.y);
			net_dyn_bone->fp_mov_z=htonf(dyn_bone->mov.z);
			net_dyn_bone->fp_rot_x=htonf(dyn_bone->rot.x);
			net_dyn_bone->fp_rot_y=htonf(dyn_bone->rot.y);
			net_dyn_bone->fp_rot_z=htonf(dyn_bone->rot.z);
			net_dyn_bone->fp_resize=htonf(dyn_bone->resize);
		}

		dyn_bone++;
		net_dyn_bone++;
	}

		// send update
		
	if (net_setup.host.hosting) {
		net_host_client_handle_update(remote_uid,&update);
	}
	else {
		net_send_message(client_socket,net_action_request_remote_update,remote_uid,(unsigned char*)&update,sizeof(network_request_remote_update));
	}
}

void net_client_send_death(int remote_uid,int kill_uid,bool telefrag)
{
	int								send_kill_uid,send_telefrag_uid;
	obj_type						*obj;
	network_request_remote_death	death;
	
		// normal deaths

	if (!telefrag) {
		send_kill_uid=-1;
		
		if (kill_uid!=-1) {
			if (kill_uid==server.player_obj_uid) {
				send_kill_uid=object_player_get_remote_uid();
			}
			else {
				obj=object_find_uid(kill_uid);
				if (obj!=NULL) {
					if (obj->type_idx==object_type_remote) {
						send_kill_uid=obj->remote.uid;
					}
				}
			}
		}
		
		death.kill_remote_uid=htons((short)send_kill_uid);
		death.telefrag=htons(0);

	}

		// telefrag deaths

	else {

		if (kill_uid==server.player_obj_uid) {
			send_telefrag_uid=object_player_get_remote_uid();
		}
		else {
			obj=object_find_uid(kill_uid);		// only remote objects can telefrag each other, so no other checks necessary
			send_telefrag_uid=obj->remote.uid;
		}
		
		death.kill_remote_uid=htons((short)send_telefrag_uid);
		death.telefrag=htons(1);

	}

	if (net_setup.host.hosting) {
		net_host_player_send_others_packet(remote_uid,net_action_request_remote_death,(unsigned char*)&death,sizeof(network_request_remote_death));
	}
	else {
		net_send_message(client_socket,net_action_request_remote_death,remote_uid,(unsigned char*)&death,sizeof(network_request_remote_death));
	}
}

/* =======================================================

      Chats and Sound Messages
	        
======================================================= */

void net_client_send_chat(int remote_uid,char *str)
{
	network_request_remote_chat		chat;
	
	strcpy(chat.str,str);

	if (net_setup.host.hosting) {
		net_host_player_send_others_packet(remote_uid,net_action_request_remote_chat,(unsigned char*)&chat,sizeof(network_request_remote_chat));
	}
	else {
		net_send_message(client_socket,net_action_request_remote_chat,remote_uid,(unsigned char*)&chat,sizeof(network_request_remote_chat));
	}
}

void net_client_send_sound(int remote_uid,int x,int y,int z,float pitch,char *name)
{
	network_request_remote_sound	sound;
	
	sound.pnt_x=htonl(x);
	sound.pnt_y=htonl(y);
	sound.pnt_z=htonl(z);
	
	sound.fp_pitch=htonf(pitch);
	
	strcpy(sound.name,name);

	if (net_setup.host.hosting) {
		net_host_player_send_others_packet(remote_uid,net_action_request_remote_sound,(unsigned char*)&sound,sizeof(network_request_remote_sound));
	}
	else {
		net_send_message(client_socket,net_action_request_remote_sound,remote_uid,(unsigned char*)&sound,sizeof(network_request_remote_sound));
	}
}

/* =======================================================

      Weapon Messages
	        
======================================================= */

void net_client_send_projectile_add(int remote_uid,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang)
{
	network_request_remote_fire		fire;
	
	fire.fire_type=htons(net_remote_fire_type_projectile);

	fire.pt_x=htonl(pt->x);
	fire.pt_y=htonl(pt->y);
	fire.pt_z=htonl(pt->z);
	
	fire.fp_ang_x=htonf(ang->x);
	fire.fp_ang_y=htonf(ang->y);
	fire.fp_ang_z=htonf(ang->z);
	
	strcpy(fire.weap_name,weap_name);
	strcpy(fire.proj_setup_name,proj_setup_name);

	fire.radius=0;
	fire.distance=0;
	fire.damage=0;
	fire.force=0;

	if (net_setup.host.hosting) {
		net_host_player_send_others_packet(remote_uid,net_action_request_remote_fire,(unsigned char*)&fire,sizeof(network_request_remote_fire));
	}
	else {
		net_send_message(client_socket,net_action_request_remote_fire,remote_uid,(unsigned char*)&fire,sizeof(network_request_remote_fire));
	}
}

void net_client_send_hitscan_add(int remote_uid,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang)
{
	network_request_remote_fire		fire;
	
	fire.fire_type=htons(net_remote_fire_type_hit_scan);
	
	fire.pt_x=htonl(pt->x);
	fire.pt_y=htonl(pt->y);
	fire.pt_z=htonl(pt->z);
	
	fire.fp_ang_x=htonf(ang->x);
	fire.fp_ang_y=htonf(ang->y);
	fire.fp_ang_z=htonf(ang->z);
	
	strcpy(fire.weap_name,weap_name);
	strcpy(fire.proj_setup_name,proj_setup_name);

	fire.radius=0;
	fire.distance=0;
	fire.damage=0;
	fire.force=0;

	if (net_setup.host.hosting) {
		net_host_player_send_others_packet(remote_uid,net_action_request_remote_fire,(unsigned char*)&fire,sizeof(network_request_remote_fire));
	}
	else {
		net_send_message(client_socket,net_action_request_remote_fire,remote_uid,(unsigned char*)&fire,sizeof(network_request_remote_fire));
	}
}

void net_client_send_melee_add(int remote_uid,char *weap_name,int radius,int distance,int damage,int force,d3pnt *pt,d3ang *ang)
{
	network_request_remote_fire		fire;
	
	fire.fire_type=htons(net_remote_fire_type_melee);
	
	fire.pt_x=htonl(pt->x);
	fire.pt_y=htonl(pt->y);
	fire.pt_z=htonl(pt->z);
	
	fire.fp_ang_x=htonf(ang->x);
	fire.fp_ang_y=htonf(ang->y);
	fire.fp_ang_z=htonf(ang->z);
	
	strcpy(fire.weap_name,weap_name);
	fire.proj_setup_name[0]=0x0;
	
	fire.radius=htons((short)radius);
	fire.distance=htons((short)distance);
	fire.damage=htons((short)damage);
	fire.force=htons((short)force);

	if (net_setup.host.hosting) {
		net_host_player_send_others_packet(remote_uid,net_action_request_remote_fire,(unsigned char*)&fire,sizeof(network_request_remote_fire));
	}
	else {
		net_send_message(client_socket,net_action_request_remote_fire,remote_uid,(unsigned char*)&fire,sizeof(network_request_remote_fire));
	}
}

/* =======================================================

      Pickup Messages
	        
======================================================= */

void net_client_setup_pickup(obj_type *obj,network_request_remote_pickup *pickup)
{
	int									n,idx;
	weapon_type							*weap;
	
		// make sure pickup can work by
		// moving over settings pickups are concerned with

	pickup->pt_x=htonl(obj->pnt.x);
	pickup->pt_y=htonl(obj->pnt.y);
	pickup->pt_z=htonl(obj->pnt.z);

	pickup->health=htons((short)obj->status.health);

	idx=0;
	weap=server.weapons;
		
	for (n=0;n!=server.count.weapon;n++) {

		if (weap->obj_uid==obj->uid) {
			pickup->ammos[idx].hidden=htons((short)(weap->hidden?0:1));
			pickup->ammos[idx].ammo_count=htons((short)weap->ammo.count);
			pickup->ammos[idx].clip_count=htons((short)weap->ammo.clip_count);
			pickup->ammos[idx].alt_ammo_count=htons((short)weap->alt_ammo.count);
			pickup->ammos[idx].alt_clip_count=htons((short)weap->alt_ammo.clip_count);

			idx++;
			if (idx==net_max_weapon_per_remote) break;
		}

		weap++;
	}
}

void net_client_send_pickup(int remote_uid,network_request_remote_pickup *pickup)
{
	if (net_setup.host.hosting) {
		net_host_player_send_others_packet(remote_uid,net_action_request_remote_pickup,(unsigned char*)pickup,sizeof(network_request_remote_pickup));
	}
	else {
		net_send_message(client_socket,net_action_request_remote_pickup,remote_uid,(unsigned char*)pickup,sizeof(network_request_remote_pickup));
	}
}

/* =======================================================

      Click Messages
	        
======================================================= */

void net_client_send_click(int remote_uid,d3pnt *pt,d3ang *ang)
{
	network_request_remote_click		click;
	
	click.pt_x=htonl(pt->x);
	click.pt_y=htonl(pt->y);
	click.pt_z=htonl(pt->z);
	
	click.fp_ang_x=htonf(ang->x);
	click.fp_ang_y=htonf(ang->y);
	click.fp_ang_z=htonf(ang->z);
	
	if (net_setup.host.hosting) {
		net_host_player_send_others_packet(remote_uid,net_action_request_remote_click,(unsigned char*)&click,sizeof(network_request_remote_click));
	}
	else {
		net_send_message(client_socket,net_action_request_remote_click,remote_uid,(unsigned char*)&click,sizeof(network_request_remote_click));
	}
}
