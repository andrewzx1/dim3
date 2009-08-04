/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.hand object

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
#include "weapons.h"

extern js_type			js;

JSBool js_weap_hand_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_hand_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_hand_get_raiseTick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_hand_get_lowerTick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_hand_get_selectShift(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_hand_get_bobSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_hand_get_bobAngle(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_hand_set_raiseTick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_hand_set_lowerTick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_hand_set_selectShift(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_hand_set_bobSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_hand_set_bobAngle(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

script_js_property	weap_hand_props[]={
							{"raiseTick",			js_weap_hand_get_raiseTick,				js_weap_hand_set_raiseTick},
							{"lowerTick",			js_weap_hand_get_lowerTick,				js_weap_hand_set_lowerTick},
							{"selectShift",			js_weap_hand_get_selectShift,			js_weap_hand_set_selectShift},
							{"bobSpeed",			js_weap_hand_get_bobSpeed,				js_weap_hand_set_bobSpeed},
							{"bobAngle",			js_weap_hand_get_bobAngle,				js_weap_hand_set_bobAngle},
							{0}};

JSClassRef			weap_hand_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_hand_object(void)
{
	weap_hand_class=script_create_class("weap_hand_class",js_weap_hand_get_property,js_weap_hand_set_property);
}

void script_free_weap_hand_object(void)
{
	script_free_class(weap_hand_class);
}

JSObject* script_add_weap_hand_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,weap_hand_class,"hand",weap_hand_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_weap_hand_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,weap_hand_props));
}

JSBool js_weap_hand_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,weap_hand_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_weap_hand_get_raiseTick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->hand.raise_tick);
	
	return(JS_TRUE);
}

bool js_weap_hand_get_lowerTick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->hand.lower_tick);
	
	return(JS_TRUE);
}

bool js_weap_hand_get_selectShift(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->hand.select_shift);
	
	return(JS_TRUE);
}

bool js_weap_hand_get_bobSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->hand.bounce_speed);
	
	return(JS_TRUE);
}

bool js_weap_hand_get_bobAngle(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->hand.bounce_ang);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_hand_set_raiseTick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->hand.raise_tick=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

bool js_weap_hand_set_lowerTick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->hand.lower_tick=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

bool js_weap_hand_set_selectShift(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->hand.select_shift=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

bool js_weap_hand_set_bobSpeed(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->hand.bounce_speed=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

bool js_weap_hand_set_bobAngle(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->hand.bounce_ang=script_value_to_float(*vp);
	
	return(JS_TRUE);
}


