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

JSValueRef js_weap_projectile_get_fireBoneTag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_projectile_get_barrelBoneTag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_projectile_get_firePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_projectile_get_objectFireBoneTag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_projectile_get_objectFirePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_projectile_get_repeat_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_projectile_get_repeat_tick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_projectile_set_fireBoneTag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_projectile_set_barrelBoneTag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_projectile_set_firePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_projectile_set_objectFireBoneTag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_projectile_set_objectFirePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_projectile_set_repeat_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_weap_projectile_set_repeat_tick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_projectile_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_weapon_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_weapon_bone_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_weapon_bone_multi_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_weapon_bone_offset_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_object_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_object_bone_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_object_bone_multi_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_object_bone_offset_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_barrel_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_barrel_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_barrel_multi_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_barrel_offset_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_center_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_center_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_center_multi_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_weap_projectile_spawn_from_center_offset_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		weap_projectile_props[]={
							{"fireBoneTag",				js_weap_projectile_get_fireBoneTag,				js_weap_projectile_set_fireBoneTag,				kJSPropertyAttributeDontDelete},
							{"barrelBoneTag",			js_weap_projectile_get_barrelBoneTag,			js_weap_projectile_set_barrelBoneTag,			kJSPropertyAttributeDontDelete},
							{"firePoseName",			js_weap_projectile_get_firePoseName,			js_weap_projectile_set_firePoseName,			kJSPropertyAttributeDontDelete},
							{"objectFireBoneTag",		js_weap_projectile_get_objectFireBoneTag,		js_weap_projectile_set_objectFireBoneTag,		kJSPropertyAttributeDontDelete},
							{"objectFirePoseName",		js_weap_projectile_get_objectFirePoseName,		js_weap_projectile_set_objectFirePoseName,		kJSPropertyAttributeDontDelete},
							{"repeatOn",				js_weap_projectile_get_repeat_on,				js_weap_projectile_set_repeat_on,				kJSPropertyAttributeDontDelete},
							{"repeatTick",				js_weap_projectile_get_repeat_tick,				js_weap_projectile_set_repeat_tick,				kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	weap_projectile_functions[]={
							{"add",										js_weap_projectile_add_func,											kJSPropertyAttributeDontDelete},
							{"spawnFromWeaponBone",						js_weap_projectile_spawn_from_weapon_bone_func,							kJSPropertyAttributeDontDelete},
							{"spawnFromWeaponBoneSlop",					js_weap_projectile_spawn_from_weapon_bone_slop_func,					kJSPropertyAttributeDontDelete},
							{"spawnFromWeaponBoneMultiSlop",			js_weap_projectile_spawn_from_weapon_bone_multi_slop_func,				kJSPropertyAttributeDontDelete},
							{"spawnFromWeaponBoneOffsetAngle",			js_weap_projectile_spawn_from_weapon_bone_offset_angle_func,			kJSPropertyAttributeDontDelete},
							{"spawnFromObjectBone",						js_weap_projectile_spawn_from_object_bone_func,							kJSPropertyAttributeDontDelete},
							{"spawnFromObjectBoneSlop",					js_weap_projectile_spawn_from_object_bone_slop_func,					kJSPropertyAttributeDontDelete},
							{"spawnFromObjectBoneMultiSlop",			js_weap_projectile_spawn_from_object_bone_multi_slop_func,				kJSPropertyAttributeDontDelete},
							{"spawnFromObjectBoneOffsetAngle",			js_weap_projectile_spawn_from_object_bone_offset_angle_func,			kJSPropertyAttributeDontDelete},
							{"spawnFromBarrel",							js_weap_projectile_spawn_from_barrel_func,								kJSPropertyAttributeDontDelete},
							{"spawnFromBarrelSlop",						js_weap_projectile_spawn_from_barrel_slop_func,							kJSPropertyAttributeDontDelete},
							{"spawnFromBarrelMultiSlop",				js_weap_projectile_spawn_from_barrel_multi_slop_func,					kJSPropertyAttributeDontDelete},
							{"spawnFromBarrelOffsetAngle",				js_weap_projectile_spawn_from_barrel_offset_angle_func,					kJSPropertyAttributeDontDelete},
							{"spawnFromCenter",							js_weap_projectile_spawn_from_center_func,								kJSPropertyAttributeDontDelete},
							{"spawnFromCenterSlop",						js_weap_projectile_spawn_from_center_slop_func,							kJSPropertyAttributeDontDelete},
							{"spawnFromCenterMultiSlop",				js_weap_projectile_spawn_from_center_multi_slop_func,					kJSPropertyAttributeDontDelete},
							{"spawnFromCenterOffsetAngle",				js_weap_projectile_spawn_from_center_offset_angle_func,					kJSPropertyAttributeDontDelete},
							{0,0,0}};							

JSClassRef			weap_projectile_class;

/* =======================================================

      Create Projectile
      
======================================================= */

void script_init_weap_projectile_object(void)
{
	weap_projectile_class=script_create_class("weap_projectile_class",weap_projectile_props,weap_projectile_functions);
}

void script_free_weap_projectile_object(void)
{
	script_free_class(weap_projectile_class);
}

JSObjectRef script_add_weap_projectile_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,weap_projectile_class,"projectile"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_projectile_get_fireBoneTag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	char			str[32];
	weapon_type		*weap;

	weap=weapon_script_lookup();
	
	model_tag_to_text(weap->proj.fire_bone_tag,str);
	return(script_string_to_value(cx,str));
}

JSValueRef js_weap_projectile_get_barrelBoneTag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	char			str[32];
	weapon_type		*weap;

	weap=weapon_script_lookup();

	model_tag_to_text(weap->proj.barrel_bone_tag,str);
	return(script_string_to_value(cx,str));
}

