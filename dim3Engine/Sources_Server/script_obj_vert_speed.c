/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.vertSpeed object

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

JSValueRef js_obj_vert_speed_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_vert_speed_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_vert_speed_get_normal(void);
JSValueRef js_obj_vert_speed_get_acceleration(void);
JSValueRef js_obj_vert_speed_get_deceleration(void);
JSValueRef js_obj_vert_speed_get_flySlop(void);
bool js_obj_vert_speed_set_normal(JSValueRef vp);
bool js_obj_vert_speed_set_acceleration(JSValueRef vp);
bool js_obj_vert_speed_set_deceleration(JSValueRef vp);
bool js_obj_vert_speed_set_flySlop(JSValueRef vp);

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

JSObjectRef script_add_obj_vert_speed_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_vert_speed_class,"verticalSpeed",obj_vert_speed_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_vert_speed_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_vert_speed_props));
}

bool js_obj_vert_speed_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,obj_vert_speed_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_vert_speed_get_normal(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->vert_move.max_walk_speed);
	
	return(TRUE);
}

JSValueRef js_obj_vert_speed_get_acceleration(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->vert_move.accelerate);
	
	return(TRUE);
}

JSValueRef js_obj_vert_speed_get_deceleration(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->vert_move.decelerate);
	
	return(TRUE);
}

JSValueRef js_obj_vert_speed_get_flySlop(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->vert_move.slop);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_vert_speed_set_normal(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->vert_move.max_walk_speed=fabsf(script_value_to_float(*vp));
	
	return(TRUE);
}

bool js_obj_vert_speed_set_acceleration(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->vert_move.accelerate=fabsf(script_value_to_float(*vp));
	
	return(TRUE);
}

bool js_obj_vert_speed_set_deceleration(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->vert_move.decelerate=fabsf(script_value_to_float(*vp));
	
	return(TRUE);
}

bool js_obj_vert_speed_set_flySlop(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->vert_move.slop=fabsf(script_value_to_float(*vp));
	
	return(TRUE);
}


