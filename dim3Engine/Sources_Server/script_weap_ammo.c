/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.ammo object

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
#include "weapons.h"

extern js_type				js;

JSValueRef js_weap_ammo_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_ammo_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_ammo_get_ammo(JSContextRef cx);
JSValueRef js_weap_ammo_get_clip(JSContextRef cx);
JSValueRef js_weap_ammo_get_count(JSContextRef cx);
JSValueRef js_weap_ammo_get_initCount(JSContextRef cx);
JSValueRef js_weap_ammo_get_maxCount(JSContextRef cx);
JSValueRef js_weap_ammo_get_clipCount(JSContextRef cx);
JSValueRef js_weap_ammo_get_initClipCount(JSContextRef cx);
JSValueRef js_weap_ammo_get_maxClipCount(JSContextRef cx);
JSValueRef js_weap_ammo_get_lastReloadTick(JSContextRef cx);
void js_weap_ammo_set_ammo(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_ammo_set_clip(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_ammo_set_count(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_ammo_set_initCount(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_ammo_set_maxCount(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_ammo_set_clipCount(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_ammo_set_initClipCount(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_ammo_set_maxClipCount(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_ammo_use_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_ammo_add_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_ammo_change_clip_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		weap_ammo_props[]={
							{"ammo",				js_weap_ammo_get_ammo,					js_weap_ammo_set_ammo,				kJSPropertyAttributeDontDelete},
							{"clip",				js_weap_ammo_get_clip,					js_weap_ammo_set_clip,				kJSPropertyAttributeDontDelete},
							{"count",				js_weap_ammo_get_count,					js_weap_ammo_set_count,				kJSPropertyAttributeDontDelete},
							{"initCount",			js_weap_ammo_get_initCount,				js_weap_ammo_set_initCount,			kJSPropertyAttributeDontDelete},
							{"maxCount",			js_weap_ammo_get_maxCount,				js_weap_ammo_set_maxCount,			kJSPropertyAttributeDontDelete},
							{"clipCount",			js_weap_ammo_get_clipCount,				js_weap_ammo_set_clipCount,			kJSPropertyAttributeDontDelete},
							{"initClipCount",		js_weap_ammo_get_initClipCount,			js_weap_ammo_set_initClipCount,		kJSPropertyAttributeDontDelete},
							{"maxClipCount",		js_weap_ammo_get_maxClipCount,			js_weap_ammo_set_maxClipCount,		kJSPropertyAttributeDontDelete},
							{"lastReloadTick",		js_weap_ammo_get_lastReloadTick,		NULL,								kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	weap_ammo_functions[]={
							{"useAmmo",				js_weap_ammo_use_ammo_func,				kJSPropertyAttributeDontDelete},
							{"addAmmo",				js_weap_ammo_add_ammo_func,				kJSPropertyAttributeDontDelete},
							{"changeClip",			js_weap_ammo_change_clip_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			weap_ammo_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_ammo_object(void)
{
	weap_ammo_class=script_create_class("weap_ammo_class",weap_ammo_props,weap_ammo_functions);
}

void script_free_weap_ammo_object(void)
{
	script_free_class(weap_ammo_class);
}

JSObjectRef script_add_weap_ammo_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,weap_ammo_class,"ammo",weap_ammo_props,weap_ammo_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_weap_ammo_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,weap_ammo_props));
}

bool js_weap_ammo_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,weap_ammo_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_ammo_get_ammo(JSContextRef cx)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	
	return(script_bool_to_value(cx,ammo->use_ammo));
}

JSValueRef js_weap_ammo_get_clip(JSContextRef cx)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	return(script_bool_to_value(cx,ammo->use_clips));
}

JSValueRef js_weap_ammo_get_count(JSContextRef cx)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	if (ammo->use_ammo) {
		if (weap->dual.in_dual) {
			return(script_int_to_value(cx,ammo->count_dual));
		}
		else {
			return(script_int_to_value(cx,ammo->count));
		}
	}
	
	return(script_int_to_value(cx,0));
}

JSValueRef js_weap_ammo_get_initCount(JSContextRef cx)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	if (ammo->use_ammo) return(script_int_to_value(cx,ammo->init_count));
	
	return(script_int_to_value(cx,0));
}

JSValueRef js_weap_ammo_get_maxCount(JSContextRef cx)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	if (ammo->use_ammo) return(script_int_to_value(cx,ammo->max_count));
	
	return(script_int_to_value(cx,0));
}

JSValueRef js_weap_ammo_get_clipCount(JSContextRef cx)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	if ((ammo->use_ammo) && (ammo->use_clips)) return(script_int_to_value(cx,ammo->clip_count));
	
	return(script_int_to_value(cx,0));
}

JSValueRef js_weap_ammo_get_initClipCount(JSContextRef cx)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	if ((ammo->use_ammo) && (ammo->use_clips)) return(script_int_to_value(cx,ammo->init_clip_count));

	return(script_int_to_value(cx,0));
}

JSValueRef js_weap_ammo_get_maxClipCount(JSContextRef cx)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	if ((ammo->use_ammo) && (ammo->use_clips)) return(script_int_to_value(cx,ammo->max_clip_count));

	return(script_int_to_value(cx,0));
}

JSValueRef js_weap_ammo_get_lastReloadTick(JSContextRef cx)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	
	if (weap->dual.in_dual) return(script_int_to_value(cx,ammo->last_reload_dual_tick));

	return(script_int_to_value(cx,ammo->last_reload_tick));
}

