/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.movement object

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

extern map_type			map;
extern js_type			js;

extern void map_movements_script_start(int movement_idx,bool reverse);
extern void map_movements_script_start_or_thaw(int movement_idx);
extern void map_movements_script_freeze(int movement_idx);
extern void map_movements_script_thaw(int movement_idx);
extern bool map_movements_script_is_looping(int movement_idx);

JSBool js_map_movement_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_movement_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_movement_start_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_movement_start_reverse_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_movement_start_or_thaw_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_movement_freeze_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_movement_thaw_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_movement_is_looping_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_function	map_movement_functions[]={
							{"start",				js_map_movement_start_func,			1},
							{"startReverse",		js_map_movement_start_reverse_func,	1},
							{"startOrThaw",			js_map_movement_start_or_thaw_func,	1},
							{"freeze",				js_map_movement_freeze_func,		1},
							{"thaw",				js_map_movement_thaw_func,			1},
							{"isLooping",			js_map_movement_is_looping_func,	1},
							{0}};

JSClass				*map_movement_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_movement_object(void)
{
	map_movement_class=script_create_class("map_movement_class",js_map_movement_get_property,js_map_movement_set_property);
}

void script_free_map_movement_object(void)
{
	script_free_class(map_movement_class);
}

void script_add_map_movement_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"movement",NULL,map_movement_functions);
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_map_movement_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,NULL));
}

JSBool js_map_movement_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,NULL));
}

/* =======================================================

      Movement Function
      
======================================================= */

JSBool js_map_movement_start_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				movement_idx;
	
	movement_idx=script_find_map_movement_from_name(argv[0]);
	if (movement_idx==-1) return(JS_FALSE);
	
	map_movements_script_start(movement_idx,FALSE);
	return(JS_TRUE);
}

JSBool js_map_movement_start_reverse_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				movement_idx;
	
	movement_idx=script_find_map_movement_from_name(argv[0]);
	if (movement_idx==-1) return(JS_FALSE);
	
	map_movements_script_start(movement_idx,TRUE);
	return(JS_TRUE);
}

JSBool js_map_movement_start_or_thaw_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				movement_idx;
	
	movement_idx=script_find_map_movement_from_name(argv[0]);
	if (movement_idx==-1) return(JS_FALSE);
	
	map_movements_script_start_or_thaw(movement_idx);
	return(JS_TRUE);
}

JSBool js_map_movement_freeze_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				movement_idx;
	
	movement_idx=script_find_map_movement_from_name(argv[0]);
	if (movement_idx==-1) return(JS_FALSE);
	
	map_movements_script_freeze(movement_idx);
	return(JS_TRUE);
}

JSBool js_map_movement_thaw_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				movement_idx;
	
	movement_idx=script_find_map_movement_from_name(argv[0]);
	if (movement_idx==-1) return(JS_FALSE);
	
	map_movements_script_thaw(movement_idx);
	return(JS_TRUE);
}

JSBool js_map_movement_is_looping_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				movement_idx;
	
	movement_idx=script_find_map_movement_from_name(argv[0]);
	if (movement_idx==-1) return(JS_FALSE);
	
	*rval=script_bool_to_value(map_movements_script_is_looping(movement_idx));
	return(JS_TRUE);
}

