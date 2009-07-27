/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.objectSpeed object

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

extern js_type			js;

JSBool js_obj_object_speed_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_object_speed_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_object_speed_get_jumpHeight(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_object_speed_get_duckAdd(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_object_speed_get_duckChange(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_object_speed_get_bumpHeight(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_object_speed_get_bumpSmoothFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_object_speed_get_bounceFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_object_speed_get_noSlide(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_object_speed_set_jumpHeight(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_object_speed_set_duckAdd(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_object_speed_set_duckChange(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_object_speed_set_bumpHeight(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_object_speed_set_bumpSmoothFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_object_speed_set_bounceFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_object_speed_set_noSlide(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

script_js_property	obj_object_speed_props[]={
							{"jumpHeight",				js_obj_object_speed_get_jumpHeight,			js_obj_object_speed_set_jumpHeight},
							{"duckAdd",					js_obj_object_speed_get_duckAdd,			js_obj_object_speed_set_duckAdd},
							{"duckChange",				js_obj_object_speed_get_duckChange,			js_obj_object_speed_set_duckChange},
							{"bumpHeight",				js_obj_object_speed_get_bumpHeight,			js_obj_object_speed_set_bumpHeight},
							{"bumpSmoothFactor",		js_obj_object_speed_get_bumpSmoothFactor,	js_obj_object_speed_set_bumpSmoothFactor},
							{"bounceFactor",			js_obj_object_speed_get_bounceFactor,		js_obj_object_speed_set_bounceFactor},
							{"noSlide",					js_obj_object_speed_get_noSlide,			js_obj_object_speed_set_noSlide},
							{0}};

JSClass				*obj_object_speed_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_object_speed_object(void)
{
	obj_object_speed_class=script_create_class("obj_object_speed_class",js_obj_object_speed_get_property,js_obj_object_speed_set_property);
}

void script_free_obj_object_speed_object(void)
{
	script_free_class(obj_object_speed_class);
}

void script_add_obj_object_speed_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"objectSpeed",obj_object_speed_props,NULL);
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_obj_object_speed_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,obj_object_speed_props));
}

JSBool js_obj_object_speed_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,obj_object_speed_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSBool js_obj_object_speed_get_jumpHeight(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->jump.y_add);
	
	return(JS_TRUE);
}

JSBool js_obj_object_speed_get_duckAdd(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->duck.y_add);
	
	return(JS_TRUE);
}

JSBool js_obj_object_speed_get_duckChange(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->duck.y_change);
	
	return(JS_TRUE);
}

JSBool js_obj_object_speed_get_bumpHeight(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=INT_TO_JSVAL(obj->bump.high);
	
	return(JS_TRUE);
}

JSBool js_obj_object_speed_get_bumpSmoothFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->bump.smooth_factor);
	
	return(JS_TRUE);
}

JSBool js_obj_object_speed_get_bounceFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_float_to_value(obj->bounce.factor);
	
	return(JS_TRUE);
}

JSBool js_obj_object_speed_get_noSlide(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->no_slide);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

JSBool js_obj_object_speed_set_jumpHeight(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->jump.y_add=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_object_speed_set_duckAdd(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->duck.y_add=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_object_speed_set_duckChange(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->duck.y_change=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_object_speed_set_bumpHeight(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->bump.high=JSVAL_TO_INT(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_object_speed_set_bumpSmoothFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->bump.smooth_factor=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_object_speed_set_bounceFactor(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->bounce.factor=script_value_to_float(*vp);
	
	return(JS_TRUE);
}

JSBool js_obj_object_speed_set_noSlide(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->no_slide=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}


