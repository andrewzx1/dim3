/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Spawning

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

#include "network.h"
#include "scripts.h"
#include "objects.h"
#include "weapons.h"
#include "remotes.h"
#include "models.h"
#include "physics.h"
#include "interfaces.h"
#include "timing.h"

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern js_type				js;
extern hud_type				hud;
extern network_setup_type	net_setup;

extern bool game_file_reload_ok(void);
extern bool game_file_reload(char *err_str);
extern void mesh_triggers(obj_type *obj,int old_mesh_idx,int mesh_idx);

/* =======================================================

      Object Spawn Position
      
======================================================= */

int object_choose_spawn_spot(obj_type *obj,char *err_str)
{
	int			spot_idx;
	
		// spots when not in a network game
		
	if (net_setup.mode==net_mode_none) {
	
			// if this object is the player, then spawn
			// at the player start
		
		if (obj->idx==server.player_obj_idx) {
			spot_idx=map_find_random_spot(&map,map.info.player_start_name,spot_type_player);
			if (spot_idx!=-1) return(spot_idx);
			
			sprintf(err_str,"Could not find spot: %s-Player",map.info.player_start_name);
			return(-1);
		}
	
			// otherwise, any spawn spot
			
		spot_idx=map_find_random_spot(&map,NULL,spot_type_spawn);
		if (spot_idx!=-1) return(spot_idx);
		
		strcpy(err_str,"Could not find spot: *-Spawn");
		return(-1);
	}
	
		// spots when in a network game
		
		// if we have a spot type (for instance, network games
		// need to start objects at the right place) then look for that
		
	if (obj->spawn_spot_name[0]!=0x0) {
		spot_idx=map_find_random_spot(&map,obj->spawn_spot_name,spot_type_spawn);
		if (spot_idx!=-1) return(spot_idx);
	
		sprintf(err_str,"Could not find spot: %s-Spawn",obj->spawn_spot_name);
		return(-1);
	}
	
		// otherwise any spawn spot
			
	spot_idx=map_find_random_spot(&map,NULL,spot_type_spawn);
	if (spot_idx!=-1) return(spot_idx);
		
	strcpy(err_str,"Could not find spot: *-Spawn");
	return(-1);
}

bool object_spawn_position(obj_type *obj,int sub_event,char *err_str)
{
	int				idx;
	spot_type		*spot;

		// if it's the initial spawn, then we
		// check to see if there's an editor override,
		// else we let the spot pick functions deal with
		// picking the correct spot

	if (sub_event==sd_event_spawn_init) {
	
			// if editor override then try to start at editor
			// cursor position

		if ((setup.editor_override.on) && (!map.settings.editor.link_always_start)) {
			setup.editor_override.on=FALSE;
			object_set_position(obj,setup.editor_override.pt.x,(setup.editor_override.pt.y-obj->size.eye_offset),setup.editor_override.pt.z,setup.editor_override.ang.y,0);
			return(TRUE);
		}

	}

		// otherwise spawn at object spot

	idx=object_choose_spawn_spot(obj,err_str);
	if (idx==-1) return(FALSE);
		
	spot=&map.spots[idx];
	object_set_position(obj,spot->pnt.x,spot->pnt.y,spot->pnt.z,spot->ang.y,0);

	return(TRUE);
}

/* =======================================================

      Object Spawning
      
======================================================= */

bool object_spawn(obj_type *obj,char *err_str)
{
	int				mesh_idx,sub_event;
	
	sub_event=obj->next_spawn_sub_event;

		// reset health and weapons
		// if not map change

	if (sub_event!=sd_event_spawn_map_change) {
		obj->status.health=obj->status.start_health;
		weapon_reset_ammo_object(obj);
	}

		// game resets change scores

	if (sub_event==sd_event_spawn_game_reset) {
		obj->score.kill=obj->score.death=obj->score.suicide=obj->score.goal=obj->score.score=0;
		obj->score.place=1;
	}

		// reborn resets some things that
		// death flips

	if (sub_event==sd_event_spawn_reborn) {
		obj->input_freeze=FALSE;
		obj->death_trigger=FALSE;
	}

		// make sure to stop all movement

	object_stop(obj);
	
		// fix some status
	
	obj->air_mode=am_ground;
    obj->liquid.mode=lm_out;
	obj->fall.dist=0;
    obj->fall.change=FALSE;

		// clear weapon states (like switches)

	weapon_clear_state(obj);

		// handle any repositioning
		// only players and multiplayer bots can reposition
		
	if ((obj->idx==server.player_obj_idx) || (obj->type==object_type_bot_multiplayer)) {	
		if (!object_spawn_position(obj,sub_event,err_str)) return(FALSE);
	}
	
		// setup script resets
		// in case we need to return to spawn
	
	object_reset_prepare(obj);
		
		// any mesh messages

	mesh_idx=map_mesh_find(&map,&obj->pnt);
	if (mesh_idx!=-1) mesh_triggers(obj,-1,mesh_idx);
    
		// call the spawn event
		
	scripts_post_event_console(&obj->attach,sd_event_spawn,sub_event,0);

		// if this is the player, then make
		// sure to call the spawn event on other
		// machines

	if (net_setup.mode!=net_mode_none) {
		if (obj->idx==server.player_obj_idx) net_client_send_spawn(obj,sub_event);
	}
	
		// can't respawn until we die
		
	obj->status.respawn_tick=-1;
	
		// and handle any telefragging
		
	object_telefrag_players(obj,FALSE);
	
	return(TRUE);
}

