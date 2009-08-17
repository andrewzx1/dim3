/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: weap.recoil object

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

JSValueRef js_weap_recoil_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_weap_recoil_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_recoil_get_minX(JSContextRef cx);
JSValueRef js_weap_recoil_get_maxX(JSContextRef cx);
JSValueRef js_weap_recoil_get_resetX(JSContextRef cx);
JSValueRef js_weap_recoil_get_minY(JSContextRef cx);
JSValueRef js_weap_recoil_get_maxY(JSContextRef cx);
JSValueRef js_weap_recoil_get_resetY(JSContextRef cx);
JSValueRef js_weap_recoil_get_minZ(JSContextRef cx);
JSValueRef js_weap_recoil_get_maxZ(JSContextRef cx);
JSValueRef js_weap_recoil_get_resetZ(JSContextRef cx);
JSValueRef js_weap_recoil_get_resetLook(JSContextRef cx);
JSValueRef js_weap_recoil_get_resetTurn(JSContextRef cx);
void js_weap_recoil_set_minX(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_recoil_set_maxX(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_recoil_set_resetX(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_recoil_set_minY(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_recoil_set_maxY(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_recoil_set_resetY(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_recoil_set_minZ(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_recoil_set_maxZ(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_recoil_set_resetZ(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_recoil_set_resetLook(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_weap_recoil_set_resetTurn(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
JSValueRef js_weap_recoil_go_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_property	weap_recoil_props[]={
							{"minX",				js_weap_recoil_get_minX,			js_weap_recoil_set_minX},
							{"maxX",				js_weap_recoil_get_maxX,			js_weap_recoil_set_maxX},
							{"resetX",				js_weap_recoil_get_resetX,			js_weap_recoil_set_resetX},
							{"minY",				js_weap_recoil_get_minY,			js_weap_recoil_set_minY},
							{"maxY",				js_weap_recoil_get_maxY,			js_weap_recoil_set_maxY},
							{"resetY",				js_weap_recoil_get_resetY,			js_weap_recoil_set_resetY},
							{"minZ",				js_weap_recoil_get_minZ,			js_weap_recoil_set_minZ},
							{"maxZ",				js_weap_recoil_get_maxZ,			js_weap_recoil_set_maxZ},
							{"resetZ",				js_weap_recoil_get_resetZ,			js_weap_recoil_set_resetZ},
							{"resetLook",			js_weap_recoil_get_resetLook,		js_weap_recoil_set_resetLook},
							{"resetTurn",			js_weap_recoil_get_resetTurn,		js_weap_recoil_set_resetTurn},
							{0}};
							
script_js_function	weap_recoil_functions[]={
							{"go",					js_weap_recoil_go_func,				0},
							{0}};

JSClassRef			weap_recoil_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_weap_recoil_object(void)
{
	weap_recoil_class=script_create_class("weap_recoil_class",js_weap_recoil_get_property,js_weap_recoil_set_property);
}

void script_free_weap_recoil_object(void)
{
	script_free_class(weap_recoil_class);
}

JSObjectRef script_add_weap_recoil_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,weap_recoil_class,"recoil",weap_recoil_props,weap_recoil_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_weap_recoil_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,weap_recoil_props));
}

bool js_weap_recoil_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,weap_recoil_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_weap_recoil_get_minX(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,weap->recoil.min_ang.x));
}

JSValueRef js_weap_recoil_get_maxX(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,weap->recoil.max_ang.x));
}

JSValueRef js_weap_recoil_get_resetX(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,weap->recoil.reset_ang.x));
}

JSValueRef js_weap_recoil_get_minY(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,weap->recoil.min_ang.y));
}

JSValueRef js_weap_recoil_get_maxY(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,weap->recoil.max_ang.y));
}

JSValueRef js_weap_recoil_get_resetY(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,weap->recoil.reset_ang.y));
}

JSValueRef js_weap_recoil_get_minZ(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,weap->recoil.min_ang.z));
}

JSValueRef js_weap_recoil_get_maxZ(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,weap->recoil.max_ang.z));
}

JSValueRef js_weap_recoil_get_resetZ(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,weap->recoil.reset_ang.z));
}

JSValueRef js_weap_recoil_get_resetLook(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,weap->recoil.look_reset));
}

JSValueRef js_weap_recoil_get_resetTurn(JSContextRef cx)
{
	weapon_type		*weap;

	weap=weapon_find_uid(js.attach.thing_uid);
	return(script_float_to_value(cx,weap->recoil.turn_reset));
}

/* =======================================================

      Setters
      
======================================================= */

void js_weap_recoil_set_minX(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->recoil.min_ang.x=script_value_to_float(cx,vp);
}

void js_weap_recoil_set_maxX(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->recoil.max_ang.x=script_value_to_float(cx,vp);
}

void js_weap_recoil_set_resetX(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->recoil.reset_ang.x=fabsf(script_value_to_float(cx,vp));
}

void js_weap_recoil_set_minY(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->recoil.min_ang.y=script_value_to_float(cx,vp);
}

void js_weap_recoil_set_maxY(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->recoil.max_ang.y=script_value_to_float(cx,vp);
}

void js_weap_recoil_set_resetY(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->recoil.reset_ang.y=fabsf(script_value_to_float(cx,vp));
}

void js_weap_recoil_set_minZ(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->recoil.min_ang.z=script_value_to_float(cx,vp);
}

void js_weap_recoil_set_maxZ(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->recoil.max_ang.z=script_value_to_float(cx,vp);
}

void js_weap_recoil_set_resetZ(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->recoil.reset_ang.z=fabsf(script_value_to_float(cx,vp));
}

void js_weap_recoil_set_resetLook(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->recoil.look_reset=fabsf(script_value_to_float(cx,vp));
}

void js_weap_recoil_set_resetTurn(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	weapon_type		*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	weap->recoil.turn_reset=fabsf(script_value_to_float(cx,vp));
}

/* =======================================================

      Recoil Functions
      
======================================================= */

JSValueRef js_weap_recoil_go_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type			*obj;
	weapon_type			*weap;
	
	weap=weapon_find_uid(js.attach.thing_uid);
	obj=object_find_uid(weap->obj_uid);
	
	weapon_recoil(obj,weap);

	return(script_null_to_value(cx));
}

