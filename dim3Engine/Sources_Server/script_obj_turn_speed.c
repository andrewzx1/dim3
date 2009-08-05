/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.turnSpeed object

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

JSBool js_obj_turn_speed_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_turn_speed_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
bool js_obj_turn_speed_get_facingWalk(jsval *vp);
bool js_obj_turn_speed_get_motionWalk(jsval *vp);
bool js_obj_turn_speed_get_facingRun(jsval *vp);
bool js_obj_turn_speed_get_motionRun(jsval *vp);
bool js_obj_turn_speed_get_facingCrawl(jsval *vp);
bool js_obj_turn_speed_get_motionCrawl(jsval *vp);
bool js_obj_turn_speed_get_facingAir(jsval *vp);
bool js_obj_turn_speed_get_motionAir(jsval *vp);
bool js_obj_turn_speed_get_key(jsval *vp);
bool js_obj_turn_speed_get_topDownAngleOffset(jsval *vp);
bool js_obj_turn_speed_set_facingWalk(jsval *vp);
bool js_obj_turn_speed_set_motionWalk(jsval *vp);
bool js_obj_turn_speed_set_facingRun(jsval *vp);
bool js_obj_turn_speed_set_motionRun(jsval *vp);
bool js_obj_turn_speed_set_facingCrawl(jsval *vp);
bool js_obj_turn_speed_set_motionCrawl(jsval *vp);
bool js_obj_turn_speed_set_facingAir(jsval *vp);
bool js_obj_turn_speed_set_motionAir(jsval *vp);
bool js_obj_turn_speed_set_key(jsval *vp);
bool js_obj_turn_speed_set_topDownAngleOffset(jsval *vp);

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

JSObject* script_add_obj_turn_speed_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,obj_turn_speed_class,"turnSpeed",obj_turn_speed_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_obj_turn_speed_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,obj_turn_speed_props));
}

JSBool js_obj_turn_speed_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,obj_turn_speed_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_obj_turn_speed_get_facingWalk(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.walk_speed);
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_get_motionWalk(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.walk_motion_speed);
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_get_facingRun(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.run_speed);
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_get_motionRun(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.run_motion_speed);
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_get_facingCrawl(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.crawl_speed);
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_get_motionCrawl(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.crawl_motion_speed);
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_get_facingAir(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.air_speed);
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_get_motionAir(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.air_motion_speed);
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_get_key(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.key_speed);
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_get_topDownAngleOffset(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->turn.top_down_ang_offset);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_turn_speed_set_facingWalk(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.walk_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_set_motionWalk(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.walk_motion_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_set_facingRun(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.run_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_set_motionRun(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.run_motion_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_set_facingCrawl(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.crawl_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_set_motionCrawl(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.crawl_motion_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_set_facingAir(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.air_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_set_motionAir(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.air_motion_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_set_key(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.key_speed=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

bool js_obj_turn_speed_set_topDownAngleOffset(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.top_down_ang_offset=fabsf(script_value_to_float(*vp));
	
	return(JS_TRUE);
}

