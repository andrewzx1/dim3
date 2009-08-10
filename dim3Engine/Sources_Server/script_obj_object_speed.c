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
JSValueRef js_obj_object_speed_get_jumpHeight(void);
JSValueRef js_obj_object_speed_get_duckAdd(void);
JSValueRef js_obj_object_speed_get_duckChange(void);
JSValueRef js_obj_object_speed_get_bumpHeight(void);
JSValueRef js_obj_object_speed_get_bumpSmoothFactor(void);
JSValueRef js_obj_object_speed_get_bounceFactor(void);
JSValueRef js_obj_object_speed_get_noSlide(void);
bool js_obj_object_speed_set_jumpHeight(JSValueRef vp);
bool js_obj_object_speed_set_duckAdd(JSValueRef vp);
bool js_obj_object_speed_set_duckChange(JSValueRef vp);
bool js_obj_object_speed_set_bumpHeight(JSValueRef vp);
bool js_obj_object_speed_set_bumpSmoothFactor(JSValueRef vp);
bool js_obj_object_speed_set_bounceFactor(JSValueRef vp);
bool js_obj_object_speed_set_noSlide(JSValueRef vp);

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
	return(script_set_property(cx,j_obj,name,vp,obj_object_speed_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_object_speed_get_jumpHeight(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->jump.y_add);
	
	return(TRUE);
}

JSValueRef js_obj_object_speed_get_duckAdd(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->duck.y_add);
	
	return(TRUE);
}

JSValueRef js_obj_object_speed_get_duckChange(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->duck.y_change);
	
	return(TRUE);
}

JSValueRef js_obj_object_speed_get_bumpHeight(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->bump.high);
	
	return(TRUE);
}

JSValueRef js_obj_object_speed_get_bumpSmoothFactor(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->bump.smooth_factor);
	
	return(TRUE);
}

JSValueRef js_obj_object_speed_get_bounceFactor(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->bounce.factor);
	
	return(TRUE);
}

JSValueRef js_obj_object_speed_get_noSlide(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->no_slide);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_object_speed_set_jumpHeight(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->jump.y_add=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_obj_object_speed_set_duckAdd(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->duck.y_add=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_obj_object_speed_set_duckChange(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->duck.y_change=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_obj_object_speed_set_bumpHeight(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->bump.high=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_obj_object_speed_set_bumpSmoothFactor(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->bump.smooth_factor=script_value_to_float(*vp);
	
	return(TRUE);
}

bool js_obj_object_speed_set_bounceFactor(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->bounce.factor=script_value_to_float(*vp);
	
	return(TRUE);
}

bool js_obj_object_speed_set_noSlide(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->no_slide=script_value_to_bool(*vp);
	
	return(TRUE);
}


