/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: data object

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

extern js_type			js;

JSValueRef js_data_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_data_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_data_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_data_sub_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_data_set_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_data_get_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_data_add_specific_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_data_sub_specific_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_data_set_specific_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_data_get_specific_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_function	data_functions[]={
							{"add",					js_data_add_func,					2},
							{"sub",					js_data_sub_func,					1},
							{"set",					js_data_set_func,					2},
							{"get",					js_data_get_func,					1},
							{"addScriptSpecific",	js_data_add_specific_func,			2},
							{"subScriptSpecific",	js_data_sub_specific_func,			1},
							{"setScriptSpecific",	js_data_set_specific_func,			2},
							{"getScriptSpecific",	js_data_get_specific_func,			1},
							{0}};

JSClassRef			data_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_global_data_object(void)
{
	data_class=script_create_class("data_class",js_data_get_property,js_data_set_property);
}

void script_free_global_data_object(void)
{
	script_free_class(data_class);
}

JSObjectRef script_add_global_data_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,data_class,"data",NULL,data_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_data_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,NULL));
}

bool js_data_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,NULL));
}

/* =======================================================

      Data Functions
      
======================================================= */

JSValueRef js_data_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];

	script_value_to_string(argv[0],name,name_str_len);
	script_add_global(name,-1,argv[1]);
	
	return(TRUE);
}

JSValueRef js_data_sub_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];

	script_value_to_string(argv[0],name,name_str_len);
	script_delete_global(name,-1);
	
	return(TRUE);
}

JSValueRef js_data_set_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];

	script_value_to_string(argv[0],name,name_str_len);
	script_set_global(name,-1,argv[1]);

	return(TRUE);
}

JSValueRef js_data_get_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];

	script_value_to_string(argv[0],name,name_str_len);
	*rval=script_get_global(name,-1);
	
	return(TRUE);
}

JSValueRef js_data_add_specific_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];

	script_value_to_string(argv[0],name,name_str_len);
	script_add_global(name,js.attach.script_uid,argv[1]);
	
	return(TRUE);
}

JSValueRef js_data_sub_specific_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];

	script_value_to_string(argv[0],name,name_str_len);
	script_delete_global(name,js.attach.script_uid);
	
	return(TRUE);
}

JSValueRef js_data_set_specific_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];

	script_value_to_string(argv[0],name,name_str_len);
	script_set_global(name,js.attach.script_uid,argv[1]);

	return(TRUE);
}

JSValueRef js_data_get_specific_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];

	script_value_to_string(argv[0],name,name_str_len);
	*rval=script_get_global(name,js.attach.script_uid);
	
	return(TRUE);
}

