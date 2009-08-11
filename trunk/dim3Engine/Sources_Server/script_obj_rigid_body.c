/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.rigidBody object

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

extern js_type			js;

JSValueRef js_obj_rigid_body_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_rigid_body_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_rigid_body_get_on(void);
JSValueRef js_obj_rigid_body_get_maxDropY(void);
JSValueRef js_obj_rigid_body_get_resetFactorY(void);
JSValueRef js_obj_rigid_body_get_smoothFactorY(void);
JSValueRef js_obj_rigid_body_get_maxAngleX(void);
JSValueRef js_obj_rigid_body_get_resetFactorX(void);
JSValueRef js_obj_rigid_body_get_smoothFactorX(void);
JSValueRef js_obj_rigid_body_get_maxAngleZ(void);
JSValueRef js_obj_rigid_body_get_resetFactorZ(void);
JSValueRef js_obj_rigid_body_get_smoothFactorZ(void);
bool js_obj_rigid_body_set_on(JSValueRef vp);
bool js_obj_rigid_body_set_maxDropY(JSValueRef vp);
bool js_obj_rigid_body_set_resetFactorY(JSValueRef vp);
bool js_obj_rigid_body_set_smoothFactorY(JSValueRef vp);
bool js_obj_rigid_body_set_maxAngleX(JSValueRef vp);
bool js_obj_rigid_body_set_resetFactorX(JSValueRef vp);
bool js_obj_rigid_body_set_smoothFactorX(JSValueRef vp);
bool js_obj_rigid_body_set_maxAngleZ(JSValueRef vp);
bool js_obj_rigid_body_set_resetFactorZ(JSValueRef vp);
bool js_obj_rigid_body_set_smoothFactorZ(JSValueRef vp);

script_js_property	obj_rigid_body_props[]={
							{"on",					js_obj_rigid_body_get_on,				js_obj_rigid_body_set_on},
							{"maxDropY",			js_obj_rigid_body_get_maxDropY,			js_obj_rigid_body_set_maxDropY},
							{"resetFactorY",		js_obj_rigid_body_get_resetFactorY,		js_obj_rigid_body_set_resetFactorY},
							{"smoothFactorY",		js_obj_rigid_body_get_smoothFactorY,	js_obj_rigid_body_set_smoothFactorY},
							{"maxAngleX",			js_obj_rigid_body_get_maxAngleX,		js_obj_rigid_body_set_maxAngleX},
							{"resetFactorX",		js_obj_rigid_body_get_resetFactorX,		js_obj_rigid_body_set_resetFactorX},
							{"smoothFactorX",		js_obj_rigid_body_get_smoothFactorX,	js_obj_rigid_body_set_smoothFactorX},
							{"maxAngleZ",			js_obj_rigid_body_get_maxAngleZ,		js_obj_rigid_body_set_maxAngleZ},
							{"resetFactorZ",		js_obj_rigid_body_get_resetFactorZ,		js_obj_rigid_body_set_resetFactorZ},
							{"smoothFactorZ",		js_obj_rigid_body_get_smoothFactorZ,	js_obj_rigid_body_set_smoothFactorZ},
							{0}};

JSClassRef			obj_rigid_body_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_rigid_body_object(void)
{
	obj_rigid_body_class=script_create_class("obj_rigid_body_class",js_obj_rigid_body_get_property,js_obj_rigid_body_set_property);
}

void script_free_obj_rigid_body_object(void)
{
	script_free_class(obj_rigid_body_class);
}

JSObjectRef script_add_obj_rigid_body_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_rigid_body_class,"rigidBody",obj_rigid_body_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_rigid_body_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_rigid_body_props));
}

bool js_obj_rigid_body_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,obj_rigid_body_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_rigid_body_get_on(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->rigid_body.on);
	
	return(TRUE);
}

JSValueRef js_obj_rigid_body_get_maxDropY(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->rigid_body.max_drop_y);
	
	return(TRUE);
}

JSValueRef js_obj_rigid_body_get_resetFactorY(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->rigid_body.reset_factor_y);
	
	return(TRUE);
}

JSValueRef js_obj_rigid_body_get_smoothFactorY(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->rigid_body.smooth_factor_y);
	
	return(TRUE);
}

JSValueRef js_obj_rigid_body_get_maxAngleX(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->rigid_body.max_ang_x);
	
	return(TRUE);
}

JSValueRef js_obj_rigid_body_get_resetFactorX(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->rigid_body.reset_factor_x);
	
	return(TRUE);
}

JSValueRef js_obj_rigid_body_get_smoothFactorX(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->rigid_body.smooth_factor_x);
	
	return(TRUE);
}

JSValueRef js_obj_rigid_body_get_maxAngleZ(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->rigid_body.max_ang_z);
	
	return(TRUE);
}

JSValueRef js_obj_rigid_body_get_resetFactorZ(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->rigid_body.reset_factor_z);
	
	return(TRUE);
}

JSValueRef js_obj_rigid_body_get_smoothFactorZ(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->rigid_body.smooth_factor_z);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_rigid_body_set_on(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.on=script_value_to_bool(*vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_maxDropY(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.max_drop_y=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_resetFactorY(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.reset_factor_y=script_value_to_float(*vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_smoothFactorY(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.smooth_factor_y=script_value_to_float(*vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_maxAngleX(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.max_ang_x=script_value_to_float(*vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_resetFactorX(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.reset_factor_x=script_value_to_float(*vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_smoothFactorX(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.smooth_factor_x=script_value_to_float(*vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_maxAngleZ(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.max_ang_z=script_value_to_float(*vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_resetFactorZ(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.reset_factor_z=script_value_to_float(*vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_smoothFactorZ(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->rigid_body.smooth_factor_z=script_value_to_float(*vp);
	
	return(TRUE);
}

