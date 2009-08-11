/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: game.score object

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
#include "interfaces.h"

extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern int				game_obj_rule_uid;

JSValueRef js_game_score_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_game_score_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_game_score_get_objectId(void);
JSValueRef js_game_score_get_kill(void);
JSValueRef js_game_score_get_death(void);
JSValueRef js_game_score_get_suicide(void);
JSValueRef js_game_score_get_goal(void);
JSValueRef js_game_score_set_score_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_property	game_score_props[]={
							{"objectId",			js_game_score_get_objectId,			NULL},
							{"kill",				js_game_score_get_kill,				NULL},
							{"death",				js_game_score_get_death,			NULL},
							{"suicide",				js_game_score_get_suicide,			NULL},
							{"goal",				js_game_score_get_goal,				NULL},
							{0}};
							
script_js_function	game_score_functions[]={
							{"setScore",			js_game_score_set_score_func,		1},
							{0}};

JSClassRef			game_score_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_game_score_object(void)
{
	game_score_class=script_create_class("game_score_class",js_game_score_get_property,js_game_score_set_property);
}

void script_free_game_score_object(void)
{
	script_free_class(game_score_class);
}

JSObjectRef script_add_game_score_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,game_score_class,"score",game_score_props,game_score_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_game_score_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,game_score_props));
}

bool js_game_score_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,game_score_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_game_score_get_objectId(void)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	*vp=script_int_to_value(obj->uid);
	
	return(TRUE);
}

JSValueRef js_game_score_get_kill(void)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	*vp=script_int_to_value(obj->score.kill);
	
	return(TRUE);
}

JSValueRef js_game_score_get_death(void)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	*vp=script_int_to_value(obj->score.death);
	
	return(TRUE);
}

JSValueRef js_game_score_get_suicide(void)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	*vp=script_int_to_value(obj->score.suicide);
	
	return(TRUE);
}

JSValueRef js_game_score_get_goal(void)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(TRUE);

	obj=object_find_uid(game_obj_rule_uid);
	*vp=script_int_to_value(obj->score.goal);
	
	return(TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_game_score_set_score_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(TRUE);

		// update score

	obj=object_find_uid(game_obj_rule_uid);
	obj->score.score=script_value_to_int(argv[0]);

		// have we hit a network score limit?

	if (net_setup.client.joined) score_limit_trigger_set_check_scores();

	return(TRUE);
}

