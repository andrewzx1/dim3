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

JSValueRef js_weap_setting_get_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_setting_get_failInLiquid(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_weap_setting_get_parentObjectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_setting_set_failInLiquid(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		weap_setting_props[]={
							{"name",				js_weap_setting_get_name,				NULL,									kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"failInLiquid",		js_weap_setting_get_failInLiquid,		js_weap_setting_set_failInLiquid,		kJSPropertyAttributeDontDelete},
							{"parentObjectId",		js_weap_setting_get_parentObjectId,		NULL,									kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			weap_setting_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_setting_object(void)
{
	weap_setting_class=script_create_class("weap_setting_class",weap_setting_props,NULL);
}

void script_free_weap_setting_object(void)
{
	script_free_class(weap_setting_class);
}

JSObjectRef script_add_weap_setting_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,weap_setting_class,"setting"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_setting_get_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.weap_uid);
	return(script_string_to_value(cx,weap->name));
}

JSValueRef js_weap_setting_get_failInLiquid(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.weap_uid);
	return(script_bool_to_value(cx,weap->fail_in_liquid));
}

JSValueRef js_weap_setting_get_parentObjectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.weap_uid);
	return(script_int_to_value(cx,weap->obj_index));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_setting_set_failInLiquid(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.weap_uid);
	weap->fail_in_liquid=script_value_to_bool(cx,vp);

	return(TRUE);
}


