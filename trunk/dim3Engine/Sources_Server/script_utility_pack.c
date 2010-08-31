/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: utility.pack object

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

extern js_type			js;

JSValueRef js_utility_pack_pack_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_utility_pack_unpack_high_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_utility_pack_unpack_low_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	utility_pack_functions[]={
							{"pack",				js_utility_pack_pack_func,				kJSPropertyAttributeDontDelete},
							{"unpackHigh",			js_utility_pack_unpack_high_func,		kJSPropertyAttributeDontDelete},
							{"unpackLow",			js_utility_pack_unpack_low_func,		kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			utility_pack_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_utility_pack_object(void)
{
	utility_pack_class=script_create_class("utility_pack_class",NULL,utility_pack_functions);
}

void script_free_utility_pack_object(void)
{
	script_free_class(utility_pack_class);
}

JSObjectRef script_add_utility_pack_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,utility_pack_class,"pack"));
}

/* =======================================================

      Random Functions
      
======================================================= */

JSValueRef js_utility_pack_pack_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	unsigned long			i,high,low;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));

	high=(unsigned long)script_value_to_int(cx,argv[0]);
	low=(unsigned long)script_value_to_int(cx,argv[1]);
	i=(int)(((high<<16)&0xFFFF0000)|(low&0xFFFF));

	return(script_int_to_value(cx,i));
}

JSValueRef js_utility_pack_unpack_high_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	unsigned int			i;
	int						high;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	i=(unsigned int)script_value_to_int(cx,argv[0]);
	high=(int)((i>>16)&0xFFFF);
	
	return(script_int_to_value(cx,high));
}

JSValueRef js_utility_pack_unpack_low_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	unsigned int			i;
	int						low;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	i=(unsigned int)script_value_to_int(cx,argv[0]);
	low=(int)(i&0xFFFF);
	
	return(script_int_to_value(cx,low));
}
