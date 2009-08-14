/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.projectile object

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
#include "projectiles.h"

extern js_type			js;

JSValueRef js_weap_projectile_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_projectile_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_projectile_get_fireBoneTag(void);
JSValueRef js_weap_projectile_get_barrelBoneTag(void);
JSValueRef js_weap_projectile_get_firePoseName(void);
JSValueRef js_weap_projectile_get_objectFireBoneTag(void);
JSValueRef js_weap_projectile_get_objectFirePoseName(void);
JSValueRef js_weap_projectile_get_repeat_on(void);
JSValueRef js_weap_projectile_get_repeat_tick(void);
void js_weap_projectile_set_fireBoneTag(JSValueRef vp,JSValueRef *exception);
void js_weap_projectile_set_barrelBoneTag(JSValueRef vp,JSValueRef *exception);
void js_weap_projectile_set_firePoseName(JSValueRef vp,JSValueRef *exception);
void js_weap_projectile_set_objectFireBoneTag(JSValueRef vp,JSValueRef *exception);
void js_weap_projectile_set_objectFirePoseName(JSValueRef vp,JSValueRef *exception);
void js_weap_projectile_set_repeat_on(JSValueRef vp,JSValueRef *exception);
void js_weap_projectile_set_repeat_tick(JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_projectile_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_weapon_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_weapon_bone_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_weapon_bone_multi_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_weapon_bone_offset_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_object_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_object_bone_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_object_bone_multi_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_object_bone_offset_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_barrel_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_barrel_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_barrel_multi_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_barrel_offset_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_center_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_center_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_center_multi_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_center_offset_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_property	weap_projectile_props[]={
							{"fireBoneTag",				js_weap_projectile_get_fireBoneTag,				js_weap_projectile_set_fireBoneTag},
							{"barrelBoneTag",			js_weap_projectile_get_barrelBoneTag,			js_weap_projectile_set_barrelBoneTag},
							{"firePoseName",			js_weap_projectile_get_firePoseName,			js_weap_projectile_set_firePoseName},
							{"objectFireBoneTag",		js_weap_projectile_get_objectFireBoneTag,		js_weap_projectile_set_objectFireBoneTag},
							{"objectFirePoseName",		js_weap_projectile_get_objectFirePoseName,		js_weap_projectile_set_objectFirePoseName},
							{"repeatOn",				js_weap_projectile_get_repeat_on,				js_weap_projectile_set_repeat_on},
							{"repeatTick",				js_weap_projectile_get_repeat_tick,				js_weap_projectile_set_repeat_tick},
							{0}};

script_js_function	weap_projectile_functions[]={
							{"add",										js_weap_projectile_add_func,											2},
							{"spawnFromWeaponBone",						js_weap_projectile_spawn_from_weapon_bone_func,							1},
							{"spawnFromWeaponBoneSlop",					js_weap_projectile_spawn_from_weapon_bone_slop_func,					2},
							{"spawnFromWeaponBoneMultiSlop",			js_weap_projectile_spawn_from_weapon_bone_multi_slop_func,				3},
							{"spawnFromWeaponBoneOffsetAngle",			js_weap_projectile_spawn_from_weapon_bone_offset_angle_func,			5},
							{"spawnFromObjectBone",						js_weap_projectile_spawn_from_object_bone_func,							1},
							{"spawnFromObjectBoneSlop",					js_weap_projectile_spawn_from_object_bone_slop_func,					2},
							{"spawnFromObjectBoneMultiSlop",			js_weap_projectile_spawn_from_object_bone_multi_slop_func,				3},
							{"spawnFromObjectBoneOffsetAngle",			js_weap_projectile_spawn_from_object_bone_offset_angle_func,			5},
							{"spawnFromBarrel",							js_weap_projectile_spawn_from_barrel_func,								1},
							{"spawnFromBarrelSlop",						js_weap_projectile_spawn_from_barrel_slop_func,							2},
							{"spawnFromBarrelMultiSlop",				js_weap_projectile_spawn_from_barrel_multi_slop_func,					3},
							{"spawnFromBarrelOffsetAngle",				js_weap_projectile_spawn_from_barrel_offset_angle_func,					5},
							{"spawnFromCenter",							js_weap_projectile_spawn_from_center_func,								1},
							{"spawnFromCenterSlop",						js_weap_projectile_spawn_from_center_slop_func,							2},
							{"spawnFromCenterMultiSlop",				js_weap_projectile_spawn_from_center_multi_slop_func,					3},
							{"spawnFromCenterOffsetAngle",				js_weap_projectile_spawn_from_center_offset_angle_func,					5},
							{0}};							

JSClassRef			weap_projectile_class;

/* =======================================================

      Create Projectile
      
======================================================= */

void script_init_weap_projectile_object(void)
{
	weap_projectile_class=script_create_class("weap_projectile_class",js_weap_projectile_get_property,js_weap_projectile_set_property);
}

void script_free_weap_projectile_object(void)
{
	script_free_class(weap_projectile_class);
}

JSObjectRef script_add_weap_projectile_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,weap_projectile_class,"projectile",weap_projectile_props,weap_projectile_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_weap_projectile_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,weap_projectile_props));
}

