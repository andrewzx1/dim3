/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.touch object

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

JSValueRef js_obj_touch_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_touch_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_touch_get_objectId(void);
JSValueRef js_obj_touch_get_objectName(void);
JSValueRef js_obj_touch_get_objectIsPlayer(void);
JSValueRef js_obj_touch_get_stand(void);

script_js_property	obj_touch_props[]={
							{"objectId",			js_obj_touch_get_objectId,			NULL},
							{"objectName",			js_obj_touch_get_objectName,		NULL},
							{"objectIsPlayer",		js_obj_touch_get_objectIsPlayer,	NULL},
							{"stand",				js_obj_touch_get_stand,				NULL},
							{0}};

JSClassRef			obj_touch_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_touch_object(void)
{
	obj_touch_class=script_create_class("obj_touch_class",js_obj_touch_get_property,js_obj_touch_set_property);
}

void script_free_obj_touch_object(void)
{
	script_free_class(obj_touch_class);
}

JSObjectRef script_add_obj_touch_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_touch_class,"touch",obj_touch_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_touch_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_touch_props));
}

bool js_obj_touch_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,obj_touch_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_touch_get_objectId(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->touch.obj_uid);
	
	return(TRUE);
}

JSValueRef js_obj_touch_get_objectName(void)
{
	obj_type		*obj,*touch_obj;

	obj=object_find_uid(js.attach.thing_uid);
	
	touch_obj=object_find_uid(obj->touch.obj_uid);
	if (touch_obj==NULL) {
		*vp=script_null_to_value();
	}
	else {
		*vp=script_string_to_value(touch_obj->name);
	}
	
	return(TRUE);
}

JSValueRef js_obj_touch_get_objectIsPlayer(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->touch.obj_uid==server.player_obj_uid);
	
	return(TRUE);
}

JSValueRef js_obj_touch_get_stand(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->touch.stand);
	
	return(TRUE);
}


