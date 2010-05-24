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

JSValueRef js_game_score_get_objectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_game_score_get_kill(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_game_score_get_death(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_game_score_get_suicide(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_game_score_get_goal(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_game_score_set_score_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		game_score_props[]={
							{"objectId",			js_game_score_get_objectId,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"kill",				js_game_score_get_kill,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"death",				js_game_score_get_death,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"suicide",				js_game_score_get_suicide,			NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"goal",				js_game_score_get_goal,				NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};
							
JSStaticFunction	game_score_functions[]={
							{"setScore",			js_game_score_set_score_func,		kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			game_score_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_game_score_object(void)
{
	game_score_class=script_create_class("game_score_class",game_score_props,game_score_functions);
}

void script_free_game_score_object(void)
{
	script_free_class(game_score_class);
}

JSObjectRef script_add_game_score_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,game_score_class,"score"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_game_score_get_objectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(script_int_to_value(cx,-1));

	obj=object_find_uid(game_obj_rule_uid);
	return(script_int_to_value(cx,obj->index));
}

JSValueRef js_game_score_get_kill(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(script_int_to_value(cx,0));

	obj=object_find_uid(game_obj_rule_uid);
	return(script_int_to_value(cx,obj->score.kill));
}

JSValueRef js_game_score_get_death(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(script_int_to_value(cx,0));

	obj=object_find_uid(game_obj_rule_uid);
	return(script_int_to_value(cx,obj->score.death));
}

JSValueRef js_game_score_get_suicide(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(script_int_to_value(cx,0));

	obj=object_find_uid(game_obj_rule_uid);
	return(script_int_to_value(cx,obj->score.suicide));
}

JSValueRef js_game_score_get_goal(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	obj_type		*obj;

	if (game_obj_rule_uid==-1) return(script_int_to_value(cx,0));

	obj=object_find_uid(game_obj_rule_uid);
	return(script_int_to_value(cx,obj->score.goal));
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_game_score_set_score_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	if (game_obj_rule_uid==-1) return(script_null_to_value(cx));

		// update score

	obj=object_find_uid(game_obj_rule_uid);
	obj->score.score=script_value_to_int(cx,argv[0]);

		// have we hit a network score limit?

	score_limit_check_scores();

	return(script_null_to_value(cx));
}