int object_get_respawn_time(obj_type *obj)
{
	int				secs;
	
	secs=(obj->status.respawn_tick-game_time_get())/1000;
	if (secs<0) return(0);
	
	return(secs);
}

void object_check_respawn(obj_type *obj)
{
	char			err_str[256];

		// network respawning is automatic
		// single player responding happens with
		// a keypress

	if (net_setup.mode==net_mode_none) return;
	
		// only players and bots
		// can respawn, remotes handle their own
		// respawning
		
	if ((obj->type!=object_type_player) && (obj->type!=object_type_bot_multiplayer)) return;
	
	if (obj->status.health>0) return;
	if (obj->status.respawn_tick==-1) return;
	
		// time to respawn?
		
	if (obj->status.respawn_tick<game_time_get()) {
		obj->next_spawn_sub_event=sd_event_spawn_reborn;
		if (!object_spawn(obj,err_str)) {
			error_setup(err_str,"Network Game Canceled");
			server.next_state=gs_error;
		}
	}
}

/* =======================================================

      Create Game Objects
      
======================================================= */

int game_player_create(char *err_str)
{
	int			uid;
	
	uid=object_start(NULL,setup.network.name,object_type_player,bt_game,err_str);
	return(uid);
}

void game_multiplayer_bots_create(void)
{
	int				n,uid;
	char			name[name_str_len],err_str[256];
	spot_type		spot;
	
		// only hosts have multiplayer bots
		
	if ((net_setup.mode!=net_mode_host) && (net_setup.mode!=net_mode_host_dedicated)) return;

		// are bots allowed in this game?

	if (!hud.net_bot.on) return;

		// spawn bots

	for (n=0;n!=setup.network.bot.count;n++) {
	
			// setup a fake spot for object spawning
			
		if (hud.net_bot.bots[n].name[0]!=0x0) {
			strcpy(name,hud.net_bot.bots[n].name);
		}
		else {
			sprintf(name,"Bot %d",(n+1));
		}
		
		strcpy(spot.name,name);
		spot.type=spot_type_bot;
		strcpy(spot.script,"Bot");
		spot.params[0]=0x0;
		
		uid=object_start(&spot,name,object_type_bot_multiplayer,bt_game,err_str);
	}
}

void game_remotes_create(network_reply_join_remotes *remotes)
{
	int							n;
	network_request_object_add	*obj_add;

		// only clients add remotes
		
	if (net_setup.mode!=net_mode_client) return;
	
		// add remotes from host
		
	obj_add=remotes->objects;
	
	for (n=0;n!=remotes->count;n++) {
		remote_add(obj_add,FALSE);
		obj_add++;
	}
}

/* =======================================================

      Create Map Object
      
======================================================= */

void map_objects_create(void)
{
	int					n,spawn_type;
	char				err_str[256];
	spot_type			*spot;
	
	for (n=0;n!=map.nspot;n++) {
	
		spot=&map.spots[n];
		
			// can only spawn objects and
			// map bots
			
		if ((spot->type!=spot_type_object) && (spot->type!=spot_type_bot)) continue;
		
			// check for correct skill level
			// and multiplayer spawning flags
			
		if (spot->skill>server.skill) continue;
		if ((spot->spawn==spawn_single_player_only) && (net_setup.mode!=net_mode_none)) continue;
		if ((spot->spawn==spawn_multiplayer_only) && (net_setup.mode==net_mode_none)) continue;
		
			// get the spawn type
			
		spawn_type=object_type_object;
		if (spot->type==spot_type_bot) spawn_type=object_type_bot_map;

			// create the object

		object_start(spot,spot->name,spawn_type,bt_map,err_str);
	}
}

/* =======================================================

      Map Object Spawns
      
======================================================= */

bool map_object_attach_all(char *err_str)
{
	int				n;
	obj_type		*obj;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (obj->attach.script_idx!=-1) {
			if (!object_spawn(obj,err_str)) return(FALSE);
		}
	}
	
	return(TRUE);
}

void map_object_detach_all(void)
{
	int				n;
	obj_type		*obj;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
	
		if (obj->attach.script_idx!=-1) {
		
				// exit any vehicles
					// supergumba -- test vehicle exiting
				
			if (obj->vehicle.attach_obj_uid!=-1) {
				object_exit_vehicle(obj,TRUE,NULL);
			}
		}
	}

}

