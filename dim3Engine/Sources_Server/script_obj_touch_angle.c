/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.touchAngle object

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

JSBool js_obj_touch_angle_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_touch_angle_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
bool js_obj_touch_angle_get_x(jsval *vp);
bool js_obj_touch_angle_get_y(jsval *vp);
bool js_obj_touch_angle_get_z(jsval *vp);

script_js_property	obj_touch_angle_props[]={
							{"x",					js_obj_touch_angle_get_x,				NULL},
							{"y",					js_obj_touch_angle_get_y,				NULL},
							{"z",					js_obj_touch_angle_get_z,				NULL},
							{0}};

JSClassRef			obj_touch_angle_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_touch_angle_object(void)
{
	obj_touch_angle_class=script_create_class("obj_touch_angle_class",js_obj_touch_angle_get_property,js_obj_touch_angle_set_property);
}

void script_free_obj_touch_angle_object(void)
{
	script_free_class(obj_touch_angle_class);
}

JSObject* script_add_obj_touch_angle_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,obj_touch_angle_class,"touchAngle",obj_touch_angle_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_obj_touch_angle_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,obj_touch_angle_props));
}

JSBool js_obj_touch_angle_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,obj_touch_angle_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_obj_touch_angle_get_x(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->touch.ang.x);
	
	return(JS_TRUE);
}

bool js_obj_touch_angle_get_y(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->touch.ang.y);
	
	return(JS_TRUE);
}

bool js_obj_touch_angle_get_z(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->touch.ang.z);
	
	return(JS_TRUE);
}

