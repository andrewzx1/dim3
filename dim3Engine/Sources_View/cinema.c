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
#include "consoles.h"
#include "cameras.h"
#include "inputs.h"
#include "timing.h"

extern void server_loop(void);
extern void view_run(void);
extern void view_loop_draw(void);
extern void map_movements_cinema_start(int movement_idx,bool reverse);

extern server_type				server;
extern map_type					map;
extern js_type					js;
extern network_setup_type		net_setup;

extern camera_type				camera;

int								cinema_last_state,cinema_event_id,
								cinema_idx,cinema_start_tick,cinema_last_tick;
camera_type						cinema_camera_state;

/* =======================================================

      Cinema Operations
      
======================================================= */

void cinema_open(void)
{
	cinema_start_tick=game_time_get();
	cinema_last_tick=0;

	memmove(&cinema_camera_state,&camera,sizeof(camera_type));
}

void cinema_close(void)
{
	memmove(&camera,&cinema_camera_state,sizeof(camera_type));
}

bool cinema_setup(char *name,int event_id,char *err_str)
{
	int				n;

		// get the cinema

	cinema_idx=-1;

	for (n=0;n!=map.cinema.ncinema;n++) {
		if (strcmp(name,map.cinema.cinemas[n].name)==0) {
			cinema_idx=n;
			break;
		}
	}

	if (cinema_idx==-1) {
		sprintf(err_str,"No cinema exists with this name: %s",name);
		return(FALSE);
	}

		// remember last state and event

	cinema_last_state=server.next_state;
	cinema_event_id=event_id;
	
		// switch to cinema state
		
	server.next_state=gs_cinema;
	
	return(TRUE);
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
		camera_static_update(&node->pnt);

		camera.setup.ang.x=node->ang.x;
		camera.setup.ang.y=node->ang.y;
		camera.setup.ang.z=node->ang.z;
		
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

			// start walk

		if (!object_auto_walk_node_setup(obj,start_node_idx,node_idx,-1,err_str)) console_add_error(err_str);

		return;
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
	int					node_idx,movement_idx;
	obj_type			*obj;

		// determine any nodes

	node_idx=map_find_node(&map,action->node_name);

		// camera actor

	if (strcasecmp(action->actor_name,"camera")==0) {
		cinema_action_run_camera(node_idx,action);
		return;
	}

		// object actor

	obj=object_find_name(action->actor_name);
	if (obj!=NULL) {
		cinema_action_run_object(obj,node_idx,action);
		return;
	}

		// movement actor

	movement_idx=map_movement_find(&map,action->actor_name);
	if (movement_idx!=-1) {
		map_movements_cinema_start(movement_idx,action->move_reverse);
		return;
	}

		// particle actor


	// supergumba -- particles here

}

bool cinema_actions(void)
{
	int						n,tick;
	map_cinema_type			*cinema;
	map_cinema_action_type	*action;

	cinema=&map.cinema.cinemas[cinema_idx];

		// is cinema over?

	tick=game_time_get()-cinema_start_tick;
	if (tick>=cinema->len_msec) return(TRUE);

		// run actions

	action=cinema->actions;

	for (n=0;n!=cinema->naction;n++) {

			// time to start action?

		if ((action->start_msec>=cinema_last_tick) && (action->start_msec<tick)) {
			cinema_action_run_generic(action);
		}

		action++;
	}

		// remember last tick

	cinema_last_tick=tick;

	return(FALSE);
}

/* =======================================================

      Run Cinema
      
======================================================= */

void cinema_run(void)
{
	bool				cinema_done;
	obj_type			*obj;

		// run actions

	cinema_done=cinema_actions();

		// run and draw the game

	server_loop();
	view_run();
	view_loop_draw();
	
		// cinema over or mouse button pushed?
	
	if ((cinema_done) || (input_gui_get_mouse_left_button_down())) {

			// send the finish event

		if (cinema_event_id!=-1) {
			scripts_post_event_console(&js.game_attach,sd_event_interface,sd_event_interface_cinema_done,cinema_event_id);
			scripts_post_event_console(&js.course_attach,sd_event_map,sd_event_interface_cinema_done,cinema_event_id);

			if (net_setup.mode!=net_mode_host_dedicated) {
				obj=server.obj_list.objs[server.player_obj_idx];
				scripts_post_event_console(&obj->attach,sd_event_map,sd_event_interface_cinema_done,cinema_event_id);
			}
		}

		server.next_state=cinema_last_state;
	}
}


