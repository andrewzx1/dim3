/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.text object

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

#include "objects.h"
#include "scripts.h"
#include "interfaces.h"

extern js_type			js;

JSValueRef js_interface_text_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_interface_text_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_interface_text_show_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_text_hide_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_text_hide_all_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_text_move_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_text_move_relative_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_text_set_text_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_text_set_size_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_text_set_color_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_text_set_team_color_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_text_set_object_color_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_text_set_alpha_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_text_start_fade_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_text_set_text_and_fade_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	interface_text_functions[]={
							{"show",						js_interface_text_show_func,				kJSPropertyAttributeDontDelete},
							{"hide",						js_interface_text_hide_func,				kJSPropertyAttributeDontDelete},
							{"hideAll",						js_interface_text_hide_all_func,			kJSPropertyAttributeDontDelete},
							{"move",						js_interface_text_move_func,				kJSPropertyAttributeDontDelete},
							{"moveRelative",				js_interface_text_move_relative_func,		kJSPropertyAttributeDontDelete},
							{"setText",						js_interface_text_set_text_func,			kJSPropertyAttributeDontDelete},
							{"setSize",						js_interface_text_set_size_func,			kJSPropertyAttributeDontDelete},
							{"setColor",					js_interface_text_set_color_func,			kJSPropertyAttributeDontDelete},
							{"setTeamColor",				js_interface_text_set_team_color_func,		kJSPropertyAttributeDontDelete},
							{"setObjectColor",				js_interface_text_set_object_color_func,	kJSPropertyAttributeDontDelete},
							{"setAlpha",					js_interface_text_set_alpha_func,			kJSPropertyAttributeDontDelete},
							{"startFade",					js_interface_text_start_fade_func,			kJSPropertyAttributeDontDelete},
							{"setTextAndFade",				js_interface_text_set_text_and_fade_func,	kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			interface_text_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_interface_text_object(void)
{
	interface_text_class=script_create_class("interface_text_class",NULL,interface_text_functions);
}

void script_free_interface_text_object(void)
{
	script_free_class(interface_text_class);
}

JSObjectRef script_add_interface_text_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,interface_text_class,"text",NULL,interface_text_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_interface_text_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,NULL));
}

bool js_interface_text_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,NULL));
}

/* =======================================================

      Bitmap Functions
      
======================================================= */

JSValueRef js_interface_text_show_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	hud_text_type			*text;
	
	text=script_find_text_from_name(cx,argv[0],exception);
	if (text!=NULL) text->show=TRUE;
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_text_hide_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	hud_text_type			*text;
	
	text=script_find_text_from_name(cx,argv[0],exception);
	if (text!=NULL) text->show=FALSE;
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_text_hide_all_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	hud_texts_hide_all();
	return(script_null_to_value(cx));
}

JSValueRef js_interface_text_move_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	hud_text_type			*text;
	
	text=script_find_text_from_name(cx,argv[0],exception);
	if (text!=NULL) {
		text->x=script_value_to_int(cx,argv[1]);
		text->y=script_value_to_int(cx,argv[2]);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_interface_text_move_relative_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	hud_text_type			*text;
	
	text=script_find_text_from_name(cx,argv[0],exception);
	if (text!=NULL) {
		text->x+=script_value_to_int(cx,argv[1]);
		text->y+=script_value_to_int(cx,argv[2]);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_interface_text_set_text_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					data[max_hud_text_str_sz];
	hud_text_type			*text;
	
	text=script_find_text_from_name(cx,argv[0],exception);
	if (text!=NULL) {
		script_value_to_string(cx,argv[1],data,max_hud_text_str_sz);
		hud_text_set(text,data);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_interface_text_set_size_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	hud_text_type			*text;
	
	text=script_find_text_from_name(cx,argv[0],exception);
	if (text!=NULL) text->size=script_value_to_int(cx,argv[1]);
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_text_set_color_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	hud_text_type			*text;
	
	text=script_find_text_from_name(cx,argv[0],exception);
	if (text!=NULL) {
		text->color.r=script_value_to_float(cx,argv[1]);
		text->color.g=script_value_to_float(cx,argv[2]);
		text->color.b=script_value_to_float(cx,argv[3]);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_interface_text_set_team_color_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int						team_idx;
	hud_text_type			*text;
	
	text=script_find_text_from_name(cx,argv[0],exception);
	if (text!=NULL) {
		team_idx=script_value_to_int(cx,argv[1])-sd_team_none;
		object_team_get_tint(team_idx,&text->color);
	}

	return(script_null_to_value(cx));
}

JSValueRef js_interface_text_set_object_color_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type				*obj;
	hud_text_type			*text;
	
	text=script_find_text_from_name(cx,argv[0],exception);
	if (text==NULL) return(script_null_to_value(cx));

	obj=script_find_obj_from_uid_arg(cx,argv[1],exception);
	if (obj==NULL) return(script_null_to_value(cx));
	
	object_get_tint(obj,&text->color);
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_text_set_alpha_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	hud_text_type			*text;
	
	text=script_find_text_from_name(cx,argv[0],exception);
	if (text!=NULL) text->alpha=script_value_to_float(cx,argv[1]);
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_text_start_fade_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	hud_text_type			*text;
	
	text=script_find_text_from_name(cx,argv[0],exception);
	if (text!=NULL) {
		text->show=TRUE;
		text->fade.on=TRUE;
		text->fade.start_tick=js.time.current_tick;
	}

	return(script_null_to_value(cx));
}

JSValueRef js_interface_text_set_text_and_fade_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char					data[max_hud_text_str_sz];
	hud_text_type			*text;
	
	text=script_find_text_from_name(cx,argv[0],exception);
	if (text!=NULL) {
		script_value_to_string(cx,argv[1],data,max_hud_text_str_sz);
		hud_text_set(text,data);
		
		text->show=TRUE;
		text->fade.on=TRUE;
		text->fade.start_tick=js.time.current_tick;
	}

	return(script_null_to_value(cx));
}
