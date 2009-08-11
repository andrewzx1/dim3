/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.pickup object

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

extern server_type		server;
extern js_type			js;

JSValueRef js_obj_pickup_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_pickup_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_pickup_get_objectId(void);
JSValueRef js_obj_pickup_get_objectName(void);
JSValueRef js_obj_pickup_get_objectIsPlayer(void);
JSValueRef js_obj_pickup_get_itemId(void);
JSValueRef js_obj_pickup_get_itemName(void);
JSValueRef js_obj_pickup_add_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_swap_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_add_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_add_clip_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_add_alt_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_add_alt_clip_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_add_health_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_add_custom_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_pickup_cancel_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_property	obj_pickup_props[]={
							{"objectId",			js_obj_pickup_get_objectId,			NULL},
							{"objectName",			js_obj_pickup_get_objectName,		NULL},
							{"objectIsPlayer",		js_obj_pickup_get_objectIsPlayer,	NULL},
							{"itemId",				js_obj_pickup_get_itemId,			NULL},
							{"itemName",			js_obj_pickup_get_itemName,			NULL},
							{0}};
							
script_js_function	obj_pickup_functions[]={
							{"addWeapon",			js_obj_pickup_add_weapon_func,		2},
							{"swapWeapon",			js_obj_pickup_swap_weapon_func,		2},
							{"addAmmo",				js_obj_pickup_add_ammo_func,		3},
							{"addClip",				js_obj_pickup_add_clip_func,		3},
							{"addAltAmmo",			js_obj_pickup_add_alt_ammo_func,	3},
							{"addAltClip",			js_obj_pickup_add_alt_clip_func,	3},
							{"addHealth",			js_obj_pickup_add_health_func,		2},
							{"addCustom",			js_obj_pickup_add_custom_func,		2},
							{"cancel",				js_obj_pickup_cancel_func,			0},
							{0}};

JSClassRef			obj_pickup_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_pickup_object(void)
{
	obj_pickup_class=script_create_class("obj_pickup_class",js_obj_pickup_get_property,js_obj_pickup_set_property);
}

void script_free_obj_pickup_object(void)
{
	script_free_class(obj_pickup_class);
}

JSObjectRef script_add_obj_pickup_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_pickup_class,"pickup",obj_pickup_props,obj_pickup_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_pickup_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_pickup_props));
}

bool js_obj_pickup_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,obj_pickup_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_pickup_get_objectId(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->pickup.obj_uid);
	
	return(TRUE);
}

JSValueRef js_obj_pickup_get_objectName(void)
{
	obj_type		*obj,*pickup_obj;

	obj=object_find_uid(js.attach.thing_uid);

	pickup_obj=object_find_uid(obj->pickup.obj_uid);
	if (pickup_obj==NULL) {
		*vp=script_null_to_value();
	}
	else {
		*vp=script_string_to_value(pickup_obj->name);
	}
	
	return(TRUE);
}

JSValueRef js_obj_pickup_get_objectIsPlayer(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->pickup.obj_uid==server.player_obj_uid);
	
	return(TRUE);
}

JSValueRef js_obj_pickup_get_itemId(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->pickup.item_uid);
	
	return(TRUE);
}

JSValueRef js_obj_pickup_get_itemName(void)
{
	obj_type		*obj,*pickup_item;

	obj=object_find_uid(js.attach.thing_uid);

	pickup_item=object_find_uid(obj->pickup.item_uid);
	if (pickup_item==NULL) {
		*vp=script_null_to_value();
	}
	else {
		*vp=script_string_to_value(pickup_item->name);
	}
	
	return(TRUE);
}

/* =======================================================

      Add Functions
      
======================================================= */

JSValueRef js_obj_pickup_add_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	weapon_type	*weap;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(FALSE);
	
	weap=script_find_weapon_from_name_arg(obj,argv[1],exception);
	if (weap==NULL) return(FALSE);
	
    *rval=script_bool_to_value(item_add_weapon(obj,weap));
	return(TRUE);
}

JSValueRef js_obj_pickup_swap_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	weapon_type	*weap;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(FALSE);
	
	weap=script_find_weapon_from_name_arg(obj,argv[1],exception);
	if (weap==NULL) return(FALSE);
	
    *rval=script_bool_to_value(item_swap_weapon(obj,weap));
	return(TRUE);
}

JSValueRef js_obj_pickup_add_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	weapon_type	*weap;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(FALSE);
	
	weap=script_find_weapon_from_name_arg(obj,argv[1],exception);
	if (weap==NULL) return(FALSE);
	
    *rval=script_bool_to_value(item_add_ammo(obj,weap,script_value_to_int(argv[2])));
	return(TRUE);
}

JSValueRef js_obj_pickup_add_clip_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	weapon_type	*weap;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(FALSE);
	
	weap=script_find_weapon_from_name_arg(obj,argv[1],exception);
	if (weap==NULL) return(FALSE);
	
    *rval=script_bool_to_value(item_add_clip(obj,weap,script_value_to_int(argv[2])));
	return(TRUE);
}

JSValueRef js_obj_pickup_add_alt_ammo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	weapon_type	*weap;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(FALSE);
	
	weap=script_find_weapon_from_name_arg(obj,argv[1],exception);
	if (weap==NULL) return(FALSE);
	
    *rval=script_bool_to_value(item_add_alt_ammo(obj,weap,script_value_to_int(argv[2])));
	return(TRUE);
}

JSValueRef js_obj_pickup_add_alt_clip_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	weapon_type	*weap;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(FALSE);
	
	weap=script_find_weapon_from_name_arg(obj,argv[1],exception);
	if (weap==NULL) return(FALSE);
	
    *rval=script_bool_to_value(item_add_alt_clip(obj,weap,script_value_to_int(argv[2])));
	return(TRUE);
}

JSValueRef js_obj_pickup_add_health_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(FALSE);

   *rval=script_bool_to_value(item_add_health(obj,script_value_to_int(argv[1])));
	return(TRUE);
}

JSValueRef js_obj_pickup_add_custom_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(FALSE);

	item_add_custom(obj,script_value_to_int(argv[1]));
	return(TRUE);
}

/* =======================================================

      Cancel Functions
      
======================================================= */

JSValueRef js_obj_pickup_cancel_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type			*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->pickup.canceled=TRUE;
	
	return(TRUE);
}
