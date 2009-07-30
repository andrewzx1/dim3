/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: game.setting object

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

JSBool js_game_setting_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_game_setting_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_game_setting_get_type(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_game_setting_get_multiplayer(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_game_setting_get_skill(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

script_js_property	game_setting_props[]={
							{"type",				js_game_setting_get_type,				NULL},
							{"multiplayer",			js_game_setting_get_multiplayer,		NULL},
							{"skill",				js_game_setting_get_skill,				NULL},
							{0}};

JSClass				*game_setting_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_game_setting_object(void)
{
	game_setting_class=script_create_class("game_setting_class",js_game_setting_get_property,js_game_setting_set_property);
}

void script_free_game_setting_object(void)
{
	script_free_class(game_setting_class);
}

void script_add_game_setting_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,game_setting_class,"setting",game_setting_props,NULL);
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_game_setting_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,game_setting_props));
}

JSBool js_game_setting_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,game_setting_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_game_setting_get_type(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (!net_setup.client.joined) {
		*vp=script_null_to_value();
	}
	else {
		*vp=script_string_to_value(net_setup.games[net_setup.game_idx].name);
	}
	
	return(JS_TRUE);
}

JSBool js_game_setting_get_multiplayer(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_bool_to_value(net_setup.client.joined);
	return(JS_TRUE);
}

JSBool js_game_setting_get_skill(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_int_to_value(server.skill);
	return(JS_TRUE);
}

