/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.action object

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
extern setup_type			setup;
extern network_setup_type	net_setup;
extern js_type				js;

extern bool game_file_reload_ok(void);
extern bool game_file_reload(char *err_str);
extern void game_time_pause_end(void);

JSValueRef js_map_action_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_map_action_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_map_action_set_map_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_action_set_host_map_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_action_restart_map_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_action_restart_map_from_save_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_function	map_action_functions[]={
							{"setMap",				js_map_action_set_map_func,					3},
							{"setHostMap",			js_map_action_set_host_map_func,			0},
							{"restartMap",			js_map_action_restart_map_func,				0},
							{"restartMapFromSave",	js_map_action_restart_map_from_save_func,	0},
							{0}};

JSClassRef			map_action_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_action_object(void)
{
	map_action_class=script_create_class("map_action_class",js_map_action_get_property,js_map_action_set_property);
}

void script_free_map_action_object(void)
{
	script_free_class(map_action_class);
}

JSObjectRef script_add_map_action_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,map_action_class,"action",NULL,map_action_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_map_action_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,NULL));
}

bool js_map_action_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,NULL));
}

/* =======================================================

      Map Functions
      
======================================================= */

JSValueRef js_map_action_set_map_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
		// clients can not set maps
		
	if (net_setup.client.joined) {
		*exception=script_create_exception("setMap() illegal for client games");
		return(script_null_to_value());
	}
	
		// set the map
		
	script_value_to_string(argv[0],map.info.name,name_str_len);
	script_value_to_string(argv[1],map.info.player_start_name,name_str_len);
	script_value_to_string(argv[2],map.info.player_start_type,name_str_len);
	map.info.in_load=FALSE;

	server.map_change=TRUE;
	
	return(script_null_to_value());
}

JSValueRef js_map_action_set_host_map_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
		// clients can not set maps

	if (!net_setup.client.joined) {
		*exception=script_create_exception("setHostMap() illegal for normal games");
		return(script_null_to_value());
	}

		// set the map
		
	strcpy(map.info.name,map.info.host_name);
	map.info.player_start_name[0]=0x0;
	map.info.player_start_type[0]=0x0;
	map.info.in_load=FALSE;

	server.map_change=TRUE;
	
	return(script_null_to_value());
}

JSValueRef js_map_action_restart_map_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
		// clients can not restart maps
		
	if (net_setup.client.joined) {
		*exception=script_create_exception("restartMap() illegal for client games");
		return(script_null_to_value());
	}
	
		// use the last values to restart
		
	server.map_change=TRUE;
	server.skip_media=TRUE;
	
	return(script_null_to_value());
}

JSValueRef js_map_action_restart_map_from_save_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			err_str[256],err_str_2[256];
	
		// clients can not restart maps from saves
		
	if (net_setup.host.hosting) {
		*exception=script_create_exception("restartMapFromSave() illegal for client games");
		return(script_null_to_value());
	}
	
		// if no save game file, restart from map
		
	if (!game_file_reload_ok()) {
		server.map_change=TRUE;
		server.skip_media=TRUE;
		return(script_null_to_value());
	}
	
		// else reload
	
	if (!game_file_reload(err_str)) {
		sprintf(err_str_2,"Reload failed (%s)",err_str);
		*exception=script_create_exception(err_str_2);
		return(script_null_to_value());
	}
	
	game_time_pause_end();			// loaded files are in paused mode
	
	return(script_null_to_value());
}

