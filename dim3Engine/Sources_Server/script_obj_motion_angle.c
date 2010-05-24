/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.motionAngle object

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

JSValueRef js_obj_motion_angle_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_motion_angle_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_motion_angle_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_motion_angle_turn_to_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_angle_turn_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_motion_angle_props[]={
							{"x",					js_obj_motion_angle_get_x,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"y",					js_obj_motion_angle_get_y,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"z",					js_obj_motion_angle_get_z,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	obj_motion_angle_functions[]={
							{"turnToAngle",			js_obj_motion_angle_turn_to_angle_func,	kJSPropertyAttributeDontDelete},
							{"turnStop",			js_obj_motion_angle_turn_stop_func,		kJSPropertyAttributeDontDelete},
							{0,0,0}};
							
JSClassRef			obj_motion_angle_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_motion_angle_object(void)
{
	obj_motion_angle_class=script_create_class("obj_motion_angle_class",obj_motion_angle_props,obj_motion_angle_functions);
}

void script_free_obj_motion_angle_object(void)
{
	script_free_class(obj_motion_angle_class);
}

JSObjectRef script_add_obj_motion_angle_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,obj_motion_angle_class,"motionAngle"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_motion_angle_get_x(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.obj_index);
	return(script_float_to_value(cx,obj->motion.ang.x));
}

JSValueRef js_obj_motion_angle_get_y(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.obj_index);
	return(script_float_to_value(cx,obj->motion.ang.y));
}

JSValueRef js_obj_motion_angle_get_z(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.obj_index);
	return(script_float_to_value(cx,obj->motion.ang.z));
}

/* =======================================================

      Turn Functions
      
======================================================= */

JSValueRef js_obj_motion_angle_turn_to_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			turn_speed;
	bool			cwise;
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.obj_index);
	
	obj->turn.ang_to.y=script_value_to_float(cx,argv[0]);
	if (obj->turn.ang_to.y<0.0f) obj->turn.ang_to.y=360.0f+obj->turn.ang_to.y;
	if (obj->turn.ang_to.y>=360.0f) obj->turn.ang_to.y=obj->turn.ang_to.y-360.0f;

	turn_speed=object_get_turn_speed(obj);

	switch (script_value_to_int(cx,argv[1])) {
	
		case -1:
			obj->turn.ang_add.y=-turn_speed;
			break;
		case 1:
			obj->turn.ang_add.y=turn_speed;
			break;
		default:
			angle_dif(obj->turn.ang_to.y,obj->ang.y,&cwise);
			if (cwise) {
				obj->turn.ang_add.y=-turn_speed;
			}
			else {
				obj->turn.ang_add.y=turn_speed;
			}
			break;
	
	}

	return(script_null_to_value(cx));
}

JSValueRef js_obj_motion_angle_turn_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.obj_index);
	object_turn_stop(obj);

	return(script_null_to_value(cx));
}
