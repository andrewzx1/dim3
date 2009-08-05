/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.angle object

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

extern map_type			map;
extern js_type			js;

extern void object_setup_motion(obj_type *obj,float ang,float speed);

JSBool js_obj_angle_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_angle_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
bool js_obj_angle_get_x(jsval *vp);
bool js_obj_angle_get_y(jsval *vp);
bool js_obj_angle_get_z(jsval *vp);
JSBool js_obj_angle_rotate_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_angle_rotate_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_property	obj_angle_props[]={
							{"x",					js_obj_angle_get_x,					NULL},
							{"y",					js_obj_angle_get_y,					NULL},
							{"z",					js_obj_angle_get_z,					NULL},
							{0}};

script_js_function	obj_angle_functions[]={
							{"rotate",				js_obj_angle_rotate_func,			1},
							{"rotateTo",			js_obj_angle_rotate_to_func,		1},
							{0}};

JSClassRef			obj_angle_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_angle_object(void)
{
	obj_angle_class=script_create_class("obj_angle_class",js_obj_angle_get_property,js_obj_angle_set_property);
}

void script_free_obj_angle_object(void)
{
	script_free_class(obj_angle_class);
}

JSObject* script_add_obj_angle_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,obj_angle_class,"angle",obj_angle_props,obj_angle_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_obj_angle_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,obj_angle_props));
}

JSBool js_obj_angle_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,obj_angle_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_obj_angle_get_x(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->ang.x);
	
	return(JS_TRUE);
}

bool js_obj_angle_get_y(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->ang.y);
	
	return(JS_TRUE);
}

bool js_obj_angle_get_z(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->ang.z);
	
	return(JS_TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSBool js_obj_angle_rotate_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	float			ang_add;
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	ang_add=script_value_to_float(argv[0]);
	obj->ang.y=angle_add(obj->ang.y,ang_add);

	return(JS_TRUE);
}

JSBool js_obj_angle_rotate_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->ang.y=script_value_to_float(argv[0]);

	return(JS_TRUE);
}






