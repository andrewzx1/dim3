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

extern void script_add_model_offset_object(JSObject *parent_obj);
extern void script_add_model_rotate_object(JSObject *parent_obj);
extern void script_add_model_spin_object(JSObject *parent_obj);
extern void script_add_model_light_object(JSObject *parent_obj);
extern void script_add_model_light_color_object(JSObject *parent_obj);
extern void script_add_model_halo_object(JSObject *parent_obj);
extern void script_add_model_shadow_object(JSObject *parent_obj);
extern void script_add_model_animation_object(JSObject *parent_obj);
extern void script_add_model_mesh_object(JSObject *parent_obj);
extern void script_add_model_bone_object(JSObject *parent_obj);
extern void script_add_model_fill_object(JSObject *parent_obj);

JSBool js_model_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_get_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_get_bounce(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_get_alpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_get_resize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_get_faceForward(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_set_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_set_bounce(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_set_alpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_set_resize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_set_faceForward(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_start_animation_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_stop_animation_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_cancel_animation_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_change_animation_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_interrupt_animation_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_change_fill_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_find_bone_offset_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_model_find_bone_position_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_property	model_props[]={
							{"on",					js_model_get_on,					js_model_set_on},
							{"name",				js_model_get_name,					js_model_set_name},
							{"bounce",				js_model_get_bounce,				js_model_set_bounce},
							{"alpha",				js_model_get_alpha,					js_model_set_alpha},
							{"resize",				js_model_get_resize,				js_model_set_resize},
							{"faceForward",			js_model_get_faceForward,			js_model_set_faceForward},
							{0}};

JSClass				*model_class;

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

void script_add_model_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=script_create_child_object(parent_obj,model_class,"model",model_props,NULL);
	
	script_add_model_offset_object(j_obj);
	script_add_model_rotate_object(j_obj);
	script_add_model_spin_object(j_obj);
	script_add_model_light_object(j_obj);
	script_add_model_light_color_object(j_obj);
	script_add_model_halo_object(j_obj);
	script_add_model_shadow_object(j_obj);
	script_add_model_animation_object(j_obj);
	script_add_model_mesh_object(j_obj);
	script_add_model_bone_object(j_obj);
	script_add_model_fill_object(j_obj);
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

JSBool js_model_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj,FALSE);
	*vp=script_bool_to_value(draw->on);

	return(JS_TRUE);
}

JSBool js_model_get_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj,FALSE);
	*vp=script_string_to_value(draw->name);

	return(JS_TRUE);
}

JSBool js_model_get_bounce(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj,FALSE);
	*vp=script_bool_to_value(draw->bounce);

	return(JS_TRUE);
}

JSBool js_model_get_alpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj,FALSE);
	*vp=script_float_to_value(draw->alpha);

	return(JS_TRUE);
}

JSBool js_model_get_resize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj,FALSE);
	*vp=script_float_to_value(draw->resize);

	return(JS_TRUE);
}

JSBool js_model_get_faceForward(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj,FALSE);
	*vp=script_bool_to_value(draw->face_forward);

	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_model_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj,FALSE);
	draw->on=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

JSBool js_model_set_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj,FALSE);
	script_value_to_string(*vp,draw->name,name_str_len);
	
	return(JS_TRUE);
}

JSBool js_model_set_bounce(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj,FALSE);
	draw->bounce=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

JSBool js_model_set_alpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj,FALSE);
	draw->alpha=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_model_set_resize(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj,FALSE);
	draw->resize=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_model_set_faceForward(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw		*draw;

	draw=script_find_model_draw(j_obj,FALSE);
	draw->face_forward=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

