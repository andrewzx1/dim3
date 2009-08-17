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

JSValueRef js_interface_screen_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_interface_screen_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_interface_screen_get_width(JSContextRef cx);
JSValueRef js_interface_screen_get_height(JSContextRef cx);

script_js_property	interface_screen_props[]={
							{"width",				js_interface_screen_get_width,		NULL},
							{"height",				js_interface_screen_get_height,		NULL},
							{0}};

JSClassRef			interface_screen_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_interface_screen_object(void)
{
	interface_screen_class=script_create_class("interface_screen_class",js_interface_screen_get_property,js_interface_screen_set_property);
}

void script_free_interface_screen_object(void)
{
	script_free_class(interface_screen_class);
}

JSObjectRef script_add_interface_screen_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,interface_screen_class,"screen",interface_screen_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_interface_screen_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,interface_screen_props));
}

bool js_interface_screen_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,interface_screen_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_interface_screen_get_width(JSContextRef cx)
{
	return(script_int_to_value(cx,setup.screen.x_sz));
}

JSValueRef js_interface_screen_get_height(JSContextRef cx)
{
	return(script_int_to_value(cx,setup.screen.y_sz));
}
