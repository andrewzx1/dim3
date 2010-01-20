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

JSStaticFunction	obj_weapon_functions[]={
							{"add",					js_obj_weapon_add_func,						kJSPropertyAttributeDontDelete},
							{"getSelect",			js_obj_weapon_get_select_func,				kJSPropertyAttributeDontDelete},
							{"setSelect",			js_obj_weapon_set_select_func,				kJSPropertyAttributeDontDelete},
							{"fire",				js_obj_weapon_fire_func,					kJSPropertyAttributeDontDelete},
							{"hide",				js_obj_weapon_hide_func,					kJSPropertyAttributeDontDelete},
							{"reset",				js_obj_weapon_reset_func,					kJSPropertyAttributeDontDelete},
							{"hideSingle",			js_obj_weapon_hide_single_func,				kJSPropertyAttributeDontDelete},
							{"resetSingle",			js_obj_weapon_reset_single_func,			kJSPropertyAttributeDontDelete},
							{"isHiddenSingle",		js_obj_weapon_is_hidden_single_func,		kJSPropertyAttributeDontDelete},
							{"getAmmoCount",		js_obj_weapon_get_ammo_count_func,			kJSPropertyAttributeDontDelete},
							{"getMaxAmmoCount",		js_obj_weapon_get_max_ammo_count_func,		kJSPropertyAttributeDontDelete},
							{"getClipCount",		js_obj_weapon_get_clip_count_func,			kJSPropertyAttributeDontDelete},
							{"getMaxClipCount",		js_obj_weapon_get_max_clip_count_func,		kJSPropertyAttributeDontDelete},
							{"getAltAmmoCount",		js_obj_weapon_get_alt_ammo_count_func,		kJSPropertyAttributeDontDelete},
							{"getAltMaxAmmoCount",	js_obj_weapon_get_alt_max_ammo_count_func,	kJSPropertyAttributeDontDelete},
							{"getAltClipCount",		js_obj_weapon_get_alt_clip_count_func,		kJSPropertyAttributeDontDelete},
							{"getAltMaxClipCount",	js_obj_weapon_get_alt_max_clip_count_func,	kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_weapon_class;

/* =======================================================

      Create Weapon Array
      
======================================================= */

void script_init_obj_weapon_object(void)
{
	obj_weapon_class=script_create_class("obj_weapon_class",NULL,obj_weapon_functions);
}

void script_free_obj_weapon_object(void)
{
	script_free_class(obj_weapon_class);
}

JSObjectRef script_add_obj_weapon_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,obj_weapon_class,"weapon"));
}

/* =======================================================

      Weapon Add and Get Functions
      
======================================================= */

JSValueRef js_obj_weapon_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
    obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.thing_uid);

	script_value_to_string(cx,argv[0],name,name_str_len);
	
		// add weapon
		
	script_bool_to_value(cx,weapon_create(obj,name));

	return(script_null_to_value(cx));
}

/* =======================================================

      Weapon Select Functions
      
======================================================= */

JSValueRef js_obj_weapon_get_select_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
    obj=object_find_uid(js.attach.thing_uid);
	weap=weapon_find_uid(obj->held_weapon.next_uid);
	if (weap==NULL) return(script_null_to_value(cx));
	
    return(script_string_to_value(cx,weap->name));
}

JSValueRef js_obj_weapon_set_select_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(cx,obj,argv[0],exception);
	if (weap!=NULL) weapon_set(obj,weap);

	return(script_null_to_value(cx));
}

/* =======================================================

      Fire Weapon Function
      
======================================================= */

JSValueRef js_obj_weapon_fire_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(cx,obj,argv[0],exception);
	if (weap==NULL) return(script_null_to_value(cx));

	return(script_bool_to_value(cx,weapon_script_fire(js.time.current_tick,obj,weap,script_value_to_int(cx,argv[1]))));
}

/* =======================================================

      Hide and Reset Weapons Function
      
======================================================= */

JSValueRef js_obj_weapon_hide_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->hide_all_weapons=script_value_to_bool(cx,argv[0]);

	return(script_null_to_value(cx));
}

JSValueRef js_obj_weapon_reset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.thing_uid);
	weapon_reset_ammo_object(obj);

	return(script_null_to_value(cx));
}

JSValueRef js_obj_weapon_hide_single_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(cx,obj,argv[0],exception);
	if (weap!=NULL) weap->hidden=script_value_to_bool(cx,argv[1]);

	return(script_null_to_value(cx));
}

JSValueRef js_obj_weapon_reset_single_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(cx,obj,argv[0],exception);
	if (weap!=NULL) weapon_reset_ammo(weap);

	return(script_null_to_value(cx));
}

JSValueRef js_obj_weapon_is_hidden_single_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(cx,obj,argv[0],exception);
	if (weap==NULL) return(script_null_to_value(cx));
	
	return(script_bool_to_value(cx,weap->hidden));
}

/* =======================================================

      Ammo Count Functions
      
======================================================= */

JSValueRef js_obj_weapon_get_ammo_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(cx,obj,argv[0],exception);
	if (weap==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,weap->ammo.count));
}

JSValueRef js_obj_weapon_get_max_ammo_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(cx,obj,argv[0],exception);
	if (weap==NULL) return(script_null_to_value(cx));

	return(script_int_to_value(cx,weap->ammo.max_count));
}

JSValueRef js_obj_weapon_get_clip_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(cx,obj,argv[0],exception);
	if (weap==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,weap->ammo.clip_count));
}

JSValueRef js_obj_weapon_get_max_clip_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(cx,obj,argv[0],exception);
	if (weap==NULL) return(script_null_to_value(cx));

	return(script_int_to_value(cx,weap->ammo.max_clip_count));
}

/* =======================================================

      Alt Ammo Count Functions
      
======================================================= */

JSValueRef js_obj_weapon_get_alt_ammo_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(cx,obj,argv[0],exception);
	if (weap==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,weap->alt_ammo.count));
}

JSValueRef js_obj_weapon_get_alt_max_ammo_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(cx,obj,argv[0],exception);
	if (weap==NULL) return(script_null_to_value(cx));

	return(script_int_to_value(cx,weap->alt_ammo.max_count));
}

JSValueRef js_obj_weapon_get_alt_clip_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(cx,obj,argv[0],exception);
	if (weap==NULL) return(script_null_to_value(cx));
	
	return(script_int_to_value(cx,weap->alt_ammo.clip_count));
}

JSValueRef js_obj_weapon_get_alt_max_clip_count_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    obj_type		*obj;
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(js.attach.thing_uid);
	weap=script_find_weapon_from_name_arg(cx,obj,argv[0],exception);
	if (weap==NULL) return(script_null_to_value(cx));

	return(script_int_to_value(cx,weap->alt_ammo.max_clip_count));
}

