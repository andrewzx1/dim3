/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Auto Walking

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
#include "scripts.h"
#include "objects.h"

extern map_type				map;
extern server_type			server;
extern js_type				js;

/* =======================================================

      Setup Walk
      
======================================================= */

bool object_auto_walk_node_setup(obj_type *obj,int from_idx,int to_idx,bool skip_event,int event_id,char *err_str)
{
		// check for valid nodes
		
	if ((from_idx<0) || (from_idx>=map.nnode)) {
		strcpy(err_str,"Start node is not a valid node Id");
		return(FALSE);
	}
	
	if ((to_idx<0) || (to_idx>=map.nnode)) {
		strcpy(err_str,"End node is not a valid node Id");
		return(FALSE);
	}

		// is end node in start node path?
		// skip check if to and from are the same
		
	if (from_idx!=to_idx) {
		if (map_find_next_node_in_path(&map,from_idx,to_idx)==-1) {
			strcpy(err_str,"End node is not in the same path as the start node");
			return(FALSE);
		}
	}
	
		// setup seeking
		
	obj->auto_walk.mode=aw_node;
	obj->auto_walk.node_from_idx=obj->auto_walk.node_seek_idx=obj->auto_walk.node_last_seek_idx=from_idx;
	obj->auto_walk.node_dest_idx=to_idx;
	
	obj->auto_walk.dodge.on=FALSE;
	
		// setup event and start walking
		
	obj->auto_walk.skip_event=skip_event;
	obj->auto_walk.node_event_id=event_id;
	obj->auto_walk.pause_for_turn=FALSE;

	obj->forward_move.moving=TRUE;

	return(TRUE);
}

bool object_auto_walk_node_name_setup(obj_type *obj,char *start_node,char *end_node,int event_id,char *err_str)
{
	int			from_idx,to_idx;
	
		// get the nodes
		
	from_idx=map_find_node(&map,start_node);
	if (from_idx==-1) {
		sprintf(err_str,"Named node does not exist: %s",start_node);
		return(FALSE);
	}
	
	to_idx=map_find_node(&map,end_node);
	if (to_idx==-1) {
		sprintf(err_str,"Named node does not exist: %s",end_node);
		return(FALSE);
	}
	
	return(object_auto_walk_node_setup(obj,from_idx,to_idx,FALSE,event_id,err_str));
}

bool object_auto_walk_object_setup(obj_type *obj,int uid,bool turn_only,char *err_str)
{
		// does seek object exist?
		
	if (server.obj_list.objs[uid]==NULL) {
		sprintf(err_str,"An object with unique ID '%d' does not exist'",uid);
		return(FALSE);
	}
	
		// setup to seek object
		
	obj->auto_walk.mode=turn_only?aw_object_turn_only:aw_object;
	obj->auto_walk.obj_idx=uid;
	
	obj->auto_walk.dodge.on=FALSE;

		// start walking
		
	obj->forward_move.moving=!turn_only;

	return(TRUE);
}

bool object_auto_walk_player_setup(obj_type *obj,bool turn_only,char *err_str)
{
	return(object_auto_walk_object_setup(obj,server.player_obj_idx,turn_only,err_str));
}

void object_auto_walk_position_setup(obj_type *obj,d3pnt *pnt)
{
		// setup walk to position
		
	obj->auto_walk.mode=aw_position;
	memmove(&obj->auto_walk.pnt,pnt,sizeof(d3pnt));
	
	obj->auto_walk.dodge.on=FALSE;

		// start walking
		
	obj->auto_walk.pause_for_turn=FALSE;
	obj->forward_move.moving=TRUE;
}

bool object_auto_walk_node_resume(obj_type *obj,char *err_str)
{
		// is there something to resume?
		
	if ((obj->auto_walk.node_seek_idx==-1) || (obj->auto_walk.node_dest_idx==-1)) {
		strcpy(err_str,"There is no node walk to resume from");
		return(FALSE);
	}
	
		// resume seeking
		
	obj->auto_walk.mode=aw_node;
	obj->auto_walk.dodge.on=FALSE;

		// start walking
		
	obj->forward_move.moving=TRUE;

	return(TRUE);
}

bool object_auto_walk_node_reverse(obj_type *obj,char *err_str)
{
		// is there something to resume?
		
	if ((obj->auto_walk.node_from_idx==-1) || (obj->auto_walk.node_last_seek_idx==-1)) {
		strcpy(err_str,"There is no node walk to reverse");
		return(FALSE);
	}
	
		// reverse
		
	obj->auto_walk.mode=aw_node;
	obj->auto_walk.node_dest_idx=obj->auto_walk.node_from_idx;
	obj->auto_walk.node_seek_idx=obj->auto_walk.node_last_seek_idx;
	obj->auto_walk.dodge.on=FALSE;
	
		// start walking if not already
		
	obj->forward_move.moving=TRUE;

	return(TRUE);
}

