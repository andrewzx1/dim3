/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.radar object

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
#include "interfaces.h"

extern js_type			js;
extern setup_type		setup;
extern hud_type			hud;

JSValueRef js_interface_radar_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_interface_radar_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_interface_radar_get_on(JSContextRef cx);
JSValueRef js_interface_radar_get_x(JSContextRef cx);
JSValueRef js_interface_radar_get_y(JSContextRef cx);
JSValueRef js_interface_radar_get_displayRadius(JSContextRef cx);
JSValueRef js_interface_radar_get_viewRadius(JSContextRef cx);
void js_interface_radar_set_on(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_interface_radar_set_x(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_interface_radar_set_y(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_interface_radar_set_displayRadius(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_interface_radar_set_viewRadius(JSContextRef cx,JSValueRef vp,JSValueRef *exception);

script_js_property	interface_radar_props[]={
							{"on",					js_interface_radar_get_on,				js_interface_radar_set_on},
							{"x",					js_interface_radar_get_x,				js_interface_radar_set_x},
							{"y",					js_interface_radar_get_y,				js_interface_radar_set_y},
							{"displayRadius",		js_interface_radar_get_displayRadius,	js_interface_radar_set_displayRadius},
							{"viewRadius",			js_interface_radar_get_viewRadius,		js_interface_radar_set_viewRadius},
							{0}};

JSClassRef			interface_radar_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_interface_radar_object(void)
{
	interface_radar_class=script_create_class("interface_radar_class",js_interface_radar_get_property,js_interface_radar_set_property);
}

void script_free_interface_radar_object(void)
{
	script_free_class(interface_radar_class);
}

JSObjectRef script_add_interface_radar_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,interface_radar_class,"radar",interface_radar_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_interface_radar_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,interface_radar_props));
}

bool js_interface_radar_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,interface_radar_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_interface_radar_get_on(JSContextRef cx)
{
	return(script_bool_to_value(cx,hud.radar.on));
}

JSValueRef js_interface_radar_get_x(JSContextRef cx)
{
	return(script_int_to_value(cx,hud.radar.x));
}

JSValueRef js_interface_radar_get_y(JSContextRef cx)
{
	return(script_int_to_value(cx,hud.radar.y));
}

JSValueRef js_interface_radar_get_displayRadius(JSContextRef cx)
{
	return(script_int_to_value(cx,hud.radar.display_radius));
}

JSValueRef js_interface_radar_get_viewRadius(JSContextRef cx)
{
	return(script_int_to_value(cx,hud.radar.view_radius));
}

/* =======================================================

      Setters
      
======================================================= */

void js_interface_radar_set_on(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	hud.radar.on=script_value_to_bool(cx,vp);
}

void js_interface_radar_set_x(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	hud.radar.x=script_value_to_int(cx,vp);
}

void js_interface_radar_set_y(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	hud.radar.y=script_value_to_int(cx,vp);
}

void js_interface_radar_set_displayRadius(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	hud.radar.display_radius=script_value_to_int(cx,vp);
}

void js_interface_radar_set_viewRadius(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	hud.radar.view_radius=script_value_to_int(cx,vp);
}

