/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: multiplayer.bot object

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

#include "objects.h"
#include "effects.h"
#include "scripts.h"
#include "physics.h"

extern server_type		server;
extern js_type			js;
extern setup_type		setup;

JSValueRef js_multiplayer_bot_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_multiplayer_bot_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_multiplayer_bot_get_skill(JSContextRef cx);
JSValueRef js_multiplayer_bot_get_from_min_max_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_property	multiplayer_bot_props[]={
							{"skill",				js_multiplayer_bot_get_skill,				NULL},
							{0}};
							
script_js_function	multiplayer_bot_functions[]={
							{"getFromMinMax",		js_multiplayer_bot_get_from_min_max_func,	2},
							{0}};

JSClassRef			multiplayer_bot_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_multiplayer_bot_object(void)
{
	multiplayer_bot_class=script_create_class("multiplayer_bot_class",js_multiplayer_bot_get_property,js_multiplayer_bot_set_property);
}

void script_free_multiplayer_bot_object(void)
{
	script_free_class(multiplayer_bot_class);
}

JSObjectRef script_add_multiplayer_bot_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,multiplayer_bot_class,"bot",multiplayer_bot_props,multiplayer_bot_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_multiplayer_bot_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,multiplayer_bot_props));
}

bool js_multiplayer_bot_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,multiplayer_bot_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_multiplayer_bot_get_skill(JSContextRef cx)
{
	return(script_int_to_value(cx,server.skill));
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_multiplayer_bot_get_from_min_max_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			skill,min,max;

	min=script_value_to_float(cx,argv[0]);
	max=script_value_to_float(cx,argv[1]);
	
	skill=(float)setup.network.bot.skill;

	return(script_float_to_value(cx,min+(((max-min)*skill)/4.0f)));
}

