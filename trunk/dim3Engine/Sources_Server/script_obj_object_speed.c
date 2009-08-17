/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.objectSpeed object

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

JSValueRef js_obj_object_speed_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_object_speed_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_object_speed_get_jumpHeight(JSContextRef cx);
JSValueRef js_obj_object_speed_get_duckAdd(JSContextRef cx);
JSValueRef js_obj_object_speed_get_duckChange(JSContextRef cx);
JSValueRef js_obj_object_speed_get_bumpHeight(JSContextRef cx);
JSValueRef js_obj_object_speed_get_bumpSmoothFactor(JSContextRef cx);
JSValueRef js_obj_object_speed_get_bounceFactor(JSContextRef cx);
JSValueRef js_obj_object_speed_get_noSlide(JSContextRef cx);
void js_obj_object_speed_set_jumpHeight(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_object_speed_set_duckAdd(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_object_speed_set_duckChange(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_object_speed_set_bumpHeight(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_object_speed_set_bumpSmoothFactor(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_object_speed_set_bounceFactor(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_object_speed_set_noSlide(JSContextRef cx,JSValueRef vp,JSValueRef *exception);

script_js_property	obj_object_speed_props[]={
							{"jumpHeight",				js_obj_object_speed_get_jumpHeight,			js_obj_object_speed_set_jumpHeight},
							{"duckAdd",					js_obj_object_speed_get_duckAdd,			js_obj_object_speed_set_duckAdd},
							{"duckChange",				js_obj_object_speed_get_duckChange,			js_obj_object_speed_set_duckChange},
							{"bumpHeight",				js_obj_object_speed_get_bumpHeight,			js_obj_object_speed_set_bumpHeight},
							{"bumpSmoothFactor",		js_obj_object_speed_get_bumpSmoothFactor,	js_obj_object_speed_set_bumpSmoothFactor},
							{"bounceFactor",			js_obj_object_speed_get_bounceFactor,		js_obj_object_speed_set_bounceFactor},
							{"noSlide",					js_obj_object_speed_get_noSlide,			js_obj_object_speed_set_noSlide},
							{0}};

JSClassRef			obj_object_speed_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_object_speed_object(void)
{
	obj_object_speed_class=script_create_class("obj_object_speed_class",js_obj_object_speed_get_property,js_obj_object_speed_set_property);
}

void script_free_obj_object_speed_object(void)
{
	script_free_class(obj_object_speed_class);
}

JSObjectRef script_add_obj_object_speed_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_object_speed_class,"objectSpeed",obj_object_speed_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_object_speed_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_object_speed_props));
}

bool js_obj_object_speed_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,obj_object_speed_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_object_speed_get_jumpHeight(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->jump.y_add));
}

JSValueRef js_obj_object_speed_get_duckAdd(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->duck.y_add));
}

JSValueRef js_obj_object_speed_get_duckChange(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->duck.y_change));
}

JSValueRef js_obj_object_speed_get_bumpHeight(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->bump.high));
}

JSValueRef js_obj_object_speed_get_bumpSmoothFactor(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,obj->bump.smooth_factor));
}

JSValueRef js_obj_object_speed_get_bounceFactor(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,obj->bounce.factor));
}

JSValueRef js_obj_object_speed_get_noSlide(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->no_slide));
}

/* =======================================================

      Setters
      
======================================================= */

void js_obj_object_speed_set_jumpHeight(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->jump.y_add=script_value_to_int(cx,vp);
}

void js_obj_object_speed_set_duckAdd(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->duck.y_add=script_value_to_int(cx,vp);
}

void js_obj_object_speed_set_duckChange(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->duck.y_change=script_value_to_int(cx,vp);
}

void js_obj_object_speed_set_bumpHeight(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->bump.high=script_value_to_int(cx,vp);
}

void js_obj_object_speed_set_bumpSmoothFactor(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->bump.smooth_factor=script_value_to_float(cx,vp);
}

void js_obj_object_speed_set_bounceFactor(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->bounce.factor=script_value_to_float(cx,vp);
}

void js_obj_object_speed_set_noSlide(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->no_slide=script_value_to_bool(cx,vp);
}


