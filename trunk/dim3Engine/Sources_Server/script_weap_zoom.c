/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.zoom object

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
#include "weapons.h"

extern js_type			js;

JSBool js_weap_zoom_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_zoom_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
bool js_weap_zoom_get_on(jsval *vp);
bool js_weap_zoom_get_active(jsval *vp);
bool js_weap_zoom_get_fovMinimum(jsval *vp);
bool js_weap_zoom_get_fovMaximum(jsval *vp);
bool js_weap_zoom_get_fovSteps(jsval *vp);
bool js_weap_zoom_get_turnFactor(jsval *vp);
bool js_weap_zoom_get_crawlTurnFactor(jsval *vp);
bool js_weap_zoom_get_lookFactor(jsval *vp);
bool js_weap_zoom_get_maskName(jsval *vp);
bool js_weap_zoom_get_showWeapon(jsval *vp);
bool js_weap_zoom_get_tick(jsval *vp);
bool js_weap_zoom_set_on(jsval *vp);
bool js_weap_zoom_set_fovMinimum(jsval *vp);
bool js_weap_zoom_set_fovMaximum(jsval *vp);
bool js_weap_zoom_set_fovSteps(jsval *vp);
bool js_weap_zoom_set_turnFactor(jsval *vp);
bool js_weap_zoom_set_crawlTurnFactor(jsval *vp);
bool js_weap_zoom_set_lookFactor(jsval *vp);
bool js_weap_zoom_set_maskName(jsval *vp);
bool js_weap_zoom_set_showWeapon(jsval *vp);
bool js_weap_zoom_set_tick(jsval *vp);

script_js_property	weap_zoom_props[]={
							{"on",					js_weap_zoom_get_on,						js_weap_zoom_set_on},
							{"active",				js_weap_zoom_get_active,					NULL},
							{"fovMinimum",			js_weap_zoom_get_fovMinimum,				js_weap_zoom_set_fovMinimum},
							{"fovMaximum",			js_weap_zoom_get_fovMaximum,				js_weap_zoom_set_fovMaximum},
							{"fovSteps",			js_weap_zoom_get_fovSteps,					js_weap_zoom_set_fovSteps},
							{"turnFactor",			js_weap_zoom_get_turnFactor,				js_weap_zoom_set_turnFactor},
							{"crawlTurnFactor",		js_weap_zoom_get_crawlTurnFactor,			js_weap_zoom_set_crawlTurnFactor},
							{"lookFactor",			js_weap_zoom_get_lookFactor,				js_weap_zoom_set_lookFactor},
							{"maskName",			js_weap_zoom_get_maskName,					js_weap_zoom_set_maskName},
							{"showWeapon",			js_weap_zoom_get_showWeapon,				js_weap_zoom_set_showWeapon},
							{"tick",				js_weap_zoom_get_tick,						js_weap_zoom_set_tick},
							{0}};

JSClassRef			weap_zoom_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_zoom_object(void)
{
	weap_zoom_class=script_create_class("weap_zoom_class",js_weap_zoom_get_property,js_weap_zoom_set_property);
}

void script_free_weap_zoom_object(void)
{
	script_free_class(weap_zoom_class);
}

JSObject* script_add_weap_zoom_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,weap_zoom_class,"zoom",weap_zoom_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_weap_zoom_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,weap_zoom_props));
}

JSBool js_weap_zoom_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,weap_zoom_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_weap_zoom_get_on(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(weap->zoom.on);
	
	return(JS_TRUE);
}

bool js_weap_zoom_get_active(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(weap->zoom.mode!=zoom_mode_off);
	
	return(JS_TRUE);
}

bool js_weap_zoom_get_fovMinimum(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->zoom.fov_min);
	
	return(JS_TRUE);
}

bool js_weap_zoom_get_fovMaximum(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->zoom.fov_max);
	
	return(JS_TRUE);
}

bool js_weap_zoom_get_fovSteps(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->zoom.step_count);
	
	return(JS_TRUE);
}

bool js_weap_zoom_get_turnFactor(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->zoom.turn_factor);
	
	return(JS_TRUE);
}

bool js_weap_zoom_get_crawlTurnFactor(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->zoom.crawl_turn_factor);
	
	return(JS_TRUE);
}

bool js_weap_zoom_get_lookFactor(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(weap->zoom.look_factor);
	
	return(JS_TRUE);
}

bool js_weap_zoom_get_maskName(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(weap->zoom.mask_name);
	
	return(JS_TRUE);
}

bool js_weap_zoom_get_showWeapon(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(weap->zoom.show_weapon);
	
	return(JS_TRUE);
}

bool js_weap_zoom_get_tick(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->zoom.tick);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_zoom_set_on(jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.on=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_weap_zoom_set_fovMinimum(jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.fov_min=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

bool js_weap_zoom_set_fovMaximum(jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.fov_max=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

bool js_weap_zoom_set_fovSteps(jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.step_count=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

bool js_weap_zoom_set_turnFactor(jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.turn_factor=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

bool js_weap_zoom_set_crawlTurnFactor(jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.crawl_turn_factor=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

bool js_weap_zoom_set_lookFactor(jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.look_factor=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

bool js_weap_zoom_set_maskName(jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	script_value_to_string(*vp,weap->zoom.mask_name,name_str_len);
	weapon_attach_zoom_mask(weap);
	
	return(JS_TRUE);
}

bool js_weap_zoom_set_showWeapon(jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.show_weapon=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_weap_zoom_set_tick(jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->zoom.tick=script_value_to_int(*vp);
	
	return(JS_TRUE);
}




