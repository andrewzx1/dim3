/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.offset object

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

JSValueRef js_model_offset_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_model_offset_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_model_offset_get_x(void);
JSValueRef js_model_offset_get_y(void);
JSValueRef js_model_offset_get_z(void);
bool js_model_offset_set_x(JSValueRef vp);
bool js_model_offset_set_y(JSValueRef vp);
bool js_model_offset_set_z(JSValueRef vp);

extern js_type			js;

script_js_property	model_offset_props[]={
							{"x",					js_model_offset_get_x,				js_model_offset_set_x},
							{"y",					js_model_offset_get_y,				js_model_offset_set_y},
							{"z",					js_model_offset_get_z,				js_model_offset_set_z},
							{0}};

JSClassRef			model_offset_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_offset_object(void)
{
	model_offset_class=script_create_class("model_offset_class",js_model_offset_get_property,js_model_offset_set_property);
}

void script_free_model_offset_object(void)
{
	script_free_class(model_offset_class);
}

JSObjectRef script_add_model_offset_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,model_offset_class,"offset",model_offset_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_model_offset_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,model_offset_props));
}

bool js_model_offset_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,model_offset_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_model_offset_get_x(void)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	*vp=script_int_to_value(draw->offset.x);

	return(TRUE);
}

JSValueRef js_model_offset_get_y(void)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	*vp=script_int_to_value(draw->offset.y);

	return(TRUE);
}

JSValueRef js_model_offset_get_z(void)
{
	model_draw		*draw;

	draw=script_find_model_draw();
	*vp=script_int_to_value(draw->offset.z);

	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_model_offset_set_x(JSValueRef vp)
{
	model_draw		*draw;
	
	draw=script_find_model_draw();
	draw->offset.x=script_value_to_int(*vp);

	return(TRUE);
}

bool js_model_offset_set_y(JSValueRef vp)
{
	model_draw		*draw;
	
	draw=script_find_model_draw();
	draw->offset.y=script_value_to_int(*vp);

	return(TRUE);
}

bool js_model_offset_set_z(JSValueRef vp)
{
	model_draw		*draw;
	
	draw=script_find_model_draw();
	draw->offset.z=script_value_to_int(*vp);

	return(TRUE);
}


