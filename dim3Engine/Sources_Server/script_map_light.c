/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.light object

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

JSBool js_map_light_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_light_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_map_light_toggle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_function	map_light_functions[]={
							{"toggle",				js_map_light_toggle_func,			2},
							{0}};

JSClassRef			map_light_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_light_object(void)
{
	map_light_class=script_create_class("map_light_class",NULL,NULL);
}

void script_free_map_light_object(void)
{
	script_free_class(map_light_class);
}

JSObject* script_add_map_light_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,map_light_class,"light",NULL,map_light_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_map_light_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,NULL));
}

JSBool js_map_light_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,NULL));
}

/* =======================================================

      Functions
      
======================================================= */

JSBool js_map_light_toggle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	map_light_type		*map_light;

	map_light=script_find_light_from_name(argv[0]);
	if (map_light==NULL) return(JS_FALSE);
	
	map_light->on=script_value_to_bool(argv[1]);
	
	return(JS_TRUE);
}
