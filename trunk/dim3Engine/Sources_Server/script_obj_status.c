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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"
#include "objects.h"

extern map_type			map;
extern js_type			js;

extern void view_draw_tint_start(d3col *col,float alpha,int fade_in_msec,int life_msec,int fade_out_msec);

JSValueRef js_obj_status_get_speed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_status_get_moving(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_status_get_running(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_status_get_backward(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_status_get_sliding(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_status_get_stand(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_status_get_air(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_status_get_liquid(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_status_get_standOnObjectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_status_get_standUnderObjectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_status_freeze_input_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_status_tint_view_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_status_props[]={
							{"speed",				js_obj_status_get_speed,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"moving",				js_obj_status_get_moving,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"running",				js_obj_status_get_running,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"backward",			js_obj_status_get_backward,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"sliding",				js_obj_status_get_sliding,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"stand",				js_obj_status_get_stand,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"air",					js_obj_status_get_air,					NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"liquid",				js_obj_status_get_liquid,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"standOnObjectId",		js_obj_status_get_standOnObjectId,		NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"standUnderObjectId",	js_obj_status_get_standUnderObjectId,	NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	obj_status_functions[]={
							{"freezeInput",			js_obj_status_freeze_input_func,		kJSPropertyAttributeDontDelete},
							{"tintView",			js_obj_status_tint_view_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_status_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_status_object(void)
{
	obj_status_class=script_create_class("obj_status_class",obj_status_props,obj_status_functions);
}

void script_free_obj_status_object(void)
{
	script_free_class(obj_status_class);
}

JSObjectRef script_add_obj_status_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,obj_status_class,"status"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_status_get_speed(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_float_to_value(cx,obj->forward_move.speed));
}

JSValueRef js_obj_status_get_moving(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_bool_to_value(cx,obj->forward_move.moving));
}

JSValueRef js_obj_status_get_running(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_bool_to_value(cx,obj->forward_move.running));
}

JSValueRef js_obj_status_get_backward(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_bool_to_value(cx,obj->forward_move.reverse));
}

JSValueRef js_obj_status_get_sliding(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_bool_to_value(cx,obj->side_move.moving));
}

JSValueRef js_obj_status_get_stand(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->duck.mode+sd_stand_standing));
}

JSValueRef js_obj_status_get_air(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->air_mode+sd_air_up));
}

JSValueRef js_obj_status_get_liquid(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->liquid.mode+sd_liquid_out));
}

JSValueRef js_obj_status_get_standOnObjectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	if (!obj->touch.stand) return(script_int_to_value(cx,-1));
	
	return(script_int_to_value(cx,obj->touch.obj_idx));
}

JSValueRef js_obj_status_get_standUnderObjectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,object_find_idx_by_stood_on_object_idx(obj->idx)));
}

/* =======================================================

      Input Freeze Functions
      
======================================================= */

JSValueRef js_obj_status_freeze_input_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	object_input_freeze(obj,script_value_to_bool(cx,argv[0]));

	return(script_null_to_value(cx));
}

/* =======================================================

      Full Screen Effects
      
======================================================= */

JSValueRef js_obj_status_tint_view_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3col			col;
	
	if (!script_check_param_count(cx,func,argc,7,exception)) return(script_null_to_value(cx));
	
	col.r=script_value_to_float(cx,argv[0]);
	col.g=script_value_to_float(cx,argv[1]);
	col.b=script_value_to_float(cx,argv[2]);

	view_draw_tint_start(&col,script_value_to_float(cx,argv[3]),script_value_to_int(cx,argv[4]),script_value_to_int(cx,argv[5]),script_value_to_int(cx,argv[6]));
 
	return(script_bool_to_value(cx,TRUE));
}
