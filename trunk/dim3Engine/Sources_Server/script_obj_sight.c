/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.sight object

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

extern server_type		server;
extern js_type			js;

JSBool js_obj_sight_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_sight_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_sight_get_sideFieldAngle(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_sight_get_lookFieldAngle(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_sight_get_sideFieldDivision(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_sight_get_lookFieldDivision(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_sight_get_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_sight_set_sideFieldAngle(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_sight_set_lookFieldAngle(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_sight_set_sideFieldDivision(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_sight_set_lookFieldDivision(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_sight_set_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_sight_test_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_sight_test_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_property	obj_sight_props[]={
							{"sideFieldAngle",		js_obj_sight_get_sideFieldAngle,	js_obj_sight_set_sideFieldAngle},
							{"lookFieldAngle",		js_obj_sight_get_lookFieldAngle,	js_obj_sight_set_lookFieldAngle},
							{"sideFieldDivision",	js_obj_sight_get_sideFieldDivision,	js_obj_sight_set_sideFieldDivision},
							{"lookFieldDivision",	js_obj_sight_get_lookFieldDivision,	js_obj_sight_set_lookFieldDivision},
							{"distance",			js_obj_sight_get_distance,			js_obj_sight_set_distance},
							{0}};

script_js_function	obj_sight_functions[]={
							{"testObject",			js_obj_sight_test_object_func,		1},
							{"testPlayer",			js_obj_sight_test_player_func,		0},
							{0}};

JSClass				*obj_sight_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_sight_object(void)
{
	obj_sight_class=script_create_class("obj_sight_class",js_obj_sight_get_property,js_obj_sight_set_property);
}

void script_free_obj_sight_object(void)
{
	script_free_class(obj_sight_class);
}

JSObject* script_add_obj_sight_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,obj_sight_class,"sight",obj_sight_props,obj_sight_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_obj_sight_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,obj_sight_props));
}

JSBool js_obj_sight_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,obj_sight_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_obj_sight_get_sideFieldAngle(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->sight.side_angle);
	
	return(JS_TRUE);
}

JSBool js_obj_sight_get_lookFieldAngle(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->sight.look_angle);
	
	return(JS_TRUE);
}

JSBool js_obj_sight_get_sideFieldDivision(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->sight.side_division);
	
	return(JS_TRUE);
}

JSBool js_obj_sight_get_lookFieldDivision(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->sight.look_division);
	
	return(JS_TRUE);
}

JSBool js_obj_sight_get_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->sight.distance);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_obj_sight_set_sideFieldAngle(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->sight.side_angle=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_sight_set_lookFieldAngle(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->sight.look_angle=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_sight_set_sideFieldDivision(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->sight.side_division=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_sight_set_lookFieldDivision(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->sight.look_division=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_sight_set_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->sight.distance=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSBool js_obj_sight_test_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				id;
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	id=script_value_to_int(argv[0]);
	*rval=script_bool_to_value(object_sight_test_object(obj,id));
	
	return(JS_TRUE);
}

JSBool js_obj_sight_test_player_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	*rval=script_bool_to_value(object_sight_test_object(obj,server.player_obj_uid));

	return(JS_TRUE);
}
