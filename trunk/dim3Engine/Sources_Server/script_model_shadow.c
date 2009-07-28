/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.shadow object

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

JSBool js_model_shadow_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_shadow_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_shadow_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_shadow_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

extern js_type			js;

script_js_property	model_shadow_props[]={
							{"on",					js_model_shadow_get_on,				js_model_shadow_set_on},
							{0}};

JSClass				*model_shadow_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_shadow_object(void)
{
	model_shadow_class=script_create_class("model_shadow_class",js_model_shadow_get_property,js_model_shadow_set_property);
}

void script_free_model_shadow_object(void)
{
	script_free_class(model_shadow_class);
}

void script_add_model_shadow_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,model_shadow_class,"shadow",model_shadow_props,NULL);
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_model_shadow_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,model_shadow_props));
}

JSBool js_model_shadow_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,model_shadow_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_model_shadow_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_shadow	*shadow;

	draw=script_find_model_draw(j_obj,TRUE);
	shadow=&draw->shadow;
	
	*vp=script_bool_to_value(shadow->on);

	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_model_shadow_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	model_draw			*draw;
	model_draw_shadow	*shadow;

	draw=script_find_model_draw(j_obj,TRUE);
	shadow=&draw->shadow;

	shadow->on=script_value_to_bool(*vp);

	return(JS_TRUE);
}

