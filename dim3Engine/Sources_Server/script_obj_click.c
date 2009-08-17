/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.click object

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

extern server_type		server;
extern js_type			js;

JSValueRef js_obj_click_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_click_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_click_get_crosshairUp(JSContextRef cx);
JSValueRef js_obj_click_get_crosshairDown(JSContextRef cx);
JSValueRef js_obj_click_get_objectId(JSContextRef cx);
JSValueRef js_obj_click_get_objectName(JSContextRef cx);
void js_obj_click_set_crosshairUp(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_click_set_crosshairDown(JSContextRef cx,JSValueRef vp,JSValueRef *exception);

script_js_property	obj_click_props[]={
							{"crosshairUp",			js_obj_click_get_crosshairUp,			js_obj_click_set_crosshairUp},
							{"crosshairDown",		js_obj_click_get_crosshairDown,			js_obj_click_set_crosshairDown},
							{"objectId",			js_obj_click_get_objectId,				NULL},
							{"objectName",			js_obj_click_get_objectName,			NULL},
							{0}};

JSClassRef			obj_click_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_click_object(void)
{
	obj_click_class=script_create_class("obj_click_class",js_obj_click_get_property,js_obj_click_set_property);
}

void script_free_obj_click_object(void)
{
	script_free_class(obj_click_class);
}

JSObjectRef script_add_obj_click_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_click_class,"click",obj_click_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_click_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_click_props));
}

bool js_obj_click_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,obj_click_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_click_get_crosshairUp(JSContextRef cx)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_string_to_value(cx,obj->click.crosshair_up_name));
}

JSValueRef js_obj_click_get_crosshairDown(JSContextRef cx)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_string_to_value(cx,obj->click.crosshair_down_name));
}

JSValueRef js_obj_click_get_objectId(JSContextRef cx)
{
	obj_type			*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->click.current_click_obj_uid));
}

JSValueRef js_obj_click_get_objectName(JSContextRef cx)
{
	obj_type			*obj,*click_obj;

	obj=object_find_uid(js.attach.thing_uid);

	if (obj->click.current_click_obj_uid==-1) return(script_null_to_value(cx));

	click_obj=object_find_uid(obj->click.current_click_obj_uid);
	return(script_string_to_value(cx,click_obj->name));
}

/* =======================================================

      Setters
      
======================================================= */

void js_obj_click_set_crosshairUp(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type			*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	script_value_to_string(cx,vp,obj->click.crosshair_up_name,name_str_len);
	object_attach_click_crosshair_up(obj);
}

void js_obj_click_set_crosshairDown(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type			*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	script_value_to_string(cx,vp,obj->click.crosshair_down_name,name_str_len);
	object_attach_click_crosshair_down(obj);
}

