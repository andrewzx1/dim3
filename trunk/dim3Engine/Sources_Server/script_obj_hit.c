/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.hit object

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

extern server_type		server;
extern js_type			js;

JSValueRef js_obj_hit_get_objectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_hit_get_objectName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_hit_get_objectIsPlayer(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_hit_get_weaponName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_hit_get_projectileName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_hit_get_hitBoxName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_hit_get_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);

JSStaticValue 		obj_hit_props[]={
							{"objectId",			js_obj_hit_get_objectId,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectName",			js_obj_hit_get_objectName,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"objectIsPlayer",		js_obj_hit_get_objectIsPlayer,		NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"weaponName",			js_obj_hit_get_weaponName,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"projectileName",		js_obj_hit_get_projectileName,		NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"hitBoxName",			js_obj_hit_get_hitBoxName,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"damage",				js_obj_hit_get_damage,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			obj_hit_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_hit_object(void)
{
	obj_hit_class=script_create_class("obj_hit_class",obj_hit_props,NULL);
}

void script_free_obj_hit_object(void)
{
	script_free_class(obj_hit_class);
}

JSObjectRef script_add_obj_hit_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,obj_hit_class,"hit"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_hit_get_objectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.obj_index);
	return(script_int_to_value(cx,obj->hit.obj_uid));
}

JSValueRef js_obj_hit_get_objectName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type			*obj,*hit_obj;

	obj=object_find_uid(js.attach.obj_index);

	hit_obj=object_find_uid(obj->hit.obj_uid);
	if (hit_obj==NULL) return(script_null_to_value(cx));
	
	return(script_string_to_value(cx,hit_obj->name));
}

JSValueRef js_obj_hit_get_objectIsPlayer(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.obj_index);
	return(script_bool_to_value(cx,obj->hit.obj_uid==server.player_obj_index));
}

JSValueRef js_obj_hit_get_weaponName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type			*obj;
	weapon_type			*hit_weap;

	obj=object_find_uid(js.attach.obj_index);

	hit_weap=weapon_script_lookup();
	if (hit_weap==NULL) return(script_null_to_value(cx));

	return(script_string_to_value(cx,hit_weap->name));
}

JSValueRef js_obj_hit_get_projectileName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type			*obj;
	weapon_type			*weap;
	proj_type			*hit_proj;
	proj_setup_type		*hit_proj_setup;

	obj=object_find_uid(js.attach.obj_index);

	hit_proj=projectile_find_uid(obj->hit.proj_uid);
	if (hit_proj==NULL) return(script_null_to_value(cx));
	
	weap=weapon_script_lookup();

	hit_proj_setup=proj_setups_find_uid(weap,hit_proj->proj_setup_index);
	return(script_string_to_value(cx,hit_proj_setup->name));
}

JSValueRef js_obj_hit_get_hitBoxName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.obj_index);

	if (obj->hit.hit_box_name[0]==0x0) return(script_null_to_value(cx));

	return(script_string_to_value(cx,obj->hit.hit_box_name));
}

JSValueRef js_obj_hit_get_damage(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.obj_index);
	return(script_int_to_value(cx,obj->hit.damage));
}

