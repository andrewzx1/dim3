/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.fog object

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

JSBool js_map_fog_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_map_fog_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
bool js_map_fog_get_on(JSValueRef *vp);
bool js_map_fog_get_count(JSValueRef *vp);
bool js_map_fog_get_outerRadius(JSValueRef *vp);
bool js_map_fog_get_innerRadius(JSValueRef *vp);
bool js_map_fog_get_high(JSValueRef *vp);
bool js_map_fog_get_drop(JSValueRef *vp);
bool js_map_fog_get_textureIndex(JSValueRef *vp);
bool js_map_fog_get_speed(JSValueRef *vp);
bool js_map_fog_get_textureXFact(JSValueRef *vp);
bool js_map_fog_get_textureYFact(JSValueRef *vp);
bool js_map_fog_get_alpha(JSValueRef *vp);
bool js_map_fog_get_useSolidColor(JSValueRef *vp);
bool js_map_fog_set_on(JSValueRef *vp);
bool js_map_fog_set_count(JSValueRef *vp);
bool js_map_fog_set_outerRadius(JSValueRef *vp);
bool js_map_fog_set_innerRadius(JSValueRef *vp);
bool js_map_fog_set_high(JSValueRef *vp);
bool js_map_fog_set_drop(JSValueRef *vp);
bool js_map_fog_set_textureIndex(JSValueRef *vp);
bool js_map_fog_set_speed(JSValueRef *vp);
bool js_map_fog_set_textureXFact(JSValueRef *vp);
bool js_map_fog_set_textureYFact(JSValueRef *vp);
bool js_map_fog_set_alpha(JSValueRef *vp);
bool js_map_fog_set_useSolidColor(JSValueRef *vp);

script_js_property	map_fog_props[]={
							{"on",					js_map_fog_get_on,					js_map_fog_set_on},
							{"layerCount",			js_map_fog_get_count,				js_map_fog_set_count},
							{"outerRadius",			js_map_fog_get_outerRadius,			js_map_fog_set_outerRadius},
							{"innerRadius",			js_map_fog_get_innerRadius,			js_map_fog_set_innerRadius},
							{"height",				js_map_fog_get_high,				js_map_fog_set_high},
							{"drop",				js_map_fog_get_drop,				js_map_fog_set_drop},
							{"textureIndex",		js_map_fog_get_textureIndex,		js_map_fog_set_textureIndex},
							{"speed",				js_map_fog_get_speed,				js_map_fog_set_speed},
							{"textureXFact",		js_map_fog_get_textureXFact,		js_map_fog_set_textureXFact},
							{"textureYFact",		js_map_fog_get_textureYFact,		js_map_fog_set_textureYFact},
							{"alpha",				js_map_fog_get_alpha,				js_map_fog_set_alpha},
							{"useSolidColor",		js_map_fog_get_useSolidColor,		js_map_fog_set_useSolidColor},
							{0}};

JSClassRef			map_fog_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_fog_object(void)
{
	map_fog_class=script_create_class("map_fog_class",js_map_fog_get_property,js_map_fog_set_property);
}

void script_free_map_fog_object(void)
{
	script_free_class(map_fog_class);
}

JSObject* script_add_map_fog_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,map_fog_class,"fog",map_fog_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_map_fog_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,map_fog_props));
}

JSBool js_map_fog_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,map_fog_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_map_fog_get_on(JSValueRef *vp)
{
	*vp=script_bool_to_value(map.fog.on);
	return(TRUE);
}

bool js_map_fog_get_count(JSValueRef *vp)
{
	*vp=script_int_to_value(map.fog.count);
	return(TRUE);
}

bool js_map_fog_get_outerRadius(JSValueRef *vp)
{
	*vp=script_int_to_value(map.fog.outer_radius);
	return(TRUE);
}

bool js_map_fog_get_innerRadius(JSValueRef *vp)
{
	*vp=script_int_to_value(map.fog.inner_radius);
	return(TRUE);
}

bool js_map_fog_get_high(JSValueRef *vp)
{
	*vp=script_int_to_value(map.fog.high);
	return(TRUE);
}

bool js_map_fog_get_drop(JSValueRef *vp)
{
	*vp=script_int_to_value(map.fog.drop);
	return(TRUE);
}

bool js_map_fog_get_textureIndex(JSValueRef *vp)
{
	*vp=script_int_to_value(map.fog.texture_idx);
	return(TRUE);
}

bool js_map_fog_get_speed(JSValueRef *vp)
{
	*vp=script_float_to_value(map.fog.speed);
	return(TRUE);
}

bool js_map_fog_get_textureXFact(JSValueRef *vp)
{
	*vp=script_float_to_value(map.fog.txt_x_fact);
	return(TRUE);
}

bool js_map_fog_get_textureYFact(JSValueRef *vp)
{
	*vp=script_float_to_value(map.fog.txt_y_fact);
	return(TRUE);
}

bool js_map_fog_get_alpha(JSValueRef *vp)
{
	*vp=script_float_to_value(map.fog.alpha);
	return(TRUE);
}

bool js_map_fog_get_useSolidColor(JSValueRef *vp)
{
	*vp=script_bool_to_value(map.fog.use_solid_color);
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_map_fog_set_on(JSValueRef *vp)
{
	map.fog.on=script_value_to_bool(*vp);
	return(TRUE);
}

bool js_map_fog_set_count(JSValueRef *vp)
{
	map.fog.count=script_value_to_int(*vp);
	return(TRUE);
}

bool js_map_fog_set_outerRadius(JSValueRef *vp)
{
	map.fog.outer_radius=script_value_to_int(*vp);
	return(TRUE);
}

bool js_map_fog_set_innerRadius(JSValueRef *vp)
{
	map.fog.inner_radius=script_value_to_int(*vp);
	return(TRUE);
}

bool js_map_fog_set_high(JSValueRef *vp)
{
	map.fog.high=script_value_to_int(*vp);
	return(TRUE);
}

bool js_map_fog_set_drop(JSValueRef *vp)
{
	map.fog.drop=script_value_to_int(*vp);
	return(TRUE);
}

bool js_map_fog_set_textureIndex(JSValueRef *vp)
{
	map.fog.texture_idx=script_value_to_int(*vp);
	return(TRUE);
}

bool js_map_fog_set_speed(JSValueRef *vp)
{
	map.fog.speed=script_value_to_float(*vp);
	return(TRUE);
}

bool js_map_fog_set_textureXFact(JSValueRef *vp)
{
	map.fog.txt_x_fact=script_value_to_float(*vp);
	return(TRUE);
}

bool js_map_fog_set_textureYFact(JSValueRef *vp)
{
	map.fog.txt_y_fact=script_value_to_float(*vp);
	return(TRUE);
}

bool js_map_fog_set_alpha(JSValueRef *vp)
{
	map.fog.alpha=script_value_to_float(*vp);
	return(TRUE);
}

bool js_map_fog_set_useSolidColor(JSValueRef *vp)
{
	map.fog.use_solid_color=script_value_to_bool(*vp);
	return(TRUE);
}


