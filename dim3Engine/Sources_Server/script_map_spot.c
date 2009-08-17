/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.spot object

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

extern map_type			map;
extern js_type			js;

JSValueRef js_map_spot_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_map_spot_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_map_spot_get_count(JSContextRef cx);
JSValueRef js_map_spot_find_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_spot_get_name_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_spot_get_type_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_spot_get_script_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_spot_get_parameter_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_spot_get_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_spot_get_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_spot_attach_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

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

JSObjectRef script_add_map_spot_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,map_spot_class,"spot",map_spot_props,map_spot_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_map_spot_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,map_spot_props));
}

bool js_map_spot_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,map_spot_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_map_spot_get_count(JSContextRef cx)
{
	return(script_int_to_value(cx,map.nspot));
}

/* =======================================================

      Spot Finds
      
======================================================= */

JSValueRef js_map_spot_find_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char		name[name_str_len],type[name_str_len];
	
	if ((script_is_value_null(cx,argv[0])) && (script_is_value_null(cx,argv[1]))) {
		return(script_int_to_value(cx,-1));
	}
	
	if ((!script_is_value_null(cx,argv[0])) && (!script_is_value_null(cx,argv[1]))) {
		script_value_to_string(cx,argv[0],name,name_str_len);
		script_value_to_string(cx,argv[1],type,name_str_len);
		return(script_int_to_value(cx,map_find_spot(&map,name,type)));
	}
	
	if (!script_is_value_null(cx,argv[0])) {
		script_value_to_string(cx,argv[0],name,name_str_len);
		return(script_int_to_value(cx,map_find_spot(&map,name,NULL)));
	}
		
	script_value_to_string(cx,argv[1],type,name_str_len);
	return(script_int_to_value(cx,map_find_spot(&map,NULL,type)));
}

/* =======================================================

      Spot Info
      
======================================================= */

JSValueRef js_map_spot_get_name_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	spot_type	*spot;
	
	spot=script_find_spot_from_idx_arg(cx,argv[0],exception);
	if (spot==NULL) return(script_null_to_value(cx));
	
	return(script_string_to_value(cx,spot->name));
}

JSValueRef js_map_spot_get_type_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	spot_type	*spot;
	
	spot=script_find_spot_from_idx_arg(cx,argv[0],exception);
	if (spot==NULL) return(script_null_to_value(cx));
	
	return(script_string_to_value(cx,spot->type));
}

JSValueRef js_map_spot_get_script_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	spot_type	*spot;
	
	spot=script_find_spot_from_idx_arg(cx,argv[0],exception);
	if (spot==NULL) return(script_null_to_value(cx));
	
	return(script_string_to_value(cx,spot->script));
}

JSValueRef js_map_spot_get_parameter_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	spot_type	*spot;
	
	spot=script_find_spot_from_idx_arg(cx,argv[0],exception);
	if (spot==NULL) return(script_null_to_value(cx));
	
	return(script_string_to_value(cx,spot->params));
}

/* =======================================================

      Spot Position and Angle
      
======================================================= */

JSValueRef js_map_spot_get_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	spot_type   *spot;

		// find spot
	
	spot=script_find_spot_from_idx_arg(cx,argv[0],exception);
	if (spot==NULL) return(script_null_to_value(cx));
	
		// get position
		
	return(script_point_to_value(cx,spot->pnt.x,spot->pnt.y,spot->pnt.z));
}

JSValueRef js_map_spot_get_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	spot_type   *spot;

		// find spot
	
	spot=script_find_spot_from_idx_arg(cx,argv[0],exception);
	if (spot==NULL) return(script_null_to_value(cx));
	
		// get position
		
	return(script_angle_to_value(cx,0,spot->ang.y,0));
}

/* =======================================================

      Spot Object Attachments
      
======================================================= */

JSValueRef js_map_spot_attach_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char		name[name_str_len],type[name_str_len],
				script[file_str_len],params[param_str_len];
	spot_type   *spot;

		// find spot
	
	spot=script_find_spot_from_idx_arg(cx,argv[0],exception);
	if (spot==NULL) return(script_null_to_value(cx));

		// attach
		
	script_value_to_string(cx,argv[1],name,name_str_len);
	script_value_to_string(cx,argv[2],type,name_str_len);
	script_value_to_string(cx,argv[3],script,file_str_len);
	script_value_to_string(cx,argv[4],params,param_str_len);
	
	map_spot_attach_object(spot,name,type,script,params);
		
	return(script_null_to_value(cx));
}

