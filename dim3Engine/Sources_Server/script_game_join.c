/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: game.join object

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"
#include "objects.h"

extern server_type			server;
extern js_type				js;
extern network_setup_type	net_setup;

extern int					game_obj_rule_idx;

JSValueRef js_game_join_get_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_game_join_get_team(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_game_join_set_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_game_join_set_team_even_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_game_join_clear_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_game_join_count_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_game_join_set_spawn_spot_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_game_join_set_spawn_spot_to_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_game_join_clear_spawn_spot_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		game_join_props[]={
							{"name",				js_game_join_get_name,						NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"team",				js_game_join_get_team,						NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	game_join_functions[]={
							{"setTeam",				js_game_join_set_team_func,					kJSPropertyAttributeDontDelete},
							{"setTeamEven",			js_game_join_set_team_even_func,			kJSPropertyAttributeDontDelete},
							{"clearTeam",			js_game_join_clear_team_func,				kJSPropertyAttributeDontDelete},
							{"countTeam",			js_game_join_count_team_func,				kJSPropertyAttributeDontDelete},
							{"setSpawnSpot",		js_game_join_set_spawn_spot_func,			kJSPropertyAttributeDontDelete},
							{"setSpawnSpotToTeam",	js_game_join_set_spawn_spot_to_team_func,	kJSPropertyAttributeDontDelete},
							{"clearSpawnSpot",		js_game_join_clear_spawn_spot_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			game_join_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_game_join_object(void)
{
	game_join_class=script_create_class("game_join_class",game_join_props,game_join_functions);
}

void script_free_game_join_object(void)
{
	script_free_class(game_join_class);
}

JSObjectRef script_add_game_join_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,game_join_class,"join"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_game_join_get_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	if (game_obj_rule_idx==-1) return(script_null_to_value(cx));

	obj=server.obj_list.objs[game_obj_rule_idx];
	return(script_string_to_value(cx,obj->name));
}

JSValueRef js_game_join_get_team(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	if (game_obj_rule_idx==-1) return(script_null_to_value(cx));

	obj=server.obj_list.objs[game_obj_rule_idx];
    return(script_int_to_value(cx,obj->team_idx+sd_team_none));
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_game_join_set_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	if (game_obj_rule_idx==-1) return(script_null_to_value(cx));

	obj=server.obj_list.objs[game_obj_rule_idx];
	obj->team_idx=script_value_to_int(cx,argv[0])-sd_team_none;

	return(script_null_to_value(cx));
}

JSValueRef js_game_join_set_team_even_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	if (game_obj_rule_idx==-1) return(script_null_to_value(cx));

	obj=server.obj_list.objs[game_obj_rule_idx];
	object_set_even_team(obj);

	return(script_null_to_value(cx));
}

JSValueRef js_game_join_clear_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	if (game_obj_rule_idx==-1) return(script_null_to_value(cx));

	obj=server.obj_list.objs[game_obj_rule_idx];
	obj->team_idx=net_team_none;

	return(script_null_to_value(cx));
}

JSValueRef js_game_join_count_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int			team_idx;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	team_idx=script_value_to_int(cx,argv[0])-sd_team_none;
	return(script_int_to_value(cx,object_count_team(team_idx,game_obj_rule_idx)));
}

JSValueRef js_game_join_set_spawn_spot_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	if (game_obj_rule_idx==-1) return(script_null_to_value(cx));

	obj=server.obj_list.objs[game_obj_rule_idx];
	script_value_to_string(cx,argv[0],obj->spawn_spot_name,name_str_len);

	return(script_null_to_value(cx));
}

JSValueRef js_game_join_set_spawn_spot_to_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	if (game_obj_rule_idx==-1) return(script_null_to_value(cx));

	obj=server.obj_list.objs[game_obj_rule_idx];
	
	if (obj->team_idx==net_team_red) {
		strcpy(obj->spawn_spot_name,"Red");
	}
	else {
		strcpy(obj->spawn_spot_name,"Blue");
	}

	return(script_null_to_value(cx));
}

JSValueRef js_game_join_clear_spawn_spot_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	if (game_obj_rule_idx==-1) return(script_null_to_value(cx));

	obj=server.obj_list.objs[game_obj_rule_idx];
	obj->spawn_spot_name[0]=0x0;

	return(script_null_to_value(cx));
}
