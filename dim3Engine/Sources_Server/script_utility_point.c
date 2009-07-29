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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"

extern js_type			js;

extern void view_script_transform_3D_to_2D(int x,int y,int z,int *x2,int *y2);

JSBool js_utility_point_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_utility_point_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_utility_point_equal_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_utility_point_angle_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_utility_point_distance_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_utility_point_transform_3D_to_2D_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_function	utility_point_functions[]={
							{"equal",				js_utility_point_equal_func,				7},
							{"angleTo",				js_utility_point_angle_to_func,				4},
							{"distanceTo",			js_utility_point_distance_to_func,			6},
							{"transform3Dto2D",		js_utility_point_transform_3D_to_2D_func,	3},
							{0}};

JSClass				*utility_point_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_utility_point_object(void)
{
	utility_point_class=script_create_class("utility_point_class",js_utility_point_get_property,js_utility_point_set_property);
}

void script_free_utility_point_object(void)
{
	script_free_class(utility_point_class);
}

void script_add_utility_point_object(JSObject *parent_obj)
{
	script_create_child_object(parent_obj,utility_point_class,"point",NULL,utility_point_functions);
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_utility_point_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,NULL));
}

JSBool js_utility_point_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,NULL));
}

/* =======================================================

      Point Functions
      
======================================================= */

JSBool js_utility_point_equal_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				x,z,y,to_x,to_z,to_y,slop;

	x=script_value_to_int(argv[0]);
	z=script_value_to_int(argv[1]);
	y=script_value_to_int(argv[2]);
	to_x=script_value_to_int(argv[3]);
	to_z=script_value_to_int(argv[4]);
	to_y=script_value_to_int(argv[5]);
	slop=script_value_to_int(argv[6]);
	
	*rval=script_bool_to_value(!((x<(to_x-slop)) || (x>(to_x+slop)) || (z<(to_z-slop)) || (z>(to_z+slop)) || (y<(to_y-slop)) || (y>(to_y+slop))));
	
	return(JS_TRUE);
}

JSBool js_utility_point_angle_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				x,z,to_x,to_z;
	float			ang;
	
	x=script_value_to_int(argv[0]);
	z=script_value_to_int(argv[1]);
	to_x=script_value_to_int(argv[2]);
	to_z=script_value_to_int(argv[3]);
	
	ang=angle_find(x,z,to_x,to_z);
    *rval=script_float_to_value(ang);
	
	return(JS_TRUE);
}

JSBool js_utility_point_distance_to_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				dist,x,z,y,to_x,to_z,to_y;
	
	x=script_value_to_int(argv[0]);
	z=script_value_to_int(argv[1]);
	y=script_value_to_int(argv[2]);
	to_x=script_value_to_int(argv[3]);
	to_z=script_value_to_int(argv[4]);
	to_y=script_value_to_int(argv[5]);

	dist=distance_get(x,y,z,to_x,to_y,to_z);
	*rval=INT_TO_JSVAL(dist);
	
	return(JS_TRUE);
}

JSBool js_utility_point_transform_3D_to_2D_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				x,z,y,x2,y2;
	
	x=script_value_to_int(argv[0]);
	z=script_value_to_int(argv[1]);
	y=script_value_to_int(argv[2]);
	
	view_script_transform_3D_to_2D(x,y,z,&x2,&y2);

	*rval=script_point_to_value(x2,y2,z);
	
	return(JS_TRUE);
}



