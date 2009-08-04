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

JSBool js_obj_watch_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_watch_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_watch_get_objectId(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_watch_get_objectName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_watch_get_objectIsPlayer(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_watch_get_objectIsRemote(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_watch_get_objectIsBot(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_watch_get_objectIsPlayerRemoteBot(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_watch_get_objectTeam(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_watch_get_baseTeam(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_watch_get_soundName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_watch_start_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_watch_stop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_watch_set_restrict_sight_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_watch_clear_restrict_sight_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

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

JSObject* script_add_obj_watch_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,obj_watch_class,"watch",obj_watch_props,obj_watch_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_obj_watch_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,obj_watch_props));
}

JSBool js_obj_watch_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,obj_watch_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_obj_watch_get_objectId(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->watch.obj_uid);
	
	return(JS_TRUE);
}

bool js_obj_watch_get_objectName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj,*watch_obj;

	obj=object_find_uid(js.attach.thing_uid);
	
	watch_obj=object_find_uid(obj->watch.obj_uid);
	if (watch_obj==NULL) {
		*vp=script_null_to_value();
	}
	else {
		*vp=script_string_to_value(watch_obj->name);
	}
	
	return(JS_TRUE);
}

bool js_obj_watch_get_objectIsPlayer(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->watch.obj_uid==server.player_obj_uid);
	
	return(JS_TRUE);
}

bool js_obj_watch_get_objectIsRemote(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj,*watch_obj;

	obj=object_find_uid(js.attach.thing_uid);
	
	watch_obj=object_find_uid(obj->watch.obj_uid);
	if (watch_obj==NULL) {
		*vp=script_bool_to_value(FALSE);
	}
	else {
		*vp=script_bool_to_value(watch_obj->remote.on);
	}
	
	return(JS_TRUE);
}

bool js_obj_watch_get_objectIsBot(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj,*watch_obj;

	obj=object_find_uid(js.attach.thing_uid);
	
	watch_obj=object_find_uid(obj->watch.obj_uid);
	if (watch_obj==NULL) {
		*vp=script_bool_to_value(FALSE);
	}
	else {
		*vp=script_bool_to_value(watch_obj->bot);
	}
	
	return(JS_TRUE);
}

bool js_obj_watch_get_objectIsPlayerRemoteBot(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj,*watch_obj;

	obj=object_find_uid(js.attach.thing_uid);
	
	if (obj->watch.obj_uid==server.player_obj_uid) {
		*vp=script_bool_to_value(TRUE);
	}
	else {
		watch_obj=object_find_uid(obj->watch.obj_uid);
		if (watch_obj==NULL) {
			*vp=script_bool_to_value(FALSE);
		}
		else {
			*vp=script_bool_to_value((watch_obj->remote.on) || (watch_obj->bot));
		}
	}
	
	return(JS_TRUE);
}

bool js_obj_watch_get_objectTeam(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj,*watch_obj;

	obj=object_find_uid(js.attach.thing_uid);
	
	watch_obj=object_find_uid(obj->watch.obj_uid);
	if (watch_obj==NULL) {
		*vp=script_int_to_value(sd_team_none);
	}
	else {
		*vp=script_int_to_value(watch_obj->team_idx+sd_team_none);
	}
	
	return(JS_TRUE);
}

bool js_obj_watch_get_baseTeam(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->watch.base_team+sd_team_none);
	
	return(JS_TRUE);
}

bool js_obj_watch_get_soundName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(obj->watch.sound_name);
	
	return(JS_TRUE);
}

/* =======================================================

      Start/Stop Functions
      
======================================================= */

JSBool js_obj_watch_start_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type	*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	obj->watch.on=TRUE;
	obj->watch.dist=script_value_to_int(argv[0]);
	
	return(JS_TRUE);
}

JSBool js_obj_watch_stop_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type	*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_clear_watch(&obj->watch);

	return(JS_TRUE);
}

/* =======================================================

      Sight Restriction Functions
      
======================================================= */

JSBool js_obj_watch_set_restrict_sight_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type	*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	obj->watch.restrict_on=TRUE;
	obj->watch.restrict_ang=script_value_to_float(argv[0]);
	
	return(JS_TRUE);
}

JSBool js_obj_watch_clear_restrict_sight_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type	*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	obj->watch.restrict_on=FALSE;
	
	return(JS_TRUE);
}
