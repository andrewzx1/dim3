/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.health object

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

JSValueRef js_obj_health_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_health_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_health_get_maximum(JSContextRef cx);
JSValueRef js_obj_health_get_start(JSContextRef cx);
JSValueRef js_obj_health_get_current(JSContextRef cx);
JSValueRef js_obj_health_get_recoverTick(JSContextRef cx);
JSValueRef js_obj_health_get_recoverAmount(JSContextRef cx);
JSValueRef js_obj_health_get_fallDamageMinimumHeight(JSContextRef cx);
JSValueRef js_obj_health_get_fallDamageFactor(JSContextRef cx);
JSValueRef js_obj_health_get_factor(JSContextRef cx);
void js_obj_health_set_maximum(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_health_set_start(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_health_set_recoverTick(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_health_set_recoverAmount(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_health_set_fallDamageMinimumHeight(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_health_set_fallDamageFactor(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_health_set_factor(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_health_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_health_remove_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_health_reset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_property	obj_health_props[]={
							{"maximum",					js_obj_health_get_maximum,					js_obj_health_set_maximum},
							{"start",					js_obj_health_get_start,					js_obj_health_set_start},
							{"current",					js_obj_health_get_current,					NULL},
							{"recoverTick",				js_obj_health_get_recoverTick,				js_obj_health_set_recoverTick},
							{"recoverAmount",			js_obj_health_get_recoverAmount,			js_obj_health_set_recoverAmount},
							{"fallDamageMinimumHeight",	js_obj_health_get_fallDamageMinimumHeight,	js_obj_health_set_fallDamageMinimumHeight},
							{"fallDamageFactor",		js_obj_health_get_fallDamageFactor,			js_obj_health_set_fallDamageFactor},
							{"factor",					js_obj_health_get_factor,					js_obj_health_set_factor},
							{0}};
							
script_js_function	obj_health_functions[]={
							{"add",						js_obj_health_add_func,						1},
							{"remove",					js_obj_health_remove_func,					1},
							{"reset",					js_obj_health_reset_func,					0},
							{0}};

JSClassRef			obj_health_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_health_object(void)
{
	obj_health_class=script_create_class("obj_health_class",js_obj_health_get_property,js_obj_health_set_property);
}

void script_free_obj_health_object(void)
{
	script_free_class(obj_health_class);
}

JSObjectRef script_add_obj_health_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_health_class,"health",obj_health_props,obj_health_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_health_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_health_props));
}

bool js_obj_health_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,obj_health_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_health_get_maximum(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->status.max_health));
}

JSValueRef js_obj_health_get_start(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->status.start_health));
}

JSValueRef js_obj_health_get_current(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->status.health));
}

JSValueRef js_obj_health_get_recoverTick(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->status.health_recover_tick));
}

JSValueRef js_obj_health_get_recoverAmount(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->status.health_recover_amount));
}

JSValueRef js_obj_health_get_fallDamageMinimumHeight(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->fall.damage_minimum_height));
}

JSValueRef js_obj_health_get_fallDamageFactor(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,obj->fall.damage_factor));
}

JSValueRef js_obj_health_get_factor(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,obj->status.health_factor));
}

/* =======================================================

      Setters
      
======================================================= */

void js_obj_health_set_maximum(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->status.max_health=script_value_to_int(cx,vp);
}

void js_obj_health_set_start(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->status.start_health=script_value_to_int(cx,vp);
}

void js_obj_health_set_recoverTick(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->status.health_recover_tick=script_value_to_int(cx,vp);
	obj->status.health_recover_count=0;			// restart recover count
}

void js_obj_health_set_recoverAmount(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->status.health_recover_amount=script_value_to_int(cx,vp);
}

void js_obj_health_set_fallDamageMinimumHeight(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->fall.damage_minimum_height=script_value_to_int(cx,vp);
}

void js_obj_health_set_fallDamageFactor(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->fall.damage_factor=script_value_to_float(cx,vp);
}

void js_obj_health_set_factor(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->status.health_factor=script_value_to_float(cx,vp);
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_obj_health_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	obj_status		*status;
	
	obj=object_find_uid(js.attach.thing_uid);
    status=&obj->status;
	
    status->health+=script_value_to_int(cx,argv[0]);
    if (status->health>status->max_health) status->health=status->max_health;

	return(script_null_to_value(cx));
}

JSValueRef js_obj_health_remove_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	obj_status		*status;
	
	obj=object_find_uid(js.attach.thing_uid);
    status=&obj->status;
	
	obj->damage_obj_uid=-1;			// self-inflicted damage
	
    status->health-=script_value_to_int(cx,argv[0]);
    if (status->health<0) {
		status->health=0;
		obj->death_trigger=TRUE;
	}

	return(script_null_to_value(cx));
}

JSValueRef js_obj_health_reset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	obj_status		*status;
	
	obj=object_find_uid(js.attach.thing_uid);
    status=&obj->status;
	
	status->health=status->max_health;

	return(script_null_to_value(cx));
}


