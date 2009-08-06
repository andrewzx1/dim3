/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.console object

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
#include "consoles.h"

extern char				console_input_str[max_console_txt_sz];
extern js_type			js;

JSBool js_interface_console_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_interface_console_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_interface_console_write_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_console_read_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);

script_js_function	interface_console_functions[]={
							{"write",				js_interface_console_write_func,		1},
							{"read",				js_interface_console_read_func,			0},
							{0}};

JSClassRef			interface_console_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_interface_console_object(void)
{
	interface_console_class=script_create_class("interface_console_class",js_interface_console_get_property,js_interface_console_set_property);
}

void script_free_interface_console_object(void)
{
	script_free_class(interface_console_class);
}

JSObject* script_add_interface_console_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,interface_console_class,"console",NULL,interface_console_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_interface_console_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,NULL));
}

JSBool js_interface_console_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,NULL));
}

/* =======================================================

      Console Function
      
======================================================= */

JSBool js_interface_console_write_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	char			txt[256];
	
	script_value_to_string(argv[0],txt,256);
	console_add(txt);
	
	return(TRUE);
}

JSBool js_interface_console_read_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	*rval=script_string_to_value(console_input_str);
	return(TRUE);
}


