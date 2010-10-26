/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Static Camera

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

      Setup Node To Node Walk Values
      
======================================================= */

void camera_walk_to_node_setup_speed_turn(void)
{
	float				f_dist;
	bool				cwise;
	node_type			*node;

	node=&map.nodes[camera.auto_walk.node_seek_idx];

		// get walk distance to this node
		// and normalize

	vector_create(&camera.auto_walk.walk_vct,node->pnt.x,node->pnt.y,node->pnt.z,camera.setup.pnt.x,camera.setup.pnt.y,camera.setup.pnt.z);

		// calculate speed

	camera.auto_walk.walk_vct.x*=camera.auto_walk.speed;
	camera.auto_walk.walk_vct.y*=camera.auto_walk.speed;
	camera.auto_walk.walk_vct.z*=camera.auto_walk.speed;

		// the walk add vector, to get
		// around the lose in percision

	camera.auto_walk.walk_add_vct.x=0.0f;
	camera.auto_walk.walk_add_vct.y=0.0f;
	camera.auto_walk.walk_add_vct.z=0.0f;

		// calulcate the turning
		// get the % of distance, and find the number
		// of ticks we'll cross

	f_dist=(float)distance_get(node->pnt.x,node->pnt.y,node->pnt.z,camera.setup.pnt.x,camera.setup.pnt.y,camera.setup.pnt.z);
	f_dist=f_dist/((float)camera.auto_walk.total_dist);
	f_dist=(((float)camera.auto_walk.msec)*0.1f)*f_dist;

	camera.auto_walk.turn_ang.x=angle_dif(node->ang.x,camera.setup.ang.x,&cwise)/f_dist;
	camera.auto_walk.turn_ang.y=angle_dif(node->ang.y,camera.setup.ang.y,&cwise)/f_dist;
	camera.auto_walk.turn_ang.z=angle_dif(node->ang.z,camera.setup.ang.z,&cwise)/f_dist;
}

/* =======================================================

      Setup Camera Auto Moves
      
======================================================= */

bool camera_walk_to_node_by_index_setup(int from_idx,int to_idx,int msec,int event_id,bool open_doors,bool in_freeze,char *err_str)
{
	int			dist;
	float		speed;
	obj_type	*player_obj;
	node_type	*node;

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
		// make sure to add in the distance it takes to
		// get to the first node

	node=&map.nodes[from_idx];

	dist=map_node_to_node_distance(&map,from_idx,to_idx);
	dist+=distance_get(camera.setup.pnt.x,camera.setup.pnt.y,camera.setup.pnt.z,node->pnt.x,node->pnt.y,node->pnt.z);

	if (dist==0) {
		strcpy(err_str,"Camera walk covers no distance");
		return(FALSE);
	}

	speed=((float)dist)/(float)(msec/10);
	
		// get from node

	from_idx=map_find_next_node_in_path(&map,from_idx,to_idx);
	if (from_idx==-1) return(TRUE);
	
		// setup seeking
		
	camera.auto_walk.on=TRUE;
	camera.auto_walk.open_doors=open_doors;
	camera.auto_walk.in_freeze=in_freeze;
	camera.auto_walk.node_seek_idx=from_idx;
	camera.auto_walk.node_dest_idx=to_idx;
	camera.auto_walk.total_dist=dist;
	camera.auto_walk.msec=msec;
	camera.auto_walk.speed=speed;
	camera.auto_walk.event_id=event_id;
	
		// setup the speed and turning
		// for walking to next node

	camera_walk_to_node_setup_speed_turn();
	
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
	int			x,y,z,dist,seek_idx,dest_idx;
	node_type	*node;
	obj_type	*player_obj;

		// auto-walk on?

	if (!camera.auto_walk.on) return;
	
		// get nodes
		
	seek_idx=camera.auto_walk.node_seek_idx;
	dest_idx=camera.auto_walk.node_dest_idx;
		
	node=&map.nodes[seek_idx];

		// walk towards node

	camera.auto_walk.walk_add_vct.x+=camera.auto_walk.walk_vct.x;
	camera.auto_walk.walk_add_vct.x+=camera.auto_walk.walk_vct.x;
	camera.auto_walk.walk_add_vct.x+=camera.auto_walk.walk_vct.x;

	x=(int)camera.auto_walk.walk_add_vct.x;
	y=(int)camera.auto_walk.walk_add_vct.y;
	z=(int)camera.auto_walk.walk_add_vct.z;

	camera.setup.pnt.x+=x;
	camera.setup.pnt.y+=y;
	camera.setup.pnt.z+=z;

	camera.auto_walk.walk_add_vct.x-=(float)x;
	camera.auto_walk.walk_add_vct.y-=(float)y;
	camera.auto_walk.walk_add_vct.z-=(float)z;

		// get the look angle if not following

    if (!camera.setup.c_static.follow) {
		camera.setup.ang.x=angle_turn_toward(camera.setup.ang.x,node->ang.x,camera.auto_walk.turn_ang.x);
		camera.setup.ang.y=angle_turn_toward(camera.setup.ang.y,node->ang.y,camera.auto_walk.turn_ang.y);
		camera.setup.ang.z=angle_turn_toward(camera.setup.ang.z,node->ang.z,camera.auto_walk.turn_ang.z);
	}
	
		// near current seek node?

	dist=distance_get(node->pnt.x,node->pnt.y,node->pnt.z,camera.setup.pnt.x,camera.setup.pnt.y,camera.setup.pnt.z);
	
	if (dist>(int)(camera.auto_walk.speed*node_slop_speed_factor)) return;
	
		// move on to next node
		
	if (seek_idx!=dest_idx) {
		camera.auto_walk.node_seek_idx=map_find_next_node_in_path(&map,seek_idx,dest_idx);
		camera_walk_to_node_setup_speed_turn();

		scripts_post_event_console(&js.course_attach,sd_event_path,sd_event_path_node,node->event_id);
		return;
	}
	
		// at last node
		
	camera.auto_walk.on=FALSE;
	
		// finish by setting everything
		// to end node to eliminate any slop
		
	memmove(&camera.setup.pnt,&node->pnt,sizeof(d3pnt));
	if (!camera.setup.c_static.follow) memmove(&camera.setup.ang,&node->ang,sizeof(d3ang));

		// player freeze
		
	if (camera.auto_walk.in_freeze) {
		player_obj=server.obj_list.objs[server.player_obj_idx];
		object_input_freeze(player_obj,FALSE);
	}
	
		// send event
	
	scripts_post_event_console(&js.course_attach,sd_event_path,sd_event_path_done,camera.auto_walk.event_id);
}
