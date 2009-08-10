/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: camera.staticPosition object

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
#include "cameras.h"

extern map_type			map;
extern camera_type		camera;
extern js_type			js;

JSValueRef js_camera_static_position_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_camera_static_position_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_camera_static_position_get_follow(void);
JSValueRef js_camera_static_position_get_walkTurnSpeed(void);
bool js_camera_static_position_set_follow(JSValueRef vp);
bool js_camera_static_position_set_walkTurnSpeed(JSValueRef vp);
JSValueRef js_camera_static_position_move_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_camera_static_position_move_to_spot_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_camera_static_position_walk_to_node_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_property	camera_static_position_props[]={
							{"follow",				js_camera_static_position_get_follow,			js_camera_static_position_set_follow},
							{"walkTurnSpeed",		js_camera_static_position_get_walkTurnSpeed,	js_camera_static_position_set_walkTurnSpeed},
							{0}};
							
script_js_function	camera_static_position_functions[]={
							{"move",				js_camera_static_position_move_func,			3},
							{"moveToSpot",			js_camera_static_position_move_to_spot_func,	1},
							{"walkToNode",			js_camera_static_position_walk_to_node_func,	6},
							{0}};

JSClassRef			camera_static_position_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_camera_static_position_object(void)
{
	camera_static_position_class=script_create_class("camera_static_position_class",js_camera_static_position_get_property,js_camera_static_position_set_property);
}

void script_free_camera_static_position_object(void)
{
	script_free_class(camera_static_position_class);
}

JSObjectRef script_add_camera_static_position_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,camera_static_position_class,"staticPosition",camera_static_position_props,camera_static_position_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_camera_static_position_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,camera_static_position_props));
}

bool js_camera_static_position_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,camera_static_position_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_camera_static_position_get_follow(void)
{
	*vp=script_bool_to_value(camera.static_follow);
	return(TRUE);
}

JSValueRef js_camera_static_position_get_walkTurnSpeed(void)
{
	*vp=script_float_to_value(camera.auto_walk.turn_speed);
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_camera_static_position_set_follow(JSValueRef vp)
{
	camera.static_follow=script_value_to_bool(*vp);
	return(TRUE);
}

bool js_camera_static_position_set_walkTurnSpeed(JSValueRef vp)
{
	camera.auto_walk.turn_speed=script_value_to_float(*vp);
	return(TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_camera_static_position_move_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	camera_static_update(script_value_to_int(argv[0]),script_value_to_int(argv[1]),script_value_to_int(argv[2]));
	
	return(TRUE);
}

JSValueRef js_camera_static_position_move_to_spot_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int			idx;
	spot_type	*spot;
	
	idx=script_value_to_int(argv[0]);
	if ((idx<0) || (idx>=map.nspot)) {
		JS_ReportError(js.cx,"Unknown spot");
		return(FALSE);
	}
	
	spot=&map.spots[idx];
	camera_static_update(spot->pnt.x,spot->pnt.z,spot->pnt.y);
	
	return(TRUE);
}

JSValueRef js_camera_static_position_walk_to_node_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				msec,event_id;
	char			start_name[name_str_len],end_name[name_str_len];
	bool			open_doors,in_freeze;

	script_value_to_string(argv[0],start_name,name_str_len);
	script_value_to_string(argv[1],end_name,name_str_len);

	msec=script_value_to_int(argv[2]);
	event_id=script_value_to_int(argv[3]);
	open_doors=script_value_to_bool(argv[4]);
	in_freeze=script_value_to_bool(argv[5]);
		
	if (!camera_walk_to_node_setup(start_name,end_name,msec,event_id,open_doors,in_freeze)) return(FALSE);

	return(TRUE);
}
