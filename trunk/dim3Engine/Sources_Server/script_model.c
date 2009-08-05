/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model object

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
#include "objects.h"
#include "weapons.h"
#include "projectiles.h"

extern js_type			js;
extern hud_type			hud;
extern setup_type		setup;

JSBool js_model_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
bool js_model_get_on(jsval *vp);
bool js_model_get_name(jsval *vp);
bool js_model_get_bounce(jsval *vp);
bool js_model_get_alpha(jsval *vp);
bool js_model_get_resize(jsval *vp);
bool js_model_get_faceForward(jsval *vp);
bool js_model_set_on(jsval *vp);
bool js_model_set_name(jsval *vp);
bool js_model_set_bounce(jsval *vp);
bool js_model_set_alpha(jsval *vp);
bool js_model_set_resize(jsval *vp);
bool js_model_set_faceForward(jsval *vp);

script_js_property	model_props[]={
							{"on",					js_model_get_on,					js_model_set_on},
							{"name",				js_model_get_name,					js_model_set_name},
							{"bounce",				js_model_get_bounce,				js_model_set_bounce},
							{"alpha",				js_model_get_alpha,					js_model_set_alpha},
							{"resize",				js_model_get_resize,				js_model_set_resize},
							{"faceForward",			js_model_get_faceForward,			js_model_set_faceForward},
							{0}};

JSClassRef			model_class;

/* =======================================================

      Create Model
      
======================================================= */

void script_init_model_object(void)
{
	model_class=script_create_class("model_class",js_model_get_property,js_model_set_property);
}

void script_free_model_object(void)
{
	script_free_class(model_class);
}

JSObject* script_add_model_object(JSObject *parent_obj)
{
    return(script_create_child_object(parent_obj,model_class,"model",model_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_model_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,model_props));
}

JSBool js_model_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,model_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_model_get_on(jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	*vp=script_bool_to_value(draw->on);

	return(JS_TRUE);
}

bool js_model_get_name(jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	*vp=script_string_to_value(draw->name);

	return(JS_TRUE);
}

bool js_model_get_bounce(jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	*vp=script_bool_to_value(draw->bounce);

	return(JS_TRUE);
}

bool js_model_get_alpha(jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	*vp=script_float_to_value(draw->alpha);

	return(JS_TRUE);
}

bool js_model_get_resize(jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	*vp=script_float_to_value(draw->resize);

	return(JS_TRUE);
}

bool js_model_get_faceForward(jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	*vp=script_bool_to_value(draw->face_forward);

	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_model_set_on(jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	draw->on=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_model_set_name(jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	script_value_to_string(*vp,draw->name,name_str_len);
	
	return(JS_TRUE);
}

bool js_model_set_bounce(jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	draw->bounce=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_model_set_alpha(jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	draw->alpha=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

bool js_model_set_resize(jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	draw->resize=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

bool js_model_set_faceForward(jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	draw->face_forward=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

