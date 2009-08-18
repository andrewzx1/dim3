/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.screen object

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
extern setup_type		setup;

JSValueRef js_interface_screen_get_width(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_interface_screen_get_height(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);

JSStaticValue 		interface_screen_props[]={
							{"width",				js_interface_screen_get_width,		NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"height",				js_interface_screen_get_height,		NULL,			kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			interface_screen_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_interface_screen_object(void)
{
	interface_screen_class=script_create_class("interface_screen_class",interface_screen_props,NULL);
}

void script_free_interface_screen_object(void)
{
	script_free_class(interface_screen_class);
}

JSObjectRef script_add_interface_screen_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,interface_screen_class,"screen"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_interface_screen_get_width(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,setup.screen.x_sz));
}

JSValueRef js_interface_screen_get_height(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_int_to_value(cx,setup.screen.y_sz));
}
