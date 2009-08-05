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

JSBool js_camera_plane_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_camera_plane_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
bool js_camera_plane_get_projectionType(jsval *vp);
bool js_camera_plane_get_fov(jsval *vp);
bool js_camera_plane_get_aspectRatio(jsval *vp);
bool js_camera_plane_get_left(jsval *vp);
bool js_camera_plane_get_right(jsval *vp);
bool js_camera_plane_get_top(jsval *vp);
bool js_camera_plane_get_bottom(jsval *vp);
bool js_camera_plane_get_near(jsval *vp);
bool js_camera_plane_get_far(jsval *vp);
bool js_camera_plane_get_nearOffset(jsval *vp);
bool js_camera_plane_set_projectionType(jsval *vp);
bool js_camera_plane_set_fov(jsval *vp);
bool js_camera_plane_set_aspectRatio(jsval *vp);
bool js_camera_plane_set_left(jsval *vp);
bool js_camera_plane_set_right(jsval *vp);
bool js_camera_plane_set_top(jsval *vp);
bool js_camera_plane_set_bottom(jsval *vp);
bool js_camera_plane_set_near(jsval *vp);
bool js_camera_plane_set_far(jsval *vp);
bool js_camera_plane_set_nearOffset(jsval *vp);

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

JSObject* script_add_camera_plane_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,camera_plane_class,"plane",camera_plane_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_camera_plane_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,camera_plane_props));
}

JSBool js_camera_plane_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,camera_plane_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_camera_plane_get_projectionType(jsval *vp)
{
	*vp=script_int_to_value(camera.plane.type+sd_projection_type_fov);
	return(JS_TRUE);
}

bool js_camera_plane_get_fov(jsval *vp)
{
	*vp=script_float_to_value(camera.plane.fov);
	return(JS_TRUE);
}

bool js_camera_plane_get_aspectRatio(jsval *vp)
{
	*vp=script_float_to_value(camera.plane.aspect_ratio);
	return(JS_TRUE);
}

bool js_camera_plane_get_left(jsval *vp)
{
	*vp=script_int_to_value(camera.plane.lft);
	return(JS_TRUE);
}

bool js_camera_plane_get_right(jsval *vp)
{
	*vp=script_int_to_value(camera.plane.rgt);
	return(JS_TRUE);
}

bool js_camera_plane_get_top(jsval *vp)
{
	*vp=script_int_to_value(camera.plane.top);
	return(JS_TRUE);
}

bool js_camera_plane_get_bottom(jsval *vp)
{
	*vp=script_int_to_value(camera.plane.bot);
	return(JS_TRUE);
}

bool js_camera_plane_get_near(jsval *vp)
{
	*vp=script_int_to_value(camera.plane.near_z);
	return(JS_TRUE);
}

bool js_camera_plane_get_far(jsval *vp)
{
	*vp=script_int_to_value(camera.plane.far_z);
	return(JS_TRUE);
}

bool js_camera_plane_get_nearOffset(jsval *vp)
{
	*vp=script_int_to_value(camera.plane.near_z_offset);
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_camera_plane_set_projectionType(jsval *vp)
{
	camera.plane.type=script_value_to_int(*vp)-sd_projection_type_fov;
	return(JS_TRUE);
}

bool js_camera_plane_set_fov(jsval *vp)
{
	camera.plane.fov=script_value_to_float(*vp);
	return(JS_TRUE);
}

bool js_camera_plane_set_aspectRatio(jsval *vp)
{
	camera.plane.aspect_ratio=script_value_to_float(*vp);
	return(JS_TRUE);
}

bool js_camera_plane_set_left(jsval *vp)
{
	camera.plane.lft=script_value_to_int(*vp);
	return(JS_TRUE);
}

bool js_camera_plane_set_right(jsval *vp)
{
	camera.plane.rgt=script_value_to_int(*vp);
	return(JS_TRUE);
}

bool js_camera_plane_set_top(jsval *vp)
{
	camera.plane.top=script_value_to_int(*vp);
	return(JS_TRUE);
}

bool js_camera_plane_set_bottom(jsval *vp)
{
	camera.plane.bot=script_value_to_int(*vp);
	return(JS_TRUE);
}

bool js_camera_plane_set_near(jsval *vp)
{
	camera.plane.near_z=script_value_to_int(*vp);
	return(JS_TRUE);
}

bool js_camera_plane_set_far(jsval *vp)
{
	camera.plane.far_z=script_value_to_int(*vp);
	return(JS_TRUE);
}

bool js_camera_plane_set_nearOffset(jsval *vp)
{
	camera.plane.near_z_offset=script_value_to_int(*vp);
	return(JS_TRUE);
}
