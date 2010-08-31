/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.angle object

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
#include "objects.h"

extern map_type			map;
extern js_type			js;

extern void object_setup_motion(obj_type *obj,float ang,float speed);

JSValueRef js_obj_angle_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_angle_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_angle_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_angle_rotate_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_angle_rotate_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_angle_props[]={
							{"x",					js_obj_angle_get_x,					NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"y",					js_obj_angle_get_y,					NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"z",					js_obj_angle_get_z,					NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	obj_angle_functions[]={
							{"rotate",				js_obj_angle_rotate_func,			kJSPropertyAttributeDontDelete},
							{"rotateTo",			js_obj_angle_rotate_to_func,		kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_angle_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_angle_object(void)
{
	obj_angle_class=script_create_class("obj_angle_class",obj_angle_props,obj_angle_functions);
}

void script_free_obj_angle_object(void)
{
	script_free_class(obj_angle_class);
}

JSObjectRef script_add_obj_angle_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,obj_angle_class,"angle"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_angle_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_float_to_value(cx,obj->ang.x));
}

JSValueRef js_obj_angle_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_float_to_value(cx,obj->ang.y));
}

JSValueRef js_obj_angle_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_float_to_value(cx,obj->ang.z));
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_obj_angle_rotate_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			ang_add;
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();

	ang_add=script_value_to_float(cx,argv[0]);
	obj->ang.y=angle_add(obj->ang.y,ang_add);

	return(script_null_to_value(cx));
}

JSValueRef js_obj_angle_rotate_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	obj->ang.y=script_value_to_float(cx,argv[0]);

	return(script_null_to_value(cx));
}