bool js_weap_projectile_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,weap_projectile_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_projectile_get_fireBoneTag(void)
{
	char			str[32];
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	
	model_tag_to_text(weap->proj.fire_bone_tag,str);
	return(script_string_to_value(str));
}

JSValueRef js_weap_projectile_get_barrelBoneTag(void)
{
	char			str[32];
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);

	model_tag_to_text(weap->proj.barrel_bone_tag,str);
	return(script_string_to_value(str));
}

JSValueRef js_weap_projectile_get_firePoseName(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_string_to_value(weap->proj.fire_pose_name));
}

JSValueRef js_weap_projectile_get_objectFireBoneTag(void)
{
	char			str[32];
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);

	model_tag_to_text(weap->proj.object_fire_bone_tag,str);
	return(script_string_to_value(str));
}

JSValueRef js_weap_projectile_get_objectFirePoseName(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_string_to_value(weap->proj.object_fire_pose_name));
}

JSValueRef js_weap_projectile_get_repeat_on(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(weap->proj.repeat_on));
}

JSValueRef js_weap_projectile_get_repeat_tick(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_int_to_value(weap->proj.repeat_tick));
}

/* =======================================================

      Setters
      
======================================================= */

void js_weap_projectile_set_fireBoneTag(JSValueRef vp,JSValueRef *exception)
{
	char			str[32];
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);

	script_value_to_string(vp,str,32);
	weap->proj.fire_bone_tag=text_to_model_tag(str);
}

void js_weap_projectile_set_barrelBoneTag(JSValueRef vp,JSValueRef *exception)
{
	char			str[32];
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);

	script_value_to_string(vp,str,32);
	weap->proj.barrel_bone_tag=text_to_model_tag(str);
}

void js_weap_projectile_set_firePoseName(JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	script_value_to_string(vp,weap->proj.fire_pose_name,name_str_len);
}

void js_weap_projectile_set_objectFireBoneTag(JSValueRef vp,JSValueRef *exception)
{
	char			str[32];
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);

	script_value_to_string(vp,str,32);
	weap->proj.object_fire_bone_tag=text_to_model_tag(str);
}

void js_weap_projectile_set_objectFirePoseName(JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	script_value_to_string(vp,weap->proj.object_fire_pose_name,name_str_len);
}

void js_weap_projectile_set_repeat_on(JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->proj.repeat_on=script_value_to_bool(vp);
}

void js_weap_projectile_set_repeat_tick(JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->proj.repeat_tick=script_value_to_int(vp);
}

/* =======================================================

      Projectile Add Function
      
======================================================= */

JSValueRef js_weap_projectile_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char				str[name_str_len];
    obj_type			*obj;
	weapon_type			*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],str,name_str_len);

	return(script_bool_to_value(proj_setup_add(obj,weap,str)));
}

/* =======================================================

      Projectile Spawn from Weapon
      
======================================================= */

JSValueRef js_weap_projectile_spawn_from_weapon_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	if (!weapon_script_projectile_spawn_weapon_model(js.time.current_tick,obj,weap,proj_name,1,0.0f,NULL,err_str)) {
		*exception=script_create_exception(err_str);
	}

	return(script_null_to_value());
}

JSValueRef js_weap_projectile_spawn_from_weapon_bone_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	slop=script_value_to_float(argv[1]);

	if (!weapon_script_projectile_spawn_weapon_model(js.time.current_tick,obj,weap,proj_name,1,slop,NULL,err_str)) {
		*exception=script_create_exception(err_str);
	}

	return(script_null_to_value());
}

JSValueRef js_weap_projectile_spawn_from_weapon_bone_multi_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int						count;
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	count=script_value_to_int(argv[1]);
	slop=script_value_to_float(argv[2]);

	if (!weapon_script_projectile_spawn_weapon_model(js.time.current_tick,obj,weap,proj_name,count,slop,NULL,err_str)) {
		*exception=script_create_exception(err_str);
	}

	return(script_null_to_value());
}

JSValueRef js_weap_projectile_spawn_from_weapon_bone_offset_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
	d3ang					off_ang;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	off_ang.x=script_value_to_float(argv[1]);
	off_ang.z=script_value_to_float(argv[2]);
	off_ang.y=script_value_to_float(argv[3]);

	if (!weapon_script_projectile_spawn_weapon_model(js.time.current_tick,obj,weap,proj_name,1,0.0f,&off_ang,err_str)) {
		*exception=script_create_exception(err_str);
	}

	return(script_null_to_value());
}

/* =======================================================

      Projectile Spawn from Object
      
======================================================= */

