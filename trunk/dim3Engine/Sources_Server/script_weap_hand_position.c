/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.handPosition object

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

JSValueRef js_weap_hand_position_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_hand_position_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_hand_position_get_x(void);
JSValueRef js_weap_hand_position_get_y(void);
JSValueRef js_weap_hand_position_get_z(void);
void js_weap_hand_position_set_x(JSValueRef vp,JSValueRef *exception);
void js_weap_hand_position_set_y(JSValueRef vp,JSValueRef *exception);
void js_weap_hand_position_set_z(JSValueRef vp,JSValueRef *exception);

script_js_property	weap_hand_position_props[]={
							{"x",						js_weap_hand_position_get_x,				js_weap_hand_position_set_x},
							{"y",						js_weap_hand_position_get_y,				js_weap_hand_position_set_y},
							{"z",						js_weap_hand_position_get_z,				js_weap_hand_position_set_z},
							{0}};
							
JSClassRef			weap_hand_position_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_hand_position_object(void)
{
	weap_hand_position_class=script_create_class("weap_hand_position_class",js_weap_hand_position_get_property,js_weap_hand_position_set_property);
}

void script_free_weap_hand_position_object(void)
{
	script_free_class(weap_hand_position_class);
}

JSObjectRef script_add_weap_hand_position_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,weap_hand_position_class,"handPosition",weap_hand_position_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_weap_hand_position_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,weap_hand_position_props));
}

bool js_weap_hand_position_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,weap_hand_position_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_hand_position_get_x(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->hand.shift.x);
	
	return(TRUE);
}

JSValueRef js_weap_hand_position_get_y(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->hand.shift.y);
	
	return(TRUE);
}

JSValueRef js_weap_hand_position_get_z(void)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->hand.shift.z);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

void js_weap_hand_position_set_x(JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->hand.shift.x=script_value_to_int(*vp);
}

void js_weap_hand_position_set_y(JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->hand.shift.y=script_value_to_int(*vp);
}

void js_weap_hand_position_set_z(JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->hand.shift.z=script_value_to_int(*vp);
}



