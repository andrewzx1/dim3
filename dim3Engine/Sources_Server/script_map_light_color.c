/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.lightColor object

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
#include "lights.h"

extern map_type			map;
extern js_type			js;

JSValueRef js_map_light_color_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_map_light_color_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_map_light_color_get_red(void);
JSValueRef js_map_light_color_get_green(void);
JSValueRef js_map_light_color_get_blue(void);
void js_map_light_color_set_red(JSValueRef vp,JSValueRef *exception);
void js_map_light_color_set_green(JSValueRef vp,JSValueRef *exception);
void js_map_light_color_set_blue(JSValueRef vp,JSValueRef *exception);

script_js_property	map_light_color_props[]={
							{"red",					js_map_light_color_get_red,				js_map_light_color_set_red},
							{"green",				js_map_light_color_get_green,			js_map_light_color_set_green},
							{"blue",				js_map_light_color_get_blue,			js_map_light_color_set_blue},
							{0}};

JSClassRef			map_light_color_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_light_color_object(void)
{
	map_light_color_class=script_create_class("map_light_color_class",js_map_light_color_get_property,js_map_light_color_set_property);
}

void script_free_map_light_color_object(void)
{
	script_free_class(map_light_color_class);
}

JSObjectRef script_add_map_light_color_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,map_light_color_class,"lightColor",map_light_color_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_map_light_color_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,map_light_color_props));
}

bool js_map_light_color_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,map_light_color_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_map_light_color_get_red(void)
{
	*vp=script_float_to_value(map.ambient.light_color.r);
	return(TRUE);
}

JSValueRef js_map_light_color_get_green(void)
{
	*vp=script_float_to_value(map.ambient.light_color.g);
	return(TRUE);
}

JSValueRef js_map_light_color_get_blue(void)
{
	*vp=script_float_to_value(map.ambient.light_color.b);
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

void js_map_light_color_set_red(JSValueRef vp,JSValueRef *exception)
{
	map.ambient.light_color.r=script_value_to_float(*vp);
}

void js_map_light_color_set_green(JSValueRef vp,JSValueRef *exception)
{
	map.ambient.light_color.g=script_value_to_float(*vp);
}

void js_map_light_color_set_blue(JSValueRef vp,JSValueRef *exception)
{
	map.ambient.light_color.b=script_value_to_float(*vp);
}

