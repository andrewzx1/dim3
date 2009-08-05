/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.fire object

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
#include "weapons.h"

extern js_type			js;

JSBool js_weap_fire_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_fire_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
bool js_weap_fire_get_method(jsval *vp);
bool js_weap_fire_get_lastFireTick(jsval *vp);
JSBool js_weap_fire_past_last_fire_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_fire_reset_last_fire_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_fire_cancel_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_property	weap_fire_props[]={
							{"method",				js_weap_fire_get_method,				NULL},
							{"lastFireTick",		js_weap_fire_get_lastFireTick,			NULL},
							{0}};
							
script_js_function	weap_fire_functions[]={
							{"pastLastFire",		js_weap_fire_past_last_fire_func,		1},
							{"resetLastFire",		js_weap_fire_reset_last_fire_func,		0},
							{"cancel",				js_weap_fire_cancel_func,				0},
							{0}};

JSClassRef			weap_fire_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_fire_object(void)
{
	weap_fire_class=script_create_class("weap_fire_class",js_weap_fire_get_property,js_weap_fire_set_property);
}

void script_free_weap_fire_object(void)
{
	script_free_class(weap_fire_class);
}

JSObject* script_add_weap_fire_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,weap_fire_class,"fire",weap_fire_props,weap_fire_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_weap_fire_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,weap_fire_props));
}

JSBool js_weap_fire_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,weap_fire_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_weap_fire_get_method(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->fire.method);
	
	return(JS_TRUE);
}

bool js_weap_fire_get_lastFireTick(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	if (!weap->dual.in_dual) {
		*vp=script_int_to_value(weap->fire.last_fire_tick);
	}
	else {
		*vp=script_int_to_value(weap->fire.last_fire_dual_tick);
	}
	
	return(JS_TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSBool js_weap_fire_past_last_fire_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				last_fire_tick;
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	
	if (!weap->dual.in_dual) {
		last_fire_tick=weap->fire.last_fire_tick;
	}
	else {
		last_fire_tick=weap->fire.last_fire_dual_tick;
	}

	*rval=script_bool_to_value(js.time.current_tick>(last_fire_tick+script_value_to_int(argv[0])));
    
	return(JS_TRUE);
}

JSBool js_weap_fire_reset_last_fire_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	
	if (!weap->dual.in_dual) {
		weap->fire.last_fire_tick=js.time.current_tick;
	}
	else {
		weap->fire.last_fire_dual_tick=js.time.current_tick;
	}
    
	return(JS_TRUE);
}

JSBool js_weap_fire_cancel_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	weap->fire.cancel=TRUE;
    
	return(JS_TRUE);
}

