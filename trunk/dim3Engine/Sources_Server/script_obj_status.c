/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.status object

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

JSValueRef js_obj_status_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_status_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_status_get_speed(void);
JSValueRef js_obj_status_get_moving(void);
JSValueRef js_obj_status_get_running(void);
JSValueRef js_obj_status_get_backward(void);
JSValueRef js_obj_status_get_sliding(void);
JSValueRef js_obj_status_get_stand(void);
JSValueRef js_obj_status_get_air(void);
JSValueRef js_obj_status_get_liquid(void);
JSValueRef js_obj_status_get_standOnObjectId(void);
JSValueRef js_obj_status_get_standUnderObjectId(void);
JSValueRef js_obj_status_freeze_input_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_status_tint_view_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_property	obj_status_props[]={
							{"speed",				js_obj_status_get_speed,				NULL},
							{"moving",				js_obj_status_get_moving,				NULL},
							{"running",				js_obj_status_get_running,				NULL},
							{"backward",			js_obj_status_get_backward,				NULL},
							{"sliding",				js_obj_status_get_sliding,				NULL},
							{"stand",				js_obj_status_get_stand,				NULL},
							{"air",					js_obj_status_get_air,					NULL},
							{"liquid",				js_obj_status_get_liquid,				NULL},
							{"standOnObjectId",		js_obj_status_get_standOnObjectId,		NULL},
							{"standUnderObjectId",	js_obj_status_get_standUnderObjectId,	NULL},
							{0}};
							
script_js_function	obj_status_functions[]={
							{"freezeInput",			js_obj_status_freeze_input_func,		1},
							{"tintView",			js_obj_status_tint_view_func,			7},
							{0}};

JSClassRef			obj_status_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_status_object(void)
{
	obj_status_class=script_create_class("obj_status_class",js_obj_status_get_property,js_obj_status_set_property);
}

void script_free_obj_status_object(void)
{
	script_free_class(obj_status_class);
}

JSObjectRef script_add_obj_status_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_status_class,"status",obj_status_props,obj_status_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_status_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_status_props));
}

bool js_obj_status_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,obj_status_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_status_get_speed(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(obj->forward_move.speed));
}

JSValueRef js_obj_status_get_moving(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->forward_move.moving));
}

JSValueRef js_obj_status_get_running(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->forward_move.running));
}

JSValueRef js_obj_status_get_backward(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->forward_move.reverse));
}

JSValueRef js_obj_status_get_sliding(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->side_move.moving));
}

JSValueRef js_obj_status_get_stand(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(obj->duck.mode+sd_stand_standing));
}

JSValueRef js_obj_status_get_air(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(obj->air_mode+sd_air_up));
}

JSValueRef js_obj_status_get_liquid(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(obj->liquid_mode+sd_liquid_out));
}

JSValueRef js_obj_status_get_standOnObjectId(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(obj->stand_obj_uid));
}

JSValueRef js_obj_status_get_standUnderObjectId(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(object_find_uid_by_stood_on_object_uid(obj->uid)));
}

/* =======================================================

      Input Freeze Functions
      
======================================================= */

JSValueRef js_obj_status_freeze_input_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_input_freeze(obj,script_value_to_bool(argv[0]));

	return(script_null_to_value());
}

/* =======================================================

      Full Screen Effects
      
======================================================= */

JSValueRef js_obj_status_tint_view_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			r,g,b;
	obj_type		*obj;
	
	r=script_value_to_float(argv[0]);
	g=script_value_to_float(argv[1]);
	b=script_value_to_float(argv[2]);

	obj=object_find_uid(js.attach.thing_uid);
	object_fs_tint_start(js.time.current_tick,obj,r,g,b,script_value_to_float(argv[3]),script_value_to_int(argv[4]),script_value_to_int(argv[5]),script_value_to_int(argv[6]));
    return(script_bool_to_value(TRUE));
}
