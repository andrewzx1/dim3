/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.thrust object

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

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"
#include "objects.h"

extern js_type			js;

JSBool js_obj_thrust_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_thrust_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_thrust_get_speed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_thrust_get_maxSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_thrust_get_drag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_thrust_set_speed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_thrust_set_maxSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_thrust_set_drag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

script_js_property	obj_thrust_props[]={
							{"speed",					js_obj_thrust_get_speed,			js_obj_thrust_set_speed},
							{"maxSpeed",				js_obj_thrust_get_maxSpeed,			js_obj_thrust_set_maxSpeed},
							{"drag",					js_obj_thrust_get_drag,				js_obj_thrust_set_drag},
							{0}};

JSClass				*obj_thrust_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_thrust_object(void)
{
	obj_thrust_class=script_create_class("obj_thrust_class",js_obj_thrust_get_property,js_obj_thrust_set_property);
}

void script_free_obj_thrust_object(void)
{
	script_free_class(obj_thrust_class);
}

JSObject* script_add_obj_thrust_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,obj_thrust_class,"thrust",obj_thrust_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_obj_thrust_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,obj_thrust_props));
}

JSBool js_obj_thrust_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,obj_thrust_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_obj_thrust_get_speed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->thrust.speed);
	
	return(JS_TRUE);
}

JSBool js_obj_thrust_get_maxSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	
	return(JS_TRUE);
}

JSBool js_obj_thrust_get_drag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->thrust.drag);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_obj_thrust_set_speed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->thrust.speed=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_thrust_set_maxSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->thrust.max_speed=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_thrust_set_drag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->thrust.drag=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}


