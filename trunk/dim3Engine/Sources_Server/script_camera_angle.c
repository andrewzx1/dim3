/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.angle object

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
#include "cameras.h"

extern camera_type		camera;
extern js_type			js;

JSValueRef js_camera_angle_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_camera_angle_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_camera_angle_get_x(JSContextRef cx);
JSValueRef js_camera_angle_get_y(JSContextRef cx);
JSValueRef js_camera_angle_get_z(JSContextRef cx);
void js_camera_angle_set_x(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_camera_angle_set_y(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_camera_angle_set_z(JSContextRef cx,JSValueRef vp,JSValueRef *exception);

script_js_property	camera_angle_props[]={
							{"x",					js_camera_angle_get_x,		js_camera_angle_set_x},
							{"y",					js_camera_angle_get_y,		js_camera_angle_set_y},
							{"z",					js_camera_angle_get_z,		js_camera_angle_set_z},
							{0}};

JSClassRef			camera_angle_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_camera_angle_object(void)
{
	camera_angle_class=script_create_class("camera_angle_class",js_camera_angle_get_property,js_camera_angle_set_property);
}

void script_free_camera_angle_object(void)
{
	script_free_class(camera_angle_class);
}

JSObjectRef script_add_camera_angle_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,camera_angle_class,"angle",camera_angle_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_camera_angle_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,camera_angle_props));
}

bool js_camera_angle_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,camera_angle_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_camera_angle_get_x(JSContextRef cx)
{
	return(script_float_to_value(cx,camera.ang.x));
}

JSValueRef js_camera_angle_get_y(JSContextRef cx)
{
	return(script_float_to_value(cx,camera.ang.y));
}

JSValueRef js_camera_angle_get_z(JSContextRef cx)
{
	return(script_float_to_value(cx,camera.ang.z));
}

/* =======================================================

      Setters
      
======================================================= */

void js_camera_angle_set_x(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	camera.ang.x=script_value_to_float(cx,vp);
}

void js_camera_angle_set_y(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	camera.ang.y=script_value_to_float(cx,vp);
}

void js_camera_angle_set_z(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	camera.ang.z=script_value_to_float(cx,vp);
}

