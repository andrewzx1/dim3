/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.light object

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

JSValueRef js_model_light_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_model_light_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_model_light_get_index(JSContextRef cx);
JSValueRef js_model_light_get_on(JSContextRef cx);
JSValueRef js_model_light_get_type(JSContextRef cx);
JSValueRef js_model_light_get_filter(JSContextRef cx);
JSValueRef js_model_light_get_direction(JSContextRef cx);
JSValueRef js_model_light_get_intensity(JSContextRef cx);
JSValueRef js_model_light_get_exponent(JSContextRef cx);
void js_model_light_set_index(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_model_light_set_on(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_model_light_set_type(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_model_light_set_filter(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_model_light_set_direction(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_model_light_set_intensity(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_model_light_set_exponent(JSContextRef cx,JSValueRef vp,JSValueRef *exception);

extern js_type			js;

script_js_property	model_light_props[]={
							{"index",				js_model_light_get_index,				js_model_light_set_index},
							{"on",					js_model_light_get_on,					js_model_light_set_on},
							{"type",				js_model_light_get_type,				js_model_light_set_type},
							{"filter",				js_model_light_get_filter,				js_model_light_set_filter},
							{"direction",			js_model_light_get_direction,			js_model_light_set_direction},
							{"intensity",			js_model_light_get_intensity,			js_model_light_set_intensity},
							{"exponent",			js_model_light_get_exponent,			js_model_light_set_exponent},
							{0}};
							
JSClassRef			model_light_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_light_object(void)
{
	model_light_class=script_create_class("model_light_class",js_model_light_get_property,js_model_light_set_property);
}

void script_free_model_light_object(void)
{
	script_free_class(model_light_class);
}

JSObjectRef script_add_model_light_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,model_light_class,"light",model_light_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_model_light_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,model_light_props));
}

bool js_model_light_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,model_light_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_model_light_get_index(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	return(script_int_to_value(cx,draw->script_light_idx));
}

JSValueRef js_model_light_get_on(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];

	return(script_bool_to_value(cx,light->on));
}

JSValueRef js_model_light_get_type(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	return(script_int_to_value(cx,light->type-sd_light_type_normal));
}

JSValueRef js_model_light_get_filter(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	return(script_int_to_value(cx,light->filter-sd_light_filter_none));
}

JSValueRef js_model_light_get_direction(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];

	return(script_int_to_value(cx,light->direction+sd_light_direction_all));
}

JSValueRef js_model_light_get_intensity(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	return(script_int_to_value(cx,light->intensity));
}

JSValueRef js_model_light_get_exponent(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	return(script_float_to_value(cx,light->exponent));
}

/* =======================================================

      Setters
      
======================================================= */

void js_model_light_set_index(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	draw->script_light_idx=script_value_to_int(cx,vp);
	if ((draw->script_light_idx<0) || (draw->script_light_idx>=max_model_light)) draw->script_light_idx=0;
}

void js_model_light_set_on(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	light->on=script_value_to_bool(cx,vp);
}

void js_model_light_set_type(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	light->type=script_value_to_int(cx,vp)-sd_light_type_normal;
}

void js_model_light_set_filter(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	light->filter=script_value_to_int(cx,vp)-sd_light_filter_none;
}

void js_model_light_set_direction(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	light->direction=script_value_to_int(cx,vp)-sd_light_direction_all;
}

void js_model_light_set_intensity(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];

	light->intensity=script_value_to_int(cx,vp);
}

void js_model_light_set_exponent(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	light->exponent=script_value_to_float(cx,vp);
}

