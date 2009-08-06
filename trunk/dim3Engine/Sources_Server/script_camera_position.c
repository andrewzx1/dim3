/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.position object

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
#include "cameras.h"

extern camera_type		camera;
extern view_type		view;
extern js_type			js;

JSBool js_camera_position_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_camera_position_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
bool js_camera_position_get_x(JSValueRef *vp);
bool js_camera_position_get_y(JSValueRef *vp);
bool js_camera_position_get_z(JSValueRef *vp);

script_js_property	camera_position_props[]={
							{"x",					js_camera_position_get_x,				NULL},
							{"y",					js_camera_position_get_y,				NULL},
							{"z",					js_camera_position_get_z,				NULL},
							{0}};
							
JSClassRef			camera_position_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_camera_position_object(void)
{
	camera_position_class=script_create_class("camera_position_class",js_camera_position_get_property,js_camera_position_set_property);
}

void script_free_camera_position_object(void)
{
	script_free_class(camera_position_class);
}

JSObject* script_add_camera_position_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,camera_position_class,"position",camera_position_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_camera_position_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,camera_position_props));
}

JSBool js_camera_position_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,camera_position_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_camera_position_get_x(JSValueRef *vp)
{
	*vp=script_int_to_value(view.render->camera.pnt.x);
	return(TRUE);
}

bool js_camera_position_get_y(JSValueRef *vp)
{
	*vp=script_int_to_value(view.render->camera.pnt.y);
	return(TRUE);
}

bool js_camera_position_get_z(JSValueRef *vp)
{
	*vp=script_int_to_value(view.render->camera.pnt.z);
	return(TRUE);
}

