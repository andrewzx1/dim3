/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.weapon object

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

JSValueRef js_obj_weapon_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_weapon_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_weapon_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_weapon_get_select_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_weapon_set_select_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_weapon_fire_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_weapon_hide_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_weapon_reset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_weapon_hide_single_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_weapon_reset_single_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_weapon_is_hidden_single_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_weapon_get_ammo_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_weapon_get_max_ammo_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_weapon_get_clip_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_weapon_get_max_clip_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_weapon_get_alt_ammo_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_weapon_get_alt_max_ammo_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_weapon_get_alt_clip_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_weapon_get_alt_max_clip_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_function	obj_weapon_functions[]={
							{"add",					js_obj_weapon_add_func,						1},
							{"getSelect",			js_obj_weapon_get_select_func,				0},
							{"setSelect",			js_obj_weapon_set_select_func,				1},
							{"fire",				js_obj_weapon_fire_func,					2},
							{"hide",				js_obj_weapon_hide_func,					1},
							{"reset",				js_obj_weapon_reset_func,					0},
							{"hideSingle",			js_obj_weapon_hide_single_func,				2},
							{"resetSingle",			js_obj_weapon_reset_single_func,			1},
							{"isHiddenSingle",		js_obj_weapon_is_hidden_single_func,		1},
							{"getAmmoCount",		js_obj_weapon_get_ammo_count_func,			1},
							{"getMaxAmmoCount",		js_obj_weapon_get_max_ammo_count_func,		1},
							{"getClipCount",		js_obj_weapon_get_clip_count_func,			1},
							{"getMaxClipCount",		js_obj_weapon_get_max_clip_count_func,		1},
							{"getAltAmmoCount",		js_obj_weapon_get_alt_ammo_count_func,		1},
							{"getAltMaxAmmoCount",	js_obj_weapon_get_alt_max_ammo_count_func,	1},
							{"getAltClipCount",		js_obj_weapon_get_alt_clip_count_func,		1},
							{"getAltMaxClipCount",	js_obj_weapon_get_alt_max_clip_count_func,	1},
							{0}};

JSClassRef			obj_weapon_class;

/* =======================================================

      Create Weapon Array
      
======================================================= */

void script_init_obj_weapon_object(void)
{
	obj_weapon_class=script_create_class("obj_weapon_class",js_obj_weapon_get_property,js_obj_weapon_set_property);
}

void script_free_obj_weapon_object(void)
{
	script_free_class(obj_weapon_class);
}

JSObjectRef script_add_obj_weapon_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_weapon_class,"weapon",NULL,obj_weapon_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_weapon_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,NULL));
}

bool js_obj_weapon_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,NULL));
}

/* =======================================================

      Weapon Add and Get Functions
      
======================================================= */

JSValueRef js_obj_weapon_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
    obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	script_value_to_string(argv[0],name,name_str_len);
	
		// add weapon
		
	script_bool_to_value(weapon_create(obj,name));

	return(TRUE);
}

/* =======================================================

      Weapon Select Functions
      
======================================================= */

JSValueRef js_obj_weapon_get_select_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

    obj=object_find_uid(js.attach.thing_uid);
	weap=weapon_find_uid(obj->held_weapon.next_uid);
	if (weap==NULL) {
		*rval=script_null_to_value();
		return(TRUE);
	}
	
    *rval=script_string_to_value(weap->name);

    return(TRUE);
}

JSValueRef js_obj_weapon_set_select_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(FALSE);
	
	weapon_set(obj,weap);

	return(TRUE);
}

/* =======================================================

      Fire Weapon Function
      
======================================================= */

JSValueRef js_obj_weapon_fire_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;
	
	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(FALSE);

	script_bool_to_value(weapon_script_fire(js.time.current_tick,obj,weap,script_value_to_int(argv[1])));

	return(TRUE);
}

/* =======================================================

      Hide and Reset Weapons Function
      
======================================================= */

JSValueRef js_obj_weapon_hide_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	obj->hide_all_weapons=script_value_to_bool(argv[0]);

	return(TRUE);
}

JSValueRef js_obj_weapon_reset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	weapon_reset_ammo_object(obj);

	return(TRUE);
}

JSValueRef js_obj_weapon_hide_single_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(FALSE);
	
	weap->hidden=script_value_to_bool(argv[1]);

	return(TRUE);
}

JSValueRef js_obj_weapon_reset_single_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(FALSE);
	
	weapon_reset_ammo(weap);

	return(TRUE);
}

JSValueRef js_obj_weapon_is_hidden_single_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(FALSE);
	
	*rval=script_bool_to_value(weap->hidden);

	return(TRUE);
}

/* =======================================================

      Ammo Count Functions
      
======================================================= */

JSValueRef js_obj_weapon_get_ammo_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(FALSE);
	
	*rval=script_int_to_value(weap->ammo.count);

	return(TRUE);
}

JSValueRef js_obj_weapon_get_max_ammo_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(FALSE);

	*rval=script_int_to_value(weap->ammo.max_count);

	return(TRUE);
}

JSValueRef js_obj_weapon_get_clip_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(FALSE);
	
	*rval=script_int_to_value(weap->ammo.clip_count);

	return(TRUE);
}

JSValueRef js_obj_weapon_get_max_clip_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(FALSE);

	*rval=script_int_to_value(weap->ammo.max_clip_count);

	return(TRUE);
}

/* =======================================================

      Alt Ammo Count Functions
      
======================================================= */

JSValueRef js_obj_weapon_get_alt_ammo_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(FALSE);
	
	*rval=script_int_to_value(weap->alt_ammo.count);

	return(TRUE);
}

JSValueRef js_obj_weapon_get_alt_max_ammo_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(FALSE);

	*rval=script_int_to_value(weap->alt_ammo.max_count);

	return(TRUE);
}

JSValueRef js_obj_weapon_get_alt_clip_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(FALSE);
	
	*rval=script_int_to_value(weap->alt_ammo.clip_count);

	return(TRUE);
}

JSValueRef js_obj_weapon_get_alt_max_clip_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(obj,argv[0]);
	if (weap==NULL) return(FALSE);

	*rval=script_int_to_value(weap->alt_ammo.max_clip_count);

	return(TRUE);
}



