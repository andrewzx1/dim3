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

JSValueRef js_map_setting_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_map_setting_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_map_setting_get_scale(void);
JSValueRef js_map_setting_get_gravity(void);
JSValueRef js_map_setting_get_resistance(void);
JSValueRef js_map_setting_get_multiplayer(void);
JSValueRef js_map_setting_get_multiplayerType(void);
JSValueRef js_map_setting_get_botSkill(void);
void js_map_setting_set_gravity(JSValueRef vp,JSValueRef *exception);
void js_map_setting_set_resistance(JSValueRef vp,JSValueRef *exception);
JSValueRef js_map_set_ambient_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_clear_ambient_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_check_option_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

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

JSObjectRef script_add_map_setting_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,map_setting_class,"setting",map_setting_props,map_setting_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_map_setting_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,map_setting_props));
}

bool js_map_setting_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,map_setting_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_map_setting_get_scale(void)
{
	return(script_int_to_value(map_enlarge));
}

JSValueRef js_map_setting_get_gravity(void)
{
	return(script_float_to_value(map.settings.gravity));
}

JSValueRef js_map_setting_get_resistance(void)
{
	return(script_float_to_value(map.settings.resistance));
}

JSValueRef js_map_setting_get_multiplayer(void)
{
	return(script_bool_to_value(net_setup.client.joined));
}

JSValueRef js_map_setting_get_multiplayerType(void)
{
	if (!net_setup.client.joined) return(script_null_to_value());

	return(script_string_to_value(net_setup.games[net_setup.game_idx].name));
}

JSValueRef js_map_setting_get_botSkill(void)
{
	return(script_int_to_value(setup.network.bot.skill));
}

/* =======================================================

      Setters
      
======================================================= */

void js_map_setting_set_gravity(JSValueRef vp,JSValueRef *exception)
{
	map.settings.gravity=script_value_to_float(vp);
}

void js_map_setting_set_resistance(JSValueRef vp,JSValueRef *exception)
{
	map.settings.resistance=script_value_to_float(vp);
}

/* =======================================================

      Methods
      
======================================================= */

JSValueRef js_map_set_ambient_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			pitch;
	char			name[name_str_len];
	
	script_value_to_string(argv[0],name,name_str_len);
	pitch=script_value_to_float(argv[1]);
	
	map_set_ambient(name,pitch);
	
	return(script_null_to_value());
}

JSValueRef js_map_clear_ambient_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	map_clear_ambient();
	return(script_null_to_value());
}

