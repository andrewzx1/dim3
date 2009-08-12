/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.size object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
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
#include "projectiles.h"

extern js_type			js;

JSValueRef js_proj_size_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_proj_size_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_proj_size_get_x(void);
JSValueRef js_proj_size_get_y(void);
JSValueRef js_proj_size_get_z(void);
JSValueRef js_proj_size_get_weight(void);
void js_proj_size_set_x(JSValueRef vp,JSValueRef *exception);
void js_proj_size_set_y(JSValueRef vp,JSValueRef *exception);
void js_proj_size_set_z(JSValueRef vp,JSValueRef *exception);
void js_proj_size_set_weight(JSValueRef vp,JSValueRef *exception);

script_js_property	proj_size_props[]={
							{"x",					js_proj_size_get_x,					js_proj_size_set_x},
							{"y",					js_proj_size_get_y,					js_proj_size_set_y},
							{"z",					js_proj_size_get_z,					js_proj_size_set_z},
							{"weight",				js_proj_size_get_weight,			js_proj_size_set_weight},
							{0}};

JSClassRef			proj_size_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_proj_size_object(void)
{
	proj_size_class=script_create_class("proj_size_class",js_proj_size_get_property,js_proj_size_set_property);
}

void script_free_proj_size_object(void)
{
	script_free_class(proj_size_class);
}

JSObjectRef script_add_proj_size_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,proj_size_class,"size",proj_size_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_proj_size_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,proj_size_props));
}

bool js_proj_size_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,proj_size_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_proj_size_get_x(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_int_to_value(proj_setup->size.x);
	
	return(TRUE);
}

JSValueRef js_proj_size_get_y(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_int_to_value(proj_setup->size.y);
	
	return(TRUE);
}

JSValueRef js_proj_size_get_z(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_int_to_value(proj_setup->size.z);
	
	return(TRUE);
}

JSValueRef js_proj_size_get_weight(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_int_to_value(proj_setup->size.weight);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

void js_proj_size_set_x(JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->size.x=script_value_to_int(*vp);
	proj_setup_set_radius(proj_setup);
}

void js_proj_size_set_y(JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->size.y=script_value_to_int(*vp);
}

void js_proj_size_set_z(JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->size.z=script_value_to_int(*vp);
	proj_setup_set_radius(proj_setup);
}

void js_proj_size_set_weight(JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->size.weight=script_value_to_int(*vp);
}

