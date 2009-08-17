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

JSValueRef js_obj_size_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_size_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_size_get_x(JSContextRef cx);
JSValueRef js_obj_size_get_y(JSContextRef cx);
JSValueRef js_obj_size_get_z(JSContextRef cx);
JSValueRef js_obj_size_get_eyeOffset(JSContextRef cx);
JSValueRef js_obj_size_get_weight(JSContextRef cx);
JSValueRef js_obj_size_get_clickDistance(JSContextRef cx);
JSValueRef js_obj_size_get_cameraZAdjust(JSContextRef cx);
void js_obj_size_set_x(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_size_set_y(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_size_set_z(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_size_set_eyeOffset(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_size_set_weight(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_size_set_clickDistance(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_size_set_cameraZAdjust(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_size_grow_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_size_grow_over_time_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_size_grow_over_time_change_size_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_size_grow_over_time_change_offset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_property	obj_size_props[]={
							{"x",					js_obj_size_get_x,					js_obj_size_set_x},
							{"y",					js_obj_size_get_y,					js_obj_size_set_y},
							{"z",					js_obj_size_get_z,					js_obj_size_set_z},
							{"eyeOffset",			js_obj_size_get_eyeOffset,			js_obj_size_set_eyeOffset},
							{"weight",				js_obj_size_get_weight,				js_obj_size_set_weight},
							{"clickDistance",		js_obj_size_get_clickDistance,		js_obj_size_set_clickDistance},
							{"cameraZAdjust",		js_obj_size_get_cameraZAdjust,		js_obj_size_set_cameraZAdjust},
							{0}};

script_js_function	obj_size_functions[]={
							{"growTo",						js_obj_size_grow_to_func,						1},
							{"growOverTime",				js_obj_size_grow_over_time_func,				2},
							{"growOverTimeChangeSize",		js_obj_size_grow_over_time_change_size_func,	5},
							{"growOverTimeChangeOffset",	js_obj_size_grow_over_time_change_offset_func,	5},
							{0}};

JSClassRef			obj_size_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_size_object(void)
{
	obj_size_class=script_create_class("obj_size_class",js_obj_size_get_property,js_obj_size_set_property);
}

void script_free_obj_size_object(void)
{
	script_free_class(obj_size_class);
}

JSObjectRef script_add_obj_size_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_size_class,"size",obj_size_props,obj_size_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_size_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_size_props));
}

bool js_obj_size_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,obj_size_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_size_get_x(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->size.x));
}

JSValueRef js_obj_size_get_y(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->size.y));
}

JSValueRef js_obj_size_get_z(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->size.z));
}

JSValueRef js_obj_size_get_eyeOffset(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->size.eye_offset));
}

JSValueRef js_obj_size_get_weight(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->size.weight));
}

JSValueRef js_obj_size_get_clickDistance(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->click.distance));
}

JSValueRef js_obj_size_get_cameraZAdjust(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->camera_z_adjust));
}

/* =======================================================

      Setters
      
======================================================= */

void js_obj_size_set_x(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->size.x=script_value_to_int(cx,vp);
	object_set_radius(obj);
}

void js_obj_size_set_y(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->size.y=script_value_to_int(cx,vp);
}

void js_obj_size_set_z(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->size.z=script_value_to_int(cx,vp);
	object_set_radius(obj);
}

void js_obj_size_set_eyeOffset(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->size.eye_offset=script_value_to_int(cx,vp);
}

void js_obj_size_set_weight(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->size.weight=script_value_to_int(cx,vp);
}

void js_obj_size_set_clickDistance(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->click.distance=script_value_to_int(cx,vp);
}

void js_obj_size_set_cameraZAdjust(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->camera_z_adjust=script_value_to_int(cx,vp);
}

/* =======================================================

      Function
      
======================================================= */

JSValueRef js_obj_size_grow_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			resize;
	obj_type		*obj;

	resize=script_value_to_float(cx,argv[0]);
	
	obj=object_find_uid(js.attach.thing_uid);
	object_grow_direct(obj,resize);
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_size_grow_over_time_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				msec;
	float			resize;
	obj_type		*obj;

	resize=script_value_to_float(cx,argv[0]);
	msec=script_value_to_int(cx,argv[1]);
	
	obj=object_find_uid(js.attach.thing_uid);
	object_grow_start(obj,msec,resize,NULL,NULL);
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_size_grow_over_time_change_size_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				msec;
	float			resize;
	d3pnt			size;
	obj_type		*obj;

	resize=script_value_to_float(cx,argv[0]);
	
	size.x=script_value_to_int(cx,argv[1]);
	size.z=script_value_to_int(cx,argv[2]);
	size.y=script_value_to_int(cx,argv[3]);

	msec=script_value_to_int(cx,argv[4]);
	
	obj=object_find_uid(js.attach.thing_uid);
	object_grow_start(obj,msec,resize,&size,NULL);
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_size_grow_over_time_change_offset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				msec;
	float			resize;
	d3pnt			offset;
	obj_type		*obj;

	resize=script_value_to_float(cx,argv[0]);
	
	offset.x=script_value_to_int(cx,argv[1]);
	offset.z=script_value_to_int(cx,argv[2]);
	offset.y=script_value_to_int(cx,argv[3]);

	msec=script_value_to_int(cx,argv[4]);
	
	obj=object_find_uid(js.attach.thing_uid);
	object_grow_start(obj,msec,resize,NULL,&offset);
	
	return(script_null_to_value(cx));
}
