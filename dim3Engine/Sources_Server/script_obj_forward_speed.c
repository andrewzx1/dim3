/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.forwardSpeed object

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

JSBool js_obj_forward_speed_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_forward_speed_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
bool js_obj_forward_speed_get_walk(jsval *vp);
bool js_obj_forward_speed_get_run(jsval *vp);
bool js_obj_forward_speed_get_crawl(jsval *vp);
bool js_obj_forward_speed_get_air(jsval *vp);
bool js_obj_forward_speed_get_acceleration(jsval *vp);
bool js_obj_forward_speed_get_deceleration(jsval *vp);
bool js_obj_forward_speed_get_accelerationAir(jsval *vp);
bool js_obj_forward_speed_get_decelerationAir(jsval *vp);
bool js_obj_forward_speed_set_walk(jsval *vp);
bool js_obj_forward_speed_set_run(jsval *vp);
bool js_obj_forward_speed_set_crawl(jsval *vp);
bool js_obj_forward_speed_set_air(jsval *vp);
bool js_obj_forward_speed_set_acceleration(jsval *vp);
bool js_obj_forward_speed_set_deceleration(jsval *vp);
bool js_obj_forward_speed_set_accelerationAir(jsval *vp);
bool js_obj_forward_speed_set_decelerationAir(jsval *vp);

script_js_property	obj_forward_speed_props[]={
							{"walk",					js_obj_forward_speed_get_walk,				js_obj_forward_speed_set_walk},
							{"run",						js_obj_forward_speed_get_run,				js_obj_forward_speed_set_run},
							{"crawl",					js_obj_forward_speed_get_crawl,				js_obj_forward_speed_set_crawl},
							{"air",						js_obj_forward_speed_get_air,				js_obj_forward_speed_set_air},
							{"acceleration",			js_obj_forward_speed_get_acceleration,		js_obj_forward_speed_set_acceleration},
							{"deceleration",			js_obj_forward_speed_get_deceleration,		js_obj_forward_speed_set_deceleration},
							{"accelerationAir",			js_obj_forward_speed_get_accelerationAir,	js_obj_forward_speed_set_accelerationAir},
							{"decelerationAir",			js_obj_forward_speed_get_decelerationAir,	js_obj_forward_speed_set_decelerationAir},
							{0}};

JSClassRef			obj_forward_speed_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_forward_speed_object(void)
{
	obj_forward_speed_class=script_create_class("obj_forward_speed_class",js_obj_forward_speed_get_property,js_obj_forward_speed_set_property);
}

void script_free_obj_forward_speed_object(void)
{
	script_free_class(obj_forward_speed_class);
}

JSObject* script_add_obj_forward_speed_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,obj_forward_speed_class,"forwardSpeed",obj_forward_speed_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_obj_forward_speed_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,obj_forward_speed_props));
}

JSBool js_obj_forward_speed_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,obj_forward_speed_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_obj_forward_speed_get_walk(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->forward_move.max_walk_speed);
	
	return(JS_TRUE);
}

bool js_obj_forward_speed_get_run(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->forward_move.max_run_speed);
	
	return(JS_TRUE);
}

bool js_obj_forward_speed_get_crawl(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->forward_move.max_crawl_speed);
	
	return(JS_TRUE);
}

bool js_obj_forward_speed_get_air(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->forward_move.max_air_speed);
	
	return(JS_TRUE);
}

bool js_obj_forward_speed_get_acceleration(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->forward_move.accelerate);
	
	return(JS_TRUE);
}

bool js_obj_forward_speed_get_deceleration(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->forward_move.decelerate);
	
	return(JS_TRUE);
}

bool js_obj_forward_speed_get_accelerationAir(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->forward_move.air_accelerate);
	
	return(JS_TRUE);
}

bool js_obj_forward_speed_get_decelerationAir(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->forward_move.air_decelerate);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_forward_speed_set_walk(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->forward_move.max_walk_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_forward_speed_set_run(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	obj->forward_move.max_run_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_forward_speed_set_crawl(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->forward_move.max_crawl_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_forward_speed_set_air(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->forward_move.max_air_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_forward_speed_set_acceleration(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->forward_move.accelerate=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_forward_speed_set_deceleration(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->forward_move.decelerate=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_forward_speed_set_accelerationAir(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->forward_move.air_accelerate=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_forward_speed_set_decelerationAir(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->forward_move.air_decelerate=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

