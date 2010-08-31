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

JSStaticFunction	script_functions[]={
							{"implements",			js_script_implements_func,			kJSPropertyAttributeDontDelete},
							{"attachEvent",			js_script_attach_event_func,		kJSPropertyAttributeDontDelete},
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
/*
	int				idx;
	char			name[name_str_len];
	d3pnt			pt;
	
	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));
	
	pt.x=script_value_to_int(cx,argv[0]);
	pt.z=script_value_to_int(cx,argv[1]);
	pt.y=script_value_to_int(cx,argv[2]);
	
	script_value_to_string(cx,argv[3],name,name_str_len);
	
	idx=particle_find_index(name);
	if (idx==-1) {
		*exception=js_particle_name_exception(cx,name);
	}
	else {
		script_bool_to_value(cx,particle_spawn(idx,script_get_attached_object_uid(),&pt,NULL,NULL));
	}
*/
	return(script_null_to_value(cx));
}

JSValueRef js_script_attach_event_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
/*
	int				idx;
	char			name[name_str_len];
	d3pnt			pt;
	
	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));
	
	pt.x=script_value_to_int(cx,argv[0]);
	pt.z=script_value_to_int(cx,argv[1]);
	pt.y=script_value_to_int(cx,argv[2]);
	
	script_value_to_string(cx,argv[3],name,name_str_len);
	
	idx=particle_find_index(name);
	if (idx==-1) {
		*exception=js_particle_name_exception(cx,name);
	}
	else {
		script_bool_to_value(cx,particle_spawn(idx,script_get_attached_object_uid(),&pt,NULL,NULL));
	}
*/
	return(script_null_to_value(cx));
}

