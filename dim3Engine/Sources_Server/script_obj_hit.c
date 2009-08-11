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

JSValueRef js_obj_hit_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_hit_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_hit_get_objectId(void);
JSValueRef js_obj_hit_get_objectName(void);
JSValueRef js_obj_hit_get_objectIsPlayer(void);
JSValueRef js_obj_hit_get_weaponName(void);
JSValueRef js_obj_hit_get_projectileName(void);
JSValueRef js_obj_hit_get_hitBoxName(void);
JSValueRef js_obj_hit_get_damage(void);

script_js_property	obj_hit_props[]={
							{"objectId",			js_obj_hit_get_objectId,			NULL},
							{"objectName",			js_obj_hit_get_objectName,			NULL},
							{"objectIsPlayer",		js_obj_hit_get_objectIsPlayer,		NULL},
							{"weaponName",			js_obj_hit_get_weaponName,			NULL},
							{"projectileName",		js_obj_hit_get_projectileName,		NULL},
							{"hitBoxName",			js_obj_hit_get_hitBoxName,			NULL},
							{"damage",				js_obj_hit_get_damage,				NULL},
							{0}};

JSClassRef			obj_hit_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_hit_object(void)
{
	obj_hit_class=script_create_class("obj_hit_class",js_obj_hit_get_property,js_obj_hit_set_property);
}

void script_free_obj_hit_object(void)
{
	script_free_class(obj_hit_class);
}

JSObjectRef script_add_obj_hit_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_hit_class,"hit",obj_hit_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_hit_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_hit_props));
}

bool js_obj_hit_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,obj_hit_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_hit_get_objectId(void)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->hit.obj_uid);

	return(TRUE);
}

JSValueRef js_obj_hit_get_objectName(void)
{
	obj_type			*obj,*hit_obj;

	obj=object_find_uid(js.attach.thing_uid);

	hit_obj=object_find_uid(obj->hit.obj_uid);
	if (hit_obj==NULL) {
		*vp=script_null_to_value();
	}
	else {
		*vp=script_string_to_value(hit_obj->name);
	}

	return(TRUE);
}

JSValueRef js_obj_hit_get_objectIsPlayer(void)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->hit.obj_uid==server.player_obj_uid);
	
	return(TRUE);
}

JSValueRef js_obj_hit_get_weaponName(void)
{
	obj_type			*obj;
	weapon_type			*hit_weap;

	obj=object_find_uid(js.attach.thing_uid);

	hit_weap=weapon_find_uid(obj->hit.weap_uid);
	if (hit_weap==NULL) {
		*vp=script_null_to_value();
	}
	else {
		*vp=script_string_to_value(hit_weap->name);
	}

	return(TRUE);
}

JSValueRef js_obj_hit_get_projectileName(void)
{
	obj_type			*obj;
	proj_type			*hit_proj;
	proj_setup_type		*hit_proj_setup;

	obj=object_find_uid(js.attach.thing_uid);

	hit_proj=projectile_find_uid(obj->hit.proj_uid);
	if (hit_proj==NULL) {
		*vp=script_null_to_value();
	}
	else {
		hit_proj_setup=proj_setups_find_uid(hit_proj->proj_setup_uid);
		*vp=script_string_to_value(hit_proj_setup->name);
	}

	return(TRUE);
}

JSValueRef js_obj_hit_get_hitBoxName(void)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);

	if (obj->hit.hit_box_name[0]==0x0) {
		*vp=script_null_to_value();
	}
	else {
		*vp=script_string_to_value(obj->hit.hit_box_name);
	}

	return(TRUE);
}

JSValueRef js_obj_hit_get_damage(void)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->hit.damage);
	
	return(TRUE);
}

