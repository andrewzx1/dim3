/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.spin object

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

JSBool js_model_spin_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_spin_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_spin_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_spin_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_spin_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_spin_set_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_spin_set_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_spin_set_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

extern js_type			js;

script_js_property	model_spin_props[]={
							{"x",					js_model_spin_get_x,					js_model_spin_set_x},
							{"y",					js_model_spin_get_y,					js_model_spin_set_y},
							{"z",					js_model_spin_get_z,					js_model_spin_set_z},
							{0}};

JSClassRef			model_spin_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_spin_object(void)
{
	model_spin_class=script_create_class("model_spin_class",js_model_spin_get_property,js_model_spin_set_property);
}

void script_free_model_spin_object(void)
{
	script_free_class(model_spin_class);
}

JSObject* script_add_model_spin_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,model_spin_class,"spin",model_spin_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_model_spin_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,model_spin_props));
}

JSBool js_model_spin_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,model_spin_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_model_spin_get_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	*vp=script_float_to_value(draw->spin.x);

	return(JS_TRUE);
}

bool js_model_spin_get_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	*vp=script_float_to_value(draw->spin.y);

	return(JS_TRUE);
}

bool js_model_spin_get_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	*vp=script_float_to_value(draw->spin.z);

	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_model_spin_set_x(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;
	
	draw=script_find_model_draw();

	draw->spin.x=script_value_to_float(*vp);
	draw->spin_tick=js.time.current_tick;

	return(JS_TRUE);
}

bool js_model_spin_set_y(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;
	
	draw=script_find_model_draw();

	draw->spin.y=script_value_to_float(*vp);
	draw->spin_tick=js.time.current_tick;

	return(JS_TRUE);
}

bool js_model_spin_set_z(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;
	
	draw=script_find_model_draw();

	draw->spin.z=script_value_to_float(*vp);
	draw->spin_tick=js.time.current_tick;

	return(JS_TRUE);
}

