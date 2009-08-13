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

JSValueRef js_obj_motion_angle_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_motion_angle_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_motion_angle_get_x(void);
JSValueRef js_obj_motion_angle_get_y(void);
JSValueRef js_obj_motion_angle_get_z(void);
JSValueRef js_obj_motion_angle_turn_to_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_motion_angle_turn_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_property	obj_motion_angle_props[]={
							{"x",					js_obj_motion_angle_get_x,				NULL},
							{"y",					js_obj_motion_angle_get_y,				NULL},
							{"z",					js_obj_motion_angle_get_z,				NULL},
							{0}};

script_js_function	obj_motion_angle_functions[]={
							{"turnToAngle",			js_obj_motion_angle_turn_to_angle_func,	2},
							{"turnStop",			js_obj_motion_angle_turn_stop_func,		0},
							{0}};
							
JSClassRef			obj_motion_angle_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_motion_angle_object(void)
{
	obj_motion_angle_class=script_create_class("obj_motion_angle_class",js_obj_motion_angle_get_property,js_obj_motion_angle_set_property);
}

void script_free_obj_motion_angle_object(void)
{
	script_free_class(obj_motion_angle_class);
}

JSObjectRef script_add_obj_motion_angle_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_motion_angle_class,"motionAngle",obj_motion_angle_props,obj_motion_angle_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_motion_angle_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_motion_angle_props));
}

bool js_obj_motion_angle_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,obj_motion_angle_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_motion_angle_get_x(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(obj->motion.ang.x));
}

JSValueRef js_obj_motion_angle_get_y(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(obj->motion.ang.y));
}

JSValueRef js_obj_motion_angle_get_z(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(obj->motion.ang.z));
}

/* =======================================================

      Turn Functions
      
======================================================= */

JSValueRef js_obj_motion_angle_turn_to_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			turn_speed;
	bool			cwise;
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	obj->turn.ang_to.y=script_value_to_float(argv[0]);
	if (obj->turn.ang_to.y<0.0f) obj->turn.ang_to.y=360.0f+obj->turn.ang_to.y;
	if (obj->turn.ang_to.y>=360.0f) obj->turn.ang_to.y=obj->turn.ang_to.y-360.0f;

	turn_speed=object_get_turn_speed(obj);

	switch (script_value_to_int(argv[1])) {
	
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

	return(script_null_to_value());
}

JSValueRef js_obj_motion_angle_turn_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_turn_stop(obj);

	return(script_null_to_value());
}
