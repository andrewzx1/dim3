/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.ammo object

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
#include "weapons.h"

extern js_type				js;

JSBool js_weap_ammo_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_get_ammo(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_get_clip(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_get_count(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_get_initCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_get_maxCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_get_clipCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_get_initClipCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_get_maxClipCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_get_lastReloadTick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_set_ammo(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_set_clip(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_set_count(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_set_initCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_set_maxCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_set_clipCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_set_initClipCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_set_maxClipCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_ammo_use_ammo_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_ammo_add_ammo_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_ammo_change_clip_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_property	weap_ammo_props[]={
							{"ammo",				js_weap_ammo_get_ammo,					js_weap_ammo_set_ammo},
							{"clip",				js_weap_ammo_get_clip,					js_weap_ammo_set_clip},
							{"count",				js_weap_ammo_get_count,					js_weap_ammo_set_count},
							{"initCount",			js_weap_ammo_get_initCount,				js_weap_ammo_set_initCount},
							{"maxCount",			js_weap_ammo_get_maxCount,				js_weap_ammo_set_maxCount},
							{"clipCount",			js_weap_ammo_get_clipCount,				js_weap_ammo_set_clipCount},
							{"initClipCount",		js_weap_ammo_get_initClipCount,			js_weap_ammo_set_initClipCount},
							{"maxClipCount",		js_weap_ammo_get_maxClipCount,			js_weap_ammo_set_maxClipCount},
							{"lastReloadTick",		js_weap_ammo_get_lastReloadTick,		NULL},
							{0}};
							
script_js_function	weap_ammo_functions[]={
							{"useAmmo",				js_weap_ammo_use_ammo_func,				1},
							{"addAmmo",				js_weap_ammo_add_ammo_func,				1},
							{"changeClip",			js_weap_ammo_change_clip_func,			0},
							{0}};

JSClass				*weap_ammo_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_ammo_object(void)
{
	weap_ammo_class=script_create_class("weap_ammo_class",js_weap_ammo_get_property,js_weap_ammo_set_property);
}

void script_free_weap_ammo_object(void)
{
	script_free_class(weap_ammo_class);
}

void script_add_weap_ammo_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,weap_ammo_class,"ammo",weap_ammo_props,weap_ammo_functions);
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_weap_ammo_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,weap_ammo_props));
}

JSBool js_weap_ammo_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,weap_ammo_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_weap_ammo_get_ammo(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	
	*vp=script_bool_to_value(ammo->use_ammo);
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_get_clip(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	*vp=script_bool_to_value(ammo->use_clips);
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_get_count(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	if (ammo->use_ammo) {
		if (weap->dual.in_dual) {
			*vp=INT_TO_JSVAL(ammo->count_dual);
		}
		else {
			*vp=INT_TO_JSVAL(ammo->count);
		}
	}
	else {
		*vp=INT_TO_JSVAL(0);
	}
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_get_initCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	if (ammo->use_ammo) {
		*vp=INT_TO_JSVAL(ammo->init_count);
	}
	else {
		*vp=INT_TO_JSVAL(0);
	}
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_get_maxCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	if (ammo->use_ammo) {
		*vp=INT_TO_JSVAL(ammo->max_count);
	}
	else {
		*vp=INT_TO_JSVAL(0);
	}
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_get_clipCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	if ((ammo->use_ammo) && (ammo->use_clips)) {
		*vp=INT_TO_JSVAL(ammo->clip_count);
	}
	else {
		*vp=INT_TO_JSVAL(0);
	}
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_get_initClipCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	if ((ammo->use_ammo) && (ammo->use_clips)) {
		*vp=INT_TO_JSVAL(ammo->init_clip_count);
	}
	else {
		*vp=INT_TO_JSVAL(0);
	}
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_get_maxClipCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	if ((ammo->use_ammo) && (ammo->use_clips)) {
		*vp=INT_TO_JSVAL(ammo->max_clip_count);
	}
	else {
		*vp=INT_TO_JSVAL(0);
	}
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_get_lastReloadTick(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	
	if (weap->dual.in_dual) {
		*vp=INT_TO_JSVAL(ammo->last_reload_dual_tick);
	}
	else {
		*vp=INT_TO_JSVAL(ammo->last_reload_tick);
	}
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_weap_ammo_set_ammo(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	
	ammo->use_ammo=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_set_clip(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	ammo->use_clips=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_set_count(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	if (weap->dual.in_dual) {
		ammo->count_dual=JSVAL_TO_INT(*vp);
	}
	else {
		ammo->count=JSVAL_TO_INT(*vp);
	}
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_set_initCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	ammo->init_count=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_set_maxCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	ammo->max_count=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_set_clipCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	ammo->clip_count=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_set_initClipCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	ammo->init_clip_count=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_set_maxClipCount(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	
	ammo->max_clip_count=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

/* =======================================================

      Ammo Functions
      
======================================================= */

JSBool js_weap_ammo_use_ammo_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				count;
	weapon_type		*weap;
	weap_ammo_type	*ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	
	count=JSVAL_TO_INT(argv[0]);
	
	*rval=script_bool_to_value(FALSE);
	
	if (weap->dual.in_dual) {
		if (ammo->count_dual>=count) {
			ammo->count_dual-=count;
			*rval=script_bool_to_value(TRUE);
		}
	}
	else {
		if (ammo->count>=count) {
			ammo->count-=count;
			*rval=script_bool_to_value(TRUE);
		}
	}
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_add_ammo_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				add;
	weapon_type		*weap;
	weap_ammo_type	*ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	
	add=JSVAL_TO_INT(argv[0]);
	
	*rval=script_bool_to_value(TRUE);

	if (weap->dual.in_dual) {
		if (ammo->count_dual==ammo->max_count) {
			*rval=script_bool_to_value(FALSE);
		}
		else {
			ammo->count_dual+=add;
			if (ammo->count_dual>ammo->max_count) ammo->count_dual=ammo->max_count;
		}
	}
	else {
		if (ammo->count==ammo->max_count) {
			*rval=script_bool_to_value(FALSE);
		}
		else {
			ammo->count+=add;
			if (ammo->count>ammo->max_count) ammo->count=ammo->max_count;
		}
	}
	
	return(JS_TRUE);
}

JSBool js_weap_ammo_change_clip_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	weapon_type		*weap;
	weap_ammo_type	*ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	obj=object_find_uid(weap->obj_uid);
	
		// is this a clip based weapon and enough clips?
		
	if ((!ammo->use_clips) || (ammo->clip_count==0)) {
		*rval=script_bool_to_value(FALSE);
		return(JS_TRUE);
	}
	
		// change the clip
		
	if (weap->dual.in_dual) {
		ammo->count_dual=ammo->max_count;
		ammo->last_reload_dual_tick=js.time.current_tick;
	}
	else {
		ammo->count=ammo->max_count;
		ammo->last_reload_tick=js.time.current_tick;
	}
	
	ammo->clip_count--;
	
		// alert object of clip change
		
	scripts_post_event_console(&obj->attach,sd_event_weapon_fire,sd_event_weapon_fire_clip_change,0);
	
	*rval=script_bool_to_value(TRUE);
	return(JS_TRUE);
}


