/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.vertSpeed object

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

extern js_type			js;

JSBool js_obj_vert_speed_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_vert_speed_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
bool js_obj_vert_speed_get_normal(jsval *vp);
bool js_obj_vert_speed_get_acceleration(jsval *vp);
bool js_obj_vert_speed_get_deceleration(jsval *vp);
bool js_obj_vert_speed_get_flySlop(jsval *vp);
bool js_obj_vert_speed_set_normal(jsval *vp);
bool js_obj_vert_speed_set_acceleration(jsval *vp);
bool js_obj_vert_speed_set_deceleration(jsval *vp);
bool js_obj_vert_speed_set_flySlop(jsval *vp);

script_js_property	obj_vert_speed_props[]={
							{"normal",					js_obj_vert_speed_get_normal,			js_obj_vert_speed_set_normal},
							{"acceleration",			js_obj_vert_speed_get_acceleration,		js_obj_vert_speed_set_acceleration},
							{"deceleration",			js_obj_vert_speed_get_deceleration,		js_obj_vert_speed_set_deceleration},
							{"flySlop",					js_obj_vert_speed_get_flySlop,			js_obj_vert_speed_set_flySlop},
							{0}};

JSClassRef			obj_vert_speed_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_vert_speed_object(void)
{
	obj_vert_speed_class=script_create_class("obj_vert_speed_class",js_obj_vert_speed_get_property,js_obj_vert_speed_set_property);
}

void script_free_obj_vert_speed_object(void)
{
	script_free_class(obj_vert_speed_class);
}

JSObject* script_add_obj_vert_speed_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,obj_vert_speed_class,"verticalSpeed",obj_vert_speed_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_obj_vert_speed_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,obj_vert_speed_props));
}

JSBool js_obj_vert_speed_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,obj_vert_speed_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_obj_vert_speed_get_normal(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->vert_move.max_walk_speed);
	
	return(JS_TRUE);
}

bool js_obj_vert_speed_get_acceleration(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->vert_move.accelerate);
	
	return(JS_TRUE);
}

bool js_obj_vert_speed_get_deceleration(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->vert_move.decelerate);
	
	return(JS_TRUE);
}

bool js_obj_vert_speed_get_flySlop(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->vert_move.slop);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_vert_speed_set_normal(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->vert_move.max_walk_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_vert_speed_set_acceleration(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->vert_move.accelerate=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_vert_speed_set_deceleration(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->vert_move.decelerate=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_vert_speed_set_flySlop(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->vert_move.slop=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}


