/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.animation object

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
#include "models.h"

JSValueRef js_model_animation_get_index(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_animation_get_currentAnimationName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
JSValueRef js_model_animation_get_playing(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_model_animation_set_index(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_model_animation_start_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_cancel_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_change_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_interrupt_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_start_then_change_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_animation_fade_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

extern js_type			js;

JSStaticValue 		model_animation_props[]={
							{"index",					js_model_animation_get_index,					js_model_animation_set_index,		kJSPropertyAttributeDontDelete},
							{"currentAnimationName",	js_model_animation_get_currentAnimationName,	NULL,								kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"playing",					js_model_animation_get_playing,					NULL,								kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	model_animation_functions[]={
							{"start",					js_model_animation_start_func,					kJSPropertyAttributeDontDelete},
							{"stop",					js_model_animation_stop_func,					kJSPropertyAttributeDontDelete},
							{"cancel",					js_model_animation_cancel_func,					kJSPropertyAttributeDontDelete},
							{"change",					js_model_animation_change_func,					kJSPropertyAttributeDontDelete},
							{"interrupt",				js_model_animation_interrupt_func,				kJSPropertyAttributeDontDelete},
							{"startThenChange",			js_model_animation_start_then_change_func,		kJSPropertyAttributeDontDelete},
							{"fade",					js_model_animation_fade_func,					kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			model_animation_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_animation_object(void)
{
	model_animation_class=script_create_class("model_animation_class",model_animation_props,model_animation_functions);
}

void script_free_model_animation_object(void)
{
	script_free_class(model_animation_class);
}

JSObjectRef script_add_model_animation_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,model_animation_class,"animation"));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_model_animation_get_index(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw			*draw;

	draw=script_find_model_draw();
	return(script_int_to_value(cx,draw->script_animation_idx));
}

JSValueRef js_model_animation_get_currentAnimationName(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	char				ani_name[64];
	model_draw			*draw;

	draw=script_find_model_draw();

	model_get_current_animation_name(draw,ani_name);
	return(script_string_to_value(cx,ani_name));
}

JSValueRef js_model_animation_get_playing(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	model_draw				*draw;
	model_draw_animation	*draw_animation;

	draw=script_find_model_draw();
	draw_animation=&draw->animations[draw->script_animation_idx];
	return(script_bool_to_value(cx,draw_animation->mode==am_playing));
}

/* =======================================================

      Setters
      
======================================================= */

bool js_model_animation_set_index(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	model_draw			*draw;

	draw=script_find_model_draw();

	draw->script_animation_idx=script_value_to_int(cx,vp);
	if ((draw->script_animation_idx<0) || (draw->script_animation_idx>=max_model_blend_animation)) draw->script_animation_idx=0;

	return(TRUE);
}

/* =======================================================

      Animation Exceptions
      
======================================================= */

JSValueRef js_model_animation_name_exception(JSContextRef cx,char *name)
{
	char			err_str[256];

	sprintf(err_str,"Named animation does not exist: %s",name);
	return(script_create_exception(cx,err_str));
}

/* =======================================================

      Model Animation Functions
      
======================================================= */

JSValueRef js_model_animation_start_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw();

	script_value_to_string(cx,argv[0],name,name_str_len);
	
	if (!model_start_animation(draw,name)) {
		*exception=js_model_animation_name_exception(cx,name);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_model_animation_stop_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw();
	model_stop_animation(draw);

	return(script_null_to_value(cx));
}

JSValueRef js_model_animation_cancel_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw();

	script_value_to_string(cx,argv[0],name,name_str_len);
	if (!model_cancel_animation(draw,name)) {
		*exception=js_model_animation_name_exception(cx,name);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_model_animation_change_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw();

	script_value_to_string(cx,argv[0],name,name_str_len);
	if (!model_change_animation(draw,name)) {
		*exception=js_model_animation_name_exception(cx,name);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_model_animation_interrupt_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw();

	script_value_to_string(cx,argv[0],name,name_str_len);
	if (!model_interrupt_animation(draw,name)) {
		*exception=js_model_animation_name_exception(cx,name);
	}
	
	return(script_null_to_value(cx));
}

JSValueRef js_model_animation_start_then_change_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw();

	script_value_to_string(cx,argv[0],name,name_str_len);
	if (!model_start_animation(draw,name)) {
		*exception=js_model_animation_name_exception(cx,name);
		return(script_null_to_value(cx));
	}

	script_value_to_string(cx,argv[1],name,name_str_len);
	if (!model_change_animation(draw,name)) {
		*exception=js_model_animation_name_exception(cx,name);
		return(script_null_to_value(cx));
	}

	return(script_null_to_value(cx));
}

/* =======================================================

      Fades
      
======================================================= */

JSValueRef js_model_animation_fade_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	model_draw		*draw;
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	draw=script_find_model_draw();
	model_fade_start(draw,script_value_to_int(cx,argv[1]),script_value_to_float(cx,argv[0]));
	
	return(script_null_to_value(cx));
}
