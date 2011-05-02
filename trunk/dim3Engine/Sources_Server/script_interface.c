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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"

extern js_type			js;

JSClassRef			interface_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_global_interface_object(void)
{
	interface_class=script_create_class("interface_class",NULL,NULL);
}

void script_free_global_interface_object(void)
{
	script_free_class(interface_class);
}

JSObjectRef script_add_global_interface_object(JSContextRef cx,JSObjectRef parent_obj,attach_type *attach)
{
	return(script_create_child_object(cx,parent_obj,interface_class,"iface",attach));
}

