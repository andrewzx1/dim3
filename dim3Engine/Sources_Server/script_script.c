/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: script object

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

#include "objects.h"
#include "effects.h"
#include "scripts.h"
#include "physics.h"

extern server_type		server;
extern js_type			js;

JSValueRef js_script_implements_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_script_attach_event_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_script_call_parent_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	script_functions[]={
							{"implements",			js_script_implements_func,			kJSPropertyAttributeDontDelete},
							{"attachEvent",			js_script_attach_event_func,		kJSPropertyAttributeDontDelete},
							{"callParent",			js_script_call_parent_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			script_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_global_script_object(void)
{
	script_class=script_create_class("script_class",NULL,script_functions);
}

void script_free_global_script_object(void)
{
	script_free_class(script_class);
}

JSObjectRef script_add_global_script_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,script_class,"script"));
}

/* =======================================================

      Script Functions
      
======================================================= */

JSValueRef js_script_implements_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[file_str_len],err_str[256];
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[0],name,file_str_len);

	if (!scripts_add_parent(&js.attach,name,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_script_attach_event_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				main_event;
	char			func_name[256],err_str[256];
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	main_event=script_value_to_int(cx,argv[0]);
	script_value_to_string(cx,argv[1],func_name,256);

	if (!scripts_setup_event_attach(&js.attach,main_event,func_name,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_script_call_parent_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			err_str[256];
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));

	if (!scripts_post_event_call_parent(&js.attach,err_str)) {
		*exception=script_create_exception(cx,err_str);
	}

	return(script_null_to_value(cx));
}
