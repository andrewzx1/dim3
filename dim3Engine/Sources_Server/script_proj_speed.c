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

JSBool js_proj_speed_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_proj_speed_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
bool js_proj_speed_get_speed(JSValueRef *vp);
bool js_proj_speed_get_deceleration(JSValueRef *vp);
bool js_proj_speed_get_decelerationWait(JSValueRef *vp);
bool js_proj_speed_get_decelerationMinSpeed(JSValueRef *vp);
bool js_proj_speed_get_maxHitscanDistance(JSValueRef *vp);
bool js_proj_speed_get_inheritMotionFactor(JSValueRef *vp);
bool js_proj_speed_set_speed(JSValueRef *vp);
bool js_proj_speed_set_deceleration(JSValueRef *vp);
bool js_proj_speed_set_decelerationWait(JSValueRef *vp);
bool js_proj_speed_set_decelerationMinSpeed(JSValueRef *vp);
bool js_proj_speed_set_maxHitscanDistance(JSValueRef *vp);
bool js_proj_speed_set_inheritMotionFactor(JSValueRef *vp);

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

JSObject* script_add_proj_speed_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,proj_speed_class,"speed",proj_speed_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_proj_speed_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,proj_speed_props));
}

JSBool js_proj_speed_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,proj_speed_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_proj_speed_get_speed(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
    *vp=script_float_to_value(proj_setup->speed);
	
	return(TRUE);
}

bool js_proj_speed_get_deceleration(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
    *vp=script_float_to_value(proj_setup->decel_speed);
	
	return(TRUE);
}

bool js_proj_speed_get_decelerationWait(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_int_to_value(proj_setup->decel_grace);
	
	return(TRUE);
}

bool js_proj_speed_get_decelerationMinSpeed(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
    *vp=script_float_to_value(proj_setup->decel_min_speed);
	
	return(TRUE);
}

bool js_proj_speed_get_maxHitscanDistance(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_int_to_value(proj_setup->hitscan.max_dist);
	
	return(TRUE);
}

bool js_proj_speed_get_inheritMotionFactor(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_float_to_value(proj_setup->inherit_motion_factor);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_proj_speed_set_speed(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
    proj_setup->speed=script_value_to_float(*vp);
	
	return(TRUE);
}

bool js_proj_speed_set_deceleration(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->decel_speed=script_value_to_float(*vp);
	
	return(TRUE);
}

bool js_proj_speed_set_decelerationWait(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->decel_grace=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_proj_speed_set_decelerationMinSpeed(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
    proj_setup->decel_min_speed=script_value_to_float(*vp);
	
	return(TRUE);
}

bool js_proj_speed_set_maxHitscanDistance(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->hitscan.max_dist=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_proj_speed_set_inheritMotionFactor(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->inherit_motion_factor=script_value_to_float(*vp);
	
	return(TRUE);
}



