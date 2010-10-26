/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Static Camera

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "objects.h"
#include "cameras.h"
#include "scripts.h"

extern server_type		server;
extern map_type			map;
extern camera_type		camera;
extern js_type			js;

/* =======================================================

      Setup Chase Camera Structure
      
======================================================= */

void camera_static_connect(void)
{
	camera.auto_walk.on=FALSE;
}

/* =======================================================

      Chase Camera Position
      
======================================================= */

void camera_static_get_position(d3pnt *pnt,d3ang *ang)
{
	obj_type		*obj;
    
		// get position

	memmove(pnt,&camera.setup.pnt,sizeof(d3pnt));

	if (ang==NULL) return;
	
		// if following an object, point camera at object

    if (camera.setup.c_static.follow) {
		obj=server.obj_list.objs[camera.obj_idx];
        ang->y=angle_find(camera.setup.pnt.x,camera.setup.pnt.z,obj->pnt.x,obj->pnt.z);
		ang->x=-(180.0f-angle_find(camera.setup.pnt.y,camera.setup.pnt.z,obj->pnt.y,obj->pnt.z));
 		ang->z=camera.setup.ang.z;
		return;
	}

		// else just use camera offset

	memmove(ang,&camera.setup.ang,sizeof(d3ang));
}

/* =======================================================

      Setup Camera Auto Moves
      
======================================================= */

bool camera_walk_to_node_by_index_setup(int from_idx,int to_idx,int msec,int event_id,bool open_doors,bool in_freeze,char *err_str)
{
	int			dist;
	float		speed;
	obj_type	*player_obj;

		// only works in static camera

	if (camera.setup.mode!=cv_static) {
		strcpy(err_str,"Can only walk cameras in static mode");
		return(FALSE);
	}
	
		// is end node in start node path?
		
	if (map_find_next_node_in_path(&map,from_idx,to_idx)==-1) {
		sprintf(err_str,"End node '%s' is not in the same path as the start node '%s'",map.nodes[to_idx].name,map.nodes[from_idx].name);
		return(FALSE);
	}

		// get total distance

	dist=map_node_to_node_distance(&map,from_idx,to_idx);
	if (dist==0) {
		strcpy(err_str,"Camera walk covers no distance");
		return(FALSE);
	}

	speed=(float)dist/(float)(msec/10);
	
		// start at the start node and seek to the next node
		
	memmove(&camera.setup.pnt,&map.nodes[from_idx].pnt,sizeof(d3pnt));
	memmove(&camera.setup.ang,&map.nodes[from_idx].ang,sizeof(d3ang));

	from_idx=map_find_next_node_in_path(&map,from_idx,to_idx);
	if (from_idx==-1) return(TRUE);
	
		// setup seeking
		
	camera.auto_walk.on=TRUE;
	camera.auto_walk.open_doors=open_doors;
	camera.auto_walk.in_freeze=in_freeze;
	camera.auto_walk.node_seek_idx=from_idx;
	camera.auto_walk.node_dest_idx=to_idx;
	camera.auto_walk.msec=msec;
	camera.auto_walk.speed=speed;
	camera.auto_walk.event_id=event_id;
	
		// player freeze
		
	if (in_freeze) {
		player_obj=server.obj_list.objs[server.player_obj_idx];
		object_input_freeze(player_obj,TRUE);
	}

	return(TRUE);
}

bool camera_walk_to_node_setup(char *start_node,char *end_node,int msec,int event_id,bool open_doors,bool in_freeze,char *err_str)
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

		// call the index version

	return(camera_walk_to_node_by_index_setup(from_idx,to_idx,msec,event_id,open_doors,in_freeze,err_str));
}

/* =======================================================

      Run Static Camera
      
======================================================= */

void camera_static_run(void)
{
	int			dist,seek_idx,dest_idx;
	float		sx,sy,sz,tot;
	node_type	*node;
	obj_type	*player_obj;

		// auto-walk on?

	if (!camera.auto_walk.on) return;
	
		// get nodes
		
	seek_idx=camera.auto_walk.node_seek_idx;
	dest_idx=camera.auto_walk.node_dest_idx;
		
	node=&map.nodes[seek_idx];

		// move towards node
		// get percentage of each different and use that
		// to adjust the speed

	sx=(float)abs(node->pnt.x-camera.setup.pnt.x);
	sy=(float)abs(node->pnt.y-camera.setup.pnt.y);
	sz=(float)abs(node->pnt.z-camera.setup.pnt.z);
	tot=sx+sy+sz;

	sx=sx/tot;
	sy=sy/tot;
	sz=sz/tot;

	sx*=camera.auto_walk.speed;
	if (node->pnt.x<camera.setup.pnt.x) sx=-sx;
	
	sy*=camera.auto_walk.speed;
	if (node->pnt.y<camera.setup.pnt.y) sy=-sy;
	
	sz*=camera.auto_walk.speed;
	if (node->pnt.z<camera.setup.pnt.z) sz=-sz;

	camera.setup.pnt.x+=(int)sx;
	camera.setup.pnt.y+=(int)sy;
	camera.setup.pnt.z+=(int)sz;

		// distance to seek node

	dist=distance_get(node->pnt.x,node->pnt.y,node->pnt.z,camera.setup.pnt.x,camera.setup.pnt.y,camera.setup.pnt.z);

		// get the look angle if not following

    if (!camera.setup.c_static.follow) {
		camera.setup.ang.x=angle_turn_toward(camera.setup.ang.x,node->ang.x,camera.auto_walk.turn_speed);
		camera.setup.ang.y=angle_turn_toward(camera.setup.ang.y,node->ang.y,camera.auto_walk.turn_speed);
		camera.setup.ang.z=angle_turn_toward(camera.setup.ang.z,node->ang.z,camera.auto_walk.turn_speed);
	}
	
		// near current seek node?
		
	if (dist>(int)(camera.auto_walk.speed*node_slop_speed_factor)) return;
	
		// finish with proper angle
		
    if (!camera.setup.c_static.follow) {
		memmove(&camera.setup.ang,&node->ang,sizeof(d3ang));
	}
	
		// move on to next node
		
	if (seek_idx!=dest_idx) {
		camera.auto_walk.node_seek_idx=map_find_next_node_in_path(&map,seek_idx,dest_idx);
		scripts_post_event_console(&js.course_attach,sd_event_path,sd_event_path_node,node->event_id);
		return;
	}
	
		// at last node
		
	camera.auto_walk.on=FALSE;

		// player freeze
		
	if (camera.auto_walk.in_freeze) {
		player_obj=server.obj_list.objs[server.player_obj_idx];
		object_input_freeze(player_obj,FALSE);
	}
	
		// send event
	
	scripts_post_event_console(&js.course_attach,sd_event_path,sd_event_path_done,camera.auto_walk.event_id);
}
