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

JSValueRef js_game_setting_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_game_setting_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_game_setting_get_type(void);
JSValueRef js_game_setting_get_multiplayer(void);
JSValueRef js_game_setting_get_skill(void);

script_js_property	game_setting_props[]={
							{"type",				js_game_setting_get_type,				NULL},
							{"multiplayer",			js_game_setting_get_multiplayer,		NULL},
							{"skill",				js_game_setting_get_skill,				NULL},
							{0}};

JSClassRef			game_setting_class;

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

JSObjectRef script_add_game_setting_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,game_setting_class,"setting",game_setting_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_game_setting_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,game_setting_props));
}

bool js_game_setting_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,game_setting_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_game_setting_get_type(void)
{
	if (!net_setup.client.joined) return(script_null_to_value());
	
	return(script_string_to_value(net_setup.games[net_setup.game_idx].name));
}

JSValueRef js_game_setting_get_multiplayer(void)
{
	return(script_bool_to_value(net_setup.client.joined));
}

JSValueRef js_game_setting_get_skill(void)
{
	return(script_int_to_value(server.skill));
}

