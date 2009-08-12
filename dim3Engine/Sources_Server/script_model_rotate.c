/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.rotate object

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

JSValueRef js_model_rotate_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_model_rotate_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_model_rotate_get_x(void);
JSValueRef js_model_rotate_get_y(void);
JSValueRef js_model_rotate_get_z(void);
void js_model_rotate_set_x(JSValueRef vp,JSValueRef *exception);
void js_model_rotate_set_y(JSValueRef vp,JSValueRef *exception);
void js_model_rotate_set_z(JSValueRef vp,JSValueRef *exception);

extern js_type			js;

script_js_property	model_rotate_props[]={
							{"x",					js_model_rotate_get_x,				js_model_rotate_set_x},
							{"y",					js_model_rotate_get_y,				js_model_rotate_set_y},
							{"z",					js_model_rotate_get_z,				js_model_rotate_set_z},
							{0}};

JSClassRef			model_rotate_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_rotate_object(void)
{
	model_rotate_class=script_create_class("model_rotate_class",js_model_rotate_get_property,js_model_rotate_set_property);
}

void script_free_model_rotate_object(void)
{
	script_free_class(model_rotate_class);
}

JSObjectRef script_add_model_rotate_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,model_rotate_class,"rotate",model_rotate_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_model_rotate_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,model_rotate_props));
}

bool js_model_rotate_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,model_rotate_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_model_rotate_get_x(void)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	*vp=script_float_to_value(draw->rot.x);

	return(TRUE);
}

JSValueRef js_model_rotate_get_y(void)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	*vp=script_float_to_value(draw->rot.y);

	return(TRUE);
}

JSValueRef js_model_rotate_get_z(void)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	*vp=script_float_to_value(draw->rot.z);

	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

void js_model_rotate_set_x(JSValueRef vp,JSValueRef *exception)
{
	model_draw		*draw;
	
	draw=script_find_model_draw();
	draw->rot.x=script_value_to_float(*vp);
}

void js_model_rotate_set_y(JSValueRef vp,JSValueRef *exception)
{
	model_draw		*draw;
	
	draw=script_find_model_draw();
	draw->rot.y=script_value_to_float(*vp);
}

void js_model_rotate_set_z(JSValueRef vp,JSValueRef *exception)
{
	model_draw		*draw;
	
	draw=script_find_model_draw();
	draw->rot.z=script_value_to_float(*vp);
}

