/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Physics Functions Header

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

// map collisions

extern bool collide_contact_is_wall_hit(poly_pointer_type *hit_poly);
extern bool collide_object_to_map(obj_type *obj,d3pnt *motion);
extern bool collide_object_to_map_bump(obj_type *obj,d3pnt *motion,int *bump_y_move);
extern bool collide_object_to_mesh(obj_type *obj,int mesh_idx);
extern bool collide_object_to_object(obj_type *obj,d3pnt *motion,obj_type *chk_obj,bool skip_pickup);
extern int collide_object_for_object_stand(obj_type *obj);
extern bool collide_object_to_sphere(d3pnt *sphere_pnt,int radius,obj_type *obj);

extern bool collide_projectile_to_map(proj_type *proj,d3pnt *motion);
extern bool collide_projectile_to_sphere(d3pnt *sphere_pnt,int radius,proj_type *proj);

extern void collide_objects_push(d3pnt *push_pnt,int radius,int force);

// polygon APIs

extern int collide_polygon_find_faced_by_object(obj_type *obj);
extern int collide_polygon_distance_to_object(int poly_uid,obj_type *obj);
extern void collide_polygon_hit_point_to_object(int poly_uid,obj_type *obj,d3pnt *pt);
extern void collide_polygon_get_normal(int poly_uid,d3vct *normal);
extern float collide_polygon_dot_product_to_object(int poly_uid,obj_type *obj);

// misc

extern int find_poly_nearest_stand(int x,int y,int z,int ydist,bool ignore_higher);
extern int pin_downward_movement_point(int x,int y,int z,int ydist,poly_pointer_type *stand_poly);
extern int pin_downward_movement_obj(obj_type *obj,int my);
extern int pin_upward_movement_point(int x,int y,int z,int ydist,poly_pointer_type *head_poly);
extern int pin_upward_movement_obj(obj_type *obj,int my);
extern bool map_stand_crush_object(obj_type *obj);
extern bool map_stand_check_object(obj_type *obj);

// ray tracing

extern bool ray_trace_initialize(char *err_str);
extern void ray_trace_shutdown(void);
extern ray_trace_check_item_type* ray_trace_get_last_item_list(int *item_count);
extern void ray_push(d3pnt *pt,d3ang *ang,int dist);
extern void ray_push_to_end(d3pnt *pt,d3pnt *ept,int dist);
extern bool ray_trace_map_by_angle(d3pnt *spt,d3ang *ang,int dist,d3pnt *hpt,ray_trace_contact_type *contact);
extern bool ray_trace_map_by_point(d3pnt *spt,d3pnt *ept,d3pnt *hpt,ray_trace_contact_type *contact);
extern void ray_trace_map_by_point_array(int cnt,d3pnt *spt,d3pnt *ept,d3pnt *hpt,bool *hits,ray_trace_contact_type *base_contact,ray_trace_contact_type *contacts);
extern void ray_trace_map_by_point_array_no_contact(int cnt,d3pnt *spt,d3pnt *ept,d3pnt *hpt,bool *hits,ray_trace_contact_type *base_contact);
extern bool ray_trace_mesh_poly_plane_by_vector(int cnt,d3pnt *spt,d3vct *vct,d3pnt *hpt,int mesh_idx,int poly_idx);

