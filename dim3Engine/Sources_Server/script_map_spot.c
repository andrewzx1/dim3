/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.spot object

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

extern map_type			map;
extern js_type			js;

JSBool js_map_spot_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_spot_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_spot_get_count(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_spot_find_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_spot_get_name_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_spot_get_type_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_spot_get_script_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_spot_get_parameter_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_spot_get_position_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_spot_get_angle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_map_spot_attach_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_property	map_spot_props[]={
							{"count",				js_map_spot_get_count,				NULL},
							{0}};
							
script_js_function	map_spot_functions[]={
							{"find",				js_map_spot_find_func,				2},
							{"getName",				js_map_spot_get_name_func,			1},
							{"getType",				js_map_spot_get_type_func,			1},
							{"getScript",			js_map_spot_get_script_func,		1},
							{"getParameter",		js_map_spot_get_parameter_func,		1},
							{"getPosition",			js_map_spot_get_position_func,		1},
							{"getAngle",			js_map_spot_get_angle_func,			1},
							{"attachObject",		js_map_spot_attach_object_func,		5},
							{0}};

JSClassRef			map_spot_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_spot_object(void)
{
	map_spot_class=script_create_class("map_spot_class",js_map_spot_get_property,js_map_spot_set_property);
}

void script_free_map_spot_object(void)
{
	script_free_class(map_spot_class);
}

JSObject* script_add_map_spot_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,map_spot_class,"spot",map_spot_props,map_spot_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_map_spot_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,map_spot_props));
}

JSBool js_map_spot_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,map_spot_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_map_spot_get_count(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	*vp=script_int_to_value(map.nspot);
	return(JS_TRUE);
}

/* =======================================================

      Spot Finds
      
======================================================= */

JSBool js_map_spot_find_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char		name[name_str_len],type[name_str_len];
	
	if ((script_is_value_null(argv[0])) && (script_is_value_null(argv[1]))) {
		*rval=script_int_to_value(-1);
		return(JS_TRUE);
	}
	
	if ((!script_is_value_null(argv[0])) && (!script_is_value_null(argv[1]))) {
		script_value_to_string(argv[0],name,name_str_len);
		script_value_to_string(argv[1],type,name_str_len);
		*rval=script_int_to_value(map_find_spot(&map,name,type));
		return(JS_TRUE);
	}
	
	if (!script_is_value_null(argv[0])) {
		script_value_to_string(argv[0],name,name_str_len);
		*rval=script_int_to_value(map_find_spot(&map,name,NULL));
		return(JS_TRUE);
	}
		
	script_value_to_string(argv[1],type,name_str_len);
	*rval=script_int_to_value(map_find_spot(&map,NULL,type));
	
	return(JS_TRUE);
}

/* =======================================================

      Spot Info
      
======================================================= */

JSBool js_map_spot_get_name_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	spot_type	*spot;
	
	spot=script_find_spot_from_idx_arg(argv[0]);
	if (spot==NULL) return(JS_FALSE);
	
	*rval=script_string_to_value(spot->name);
	
	return(JS_TRUE);
}

JSBool js_map_spot_get_type_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	spot_type	*spot;
	
	spot=script_find_spot_from_idx_arg(argv[0]);
	if (spot==NULL) return(JS_FALSE);
	
	*rval=script_string_to_value(spot->type);
	
	return(JS_TRUE);
}

JSBool js_map_spot_get_script_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	spot_type	*spot;
	
	spot=script_find_spot_from_idx_arg(argv[0]);
	if (spot==NULL) return(JS_FALSE);
	
	*rval=script_string_to_value(spot->script);
	
	return(JS_TRUE);
}

JSBool js_map_spot_get_parameter_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	spot_type	*spot;
	
	spot=script_find_spot_from_idx_arg(argv[0]);
	if (spot==NULL) return(JS_FALSE);
	
	*rval=script_string_to_value(spot->params);
	
	return(JS_TRUE);
}

/* =======================================================

      Spot Position and Angle
      
======================================================= */

JSBool js_map_spot_get_position_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	spot_type   *spot;

		// find spot
	
	spot=script_find_spot_from_idx_arg(argv[0]);
	if (spot==NULL) return(JS_FALSE);
	
		// get position
		
	*rval=script_point_to_value(spot->pnt.x,spot->pnt.y,spot->pnt.z);
	return(JS_TRUE);
}

JSBool js_map_spot_get_angle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	spot_type   *spot;

		// find spot
	
	spot=script_find_spot_from_idx_arg(argv[0]);
	if (spot==NULL) return(JS_FALSE);
	
		// get position
		
	*rval=script_angle_to_value(0,spot->ang.y,0);
	return(JS_TRUE);
}

/* =======================================================

      Spot Object Attachments
      
======================================================= */

JSBool js_map_spot_attach_object_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char		name[name_str_len],type[name_str_len],
				script[file_str_len],params[param_str_len];
	spot_type   *spot;

		// find spot
	
	spot=script_find_spot_from_idx_arg(argv[0]);
	if (spot==NULL) return(JS_FALSE);

		// attach
		
	script_value_to_string(argv[1],name,name_str_len);
	script_value_to_string(argv[2],type,name_str_len);
	script_value_to_string(argv[3],script,file_str_len);
	script_value_to_string(argv[4],params,param_str_len);
	
	map_spot_attach_object(spot,name,type,script,params);
		
	return(JS_TRUE);
}

