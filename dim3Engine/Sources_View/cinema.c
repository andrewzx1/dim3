/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Cinemas

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
#include "interfaces.h"
#include "video.h"
#include "objects.h"
#include "models.h"
#include "effects.h"
#include "consoles.h"
#include "cameras.h"
#include "inputs.h"
#include "timing.h"

extern void map_movements_cinema_start(int movement_idx,bool reverse);

extern server_type				server;
extern view_type				view;
extern map_type					map;
extern js_type					js;
extern network_setup_type		net_setup;

extern camera_type				camera;

/* =======================================================

      Cinema Operations
      
======================================================= */

bool cinema_start(char *name,int event_id,char *err_str)
{
	int				n;
	obj_type		*player_obj;

		// get the cinema

	view.cinema.idx=-1;

	for (n=0;n!=map.cinema.ncinema;n++) {
		if (strcmp(name,map.cinema.cinemas[n].name)==0) {
			view.cinema.idx=n;
			break;
		}
	}

	if (view.cinema.idx==-1) {
		sprintf(err_str,"No cinema exists with this name: %s",name);
		return(FALSE);
	}

		// start the timing and events

	view.cinema.on=TRUE;

	view.cinema.start_tick=game_time_get();
	view.cinema.last_tick=0;

	view.cinema.event_id=event_id;

		// backup the camera

	memmove(&view.cinema.camera_state,&camera,sizeof(camera_type));

		// run any input freeze

	if (map.cinema.cinemas[view.cinema.idx].freeze_input) {
		player_obj=server.obj_list.objs[server.player_obj_idx];
		object_input_freeze(player_obj,TRUE);
	}

	return(TRUE);
}

void cinema_end(void)
{
	obj_type		*player_obj;

		// turn off input freeze

	if (map.cinema.cinemas[view.cinema.idx].freeze_input) {
		player_obj=server.obj_list.objs[server.player_obj_idx];
		object_input_freeze(player_obj,FALSE);
	}

		// restore camera

	memmove(&camera,&view.cinema.camera_state,sizeof(camera_type));
}

/* =======================================================

      Cinema Actions Camera
      
======================================================= */

void cinema_action_run_camera(int node_idx,map_cinema_action_type *action)
{
	int			start_node_idx;
	char		err_str[256];
	d3pnt		pnt;
	d3ang		ang;
	node_type	*node;

		// always need a node for cameras

	if (node_idx==-1) return;

	node=&map.nodes[node_idx];

		// any camera action sets camera to static

	camera.setup.mode=cv_static;
	camera.setup.c_static.follow=FALSE;

		// camera placement

	if (action->action==cinema_action_place) {
		memmove(&camera.setup.pnt,&node->pnt,sizeof(d3pnt));
		memmove(&camera.setup.ang,&node->ang,sizeof(d3ang));
		
		return;
	}

		// camera walking

	if (action->action==cinema_action_move) {
		camera_get_position(&pnt,&ang);
		start_node_idx=map_find_nearest_node_by_point(&map,&pnt);
		if (start_node_idx==-1) return;

		if (!camera_walk_to_node_by_index_setup(start_node_idx,node_idx,(action->end_msec-action->start_msec),-1,FALSE,FALSE,err_str)) console_add_error(err_str);
	}
}

/* =======================================================

      Cinema Actions Objects
      
======================================================= */

void cinema_action_run_object(obj_type *obj,int node_idx,map_cinema_action_type *action)
{
	int					start_node_idx,dist;
	char				err_str[256];
	node_type			*node;

		// start any animations

	if (action->animation_name[0]!=0x0) {
		model_start_animation(&obj->draw,action->animation_name,game_time_get());
	}

	if (action->next_animation_name[0]!=0x0) {
		model_change_animation(&obj->draw,action->next_animation_name,game_time_get());
	}

		// object placement

	if (action->action==cinema_action_place) {
		if (node_idx==-1) return;

		node=&map.nodes[node_idx];

		obj->pnt.x=node->pnt.x;
		obj->pnt.y=node->pnt.y;
		obj->pnt.z=node->pnt.z;

		obj->ang.x=obj->motion.ang.x=node->ang.x;
		obj->ang.y=obj->motion.ang.y=node->ang.y;
		obj->ang.z=obj->motion.ang.z=node->ang.z;

		return;
	}

		// object walking

	if (action->action==cinema_action_move) {
		start_node_idx=map_find_nearest_node_by_point(&map,&obj->pnt);
		if (start_node_idx==-1) return;

			// find speed

		dist=map_node_to_node_distance(&map,start_node_idx,node_idx);
		obj->forward_move.speed=((float)dist)/((float)((action->end_msec-action->start_msec)/10));

			// throw an error if forward speed is
			// greater than object max speed

		if (obj->forward_move.speed>obj->forward_move.max_walk_speed) {
			sprintf(err_str,"%s max speed is too low to reach the node",obj->name);
			console_add_error(err_str);
			return;
		}

			// start walk

		if (!object_auto_walk_node_setup(obj,start_node_idx,node_idx,TRUE,-1,err_str)) console_add_error(err_str);

		return;
	}
	
		// none action causes a stop
		
	if (action->action==cinema_action_none) {
		object_move_stop(obj);
	}

		// show and hide

	if (action->action==cinema_action_show) {
		obj->hidden=FALSE;
		return;
	}

	if (action->action==cinema_action_hide) {
		obj->hidden=TRUE;
		return;
	}
}

