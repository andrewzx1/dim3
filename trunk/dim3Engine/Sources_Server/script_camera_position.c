/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.position object

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
extern view_type		view;
extern js_type			js;

JSValueRef js_camera_position_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_camera_position_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_camera_position_get_x(void);
JSValueRef js_camera_position_get_y(void);
JSValueRef js_camera_position_get_z(void);

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

JSObjectRef script_add_camera_position_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,camera_position_class,"position",camera_position_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_camera_position_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,camera_position_props));
}

bool js_camera_position_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,camera_position_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_camera_position_get_x(void)
{
	*vp=script_int_to_value(view.render->camera.pnt.x);
	return(TRUE);
}

JSValueRef js_camera_position_get_y(void)
{
	*vp=script_int_to_value(view.render->camera.pnt.y);
	return(TRUE);
}

JSValueRef js_camera_position_get_z(void)
{
	*vp=script_int_to_value(view.render->camera.pnt.z);
	return(TRUE);
}

