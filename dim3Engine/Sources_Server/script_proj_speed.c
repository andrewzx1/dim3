/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.speed object

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

JSValueRef js_proj_speed_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_proj_speed_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_proj_speed_get_speed(void);
JSValueRef js_proj_speed_get_deceleration(void);
JSValueRef js_proj_speed_get_decelerationWait(void);
JSValueRef js_proj_speed_get_decelerationMinSpeed(void);
JSValueRef js_proj_speed_get_maxHitscanDistance(void);
JSValueRef js_proj_speed_get_inheritMotionFactor(void);
void js_proj_speed_set_speed(JSValueRef vp,JSValueRef *exception);
void js_proj_speed_set_deceleration(JSValueRef vp,JSValueRef *exception);
void js_proj_speed_set_decelerationWait(JSValueRef vp,JSValueRef *exception);
void js_proj_speed_set_decelerationMinSpeed(JSValueRef vp,JSValueRef *exception);
void js_proj_speed_set_maxHitscanDistance(JSValueRef vp,JSValueRef *exception);
void js_proj_speed_set_inheritMotionFactor(JSValueRef vp,JSValueRef *exception);

script_js_property	proj_speed_props[]={
							{"speed",					js_proj_speed_get_speed,					js_proj_speed_set_speed},
							{"deceleration",			js_proj_speed_get_deceleration,				js_proj_speed_set_deceleration},
							{"decelerationWait",		js_proj_speed_get_decelerationWait,			js_proj_speed_set_decelerationWait},
							{"decelerationMinSpeed",	js_proj_speed_get_decelerationMinSpeed,		js_proj_speed_set_decelerationMinSpeed},
							{"maxHitScanDistance",		js_proj_speed_get_maxHitscanDistance,		js_proj_speed_set_maxHitscanDistance},
							{"inheritMotionFactor",		js_proj_speed_get_inheritMotionFactor,		js_proj_speed_set_inheritMotionFactor},
							{0}};

JSClassRef			proj_speed_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_proj_speed_object(void)
{
	proj_speed_class=script_create_class("proj_speed_class",js_proj_speed_get_property,js_proj_speed_set_property);
}

void script_free_proj_speed_object(void)
{
	script_free_class(proj_speed_class);
}

JSObjectRef script_add_proj_speed_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,proj_speed_class,"speed",proj_speed_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_proj_speed_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,proj_speed_props));
}

bool js_proj_speed_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,proj_speed_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_proj_speed_get_speed(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(script_null_to_value());
	
    return(script_float_to_value(proj_setup->speed));
}

JSValueRef js_proj_speed_get_deceleration(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(script_null_to_value());
	
    return(script_float_to_value(proj_setup->decel_speed));
}

JSValueRef js_proj_speed_get_decelerationWait(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(script_null_to_value());
	
	return(script_int_to_value(proj_setup->decel_grace));
}

JSValueRef js_proj_speed_get_decelerationMinSpeed(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(script_null_to_value());
	
    return(script_float_to_value(proj_setup->decel_min_speed));
}

JSValueRef js_proj_speed_get_maxHitscanDistance(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(script_null_to_value());
	
	return(script_int_to_value(proj_setup->hitscan.max_dist));
}

JSValueRef js_proj_speed_get_inheritMotionFactor(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(script_null_to_value());
	
	return(script_float_to_value(proj_setup->inherit_motion_factor));
}

/* =======================================================

      Setters
      
======================================================= */

void js_proj_speed_set_speed(JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup!=NULL) proj_setup->speed=script_value_to_float(vp);
}

void js_proj_speed_set_deceleration(JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup!=NULL) proj_setup->decel_speed=script_value_to_float(vp);
}

void js_proj_speed_set_decelerationWait(JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup!=NULL) proj_setup->decel_grace=script_value_to_int(vp);
}

void js_proj_speed_set_decelerationMinSpeed(JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup!=NULL) proj_setup->decel_min_speed=script_value_to_float(vp);
}

void js_proj_speed_set_maxHitscanDistance(JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup!=NULL) proj_setup->hitscan.max_dist=script_value_to_int(vp);
}

void js_proj_speed_set_inheritMotionFactor(JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup!=NULL) proj_setup->inherit_motion_factor=script_value_to_float(vp);
}



