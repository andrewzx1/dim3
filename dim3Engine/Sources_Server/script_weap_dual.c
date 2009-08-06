/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.dual object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

extern js_type				js;

JSBool js_weap_dual_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_weap_dual_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
bool js_weap_dual_get_on(JSValueRef *vp);
bool js_weap_dual_get_active(JSValueRef *vp);
bool js_weap_dual_get_handOffset(JSValueRef *vp);
bool js_weap_dual_set_on(JSValueRef *vp);
bool js_weap_dual_set_active(JSValueRef *vp);
bool js_weap_dual_set_handOffset(JSValueRef *vp);
JSBool js_weap_dual_switch_hand_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);

script_js_property	weap_dual_props[]={
							{"on",					js_weap_dual_get_on,					js_weap_dual_set_on},
							{"active",				js_weap_dual_get_active,				js_weap_dual_set_active},
							{"handOffset",			js_weap_dual_get_handOffset,			js_weap_dual_set_handOffset},
							{0}};
							
script_js_function	weap_dual_functions[]={
							{"switchHand",			js_weap_dual_switch_hand_func,		1},
							{0}};

JSClassRef			weap_dual_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_dual_object(void)
{
	weap_dual_class=script_create_class("weap_dual_class",js_weap_dual_get_property,js_weap_dual_set_property);
}

void script_free_weap_dual_object(void)
{
	script_free_class(weap_dual_class);
}

JSObject* script_add_weap_dual_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,weap_dual_class,"dual",weap_dual_props,weap_dual_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_weap_dual_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,weap_dual_props));
}

JSBool js_weap_dual_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,weap_dual_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_weap_dual_get_on(JSValueRef *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(weap->dual.on);
	
	return(TRUE);
}

bool js_weap_dual_get_active(JSValueRef *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(weap->dual.active);
	
	return(TRUE);
}

bool js_weap_dual_get_handOffset(JSValueRef *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->dual.hand_offset);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_dual_set_on(JSValueRef *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->dual.on=script_value_to_bool(*vp);
	
	return(TRUE);
}

bool js_weap_dual_set_active(JSValueRef *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->dual.active=script_value_to_bool(*vp);
	
	return(TRUE);
}

bool js_weap_dual_set_handOffset(JSValueRef *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->dual.hand_offset=script_value_to_int(*vp);
	
	return(TRUE);
}

/* =======================================================

      Dual Functions
      
======================================================= */

JSBool js_weap_dual_switch_hand_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	
	if ((!weap->dual.on) && (!weap->dual.active)) {
		weap->dual.in_dual=FALSE;
	}
	else {
		weap->dual.in_dual=script_value_to_bool(argv[0]);
	}
	
	return(TRUE);
}

