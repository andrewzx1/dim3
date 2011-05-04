/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.plane object

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "scripts.h"

extern camera_type		camera;
extern js_type			js;

JSValueRef js_camera_plane_get_fov(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_camera_plane_get_aspectRatio(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_camera_plane_get_near(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_camera_plane_get_far(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_camera_plane_get_nearOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_camera_plane_set_fov(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_camera_plane_set_aspectRatio(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_camera_plane_set_near(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_camera_plane_set_far(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_camera_plane_set_nearOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		camera_plane_props[]={
							{"fov",					js_camera_plane_get_fov,				js_camera_plane_set_fov,			kJSPropertyAttributeDontDelete},
							{"aspectRatio",			js_camera_plane_get_aspectRatio,		js_camera_plane_set_aspectRatio,	kJSPropertyAttributeDontDelete},
							{"near",				js_camera_plane_get_near,				js_camera_plane_set_near,			kJSPropertyAttributeDontDelete},
							{"far",					js_camera_plane_get_far,				js_camera_plane_set_far,			kJSPropertyAttributeDontDelete},
							{"nearOffset",			js_camera_plane_get_nearOffset,			js_camera_plane_set_nearOffset,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			camera_plane_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_camera_plane_object(void)
{
	camera_plane_class=script_create_class("camera_plane_class",camera_plane_props,NULL);
}

void script_free_camera_plane_object(void)
{
	script_free_class(camera_plane_class);
}

JSObjectRef script_add_camera_plane_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,camera_plane_class,"plane",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_camera_plane_get_fov(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_float_to_value(cx,camera.setup.plane.fov));
}

JSValueRef js_camera_plane_get_aspectRatio(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_float_to_value(cx,camera.setup.plane.aspect_ratio));
}

JSValueRef js_camera_plane_get_near(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,camera.setup.plane.near_z));
}

JSValueRef js_camera_plane_get_far(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,camera.setup.plane.far_z));
}

JSValueRef js_camera_plane_get_nearOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,camera.setup.plane.near_z_offset));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_camera_plane_set_fov(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	camera.setup.plane.fov=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_camera_plane_set_aspectRatio(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	camera.setup.plane.aspect_ratio=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_camera_plane_set_near(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	camera.setup.plane.near_z=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_camera_plane_set_far(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	camera.setup.plane.far_z=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_camera_plane_set_nearOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	camera.setup.plane.near_z_offset=script_value_to_int(cx,vp);
	
	return(TRUE);
}
