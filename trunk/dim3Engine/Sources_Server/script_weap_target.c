/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.target object

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
#include "weapons.h"

extern js_type			js;

JSBool js_weap_target_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_target_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_target_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_target_get_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_target_get_objectId(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_target_set_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_target_start_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_target_start_opponent_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_target_end_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_property	weap_target_props[]={
							{"on",					js_weap_target_get_on,					NULL},
							{"distance",			js_weap_target_get_distance,			js_weap_target_set_distance},
							{"objectId",			js_weap_target_get_objectId,			NULL},
							{0}};
							
script_js_function	weap_target_functions[]={
							{"start",				js_weap_target_start_func,				1},
							{"startOpponent",		js_weap_target_start_opponent_func,		0},
							{"end",					js_weap_target_end_func,				0},
							{0}};

JSClassRef			weap_target_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_target_object(void)
{
	weap_target_class=script_create_class("weap_target_class",js_weap_target_get_property,js_weap_target_set_property);
}

void script_free_weap_target_object(void)
{
	script_free_class(weap_target_class);
}

JSObject* script_add_weap_target_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,weap_target_class,"target",weap_target_props,weap_target_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_weap_target_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,weap_target_props));
}

JSBool js_weap_target_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,weap_target_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_weap_target_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(weap->target.on);
	
	return(JS_TRUE);
}

bool js_weap_target_get_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->target.distance);
	
	return(JS_TRUE);
}

bool js_weap_target_get_objectId(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->target.obj_uid);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_target_set_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->target.distance=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

/* =======================================================

      Target Functions
      
======================================================= */

JSBool js_weap_target_start_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char				target_type[name_str_len];
	obj_type			*obj;
	weapon_type			*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],target_type,name_str_len);

	*rval=script_bool_to_value(weapon_target_start(obj,weap,target_type));

	return(JS_TRUE);
}

JSBool js_weap_target_start_opponent_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type			*obj;
	weapon_type			*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);
	
	*rval=script_bool_to_value(weapon_target_start(obj,weap,NULL));

	return(JS_TRUE);
}

JSBool js_weap_target_end_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type			*obj;
	weapon_type			*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);
	
	*rval=script_bool_to_value(weapon_target_end(obj,weap));

	return(JS_TRUE);
}

