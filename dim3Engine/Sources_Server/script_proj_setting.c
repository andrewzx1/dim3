/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.setting object

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

#include "objects.h"
#include "scripts.h"
#include "projectiles.h"

extern js_type			js;

JSValueRef js_proj_setting_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_proj_setting_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_proj_setting_get_name(void);
JSValueRef js_proj_setting_get_hitscan(void);
JSValueRef js_proj_setting_get_resetAngle(void);
JSValueRef js_proj_setting_get_parentObjectId(void);
JSValueRef js_proj_setting_get_parentTeam(void);
bool js_proj_setting_set_hitscan(JSValueRef vp);
bool js_proj_setting_set_resetAngle(JSValueRef vp);

script_js_property	proj_setting_props[]={
							{"name",				js_proj_setting_get_name,				NULL},
							{"hitScan",				js_proj_setting_get_hitscan,			js_proj_setting_set_hitscan},
							{"resetAngle",			js_proj_setting_get_resetAngle,			js_proj_setting_set_resetAngle},
							{"parentObjectId",		js_proj_setting_get_parentObjectId,		NULL},
							{"parentTeam",			js_proj_setting_get_parentTeam,			NULL},
							{0}};

JSClassRef			proj_setting_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_proj_setting_object(void)
{
	proj_setting_class=script_create_class("proj_setting_class",js_proj_setting_get_property,js_proj_setting_set_property);
}

void script_free_proj_setting_object(void)
{
	script_free_class(proj_setting_class);
}

JSObjectRef script_add_proj_setting_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,proj_setting_class,"setting",proj_setting_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_proj_setting_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,proj_setting_props));
}

bool js_proj_setting_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,proj_setting_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_proj_setting_get_name(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) {
		*vp=script_null_to_value();
	}
	else {
		*vp=script_string_to_value(proj_setup->name);
	}
	
	return(TRUE);
}

JSValueRef js_proj_setting_get_hitscan(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) {
		*vp=script_bool_to_value(FALSE);
	}
	else {
		*vp=script_bool_to_value(proj_setup->hitscan.on);
	}
	
	return(TRUE);
}

JSValueRef js_proj_setting_get_resetAngle(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) {
		*vp=script_bool_to_value(FALSE);
	}
	else {
		*vp=script_bool_to_value(proj_setup->reset_angle);
	}
	
	return(TRUE);
}

JSValueRef js_proj_setting_get_parentObjectId(void)
{
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) {
		*vp=script_int_to_value(-1);
	}
	else {
		*vp=script_int_to_value(proj->obj_uid);
	}
	
	return(TRUE);
}

JSValueRef js_proj_setting_get_parentTeam(void)
{
	proj_type			*proj;
	obj_type			*obj;

	proj=proj_get_attach();
	if (proj==NULL) {
		*vp=script_int_to_value(-1);
		return(TRUE);
	}

	obj=object_find_uid(proj->obj_uid);
	if (obj==NULL) {
		*vp=script_int_to_value(-1);
		return(TRUE);
	}
	
	*vp=script_int_to_value(obj->team_idx);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_proj_setting_set_hitscan(JSValueRef vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->hitscan.on=script_value_to_bool(*vp);
	
	return(TRUE);
}

bool js_proj_setting_set_resetAngle(JSValueRef vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);

	proj_setup->reset_angle=script_value_to_bool(*vp);
	
	return(TRUE);
}
