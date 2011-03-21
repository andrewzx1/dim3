/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.melee object

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "scripts.h"
#include "objects.h"

extern js_type			js;

JSValueRef js_obj_melee_get_strikeBoneTag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_melee_get_strikePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_melee_get_radius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_melee_get_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_melee_get_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_melee_get_force(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_melee_set_strikeBoneTag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_melee_set_strikePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_melee_set_radius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_melee_set_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_melee_set_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_melee_set_force(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_melee_spawn_from_object_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_melee_spawn_from_touch_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_melee_props[]={
							{"strikeBoneTag",			js_obj_melee_get_strikeBoneTag,					js_obj_melee_set_strikeBoneTag,		kJSPropertyAttributeDontDelete},
							{"strikePoseName",			js_obj_melee_get_strikePoseName,				js_obj_melee_set_strikePoseName,	kJSPropertyAttributeDontDelete},
							{"radius",					js_obj_melee_get_radius,						js_obj_melee_set_radius,			kJSPropertyAttributeDontDelete},
							{"distance",				js_obj_melee_get_distance,						js_obj_melee_set_distance,			kJSPropertyAttributeDontDelete},
							{"damage",					js_obj_melee_get_damage,						js_obj_melee_set_damage,			kJSPropertyAttributeDontDelete},
							{"force",					js_obj_melee_get_force,							js_obj_melee_set_force,				kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	obj_melee_functions[]={
							{"spawnFromObjectBone",		js_obj_melee_spawn_from_object_bone_func,		kJSPropertyAttributeDontDelete},
							{"spawnFromTouch",			js_obj_melee_spawn_from_touch_func,				kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_melee_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_melee_object(void)
{
	obj_melee_class=script_create_class("obj_melee_class",obj_melee_props,obj_melee_functions);
}

void script_free_obj_melee_object(void)
{
	script_free_class(obj_melee_class);
}

JSObjectRef script_add_obj_melee_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,obj_melee_class,"melee"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_melee_get_strikeBoneTag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	char				str[32];
    obj_type			*obj;

	obj=object_script_lookup();
	model_tag_to_text(obj->melee.object_strike_bone_tag,str);
	return(script_string_to_value(cx,str));
}

JSValueRef js_obj_melee_get_strikePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
    obj_type			*obj;

	obj=object_script_lookup();
	return(script_string_to_value(cx,obj->melee.object_strike_pose_name));
}

JSValueRef js_obj_melee_get_radius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
    obj_type			*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->melee.radius));
}

JSValueRef js_obj_melee_get_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
    obj_type			*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->melee.distance));
}

JSValueRef js_obj_melee_get_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
    obj_type			*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->melee.damage));
}

JSValueRef js_obj_melee_get_force(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
    obj_type			*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->melee.force));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_melee_set_strikeBoneTag(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	char				str[32];
    obj_type			*obj;
	
	obj=object_script_lookup();
	script_value_to_string(cx,vp,str,32);
	obj->melee.object_strike_bone_tag=text_to_model_tag(str);
	
	return(TRUE);
}

bool js_obj_melee_set_strikePoseName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
    obj_type			*obj;
	
	obj=object_script_lookup();
	script_value_to_string(cx,vp,obj->melee.object_strike_pose_name,name_str_len);
	
	return(TRUE);
}

bool js_obj_melee_set_radius(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
    obj_type			*obj;
	
	obj=object_script_lookup();
	obj->melee.radius=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_obj_melee_set_distance(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
    obj_type			*obj;
	
	obj=object_script_lookup();
	obj->melee.distance=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_obj_melee_set_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
    obj_type			*obj;
	
	obj=object_script_lookup();
	obj->melee.damage=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_obj_melee_set_force(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
    obj_type			*obj;
	
	obj=object_script_lookup();
	obj->melee.force=script_value_to_int(cx,vp);
	
	return(TRUE);
}

/* =======================================================

      Spawn Melee
      
======================================================= */

JSValueRef js_obj_melee_spawn_from_object_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char				err_str[256];
    obj_type			*obj;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();

	if (!melee_script_spawn_object_model(obj,NULL,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_obj_melee_spawn_from_touch_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char				err_str[256];
    obj_type			*obj;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_script_lookup();

	if (!melee_script_spawn_object_touch(obj,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}
