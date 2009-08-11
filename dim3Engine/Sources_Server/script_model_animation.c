/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.animation object

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
#include "models.h"

JSValueRef js_model_animation_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_model_animation_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_model_animation_get_index(void);
JSValueRef js_model_animation_get_currentAnimationName(void);
JSValueRef js_model_animation_get_playing(void);
bool js_model_animation_set_index(JSValueRef vp);
JSValueRef js_model_animation_start_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_cancel_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_change_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_interrupt_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_start_then_change_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_fade_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

extern js_type			js;

script_js_property	model_animation_props[]={
							{"index",					js_model_animation_get_index,					js_model_animation_set_index},
							{"currentAnimationName",	js_model_animation_get_currentAnimationName,	NULL},
							{"playing",					js_model_animation_get_playing,					NULL},
							{0}};

script_js_function	model_animation_functions[]={
							{"start",					js_model_animation_start_func,					1},
							{"stop",					js_model_animation_stop_func,					0},
							{"cancel",					js_model_animation_cancel_func,					1},
							{"change",					js_model_animation_change_func,					1},
							{"interrupt",				js_model_animation_interrupt_func,				1},
							{"startThenChange",			js_model_animation_start_then_change_func,		2},
							{"fade",					js_model_animation_fade_func,					2},
							{0}};

JSClassRef			model_animation_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_animation_object(void)
{
	model_animation_class=script_create_class("model_animation_class",js_model_animation_get_property,js_model_animation_set_property);
}

void script_free_model_animation_object(void)
{
	script_free_class(model_animation_class);
}

JSObjectRef script_add_model_animation_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,model_animation_class,"animation",model_animation_props,model_animation_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_model_animation_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,model_animation_props));
}

bool js_model_animation_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,model_animation_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_model_animation_get_index(void)
{
	model_draw			*draw;

	draw=script_find_model_draw();
	*vp=script_int_to_value(draw->script_animation_idx);

	return(TRUE);
}

JSValueRef js_model_animation_get_currentAnimationName(void)
{
	char				name[64];
	model_draw			*draw;

	draw=script_find_model_draw();

	model_get_current_animation_name(draw,name);
	*vp=script_string_to_value(name);

	return(TRUE);
}

JSValueRef js_model_animation_get_playing(void)
{
	model_draw				*draw;
	model_draw_animation	*draw_animation;

	draw=script_find_model_draw();
	draw_animation=&draw->animations[draw->script_animation_idx];
	*vp=script_bool_to_value(draw_animation->mode==am_playing);

	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_model_animation_set_index(JSValueRef vp)
{
	model_draw			*draw;

	draw=script_find_model_draw();

	draw->script_animation_idx=script_value_to_int(*vp);
	if ((draw->script_animation_idx<0) || (draw->script_animation_idx>=max_model_blend_animation)) draw->script_animation_idx=0;

	return(TRUE);
}

/* =======================================================

      Animation Exceptions
      
======================================================= */

JSValueRef js_model_animation_name_exception(char *name)
{
	char			err_str[256];

	sprintf(err_str,"Named animation does not exist: %s",name);
	return(script_create_exception(err_str));
}

/* =======================================================

      Model Animation Functions
      
======================================================= */

JSValueRef js_model_animation_start_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len],err_str[256];
	model_draw		*draw;
	
	draw=script_find_model_draw();

	script_value_to_string(argv[0],name,name_str_len);
	
	if (!model_start_animation(draw,name)) {
		*exception=js_model_animation_name_exception(name);
		return(FALSE);
	}
	
	return(TRUE);
}

JSValueRef js_model_animation_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	model_draw		*draw;
	
	draw=script_find_model_draw();
	
	model_stop_animation(draw);
	return(TRUE);
}

JSValueRef js_model_animation_cancel_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	draw=script_find_model_draw();

	script_value_to_string(argv[0],name,name_str_len);
	if (!model_cancel_animation(draw,name)) {
		*exception=js_model_animation_name_exception(name);
		return(FALSE);
	}
	
	return(TRUE);
}

JSValueRef js_model_animation_change_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	draw=script_find_model_draw();

	script_value_to_string(argv[0],name,name_str_len);
	if (!model_change_animation(draw,name)) {
		*exception=js_model_animation_name_exception(name);
		return(FALSE);
	}
	
	return(TRUE);
}

JSValueRef js_model_animation_interrupt_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	draw=script_find_model_draw();

	script_value_to_string(argv[0],name,name_str_len);
	if (!model_interrupt_animation(draw,name)) {
		*exception=js_model_animation_name_exception(name);
		return(FALSE);
	}
	
	return(TRUE);
}

JSValueRef js_model_animation_start_then_change_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	draw=script_find_model_draw();

	script_value_to_string(argv[0],name,name_str_len);
	if (!model_start_animation(draw,name)) {
		*exception=js_model_animation_name_exception(name);
		return(FALSE);
	}

	script_value_to_string(argv[1],name,name_str_len);
	if (!model_change_animation(draw,name)) {
		*exception=js_model_animation_name_exception(name);
		return(FALSE);
	}

	return(TRUE);
}

/* =======================================================

      Fades
      
======================================================= */

JSValueRef js_model_animation_fade_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	model_draw		*draw;
	
	draw=script_find_model_draw();
	model_fade_start(draw,script_value_to_int(argv[1]),script_value_to_float(argv[0]));
	
	return(TRUE);
}

