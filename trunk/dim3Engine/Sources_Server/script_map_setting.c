/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.setting object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

extern map_type				map;
extern server_type			server;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern void map_set_ambient(char *name,float pitch);
extern void map_clear_ambient(void);

JSBool js_map_setting_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_setting_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_setting_get_scale(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_setting_get_gravity(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_setting_get_resistance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_setting_get_multiplayer(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_setting_get_multiplayerType(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_setting_get_botSkill(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_setting_set_gravity(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_setting_set_resistance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_set_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_clear_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_check_option_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_property	map_setting_props[]={
							{"scale",				js_map_setting_get_scale,				NULL},
							{"gravity",				js_map_setting_get_gravity,				js_map_setting_set_gravity},
							{"resistance",			js_map_setting_get_resistance,			js_map_setting_set_resistance},
							{"multiplayer",			js_map_setting_get_multiplayer,			NULL},
							{"multiplayerType",		js_map_setting_get_multiplayerType,		NULL},
							{"botSkill",			js_map_setting_get_botSkill,			NULL},
							{0}};
							
script_js_function	map_setting_functions[]={
							{"setAmbient",			js_map_set_ambient_func,				2},
							{"clearAmbient",		js_map_clear_ambient_func,				0},
							{0}};

JSClassRef			map_setting_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_setting_object(void)
{
	map_setting_class=script_create_class("map_setting_class",js_map_setting_get_property,js_map_setting_set_property);
}

void script_free_map_setting_object(void)
{
	script_free_class(map_setting_class);
}

JSObject* script_add_map_setting_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,map_setting_class,"setting",map_setting_props,map_setting_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_map_setting_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,map_setting_props));
}

JSBool js_map_setting_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,map_setting_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_map_setting_get_scale(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_int_to_value(map_enlarge);
	return(JS_TRUE);
}

bool js_map_setting_get_gravity(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_float_to_value(map.settings.gravity);
	return(JS_TRUE);
}

bool js_map_setting_get_resistance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_float_to_value(map.settings.resistance);
	return(JS_TRUE);
}

bool js_map_setting_get_multiplayer(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_bool_to_value(net_setup.client.joined);
	return(JS_TRUE);
}

bool js_map_setting_get_multiplayerType(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!net_setup.client.joined) {
		*vp=script_null_to_value();
	}
	else {
		*vp=script_string_to_value(net_setup.games[net_setup.game_idx].name);
	}
	
	return(JS_TRUE);
}

bool js_map_setting_get_botSkill(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_int_to_value(setup.network.bot.skill);
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_map_setting_set_gravity(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	map.settings.gravity=script_value_to_float(*vp);
	return(JS_TRUE);
}

bool js_map_setting_set_resistance(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	map.settings.resistance=script_value_to_float(*vp);
	return(JS_TRUE);
}

/* =======================================================

      Methods
      
======================================================= */

JSBool js_map_set_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	float			pitch;
	char			name[name_str_len];
	
	script_value_to_string(argv[0],name,name_str_len);
	pitch=script_value_to_float(argv[1]);
	
	map_set_ambient(name,pitch);
	
	return(JS_TRUE);
}

JSBool js_map_clear_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	map_clear_ambient();
	return(JS_TRUE);
}

