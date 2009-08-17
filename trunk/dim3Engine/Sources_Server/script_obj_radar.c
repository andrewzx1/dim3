/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.radar object

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

extern server_type		server;
extern js_type			js;

JSValueRef js_obj_radar_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_radar_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_radar_get_on(JSContextRef cx);
JSValueRef js_obj_radar_get_icon(JSContextRef cx);
JSValueRef js_obj_radar_get_motionOnly(JSContextRef cx);
JSValueRef js_obj_radar_get_alwaysVisible(JSContextRef cx);
void js_obj_radar_set_on(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_radar_set_icon(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_radar_set_motionOnly(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_radar_set_alwaysVisible(JSContextRef cx,JSValueRef vp,JSValueRef *exception);

script_js_property	obj_radar_props[]={
							{"on",					js_obj_radar_get_on,				js_obj_radar_set_on},
							{"icon",				js_obj_radar_get_icon,				js_obj_radar_set_icon},
							{"motionOnly",			js_obj_radar_get_motionOnly,		js_obj_radar_set_motionOnly},
							{"alwaysVisible",		js_obj_radar_get_alwaysVisible,		js_obj_radar_set_alwaysVisible},
							{0}};
							
JSClassRef			obj_radar_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_radar_object(void)
{
	obj_radar_class=script_create_class("obj_radar_class",js_obj_radar_get_property,js_obj_radar_set_property);
}

void script_free_obj_radar_object(void)
{
	script_free_class(obj_radar_class);
}

JSObjectRef script_add_obj_radar_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_radar_class,"radar",obj_radar_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_radar_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_radar_props));
}

bool js_obj_radar_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,obj_radar_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_radar_get_on(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->radar.on));
}

JSValueRef js_obj_radar_get_icon(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_string_to_value(cx,obj->radar.icon));
}

JSValueRef js_obj_radar_get_motionOnly(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->radar.motion_only));
}

JSValueRef js_obj_radar_get_alwaysVisible(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->radar.always_visible));
}

/* =======================================================

      Setters
      
======================================================= */

void js_obj_radar_set_on(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->radar.on=script_value_to_bool(cx,vp);
}

void js_obj_radar_set_icon(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	char			err_str[256];
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	script_value_to_string(cx,vp,obj->radar.icon,name_str_len);
	if (!object_set_radar_icon(obj,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}
}

void js_obj_radar_set_motionOnly(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->radar.motion_only=script_value_to_bool(cx,vp);
}

void js_obj_radar_set_alwaysVisible(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->radar.always_visible=script_value_to_bool(cx,vp);
}


