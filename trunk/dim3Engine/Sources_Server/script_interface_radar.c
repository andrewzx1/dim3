/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.radar object

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
#include "interfaces.h"

extern js_type			js;
extern setup_type		setup;
extern hud_type			hud;

JSBool js_interface_radar_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_interface_radar_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
bool js_interface_radar_get_on(JSValueRef *vp);
bool js_interface_radar_get_x(JSValueRef *vp);
bool js_interface_radar_get_y(JSValueRef *vp);
bool js_interface_radar_get_displayRadius(JSValueRef *vp);
bool js_interface_radar_get_viewRadius(JSValueRef *vp);
bool js_interface_radar_set_on(JSValueRef *vp);
bool js_interface_radar_set_x(JSValueRef *vp);
bool js_interface_radar_set_y(JSValueRef *vp);
bool js_interface_radar_set_displayRadius(JSValueRef *vp);
bool js_interface_radar_set_viewRadius(JSValueRef *vp);

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

JSObject* script_add_interface_radar_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,interface_radar_class,"radar",interface_radar_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_interface_radar_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,interface_radar_props));
}

JSBool js_interface_radar_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,interface_radar_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_interface_radar_get_on(JSValueRef *vp)
{
	*vp=script_bool_to_value(hud.radar.on);
	return(TRUE);
}

bool js_interface_radar_get_x(JSValueRef *vp)
{
	*vp=script_int_to_value(hud.radar.x);
	return(TRUE);
}

bool js_interface_radar_get_y(JSValueRef *vp)
{
	*vp=script_int_to_value(hud.radar.y);
	return(TRUE);
}

bool js_interface_radar_get_displayRadius(JSValueRef *vp)
{
	*vp=script_int_to_value(hud.radar.display_radius);
	return(TRUE);
}

bool js_interface_radar_get_viewRadius(JSValueRef *vp)
{
	*vp=script_int_to_value(hud.radar.view_radius);
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_interface_radar_set_on(JSValueRef *vp)
{
	hud.radar.on=script_value_to_bool(*vp);
	return(TRUE);
}

bool js_interface_radar_set_x(JSValueRef *vp)
{
	hud.radar.x=script_value_to_int(*vp);
	return(TRUE);
}

bool js_interface_radar_set_y(JSValueRef *vp)
{
	hud.radar.y=script_value_to_int(*vp);
	return(TRUE);
}

bool js_interface_radar_set_displayRadius(JSValueRef *vp)
{
	hud.radar.display_radius=script_value_to_int(*vp);
	return(TRUE);
}

bool js_interface_radar_set_viewRadius(JSValueRef *vp)
{
	hud.radar.view_radius=script_value_to_int(*vp);
	return(TRUE);
}