/* =======================================================

      Cinema Actions
      
======================================================= */

void cinema_action_run_generic(map_cinema_action_type *action)
{
	int					node_idx,movement_idx,particle_idx;
	char				err_str[256];
	obj_type			*obj;

		// determine any nodes

	node_idx=map_find_node(&map,action->node_name);

		// route to proper action

	switch (action->actor_type) {

			// camera actor

		case cinema_actor_camera:
			cinema_action_run_camera(node_idx,action);
			return;

			// player actor

		case cinema_actor_player:
			obj=server.obj_list.objs[server.player_obj_idx];
			cinema_action_run_object(obj,node_idx,action);
			return;

			// object actor

		case cinema_actor_object:
			obj=object_find_name(action->actor_name);
			if (obj==NULL) {
				sprintf(err_str,"Unknown object in cinema: %s",action->actor_name);
				console_add_error(err_str);
				return;
			}
			cinema_action_run_object(obj,node_idx,action);
			return;

			// movement actor

		case cinema_actor_movement:
			movement_idx=map_movement_find(&map,action->actor_name);
			if (movement_idx==-1) {
				sprintf(err_str,"Unknown movement in cinema: %s",action->actor_name);
				console_add_error(err_str);
				return;
			}
			map_movements_cinema_start(movement_idx,action->move_reverse);
			return;

			// particle actor

		case cinema_actor_particle:
			particle_idx=particle_find_index(action->actor_name);
			if (particle_idx==-1) {
				sprintf(err_str,"Unknown particle in cinema: %s",action->actor_name);
				console_add_error(err_str);
				return;
			}
			if (node_idx==-1) {
				sprintf(err_str,"Need node to launch cinema particle: %s",action->actor_name);
				console_add_error(err_str);
				return;
			}
			particle_spawn(particle_idx,-1,&map.nodes[node_idx].pnt,NULL,NULL);
			return;
	}
}

bool cinema_actions(void)
{
	int						n,tick;
	map_cinema_type			*cinema;
	map_cinema_action_type	*action;

	cinema=&map.cinema.cinemas[view.cinema.idx];

		// is cinema over?

	tick=game_time_get()-view.cinema.start_tick;
	if (tick>=cinema->len_msec) return(TRUE);

		// run actions

	action=cinema->actions;

	for (n=0;n!=cinema->naction;n++) {

			// time to start action?

		if ((action->start_msec>=view.cinema.last_tick) && (action->start_msec<tick)) {
			cinema_action_run_generic(action);
		}

		action++;
	}

		// remember last tick

	view.cinema.last_tick=tick;

	return(FALSE);
}

/* =======================================================

      Run Cinema
      
======================================================= */

void cinema_run(void)
{
	bool				cinema_done;
	obj_type			*obj;

	if (!view.cinema.on) return;

		// run actions

	cinema_done=cinema_actions();
	
		// cinema over or mouse button pushed?
	
	if ((cinema_done) || (input_gui_get_mouse_left_button_down())) {

			// send the finish event

		if (view.cinema.event_id!=-1) {
			scripts_post_event_console(&js.game_attach,sd_event_interface,sd_event_interface_cinema_done,view.cinema.event_id);
			scripts_post_event_console(&js.course_attach,sd_event_map,sd_event_interface_cinema_done,view.cinema.event_id);

			if (net_setup.mode!=net_mode_host_dedicated) {
				obj=server.obj_list.objs[server.player_obj_idx];
				scripts_post_event_console(&obj->attach,sd_event_map,sd_event_interface_cinema_done,view.cinema.event_id);
			}
		}

		cinema_end();
	}
}


