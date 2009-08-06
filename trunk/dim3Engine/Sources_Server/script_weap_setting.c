/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.setting object

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
#include "weapons.h"

extern js_type			js;

JSBool js_weap_setting_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_weap_setting_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
bool js_weap_setting_get_name(JSValueRef *vp);
bool js_weap_setting_get_failInLiquid(JSValueRef *vp);
bool js_weap_setting_get_parentObjectId(JSValueRef *vp);
bool js_weap_setting_set_failInLiquid(JSValueRef *vp);

script_js_property	weap_setting_props[]={
							{"name",				js_weap_setting_get_name,				NULL},
							{"failInLiquid",		js_weap_setting_get_failInLiquid,		js_weap_setting_set_failInLiquid},
							{"parentObjectId",		js_weap_setting_get_parentObjectId,		NULL},
							{0}};

JSClassRef			weap_setting_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_setting_object(void)
{
	weap_setting_class=script_create_class("weap_setting_class",js_weap_setting_get_property,js_weap_setting_set_property);
}

void script_free_weap_setting_object(void)
{
	script_free_class(weap_setting_class);
}

JSObject* script_add_weap_setting_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,weap_setting_class,"setting",weap_setting_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_weap_setting_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,weap_setting_props));
}

JSBool js_weap_setting_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,weap_setting_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_weap_setting_get_name(JSValueRef *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(weap->name);
	
	return(TRUE);
}

bool js_weap_setting_get_failInLiquid(JSValueRef *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(weap->fail_in_liquid);
	
	return(TRUE);
}

bool js_weap_setting_get_parentObjectId(JSValueRef *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->obj_uid);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_setting_set_failInLiquid(JSValueRef *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->fail_in_liquid=script_value_to_bool(*vp);
	
	return(TRUE);
}


