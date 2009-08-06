/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.lightColor object

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

JSBool js_model_light_color_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_model_light_color_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
bool js_model_light_color_get_red(JSValueRef *vp);
bool js_model_light_color_get_green(JSValueRef *vp);
bool js_model_light_color_get_blue(JSValueRef *vp);
bool js_model_light_color_set_red(JSValueRef *vp);
bool js_model_light_color_set_green(JSValueRef *vp);
bool js_model_light_color_set_blue(JSValueRef *vp);

extern js_type			js;

script_js_property	model_light_color_props[]={
							{"red",					js_model_light_color_get_red,			js_model_light_color_set_red},
							{"green",				js_model_light_color_get_green,			js_model_light_color_set_green},
							{"blue",				js_model_light_color_get_blue,			js_model_light_color_set_blue},
							{0}};

JSClassRef			model_light_color_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_light_color_object(void)
{
	model_light_color_class=script_create_class("model_light_color_class",js_model_light_color_get_property,js_model_light_color_set_property);
}

void script_free_model_light_color_object(void)
{
	script_free_class(model_light_color_class);
}

JSObject* script_add_model_light_color_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,model_light_color_class,"lightColor",model_light_color_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_model_light_color_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,model_light_color_props));
}

JSBool js_model_light_color_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,model_light_color_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_model_light_color_get_red(JSValueRef *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	*vp=script_float_to_value(light->col.r);

	return(TRUE);
}

bool js_model_light_color_get_green(JSValueRef *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];

	*vp=script_float_to_value(light->col.g);

	return(TRUE);
}

bool js_model_light_color_get_blue(JSValueRef *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];
	
	*vp=script_float_to_value(light->col.b);

	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_model_light_color_set_red(JSValueRef *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];

	light->col.r=script_value_to_float(*vp);

	return(TRUE);
}

bool js_model_light_color_set_green(JSValueRef *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];

	light->col.g=script_value_to_float(*vp);

	return(TRUE);
}

bool js_model_light_color_set_blue(JSValueRef *vp)
{
	model_draw			*draw;
	model_draw_light	*light;

	draw=script_find_model_draw();
	light=&draw->lights[draw->script_light_idx];

	light->col.b=script_value_to_float(*vp);

	return(TRUE);
}

