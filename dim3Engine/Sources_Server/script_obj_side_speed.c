/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.sideSpeed object

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"
#include "objects.h"

extern js_type			js;

JSBool js_obj_side_speed_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_obj_side_speed_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
bool js_obj_side_speed_get_walk(JSValueRef *vp);
bool js_obj_side_speed_get_run(JSValueRef *vp);
bool js_obj_side_speed_get_crawl(JSValueRef *vp);
bool js_obj_side_speed_get_air(JSValueRef *vp);
bool js_obj_side_speed_get_acceleration(JSValueRef *vp);
bool js_obj_side_speed_get_deceleration(JSValueRef *vp);
bool js_obj_side_speed_get_accelerationAir(JSValueRef *vp);
bool js_obj_side_speed_get_decelerationAir(JSValueRef *vp);
bool js_obj_side_speed_set_walk(JSValueRef *vp);
bool js_obj_side_speed_set_run(JSValueRef *vp);
bool js_obj_side_speed_set_crawl(JSValueRef *vp);
bool js_obj_side_speed_set_air(JSValueRef *vp);
bool js_obj_side_speed_set_acceleration(JSValueRef *vp);
bool js_obj_side_speed_set_deceleration(JSValueRef *vp);
bool js_obj_side_speed_set_accelerationAir(JSValueRef *vp);
bool js_obj_side_speed_set_decelerationAir(JSValueRef *vp);

script_js_property	obj_side_speed_props[]={
							{"walk",					js_obj_side_speed_get_walk,				js_obj_side_speed_set_walk},
							{"run",						js_obj_side_speed_get_run,				js_obj_side_speed_set_run},
							{"crawl",					js_obj_side_speed_get_crawl,			js_obj_side_speed_set_crawl},
							{"air",						js_obj_side_speed_get_air,				js_obj_side_speed_set_air},
							{"acceleration",			js_obj_side_speed_get_acceleration,		js_obj_side_speed_set_acceleration},
							{"deceleration",			js_obj_side_speed_get_deceleration,		js_obj_side_speed_set_deceleration},
							{"accelerationAir",			js_obj_side_speed_get_accelerationAir,	js_obj_side_speed_set_accelerationAir},
							{"decelerationAir",			js_obj_side_speed_get_decelerationAir,	js_obj_side_speed_set_decelerationAir},
							{0}};

JSClassRef			obj_side_speed_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_side_speed_object(void)
{
	obj_side_speed_class=script_create_class("obj_side_speed_class",js_obj_side_speed_get_property,js_obj_side_speed_set_property);
}

void script_free_obj_side_speed_object(void)
{
	script_free_class(obj_side_speed_class);
}

JSObject* script_add_obj_side_speed_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,obj_side_speed_class,"sideSpeed",obj_side_speed_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_obj_side_speed_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,obj_side_speed_props));
}

JSBool js_obj_side_speed_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,obj_side_speed_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_obj_side_speed_get_walk(JSValueRef *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->side_move.max_walk_speed);
	
	return(TRUE);
}

bool js_obj_side_speed_get_run(JSValueRef *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->side_move.max_run_speed);
	
	return(TRUE);
}

bool js_obj_side_speed_get_crawl(JSValueRef *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->side_move.max_crawl_speed);
	
	return(TRUE);
}

bool js_obj_side_speed_get_air(JSValueRef *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->side_move.max_air_speed);
	
	return(TRUE);
}

bool js_obj_side_speed_get_acceleration(JSValueRef *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->side_move.accelerate);
	
	return(TRUE);
}

bool js_obj_side_speed_get_deceleration(JSValueRef *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->side_move.decelerate);
	
	return(TRUE);
}

bool js_obj_side_speed_get_accelerationAir(JSValueRef *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->side_move.air_accelerate);
	
	return(TRUE);
}

bool js_obj_side_speed_get_decelerationAir(JSValueRef *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->side_move.air_decelerate);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_side_speed_set_walk(JSValueRef *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_move.max_walk_speed=fabsf(script_value_to_float(*vp));
	
	return(TRUE);
}

bool js_obj_side_speed_set_run(JSValueRef *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_move.max_run_speed=fabsf(script_value_to_float(*vp));
	
	return(TRUE);
}

bool js_obj_side_speed_set_crawl(JSValueRef *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_move.max_crawl_speed=fabsf(script_value_to_float(*vp));
	
	return(TRUE);
}

bool js_obj_side_speed_set_air(JSValueRef *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_move.max_air_speed=fabsf(script_value_to_float(*vp));
	
	return(TRUE);
}

bool js_obj_side_speed_set_acceleration(JSValueRef *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_move.accelerate=fabsf(script_value_to_float(*vp));
	
	return(TRUE);
}

bool js_obj_side_speed_set_deceleration(JSValueRef *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_move.decelerate=fabsf(script_value_to_float(*vp));
	
	return(TRUE);
}

bool js_obj_side_speed_set_accelerationAir(JSValueRef *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_move.air_accelerate=fabsf(script_value_to_float(*vp));
	
	return(TRUE);
}

bool js_obj_side_speed_set_decelerationAir(JSValueRef *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_move.air_decelerate=fabsf(script_value_to_float(*vp));
	
	return(TRUE);
}

