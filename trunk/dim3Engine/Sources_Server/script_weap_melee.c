/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.melee object

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

extern js_type			js;

JSValueRef js_weap_melee_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_melee_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_melee_get_strikeBoneTag(JSContextRef cx);
JSValueRef js_weap_melee_get_strikePoseName(JSContextRef cx);
JSValueRef js_weap_melee_get_objectStrikeBoneTag(JSContextRef cx);
JSValueRef js_weap_melee_get_objectStrikePoseName(JSContextRef cx);
JSValueRef js_weap_melee_get_radius(JSContextRef cx);
JSValueRef js_weap_melee_get_distance(JSContextRef cx);
JSValueRef js_weap_melee_get_damage(JSContextRef cx);
JSValueRef js_weap_melee_get_force(JSContextRef cx);
JSValueRef js_weap_melee_get_fallOff(JSContextRef cx);
void js_weap_melee_set_strikeBoneTag(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_melee_set_strikePoseName(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_melee_set_objectStrikeBoneTag(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_melee_set_objectStrikePoseName(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_melee_set_radius(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_melee_set_distance(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_melee_set_damage(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_melee_set_force(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_melee_set_fallOff(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_melee_spawn_from_weapon_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_melee_spawn_from_object_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_property	weap_melee_props[]={
							{"strikeBoneTag",			js_weap_melee_get_strikeBoneTag,			js_weap_melee_set_strikeBoneTag},
							{"strikePoseName",			js_weap_melee_get_strikePoseName,			js_weap_melee_set_strikePoseName},
							{"objectStrikeBoneTag",		js_weap_melee_get_objectStrikeBoneTag,		js_weap_melee_set_objectStrikeBoneTag},
							{"objectStrikePoseName",	js_weap_melee_get_objectStrikePoseName,		js_weap_melee_set_objectStrikePoseName},
							{"radius",					js_weap_melee_get_radius,					js_weap_melee_set_radius},
							{"distance",				js_weap_melee_get_distance,					js_weap_melee_set_distance},
							{"damage",					js_weap_melee_get_damage,					js_weap_melee_set_damage},
							{"force",					js_weap_melee_get_force,					js_weap_melee_set_force},
							{"fallOff",					js_weap_melee_get_fallOff,					js_weap_melee_set_fallOff},
							{0}};

script_js_function	weap_melee_functions[]={
							{"spawnFromWeaponBone",		js_weap_melee_spawn_from_weapon_bone_func,		0},
							{"spawnFromObjectBone",		js_weap_melee_spawn_from_object_bone_func,		0},
							{0}};

JSClassRef			weap_melee_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_melee_object(void)
{
	weap_melee_class=script_create_class("weap_melee_class",js_weap_melee_get_property,js_weap_melee_set_property);
}

void script_free_weap_melee_object(void)
{
	script_free_class(weap_melee_class);
}

JSObjectRef script_add_weap_melee_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,weap_melee_class,"melee",weap_melee_props,weap_melee_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_weap_melee_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,weap_melee_props));
}

bool js_weap_melee_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,weap_melee_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_melee_get_strikeBoneTag(JSContextRef cx)
{
	char			str[32];
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	
	model_tag_to_text(weap->melee.strike_bone_tag,str);
	return(script_string_to_value(cx,str));
}

JSValueRef js_weap_melee_get_strikePoseName(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_string_to_value(cx,weap->melee.strike_pose_name));
}

JSValueRef js_weap_melee_get_objectStrikeBoneTag(JSContextRef cx)
{
	char			str[32];
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);

	model_tag_to_text(weap->melee.object_strike_bone_tag,str);
	return(script_string_to_value(cx,str));
}

JSValueRef js_weap_melee_get_objectStrikePoseName(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_string_to_value(cx,weap->melee.object_strike_pose_name));
}

JSValueRef js_weap_melee_get_radius(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,weap->melee.radius));
}

JSValueRef js_weap_melee_get_distance(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,weap->melee.distance));
}

JSValueRef js_weap_melee_get_damage(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,weap->melee.damage));
}

JSValueRef js_weap_melee_get_force(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,weap->melee.force));
}

JSValueRef js_weap_melee_get_fallOff(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,weap->melee.fall_off));
}

/* =======================================================

      Setters
      
======================================================= */

void js_weap_melee_set_strikeBoneTag(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	char			str[32];
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	
	script_value_to_string(cx,vp,str,32);
	weap->melee.strike_bone_tag=text_to_model_tag(str);
}

void js_weap_melee_set_strikePoseName(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	script_value_to_string(cx,vp,weap->melee.strike_pose_name,name_str_len);
}

void js_weap_melee_set_objectStrikeBoneTag(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	char			str[32];
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);

	script_value_to_string(cx,vp,str,32);
	weap->melee.object_strike_bone_tag=text_to_model_tag(str);
}

void js_weap_melee_set_objectStrikePoseName(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	script_value_to_string(cx,vp,weap->melee.object_strike_pose_name,name_str_len);
}

void js_weap_melee_set_radius(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->melee.radius=script_value_to_int(cx,vp);
}

void js_weap_melee_set_distance(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->melee.distance=script_value_to_int(cx,vp);
}

void js_weap_melee_set_damage(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->melee.damage=script_value_to_int(cx,vp);
}

void js_weap_melee_set_force(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->melee.force=script_value_to_int(cx,vp);
}

void js_weap_melee_set_fallOff(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->melee.fall_off=script_value_to_bool(cx,vp);
}

/* =======================================================

      Spawn Melee
      
======================================================= */

JSValueRef js_weap_melee_spawn_from_weapon_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			err_str[256];
    obj_type		*obj;
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
    obj=object_find_uid(weap->obj_uid);

	if (!melee_script_spawn_weapon_model(js.time.current_tick,obj,weap,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_weap_melee_spawn_from_object_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			err_str[256];
    obj_type		*obj;
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
    obj=object_find_uid(weap->obj_uid);

	if (!melee_script_spawn_object_model(js.time.current_tick,obj,weap,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

