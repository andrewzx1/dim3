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

JSBool js_multiplayer_score_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_multiplayer_score_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_multiplayer_score_get_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_multiplayer_score_get_team_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_function	multiplayer_score_functions[]={
							{"getObject",			js_multiplayer_score_get_object_func,		1},
							{"getTeam",				js_multiplayer_score_get_team_func,			1},
							{0}};

JSClass				*multiplayer_score_class;

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

void script_add_multiplayer_score_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,multiplayer_score_class,"score",NULL,multiplayer_score_functions);
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_multiplayer_score_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,NULL));
}

JSBool js_multiplayer_score_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,NULL));
}

/* =======================================================

      Score Functions
      
======================================================= */

JSBool js_multiplayer_score_get_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=script_find_obj_from_uid_arg(argv[0]);
	if (obj==NULL) return(JS_FALSE);

	*rval=INT_TO_JSVAL(obj->score.score);
	return(JS_TRUE);
}

JSBool js_multiplayer_score_get_team_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
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

	*rval=INT_TO_JSVAL(score);
	return(JS_TRUE);
}

