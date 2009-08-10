/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.mesh object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

JSValueRef js_model_mesh_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_model_mesh_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_model_mesh_show_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_mesh_show_all_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_mesh_show_only_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_mesh_hide_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_mesh_hide_all_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

extern js_type			js;

script_js_function	model_mesh_functions[]={
							{"showMesh",			js_model_mesh_show_mesh_func,				1},
							{"showAllMesh",			js_model_mesh_show_all_mesh_func,			0},
							{"showOnlyMesh",		js_model_mesh_show_only_mesh_func,			1},
							{"hideMesh",			js_model_mesh_hide_mesh_func,				1},
							{"hideAllMesh",			js_model_mesh_hide_all_mesh_func,			0},
							{0}};

JSClassRef			model_mesh_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_mesh_object(void)
{
	model_mesh_class=script_create_class("model_mesh_class",js_model_mesh_get_property,js_model_mesh_set_property);
}

void script_free_model_mesh_object(void)
{
	script_free_class(model_mesh_class);
}

JSObjectRef script_add_model_mesh_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,model_mesh_class,"mesh",NULL,model_mesh_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_model_mesh_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,NULL));
}

bool js_model_mesh_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,NULL));
}

/* =======================================================

      Model Mesh Functions
      
======================================================= */

JSValueRef js_model_mesh_show_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	draw=script_find_model_draw();

	script_value_to_string(argv[0],name,name_str_len);
	if (!model_show_mesh(draw,name)) {
		JS_ReportError(js.cx,"Named mesh does not exist: %s",name);
		return(FALSE);
	}
	
	return(TRUE);
}

JSValueRef js_model_mesh_show_all_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	model_draw		*draw;
	
	draw=script_find_model_draw();
	model_show_all_mesh(draw);

	return(TRUE);
}

JSValueRef js_model_mesh_show_only_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	draw=script_find_model_draw();

	script_value_to_string(argv[0],name,name_str_len);
	if (!model_show_only_mesh(draw,name)) {
		JS_ReportError(js.cx,"Named mesh does not exist: %s",name);
		return(FALSE);
	}
	
	return(TRUE);
}

JSValueRef js_model_mesh_hide_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	draw=script_find_model_draw();

	script_value_to_string(argv[0],name,name_str_len);
	if (!model_hide_mesh(draw,name)) {
		JS_ReportError(js.cx,"Named mesh does not exist: %s",name);
		return(FALSE);
	}
	
	return(TRUE);
}

JSValueRef js_model_mesh_hide_all_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	model_draw		*draw;
	
	draw=script_find_model_draw();
	model_hide_all_mesh(draw);

	return(TRUE);
}

