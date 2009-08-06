/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.node object

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

#include "scripts.h"

extern map_type			map;
extern server_type		server;
extern js_type			js;

JSBool js_map_node_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_map_node_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_map_node_find_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_map_node_find_random_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_map_node_find_nearest_to_object_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_map_node_find_nearest_names_in_path_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_map_node_find_nearest_unheld_weapon_in_path_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_map_node_next_in_path_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_map_node_get_adjacent_nodes_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_map_node_get_name_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_map_node_get_distance_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_map_node_get_angle_to_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_map_node_get_position_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);

script_js_function	map_node_functions[]={
							{"find",							js_map_node_find_func,									1},
							{"findRandom",						js_map_node_find_random_func,							1},
							{"findNearestToObject",				js_map_node_find_nearest_to_object_func,				1},
							{"findNearestNamesInPath",			js_map_node_find_nearest_names_in_path_func,			2},
							{"findNearestUnheldWeaponInPath",	js_map_node_find_nearest_unheld_weapon_in_path_func,	2},
							{"nextInPath",						js_map_node_next_in_path_func,							2},
							{"getAdjacentNodes",				js_map_node_get_adjacent_nodes_func,					1},
							{"getName",							js_map_node_get_name_func,								1},
							{"getDistance",						js_map_node_get_distance_func,							4},
							{"getAngleTo",						js_map_node_get_angle_to_func,							4},
							{"getPosition",						js_map_node_get_position_func,							1},
							{0}};

JSClassRef			map_node_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_node_object(void)
{
	map_node_class=script_create_class("map_node_class",js_map_node_get_property,js_map_node_set_property);
}

void script_free_map_node_object(void)
{
	script_free_class(map_node_class);
}

JSObject* script_add_map_node_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,map_node_class,"node",NULL,map_node_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_map_node_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,NULL));
}

JSBool js_map_node_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,NULL));
}

/* =======================================================

      Find Node Find Functions
      
======================================================= */

JSBool js_map_node_find_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				idx;
	char			name[name_str_len];
	
	script_value_to_string(argv[0],name,name_str_len);
	idx=map_find_node(&map,name);
	*rval=script_int_to_value(idx);
	return(TRUE);
}

JSBool js_map_node_find_random_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				idx;
	char			name[name_str_len];
	
	script_value_to_string(argv[0],name,name_str_len);
	idx=map_find_random_node(&map,name);
	*rval=script_int_to_value(idx);

	return(TRUE);
}

JSBool js_map_node_find_nearest_to_object_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				idx;
	obj_type		*obj;

	obj=script_find_obj_from_uid_arg(argv[0]);
	if (obj==NULL) return(FALSE);
	
	idx=map_find_nearest_node_by_point(&map,&obj->pnt);
	*rval=script_int_to_value(idx);
	
	return(TRUE);
}

JSBool js_map_node_find_nearest_names_in_path_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				n,idx,from_idx,k,len,d,dist;
	unsigned int	u_len;
	char			name[name_str_len];
	bool			good_param;
	JSObject		*array;
	JSValueRef			vp;
	
		// from node and name
		
	from_idx=script_value_to_int(argv[0]);
	
		// get number of array elements
		
	good_param=(argc>=2);
	if (good_param) {
		if (!JSVAL_IS_OBJECT(argv[1])) {
			good_param=FALSE;
		}
		else {
			good_param=JS_IsArrayObject(js.cx,JSVAL_TO_OBJECT(argv[1]));
		}
	}
	
	if (!good_param) {
		JS_ReportError(js.cx,"Second parameter to findNearestNamesInPath needs to be an array");
		return(FALSE);
	}
	
	array=JSVAL_TO_OBJECT(argv[1]);
	JS_GetArrayLength(js.cx,array,&u_len);
	
	len=(int)u_len;
	
		// check all names
		
	idx=-1;
	dist=-1;
		
	for (n=0;n!=len;n++) {
		JS_GetElement(js.cx,array,n,&vp);
		script_value_to_string(vp,name,name_str_len);
	
		k=map_find_nearest_node_in_path(&map,from_idx,name,&d);
		if (k==-1) continue;
		
		if ((dist==-1) || (d<dist)) {
			dist=d;
			idx=k;
		}
	}
	
	*rval=script_int_to_value(idx);
	
	return(TRUE);
}

