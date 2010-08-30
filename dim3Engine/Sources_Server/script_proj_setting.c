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

extern server_type		server;
extern js_type			js;

JSValueRef js_proj_setting_get_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_setting_get_hitscan(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_setting_get_resetAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_setting_get_parentObjectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_proj_setting_get_parentTeam(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_proj_setting_set_hitscan(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
bool js_proj_setting_set_resetAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);

JSStaticValue 		proj_setting_props[]={
							{"name",				js_proj_setting_get_name,				NULL,								kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"hitScan",				js_proj_setting_get_hitscan,			js_proj_setting_set_hitscan,		kJSPropertyAttributeDontDelete},
							{"resetAngle",			js_proj_setting_get_resetAngle,			js_proj_setting_set_resetAngle,		kJSPropertyAttributeDontDelete},
							{"parentObjectId",		js_proj_setting_get_parentObjectId,		NULL,								kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"parentTeam",			js_proj_setting_get_parentTeam,			NULL,								kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSClassRef			proj_setting_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_proj_setting_object(void)
{
	proj_setting_class=script_create_class("proj_setting_class",proj_setting_props,NULL);
}

void script_free_proj_setting_object(void)
{
	script_free_class(proj_setting_class);
}

JSObjectRef script_add_proj_setting_object(JSContextRef cx,JSObjectRef parent_obj,int script_idx)
{
	return(script_create_child_object(cx,parent_obj,proj_setting_class,"setting",script_idx));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_proj_setting_get_name(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(script_null_to_value(cx));
	
	return(script_string_to_value(cx,proj_setup->name));
}

JSValueRef js_proj_setting_get_hitscan(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(script_bool_to_value(cx,FALSE));

	return(script_bool_to_value(cx,proj_setup->hitscan.on));
}

JSValueRef js_proj_setting_get_resetAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(script_bool_to_value(cx,FALSE));
	
	return(script_bool_to_value(cx,proj_setup->reset_angle));
}

JSValueRef js_proj_setting_get_parentObjectId(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(script_int_to_value(cx,-1));
	
	return(script_int_to_value(cx,proj->obj_idx));
}

JSValueRef js_proj_setting_get_parentTeam(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	proj_type			*proj;
	obj_type			*obj;

	proj=proj_get_attach();
	if (proj==NULL) return(script_int_to_value(cx,-1));

	obj=server.obj_list.objs[proj->obj_idx];
	if (obj==NULL) return(script_int_to_value(cx,-1));
	
	return(script_int_to_value(cx,obj->team_idx));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_proj_setting_set_hitscan(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup!=NULL) proj_setup->hitscan.on=script_value_to_bool(cx,vp);

	return(TRUE);
}

bool js_proj_setting_set_resetAngle(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup!=NULL) proj_setup->reset_angle=script_value_to_bool(cx,vp);

	return(TRUE);
}
