/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.text object

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
#include "interfaces.h"

extern js_type			js;

JSBool js_interface_text_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_interface_text_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_interface_text_show_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_text_hide_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_text_hide_all_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_text_move_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_text_move_relative_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_text_set_text_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_text_set_size_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_text_set_color_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_text_set_team_color_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_text_set_object_color_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_text_set_alpha_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_text_start_fade_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_text_set_text_and_fade_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);

script_js_function	interface_text_functions[]={
							{"show",						js_interface_text_show_func,				1},
							{"hide",						js_interface_text_hide_func,				1},
							{"hideAll",						js_interface_text_hide_all_func,			0},
							{"move",						js_interface_text_move_func,				3},
							{"moveRelative",				js_interface_text_move_relative_func,		3},
							{"setText",						js_interface_text_set_text_func,			2},
							{"setSize",						js_interface_text_set_size_func,			2},
							{"setColor",					js_interface_text_set_color_func,			4},
							{"setTeamColor",				js_interface_text_set_team_color_func,		2},
							{"setObjectColor",				js_interface_text_set_object_color_func,	2},
							{"setAlpha",					js_interface_text_set_alpha_func,			2},
							{"startFade",					js_interface_text_start_fade_func,			1},
							{"setTextAndFade",				js_interface_text_set_text_and_fade_func,	2},
							{0}};

JSClassRef			interface_text_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_interface_text_object(void)
{
	interface_text_class=script_create_class("interface_text_class",js_interface_text_get_property,js_interface_text_set_property);
}

void script_free_interface_text_object(void)
{
	script_free_class(interface_text_class);
}

JSObject* script_add_interface_text_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,interface_text_class,"text",NULL,interface_text_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_interface_text_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,NULL));
}

JSBool js_interface_text_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,NULL));
}

/* =======================================================

      Bitmap Functions
      
======================================================= */

JSBool js_interface_text_show_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	hud_text_type			*text;
	
	text=script_find_text_from_name(argv[0]);
	if (text==NULL) return(FALSE);
	
	text->show=TRUE;
	
	return(TRUE);
}

JSBool js_interface_text_hide_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	hud_text_type			*text;
	
	text=script_find_text_from_name(argv[0]);
	if (text==NULL) return(FALSE);
	
	text->show=FALSE;
	
	return(TRUE);
}

JSBool js_interface_text_hide_all_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	hud_texts_hide_all();
	return(TRUE);
}

JSBool js_interface_text_move_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	hud_text_type			*text;
	
	text=script_find_text_from_name(argv[0]);
	if (text==NULL) return(FALSE);
	
	text->x=script_value_to_int(argv[1]);
	text->y=script_value_to_int(argv[2]);
	
	return(TRUE);
}

JSBool js_interface_text_move_relative_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	hud_text_type			*text;
	
	text=script_find_text_from_name(argv[0]);
	if (text==NULL) return(FALSE);
	
	text->x+=script_value_to_int(argv[1]);
	text->y+=script_value_to_int(argv[2]);
	
	return(TRUE);
}

JSBool js_interface_text_set_text_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	char					data[max_hud_text_str_sz];
	hud_text_type			*text;
	
	text=script_find_text_from_name(argv[0]);
	if (text==NULL) return(FALSE);

	script_value_to_string(argv[1],data,max_hud_text_str_sz);
	hud_text_set(text,data);
	
	return(TRUE);
}

JSBool js_interface_text_set_size_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	hud_text_type			*text;
	
	text=script_find_text_from_name(argv[0]);
	if (text==NULL) return(FALSE);
	
	text->size=script_value_to_int(argv[1]);
	
	return(TRUE);
}

JSBool js_interface_text_set_color_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	hud_text_type			*text;
	
	text=script_find_text_from_name(argv[0]);
	if (text==NULL) return(FALSE);
	
	text->color.r=script_value_to_float(argv[1]);
	text->color.g=script_value_to_float(argv[2]);
	text->color.b=script_value_to_float(argv[3]);
	
	return(TRUE);
}

JSBool js_interface_text_set_team_color_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int						team_idx;
	hud_text_type			*text;
	
	text=script_find_text_from_name(argv[0]);
	if (text==NULL) return(FALSE);

	team_idx=script_value_to_int(argv[1])-sd_team_none;
	
	object_team_get_tint(team_idx,&text->color);
	
	return(TRUE);
}

JSBool js_interface_text_set_object_color_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	obj_type				*obj;
	hud_text_type			*text;
	
	text=script_find_text_from_name(argv[0]);
	if (text==NULL) return(FALSE);

	obj=script_find_obj_from_uid_arg(argv[1]);
	if (obj==NULL) return(TRUE);
	
	object_get_tint(obj,&text->color);
	
	return(TRUE);
}

JSBool js_interface_text_set_alpha_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	hud_text_type			*text;
	
	text=script_find_text_from_name(argv[0]);
	if (text==NULL) return(FALSE);
	
	text->alpha=script_value_to_float(argv[1]);
	
	return(TRUE);
}

JSBool js_interface_text_start_fade_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	hud_text_type			*text;
	
	text=script_find_text_from_name(argv[0]);
	if (text==NULL) return(FALSE);
	
	text->show=TRUE;
	text->fade.on=TRUE;
	text->fade.start_tick=js.time.current_tick;
	
	return(TRUE);
}

JSBool js_interface_text_set_text_and_fade_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	char					data[max_hud_text_str_sz];
	hud_text_type			*text;
	
	text=script_find_text_from_name(argv[0]);
	if (text==NULL) return(FALSE);

	script_value_to_string(argv[1],data,max_hud_text_str_sz);
	hud_text_set(text,data);
	
	text->show=TRUE;
	text->fade.on=TRUE;
	text->fade.start_tick=js.time.current_tick;
	
	return(TRUE);
}