JSValueRef js_weap_projectile_get_firePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_script_lookup();
	return(script_string_to_value(cx,weap->proj.fire_pose_name));
}

JSValueRef js_weap_projectile_get_objectFireBoneTag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	char			str[32];
	weapon_type		*weap;

	weap=weapon_script_lookup();

	model_tag_to_text(weap->proj.object_fire_bone_tag,str);
	return(script_string_to_value(cx,str));
}

JSValueRef js_weap_projectile_get_objectFirePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_script_lookup();
	return(script_string_to_value(cx,weap->proj.object_fire_pose_name));
}

JSValueRef js_weap_projectile_get_repeat_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_script_lookup();
	return(script_bool_to_value(cx,weap->proj.repeat_on));
}

JSValueRef js_weap_projectile_get_repeat_tick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_script_lookup();
	return(script_int_to_value(cx,weap->proj.repeat_tick));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_projectile_set_fireBoneTag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	char			str[32];
	weapon_type		*weap;
	
	weap=weapon_script_lookup();

	script_value_to_string(cx,vp,str,32);
	weap->proj.fire_bone_tag=text_to_model_tag(str);

	return(TRUE);
}

bool js_weap_projectile_set_barrelBoneTag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	char			str[32];
	weapon_type		*weap;
	
	weap=weapon_script_lookup();

	script_value_to_string(cx,vp,str,32);
	weap->proj.barrel_bone_tag=text_to_model_tag(str);

	return(TRUE);
}

bool js_weap_projectile_set_firePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_script_lookup();
	script_value_to_string(cx,vp,weap->proj.fire_pose_name,name_str_len);

	return(TRUE);
}

bool js_weap_projectile_set_objectFireBoneTag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	char			str[32];
	weapon_type		*weap;
	
	weap=weapon_script_lookup();

	script_value_to_string(cx,vp,str,32);
	weap->proj.object_fire_bone_tag=text_to_model_tag(str);

	return(TRUE);
}

bool js_weap_projectile_set_objectFirePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_script_lookup();
	script_value_to_string(cx,vp,weap->proj.object_fire_pose_name,name_str_len);

	return(TRUE);
}