JSValueRef js_weap_projectile_spawn_from_object_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	if (!weapon_script_projectile_spawn_object_model(js.time.current_tick,obj,weap,proj_name,1,0.0f,NULL,err_str)) {
		*exception=script_create_exception(err_str);
	}

	return(script_null_to_value());
}

JSValueRef js_weap_projectile_spawn_from_object_bone_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	slop=script_value_to_float(argv[1]);

	if (!weapon_script_projectile_spawn_object_model(js.time.current_tick,obj,weap,proj_name,1,slop,NULL,err_str)) {
		*exception=script_create_exception(err_str);
	}

	return(script_null_to_value());
}

JSValueRef js_weap_projectile_spawn_from_object_bone_multi_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int						count;
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	count=script_value_to_int(argv[1]);
	slop=script_value_to_float(argv[2]);

	if (!weapon_script_projectile_spawn_object_model(js.time.current_tick,obj,weap,proj_name,count,slop,NULL,err_str)) {
		*exception=script_create_exception(err_str);
	}

	return(script_null_to_value());
}

JSValueRef js_weap_projectile_spawn_from_object_bone_offset_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
	d3ang					off_ang;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	off_ang.x=script_value_to_float(argv[1]);
	off_ang.z=script_value_to_float(argv[2]);
	off_ang.y=script_value_to_float(argv[3]);

	if (!weapon_script_projectile_spawn_object_model(js.time.current_tick,obj,weap,proj_name,1,0.0f,&off_ang,err_str)) {
		*exception=script_create_exception(err_str);
	}

	return(script_null_to_value());
}

/* =======================================================

      Projectile Spawn from Barrel
      
======================================================= */

JSValueRef js_weap_projectile_spawn_from_barrel_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	if (!weapon_script_projectile_spawn_weapon_barrel(js.time.current_tick,obj,weap,proj_name,1,0.0f,NULL,err_str)) {
		*exception=script_create_exception(err_str);
	}

	return(script_null_to_value());
}

JSValueRef js_weap_projectile_spawn_from_barrel_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	slop=script_value_to_float(argv[1]);

	if (!weapon_script_projectile_spawn_weapon_barrel(js.time.current_tick,obj,weap,proj_name,1,slop,NULL,err_str)) {
		*exception=script_create_exception(err_str);
	}

	return(script_null_to_value());
}

JSValueRef js_weap_projectile_spawn_from_barrel_multi_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int						count;
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	count=script_value_to_int(argv[1]);
	slop=script_value_to_float(argv[2]);

	if (!weapon_script_projectile_spawn_weapon_barrel(js.time.current_tick,obj,weap,proj_name,count,slop,NULL,err_str)) {
		*exception=script_create_exception(err_str);
	}

	return(script_null_to_value());
}

JSValueRef js_weap_projectile_spawn_from_barrel_offset_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
	d3ang					off_ang;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	off_ang.x=script_value_to_float(argv[1]);
	off_ang.z=script_value_to_float(argv[2]);
	off_ang.y=script_value_to_float(argv[3]);

	if (!weapon_script_projectile_spawn_weapon_barrel(js.time.current_tick,obj,weap,proj_name,1,0.0f,&off_ang,err_str)) {
		*exception=script_create_exception(err_str);
	}

	return(script_null_to_value());
}

/* =======================================================

      Projectile Spawn from Object To Center
      
======================================================= */

JSValueRef js_weap_projectile_spawn_from_center_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	if (!weapon_script_projectile_spawn_center(js.time.current_tick,obj,weap,proj_name,1,0.0f,NULL,err_str)) {
		*exception=script_create_exception(err_str);
	}

	return(script_null_to_value());
}

JSValueRef js_weap_projectile_spawn_from_center_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	slop=script_value_to_float(argv[1]);

	if (!weapon_script_projectile_spawn_center(js.time.current_tick,obj,weap,proj_name,1,slop,NULL,err_str)) {
		*exception=script_create_exception(err_str);
	}

	return(script_null_to_value());
}

JSValueRef js_weap_projectile_spawn_from_center_multi_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int						count;
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	count=script_value_to_int(argv[1]);
	slop=script_value_to_float(argv[2]);

	if (!weapon_script_projectile_spawn_center(js.time.current_tick,obj,weap,proj_name,count,slop,NULL,err_str)) {
		*exception=script_create_exception(err_str);
	}

	return(script_null_to_value());
}

JSValueRef js_weap_projectile_spawn_from_center_offset_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
	d3ang					off_ang;
    obj_type				*obj;
	weapon_type				*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);

	script_value_to_string(argv[0],proj_name,name_str_len);

	off_ang.x=script_value_to_float(argv[1]);
	off_ang.z=script_value_to_float(argv[2]);
	off_ang.y=script_value_to_float(argv[3]);

	if (!weapon_script_projectile_spawn_center(js.time.current_tick,obj,weap,proj_name,1,0.0f,&off_ang,err_str)) {
		*exception=script_create_exception(err_str);
	}

	return(script_null_to_value());
}


