/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.handPosition object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

JSBool js_weap_hand_position_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_weap_hand_position_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
bool js_weap_hand_position_get_x(JSValueRef *vp);
bool js_weap_hand_position_get_y(JSValueRef *vp);
bool js_weap_hand_position_get_z(JSValueRef *vp);
bool js_weap_hand_position_set_x(JSValueRef *vp);
bool js_weap_hand_position_set_y(JSValueRef *vp);
bool js_weap_hand_position_set_z(JSValueRef *vp);

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

JSObject* script_add_weap_hand_position_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,weap_hand_position_class,"handPosition",weap_hand_position_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_weap_hand_position_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,weap_hand_position_props));
}

JSBool js_weap_hand_position_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,weap_hand_position_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_weap_hand_position_get_x(JSValueRef *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->hand.shift.x);
	
	return(TRUE);
}

bool js_weap_hand_position_get_y(JSValueRef *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->hand.shift.y);
	
	return(TRUE);
}

bool js_weap_hand_position_get_z(JSValueRef *vp)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(weap->hand.shift.z);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_weap_hand_position_set_x(JSValueRef *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->hand.shift.x=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_weap_hand_position_set_y(JSValueRef *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->hand.shift.y=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_weap_hand_position_set_z(JSValueRef *vp)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->hand.shift.z=script_value_to_int(*vp);
	
	return(TRUE);
}



