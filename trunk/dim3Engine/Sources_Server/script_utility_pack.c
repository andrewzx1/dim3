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

JSBool js_utility_pack_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_utility_pack_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_utility_pack_pack_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_utility_pack_unpack_high_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_utility_pack_unpack_low_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_function	utility_pack_functions[]={
							{"pack",				js_utility_pack_pack_func,				2},
							{"unpackHigh",			js_utility_pack_unpack_high_func,		1},
							{"unpackLow",			js_utility_pack_unpack_low_func,		1},
							{0}};

JSClass				*utility_pack_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_utility_pack_object(void)
{
	utility_pack_class=script_create_class("utility_pack_class",js_utility_pack_get_property,js_utility_pack_set_property);
}

void script_free_utility_pack_object(void)
{
	script_free_class(utility_pack_class);
}

void script_add_utility_pack_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,utility_pack_class,"pack",NULL,utility_pack_functions);
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_utility_pack_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,NULL));
}

JSBool js_utility_pack_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,NULL));
}

/* =======================================================

      Random Functions
      
======================================================= */

JSBool js_utility_pack_pack_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	unsigned long			i,high,low;

	high=(unsigned long)script_value_to_int(argv[0]);
	low=(unsigned long)script_value_to_int(argv[1]);
	i=(int)(((high<<16)&0xFFFF0000)|(low&0xFFFF));

	*rval=INT_TO_JSVAL(i);
	
	return(JS_TRUE);
}

JSBool js_utility_pack_unpack_high_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	unsigned int			i;
	int						high;
	
	i=(unsigned int)script_value_to_int(argv[0]);
	high=(int)((i>>16)&0xFFFF);
	
	*rval=INT_TO_JSVAL(high);
	
	return(JS_TRUE);
}

JSBool js_utility_pack_unpack_low_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	unsigned int			i;
	int						low;
	
	i=(unsigned int)script_value_to_int(argv[0]);
	low=(int)(i&0xFFFF);
	
	*rval=INT_TO_JSVAL(low);
	
	return(JS_TRUE);
}
