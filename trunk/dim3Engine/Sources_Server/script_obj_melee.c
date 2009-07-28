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

JSBool js_obj_melee_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_melee_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_melee_get_strikeBoneTag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_melee_get_strikePoseName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_melee_get_radius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_melee_get_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_melee_get_damage(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_melee_get_force(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_melee_set_strikeBoneTag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_melee_set_strikePoseName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_melee_set_radius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_melee_set_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_melee_set_damage(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_melee_set_force(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_melee_spawn_from_object_bone_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

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

JSClass				*obj_melee_class;

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

void script_add_obj_melee_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,obj_melee_class,"melee",obj_melee_props,obj_melee_functions);
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_obj_melee_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,obj_melee_props));
}

JSBool js_obj_melee_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,obj_melee_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_obj_melee_get_strikeBoneTag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	char				str[32];
    obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	model_tag_to_text(obj->melee.object_strike_bone_tag,str);
	*vp=script_string_to_value(str);
	
	return(JS_TRUE);
}

JSBool js_obj_melee_get_strikePoseName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
    obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(obj->melee.object_strike_pose_name);
	
	return(JS_TRUE);
}

JSBool js_obj_melee_get_radius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
    obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->melee.radius);
	
	return(JS_TRUE);
}

JSBool js_obj_melee_get_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
    obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->melee.distance);
	
	return(JS_TRUE);
}

JSBool js_obj_melee_get_damage(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
    obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->melee.damage);
	
	return(JS_TRUE);
}

JSBool js_obj_melee_get_force(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
    obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->melee.force);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_obj_melee_set_strikeBoneTag(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	char				str[32];
    obj_type			*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	script_value_to_string(*vp,str,32);
	obj->melee.object_strike_bone_tag=text_to_model_tag(str);
	
	return(JS_TRUE);
}

JSBool js_obj_melee_set_strikePoseName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
    obj_type			*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	script_value_to_string(*vp,obj->melee.object_strike_pose_name,name_str_len);
	
	return(JS_TRUE);
}

JSBool js_obj_melee_set_radius(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
    obj_type			*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->melee.radius=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_melee_set_distance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
    obj_type			*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->melee.distance=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_melee_set_damage(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
    obj_type			*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->melee.damage=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_melee_set_force(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
    obj_type			*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->melee.force=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

/* =======================================================

      Spawn Melee
      
======================================================= */

JSBool js_obj_melee_spawn_from_object_bone_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char				err_str[256];
    obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);

	if (!melee_script_spawn_object_model(js.time.current_tick,obj,NULL,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(JS_FALSE);
	}

	return(JS_TRUE);
}
