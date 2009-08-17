/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.turnSpeed object

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
#include "objects.h"

extern js_type			js;

JSValueRef js_obj_turn_speed_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_turn_speed_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_turn_speed_get_facingWalk(JSContextRef cx);
JSValueRef js_obj_turn_speed_get_motionWalk(JSContextRef cx);
JSValueRef js_obj_turn_speed_get_facingRun(JSContextRef cx);
JSValueRef js_obj_turn_speed_get_motionRun(JSContextRef cx);
JSValueRef js_obj_turn_speed_get_facingCrawl(JSContextRef cx);
JSValueRef js_obj_turn_speed_get_motionCrawl(JSContextRef cx);
JSValueRef js_obj_turn_speed_get_facingAir(JSContextRef cx);
JSValueRef js_obj_turn_speed_get_motionAir(JSContextRef cx);
JSValueRef js_obj_turn_speed_get_key(JSContextRef cx);
JSValueRef js_obj_turn_speed_get_topDownAngleOffset(JSContextRef cx);
void js_obj_turn_speed_set_facingWalk(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_turn_speed_set_motionWalk(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_turn_speed_set_facingRun(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_turn_speed_set_motionRun(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_turn_speed_set_facingCrawl(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_turn_speed_set_motionCrawl(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_turn_speed_set_facingAir(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_turn_speed_set_motionAir(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_turn_speed_set_key(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_turn_speed_set_topDownAngleOffset(JSContextRef cx,JSValueRef vp,JSValueRef *exception);

script_js_property	obj_turn_speed_props[]={
							{"facingWalk",				js_obj_turn_speed_get_facingWalk,			js_obj_turn_speed_set_facingWalk},
							{"motionWalk",				js_obj_turn_speed_get_motionWalk,			js_obj_turn_speed_set_motionWalk},
							{"facingRun",				js_obj_turn_speed_get_facingRun,			js_obj_turn_speed_set_facingRun},
							{"motionRun",				js_obj_turn_speed_get_motionRun,			js_obj_turn_speed_set_motionRun},
							{"facingCrawl",				js_obj_turn_speed_get_facingCrawl,			js_obj_turn_speed_set_facingCrawl},
							{"motionCrawl",				js_obj_turn_speed_get_motionCrawl,			js_obj_turn_speed_set_motionCrawl},
							{"facingAir",				js_obj_turn_speed_get_facingAir,			js_obj_turn_speed_set_facingAir},
							{"motionAir",				js_obj_turn_speed_get_motionAir,			js_obj_turn_speed_set_motionAir},
							{"key",						js_obj_turn_speed_get_key,					js_obj_turn_speed_set_key},
							{"topDownAngleOffset",		js_obj_turn_speed_get_topDownAngleOffset,	js_obj_turn_speed_set_topDownAngleOffset},
							{0}};

JSClassRef			obj_turn_speed_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_turn_speed_object(void)
{
	obj_turn_speed_class=script_create_class("obj_turn_speed_class",js_obj_turn_speed_get_property,js_obj_turn_speed_set_property);
}

void script_free_obj_turn_speed_object(void)
{
	script_free_class(obj_turn_speed_class);
}

JSObjectRef script_add_obj_turn_speed_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_turn_speed_class,"turnSpeed",obj_turn_speed_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_turn_speed_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_turn_speed_props));
}

bool js_obj_turn_speed_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,obj_turn_speed_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_turn_speed_get_facingWalk(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,obj->turn.walk_speed));
}

JSValueRef js_obj_turn_speed_get_motionWalk(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,obj->turn.walk_motion_speed));
}

JSValueRef js_obj_turn_speed_get_facingRun(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,obj->turn.run_speed));
}

JSValueRef js_obj_turn_speed_get_motionRun(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,obj->turn.run_motion_speed));
}

JSValueRef js_obj_turn_speed_get_facingCrawl(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,obj->turn.crawl_speed));
}

JSValueRef js_obj_turn_speed_get_motionCrawl(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,obj->turn.crawl_motion_speed));
}

JSValueRef js_obj_turn_speed_get_facingAir(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,obj->turn.air_speed));
}

JSValueRef js_obj_turn_speed_get_motionAir(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,obj->turn.air_motion_speed));
}

JSValueRef js_obj_turn_speed_get_key(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,obj->turn.key_speed));
}

JSValueRef js_obj_turn_speed_get_topDownAngleOffset(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,obj->turn.top_down_ang_offset));
}

/* =======================================================

      Setters
      
======================================================= */

void js_obj_turn_speed_set_facingWalk(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.walk_speed=fabsf(script_value_to_float(cx,vp));
}

void js_obj_turn_speed_set_motionWalk(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.walk_motion_speed=fabsf(script_value_to_float(cx,vp));
}

void js_obj_turn_speed_set_facingRun(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.run_speed=fabsf(script_value_to_float(cx,vp));
}

void js_obj_turn_speed_set_motionRun(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.run_motion_speed=fabsf(script_value_to_float(cx,vp));
}

void js_obj_turn_speed_set_facingCrawl(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.crawl_speed=fabsf(script_value_to_float(cx,vp));
}

void js_obj_turn_speed_set_motionCrawl(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.crawl_motion_speed=fabsf(script_value_to_float(cx,vp));
}

void js_obj_turn_speed_set_facingAir(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.air_speed=fabsf(script_value_to_float(cx,vp));
}

void js_obj_turn_speed_set_motionAir(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.air_motion_speed=fabsf(script_value_to_float(cx,vp));
}

void js_obj_turn_speed_set_key(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.key_speed=fabsf(script_value_to_float(cx,vp));
}

void js_obj_turn_speed_set_topDownAngleOffset(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.top_down_ang_offset=fabsf(script_value_to_float(cx,vp));
}

