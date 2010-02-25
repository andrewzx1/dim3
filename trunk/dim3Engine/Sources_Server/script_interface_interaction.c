/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.interaction object

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

#include "scripts.h"
#include "interfaces.h"

extern js_type			js;

JSValueRef js_interface_interaction_start_story_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_interaction_start_title_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_interaction_start_chooser_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_interaction_start_movie_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_interaction_start_save_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_interaction_start_load_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_interaction_start_setup_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_interaction_start_menu_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_interaction_quit_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	interface_interaction_functions[]={
							{"startStory",			js_interface_interaction_start_story_func,				kJSPropertyAttributeDontDelete},
							{"startTitle",			js_interface_interaction_start_title_func,				kJSPropertyAttributeDontDelete},
							{"startMovie",			js_interface_interaction_start_movie_func,				kJSPropertyAttributeDontDelete},
							{"startChooser",		js_interface_interaction_start_chooser_func,			kJSPropertyAttributeDontDelete},
							{"startSave",			js_interface_interaction_start_save_func,				kJSPropertyAttributeDontDelete},
							{"startLoad",			js_interface_interaction_start_load_func,				kJSPropertyAttributeDontDelete},
							{"startSetup",			js_interface_interaction_start_setup_func,				kJSPropertyAttributeDontDelete},
							{"startMenu",			js_interface_interaction_start_menu_func,				kJSPropertyAttributeDontDelete},
							{"quit",				js_interface_interaction_quit_func,						kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			interface_interaction_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_interface_interaction_object(void)
{
	interface_interaction_class=script_create_class("interface_interaction_class",NULL,interface_interaction_functions);
}

void script_free_interface_interaction_object(void)
{
	script_free_class(interface_interaction_class);
}

JSObjectRef script_add_interface_interaction_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,interface_interaction_class,"interaction"));
}

/* =======================================================

      Start Functions
      
======================================================= */

JSValueRef js_interface_interaction_start_story_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));

	script_value_to_string(cx,argv[0],name,name_str_len);
	story_trigger_set(name,script_value_to_int(cx,argv[1]));
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_interaction_start_title_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len],sound_name[name_str_len];

	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));

	script_value_to_string(cx,argv[0],name,name_str_len);
	script_value_to_string(cx,argv[1],sound_name,name_str_len);
	
	title_trigger_set("Titles",name,sound_name,script_value_to_int(cx,argv[2]));
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_interaction_start_movie_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));

	script_value_to_string(cx,argv[0],name,name_str_len);
	movie_trigger_set(name,script_value_to_int(cx,argv[1]));
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_interaction_start_chooser_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,cnt;
	char			name[name_str_len],
					sub_txt[max_chooser_sub_txt][max_chooser_text_data_sz];

	if (!script_check_param_at_least_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

		// get name

	script_value_to_string(cx,argv[0],name,name_str_len);

		// get any substitution text

	for (n=0;n!=max_chooser_sub_txt;n++) {
		sub_txt[n][0]=0x0;
	}

	cnt=argc-1;
	if (cnt<0) cnt=0;
	if (cnt>max_chooser_sub_txt) cnt=max_chooser_sub_txt;

	for (n=0;n!=cnt;n++) {
		script_value_to_string(cx,argv[n+1],sub_txt[n],max_chooser_text_data_sz);
	}

		// start chooser

	chooser_trigger_set(name,(char*)sub_txt);
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_interaction_start_save_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));

	file_trigger_set(TRUE);
	return(script_null_to_value(cx));
}

JSValueRef js_interface_interaction_start_load_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));

	file_trigger_set(FALSE);
	return(script_null_to_value(cx));
}

JSValueRef js_interface_interaction_start_setup_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));

	setup_game_trigger_set();
	return(script_null_to_value(cx));
}

JSValueRef js_interface_interaction_start_menu_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));

	// supergumba -- depreciated, delete later
	return(script_null_to_value(cx));
}

JSValueRef js_interface_interaction_quit_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));

	interface_quit_trigger_set();
	return(script_null_to_value(cx));
}