bool js_weap_projectile_set_repeat_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_script_lookup();
	weap->proj.repeat_on=script_value_to_bool(cx,vp);

	return(TRUE);
}

bool js_weap_projectile_set_repeat_tick(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_script_lookup();
	weap->proj.repeat_tick=script_value_to_int(cx,vp);

	return(TRUE);
}

/* =======================================================

      Projectile Add Function
      
======================================================= */

JSValueRef js_weap_projectile_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char				str[name_str_len];
    obj_type			*obj;
	weapon_type			*weap;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	weap=weapon_script_lookup();

	script_value_to_string(cx,argv[0],str,name_str_len);

	return(script_bool_to_value(cx,proj_setup_create(obj,weap,str)));
}

/* =======================================================

      Projectile Spawn from Weapon
      
======================================================= */

JSValueRef js_weap_projectile_spawn_from_weapon_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
    obj_type				*obj;
	weapon_type				*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	weap=weapon_script_lookup();

	script_value_to_string(cx,argv[0],proj_name,name_str_len);

	if (!weapon_script_projectile_spawn_weapon_model(obj,weap,proj_name,1,0.0f,NULL,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_weap_projectile_spawn_from_weapon_bone_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	weap=weapon_script_lookup();

	script_value_to_string(cx,argv[0],proj_name,name_str_len);

	slop=script_value_to_float(cx,argv[1]);

	if (!weapon_script_projectile_spawn_weapon_model(obj,weap,proj_name,1,slop,NULL,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_weap_projectile_spawn_from_weapon_bone_multi_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int						count;
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	weap=weapon_script_lookup();

	script_value_to_string(cx,argv[0],proj_name,name_str_len);

	count=script_value_to_int(cx,argv[1]);
	slop=script_value_to_float(cx,argv[2]);

	if (!weapon_script_projectile_spawn_weapon_model(obj,weap,proj_name,count,slop,NULL,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_weap_projectile_spawn_from_weapon_bone_offset_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
	d3ang					off_ang;
    obj_type				*obj;
	weapon_type				*weap;

	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	weap=weapon_script_lookup();

	script_value_to_string(cx,argv[0],proj_name,name_str_len);

	off_ang.x=script_value_to_float(cx,argv[1]);
	off_ang.z=script_value_to_float(cx,argv[2]);
	off_ang.y=script_value_to_float(cx,argv[3]);

	if (!weapon_script_projectile_spawn_weapon_model(obj,weap,proj_name,1,0.0f,&off_ang,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

/* =======================================================

      Projectile Spawn from Object
      
======================================================= */

JSValueRef js_weap_projectile_spawn_from_object_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
    obj_type				*obj;
	weapon_type				*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	weap=weapon_script_lookup();

	script_value_to_string(cx,argv[0],proj_name,name_str_len);

	if (!weapon_script_projectile_spawn_object_model(obj,weap,proj_name,1,0.0f,NULL,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_weap_projectile_spawn_from_object_bone_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	weap=weapon_script_lookup();

	script_value_to_string(cx,argv[0],proj_name,name_str_len);

	slop=script_value_to_float(cx,argv[1]);

	if (!weapon_script_projectile_spawn_object_model(obj,weap,proj_name,1,slop,NULL,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_weap_projectile_spawn_from_object_bone_multi_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int						count;
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	weap=weapon_script_lookup();

	script_value_to_string(cx,argv[0],proj_name,name_str_len);

	count=script_value_to_int(cx,argv[1]);
	slop=script_value_to_float(cx,argv[2]);

	if (!weapon_script_projectile_spawn_object_model(obj,weap,proj_name,count,slop,NULL,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_weap_projectile_spawn_from_object_bone_offset_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
	d3ang					off_ang;
    obj_type				*obj;
	weapon_type				*weap;

	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	weap=weapon_script_lookup();

	script_value_to_string(cx,argv[0],proj_name,name_str_len);

	off_ang.x=script_value_to_float(cx,argv[1]);
	off_ang.z=script_value_to_float(cx,argv[2]);
	off_ang.y=script_value_to_float(cx,argv[3]);

	if (!weapon_script_projectile_spawn_object_model(obj,weap,proj_name,1,0.0f,&off_ang,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

/* =======================================================

      Projectile Spawn from Barrel
      
======================================================= */

JSValueRef js_weap_projectile_spawn_from_barrel_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
    obj_type				*obj;
	weapon_type				*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	weap=weapon_script_lookup();

	script_value_to_string(cx,argv[0],proj_name,name_str_len);

	if (!weapon_script_projectile_spawn_weapon_barrel(obj,weap,proj_name,1,0.0f,NULL,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_weap_projectile_spawn_from_barrel_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	weap=weapon_script_lookup();

	script_value_to_string(cx,argv[0],proj_name,name_str_len);

	slop=script_value_to_float(cx,argv[1]);

	if (!weapon_script_projectile_spawn_weapon_barrel(obj,weap,proj_name,1,slop,NULL,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_weap_projectile_spawn_from_barrel_multi_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int						count;
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	weap=weapon_script_lookup();

	script_value_to_string(cx,argv[0],proj_name,name_str_len);

	count=script_value_to_int(cx,argv[1]);
	slop=script_value_to_float(cx,argv[2]);

	if (!weapon_script_projectile_spawn_weapon_barrel(obj,weap,proj_name,count,slop,NULL,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_weap_projectile_spawn_from_barrel_offset_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
	d3ang					off_ang;
    obj_type				*obj;
	weapon_type				*weap;

	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	weap=weapon_script_lookup();

	script_value_to_string(cx,argv[0],proj_name,name_str_len);

	off_ang.x=script_value_to_float(cx,argv[1]);
	off_ang.z=script_value_to_float(cx,argv[2]);
	off_ang.y=script_value_to_float(cx,argv[3]);

	if (!weapon_script_projectile_spawn_weapon_barrel(obj,weap,proj_name,1,0.0f,&off_ang,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

/* =======================================================

      Projectile Spawn from Object To Center
      
======================================================= */

JSValueRef js_weap_projectile_spawn_from_center_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
    obj_type				*obj;
	weapon_type				*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	weap=weapon_script_lookup();

	script_value_to_string(cx,argv[0],proj_name,name_str_len);

	if (!weapon_script_projectile_spawn_center(obj,weap,proj_name,1,0.0f,NULL,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_weap_projectile_spawn_from_center_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	weap=weapon_script_lookup();

	script_value_to_string(cx,argv[0],proj_name,name_str_len);

	slop=script_value_to_float(cx,argv[1]);

	if (!weapon_script_projectile_spawn_center(obj,weap,proj_name,1,slop,NULL,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_weap_projectile_spawn_from_center_multi_slop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int						count;
	char					proj_name[name_str_len],err_str[256];
	float					slop;
    obj_type				*obj;
	weapon_type				*weap;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	weap=weapon_script_lookup();

	script_value_to_string(cx,argv[0],proj_name,name_str_len);

	count=script_value_to_int(cx,argv[1]);
	slop=script_value_to_float(cx,argv[2]);

	if (!weapon_script_projectile_spawn_center(obj,weap,proj_name,count,slop,NULL,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_weap_projectile_spawn_from_center_offset_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					proj_name[name_str_len],err_str[256];
	d3ang					off_ang;
    obj_type				*obj;
	weapon_type				*weap;

	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();
	weap=weapon_script_lookup();

	script_value_to_string(cx,argv[0],proj_name,name_str_len);

	off_ang.x=script_value_to_float(cx,argv[1]);
	off_ang.z=script_value_to_float(cx,argv[2]);
	off_ang.y=script_value_to_float(cx,argv[3]);

	if (!weapon_script_projectile_spawn_center(obj,weap,proj_name,1,0.0f,&off_ang,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}


