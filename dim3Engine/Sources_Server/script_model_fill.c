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

JSBool js_model_fill_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_fill_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_model_fill_change_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_function	model_fill_functions[]={
							{"change",				js_model_fill_change_func,				2},
							{0}};

JSClass				*model_fill_class;

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

JSObject* script_add_model_fill_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,model_fill_class,"fill",NULL,model_fill_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_model_fill_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,NULL));
}

JSBool js_model_fill_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,NULL));
}

/* =======================================================

      Model Fill Functions
      
======================================================= */

JSBool js_model_fill_change_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
    model_draw		*draw;
	
	draw=script_find_model_draw(j_obj,TRUE);
    
    model_change_fill(draw,script_value_to_int(argv[0]),script_value_to_int(argv[1]));
	return(JS_TRUE);
}

