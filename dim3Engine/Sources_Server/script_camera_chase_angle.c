/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.chaseAngle object

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
#include "cameras.h"

extern camera_type		camera;
extern js_type			js;

JSValueRef js_camera_chase_angle_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_camera_chase_angle_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_camera_chase_angle_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_camera_chase_angle_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_camera_chase_angle_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_camera_chase_angle_set_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		camera_chase_angle_props[]={
							{"x",					js_camera_chase_angle_get_x,			js_camera_chase_angle_set_x,		kJSPropertyAttributeDontDelete},
							{"y",					js_camera_chase_angle_get_y,			js_camera_chase_angle_set_y,		kJSPropertyAttributeDontDelete},
							{"z",					js_camera_chase_angle_get_z,			js_camera_chase_angle_set_z,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			camera_chase_angle_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_camera_chase_angle_object(void)
{
	camera_chase_angle_class=script_create_class("camera_chase_angle_class",camera_chase_angle_props,NULL);
}

void script_free_camera_chase_angle_object(void)
{
	script_free_class(camera_chase_angle_class);
}

JSObjectRef script_add_camera_chase_angle_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,camera_chase_angle_class,"chaseAngle"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_camera_chase_angle_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_float_to_value(cx,camera.chase_ang.x));
}

JSValueRef js_camera_chase_angle_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_float_to_value(cx,camera.chase_ang.y));
}

JSValueRef js_camera_chase_angle_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_float_to_value(cx,camera.chase_ang.z));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_camera_chase_angle_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	camera.chase_ang.x=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_camera_chase_angle_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	camera.chase_ang.y=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_camera_chase_angle_set_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	camera.chase_ang.z=script_value_to_float(cx,vp);
	
	return(TRUE);
}
