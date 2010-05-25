/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.size object

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

extern js_type			js;

JSValueRef js_obj_size_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_size_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_size_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_size_get_eyeOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_size_get_weight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_size_get_clickDistance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_size_get_cameraZAdjust(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_size_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_size_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_size_set_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_size_set_eyeOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_size_set_weight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_size_set_clickDistance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_size_set_cameraZAdjust(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_size_grow_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_size_grow_over_time_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_size_grow_over_time_change_size_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_size_grow_over_time_change_offset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_size_props[]={
							{"x",					js_obj_size_get_x,					js_obj_size_set_x,					kJSPropertyAttributeDontDelete},
							{"y",					js_obj_size_get_y,					js_obj_size_set_y,					kJSPropertyAttributeDontDelete},
							{"z",					js_obj_size_get_z,					js_obj_size_set_z,					kJSPropertyAttributeDontDelete},
							{"eyeOffset",			js_obj_size_get_eyeOffset,			js_obj_size_set_eyeOffset,			kJSPropertyAttributeDontDelete},
							{"weight",				js_obj_size_get_weight,				js_obj_size_set_weight,				kJSPropertyAttributeDontDelete},
							{"clickDistance",		js_obj_size_get_clickDistance,		js_obj_size_set_clickDistance,		kJSPropertyAttributeDontDelete},
							{"cameraZAdjust",		js_obj_size_get_cameraZAdjust,		js_obj_size_set_cameraZAdjust,		kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	obj_size_functions[]={
							{"growTo",						js_obj_size_grow_to_func,						kJSPropertyAttributeDontDelete},
							{"growOverTime",				js_obj_size_grow_over_time_func,				kJSPropertyAttributeDontDelete},
							{"growOverTimeChangeSize",		js_obj_size_grow_over_time_change_size_func,	kJSPropertyAttributeDontDelete},
							{"growOverTimeChangeOffset",	js_obj_size_grow_over_time_change_offset_func,	kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_size_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_size_object(void)
{
	obj_size_class=script_create_class("obj_size_class",obj_size_props,obj_size_functions);
}

void script_free_obj_size_object(void)
{
	script_free_class(obj_size_class);
}

JSObjectRef script_add_obj_size_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,obj_size_class,"size"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_size_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->size.x));
}

JSValueRef js_obj_size_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->size.y));
}

JSValueRef js_obj_size_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->size.z));
}

JSValueRef js_obj_size_get_eyeOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->size.eye_offset));
}

JSValueRef js_obj_size_get_weight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->size.weight));
}

JSValueRef js_obj_size_get_clickDistance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->click.distance));
}

JSValueRef js_obj_size_get_cameraZAdjust(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->camera_z_adjust));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_size_set_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_script_lookup();
	obj->size.x=script_value_to_int(cx,vp);
	object_set_radius(obj);

	return(TRUE);
}

bool js_obj_size_set_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_script_lookup();
	obj->size.y=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_obj_size_set_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_script_lookup();
	obj->size.z=script_value_to_int(cx,vp);
	object_set_radius(obj);

	return(TRUE);
}

bool js_obj_size_set_eyeOffset(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_script_lookup();
	obj->size.eye_offset=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_obj_size_set_weight(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_script_lookup();
	obj->size.weight=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_obj_size_set_clickDistance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_script_lookup();
	obj->click.distance=script_value_to_int(cx,vp);

	return(TRUE);
}

bool js_obj_size_set_cameraZAdjust(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_script_lookup();
	obj->camera_z_adjust=script_value_to_int(cx,vp);

	return(TRUE);
}

/* =======================================================

      Function
      
======================================================= */

JSValueRef js_obj_size_grow_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			resize;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	resize=script_value_to_float(cx,argv[0]);
	
	obj=object_script_lookup();
	object_grow_direct(obj,resize);
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_size_grow_over_time_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				msec;
	float			resize;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	resize=script_value_to_float(cx,argv[0]);
	msec=script_value_to_int(cx,argv[1]);
	
	obj=object_script_lookup();
	object_grow_start(obj,msec,resize,NULL,NULL);
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_size_grow_over_time_change_size_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				msec;
	float			resize;
	d3pnt			size;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,5,exception)) return(script_null_to_value(cx));
	
	resize=script_value_to_float(cx,argv[0]);
	
	size.x=script_value_to_int(cx,argv[1]);
	size.z=script_value_to_int(cx,argv[2]);
	size.y=script_value_to_int(cx,argv[3]);

	msec=script_value_to_int(cx,argv[4]);
	
	obj=object_script_lookup();
	object_grow_start(obj,msec,resize,&size,NULL);
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_size_grow_over_time_change_offset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				msec;
	float			resize;
	d3pnt			offset;
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,5,exception)) return(script_null_to_value(cx));
	
	resize=script_value_to_float(cx,argv[0]);
	
	offset.x=script_value_to_int(cx,argv[1]);
	offset.z=script_value_to_int(cx,argv[2]);
	offset.y=script_value_to_int(cx,argv[3]);

	msec=script_value_to_int(cx,argv[4]);
	
	obj=object_script_lookup();
	object_grow_start(obj,msec,resize,NULL,&offset);
	
	return(script_null_to_value(cx));
}
