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

JSBool js_interface_fade_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_interface_fade_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_interface_fade_circle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_interface_fade_clear_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_function	interface_fade_functions[]={
							{"circle",				js_interface_fade_circle_func,			6},
							{"clear",				js_interface_fade_clear_func,			0},
							{0}};

JSClass				*interface_fade_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_interface_fade_object(void)
{
	interface_fade_class=script_create_class("interface_fade_class",js_interface_fade_get_property,js_interface_fade_set_property);
}

void script_free_interface_fade_object(void)
{
	script_free_class(interface_fade_class);
}

void script_add_interface_fade_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,"fade",NULL,interface_fade_functions);
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_interface_fade_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,NULL));
}

JSBool js_interface_fade_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,NULL));
}

/* =======================================================

      Fade Functions
      
======================================================= */

JSBool js_interface_fade_circle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				x,y,life_msec;
	float			start_sz,end_sz;
	bool			auto_clear;
	obj_type		*obj;
	
	x=JSVAL_TO_INT(argv[0]);
	y=JSVAL_TO_INT(argv[1]);
    
    start_sz=script_value_to_float(argv[2]);
    end_sz=script_value_to_float(argv[3]);

	life_msec=JSVAL_TO_INT(argv[4]);
	
	auto_clear=script_value_to_bool(argv[5]);
	
	obj=object_find_uid(server.player_obj_uid);
	object_fade_start(js.time.current_tick,obj,x,y,start_sz,end_sz,life_msec,auto_clear);
	
	return(JS_TRUE);
}

JSBool js_interface_fade_clear_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(server.player_obj_uid);
	object_fade_clear(obj);
	
	return(JS_TRUE);
}