/* =======================================================

      Setters
      
======================================================= */

void js_weap_ammo_set_ammo(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	
	ammo->use_ammo=script_value_to_bool(cx,vp);
}

void js_weap_ammo_set_clip(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	ammo->use_clips=script_value_to_bool(cx,vp);
}

void js_weap_ammo_set_count(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	if (weap->dual.in_dual) {
		ammo->count_dual=script_value_to_int(cx,vp);
	}
	else {
		ammo->count=script_value_to_int(cx,vp);
	}
}

void js_weap_ammo_set_initCount(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	ammo->init_count=script_value_to_int(cx,vp);
}

void js_weap_ammo_set_maxCount(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	ammo->max_count=script_value_to_int(cx,vp);
}

void js_weap_ammo_set_clipCount(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	ammo->clip_count=script_value_to_int(cx,vp);
}

void js_weap_ammo_set_initClipCount(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	ammo->init_clip_count=script_value_to_int(cx,vp);
}

void js_weap_ammo_set_maxClipCount(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	weap_ammo_type	*ammo;

	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	
	ammo->max_clip_count=script_value_to_int(cx,vp);
}

/* =======================================================

      Ammo Functions
      
======================================================= */

JSValueRef js_weap_ammo_use_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				count;
	JSValueRef		vp;
	weapon_type		*weap;
	weap_ammo_type	*ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	
	count=script_value_to_int(cx,argv[0]);
	
	vp=script_bool_to_value(cx,FALSE);
	
	if (weap->dual.in_dual) {
		if (ammo->count_dual>=count) {
			ammo->count_dual-=count;
			vp=script_bool_to_value(cx,TRUE);
		}
	}
	else {
		if (ammo->count>=count) {
			ammo->count-=count;
			vp=script_bool_to_value(cx,TRUE);
		}
	}
	
	return(vp);
}

JSValueRef js_weap_ammo_add_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				add;
	JSValueRef		vp;
	weapon_type		*weap;
	weap_ammo_type	*ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;
	
	add=script_value_to_int(cx,argv[0]);
	
	vp=script_bool_to_value(cx,TRUE);

	if (weap->dual.in_dual) {
		if (ammo->count_dual==ammo->max_count) {
			vp=script_bool_to_value(cx,FALSE);
		}
		else {
			ammo->count_dual+=add;
			if (ammo->count_dual>ammo->max_count) ammo->count_dual=ammo->max_count;
		}
	}
	else {
		if (ammo->count==ammo->max_count) {
			vp=script_bool_to_value(cx,FALSE);
		}
		else {
			ammo->count+=add;
			if (ammo->count>ammo->max_count) ammo->count=ammo->max_count;
		}
	}
	
	return(vp);
}

JSValueRef js_weap_ammo_change_clip_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	weapon_type		*weap;
	weap_ammo_type	*ammo;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	ammo=&weap->ammo;

	obj=object_find_uid(weap->obj_uid);
	
		// is this a clip based weapon and enough clips?
		
	if ((!ammo->use_clips) || (ammo->clip_count==0)) {
		return(script_bool_to_value(cx,FALSE));
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
	
	return(script_bool_to_value(cx,TRUE));
}


