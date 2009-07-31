/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: multiplayer.setting object

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

#include "objects.h"
#include "effects.h"
#include "scripts.h"
#include "physics.h"

extern server_type			server;
extern js_type				js;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;

JSBool js_multiplayer_setting_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_multiplayer_setting_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_multiplayer_setting_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_multiplayer_setting_get_type(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_multiplayer_setting_get_teamPlay(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_multiplayer_setting_get_characterName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_multiplayer_setting_get_characterModel(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_multiplayer_setting_get_characterParameter(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_multiplayer_setting_check_option_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_property	multiplayer_setting_props[]={
							{"on",					js_multiplayer_setting_get_on,					NULL},
							{"type",				js_multiplayer_setting_get_type,				NULL},
							{"teamPlay",			js_multiplayer_setting_get_teamPlay,			NULL},
							{"characterName",		js_multiplayer_setting_get_characterName,		NULL},
							{"characterModel",		js_multiplayer_setting_get_characterModel,		NULL},
							{"characterParameter",	js_multiplayer_setting_get_characterParameter,	NULL},
							{0}};

script_js_function	multiplayer_setting_functions[]={
							{"checkOption",			js_multiplayer_setting_check_option_func,		1},
							{0}};

JSClass				*multiplayer_setting_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_multiplayer_setting_object(void)
{
	multiplayer_setting_class=script_create_class("multiplayer_setting_class",js_multiplayer_setting_get_property,js_multiplayer_setting_set_property);
}

void script_free_multiplayer_setting_object(void)
{
	script_free_class(multiplayer_setting_class);
}

void script_add_multiplayer_setting_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,multiplayer_setting_class,"setting",multiplayer_setting_props,multiplayer_setting_functions);
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_multiplayer_setting_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,multiplayer_setting_props));
}

JSBool js_multiplayer_setting_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,multiplayer_setting_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_multiplayer_setting_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_bool_to_value(net_setup.client.joined);
	return(JS_TRUE);
}

JSBool js_multiplayer_setting_get_type(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
 	if (!net_setup.client.joined) {
		*vp=script_null_to_value();
	}
	else {
		*vp=script_string_to_value(net_setup.games[net_setup.game_idx].name);
	}

	return(JS_TRUE);
}

JSBool js_multiplayer_setting_get_teamPlay(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
 	if (!net_setup.client.joined) {
		*vp=script_bool_to_value(FALSE);
	}
	else {
		*vp=script_bool_to_value(net_setup.games[net_setup.game_idx].use_teams);
	}
	
	return(JS_TRUE);
}

JSBool js_multiplayer_setting_get_characterName(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (hud.model.nmodel==0) {
		*vp=script_string_to_value("Player");
	}
	else {
		*vp=script_string_to_value(hud.model.models[setup.network.player_model_idx].name);
	}

	return(JS_TRUE);
}

JSBool js_multiplayer_setting_get_characterModel(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (hud.model.nmodel==0) {
		*vp=script_string_to_value("Player");
	}
	else {
		*vp=script_string_to_value(hud.model.models[setup.network.player_model_idx].model_name);
	}

	return(JS_TRUE);
}

JSBool js_multiplayer_setting_get_characterParameter(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	if (hud.model.nmodel==0) {
		*vp=script_null_to_value();
	}
	else {
		*vp=script_string_to_value(hud.model.models[setup.network.player_model_idx].param);
	}

	return(JS_TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSBool js_multiplayer_setting_check_option_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				n;
	char			name[name_str_len];
	
		// all options are false if not in networking
		
	if (!net_setup.client.joined) {
		*rval=script_bool_to_value(FALSE);
		return(JS_TRUE);
	}

		// find if option is on

	script_value_to_string(argv[0],name,name_str_len);

	for (n=0;n!=setup.network.noption;n++) {
		if (strcasecmp(name,setup.network.options[n].name)==0) {
			*rval=script_bool_to_value(TRUE);
			return(JS_TRUE);
		}
	}

	*rval=script_bool_to_value(FALSE);
	return(JS_TRUE);
}

