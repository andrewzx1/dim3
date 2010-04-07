/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.fire object

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
#include "weapons.h"
#include "timing.h"

extern js_type			js;

JSValueRef js_weap_fire_get_method(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_fire_get_lastFireTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_fire_past_last_fire_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_fire_reset_last_fire_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_fire_cancel_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		weap_fire_props[]={
							{"method",				js_weap_fire_get_method,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"lastFireTick",		js_weap_fire_get_lastFireTick,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	weap_fire_functions[]={
							{"pastLastFire",		js_weap_fire_past_last_fire_func,		kJSPropertyAttributeDontDelete},
							{"resetLastFire",		js_weap_fire_reset_last_fire_func,		kJSPropertyAttributeDontDelete},
							{"cancel",				js_weap_fire_cancel_func,				kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			weap_fire_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_fire_object(void)
{
	weap_fire_class=script_create_class("weap_fire_class",weap_fire_props,weap_fire_functions);
}

void script_free_weap_fire_object(void)
{
	script_free_class(weap_fire_class);
}

JSObjectRef script_add_weap_fire_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,weap_fire_class,"fire"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_fire_get_method(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,weap->fire.method));
}

JSValueRef js_weap_fire_get_lastFireTick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	if (!weap->dual.in_dual) return(script_int_to_value(cx,weap->fire.last_fire_tick));

	return(script_int_to_value(cx,weap->fire.last_fire_dual_tick));
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_weap_fire_past_last_fire_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				last_fire_tick;
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	weap=weapon_find_uid(js.attach.thing_uid);
	
	if (!weap->dual.in_dual) {
		last_fire_tick=weap->fire.last_fire_tick;
	}
	else {
		last_fire_tick=weap->fire.last_fire_dual_tick;
	}

	return(script_bool_to_value(cx,(game_time_get()>(last_fire_tick+script_value_to_int(cx,argv[0])))));
}

JSValueRef js_weap_fire_reset_last_fire_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	weap=weapon_find_uid(js.attach.thing_uid);
	
	if (!weap->dual.in_dual) {
		weap->fire.last_fire_tick=game_time_get();
	}
	else {
		weap->fire.last_fire_dual_tick=game_time_get();
	}
    
	return(script_null_to_value(cx));
}

JSValueRef js_weap_fire_cancel_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->fire.cancel=TRUE;
    
	return(script_null_to_value(cx));
}
