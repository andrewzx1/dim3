/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.watch object

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

JSValueRef js_obj_watch_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_watch_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_watch_get_objectId(JSContextRef cx);
JSValueRef js_obj_watch_get_objectName(JSContextRef cx);
JSValueRef js_obj_watch_get_objectIsPlayer(JSContextRef cx);
JSValueRef js_obj_watch_get_objectIsRemote(JSContextRef cx);
JSValueRef js_obj_watch_get_objectIsBot(JSContextRef cx);
JSValueRef js_obj_watch_get_objectIsPlayerRemoteBot(JSContextRef cx);
JSValueRef js_obj_watch_get_objectTeam(JSContextRef cx);
JSValueRef js_obj_watch_get_baseTeam(JSContextRef cx);
JSValueRef js_obj_watch_get_soundName(JSContextRef cx);
JSValueRef js_obj_watch_start_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_watch_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_watch_set_restrict_sight_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_watch_clear_restrict_sight_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_property	obj_watch_props[]={
							{"objectId",				js_obj_watch_get_objectId,					NULL},
							{"objectName",				js_obj_watch_get_objectName,				NULL},
							{"objectIsPlayer",			js_obj_watch_get_objectIsPlayer,			NULL},
							{"objectIsRemote",			js_obj_watch_get_objectIsRemote,			NULL},
							{"objectIsBot",				js_obj_watch_get_objectIsBot,				NULL},
							{"objectIsPlayerRemoteBot",	js_obj_watch_get_objectIsPlayerRemoteBot,	NULL},
							{"objectTeam",				js_obj_watch_get_objectTeam,				NULL},
							{"baseTeam",				js_obj_watch_get_baseTeam,					NULL},
							{"soundName",				js_obj_watch_get_soundName,					NULL},
							{0}};
							
script_js_function	obj_watch_functions[]={
							{"start",					js_obj_watch_start_func,					1},
							{"stop",					js_obj_watch_stop_func,						0},
							{"setRestrictSight",		js_obj_watch_set_restrict_sight_func,		1},
							{"clearRestrictSight",		js_obj_watch_clear_restrict_sight_func,		0},
							{0}};

JSClassRef			obj_watch_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_watch_object(void)
{
	obj_watch_class=script_create_class("obj_watch_class",js_obj_watch_get_property,js_obj_watch_set_property);
}

void script_free_obj_watch_object(void)
{
	script_free_class(obj_watch_class);
}

JSObjectRef script_add_obj_watch_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_watch_class,"watch",obj_watch_props,obj_watch_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_watch_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_watch_props));
}

bool js_obj_watch_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,obj_watch_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_watch_get_objectId(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->watch.obj_uid));
}

JSValueRef js_obj_watch_get_objectName(JSContextRef cx)
{
	obj_type		*obj,*watch_obj;

	obj=object_find_uid(js.attach.thing_uid);
	
	watch_obj=object_find_uid(obj->watch.obj_uid);
	if (watch_obj==NULL) return(script_null_to_value(cx));

	return(script_string_to_value(cx,watch_obj->name));
}

JSValueRef js_obj_watch_get_objectIsPlayer(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->watch.obj_uid==server.player_obj_uid));
}

JSValueRef js_obj_watch_get_objectIsRemote(JSContextRef cx)
{
	obj_type		*obj,*watch_obj;

	obj=object_find_uid(js.attach.thing_uid);
	
	watch_obj=object_find_uid(obj->watch.obj_uid);
	if (watch_obj==NULL) return(script_bool_to_value(cx,FALSE));
	
	return(script_bool_to_value(cx,watch_obj->remote.on));
}

JSValueRef js_obj_watch_get_objectIsBot(JSContextRef cx)
{
	obj_type		*obj,*watch_obj;

	obj=object_find_uid(js.attach.thing_uid);
	
	watch_obj=object_find_uid(obj->watch.obj_uid);
	if (watch_obj==NULL) return(script_bool_to_value(cx,FALSE));
	
	return(script_bool_to_value(cx,watch_obj->bot));
}

JSValueRef js_obj_watch_get_objectIsPlayerRemoteBot(JSContextRef cx)
{
	obj_type		*obj,*watch_obj;

	obj=object_find_uid(js.attach.thing_uid);
	
	if (obj->watch.obj_uid==server.player_obj_uid) return(script_bool_to_value(cx,TRUE));

	watch_obj=object_find_uid(obj->watch.obj_uid);
	if (watch_obj==NULL) return(script_bool_to_value(cx,FALSE));
	
	return(script_bool_to_value(cx,(watch_obj->remote.on) || (watch_obj->bot)));
}

JSValueRef js_obj_watch_get_objectTeam(JSContextRef cx)
{
	obj_type		*obj,*watch_obj;

	obj=object_find_uid(js.attach.thing_uid);
	
	watch_obj=object_find_uid(obj->watch.obj_uid);
	if (watch_obj==NULL) return(script_int_to_value(cx,sd_team_none));
	
	return(script_int_to_value(cx,watch_obj->team_idx+sd_team_none));
}

JSValueRef js_obj_watch_get_baseTeam(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->watch.base_team+sd_team_none));
}

JSValueRef js_obj_watch_get_soundName(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_string_to_value(cx,obj->watch.sound_name));
}

/* =======================================================

      Start/Stop Functions
      
======================================================= */

JSValueRef js_obj_watch_start_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	obj->watch.on=TRUE;
	obj->watch.dist=script_value_to_int(cx,argv[0]);
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_watch_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_clear_watch(&obj->watch);

	return(script_null_to_value(cx));
}

/* =======================================================

      Sight Restriction Functions
      
======================================================= */

JSValueRef js_obj_watch_set_restrict_sight_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	obj->watch.restrict_on=TRUE;
	obj->watch.restrict_ang=script_value_to_float(cx,argv[0]);
	
	return(script_null_to_value(cx));
}

JSValueRef js_obj_watch_clear_restrict_sight_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type	*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	obj->watch.restrict_on=FALSE;
	
	return(script_null_to_value(cx));
}
