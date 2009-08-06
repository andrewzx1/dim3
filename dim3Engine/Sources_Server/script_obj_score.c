/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.score object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

extern map_type			map;
extern js_type			js;

JSBool js_obj_score_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_obj_score_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
bool js_obj_score_get_kill(JSValueRef *vp);
bool js_obj_score_get_death(JSValueRef *vp);
bool js_obj_score_get_suicide(JSValueRef *vp);
bool js_obj_score_get_goal(JSValueRef *vp);
bool js_obj_score_get_score(JSValueRef *vp);
JSBool js_obj_score_add_goal_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);

script_js_property	obj_score_props[]={
							{"kill",				js_obj_score_get_kill,				NULL},
							{"death",				js_obj_score_get_death,				NULL},
							{"suicide",				js_obj_score_get_suicide,			NULL},
							{"goal",				js_obj_score_get_goal,				NULL},
							{"score",				js_obj_score_get_score,				NULL},
							{0}};

script_js_function	obj_score_functions[]={
							{"addGoal",				js_obj_score_add_goal_func,			0},
							{0}};

JSClassRef			obj_score_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_score_object(void)
{
	obj_score_class=script_create_class("obj_score_class",js_obj_score_get_property,js_obj_score_set_property);
}

void script_free_obj_score_object(void)
{
	script_free_class(obj_score_class);
}

JSObject* script_add_obj_score_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,obj_score_class,"score",obj_score_props,obj_score_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_obj_score_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,obj_score_props));
}

JSBool js_obj_score_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,obj_score_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_obj_score_get_kill(JSValueRef *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->score.kill);
	
	return(TRUE);
}

bool js_obj_score_get_death(JSValueRef *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->score.death);
	
	return(TRUE);
}

bool js_obj_score_get_suicide(JSValueRef *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->score.suicide);
	
	return(TRUE);
}

bool js_obj_score_get_goal(JSValueRef *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->score.goal);
	
	return(TRUE);
}

bool js_obj_score_get_score(JSValueRef *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->score.score);
	
	return(TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSBool js_obj_score_add_goal_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_score_goal(obj);

	return(TRUE);
}




