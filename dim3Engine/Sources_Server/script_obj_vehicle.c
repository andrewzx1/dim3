/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.vehicle object

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
#include "objects.h"

extern map_type			map;
extern server_type		server;
extern js_type			js;

JSValueRef js_obj_vehicle_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_vehicle_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_vehicle_get_on(JSContextRef cx);
JSValueRef js_obj_vehicle_get_hasOccupant(JSContextRef cx);
void js_obj_vehicle_set_on(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_vehicle_enter_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_vehicle_exit_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_vehicle_remove_occupant_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_vehicle_props[]={
							{"on",					js_obj_vehicle_get_on,				js_obj_vehicle_set_on,		kJSPropertyAttributeDontDelete},
							{"hasOccupant",			js_obj_vehicle_get_hasOccupant,		NULL,						kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	obj_vehicle_functions[]={
							{"enter",				js_obj_vehicle_enter_func,			kJSPropertyAttributeDontDelete},
							{"exit",				js_obj_vehicle_exit_func,			kJSPropertyAttributeDontDelete},
							{"removeOccupant",		js_obj_vehicle_exit_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_vehicle_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_vehicle_object(void)
{
	obj_vehicle_class=script_create_class("obj_vehicle_class",obj_vehicle_props,obj_vehicle_functions);
}

void script_free_obj_vehicle_object(void)
{
	script_free_class(obj_vehicle_class);
}

JSObjectRef script_add_obj_vehicle_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,obj_vehicle_class,"vehicle",obj_vehicle_props,obj_vehicle_functions));
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
	return(script_set_property(cx,j_obj,name,vp,exception,obj_vehicle_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_vehicle_get_on(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->vehicle.on));
}

JSValueRef js_obj_vehicle_get_hasOccupant(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->vehicle.attach_obj_uid!=-1));
}

/* =======================================================

      Setters
      
======================================================= */

void js_obj_vehicle_set_on(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->vehicle.on=script_value_to_bool(cx,vp);
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
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_obj_vehicle_exit_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			err_str[256];
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	if (!object_exit_vehicle(obj,FALSE,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

