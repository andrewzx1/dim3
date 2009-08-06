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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"
#include "objects.h"

extern js_type				js;
extern network_setup_type	net_setup;

extern int					game_obj_rule_uid;

JSBool js_game_join_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_game_join_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
bool js_game_join_get_name(JSValueRef *vp);
bool js_game_join_get_team(JSValueRef *vp);
JSBool js_game_join_set_team_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_game_join_set_team_even_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_game_join_clear_team_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_game_join_count_team_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_game_join_set_spawn_spot_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_game_join_set_spawn_spot_to_team_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_game_join_clear_spawn_spot_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);

script_js_property	game_join_props[]={
							{"name",				js_game_join_get_name,						NULL},
							{"team",				js_game_join_get_team,						NULL},
							{0}};
							
script_js_function	game_join_functions[]={
							{"setTeam",				js_game_join_set_team_func,					1},
							{"setTeamEven",			js_game_join_set_team_even_func,			0},
							{"clearTeam",			js_game_join_clear_team_func,				0},
							{"countTeam",			js_game_join_count_team_func,				1},
							{"setSpawnSpot",		js_game_join_set_spawn_spot_func,			1},
							{"setSpawnSpotToTeam",	js_game_join_set_spawn_spot_to_team_func,	0},
							{"clearSpawnSpot",		js_game_join_clear_spawn_spot_func,			0},
							{0}};

JSClassRef			game_join_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_game_join_object(void)
{
	game_join_class=script_create_class("game_join_class",js_game_join_get_property,js_game_join_set_property);
}

void script_free_game_join_object(void)
{
	script_free_class(game_join_class);
}

JSObject* script_add_game_join_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,game_join_class,"join",game_join_props,game_join_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_game_join_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,game_join_props));
}

JSBool js_game_join_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,game_join_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_game_join_get_name(JSValueRef *vp)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	*vp=script_string_to_value(obj->name);

	return(TRUE);
}

bool js_game_join_get_team(JSValueRef *vp)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(TRUE);

	obj=object_find_uid(game_obj_rule_uid);
    *vp=script_int_to_value(obj->team_idx+sd_team_none);

	return(TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSBool js_game_join_set_team_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	obj->team_idx=script_value_to_int(argv[0])-sd_team_none;

	return(TRUE);
}

JSBool js_game_join_set_team_even_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	object_set_even_team(obj);

	return(TRUE);
}

JSBool js_game_join_clear_team_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	obj->team_idx=net_team_none;

	return(TRUE);
}

JSBool js_game_join_count_team_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int			team_idx;

	team_idx=script_value_to_int(argv[0])-sd_team_none;
	*rval=script_int_to_value(object_count_team(team_idx,game_obj_rule_uid));

	return(TRUE);
}

JSBool js_game_join_set_spawn_spot_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	script_value_to_string(argv[0],obj->spawn_spot_name,name_str_len);

	return(TRUE);
}

JSBool js_game_join_set_spawn_spot_to_team_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	
	if (obj->team_idx==net_team_red) {
		strcpy(obj->spawn_spot_name,"Red");
	}
	else {
		strcpy(obj->spawn_spot_name,"Blue");
	}

	return(TRUE);
}

JSBool js_game_join_clear_spawn_spot_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	obj->spawn_spot_name[0]=0x0;

	return(TRUE);
}
