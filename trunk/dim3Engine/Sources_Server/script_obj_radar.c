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

JSBool js_obj_radar_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_obj_radar_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
bool js_obj_radar_get_on(JSValueRef *vp);
bool js_obj_radar_get_icon(JSValueRef *vp);
bool js_obj_radar_get_motionOnly(JSValueRef *vp);
bool js_obj_radar_get_alwaysVisible(JSValueRef *vp);
bool js_obj_radar_set_on(JSValueRef *vp);
bool js_obj_radar_set_icon(JSValueRef *vp);
bool js_obj_radar_set_motionOnly(JSValueRef *vp);
bool js_obj_radar_set_alwaysVisible(JSValueRef *vp);

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

JSObject* script_add_obj_radar_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,obj_radar_class,"radar",obj_radar_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_obj_radar_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,obj_radar_props));
}

JSBool js_obj_radar_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,obj_radar_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_obj_radar_get_on(JSValueRef *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->radar.on);
	
	return(TRUE);
}

bool js_obj_radar_get_icon(JSValueRef *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(obj->radar.icon);
	
	return(TRUE);
}

bool js_obj_radar_get_motionOnly(JSValueRef *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->radar.motion_only);
	
	return(TRUE);
}

bool js_obj_radar_get_alwaysVisible(JSValueRef *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->radar.always_visible);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_radar_set_on(JSValueRef *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->radar.on=script_value_to_bool(*vp);
	
	return(TRUE);
}

bool js_obj_radar_set_icon(JSValueRef *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	script_value_to_string(*vp,obj->radar.icon,name_str_len);
	if (!object_set_radar_icon(obj)) return(FALSE);
	
	return(TRUE);
}

bool js_obj_radar_set_motionOnly(JSValueRef *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->radar.motion_only=script_value_to_bool(*vp);
	
	return(TRUE);
}

bool js_obj_radar_set_alwaysVisible(JSValueRef *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->radar.always_visible=script_value_to_bool(*vp);
	
	return(TRUE);
}


