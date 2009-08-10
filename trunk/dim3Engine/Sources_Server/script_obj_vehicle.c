/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.vehicle object

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

extern map_type			map;
extern server_type		server;
extern js_type			js;

JSValueRef js_obj_vehicle_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_vehicle_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_vehicle_get_on(void);
JSValueRef js_obj_vehicle_get_hasOccupant(void);
bool js_obj_vehicle_set_on(JSValueRef vp);
JSValueRef js_obj_vehicle_enter_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_vehicle_exit_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_vehicle_remove_occupant_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_property	obj_vehicle_props[]={
							{"on",					js_obj_vehicle_get_on,				js_obj_vehicle_set_on},
							{"hasOccupant",			js_obj_vehicle_get_hasOccupant,		NULL},
							{0}};

script_js_function	obj_vehicle_functions[]={
							{"enter",				js_obj_vehicle_enter_func,			0},
							{"exit",				js_obj_vehicle_exit_func,			0},
							{"removeOccupant",		js_obj_vehicle_exit_func,			0},
							{0}};

JSClassRef			obj_vehicle_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_vehicle_object(void)
{
	obj_vehicle_class=script_create_class("obj_vehicle_class",js_obj_vehicle_get_property,js_obj_vehicle_set_property);
}

void script_free_obj_vehicle_object(void)
{
	script_free_class(obj_vehicle_class);
}

JSObjectRef script_add_obj_vehicle_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_vehicle_class,"vehicle",obj_vehicle_props,obj_vehicle_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_vehicle_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_vehicle_props));
}

bool js_obj_vehicle_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,obj_vehicle_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_vehicle_get_on(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->vehicle.on);
	
	return(TRUE);
}

JSValueRef js_obj_vehicle_get_hasOccupant(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->vehicle.attach_obj_uid!=-1);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_vehicle_set_on(JSValueRef vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->vehicle.on=script_value_to_bool(*vp);
	
	return(TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_obj_vehicle_enter_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			err_str[256];
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	if (!object_enter_vehicle(obj,err_str)) {
		JS_ReportError(js.cx,"Vehicle: %s",err_str);
		return(FALSE);
	}

	return(TRUE);
}

JSValueRef js_obj_vehicle_exit_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			err_str[256];
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	if (!object_exit_vehicle(obj,FALSE,err_str)) {
		JS_ReportError(js.cx,"Vehicle: %s",err_str);
		return(FALSE);
	}

	return(TRUE);
}

