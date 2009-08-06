/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.interaction object

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
#include "interfaces.h"

extern js_type			js;

JSBool js_interface_interaction_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_interface_interaction_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_get_interface_interaction_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_set_interface_interaction_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_interface_interaction_start_story_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_interaction_start_title_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_interaction_start_chooser_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_interaction_start_movie_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_interaction_start_save_load_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_interaction_start_setup_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_interaction_start_menu_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_interface_interaction_quit_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);

script_js_function	interface_interaction_functions[]={
							{"startStory",			js_interface_interaction_start_story_func,				2},
							{"startTitle",			js_interface_interaction_start_title_func,				3},
							{"startMovie",			js_interface_interaction_start_movie_func,				2},
							{"startChooser",		js_interface_interaction_start_chooser_func,			1},
							{"startSaveLoad",		js_interface_interaction_start_save_load_func,			0},
							{"startSetup",			js_interface_interaction_start_setup_func,				0},
							{"startMenu",			js_interface_interaction_start_menu_func,				0},
							{"quit",				js_interface_interaction_quit_func,						0},
							{0}};

JSClassRef			interface_interaction_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_interface_interaction_object(void)
{
	interface_interaction_class=script_create_class("interface_interaction_class",js_interface_interaction_get_property,js_interface_interaction_set_property);
}

void script_free_interface_interaction_object(void)
{
	script_free_class(interface_interaction_class);
}

JSObject* script_add_interface_interaction_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,interface_interaction_class,"interaction",NULL,interface_interaction_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_interface_interaction_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,NULL));
}

JSBool js_interface_interaction_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,NULL));
}

/* =======================================================

      Start Functions
      
======================================================= */

JSBool js_interface_interaction_start_story_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				event_id;
	char			name[name_str_len];

	if (argc<2) {
		event_id=-1;
	}
	else {
		event_id=script_value_to_int(argv[1]);
	}

	script_value_to_string(argv[0],name,name_str_len);
	story_trigger_set(name,script_value_to_int(argv[1]));
	
	return(TRUE);
}

JSBool js_interface_interaction_start_title_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				event_id;
	char			name[name_str_len],sound_name[name_str_len];

	if (argc<3) {
		event_id=-1;
	}
	else {
		event_id=script_value_to_int(argv[2]);
	}

	script_value_to_string(argv[0],name,name_str_len);
	script_value_to_string(argv[1],sound_name,name_str_len);
	
	title_trigger_set("Titles",name,sound_name,event_id);
	
	return(TRUE);
}

JSBool js_interface_interaction_start_movie_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				event_id;
	char			name[name_str_len];

	if (argc<2) {
		event_id=-1;
	}
	else {
		event_id=script_value_to_int(argv[1]);
	}

	script_value_to_string(argv[0],name,name_str_len);
	movie_trigger_set(name,event_id);
	
	return(TRUE);
}

JSBool js_interface_interaction_start_chooser_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				n,cnt;
	char			name[name_str_len],
					sub_txt[max_chooser_sub_txt][max_chooser_text_data_sz];

		// get name

	script_value_to_string(argv[0],name,name_str_len);

		// get any substitution text

	for (n=0;n!=max_chooser_sub_txt;n++) {
		sub_txt[n][0]=0x0;
	}

	cnt=argc-1;
	if (cnt<0) cnt=0;
	if (cnt>max_chooser_sub_txt) cnt=max_chooser_sub_txt;

	for (n=0;n!=cnt;n++) {
		script_value_to_string(argv[n+1],sub_txt[n],max_chooser_text_data_sz);
	}

		// start chooser

	chooser_trigger_set(name,(char*)sub_txt);
	
	return(TRUE);
}

JSBool js_interface_interaction_start_save_load_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	file_trigger_set();
	return(TRUE);
}

JSBool js_interface_interaction_start_setup_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	setup_game_trigger_set();
	return(TRUE);
}

JSBool js_interface_interaction_start_menu_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	menu_trigger_set();
	return(TRUE);
}

JSBool js_interface_interaction_quit_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	interface_quit_trigger_set();
	return(TRUE);
}
