/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.melee object

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

extern js_type			js;

JSBool js_weap_melee_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_weap_melee_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
bool js_weap_melee_get_strikeBoneTag(jsval *vp);
bool js_weap_melee_get_strikePoseName(jsval *vp);
bool js_weap_melee_get_objectStrikeBoneTag(jsval *vp);
bool js_weap_melee_get_objectStrikePoseName(jsval *vp);
bool js_weap_melee_get_radius(jsval *vp);
bool js_weap_melee_get_distance(jsval *vp);
bool js_weap_melee_get_damage(jsval *vp);
bool js_weap_melee_get_force(jsval *vp);
bool js_weap_melee_get_fallOff(jsval *vp);
bool js_weap_melee_set_strikeBoneTag(jsval *vp);
bool js_weap_melee_set_strikePoseName(jsval *vp);
bool js_weap_melee_set_objectStrikeBoneTag(jsval *vp);
bool js_weap_melee_set_objectStrikePoseName(jsval *vp);
bool js_weap_melee_set_radius(jsval *vp);
bool js_weap_melee_set_distance(jsval *vp);
bool js_weap_melee_set_damage(jsval *vp);
bool js_weap_melee_set_force(jsval *vp);
bool js_weap_melee_set_fallOff(jsval *vp);
JSBool js_weap_melee_spawn_from_weapon_bone_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_weap_melee_spawn_from_object_bone_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

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

JSObject* script_add_weap_melee_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,weap_melee_class,"melee",weap_melee_props,weap_melee_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_weap_melee_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,weap_melee_props));
}

JSBool js_weap_melee_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,weap_melee_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_weap_melee_get_strikeBoneTag(jsval *vp)
{
	char			str[32];
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	
	model_tag_to_text(weap->melee.strike_bone_tag,str);
	*vp=script_string_to_value(str);

	return(JS_TRUE);
}

bool js_weap_melee_get_strikePoseName(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(weap->melee.strike_pose_name);

	return(JS_TRUE);
}

bool js_weap_melee_get_objectStrikeBoneTag(jsval *vp)
{
	char			str[32];
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);

	model_tag_to_text(weap->melee.object_strike_bone_tag,str);
	*vp=script_string_to_value(str);

	return(JS_TRUE);
}

bool js_weap_melee_get_objectStrikePoseName(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(weap->melee.object_strike_pose_name);

	return(JS_TRUE);
}

bool js_weap_melee_get_radius(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->melee.radius);

	return(JS_TRUE);
}

bool js_weap_melee_get_distance(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->melee.distance);

	return(JS_TRUE);
}

bool js_weap_melee_get_damage(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->melee.damage);

	return(JS_TRUE);
}

bool js_weap_melee_get_force(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->melee.force);

	return(JS_TRUE);
}

bool js_weap_melee_get_fallOff(jsval *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(weap->melee.fall_off);

	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_melee_set_strikeBoneTag(jsval *vp)
{
	char			str[32];
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	
	script_value_to_string(*vp,str,32);
	weap->melee.strike_bone_tag=text_to_model_tag(str);
	
	return(JS_TRUE);
}

bool js_weap_melee_set_strikePoseName(jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	script_value_to_string(*vp,weap->melee.strike_pose_name,name_str_len);
	
	return(JS_TRUE);
}

bool js_weap_melee_set_objectStrikeBoneTag(jsval *vp)
{
	char			str[32];
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);

	script_value_to_string(*vp,str,32);
	weap->melee.object_strike_bone_tag=text_to_model_tag(str);
	
	return(JS_TRUE);
}

bool js_weap_melee_set_objectStrikePoseName(jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	script_value_to_string(*vp,weap->melee.object_strike_pose_name,name_str_len);
	
	return(JS_TRUE);
}

bool js_weap_melee_set_radius(jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->melee.radius=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

bool js_weap_melee_set_distance(jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->melee.distance=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

bool js_weap_melee_set_damage(jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->melee.damage=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

bool js_weap_melee_set_force(jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->melee.force=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

bool js_weap_melee_set_fallOff(jsval *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->melee.fall_off=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

/* =======================================================

      Spawn Melee
      
======================================================= */

JSBool js_weap_melee_spawn_from_weapon_bone_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			err_str[256];
    obj_type		*obj;
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
    obj=object_find_uid(weap->obj_uid);

	if (!melee_script_spawn_weapon_model(js.time.current_tick,obj,weap,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

JSBool js_weap_melee_spawn_from_object_bone_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			err_str[256];
    obj_type		*obj;
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
    obj=object_find_uid(weap->obj_uid);

	if (!melee_script_spawn_object_model(js.time.current_tick,obj,weap,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}

