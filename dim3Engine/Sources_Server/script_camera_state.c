/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.state object

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
#include "consoles.h"
#include "cameras.h"

extern js_type			js;

JSValueRef js_camera_state_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_camera_state_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_camera_state_save_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_camera_state_restore_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_function	camera_state_functions[]={
							{"save",				js_camera_state_save_func,				0},
							{"restore",				js_camera_state_restore_func,			0},
							{0}};

JSClassRef			camera_state_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_camera_state_object(void)
{
	camera_state_class=script_create_class("camera_state_class",js_camera_state_get_property,js_camera_state_set_property);
}

void script_free_camera_state_object(void)
{
	script_free_class(camera_state_class);
}

JSObjectRef script_add_camera_state_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,camera_state_class,"state",NULL,camera_state_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_camera_state_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,NULL));
}

bool js_camera_state_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,NULL));
}

/* =======================================================

      State Function
      
======================================================= */

JSValueRef js_camera_state_save_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	camera_save();
	return(TRUE);
}

JSValueRef js_camera_state_restore_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	camera_restore();
	return(TRUE);
}

