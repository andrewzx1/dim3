/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.chase object

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

JSValueRef js_camera_chase_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_camera_chase_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_camera_chase_get_size(void);
JSValueRef js_camera_chase_get_distance(void);
JSValueRef js_camera_chase_get_trackSpeed(void);
void js_camera_chase_set_size(JSValueRef vp,JSValueRef *exception);
void js_camera_chase_set_distance(JSValueRef vp,JSValueRef *exception);
void js_camera_chase_set_trackSpeed(JSValueRef vp,JSValueRef *exception);

script_js_property	camera_chase_props[]={
							{"size",				js_camera_chase_get_size,				js_camera_chase_set_size},
							{"distance",			js_camera_chase_get_distance,			js_camera_chase_set_distance},
							{"trackSpeed",			js_camera_chase_get_trackSpeed,			js_camera_chase_set_trackSpeed},
							{0}};

JSClassRef			camera_chase_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_camera_chase_object(void)
{
	camera_chase_class=script_create_class("camera_chase_class",js_camera_chase_get_property,js_camera_chase_set_property);
}

void script_free_camera_chase_object(void)
{
	script_free_class(camera_chase_class);
}

JSObjectRef script_add_camera_chase_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,camera_chase_class,"chase",camera_chase_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_camera_chase_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,camera_chase_props));
}

bool js_camera_chase_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,camera_chase_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_camera_chase_get_size(void)
{
	*vp=script_int_to_value(camera.chase_size);
	return(TRUE);
}

JSValueRef js_camera_chase_get_distance(void)
{
	*vp=script_int_to_value(camera.chase_distance);
	return(TRUE);
}

JSValueRef js_camera_chase_get_trackSpeed(void)
{
	*vp=script_float_to_value(camera.chase_track_speed);
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

void js_camera_chase_set_size(JSValueRef vp,JSValueRef *exception)
{
	camera.chase_size=script_value_to_int(*vp);
}

void js_camera_chase_set_distance(JSValueRef vp,JSValueRef *exception)
{
	camera.chase_distance=script_value_to_int(*vp);
}

void js_camera_chase_set_trackSpeed(JSValueRef vp,JSValueRef *exception)
{
	camera.chase_track_speed=script_value_to_float(*vp);
}
