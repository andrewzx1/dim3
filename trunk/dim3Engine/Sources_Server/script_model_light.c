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

JSBool js_model_light_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_light_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
bool js_model_light_get_index(jsval *vp);
bool js_model_light_get_on(jsval *vp);
bool js_model_light_get_type(jsval *vp);
bool js_model_light_get_filter(jsval *vp);
bool js_model_light_get_direction(jsval *vp);
bool js_model_light_get_intensity(jsval *vp);
bool js_model_light_get_exponent(jsval *vp);
bool js_model_light_set_index(jsval *vp);
bool js_model_light_set_on(jsval *vp);
bool js_model_light_set_type(jsval *vp);
bool js_model_light_set_filter(jsval *vp);
bool js_model_light_set_direction(jsval *vp);
bool js_model_light_set_intensity(jsval *vp);
bool js_model_light_set_exponent(jsval *vp);

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

JSObject* script_add_model_light_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,model_light_class,"light",model_light_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_model_light_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,model_light_props));
}

JSBool js_model_light_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,model_light_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_model_light_get_index(jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	*vp=script_int_to_value(draw->script_light_idx);

	return(JS_TRUE);
}

bool js_model_light_get_on(jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];

	*vp=script_bool_to_value(light->on);

	return(JS_TRUE);
}

bool js_model_light_get_type(jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	*vp=script_int_to_value(light->type-sd_light_type_normal);

	return(JS_TRUE);
}

bool js_model_light_get_filter(jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	*vp=script_int_to_value(light->filter-sd_light_filter_none);

	return(JS_TRUE);
}

bool js_model_light_get_direction(jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];

	*vp=script_int_to_value(light->direction+sd_light_direction_all);

	return(JS_TRUE);
}

bool js_model_light_get_intensity(jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	*vp=script_int_to_value(light->intensity);

	return(JS_TRUE);
}

bool js_model_light_get_exponent(jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	*vp=script_float_to_value(light->exponent);

	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_model_light_set_index(jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	draw->script_light_idx=script_value_to_int(*vp);
	if ((draw->script_light_idx<0) || (draw->script_light_idx>=max_model_light)) draw->script_light_idx=0;

	return(JS_TRUE);
}

bool js_model_light_set_on(jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	light->on=script_value_to_bool(*vp);

	return(JS_TRUE);
}

bool js_model_light_set_type(jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	light->type=script_value_to_int(*vp)-sd_light_type_normal;

	return(JS_TRUE);
}

bool js_model_light_set_filter(jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	light->filter=script_value_to_int(*vp)-sd_light_filter_none;

	return(JS_TRUE);
}

bool js_model_light_set_direction(jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	light->direction=script_value_to_int(*vp)-sd_light_direction_all;

	return(JS_TRUE);
}

bool js_model_light_set_intensity(jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];

	light->intensity=script_value_to_int(*vp);

	return(JS_TRUE);
}

bool js_model_light_set_exponent(jsval *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	light->exponent=script_value_to_float(*vp);

	return(JS_TRUE);
}

