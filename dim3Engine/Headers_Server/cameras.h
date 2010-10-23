/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Camera Functions Header

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

extern void camera_initialize(void);
extern void camera_map_setup(void);
extern void camera_connect(obj_type *obj);
extern void camera_get_position(d3pnt *pnt,d3ang *ang);
extern void camera_get_angle_from(d3pnt *pt,d3ang *ang);
extern int camera_check_liquid(obj_type *obj,d3pnt *pnt);
extern void camera_save(void);
extern void camera_restore(void);
extern void camera_auto_move_set_ang(d3ang *ang,int life_msec);
extern void camera_run(void);

extern void camera_fpp_get_position(d3pnt *pnt,d3ang *ang);

extern void camera_chase_connect(void);
extern float camera_chase_run_slop(float ang,float goal,float slop,float chase_track_speed);
extern void camera_chase_run(void);
extern int camera_chase_get_division(int x,int z,int y);
extern void camera_chase_get_position(d3pnt *pnt,d3ang *ang);
extern void camera_chase_static_get_position(d3pnt *pnt,d3ang *ang);

extern void camera_static_connect(void);
extern void camera_static_update(d3pnt *pnt);
extern void camera_static_get_position(d3pnt *pnt,d3ang *ang);
extern bool camera_walk_to_node_by_index_setup(int from_idx,int to_idx,int msec,int event_id,bool open_doors,bool in_freeze,char *err_str);
extern bool camera_walk_to_node_setup(char *start_node,char *end_node,int msec,int event_id,bool open_doors,bool in_freeze,char *err_str);
extern void camera_static_run(void);