JSBool js_map_node_find_nearest_unheld_weapon_in_path_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				n,idx,from_idx,k,d,dist;
	obj_type		*obj;
	weapon_type		*weap;
	
		// from node and object
		
	from_idx=script_value_to_int(argv[0]);

	obj=script_find_obj_from_uid_arg(argv[1]);
	if (obj==NULL) return(FALSE);
	
		// check all unheld weapons
		
	idx=-1;
	dist=-1;
	
	for (n=0;n!=server.count.weapon;n++) {
		weap=&server.weapons[n];
		if (weap->obj_uid!=obj->uid) continue;
		if (!weap->hidden) continue;
		
		k=map_find_nearest_node_in_path(&map,from_idx,weap->name,&d);
		if (k==-1) continue;
		
		if ((dist==-1) || (d<dist)) {
			dist=d;
			idx=k;
		}
	}
	
	*rval=script_int_to_value(idx);
	
	return(TRUE);
}

/* =======================================================

      Find Node Path Functions
      
======================================================= */

JSBool js_map_node_next_in_path_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				idx;
	
	idx=map_find_next_node_in_path(&map,script_value_to_int(argv[0]),script_value_to_int(argv[1]));
	*rval=script_int_to_value(idx);

	return(TRUE);
}

JSBool js_map_node_get_adjacent_nodes_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				n,cnt,link[max_node_link];
	node_type		*node;

	node=script_find_node_from_idx_arg(argv[0]);
	if (node==NULL) return(FALSE);

	cnt=0;

	for (n=0;n!=max_node_link;n++) {
		if (node->link[n]!=-1) {
			link[cnt++]=(int)node->link[n];
		}
	}

	*rval=script_int_array_to_value(cnt,link);

	return(TRUE);
}

/* =======================================================

      Node Name
      
======================================================= */

JSBool js_map_node_get_name_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	node_type		*node;

		// node
		
	node=script_find_node_from_idx_arg(argv[0]);
	if (node==NULL) return(FALSE);
	
		// get position
		
	*rval=script_string_to_value(node->name);
	return(TRUE);
}

/* =======================================================

      Node Distance and Angle To
      
======================================================= */

JSBool js_map_node_get_distance_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				x,z,y;
	node_type		*node;
	
		// node
		
	node=script_find_node_from_idx_arg(argv[0]);
	if (node==NULL) return(FALSE);

		// x,z,y
		
	x=script_value_to_int(argv[1]);
	z=script_value_to_int(argv[2]);
	y=script_value_to_int(argv[3]);
	
		// get distance
		
	*rval=script_int_to_value(distance_get(node->pnt.x,node->pnt.y,node->pnt.z,x,y,z));
	
	return(TRUE);
}

JSBool js_map_node_get_angle_to_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				x,z,y;
	float			ang_x,ang_z,ang_y;
	node_type		*node;
	
		// node
		
	node=script_find_node_from_idx_arg(argv[0]);
	if (node==NULL) return(FALSE);

		// x,z,y
		
	x=script_value_to_int(argv[1]);
	z=script_value_to_int(argv[2]);
	y=script_value_to_int(argv[3]);
	
		// get angles to
		
	ang_x=angle_find(y,z,node->pnt.y,node->pnt.z);
	ang_y=angle_find(x,z,node->pnt.x,node->pnt.z);
	ang_z=angle_find(x,y,node->pnt.x,node->pnt.y);
		
	*rval=script_angle_to_value(ang_x,ang_y,ang_z);
	
	return(TRUE);
}

/* =======================================================

      Node Position and Angle
      
======================================================= */

JSBool js_map_node_get_position_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	node_type		*node;

		// node
		
	node=script_find_node_from_idx_arg(argv[0]);
	if (node==NULL) return(FALSE);
	
		// get position
		
	*rval=script_point_to_value(node->pnt.x,node->pnt.y,node->pnt.z);
	return(TRUE);
}

