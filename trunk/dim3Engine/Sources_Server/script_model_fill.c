/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.fill object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
#include "models.h"

extern js_type			js;

JSValueRef js_model_fill_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_model_fill_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_model_fill_change_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_function	model_fill_functions[]={
							{"change",				js_model_fill_change_func,				2},
							{0}};

JSClassRef			model_fill_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_fill_object(void)
{
	model_fill_class=script_create_class("model_fill_class",js_model_fill_get_property,js_model_fill_set_property);
}

void script_free_model_fill_object(void)
{
	script_free_class(model_fill_class);
}

JSObjectRef script_add_model_fill_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,model_fill_class,"fill",NULL,model_fill_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_model_fill_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,NULL));
}

bool js_model_fill_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,NULL));
}

/* =======================================================

      Model Fill Functions
      
======================================================= */

JSValueRef js_model_fill_change_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    model_draw		*draw;
	
	draw=script_find_model_draw();
    
    model_change_fill(draw,script_value_to_int(argv[0]),script_value_to_int(argv[1]));
	return(TRUE);
}

