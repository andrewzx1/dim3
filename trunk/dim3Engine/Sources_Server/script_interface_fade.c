/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: interface.fade object

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
#include "objects.h"
#include "interfaces.h"

extern server_type		server;
extern js_type			js;

extern void object_fade_start(int tick,obj_type *obj,int x,int y,float start_sz,float end_sz,int life_msec,bool auto_clear);
extern void object_fade_clear(obj_type *obj);

JSValueRef js_interface_fade_circle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_interface_fade_clear_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	interface_fade_functions[]={
							{"circle",				js_interface_fade_circle_func,			kJSPropertyAttributeDontDelete},
							{"clear",				js_interface_fade_clear_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			interface_fade_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_interface_fade_object(void)
{
	interface_fade_class=script_create_class("interface_fade_class",NULL,interface_fade_functions);
}

void script_free_interface_fade_object(void)
{
	script_free_class(interface_fade_class);
}

JSObjectRef script_add_interface_fade_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,interface_fade_class,"fade"));
}

/* =======================================================

      Fade Functions
      
======================================================= */

JSValueRef js_interface_fade_circle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				x,y,life_msec;
	float			start_sz,end_sz;
	bool			auto_clear;
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,6,exception)) return(script_null_to_value(cx));
	
	x=script_value_to_int(cx,argv[0]);
	y=script_value_to_int(cx,argv[1]);
    
    start_sz=script_value_to_float(cx,argv[2]);
    end_sz=script_value_to_float(cx,argv[3]);

	life_msec=script_value_to_int(cx,argv[4]);
	
	auto_clear=script_value_to_bool(cx,argv[5]);
	
	obj=object_find_uid(server.player_obj_uid);
	object_fade_start(js.time.current_tick,obj,x,y,start_sz,end_sz,life_msec,auto_clear);
	
	return(script_null_to_value(cx));
}

JSValueRef js_interface_fade_clear_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	obj=object_find_uid(server.player_obj_uid);
	object_fade_clear(obj);
	
	return(script_null_to_value(cx));
}