void object_auto_walk_stop(obj_type *obj)
{
	obj->auto_walk.mode=aw_none;
	obj->auto_walk.dodge.on=FALSE;
}

void object_auto_walk_dodge_setup(obj_type *obj,float dodge_angle,int dodge_msec)
{
	float			org_ang;
	
		// if already in a dodge, make sure
		// we dodge against the original angle not
		// the dodging angle
		
	if (obj->auto_walk.dodge.on) {
		org_ang=obj->auto_walk.dodge.org_ang;
	}
	else {
		org_ang=obj->motion.ang.y;
	}
	
		// turn on dodge
		
	obj->auto_walk.dodge.on=TRUE;
	obj->auto_walk.dodge.end_tick=game_time_get()+dodge_msec;
	obj->auto_walk.dodge.org_ang=org_ang;
	obj->auto_walk.dodge.dodge_ang=angle_add(org_ang,dodge_angle);

		// setup angles and make sure object is walking

	obj->turn.ang_add.y=0.0f;
	obj->motion.ang.y=obj->auto_walk.dodge.dodge_ang;

	obj->forward_move.moving=TRUE;
}

/* =======================================================

      Auto Walking Get End Position
      
======================================================= */

bool object_auto_walk_get_seek_position(obj_type *obj,d3pnt *pnt)
{
	obj_type		*seek_obj;

	switch (obj->auto_walk.mode) {

		case aw_node:
			memmove(pnt,&map.nodes[obj->auto_walk.node_seek_idx].pnt,sizeof(d3pnt));
			return(TRUE);

		case aw_object:
		case aw_object_turn_only:
			seek_obj=server.obj_list.objs[obj->auto_walk.obj_idx];
			if (seek_obj==NULL) return(FALSE);
			
			pnt->x=seek_obj->pnt.x;
			pnt->y=seek_obj->pnt.y;
			pnt->z=seek_obj->pnt.z;
			return(TRUE);

		case aw_position:
			memmove(pnt,&obj->auto_walk.pnt,sizeof(d3pnt));
			return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Auto Walking
      
======================================================= */

void object_auto_walk_set_vertical_move(obj_type *obj,int to_y,int to_z)
{
	int			y;
	
	obj->vert_move.seeking=FALSE;
	
	if (!obj->fly) return;
	
		// under slop?
		
	y=obj->pnt.y-(obj->size.y>>1);
	if (abs(to_y-y)<obj->vert_move.slop) return;
	
		// get seek angle
		
	obj->vert_move.seeking=TRUE;
	obj->vert_move.seek_ang=angle_find(y,obj->pnt.z,to_y,to_z);
}

void object_auto_walk_set_motion(obj_type *obj)
{
	float			dif_y,dif_ang;
	bool			cwise;
	
		// if we are already paused for
		// turning, then it's a different
		// value that takes us out of pause
		
	if (obj->auto_walk.pause_for_turn) {
		dif_ang=node_pause_min_angle;
	}
	else {
		dif_ang=node_pause_max_angle;
	}
	
		// see if we can move regularly
		
	dif_y=angle_dif(obj->turn.ang_to.y,obj->ang.y,&cwise);
	
	if (dif_y<dif_ang) {
		obj->auto_walk.pause_for_turn=FALSE;
		obj->forward_move.moving=TRUE;
		return;
	}
		
		// need to pause

	obj->auto_walk.pause_for_turn=TRUE;
	obj->forward_move.moving=FALSE;
}

void object_auto_walk_node(obj_type *obj)
{
	int				dist,seek_idx,dest_idx;
	float			ang_y,dif_y;
	bool			cwise;
	node_type		*node;
	
		// get nodes
		
	seek_idx=obj->auto_walk.node_seek_idx;
	dest_idx=obj->auto_walk.node_dest_idx;
	
		// turn towards node
		
	node=&map.nodes[seek_idx];
	
	ang_y=angle_find(obj->pnt.x,obj->pnt.z,node->pnt.x,node->pnt.z);
	dif_y=angle_dif(ang_y,obj->ang.y,&cwise);
	
	obj->turn.ang_to.y=ang_y;

	if (cwise) {
		obj->turn.ang_add.y=-object_get_turn_speed(obj);
	}
	else {
		obj->turn.ang_add.y=object_get_turn_speed(obj);
	}
	
		// stop walking if turn is too hard
		
	object_auto_walk_set_motion(obj);
	
		// if flying, put in a seek angle
		
	object_auto_walk_set_vertical_move(obj,node->pnt.y,node->pnt.z);
	
		// near node?
		// if we can't fly, only consider near X/Z
		
	if (!obj->fly) {
		dist=distance_2D_get(node->pnt.x,node->pnt.z,obj->pnt.x,obj->pnt.z);
	}
	else {
		dist=distance_get(node->pnt.x,node->pnt.y,node->pnt.z,obj->pnt.x,obj->pnt.y,obj->pnt.z);
	}

	if (dist>obj->size.node_touch_dist) return;
	
		// move on to next node
		
	if (seek_idx!=dest_idx) {
		obj->auto_walk.node_last_seek_idx=obj->auto_walk.node_seek_idx;
		obj->auto_walk.node_seek_idx=map_find_next_node_in_path(&map,seek_idx,dest_idx);
		if (!obj->auto_walk.skip_event) scripts_post_event_console(obj->script_idx,-1,sd_event_path,sd_event_path_node,node->event_id);
		return;
	}
	
		// at last node, send event
		
	obj->auto_walk.mode=aw_none;
	obj->auto_walk.node_seek_idx=-1;
	obj->auto_walk.node_dest_idx=-1;
	
	if (!obj->auto_walk.skip_event) scripts_post_event_console(obj->script_idx,-1,sd_event_path,sd_event_path_done,obj->auto_walk.node_event_id);
}

void object_auto_walk_object(obj_type *obj)
{
	float			ang_y;
	bool			cwise;
	obj_type		*seek_obj;
	
		// get object
		
	seek_obj=server.obj_list.objs[obj->auto_walk.obj_idx];
	if (seek_obj==NULL) {
		obj->auto_walk.mode=aw_none;
		return;
	}
	
		// turn towards object on y
		
	ang_y=angle_find(obj->pnt.x,obj->pnt.z,seek_obj->pnt.x,seek_obj->pnt.z);
	angle_dif(ang_y,obj->ang.y,&cwise);
	
	obj->turn.ang_to.y=ang_y;
	
	if (cwise) {
		obj->turn.ang_add.y=-object_get_turn_speed(obj);
	}
	else {
		obj->turn.ang_add.y=object_get_turn_speed(obj);
	}
	
		// if flying, put in a seek angle
		
	object_auto_walk_set_vertical_move(obj,(seek_obj->pnt.y-(seek_obj->size.y>>1)),seek_obj->pnt.z);
}

void object_auto_walk_position(obj_type *obj)
{
	float			ang_y;
	bool			cwise;

		// turn towards position on y
		
	ang_y=angle_find(obj->pnt.x,obj->pnt.z,obj->auto_walk.pnt.x,obj->auto_walk.pnt.z);
	angle_dif(ang_y,obj->ang.y,&cwise);
	
	obj->turn.ang_to.y=ang_y;
	
	if (cwise) {
		obj->turn.ang_add.y=-object_get_turn_speed(obj);
	}
	else {
		obj->turn.ang_add.y=object_get_turn_speed(obj);
	}
	
		// if flying, put in a seek angle
		
	object_auto_walk_set_vertical_move(obj,obj->auto_walk.pnt.y,obj->auto_walk.pnt.z);
}

/* =======================================================

      Auto Walk Dodging
      
======================================================= */

void object_auto_walk_dodge(obj_type *obj)
{
		// force dodge to keep dodge angle

	obj->turn.ang_add.y=0.0f;
	obj->motion.ang.y=obj->auto_walk.dodge.dodge_ang;

		// is dodge over?
		
	if (obj->auto_walk.dodge.end_tick<game_time_get()) return;
	
		// resume auto-walk
		
	obj->auto_walk.dodge.on=FALSE;
}

/* =======================================================

      Run Auto Walk
      
======================================================= */

void object_auto_walk(obj_type *obj)
{
		// are we dodging?
		
	if (obj->auto_walk.dodge.on) {
		object_auto_walk_dodge(obj);
		return;
	}
	
		// regular auto walking
		
	switch (obj->auto_walk.mode) {

		case aw_node:
			object_auto_walk_node(obj);
			return;

		case aw_object:
		case aw_object_turn_only:
			object_auto_walk_object(obj);
			return;

		case aw_position:
			object_auto_walk_position(obj);
			return;

	}
}

