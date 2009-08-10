/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.slop object

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

JSValueRef js_camera_chase_slop_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_camera_chase_slop_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_camera_chase_slop_get_x(void);
JSValueRef js_camera_chase_slop_get_y(void);
JSValueRef js_camera_chase_slop_get_z(void);
bool js_camera_chase_slop_set_x(JSValueRef vp);
bool js_camera_chase_slop_set_y(JSValueRef vp);
bool js_camera_chase_slop_set_z(JSValueRef vp);

script_js_property	camera_chase_slop_props[]={
							{"x",					js_camera_chase_slop_get_x,			js_camera_chase_slop_set_x},
							{"y",					js_camera_chase_slop_get_y,			js_camera_chase_slop_set_y},
							{"z",					js_camera_chase_slop_get_z,			js_camera_chase_slop_set_z},
							{0}};

JSClassRef			camera_chase_slop_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_camera_chase_slop_object(void)
{
	camera_chase_slop_class=script_create_class("camera_chase_slop_class",js_camera_chase_slop_get_property,js_camera_chase_slop_set_property);
}

void script_free_camera_chase_slop_object(void)
{
	script_free_class(camera_chase_slop_class);
}

JSObjectRef script_add_camera_chase_slop_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,camera_chase_slop_class,"chaseSlop",camera_chase_slop_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_camera_chase_slop_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,camera_chase_slop_props));
}

bool js_camera_chase_slop_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,camera_chase_slop_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_camera_chase_slop_get_x(void)
{
	*vp=script_float_to_value(camera.chase_slop.x);
	return(TRUE);
}

JSValueRef js_camera_chase_slop_get_y(void)
{
	*vp=script_float_to_value(camera.chase_slop.y);
	return(TRUE);
}

JSValueRef js_camera_chase_slop_get_z(void)
{
	*vp=script_float_to_value(camera.chase_slop.z);
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_camera_chase_slop_set_x(JSValueRef vp)
{
	camera.chase_slop.x=script_value_to_float(*vp);
	return(TRUE);
}

bool js_camera_chase_slop_set_y(JSValueRef vp)
{
	camera.chase_slop.y=script_value_to_float(*vp);
	return(TRUE);
}

bool js_camera_chase_slop_set_z(JSValueRef vp)
{
	camera.chase_slop.z=script_value_to_float(*vp);
	return(TRUE);
}
