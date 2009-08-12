/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.setting object

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

JSValueRef js_camera_setting_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_camera_setting_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_camera_setting_get_type(void);
JSValueRef js_camera_setting_get_attachObjectId(void);
void js_camera_setting_set_type(JSValueRef vp,JSValueRef *exception);
JSValueRef js_camera_setting_attach_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_property	camera_setting_props[]={
							{"type",				js_camera_setting_get_type,				js_camera_setting_set_type},
							{"attachObjectId",		js_camera_setting_get_attachObjectId,	NULL},
							{0}};

script_js_function	camera_setting_functions[]={
							{"attach",				js_camera_setting_attach_func,			1},
							{0}};

JSClassRef			camera_setting_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_camera_setting_object(void)
{
	camera_setting_class=script_create_class("camera_setting_class",js_camera_setting_get_property,js_camera_setting_set_property);
}

void script_free_camera_setting_object(void)
{
	script_free_class(camera_setting_class);
}

JSObjectRef script_add_camera_setting_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,camera_setting_class,"setting",camera_setting_props,camera_setting_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_camera_setting_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,camera_setting_props));
}

bool js_camera_setting_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,camera_setting_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_camera_setting_get_type(void)
{
	*vp=script_int_to_value(camera.mode+sd_camera_type_fpp);
	return(TRUE);
}

JSValueRef js_camera_setting_get_attachObjectId(void)
{
	*vp=script_int_to_value(camera.obj_uid);
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

void js_camera_setting_set_type(JSValueRef vp,JSValueRef *exception)
{
	camera.mode=script_value_to_int(*vp)-sd_camera_type_fpp;
}

/* =======================================================

      State Function
      
======================================================= */

JSValueRef js_camera_setting_attach_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(FALSE);

		// connect

	camera_connect(obj);

	return(TRUE);
}
