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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"
#include "cameras.h"

extern camera_type		camera;
extern js_type			js;

JSValueRef js_camera_plane_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_camera_plane_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_camera_plane_get_projectionType(void);
JSValueRef js_camera_plane_get_fov(void);
JSValueRef js_camera_plane_get_aspectRatio(void);
JSValueRef js_camera_plane_get_left(void);
JSValueRef js_camera_plane_get_right(void);
JSValueRef js_camera_plane_get_top(void);
JSValueRef js_camera_plane_get_bottom(void);
JSValueRef js_camera_plane_get_near(void);
JSValueRef js_camera_plane_get_far(void);
JSValueRef js_camera_plane_get_nearOffset(void);
void js_camera_plane_set_projectionType(JSValueRef vp,JSValueRef *exception);
void js_camera_plane_set_fov(JSValueRef vp,JSValueRef *exception);
void js_camera_plane_set_aspectRatio(JSValueRef vp,JSValueRef *exception);
void js_camera_plane_set_left(JSValueRef vp,JSValueRef *exception);
void js_camera_plane_set_right(JSValueRef vp,JSValueRef *exception);
void js_camera_plane_set_top(JSValueRef vp,JSValueRef *exception);
void js_camera_plane_set_bottom(JSValueRef vp,JSValueRef *exception);
void js_camera_plane_set_near(JSValueRef vp,JSValueRef *exception);
void js_camera_plane_set_far(JSValueRef vp,JSValueRef *exception);
void js_camera_plane_set_nearOffset(JSValueRef vp,JSValueRef *exception);

script_js_property	camera_plane_props[]={
							{"projectionType",		js_camera_plane_get_projectionType,		js_camera_plane_set_projectionType},
							{"fov",					js_camera_plane_get_fov,				js_camera_plane_set_fov},
							{"aspectRatio",			js_camera_plane_get_aspectRatio,		js_camera_plane_set_aspectRatio},
							{"left",				js_camera_plane_get_left,				js_camera_plane_set_left},
							{"right",				js_camera_plane_get_right,				js_camera_plane_set_right},
							{"top",					js_camera_plane_get_top,				js_camera_plane_set_top},
							{"bottom",				js_camera_plane_get_bottom,				js_camera_plane_set_bottom},
							{"near",				js_camera_plane_get_near,				js_camera_plane_set_near},
							{"far",					js_camera_plane_get_far,				js_camera_plane_set_far},
							{"nearOffset",			js_camera_plane_get_nearOffset,			js_camera_plane_set_nearOffset},
							{0}};

JSClassRef			camera_plane_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_camera_plane_object(void)
{
	camera_plane_class=script_create_class("camera_plane_class",js_camera_plane_get_property,js_camera_plane_set_property);
}

void script_free_camera_plane_object(void)
{
	script_free_class(camera_plane_class);
}

JSObjectRef script_add_camera_plane_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,camera_plane_class,"plane",camera_plane_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_camera_plane_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,camera_plane_props));
}

bool js_camera_plane_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,camera_plane_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_camera_plane_get_projectionType(void)
{
	return(script_int_to_value(camera.plane.type+sd_projection_type_fov));
}

JSValueRef js_camera_plane_get_fov(void)
{
	return(script_float_to_value(camera.plane.fov));
}

JSValueRef js_camera_plane_get_aspectRatio(void)
{
	return(script_float_to_value(camera.plane.aspect_ratio));
}

JSValueRef js_camera_plane_get_left(void)
{
	return(script_int_to_value(camera.plane.lft));
}

JSValueRef js_camera_plane_get_right(void)
{
	return(script_int_to_value(camera.plane.rgt));
}

JSValueRef js_camera_plane_get_top(void)
{
	return(script_int_to_value(camera.plane.top));
}

JSValueRef js_camera_plane_get_bottom(void)
{
	return(script_int_to_value(camera.plane.bot));
}

JSValueRef js_camera_plane_get_near(void)
{
	return(script_int_to_value(camera.plane.near_z));
}

JSValueRef js_camera_plane_get_far(void)
{
	return(script_int_to_value(camera.plane.far_z));
}

JSValueRef js_camera_plane_get_nearOffset(void)
{
	return(script_int_to_value(camera.plane.near_z_offset));
}

/* =======================================================

      Setters
      
======================================================= */

void js_camera_plane_set_projectionType(JSValueRef vp,JSValueRef *exception)
{
	camera.plane.type=script_value_to_int(vp)-sd_projection_type_fov;
}

void js_camera_plane_set_fov(JSValueRef vp,JSValueRef *exception)
{
	camera.plane.fov=script_value_to_float(vp);
}

void js_camera_plane_set_aspectRatio(JSValueRef vp,JSValueRef *exception)
{
	camera.plane.aspect_ratio=script_value_to_float(vp);
}

void js_camera_plane_set_left(JSValueRef vp,JSValueRef *exception)
{
	camera.plane.lft=script_value_to_int(vp);
}

void js_camera_plane_set_right(JSValueRef vp,JSValueRef *exception)
{
	camera.plane.rgt=script_value_to_int(vp);
}

void js_camera_plane_set_top(JSValueRef vp,JSValueRef *exception)
{
	camera.plane.top=script_value_to_int(vp);
}

void js_camera_plane_set_bottom(JSValueRef vp,JSValueRef *exception)
{
	camera.plane.bot=script_value_to_int(vp);
}

void js_camera_plane_set_near(JSValueRef vp,JSValueRef *exception)
{
	camera.plane.near_z=script_value_to_int(vp);
}

void js_camera_plane_set_far(JSValueRef vp,JSValueRef *exception)
{
	camera.plane.far_z=script_value_to_int(vp);
}

void js_camera_plane_set_nearOffset(JSValueRef vp,JSValueRef *exception)
{
	camera.plane.near_z_offset=script_value_to_int(vp);
}
