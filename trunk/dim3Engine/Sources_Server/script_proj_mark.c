/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.mark object

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
#include "projectiles.h"

extern js_type			js;

JSBool js_proj_mark_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_mark_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_mark_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_mark_get_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_mark_get_size(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_mark_get_alpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_mark_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_mark_set_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_mark_set_size(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_proj_mark_set_alpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

script_js_property	proj_mark_props[]={
							{"on",					js_proj_mark_get_on,				js_proj_mark_set_on},
							{"name",				js_proj_mark_get_name,				js_proj_mark_set_name},
							{"size",				js_proj_mark_get_size,				js_proj_mark_set_size},
							{"alpha",				js_proj_mark_get_alpha,				js_proj_mark_set_alpha},
							{0}};

JSClass				*proj_mark_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_proj_mark_object(void)
{
	proj_mark_class=script_create_class("proj_mark_class",js_proj_mark_get_property,js_proj_mark_set_property);
}

void script_free_proj_mark_object(void)
{
	script_free_class(proj_mark_class);
}

void script_add_proj_mark_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,proj_mark_class,"mark",proj_mark_props,NULL);
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_proj_mark_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,proj_mark_props));
}

JSBool js_proj_mark_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,proj_mark_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_proj_mark_get_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	*vp=script_bool_to_value(proj_setup->mark.on);
	
	return(JS_TRUE);
}

JSBool js_proj_mark_get_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	*vp=script_string_to_value(proj_setup->mark.name);
	
	return(JS_TRUE);
}

JSBool js_proj_mark_get_size(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	*vp=script_int_to_value(proj_setup->mark.size);
	
	return(JS_TRUE);
}

JSBool js_proj_mark_get_alpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
    *vp=script_float_to_value(proj_setup->mark.alpha);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_proj_mark_set_on(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
    proj_setup->mark.on=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

JSBool js_proj_mark_set_name(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	script_value_to_string(*vp,proj_setup->mark.name,name_str_len);
	proj_setup_attach_mark(proj_setup);
	
	return(JS_TRUE);
}

JSBool js_proj_mark_set_size(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	proj_setup->mark.size=script_value_to_int(*vp);
	
	return(JS_TRUE);
}

JSBool js_proj_mark_set_alpha(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(JS_TRUE);
	
	proj_setup->mark.alpha=script_value_to_float(*vp);
	
	return(JS_TRUE);
}





