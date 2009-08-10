/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.hitAngle object

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

extern server_type		server;
extern js_type			js;

JSValueRef js_obj_hit_angle_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_hit_angle_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_hit_angle_get_x(void);
JSValueRef js_obj_hit_angle_get_y(void);
JSValueRef js_obj_hit_angle_get_z(void);

script_js_property	obj_hit_angle_props[]={
							{"x",					js_obj_hit_angle_get_x,				NULL},
							{"y",					js_obj_hit_angle_get_y,				NULL},
							{"z",					js_obj_hit_angle_get_z,				NULL},
							{0}};

JSClassRef			obj_hit_angle_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_hit_angle_object(void)
{
	obj_hit_angle_class=script_create_class("obj_hit_angle_class",js_obj_hit_angle_get_property,js_obj_hit_angle_set_property);
}

void script_free_obj_hit_angle_object(void)
{
	script_free_class(obj_hit_angle_class);
}

JSObjectRef script_add_obj_hit_angle_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_hit_angle_class,"hitAngle",obj_hit_angle_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_hit_angle_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_hit_angle_props));
}

bool js_obj_hit_angle_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,obj_hit_angle_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_hit_angle_get_x(void)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->hit.ang.x);
	
	return(TRUE);
}

JSValueRef js_obj_hit_angle_get_y(void)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->hit.ang.y);
	
	return(TRUE);
}

JSValueRef js_obj_hit_angle_get_z(void)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->hit.ang.z);
	
	return(TRUE);
}

