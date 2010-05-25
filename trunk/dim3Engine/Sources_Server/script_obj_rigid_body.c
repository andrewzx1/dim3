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

JSValueRef js_obj_rigid_body_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_rigid_body_get_maxDropY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_rigid_body_get_resetFactorY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_rigid_body_get_smoothFactorY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_rigid_body_get_maxAngleX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_rigid_body_get_resetFactorX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_rigid_body_get_smoothFactorX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_rigid_body_get_maxAngleZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_rigid_body_get_resetFactorZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_obj_rigid_body_get_smoothFactorZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_rigid_body_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_rigid_body_set_maxDropY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_rigid_body_set_resetFactorY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_rigid_body_set_smoothFactorY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_rigid_body_set_maxAngleX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_rigid_body_set_resetFactorX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_rigid_body_set_smoothFactorX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_rigid_body_set_maxAngleZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_rigid_body_set_resetFactorZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_obj_rigid_body_set_smoothFactorZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		obj_rigid_body_props[]={
							{"on",					js_obj_rigid_body_get_on,				js_obj_rigid_body_set_on,				kJSPropertyAttributeDontDelete},
							{"maxDropY",			js_obj_rigid_body_get_maxDropY,			js_obj_rigid_body_set_maxDropY,			kJSPropertyAttributeDontDelete},
							{"resetFactorY",		js_obj_rigid_body_get_resetFactorY,		js_obj_rigid_body_set_resetFactorY,		kJSPropertyAttributeDontDelete},
							{"smoothFactorY",		js_obj_rigid_body_get_smoothFactorY,	js_obj_rigid_body_set_smoothFactorY,	kJSPropertyAttributeDontDelete},
							{"maxAngleX",			js_obj_rigid_body_get_maxAngleX,		js_obj_rigid_body_set_maxAngleX,		kJSPropertyAttributeDontDelete},
							{"resetFactorX",		js_obj_rigid_body_get_resetFactorX,		js_obj_rigid_body_set_resetFactorX,		kJSPropertyAttributeDontDelete},
							{"smoothFactorX",		js_obj_rigid_body_get_smoothFactorX,	js_obj_rigid_body_set_smoothFactorX,	kJSPropertyAttributeDontDelete},
							{"maxAngleZ",			js_obj_rigid_body_get_maxAngleZ,		js_obj_rigid_body_set_maxAngleZ,		kJSPropertyAttributeDontDelete},
							{"resetFactorZ",		js_obj_rigid_body_get_resetFactorZ,		js_obj_rigid_body_set_resetFactorZ,		kJSPropertyAttributeDontDelete},
							{"smoothFactorZ",		js_obj_rigid_body_get_smoothFactorZ,	js_obj_rigid_body_set_smoothFactorZ,	kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			obj_rigid_body_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_rigid_body_object(void)
{
	obj_rigid_body_class=script_create_class("obj_rigid_body_class",obj_rigid_body_props,NULL);
}

void script_free_obj_rigid_body_object(void)
{
	script_free_class(obj_rigid_body_class);
}

JSObjectRef script_add_obj_rigid_body_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,obj_rigid_body_class,"rigidBody"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_rigid_body_get_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_bool_to_value(cx,obj->rigid_body.on));
}

JSValueRef js_obj_rigid_body_get_maxDropY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_int_to_value(cx,obj->rigid_body.max_drop_y));
}

JSValueRef js_obj_rigid_body_get_resetFactorY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_float_to_value(cx,obj->rigid_body.reset_factor_y));
}

JSValueRef js_obj_rigid_body_get_smoothFactorY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_float_to_value(cx,obj->rigid_body.smooth_factor_y));
}

JSValueRef js_obj_rigid_body_get_maxAngleX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_float_to_value(cx,obj->rigid_body.max_ang_x));
}

JSValueRef js_obj_rigid_body_get_resetFactorX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_float_to_value(cx,obj->rigid_body.reset_factor_x));
}

JSValueRef js_obj_rigid_body_get_smoothFactorX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_float_to_value(cx,obj->rigid_body.smooth_factor_x));
}

JSValueRef js_obj_rigid_body_get_maxAngleZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_float_to_value(cx,obj->rigid_body.max_ang_z));
}

JSValueRef js_obj_rigid_body_get_resetFactorZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_float_to_value(cx,obj->rigid_body.reset_factor_z));
}

JSValueRef js_obj_rigid_body_get_smoothFactorZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	obj=object_script_lookup();
	return(script_float_to_value(cx,obj->rigid_body.smooth_factor_z));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_rigid_body_set_on(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_script_lookup();
	obj->rigid_body.on=script_value_to_bool(cx,vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_maxDropY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_script_lookup();
	obj->rigid_body.max_drop_y=script_value_to_int(cx,vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_resetFactorY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_script_lookup();
	obj->rigid_body.reset_factor_y=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_smoothFactorY(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_script_lookup();
	obj->rigid_body.smooth_factor_y=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_maxAngleX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_script_lookup();
	obj->rigid_body.max_ang_x=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_resetFactorX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_script_lookup();
	obj->rigid_body.reset_factor_x=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_smoothFactorX(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_script_lookup();
	obj->rigid_body.smooth_factor_x=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_maxAngleZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_script_lookup();
	obj->rigid_body.max_ang_z=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_resetFactorZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_script_lookup();
	obj->rigid_body.reset_factor_z=script_value_to_float(cx,vp);
	
	return(TRUE);
}

bool js_obj_rigid_body_set_smoothFactorZ(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_script_lookup();
	obj->rigid_body.smooth_factor_z=script_value_to_float(cx,vp);
	
	return(TRUE);
}

