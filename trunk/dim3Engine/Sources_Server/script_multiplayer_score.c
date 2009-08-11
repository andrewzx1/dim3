/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: multiplayer.score object

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

extern server_type		server;
extern js_type			js;
extern setup_type		setup;

JSValueRef js_multiplayer_score_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_multiplayer_score_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_multiplayer_score_get_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_multiplayer_score_get_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_function	multiplayer_score_functions[]={
							{"getObject",			js_multiplayer_score_get_object_func,		1},
							{"getTeam",				js_multiplayer_score_get_team_func,			1},
							{0}};

JSClassRef			multiplayer_score_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_multiplayer_score_object(void)
{
	multiplayer_score_class=script_create_class("multiplayer_score_class",js_multiplayer_score_get_property,js_multiplayer_score_set_property);
}

void script_free_multiplayer_score_object(void)
{
	script_free_class(multiplayer_score_class);
}

JSObjectRef script_add_multiplayer_score_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,multiplayer_score_class,"score",NULL,multiplayer_score_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_multiplayer_score_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,NULL));
}

bool js_multiplayer_score_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,NULL));
}

/* =======================================================

      Score Functions
      
======================================================= */

JSValueRef js_multiplayer_score_get_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(FALSE);

	*rval=script_int_to_value(obj->score.score);
	return(TRUE);
}

JSValueRef js_multiplayer_score_get_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,score,team_idx;
	obj_type		*obj;

	team_idx=script_value_to_int(argv[0])-sd_team_none;

		// add up team scores

	score=0;

	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {

		if ((obj->player) || (obj->remote.on) || (obj->bot)) {
			if (obj->team_idx==team_idx) score+=obj->score.score;
		}

		obj++;
	}

		// return team score

	*rval=script_int_to_value(score);
	return(TRUE);
}

