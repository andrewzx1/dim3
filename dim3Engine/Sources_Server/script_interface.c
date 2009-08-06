/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface object

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

JSBool js_interface_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_interface_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);

JSClassRef			interface_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_global_interface_object(void)
{
	interface_class=script_create_class("interface_class",js_interface_get_property,js_interface_set_property);
}

void script_free_global_interface_object(void)
{
	script_free_class(interface_class);
}

JSObject* script_add_global_interface_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,interface_class,"iface",NULL,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_interface_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,NULL));
}

JSBool js_interface_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,NULL));
}

