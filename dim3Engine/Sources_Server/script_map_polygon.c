/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.polygon object

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

/*
JSValueRef js_map_polygon_get_facing_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_polygon_get_distance_to_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_polygon_get_normal_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_polygon_get_dot_product_to_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
*/

JSStaticFunction	map_polygon_functions[]={
	/*
							{"setMap",				js_map_action_set_map_func,					kJSPropertyAttributeDontDelete},
							{"setHostMap",			js_map_action_set_host_map_func,			kJSPropertyAttributeDontDelete},
							{"restartMap",			js_map_action_restart_map_func,				kJSPropertyAttributeDontDelete},
							{"restartMapFromSave",	js_map_action_restart_map_from_save_func,	kJSPropertyAttributeDontDelete},
	*/
							{0,0,0}};

JSClassRef			map_polygon_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_polygon_object(void)
{
	map_polygon_class=script_create_class("map_polygon_class",NULL,map_polygon_functions);
}

void script_free_map_polygon_object(void)
{
	script_free_class(map_polygon_class);
}

JSObjectRef script_add_map_polygon_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,map_polygon_class,"polygon"));
}

/* =======================================================

      Map Functions
      
======================================================= */

/*
JSValueRef js_map_action_set_map_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
		// networked games can not call this API
		
	if (net_setup.mode!=net_mode_none) {
		*exception=script_create_exception(cx,"setMap() illegal for client games");
		return(script_null_to_value(cx));
	}
	
		// set the map
		
	script_value_to_string(cx,argv[0],map.info.name,name_str_len);
	script_value_to_string(cx,argv[1],map.info.player_start_name,name_str_len);

	server.map_change=TRUE;
	
	return(script_null_to_value(cx));
}

JSValueRef js_map_action_set_host_map_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
		// non-network games can not call this API

	if (net_setup.mode==net_mode_none) {
		*exception=script_create_exception(cx,"setHostMap() illegal for normal games");
		return(script_null_to_value(cx));
	}

		// set the map
		
	strcpy(map.info.name,map.info.host_name);
	map.info.player_start_name[0]=0x0;

	server.map_change=TRUE;
	
	return(script_null_to_value(cx));
}

JSValueRef js_map_action_restart_map_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
		// networked games can not call this API
		
	if (net_setup.mode!=net_mode_none) {
		*exception=script_create_exception(cx,"restartMap() illegal for client games");
		return(script_null_to_value(cx));
	}
	
		// use the last values to restart
		
	server.map_change=TRUE;
	server.skip_media=TRUE;
	
	return(script_null_to_value(cx));
}

JSValueRef js_map_action_restart_map_from_save_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			err_str[256],err_str_2[256];
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
		// networked games can not call this API
		
	if (net_setup.mode!=net_mode_none) {
		*exception=script_create_exception(cx,"restartMapFromSave() illegal for client games");
		return(script_null_to_value(cx));
	}
	
		// if no save game file, restart from map
		
	if (!game_file_reload_ok()) {
		server.map_change=TRUE;
		server.skip_media=TRUE;
		return(script_null_to_value(cx));
	}
	
		// else reload
	
	if (!game_file_reload(err_str)) {
		sprintf(err_str_2,"Reload failed (%s)",err_str);
		*exception=script_create_exception(cx,err_str_2);
		return(script_null_to_value(cx));
	}
	
	game_time_pause_end();			// loaded files are in paused mode
	
	return(script_null_to_value(cx));
}
*/
