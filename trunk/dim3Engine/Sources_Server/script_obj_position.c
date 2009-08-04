/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.position object

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
#include "objects.h"

extern map_type				map;
extern server_type			server;
extern js_type				js;
extern network_setup_type	net_setup;

extern void object_setup_motion(obj_type *obj,float ang,float speed);
extern JSBool js_obj_position_place_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
extern JSBool js_obj_position_pause_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
							
JSBool js_obj_position_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_position_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_position_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_position_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_position_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_position_place_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_position_place_random_spot_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_position_place_network_spot_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_position_place_random_spot_no_telefrag_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_position_place_network_spot_no_telefrag_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_position_move_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_position_reset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_position_distance_to_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_position_distance_to_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_property	obj_position_props[]={
							{"x",								js_obj_position_get_x,				NULL},
							{"y",								js_obj_position_get_y,				NULL},
							{"z",								js_obj_position_get_z,				NULL},
							{0}};

script_js_function	obj_position_functions[]={
							{"place",							js_obj_position_place_func,								4},
							{"placeRandomSpot",					js_obj_position_place_random_spot_func,					2},
							{"placeNetworkSpot",				js_obj_position_place_network_spot_func,				0},
							{"placeRandomSpotNoTelefrag",		js_obj_position_place_random_spot_no_telefrag_func,		2},
							{"placeNetworkSpotNoTelefrag",		js_obj_position_place_network_spot_no_telefrag_func,	0},
							{"move",							js_obj_position_move_func,								3},
							{"reset",							js_obj_position_reset_func,								0},
							{"distanceToPlayer",				js_obj_position_distance_to_player_func,				0},
							{"distanceToObject",				js_obj_position_distance_to_object_func,				1},
							{0}};

JSClassRef			obj_position_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_position_object(void)
{
	obj_position_class=script_create_class("obj_position_class",js_obj_position_get_property,js_obj_position_set_property);
}

void script_free_obj_position_object(void)
{
	script_free_class(obj_position_class);
}

JSObject* script_add_obj_position_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,obj_position_class,"position",obj_position_props,obj_position_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_obj_position_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,obj_position_props));
}

JSBool js_obj_position_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,obj_position_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_obj_position_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->pnt.x);
	
	return(JS_TRUE);
}

bool js_obj_position_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->pnt.y);
	
	return(JS_TRUE);
}

bool js_obj_position_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->pnt.z);
	
	return(JS_TRUE);
}

/* =======================================================

      Reposition Functions
      
======================================================= */

JSBool js_obj_position_place_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	object_set_position(obj,script_value_to_int(argv[0]),script_value_to_int(argv[2]),script_value_to_int(argv[1]),script_value_to_float(argv[3]),0);
	object_telefrag_players(obj,FALSE);
    
    *rval=script_bool_to_value(TRUE);

	return(JS_TRUE);
}

JSBool js_obj_position_place_random_spot_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	spot_type		*spot;
	
	obj=object_find_uid(js.attach.thing_uid);
	
		// find spot
		
	spot=script_find_spot_from_name_type(argv[0],argv[1]);
	if (spot==NULL) return(JS_FALSE);
	
		// move player
	
	object_set_position(obj,spot->pnt.x,spot->pnt.y,spot->pnt.z,spot->ang.y,0);
 	object_telefrag_players(obj,FALSE);

	return(JS_TRUE);
}

JSBool js_obj_position_place_network_spot_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	spot_type		*spot;
	
	obj=object_find_uid(js.attach.thing_uid);

		// get spot
		
	spot=script_find_network_spot(obj);
	if (spot==NULL) return(JS_FALSE);
	
		// move player
	
	object_set_position(obj,spot->pnt.x,spot->pnt.y,spot->pnt.z,spot->ang.y,0);
 	object_telefrag_players(obj,FALSE);
   
    *rval=script_bool_to_value(TRUE);

	return(JS_TRUE);
}

JSBool js_obj_position_place_random_spot_no_telefrag_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	d3pnt			old_pnt;
	obj_type		*obj;
	spot_type		*spot;
	
	obj=object_find_uid(js.attach.thing_uid);
		
		// find spot
		
	spot=script_find_spot_from_name_type(argv[0],argv[1]);
	if (spot==NULL) return(JS_FALSE);
	
		// can we move without telefragging?
	
	memmove(&old_pnt,&obj->pnt,sizeof(d3pnt));
	memmove(&obj->pnt,&spot->pnt,sizeof(d3pnt));

	if (object_telefrag_players(obj,TRUE)) {
		memmove(&obj->pnt,&old_pnt,sizeof(d3pnt));
		*rval=script_bool_to_value(FALSE);
		return(JS_TRUE);
	}

		// move object
		
	object_set_position(obj,spot->pnt.x,spot->pnt.y,spot->pnt.z,spot->ang.y,0);
    *rval=script_bool_to_value(TRUE);
  
	return(JS_TRUE);
}

JSBool js_obj_position_place_network_spot_no_telefrag_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	d3pnt			old_pnt;
	obj_type		*obj;
	spot_type		*spot;
	
	obj=object_find_uid(js.attach.thing_uid);

		// get spot
		
	spot=script_find_network_spot(obj);
	if (spot==NULL) return(JS_FALSE);
	
		// can we move without telefragging?
	
	memmove(&old_pnt,&obj->pnt,sizeof(d3pnt));
	memmove(&obj->pnt,&spot->pnt,sizeof(d3pnt));

	if (object_telefrag_players(obj,TRUE)) {
		memmove(&obj->pnt,&old_pnt,sizeof(d3pnt));
		*rval=script_bool_to_value(FALSE);
		return(JS_TRUE);
	}

		// move object
	
	object_set_position(obj,spot->pnt.x,spot->pnt.y,spot->pnt.z,spot->ang.y,0);
    *rval=script_bool_to_value(TRUE);

	return(JS_TRUE);
}

JSBool js_obj_position_move_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				xadd,zadd,yadd;
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	xadd=script_value_to_int(argv[0]);
	zadd=script_value_to_int(argv[1]);
	yadd=script_value_to_int(argv[2]);
	
	object_set_position(obj,(obj->pnt.x+xadd),(obj->pnt.y+yadd),(obj->pnt.z+zadd),obj->ang.y,0);
 	object_telefrag_players(obj,FALSE);

	*rval=script_bool_to_value(TRUE);

	return(JS_TRUE);
}

JSBool js_obj_position_reset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_reset(obj);
	object_telefrag_players(obj,FALSE);
	
	return(JS_TRUE);
}

/* =======================================================

      Object Distance Functions
      
======================================================= */

JSBool js_obj_position_distance_to_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj,*player_obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	player_obj=object_find_uid(server.player_obj_uid);

	*rval=script_int_to_value(distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,player_obj->pnt.x,player_obj->pnt.y,player_obj->pnt.z));
	
	return(JS_TRUE);
}

JSBool js_obj_position_distance_to_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj,*dist_obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	dist_obj=object_find_uid(script_value_to_int(argv[0]));
	if (dist_obj==NULL) return(JS_FALSE);

	*rval=script_int_to_value(distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,dist_obj->pnt.x,dist_obj->pnt.y,dist_obj->pnt.z));
	
	return(JS_TRUE);
}

