/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.group object

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

extern map_type			map;
extern js_type			js;

extern void group_show(int group_idx,bool show);
extern void group_solid(int group_idx,bool solid);
extern void group_texture(int group_idx,int index);
extern void group_texture_shift(int group_idx,float x_shift,float y_shift);
extern void group_texture_alpha(int group_idx,float alpha);

JSBool js_map_group_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_map_group_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_map_group_get_center_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_map_group_set_show_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_map_group_set_solid_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_map_group_set_texture_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_map_group_set_texture_shift_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_map_group_set_texture_alpha_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);

script_js_function	map_group_functions[]={
							{"getCenter",			js_map_group_get_center_func,			1},
							{"setShow",				js_map_group_set_show_func,				2},
							{"setSolid",			js_map_group_set_solid_func,			2},
							{"setTexture",			js_map_group_set_texture_func,			2},
							{"setTextureShift",		js_map_group_set_texture_shift_func,	3},
							{"setTextureAlpha",		js_map_group_set_texture_alpha_func,	2},
							{0}};

JSClassRef			map_group_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_group_object(void)
{
	map_group_class=script_create_class("map_group_class",js_map_group_get_property,js_map_group_set_property);
}

void script_free_map_group_object(void)
{
	script_free_class(map_group_class);
}

JSObject* script_add_map_group_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,map_group_class,"group",NULL,map_group_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_map_group_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,NULL));
}

JSBool js_map_group_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,NULL));
}

/* =======================================================

      Information Functions
      
======================================================= */

JSBool js_map_group_get_center_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				group_idx;
	group_type		*group;
	
	group_idx=script_find_group_from_name(argv[0]);
	if (group_idx==-1) return(FALSE);

	if ((group_idx>=0) && (group_idx<map.ngroup)) {
		group=&map.groups[group_idx];
		*rval=script_point_to_value(group->center_pnt.x,group->center_pnt.y,group->center_pnt.z);
	}
	else {
		*rval=script_point_to_value(0,0,0);
	}

	return(TRUE);
}

/* =======================================================

      Show and Solid Function
      
======================================================= */

JSBool js_map_group_set_show_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_from_name(argv[0]);
	if (group_idx==-1) return(FALSE);
	
	group_show(group_idx,script_value_to_bool(argv[1]));
	return(TRUE);
}

JSBool js_map_group_set_solid_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_from_name(argv[0]);
	if (group_idx==-1) return(FALSE);

	group_solid(group_idx,script_value_to_bool(argv[1]));
	return(TRUE);
}

/* =======================================================

      Texture Function
      
======================================================= */

JSBool js_map_group_set_texture_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_from_name(argv[0]);
	if (group_idx==-1) return(FALSE);

	group_texture(group_idx,script_value_to_int(argv[1]));
	return(TRUE);
}

JSBool js_map_group_set_texture_shift_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_from_name(argv[0]);
	if (group_idx==-1) return(FALSE);

	group_texture_shift(group_idx,script_value_to_float(argv[1]),script_value_to_float(argv[2]));
	return(TRUE);
}

JSBool js_map_group_set_texture_alpha_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				group_idx;
	
	group_idx=script_find_group_from_name(argv[0]);
	if (group_idx==-1) return(FALSE);

	group_texture_alpha(group_idx,script_value_to_float(argv[1]));
	return(TRUE);
}

