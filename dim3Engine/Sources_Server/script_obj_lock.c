/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.lock object

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

extern map_type			map;
extern server_type		server;
extern js_type			js;

JSValueRef js_obj_lock_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_lock_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_lock_get_x(void);
JSValueRef js_obj_lock_get_y(void);
JSValueRef js_obj_lock_get_z(void);
void js_obj_lock_set_x(JSValueRef vp,JSValueRef *exception);
void js_obj_lock_set_y(JSValueRef vp,JSValueRef *exception);
void js_obj_lock_set_z(JSValueRef vp,JSValueRef *exception);

script_js_property	obj_lock_props[]={
							{"x",					js_obj_lock_get_x,					js_obj_lock_set_x},
							{"y",					js_obj_lock_get_y,					js_obj_lock_set_y},
							{"z",					js_obj_lock_get_z,					js_obj_lock_set_z},
							{0}};
							
JSClassRef			obj_lock_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_lock_object(void)
{
	obj_lock_class=script_create_class("obj_lock_class",js_obj_lock_get_property,js_obj_lock_set_property);
}

void script_free_obj_lock_object(void)
{
	script_free_class(obj_lock_class);
}

JSObjectRef script_add_obj_lock_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_lock_class,"lock",obj_lock_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_lock_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_lock_props));
}

bool js_obj_lock_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,obj_lock_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_lock_get_x(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->lock.x);
	
	return(TRUE);
}

JSValueRef js_obj_lock_get_y(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->lock.y);
	
	return(TRUE);
}

JSValueRef js_obj_lock_get_z(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->lock.z);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

void js_obj_lock_set_x(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->lock.x=script_value_to_bool(*vp);
}

void js_obj_lock_set_y(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->lock.y=script_value_to_bool(*vp);
}

void js_obj_lock_set_z(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->lock.z=script_value_to_bool(*vp);
}

