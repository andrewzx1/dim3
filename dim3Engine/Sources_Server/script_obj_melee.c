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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"
#include "objects.h"
#include "weapons.h"

extern js_type			js;

JSValueRef js_obj_melee_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_melee_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_melee_get_strikeBoneTag(void);
JSValueRef js_obj_melee_get_strikePoseName(void);
JSValueRef js_obj_melee_get_radius(void);
JSValueRef js_obj_melee_get_distance(void);
JSValueRef js_obj_melee_get_damage(void);
JSValueRef js_obj_melee_get_force(void);
void js_obj_melee_set_strikeBoneTag(JSValueRef vp,JSValueRef *exception);
void js_obj_melee_set_strikePoseName(JSValueRef vp,JSValueRef *exception);
void js_obj_melee_set_radius(JSValueRef vp,JSValueRef *exception);
void js_obj_melee_set_distance(JSValueRef vp,JSValueRef *exception);
void js_obj_melee_set_damage(JSValueRef vp,JSValueRef *exception);
void js_obj_melee_set_force(JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_melee_spawn_from_object_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_property	obj_melee_props[]={
							{"strikeBoneTag",			js_obj_melee_get_strikeBoneTag,					js_obj_melee_set_strikeBoneTag},
							{"strikePoseName",			js_obj_melee_get_strikePoseName,				js_obj_melee_set_strikePoseName},
							{"radius",					js_obj_melee_get_radius,						js_obj_melee_set_radius},
							{"distance",				js_obj_melee_get_distance,						js_obj_melee_set_distance},
							{"damage",					js_obj_melee_get_damage,						js_obj_melee_set_damage},
							{"force",					js_obj_melee_get_force,							js_obj_melee_set_force},
							{0}};

script_js_function	obj_melee_functions[]={
							{"spawnFromObjectBone",		js_obj_melee_spawn_from_object_bone_func,		0},
							{0}};

JSClassRef			obj_melee_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_melee_object(void)
{
	obj_melee_class=script_create_class("obj_melee_class",js_obj_melee_get_property,js_obj_melee_set_property);
}

void script_free_obj_melee_object(void)
{
	script_free_class(obj_melee_class);
}

JSObjectRef script_add_obj_melee_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_melee_class,"melee",obj_melee_props,obj_melee_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_melee_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_melee_props));
}

bool js_obj_melee_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,obj_melee_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_melee_get_strikeBoneTag(void)
{
	char				str[32];
    obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	model_tag_to_text(obj->melee.object_strike_bone_tag,str);
	*vp=script_string_to_value(str);
	
	return(TRUE);
}

JSValueRef js_obj_melee_get_strikePoseName(void)
{
    obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(obj->melee.object_strike_pose_name);
	
	return(TRUE);
}

JSValueRef js_obj_melee_get_radius(void)
{
    obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->melee.radius);
	
	return(TRUE);
}

JSValueRef js_obj_melee_get_distance(void)
{
    obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->melee.distance);
	
	return(TRUE);
}

JSValueRef js_obj_melee_get_damage(void)
{
    obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->melee.damage);
	
	return(TRUE);
}

JSValueRef js_obj_melee_get_force(void)
{
    obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->melee.force);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

void js_obj_melee_set_strikeBoneTag(JSValueRef vp,JSValueRef *exception)
{
	char				str[32];
    obj_type			*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	script_value_to_string(*vp,str,32);
	obj->melee.object_strike_bone_tag=text_to_model_tag(str);
}

void js_obj_melee_set_strikePoseName(JSValueRef vp,JSValueRef *exception)
{
    obj_type			*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	script_value_to_string(*vp,obj->melee.object_strike_pose_name,name_str_len);
}

void js_obj_melee_set_radius(JSValueRef vp,JSValueRef *exception)
{
    obj_type			*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->melee.radius=script_value_to_int(*vp);
}

void js_obj_melee_set_distance(JSValueRef vp,JSValueRef *exception)
{
    obj_type			*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->melee.distance=script_value_to_int(*vp);
}

void js_obj_melee_set_damage(JSValueRef vp,JSValueRef *exception)
{
    obj_type			*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->melee.damage=script_value_to_int(*vp);
}

void js_obj_melee_set_force(JSValueRef vp,JSValueRef *exception)
{
    obj_type			*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->melee.force=script_value_to_int(*vp);
}

/* =======================================================

      Spawn Melee
      
======================================================= */

JSValueRef js_obj_melee_spawn_from_object_bone_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char				err_str[256];
    obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);

	if (!melee_script_spawn_object_model(js.time.current_tick,obj,NULL,err_str)) {
		*exception=script_create_exception(err_str);
		return(FALSE);
	}

	return(TRUE);
}
