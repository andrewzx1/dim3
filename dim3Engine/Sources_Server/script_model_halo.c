/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.halo object

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
#include "lights.h"

JSValueRef js_model_halo_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_model_halo_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_model_halo_get_index(JSContextRef cx);
JSValueRef js_model_halo_get_on(JSContextRef cx);
JSValueRef js_model_halo_get_name(JSContextRef cx);
JSValueRef js_model_halo_get_minDistance(JSContextRef cx);
JSValueRef js_model_halo_get_maxDistance(JSContextRef cx);
JSValueRef js_model_halo_get_minSize(JSContextRef cx);
JSValueRef js_model_halo_get_maxSize(JSContextRef cx);
JSValueRef js_model_halo_get_minAlpha(JSContextRef cx);
JSValueRef js_model_halo_get_maxAlpha(JSContextRef cx);
JSValueRef js_model_halo_get_noClipObject(JSContextRef cx);
JSValueRef js_model_halo_get_noClipSelf(JSContextRef cx);
void js_model_halo_set_index(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_model_halo_set_on(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_model_halo_set_name(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_model_halo_set_minDistance(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_model_halo_set_maxDistance(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_model_halo_set_minSize(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_model_halo_set_maxSize(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_model_halo_set_minAlpha(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_model_halo_set_maxAlpha(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_model_halo_set_noClipObject(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_model_halo_set_noClipSelf(JSContextRef cx,JSValueRef vp,JSValueRef *exception);

extern server_type		server;
extern js_type			js;

script_js_property	model_halo_props[]={
							{"index",				js_model_halo_get_index,				js_model_halo_set_index},
							{"on",					js_model_halo_get_on,					js_model_halo_set_on},
							{"name",				js_model_halo_get_name,					js_model_halo_set_name},
							{"minDistance",			js_model_halo_get_minDistance,			js_model_halo_set_minDistance},
							{"maxDistance",			js_model_halo_get_maxDistance,			js_model_halo_set_maxDistance},
							{"minSize",				js_model_halo_get_minSize,				js_model_halo_set_minSize},
							{"maxSize",				js_model_halo_get_maxSize,				js_model_halo_set_maxSize},
							{"minAlpha",			js_model_halo_get_minAlpha,				js_model_halo_set_minAlpha},
							{"maxAlpha",			js_model_halo_get_maxAlpha,				js_model_halo_set_maxAlpha},
							{"noClipObject",		js_model_halo_get_noClipObject,			js_model_halo_set_noClipObject},
							{"noClipSelf",			js_model_halo_get_noClipSelf,			js_model_halo_set_noClipSelf},
							{0}};

JSClassRef			model_halo_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_halo_object(void)
{
	model_halo_class=script_create_class("model_halo_class",js_model_halo_get_property,js_model_halo_set_property);
}

void script_free_model_halo_object(void)
{
	script_free_class(model_halo_class);
}

JSObjectRef script_add_model_halo_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,model_halo_class,"halo",model_halo_props,NULL));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_model_halo_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,model_halo_props));
}

bool js_model_halo_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,model_halo_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_model_halo_get_index(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];
	
	return(script_int_to_value(cx,draw->script_halo_idx));
}

JSValueRef js_model_halo_get_on(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];
	
	return(script_bool_to_value(cx,halo->on));
}

JSValueRef js_model_halo_get_name(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];
	
	if (halo->idx==-1) {
		return(script_null_to_value(cx));
	}

	return(script_string_to_value(cx,server.halos[halo->idx].name));
}

JSValueRef js_model_halo_get_minDistance(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];
	
	return(script_int_to_value(cx,halo->min_dist));
}

JSValueRef js_model_halo_get_maxDistance(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];
	
	return(script_int_to_value(cx,halo->max_dist));
}

JSValueRef js_model_halo_get_minSize(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];
	
	return(script_int_to_value(cx,halo->min_size));
}

JSValueRef js_model_halo_get_maxSize(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];
	
	return(script_int_to_value(cx,halo->max_size));
}

JSValueRef js_model_halo_get_minAlpha(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];
	
    return(script_float_to_value(cx,halo->min_alpha));
}

JSValueRef js_model_halo_get_maxAlpha(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];
	
    return(script_float_to_value(cx,halo->max_alpha));
}

JSValueRef js_model_halo_get_noClipObject(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];
	
	return(script_bool_to_value(cx,halo->no_clip_object));
}

JSValueRef js_model_halo_get_noClipSelf(JSContextRef cx)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];
	
	return(script_bool_to_value(cx,halo->no_clip_self));
}

/* =======================================================

      Setters
      
======================================================= */

void js_model_halo_set_index(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];

	draw->script_halo_idx=script_value_to_int(cx,vp);
	if ((draw->script_halo_idx<0) || (draw->script_halo_idx>=max_model_halo)) draw->script_halo_idx=0;
}

void js_model_halo_set_on(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];

	halo->on=script_value_to_bool(cx,vp);
}

void js_model_halo_set_name(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	char				name[name_str_len];
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];

	script_value_to_string(cx,vp,name,name_str_len);
	halo->idx=halo_find(name);
}

void js_model_halo_set_minDistance(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];

	halo->min_dist=script_value_to_int(cx,vp);
}

void js_model_halo_set_maxDistance(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];

	halo->max_dist=script_value_to_int(cx,vp);
}

void js_model_halo_set_minSize(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];

	halo->min_size=script_value_to_int(cx,vp);
}

void js_model_halo_set_maxSize(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];

	halo->max_size=script_value_to_int(cx,vp);
}

void js_model_halo_set_minAlpha(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];

	halo->min_alpha=script_value_to_float(cx,vp);
}

void js_model_halo_set_maxAlpha(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];

	halo->max_alpha=script_value_to_float(cx,vp);
}

void js_model_halo_set_noClipObject(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];

	halo->no_clip_object=script_value_to_bool(cx,vp);
}

void js_model_halo_set_noClipSelf(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;
	model_draw_halo		*halo;

	draw=script_find_model_draw();
	halo=&draw->halos[draw->script_halo_idx];

	halo->no_clip_self=script_value_to_bool(cx,vp);
}

