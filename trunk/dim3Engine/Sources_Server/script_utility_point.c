/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: utility.point object

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"

extern js_type			js;

extern void view_script_transform_3D_to_2D(int x,int y,int z,int *x2,int *y2);

JSValueRef js_utility_point_equal_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_utility_point_angle_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_utility_point_distance_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_utility_point_transform_3D_to_2D_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	utility_point_functions[]={
							{"equal",				js_utility_point_equal_func,				kJSPropertyAttributeDontDelete},
							{"angleTo",				js_utility_point_angle_to_func,				kJSPropertyAttributeDontDelete},
							{"distanceTo",			js_utility_point_distance_to_func,			kJSPropertyAttributeDontDelete},
							{"transform3Dto2D",		js_utility_point_transform_3D_to_2D_func,	kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			utility_point_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_utility_point_object(void)
{
	utility_point_class=script_create_class("utility_point_class",NULL,utility_point_functions);
}

void script_free_utility_point_object(void)
{
	script_free_class(utility_point_class);
}

JSObjectRef script_add_utility_point_object(JSContextRef cx,JSObjectRef parent_obj,attach_type *attach)
{
	return(script_create_child_object(cx,parent_obj,utility_point_class,"point",attach));
}

/* =======================================================

      Point Functions
      
======================================================= */

JSValueRef js_utility_point_equal_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				x,z,y,to_x,to_z,to_y,slop;

	if (!script_check_param_count(cx,func,argc,7,exception)) return(script_null_to_value(cx));

	x=script_value_to_int(cx,argv[0]);
	z=script_value_to_int(cx,argv[1]);
	y=script_value_to_int(cx,argv[2]);
	to_x=script_value_to_int(cx,argv[3]);
	to_z=script_value_to_int(cx,argv[4]);
	to_y=script_value_to_int(cx,argv[5]);
	slop=script_value_to_int(cx,argv[6]);
	
	return(script_bool_to_value(cx,!((x<(to_x-slop)) || (x>(to_x+slop)) || (z<(to_z-slop)) || (z>(to_z+slop)) || (y<(to_y-slop)) || (y>(to_y+slop)))));
}

JSValueRef js_utility_point_angle_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				x,z,to_x,to_z;
	float			ang;
	
	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));

	x=script_value_to_int(cx,argv[0]);
	z=script_value_to_int(cx,argv[1]);
	to_x=script_value_to_int(cx,argv[2]);
	to_z=script_value_to_int(cx,argv[3]);
	
	ang=angle_find(x,z,to_x,to_z);
    return(script_float_to_value(cx,ang));
}

JSValueRef js_utility_point_distance_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				dist,x,z,y,to_x,to_z,to_y;
	
	if (!script_check_param_count(cx,func,argc,6,exception)) return(script_null_to_value(cx));

	x=script_value_to_int(cx,argv[0]);
	z=script_value_to_int(cx,argv[1]);
	y=script_value_to_int(cx,argv[2]);
	to_x=script_value_to_int(cx,argv[3]);
	to_z=script_value_to_int(cx,argv[4]);
	to_y=script_value_to_int(cx,argv[5]);

	dist=distance_get(x,y,z,to_x,to_y,to_z);
	return(script_int_to_value(cx,dist));
}

JSValueRef js_utility_point_transform_3D_to_2D_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				x,z,y,x2,y2;
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));

	x=script_value_to_int(cx,argv[0]);
	z=script_value_to_int(cx,argv[1]);
	y=script_value_to_int(cx,argv[2]);
	
	view_script_transform_3D_to_2D(x,y,z,&x2,&y2);

	return(script_point_to_value(cx,x2,y2,z));
}



